/* /////////////////////////////// "PerfRegDep_SL1.cpp" /////////////////////////////////
 *  File: PerfRegDep_SL1.cpp
 *  Descriptions:  Implement register dependency checking methods for SL1.
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#include "defs.h"
#include "cregdefs.h"
#include "sl1regdefs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfModel_SL1.h"
#include "PerfGlobalOpt.h"
#include <string.h>

// For fast lookup from register index to register set. It will be initialized during construction.
INT RegSetInfo::SL1RegIndex2SetMap[SL1_ALL_REGS_SIZE];	// reg index: 0..n / type: GPR/ACC/AR/CTRL

typedef struct {
	int mc_id1;
	int mc_id2;
}MC_INSTR_PAIR;

#define MC_INSTR_MINID mcreq_id
#define MC_INSTR_MAXID mcznle_id
#define MC_INSTR_DIST (MC_INSTR_MAXID-MC_INSTR_MINID+1)	// Total instructions of conditional move

// This table is just used for building SL1McInstrPairTable
// Assume mcreq_id is the smallest and mcznle_id is the biggest
MC_INSTR_PAIR SL1McInstrPairTable[] =
{
	{mcreq_id, mcrne_id},
	{mcrne_id, mcreq_id},
	{mcrgt_id, mcrle_id},
	{mcrge_id, mcrlt_id},
	{mcrlt_id, mcrge_id},
	{mcrle_id, mcrgt_id},
	{mczceq_id, mczcne_id},
	{mczcne_id, mczceq_id},
	{mczcgt_id, mczcle_id},
	{mczcge_id, mczclt_id},
	{mczclt_id, mczcge_id},
	{mczcle_id, mczcgt_id},
	{mczeq_id,  mczne_id},
	{mczne_id,  mczeq_id},
	{mczgt_id,  mczle_id},
	{mczge_id,  mczlt_id},
	{mczlt_id,  mczge_id},
	{mczle_id,  mczgt_id},
	{mczneq_id, mcznne_id},
	{mcznne_id, mczneq_id},
	{mczngt_id, mcznle_id},
	{mcznge_id, mcznlt_id},
	{mcznlt_id, mcznge_id},
	{mcznle_id, mczngt_id},
};
int SL1McInstrPairLookupTable[MC_INSTR_DIST];

int initInstrPairLookupTable(void)
{
	int i;
	for (i=0; i<MC_INSTR_DIST; i++) {
		SL1McInstrPairLookupTable[i] = -1;
	}
	for (i=0; i<sizeof(SL1McInstrPairTable)/sizeof(MC_INSTR_PAIR); i++) {
		IsTrue(((SL1McInstrPairTable[i].mc_id1>=MC_INSTR_MINID) && 
			(SL1McInstrPairTable[i].mc_id2<=MC_INSTR_MAXID) &&
			(SL1McInstrPairTable[i].mc_id2>=MC_INSTR_MINID)), ("Invalid mc instruction table entry:%d\n", i));
		int k = SL1McInstrPairTable[i].mc_id1 - MC_INSTR_MINID;
		SL1McInstrPairLookupTable[k] = SL1McInstrPairTable[i].mc_id2;
	}
	// verify the pair
	for (i=0; i<MC_INSTR_DIST; i++) {
		int k;
		if (SL1McInstrPairLookupTable[i]>-1) {
			int pair = SL1McInstrPairLookupTable[i] - MC_INSTR_MINID;
			IsTrue(((pair>=0)&&(SL1McInstrPairLookupTable[pair]-MC_INSTR_MINID == i)), 
				("Invalid mc instruction table:not paired\n"));
		}
	}
	return 0;
}

void RegSetInfo::initTables(void)
{
	int i;
	
	// Construct register index to register set map
	for (i=0; i<SL1_ALL_REGS_SIZE; i++) SL1RegIndex2SetMap[i] = RS_UNDEFINED;
//	printf("registers: GPR:%d CTRL:%d ACC:%d AR:%d All registers:%d\n",
//		REG_GPR_SIZE, REG_ALL_CTRL_SIZE, REG_ACC_SIZE, REG_AR_SIZE, SL1_ALL_REGS_SIZE);
		
	for (i=EBASE_GPR; i<EBASE_GPR+REG_GPR_SIZE; i++)  {
//		printf("reg index:%d filled\n", i);
		IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
		SL1RegIndex2SetMap[i] = REGCLASS_GPR;
	}
	for (i=EBASE_CTRL; i<EBASE_CTRL+REG_ALL_CTRL_SIZE; i++)	{ 
//		printf("reg index:%d filled\n", i);
		IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
		SL1RegIndex2SetMap[i] = REGCLASS_CTRL;
	}
	for (i=EBASE_ACC; i<EBASE_ACC+REG_ACC_SIZE; i++) {
//		printf("reg index:%d filled\n", i);
		IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
		SL1RegIndex2SetMap[i] = REGCLASS_ACC;
	}
	for (i=EBASE_AR; i<EBASE_AR+REG_AR_SIZE*2; i++) {
//		printf("reg index:%d filled\n", i);
		IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
		SL1RegIndex2SetMap[i] = REGCLASS_AR;
	}
	
	for(i=EBASE_HI; i<EBASE_HI + REG_HI_SIZE;i++) {
	    IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
	    SL1RegIndex2SetMap[i] = REGCLASS_HI;	
	}
	i = EBASE_FFT;
	IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
    SL1RegIndex2SetMap[i] = REGCLASS_FFT;
    
    i = EBASE_VITERBI;
	IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
    SL1RegIndex2SetMap[i] = REGCLASS_VITERBI;
    
    i = EBASE_TRBACK;
	IsTrue((SL1RegIndex2SetMap[i]==RS_UNDEFINED),("register set map %d is not empty",i));
    SL1RegIndex2SetMap[i] = REGCLASS_TRBACK;
	// verify
	for (i=0; i<SL1_ALL_REGS_SIZE; i++) {
		IsTrue((SL1RegIndex2SetMap[i] != RS_UNDEFINED), ("Invalid register index-2-set map:%d\n", i));
	}
	
	initInstrPairLookupTable();
}

void RegSetInfo::lockReg(InstrItem *item)
{
	RegListIter regIter;
	// update read lock
	for (regIter=item->getSrcRegs()->begin();
		 regIter != item->getSrcRegs()->end();
		 regIter ++) {
		INT regIndex = *regIter;
		_regUse[regIndex].lock = item->perfId();
	}

	// update write lock
	for (regIter=item->getDesRegs()->begin();
		 regIter != item->getDesRegs()->end();
		 regIter ++) {
		INT regIndex = *regIter;
		_regStatus[regIndex].status  = getRegAccessType(item->instrGrp(), item->instrId());
//		printf("instr %d: reg %d status changed to %d\n", item->perfId(), regIndex, _regStatus[regIndex].status);
//		_regStatus[regIndex].cycle = PIPE_NORMAL_CYCLES;
		_regStatus[regIndex].instrItem = item;
		_regDefine[regIndex].lock  = item->perfId();
	}
}

HAZARD_STATUS RegSetInfo::checkRegReadHazard(INT threadId, INT regIndex, INT perfId)
{
	// RAR is not a hazard
	// RAW: check current issue window: instructions to be issued in the same cycle
	int iState;
	if (regIndex == 0)	// ignore regster 0 because it's read-only
		return HS_NONE;
	HAZARD_STATUS hazardStatus = HS_NONE;
//	printf("thread %d checkRAW regIndex:%d current %d: writerequest %d\n",	
//		threadId, regIndex, perfId, _regSetInfo[threadId]._regDefine[regIndex].request);
	if ((_regDefine[regIndex].request>0) &&
	(_regDefine[regIndex].request != perfId)) {
		hazardStatus = HS_REG_RAW;
		
		if (REG_RA_INDEX == regIndex) {	// Link register 'RA' dependency
			IsTrue((PSIM_SL1::g_linkRegisterLatency>0), ("Link register latency is not positive\n"));
			PerfModelSL1::getInstance().pipeLineLatency(threadId, PSIM_SL1::g_linkRegisterLatency);
			PerfModelSL1::getInstance().pipeLineStalled(threadId, true);
		}
		
		if (PSIM_SL1::g_dumpHazardDetails) {
			if (REG_RA_INDEX == regIndex) {	// Link register 'RA' dependency
				printf("  %4s: RAW hazard on link register: %d with %d on reg %d\n",
					getThreadName(threadId), perfId, _regDefine[regIndex].request, regIndex);
			} else {
				printf("  %4s: RAW hazard: %d with %d on reg %d\n",
					getThreadName(threadId), perfId, _regDefine[regIndex].request, regIndex);
			}
		}
	} else if (_regStatus[regIndex].status == RS_MEM_WB) {
		// RAW: check instructions issued in previous cycles (already in pipeline)
		
		// Check link register latency first. Return "jr.link" instructions can' be issued
		// until link register is written back or the instruction writing to link register is complete.
		if ((REG_RA_INDEX == regIndex)&&
			((_regStatus[regIndex].instrItem->instrId() == mvtc_id) ||
				(_regStatus[regIndex].instrItem->instrId() == mvtc16_id) || 
				(_regStatus[regIndex].instrItem->instrId() == mvtci_id)) 
			){
			iState = _regStatus[regIndex].instrItem->instrStage();
			hazardStatus = HS_REG_RAW;
			if (iState == PS_WB) {
				// delay one cycle
			} else {
				// delay two or more cycles
				PerfModelSL1::getInstance().pipeLineLatency(threadId, PS_WB - iState + 1);
				PerfModelSL1::getInstance().pipeLineStalled(threadId, true);
			}
			if (PSIM_SL1::g_dumpHazardDetails) {
				printf("  %4s: RAW hazard on link register: %d with an instr in pipeline on reg %d. Delay %d cycles.\n",
					getThreadName(threadId), perfId, regIndex, PS_WB - iState + 1);
			}
		} else if (_regStatus[regIndex].instrItem->instrStage() <= PS_EX+PSIM_SL1::g_RAWBasicLatency-1) {
			// In SL1, LS instructions is the only type that will cause below
			// Latency is at least 2 cycles.
			hazardStatus = HS_REG_RAW;
			if (PSIM_SL1::g_dumpHazardDetails) {
				printf("  %4s: RAW hazard: %d with an instr in pipeline on reg %d\n",
					getThreadName(threadId), perfId, regIndex);
				printf("instr %d pipeline stage %s\n", _regStatus[regIndex].instrItem->perfId(), 
					getStageName(_regStatus[regIndex].instrItem->instrStage()));
			}
		}
	}
	else if (_regStatus[regIndex].status==RS_NORMAL)
	{
		int instrid = _regStatus[regIndex].instrItem->instrId();
		if(instrid==c3fft_id || instrid==c3viterbi_id || instrid==c3traceback_id)
   				  //if(_regStatus[regIndex].instrItem->brMeta()&MODE_FLAG_MASK)
   		{
   			int brmeta = _regStatus[regIndex].instrItem->brMeta();
   			brmeta = (instrid==c3fft_id)?brmeta>>FFTPOLLPOSION:brmeta;
   			if(brmeta==0)
   			{
   				hazardStatus = HS_REG_RAW;
   				if (PSIM_SL1::g_dumpHazardDetails) {
   					printf("%4s: RAW hazard on fixed engine instruction: %d with %d in pipeline on reg %d.\n",
   						getThreadName(threadId),perfId,_regStatus[regIndex].instrItem->perfId(),regIndex);
   				  	
   				}
   			}
		}
		else if(_regStatus[regIndex].instrItem->instrStage()<=PS_EX){
		switch(_regStatus[regIndex].instrItem->instrId()){
			case c3saadds_id:
   			case c3saadda_id:
   			case c3samulsh_id:
   			case c3samulha_id:
   			case c3sasubsh_id:
   			case c3sasuba_id:
   			case c3muls_id:
   			case c3mulus_id:
   			//case c3saaddha_id:
   			//case c3sasubha_id:
   			case c3saaddsh_id:
   			case c3sasubs_id:
   			case c3dsub_id:
   			case c3dadd_id:
   				  hazardStatus = HS_REG_RAW;
   				  iState = _regStatus[regIndex].instrItem->instrStage();
   				  if (PSIM_SL1::g_dumpHazardDetails) {
   				   	  printf("%4s: RAW hazard on MAC instruction: %d with %d in pipeline on reg %d.Delay %d cycles.\n",
   				   	         getThreadName(threadId), perfId,_regStatus[regIndex].instrItem->perfId(),regIndex,PS_EX-iState+1);
   				  }  			
		}
			if (regIndex>=EBASE_ACC && regIndex<EBASE_HI)
			{
			  	if (_regStatus[regIndex].instrItem->instrStage()<=PS_EX)
			   	 	hazardStatus = HS_REG_RAW;
   				 	iState = _regStatus[regIndex].instrItem->instrStage();
   				 	if (PSIM_SL1::g_dumpHazardDetails) {
   				   	 	printf("%4s: RAW hazard on c3.mvfs instruction: %d with %d in pipeine on acc%d.Delay %d cycles.\n",
   				   	         getThreadName(threadId), perfId,_regStatus[regIndex].instrItem->perfId(),regIndex-EBASE_ACC,PS_EX-iState+1);
   				 	}	   
			}	    
		}
	}
	// update read request
	_regUse[regIndex].request = perfId;
	return hazardStatus;
}

/*
 * During long-time running, this function will simply update 'Define' table for RAW check. 
 */
HAZARD_STATUS RegSetInfo::checkRegWriteHazard(INT threadId, INT regIndex, INT perfId)
{
	HAZARD_STATUS hazardStatus = HS_NONE;
	if (!regIndex) {
//		IsTrue((0), ("Can't write register r0\n"));
		return HS_NONE;	// not readched
	}
	// WAW hazard
	int requestId=_regDefine[regIndex].request;
	if ((requestId != 0) && (requestId != perfId)) {
		//WAW hazard
		if (true/**/) {
			hazardStatus = HS_REG_WAW;
			if (PSIM_SL1::g_dumpHazardDetails)
				printf("  %4s: WAW hazard occurs between %d and %d when executing in-order\n",
					getThreadName(threadId), perfId, requestId,regIndex);
		}
	} else {
		// WAR hazard is not checked for in-order execution
	}
	
	// No WAR/WAW check for instructions in previous cycles
	
	// update write request
	_regDefine[regIndex].request = perfId;
	return hazardStatus;
}

/*
 * !!!!!!!!!!Not used now!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Special WAW dependency check.
 * This is to deal with conditional move pairs like below:
 *   mc.z.eq r7,r8,r10,1
 *   mc.z.ne r7,r8,r9,1
 * If they can issue in the same cycle, no WAW hazard here; otherwise, yes.
 * Current SL1 arch doesn't allow these pair to be issued in the same cycle, so it's still WAW hazard.
 * Return:
 * 		true	WAW hazard found
 * 		false	WAW hazard not found
 */
bool RegSetInfo::checkCondMovePairWAW(InstrItem *curInstrItem, InstrItem *prevInstrItem)
{
	// Comment out below line if it's not an hazard
	return true;
	
	int requestId = curInstrItem->perfId();
	if (prevInstrItem && (prevInstrItem->perfId() == requestId) &&
		(SL1McInstrPairLookupTable[prevInstrItem->instrId()-MC_INSTR_MINID] == curInstrItem->instrId())) {
		if (PSIM_SL1::g_dumpHazardDetails)
			printf("Conditional move pair is not WAW: %d - %d\n", curInstrItem->instrId(), requestId);
		return false;
	}
	return true;
}

HAZARD_STATUS RegSetInfo::checkRegHazard(InstrItem *item)
{
	RegListIter regIter;
	HAZARD_STATUS hazStat = HS_NONE;
	INT threadId = item->threadId();
	// check read hazard
	for (regIter=item->getSrcRegs()->begin();
		 regIter != item->getSrcRegs()->end();
		 regIter ++) {
		INT regIndex = *regIter;
		if ((hazStat = checkRegReadHazard(threadId, regIndex, item->perfId())) == HS_NONE) {
			_readPortsReq[getRegType(regIndex)] ++;
		} else {
//			printf("src reg dependency found. Reason:%d\n", hazStat);
			return hazStat;
		}
	}
	
	// check write harzard: in-order exec doesn't need this?
	for (regIter=item->getDesRegs()->begin();
		 regIter != item->getDesRegs()->end();
		 regIter ++) {
		INT regIndex = *regIter;
		if ((hazStat = checkRegWriteHazard(threadId, regIndex, item->perfId())) == HS_NONE) {
			_writePortsReq[getRegType(regIndex)] ++;
		} else {
//			printf("des reg dependency found. Reason:%d\n", hazStat);
			return hazStat;
		}
	}

	// check register read/write ports contention
	if ((_readPortsReq[REGCLASS_GPR] + _readPorts[REGCLASS_GPR] > _maxReadPorts[REGCLASS_GPR])) {
		if (PSIM_SL1::g_dumpHazardDetails)
			printf("thread %d instr %d: READ_PORT_JAM\n", item->threadId(), item->perfId());
		return HS_READ_PORT_JAM;		
	}
	if ((_writePortsReq[REGCLASS_GPR] + _writePorts[REGCLASS_GPR] > _maxWritePorts[REGCLASS_GPR])) {
		if (PSIM_SL1::g_dumpHazardDetails)
			printf("thread %d instr %d: WRITE_PORT_JAM\n", item->threadId(), item->perfId());
		return HS_WRITE_PORT_JAM;
	}
	return HS_NONE;
}

