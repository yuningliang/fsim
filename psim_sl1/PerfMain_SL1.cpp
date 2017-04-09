/* /////////////////////////////// "psim_sl1_main.cpp" /////////////////////////////////
 *  File: psim_sl1_main.cpp
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/08
 *  Author: Lingyun Tuo
 */

#include "defs.h"
#include "messg.h"
#include "SL_getopt.h"
#include "PerfGlobalOpt.h"
#include "PerfTraceFile.h"
#include "PerfExec.h"

using namespace PSIM_SL1;

int main(int argc, char *argv[]) {
	Psim_ParseArguments(argc, argv);
	
	//Create objects for decoding and disassembling SL1 instructions
#ifndef MULTI_THREAD	
	PerfTraceFile *coreTrace, *bbTrace;
	coreTrace = new PerfTraceFile(THREAD_ID_CORE);
	bbTrace = new PerfTraceFile(THREAD_ID_BB);
	if (g_appTraceFile[0])
		coreTrace->initTraceFile(g_appTraceFile);
	if (g_bbTraceFile[0])
		bbTrace->initTraceFile(g_bbTraceFile);
	//Let's start rolling with the input trace	
	//We'll have an execution summary, right?
	PerfExec exec(coreTrace, bbTrace);	
#else
	PerfTraceFile *tracefile0,*tracefile1,*tracefile2,*tracefile3;
	tracefile0 = new PerfTraceFile(THREAD_ID_0);
	tracefile1 = new PerfTraceFile(THREAD_ID_1);
	tracefile2 = new PerfTraceFile(THREAD_ID_2); 
	tracefile3 = new PerfTraceFile(THREAD_ID_3);
	if (g_trd0TraceFile[0])
	    tracefile0->initTraceFile(g_trd0TraceFile);
	if (g_trd1TraceFile[0])
	    tracefile1->initTraceFile(g_trd1TraceFile);
	if (g_trd2TraceFile[0])
	    tracefile2->initTraceFile(g_trd2TraceFile);
	if (g_trd3TraceFile[0])
	    tracefile3->initTraceFile(g_trd3TraceFile);
	PerfExec exec(tracefile0,tracefile1,tracefile2,tracefile3);    
#endif
	exec.run();
#ifndef MULTI_THREAD	
	delete coreTrace;
	delete bbTrace;
#else	
	delete tracefile0;
	delete tracefile1;
	delete tracefile2;
	delete tracefile3;
#endif	
	return 0;
}
