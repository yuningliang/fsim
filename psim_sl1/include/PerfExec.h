/* /////////////////////////////// "PerfExec.h" /////////////////////////////////
 *  File: PerfExec.h
 *  Descriptions:  Simulate execution
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
#ifndef PERFEXEC_H_
#define PERFEXEC_H_

class PerfTraceFile;

class PerfDispatch;
class PerfFetch;

class PerfExec {
private:
	PerfTraceFile *_traceList[THREAD_ID_MAX];
	
public:
#ifndef MULTI_THREAD
	PerfExec(PerfTraceFile *coreTrace, PerfTraceFile *bbTrace);
#else
	PerfExec(PerfTraceFile *tracefile0, PerfTraceFile *tracefile1, PerfTraceFile *tracefile2, PerfTraceFile *tracefile3);
#endif
	
	
	void dumpConfig(PerfDispatch *disp, PerfFetch *fetch);
	int run(void);	
};

#endif /*PERFEXEC_H_*/
