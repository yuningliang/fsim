/* /////////////////////////////// "perfTraceFile.h" /////////////////////////////////
 *  File: perfTraceFile.h
 *  Descriptions:  Provide trace file operations.
 *    .Trace file open/close.
 *    .Trace format detect.
 *    .Buffering.
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/08
 *  Author: Lingyun Tuo
 */

#ifndef PERFTRACEFILE_H_
#define PERFTRACEFILE_H_

#include "defs.h"
#include "sl1defs.h"

class InstrItem;

//#include <list>
#include <deque>
//#include <vector>

typedef std::deque<InstrItem*> TraceBuf;
typedef std::deque<InstrItem*>::iterator TraceBufIter;

class PerfTraceFile {
private:
	InstrItem *_instrItemCache;	// cyclic buffer
	unsigned _nextICPos;	// point to next free position of cyclic buffer
	
	TraceBuf *_traceBuf;
	char _traceFile[256];
	FILE *_traceFP;
	unsigned _totalCnt;
	unsigned _nopCnt;
	unsigned _skipCnt;
	unsigned _endCnt;
	bool _isTraceEnded;
	int _threadId;
	
	unsigned _traceFileLen;
	unsigned _maxLinesPerPass;
	
	inline void skip(int skipCnt) { _skipCnt = skipCnt; }
	inline unsigned skip(void) { return _skipCnt; }
	
	int jumpToAddr(ADDR addr);
	int readTrace(void);
	bool includeNOP(void);

public:
	PerfTraceFile(int thread_id);
	~PerfTraceFile();
	int initTraceFile(const char *name);
	InstrItem *getNextTraceItem(void);
	char *getTraceFile(void);
	bool empty();
	InstrItem *getFreeInstrItemInIC(void);
};

#endif /*PERFTRACEFILE_H_*/
