/* /////////////////////////////// "PerfDispatch.cpp" /////////////////////////////////
 *  File: PerfDispath.cpp
 * 
 *  Descriptions:   Implementation for simulation execution
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
 
#include <stdio.h>
#include "defs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfTraceFile.h"
#include "PerfIWin.h"
#include "PerfCache.h"
#include "PerfFetch.h"
#include "PerfDispatch.h"
#include "PerfExec.h"
#include "PerfModel_SL1.h"
#include "PerfGlobalOpt.h"
#include "ReportTime.h"
#include "PerfGlobalOpt.h"

int threadPriority[SL1_MAX_THREADS];//priority order: first thread id will be stored in 0th

/*
 * Initialize issuing prority from command line option
 */
int InitPriority(char *str)
{
	int filler[SL1_MAX_THREADS];	// for verification
	int i;
	for (i=0; i<SL1_MAX_THREADS; i++) {
		threadPriority[i] = i;
		filler[i] = -1;
	}
	
	printf("thread priority string:%s\n", str);
	
	if (str && str[0]) {
		char *p = strtok(str, ",");
		int cc = 0;
		while(p) {
#ifndef MULTI_THREAD
			if (strcasecmp(p, "core") == 0) {
				threadPriority[cc] = THREAD_ID_CORE;
			} else if (strcasecmp(p, "bb") == 0) {
				threadPriority[cc] = THREAD_ID_BB;
#else
			if (strcasecmp(p, "thread0") == 0) {
				threadPriority[cc] = THREAD_ID_0;
			} else if(strcasecmp(p, "thread1") == 0){
			    threadPriority[cc] = THREAD_ID_1;
			} else if(strcasecmp(p, "thread2") == 0){
			    threadPriority[cc] = THREAD_ID_2;
			} else if(strcasecmp(p, "thread3") == 0){
			    threadPriority[cc] = THREAD_ID_3;
#endif    
			} else {
				IsTrue((0), ("Invalid thread name '%s' in option 'thread-priority'\n", p));
			}
			filler[threadPriority[cc]] = 0;
			cc ++;
			p = strtok(NULL, ",");
		}
		for (i=0;  i<SL1_MAX_THREADS; i++) {
			if (filler[threadPriority[i]] != 0) {
				fprintf(stderr, "Not all threads are specified in option 'thread-priority'\n");
				exit(-1);
			}
		}
	}
	return 0;
}
#ifndef MULTI_THREAD
PerfExec::PerfExec(PerfTraceFile *coreTrace, PerfTraceFile *bbTrace)
#else
PerfExec::PerfExec(PerfTraceFile *tracefile0, PerfTraceFile *tracefile1, PerfTraceFile *tracefile2, PerfTraceFile *tracefile3)
#endif
{
#ifndef MULTI_THREAD
	IsTrue((0==THREAD_ID_CORE), ("You must remap core thread id by yourself\n"));
	IsTrue((1==THREAD_ID_BB), ("You must remap bb thread id by yourself\n"));
#else
	IsTrue((0==THREAD_ID_0), ("You must remap thread 0 id by yourself\n"));
	IsTrue((1==THREAD_ID_1), ("You must remap thread 1 id by yourself\n"));
	IsTrue((2==THREAD_ID_2), ("You must remap thread 2 id by yourself\n"));
	IsTrue((3==THREAD_ID_3), ("You must remap thread 3 id by yourself\n"));
#endif
	memset(_traceList, 0, sizeof(_traceList));
#ifndef MULTI_THREAD
	_traceList[0] = coreTrace;
	_traceList[1] = bbTrace;
#else
	_traceList[0] = tracefile0;
	_traceList[1] = tracefile1;
	_traceList[2] = tracefile2;
	_traceList[3] = tracefile3;
#endif	
}

void PerfExec::dumpConfig(PerfDispatch *disp, PerfFetch *fetch)
{
	int i;
	
	if (!PSIM_SL1::g_dumpConfig) return;
	
	// Dump configurations
	FILE *out = stdout;
	fprintf(out, "********Start dump of parameters ********\n");
	Psim_DumpOptions();
	fprintf(out, "********Start dump of SL1 performance model configurations********\n");
	// threads with trace file
	for (i=0; i<SL1_MAX_THREADS; i++) {
		char *trFile = NULL;
		if (_traceList[i])
			trFile = _traceList[i]->getTraceFile();
		if(!trFile || !trFile[0]) {
			fprintf(out, "thread %d: %s without trace\n",
				i, getThreadName(i));
		} else {
			fprintf(out, "thread %d: %s - trace file:%s\n",
				i, getThreadName(i), trFile);
		}
	}
	// registers, execution resources, instruction-to-resource map
	PerfModelSL1::getInstance().dumpConfig(out);
	// maximum instructions issued per thread
	fprintf(out, "Max instructions to be issued per cycle for each thread:\n");
	for (i=0; i<SL1_MAX_THREADS; i++ ) {
		fprintf(out, "\t%4s: max issue %d\n",getThreadName(i), disp[i].getMaxIssue());
	}
	fprintf(out, "Max instructions to be issued at a time:%d\n", PSIM_SL1::g_maxIssuePerCall);
	fprintf(out, "Max words to be fetched per cycle for each thread:\n");
	// maximum instructions fetched per thread
	for (i=0; i<SL1_MAX_THREADS; i++ ) {
		fprintf(out, "\t%4s: max fetch %d\n",getThreadName(i), fetch[i].getFetchLmt());
	}
	fprintf(out, "Issue priority:\n");
	// issue priority
	for (i=0;  i<SL1_MAX_THREADS; i++) {
		fprintf(out, "\tissuing order %d:%4s\n", i, getThreadName(threadPriority[i]));
	}
	fprintf(out, "Hazard check: exec units, register ports, register RAW/WAW\n");
	fprintf(out, "Writeback latency: %d cycles without memory access/%d cycles with memory access\n", 
		PSIM_SL1::g_RAWBasicLatency, PSIM_SL1::g_RAWBasicLatency+1);
	IsTrue((PSIM_SL1::g_RAWBasicLatency<DEFAULT_INSTR_STAGES), ("Invalid write back latency. Value %d is out of default pipeline stages %d\n",
		PSIM_SL1::g_RAWBasicLatency, DEFAULT_INSTR_STAGES));
	fprintf(out, "Cycle mask: %#08x\n", PSIM_SL1::g_cycleMask);
	fprintf(out, "********  End dump of SL1 performance model configurations********\n\n\n");
}

#define MAX_ISSUE_PER_CYC 10

int PerfExec::run(void) 
{
	InstrItem *bbItem;
	InstrItem *coreItem;
	int nRunning;
	int cycle = 0;
	int issueCnt[MAX_ISSUE_PER_CYC];
	unsigned threadMask = SL1_MAX_THREADS - 1;
	int nMaxPerCall = DEFAULT_MAX_ISSUE_PER_CALL;
	if (nMaxPerCall > PSIM_SL1::g_maxIssuePerCall)
		nMaxPerCall = PSIM_SL1::g_maxIssuePerCall;
    nMaxPerCall = 1;
	IsTrue(((SL1_MAX_THREADS & (SL1_MAX_THREADS-1)) == 0),("# of threads is not power of 2:%d\n", SL1_MAX_THREADS));
	
	PerfDispatch *disp = new PerfDispatch[SL1_MAX_THREADS];
	PerfFetch *fetch = new PerfFetch[SL1_MAX_THREADS];
	
	int i;
	
	memset(issueCnt, 0, sizeof(issueCnt));
	
	for (i=0; i<SL1_MAX_THREADS; i++) {
		fetch[i].setTrace(_traceList[i]);
		fetch[i].setIWin(disp[i].getIWin());
		fetch[i].setThreadId(i);
		
		disp[i].setThreadId(i);
	}
	
	// set issue limit per cycle
#ifndef MULTI_THREAD
	if (PSIM_SL1::g_maxIssue[THREAD_ID_CORE] != 0)
		disp[THREAD_ID_CORE].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_CORE]);
	if (PSIM_SL1::g_maxIssue[THREAD_ID_BB] != 0)
		disp[THREAD_ID_BB].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_BB]);
#else		
	if (PSIM_SL1::g_maxIssue[THREAD_ID_0] != 0)
		disp[THREAD_ID_0].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_0]);
	if (PSIM_SL1::g_maxIssue[THREAD_ID_1] != 0)
		disp[THREAD_ID_1].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_1]);
	if (PSIM_SL1::g_maxIssue[THREAD_ID_2] != 0)
		disp[THREAD_ID_2].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_2]);
    if (PSIM_SL1::g_maxIssue[THREAD_ID_3] != 0)
		disp[THREAD_ID_3].setMaxIssue(PSIM_SL1::g_maxIssue[THREAD_ID_3]);
#endif		
	// set fetch limit per cycle
#ifndef MULTI_THREAD
	if (PSIM_SL1::g_maxFetch[THREAD_ID_CORE] != 0) {
		fetch[THREAD_ID_CORE].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_CORE]);
		disp[THREAD_ID_CORE].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_CORE]);
	}
	if (PSIM_SL1::g_maxFetch[THREAD_ID_BB] != 0) {
		fetch[THREAD_ID_BB].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_BB]);
		disp[THREAD_ID_BB].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_BB]);
	}
#else	
	if (PSIM_SL1::g_maxFetch[THREAD_ID_0] != 0) {
		fetch[THREAD_ID_0].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_0]);
		disp[THREAD_ID_1].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_0]);
	}
	if (PSIM_SL1::g_maxFetch[THREAD_ID_1] != 0) {
		fetch[THREAD_ID_1].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_1]);
		disp[THREAD_ID_1].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_1]);
	}
	if (PSIM_SL1::g_maxFetch[THREAD_ID_2] != 0) {
		fetch[THREAD_ID_2].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_2]);
		disp[THREAD_ID_2].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_2]);
	}
	if (PSIM_SL1::g_maxFetch[THREAD_ID_3] != 0) {
		fetch[THREAD_ID_3].setFetchLmt(PSIM_SL1::g_maxFetch[THREAD_ID_3]);
		disp[THREAD_ID_3].getIWin()->setIWinSizeLmt(PSIM_SL1::g_maxFetch[THREAD_ID_3]);
	}
#endif	
	
	// cache simulation
#ifndef MULTI_THREAD     
	if (PSIM_SL1::g_icacheMissPenalty) {
		for(i=0;i<SL1_MAX_THREADS;i++){
			fetch[i].cache()->incICachePenalty(true);
			fetch[i].cache()->icacheMissCyc(PSIM_SL1::g_icacheMissPenalty);
		}
	}
	if(PSIM_SL1::g_dcacheMissPenalty)
	{
		disp[THREAD_ID_CORE].dcache()->incDCachePenalty(true);
		disp[THREAD_ID_CORE].dcache()->dcacheMissCyc(PSIM_SL1::g_dcacheMissPenalty);
	}
#else	
	if (PSIM_SL1::g_icacheMissPenalty) {
		for(i=0;i<SL1_MAX_THREADS;i++){
			fetch[i].cache()->incICachePenalty(true);
			fetch[i].cache()->icacheMissCyc(PSIM_SL1::g_icacheMissPenalty);
		}
	}
#endif

	InitPriority(PSIM_SL1::g_issuePriority);
	
	dumpConfig(disp, fetch);
	
	CReportTime reportTime;
	reportTime.Start();
	// The simulation ends when all fetching buffers and IWin buffers are empty
	int idleCycle=0;
	int oneEnded = 0;
	
	for (i=0; i<SL1_MAX_THREADS; i++) {
		if (fetch[i].empty())
			oneEnded ++;
	}
	while(1) {
		vmsg(VERBOSE_2, "\ncyc %d--\n", cycle);
		nRunning = 0;

		// Update before issuing
#if 0
		for (i=0; i<SL1_MAX_THREADS; i++) {
			disp[threadPriority[i]].update();	// update order matters
		}
#else
			PerfDispatch::updateParallel(disp, SL1_MAX_THREADS);
#endif
		// dispatch: issue from the IWin buffer
		// One dispatch object for one thread
		// The order in the dispatch queue reflects the issuing order
		// Issue strategy may be implemented here or in a strategy class
		PerfModelSL1::getInstance().update();
		
		int curIssued;
		ISSUE_STATUS issueStatus;
		int nMaxPerCyc;
		int nInactiveThreads;	// # of threads that has no more instructions to issue
		curIssued=0;
		
//		if ((cycle & PSIM_SL1::g_cycleMask) != 0) {	
        	nMaxPerCyc=PSIM_SL1::g_totalMaxIssue;
        	nMaxPerCall = 3;
        	curIssued=0;
			while (1) {
//				printf("while\n");
				nInactiveThreads = 0;								
#ifndef MULTI_THREAD				
				for (i=0; i<SL1_MAX_THREADS; i++) {
					issueStatus = disp[threadPriority[i]].issue(cycle, nMaxPerCall);
					curIssued += disp[threadPriority[i]].getPercallIssued();
					nMaxPerCyc -= disp[threadPriority[i]].getPercallIssued();
#else					
				for(i=cycle&THREAD_EXEC_MASK;i<(SL1_MAX_THREADS+(cycle&THREAD_EXEC_MASK));i++){
					issueStatus = disp[threadPriority[i&THREAD_EXEC_MASK]].issue(cycle, nMaxPerCall);
					curIssued += disp[threadPriority[i&THREAD_EXEC_MASK]].getPercallIssued();
					nMaxPerCyc -= disp[threadPriority[i&THREAD_EXEC_MASK]].getPercallIssued();					
#endif
					if (issueStatus == ISSUE_STOPPED) {
						nInactiveThreads ++;
						if (nInactiveThreads >= SL1_MAX_THREADS)
							break;	// next thread
					}
					if (!nMaxPerCyc) break;
				}
				if (!nMaxPerCyc) break;	// total # reaches limit
				if (nInactiveThreads >= SL1_MAX_THREADS) break; // all threads are inactive
			}
//		} else {
//			for (i=SL1_MAX_THREADS-1; i>=0; i--) {
//				while (1) {
//					issueStatus = disp[threadPriority[i]].issue(cycle, nMaxPerCyc);
//					curIssued += disp[threadPriority[i]].getCurIssued();
//					nMaxPerCyc -= disp[threadPriority[i]].getCurIssued();
//					if (!nMaxPerCyc) break;
//					if (issueStatus == ISSUE_NORMAL) break;	// next thread
//				}
//				if (!nMaxPerCyc) break;
//			}
//		}
		//if (curIssued>PSIM_SL1::g_totalMaxIssue)
			//printf("cyc %d:issued %d\n", cycle, curIssued);		
		issueCnt[curIssued]++;
		if (!curIssued) {
			idleCycle ++;
			if (idleCycle>(PSIM_SL1::g_fftLatency[6]+PSIM_SL1::g_viterbiLatency+PSIM_SL1::g_tracebackLatency)) {
				fprintf(stderr, "Impossible. Idle cycle is out of limit\n");
				exit(-1);
			}
		} else {
			idleCycle = 0;
		}
		

		// fetching for each thread will alternate after each cycle
		for(i=0;i<SL1_MAX_THREADS;i++)
			fetch[i].fillWin(cycle);
//		if ((cycle & 1) == 0) {
//			fetch[0].fillWin(cycle);
//		} else {
//			fetch[1].fillWin(cycle);
//		}
		
		// dumping information
		if (PSIM_SL1::g_dumpIWin) {
			for (i=0; i<SL1_MAX_THREADS; i++)
				disp[i].dumpIWin(stdout);
		}
		
		// fetch
		// One fetch object for one thread
		// Fetch object knows from which trace to get instrs and to which
		// IWin buffer to send
		// Fetch strategy may be implemented here or in a strategy class
		for (i=0; i<SL1_MAX_THREADS; i++) {
			fetch[i].fillBuf(cycle);
		} 
		if (PSIM_SL1::g_dumpFetchBuffer) {
			for (i=0; i<SL1_MAX_THREADS; i++)
				fetch[i].dumpFE(stdout);
		}

		if (PSIM_SL1::g_dumpResourceStatus) PerfModelSL1::getInstance().dump(stdout);
		for (i=0; i<SL1_MAX_THREADS; i++) {
			if (disp[i].getCurIssued()) {
				vmsg(VERBOSE_2, "\t%4s: issued %d instrs at cyc %d\n", 
					getThreadName(i), disp[i].getCurIssued(), cycle);
			}
		}
		
		// check end of execution
		for (i=0; i<SL1_MAX_THREADS; i++) {
			if ((!disp[i].empty()) || (!fetch[i].empty())) {
				nRunning ++;
			}
		}
//		printf("cyc:%d one:%d\n", cycle, oneEnded);
		
		cycle ++;

		if (oneEnded==0 && nRunning == 1 && cycle>3) {
			oneEnded ++;
			printf("----------------------------------------------------\n");
			printf("   *** Partial Summary ***\n");
			printf("----------------------------------------------------\n");
			for (int j=0; j<SL1_MAX_THREADS; j++) {
				fetch[j].dumpStats(stdout);
				disp[j].dumpStats(stdout);
			}
			PerfModelSL1::getInstance().dumpStatistics(stdout);
			{
				int j;
				printf("Cycle cnt by issue #\n");
				for (j=0; j<MAX_ISSUE_PER_CYC; j++) {
					if (issueCnt[j])
						printf("\t%d %d\n",	j, issueCnt[j]);
				}
			}	
		}

		if (!nRunning)
			break;
		
	} // while(1)
	
	reportTime.End();
	float simTime = reportTime.Total_1();
	printf("\n\n");
	printf("----------------------------------------------------\n");
	printf("   *** Simulation Summary ***\n");
	printf("----------------------------------------------------\n");
	printf("Total simulation time: %.3f seconds\n", simTime);
	for (i=0; i<SL1_MAX_THREADS; i++) {
		fetch[i].dumpStats(stdout);
		disp[i].dumpStats(stdout);
	}
	PerfModelSL1::getInstance().dumpStatistics(stdout);
//	PerfModelSL1::getInstance().dumpStatistics(stdout);
	{
		int j;
		printf("Cycle cnt by issue #\n");
		for (j=0; j<MAX_ISSUE_PER_CYC; j++) {
			if (issueCnt[j])
				printf("\t%d %d\n",	j, issueCnt[j]);
		}
	}	
	delete []disp;
	delete []fetch;
	return 0;
}
