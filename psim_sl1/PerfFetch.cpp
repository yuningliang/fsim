/* /////////////////////////////// "PerfFetch.cpp" /////////////////////////////////
 *  File: PerfFetch.cpp
 * 
 *  Descriptions:   Implementation for fetching function
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
 
#include "defs.h"
#include "sl1defs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfIWin.h"
#include "PerfTraceFile.h"
#include "PerfCache.h"
#include "PerfFetch.h"
#include "PerfModel_SL1.h"
#include "PerfGlobalOpt.h"

PerfFetch::PerfFetch()
{
	p_trace = NULL;
	p_iwin = NULL;
	
	_stallCyc = FETCH_NO_STALL;
	
	_totalFetched = 0;
	
	p_cache = new PerfCache();
	IsTrue((p_cache), ("Failed to create cache\n"));
}

PerfFetch::~PerfFetch()
{
	// The fetchBuf should be cleared in somewhere else.
	delete p_cache;
}

/*
 * Read trace to fetch buffer.
 * Return: # of instructions fetched to buffer
 */
int PerfFetch::fillBuf(int cyc)
{
	// fetch stall check
	if ((_stallCyc!=FETCH_NO_STALL) && (_stallCyc > cyc)) {
		if(p_iwin->IWinSize()!=0){
		   FetchStallInc();
		}
		if (PSIM_SL1::g_dumpFetchBuffer)
			printf("%4s: fetch stalled.\n", getThreadName(_threadId));
	}
	// check if buffer is full
	if (isBufferFull()) {
//		printf("thread %d: fetch buffer is full.\n", _threadId);
		return 0;
	}
	
	if (_stallCyc <= cyc)
		_stallCyc = FETCH_NO_STALL;
		
	int n = 0;
	InstrItem *item;
	
	// move words from trace to fetch buffer
	while (!isBufferFull()) {
		// read trace
		if ((item = p_trace->getNextTraceItem()) == NULL) {
//			printf("thread %d: trace is empty.\n", _threadId);
			break;
		}
		_fetchBuf.push_back(item);
		lastFetchAddr(item->pc());
		n++;
	}
	_totalFetched += n;
	
	if (n && PSIM_SL1::g_dumpFetchBuffer) { 
		printf("thread %d: %d instrs from trace to fetch buffer(%d)\n", _threadId, n, getFetchLmt());
//		dumpFE(stdout);
	}
	return n;
}

int PerfFetch::fillWin(int cyc)
{
	if (p_iwin->isIWinFull()){
		 if (PSIM_SL1::g_dumpFetchBuffer) printf("thread %d: iwin is full.\n", _threadId);
		return 0;
	}
	
	if (_fetchBuf.empty()) {
//		printf("thread %d: fetch buffer is empty.\n", _threadId);
		return 0;
	}
	
	int n = 0;
	InstrItem *item;
	
	// move instructions from fetch buffer to iwin 
	while (!p_iwin->isIWinFull()) {
		// read trace
		if (_fetchBuf.empty()) {
//			printf("thread %d: fetch buffer empty.\n", _threadId);
			break;
		}
		item = _fetchBuf.front();
		
		// fetch stall check
		if ((_stallCyc!=FETCH_NO_STALL) && (_stallCyc > cyc)) {
			if (PSIM_SL1::g_dumpFetchBuffer)
				printf("%4s: fetch stalled.\n", getThreadName(_threadId));
			return 0;
		}
		if (_stallCyc <= cyc)
			_stallCyc = FETCH_NO_STALL;
		
		
		// cache check
		UINT stalledCyc = p_cache->checkICache(item->pc());
		if (cyc>0 && stalledCyc>0) {
			setFetchStall(cyc, stalledCyc);
			if (PSIM_SL1::g_dumpFetchBuffer) 
				printf("thread %d: icache miss!fetch will stall %d cycles on instr %d\n", _threadId, stalledCyc, item->perfId());
			
			break;
		}
		// move this instrution to iwinlist for issuing
		_fetchBuf.pop_front();
		p_iwin->add(item, cyc);
		n++;
	}
	if (PSIM_SL1::g_dumpFetchBuffer)
		printf("thread %d: fetch %d instrs to iwin at cyc %d. %d instrs in FE\n", _threadId, n, cyc, _fetchBuf.size());
	return n;
}

void PerfFetch::dumpStats(FILE *out)
{
	fprintf(out, "%5s - Total fetched: %d, i$ miss: %d\n", 
		getThreadName(_threadId), _totalFetched, p_cache->icacheMissed());
}

void PerfFetch::dumpFE(FILE *out)
{
	ISimpleList::iterator iter;
	for (iter=_fetchBuf.begin(); iter != _fetchBuf.end(); iter++) {
		InstrItem *item = *iter;
		printf("  %4s FE %08d: %08x %08x\n",
			getThreadName(item->_threadId), 
			item->perfId(), 
			item->_pc,
			item->_raw);
	}
}
