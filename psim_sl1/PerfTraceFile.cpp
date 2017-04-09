/* /////////////////////////////// "perfTraceFile.cpp" /////////////////////////////////
 *  File: perfTraceFile.h
 *  Descriptions:  Provide trace file operations.
 *    .Trace file open/close.
 *    .Trace format detect.
 *    .Buffering.
 * 
 * 	Copyright (c) 2007 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/08
 *  Author: Lingyun Tuo
 */

#include "defs.h"
#include "sl1defs.h"
#include "PerfDefs.h"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "PerfInstrItem.h"
#include "PerfTraceFile.h"
#include "PerfExec.h"
#include "PerfGlobalOpt.h"
#include "PerfModel_SL1.h"

using namespace PSIM_SL1;

//#define _READ_MAP_BUF 0

PerfTraceFile::PerfTraceFile(int thread_id)
{
	IsTrue( (thread_id>=0 || thread_id<THREAD_ID_MAX), ("Invalid thread ID:%d\n", thread_id));
	_threadId = thread_id;
	_traceBuf = NULL;
	_traceFP = NULL;
	_nopCnt = 0;
	_totalCnt = 0;
	_isTraceEnded = true;
	_nextICPos = 0;
	_maxLinesPerPass = 0;
	if (g_endLimit[thread_id] == 0)
		_endCnt = UINT_MAX;
	else
		_endCnt = g_endLimit[thread_id];
		
	_traceBuf = new TraceBuf;
	_traceFile[0] = 0;
//	printf("Created trace object for thread %d\n", _threadId);
}

PerfTraceFile::~PerfTraceFile()
{
//	printf("Delete trace object for thread %d\n", _threadId);
	if (_traceBuf)
		delete _traceBuf;
	if (_traceFP)
		fclose(_traceFP);
	delete []_instrItemCache;
}

char *PerfTraceFile::getTraceFile()
{
	return _traceFile;
}

/*
 * 
 * 
 */
int PerfTraceFile::initTraceFile(const char *name)
{
//	vmsg(VERBOSE_1, "thread %d: Entering PerfTraceFile::initTraceFile()\n", _threadId);
	
	IsTrue( (_traceFile[0] == 0), ("Trace file already initialized for thread:%d\n", _threadId));
	
	strcpy(_traceFile, name);
	
	// Open the trace file. Trace file maybe very big.
	if ((_traceFP = fopen64(_traceFile, "rb")) == NULL) {
		fprintf(stderr, "Can't open trace file %s for thread id %d\n", _traceFile, _threadId);
		exit(-1);
	}

	// Get file length
	struct stat statBuf;
	_traceFileLen = 0;
	if (fstat(fileno(_traceFP), &statBuf) == 0) {
		_traceFileLen = statBuf.st_size;
		vmsg(VERBOSE_1, "Trace file %s size:%d\n", _traceFile, _traceFileLen);
	}
	
	_maxLinesPerPass = MAX_PREREAD_BUF;
	_instrItemCache = new InstrItem[_maxLinesPerPass + EXTRA_PAGE_SIZE];

	_nextICPos = 0;

	_isTraceEnded = false;
	// skip # of instructions
	if (g_skipCnt[_threadId] > 0)
		skip(g_skipCnt[_threadId]);
	// jump to specified address and start simulation from there
	if (g_startAddr[_threadId] != (ADDR)-1)
		jumpToAddr(g_startAddr[_threadId]);
		
	// Read traces into tracebuf
	return readTrace();
}

inline InstrItem *PerfTraceFile::getFreeInstrItemInIC(void) {
	// instrItemCache is a wraparound buffer. It must have extra storages to hold those instructions in pipeline.
	InstrItem *p =  &_instrItemCache[_nextICPos];
	_nextICPos ++;
	if (_nextICPos >= _maxLinesPerPass + EXTRA_PAGE_SIZE) {
		_nextICPos = 0;//wrap around
	}
	return p;
} 

int PerfTraceFile::jumpToAddr(ADDR addr)
{
	char buf[256];
	int curCnt = 0;
	unsigned int pc,nFields;
	while(curCnt<_maxLinesPerPass) {
		if (!fgets(buf, 255, _traceFP)) {
			//IsTrue((0), ("no such addr in the tracefile\n"));
			break;
		}
		nFields = sscanf(buf,"%x",&pc);
		if(pc==addr)
		{
		  skip(curCnt);
		  break;
		}
		curCnt++;		
	}
	//IsTrue((0), ("PerfTraceFile::jumpToAddr not implemented\n"));
	return 0;
}

int PerfTraceFile::readTrace(void)
{
	WORD raw = 0;
	ADDR pc = 0;
	BRANCH_META meta = BRANCH_PREDICTION_RIGHT;
	ADDR dataAddr = (unsigned)-1;
	ADDR ar1,ar2;
	int threadId;
	
	char buf[256];
	int nFields;
	int curCnt = 0;
	int curLineLen;

	if ((!_traceFP) || ((_isTraceEnded == true) && _traceBuf->empty()))
		return -1;

//while(!_isTraceEnded) 
{
#ifdef _READ_MAP_BUF
	unsigned curPos = ftell(_traceFP);
	char *fileMap = (char *)mmap(NULL, _mmapLen, PROT_READ, MAP_SHARED, fileno(_traceFP), curPos);
	IsTrue((fileMap), ("Invalid map\n"));	
#endif

	curCnt = 0;
	// read trace for core thread	
	while(curCnt<_maxLinesPerPass) {
//		printf("read one more from %p\n", _traceFP);
		if (!fgets(buf, 255, _traceFP)) {
			_isTraceEnded = true;
			break;
		}
		if (_totalCnt < skip() ) {
			_totalCnt ++;
			if (_totalCnt >= skip()) {
				vmsg(VERBOSE_4, "%s: Skipped %d lines\n", getThreadName(_threadId), skip());
				_totalCnt = 0;
				skip(0);
			}
			continue;
		}
		curLineLen = strlen(buf);
		
//		printf("%d: read a line:[%s] len=%d\n", curCnt, buf, curLineLen);
		
//		printf("%d: read a line:[%s]\n", curCnt, buf);
		IsTrue((buf[curLineLen-1]=='\n'), ("Invalid newline at [%s] length %d\n", buf, curLineLen));
		nFields = sscanf(buf, "%x %x %x %x ar1=%x ar2=%x", &pc, &raw, &meta, &threadId, &ar1, &ar2);
		IsTrue((nFields>=2), ("Too few fields in trace:%s\n", buf));
		if (nFields < 3)
			meta = BRANCH_PREDICTION_RIGHT;
		if (nFields < 6)
		{
			ar1= (unsigned)-1;
			ar2= (unsigned)-1;
		    nFields = sscanf(buf, "%x %x %x %x offset=%x", &pc, &raw, &meta, &threadId, &dataAddr);
		    if (nFields < 5)   
				dataAddr = (unsigned)-1;
		}
		if((includeNOP()==true) || (raw!=0)) {
			InstrItem *item = getFreeInstrItemInIC();
//			printf("%d will overwrite perfId %d. IC Index:%x %x\n", _totalCnt+1, item->perfId(), item, _instrItemCache);
			item->init(_threadId, pc, raw, meta, _totalCnt+1, dataAddr,ar1,ar2);
//			InstrItem *item = new InstrItem(_threadId, pc, raw, meta, _totalCnt+1);
			
			if ((pc&0x3) == 0x2) {
				InstrItem *lastItem=NULL;
				if (!_traceBuf->empty()) 
					lastItem = _traceBuf->back();
				if (lastItem)
					lastItem->isWordEnd(false);
			}
			_traceBuf->push_back(item);
			
			curCnt ++;
			
//			item->dump();	
		}
		if(raw==0)
			_nopCnt++;
		_totalCnt++;
		if(_totalCnt>=_endCnt) {
			_isTraceEnded = true;
			break;
			//MAX count reached
		}
	}
}
//printf("total read:%u\n", _totalCnt);
//exit(0);
	return 0;
}

/*
 * It's the owner's responsibility to free the returned object.
 */
InstrItem *PerfTraceFile::getNextTraceItem(void)
{
//	printf("thread %d: Entering PerfTraceFile::getNextTraceItem()\n", _threadId);
	if (_traceBuf->empty()) {
		if (readTrace() < 0) {
			return NULL;
		}
	}
	if (_isTraceEnded && _traceBuf->empty()) {
		printf("No more instructions for thread %d\n", _threadId);
		return NULL;
	}
	InstrItem * pItem = _traceBuf->front();
	_traceBuf->pop_front();
	return pItem;
}

bool PerfTraceFile::includeNOP(void)
{
	return g_includeNOP;
}

bool PerfTraceFile::empty()
{
	IsTrue((_traceBuf), ("Trace buffer not initialized\n"));
	return _traceBuf->empty(); 
}
