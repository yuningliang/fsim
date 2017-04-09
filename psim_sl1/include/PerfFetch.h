/* /////////////////////////////// "PerfFetch.h" /////////////////////////////////
 *  File: PerfFetch.h
 *  Descriptions:  Simulate fetching from DRAM to cache to IWIN
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#ifndef PERFFETCH_H_
#define PERFFETCH_H_

class PerfCache;

class PerfFetch 
{
private:
	ISimpleList _fetchBuf;
	
	PerfTraceFile * p_trace;
	PerfIWin * p_iwin;
	PerfCache *p_cache;
	
	bool isBufferFull(void) { return (_fetchBuf.isFull()); }
	
	int _threadId;
	int _stallCyc;	// fetch stalls until stallCyc. -1 indicates no stall.
	ADDR _lastFetchAddr;
	int _totalFetched;
	
public:
	PerfFetch();
	~PerfFetch();
	
	inline int  getFetchLmt(void) { return _fetchBuf.getMaxWords(); }
	inline void setFetchLmt(int fetchLimit) { _fetchBuf.setMaxWords(fetchLimit); }
	void setTrace(PerfTraceFile *trace_) { p_trace = trace_; }
	void setIWin(PerfIWin *iwin_) {p_iwin = iwin_; }
	void setThreadId(int threadId_) { _threadId = threadId_; }
	void setFetchStall(int curCyc, int stallCycles) { _stallCyc = curCyc + stallCycles; }
	void FetchStallInc() {_stallCyc++;}
	ADDR lastFetchAddr(void) { return _lastFetchAddr; }
	void lastFetchAddr(ADDR addr_) { _lastFetchAddr = addr_; }
	PerfCache *cache(void) { return p_cache; }
	
	int fillBuf(int cyc);
	int fillWin(int cyc);
	
	bool empty() { return _fetchBuf.empty() && p_trace->empty(); }
	void dumpStats(FILE *out);
	void dumpFE(FILE *out);
};

#endif /*PERFFETCH_H_*/
