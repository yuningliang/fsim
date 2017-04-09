/* /////////////////////////////// "PerfGlobalOpt.h" /////////////////////////////////
 *  File: PerfGlobalOpt.h
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/08
 *  Author: Lingyun Tuo
 */

#ifndef _GLOBAL_DEFS_H_
#define _GLOBAL_DEFS_H_ 1

#include "defs.h"
#include "sl1defs.h"
#include "PerfDefs.h"

#define MAX_RESOURCE_STR 1024

namespace PSIM_SL1
{
extern int  g_verbose;
extern bool g_dumpIWin;
extern bool g_dumpPipe;
extern bool g_dumpFetchBuffer;
extern bool g_dumpResourceStatus;
extern bool g_dumpHazardDetails;
extern bool g_dumpBranchInfo;
extern bool g_dumpAll;
extern bool g_dumpConfig;
#ifndef MULTI_THREAD
extern char g_appTraceFile[256];
extern char g_bbTraceFile[256];
#else
extern char g_trd0TraceFile[256]; 
extern char g_trd1TraceFile[256];
extern char g_trd2TraceFile[256];
extern char g_trd3TraceFile[256];
#endif

extern int g_skipCnt[SL1_MAX_THREADS];
extern ADDR g_startAddr[SL1_MAX_THREADS];
extern bool g_includeNOP;
extern bool g_coreNoALULS;
extern bool g_bbNoALU;
#ifndef MULTI_THREAD
extern unsigned g_bbRscMask;
extern unsigned g_coreRscMask;
#else
extern unsigned g_thread0RscMask;
extern unsigned g_thread1RscMask;
extern unsigned g_thread2RscMask;
extern unsigned g_thread3RscMask;
#endif
extern unsigned g_endLimit[SL1_MAX_THREADS];	// max instructions processed for each thread
extern unsigned g_maxGprReadPorts[SL1_MAX_THREADS];
extern unsigned g_maxGprWritePorts[SL1_MAX_THREADS];
extern unsigned g_maxIssue[SL1_MAX_THREADS];	// issue limit per thread per cycle
extern unsigned g_totalMaxIssue;	// issue limit of all threads per cycle
extern unsigned g_maxIssuePerCall;	// issue limit for a single call
extern unsigned g_maxFetch[SL1_MAX_THREADS];
extern char     g_execResources[MAX_RESOURCE_STR];
extern char     g_issuePriority[256];
extern unsigned g_RAWBasicLatency;
//extern unsigned g_dcacheMissRatio;	// in percentile
extern unsigned g_dcacheMissPenalty;	// in cycles
extern unsigned g_icacheMissPenalty;	// in cycles
extern unsigned g_brPenalty;	// cycles
extern unsigned g_jrPenalty;	// cycles
extern unsigned g_linkRegisterLatency;	// cycles: jr latency for link register to be ready
extern unsigned g_macLatency;	// cycles: MAC instructions will take one more cycle
extern EXEC_UNIT_SET g_euSetType; 
extern unsigned g_cycleMask;	// if cycle & mask is true, BB has priority over execution resources
extern unsigned g_externalmemLatency;
extern unsigned g_externalmemLow;
extern unsigned g_externalmemHigh;
extern unsigned g_membankLatency;
extern unsigned g_fftLatency[FFTMAXMODE];
extern unsigned g_viterbiLatency;
extern unsigned g_tracebackLatency;
};

int  Psim_ParseArguments(int argc, char *argv[]);
int  Psim_DumpOptions(void);
void Psim_Usage(char *cmd);
int Psim_EnableDumpAll(void);

#endif /* _GLOBAL_DEFS_H_ */
/* ///////////////////////// End of file "PerfGlobalOpt.h" //////////////////////// */
