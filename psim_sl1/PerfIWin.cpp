/* /////////////////////////////// "PerfIWin.cpp" /////////////////////////////////
 *  File: PerfIWin.cpp
 * 
 *  Descriptions:   Implementation for issuing window management
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
 
#include "defs.h"
#include "messg.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfIWin.h"
#include "PerfModel_SL1.h"

#include "memory.h"
#include "sl1address.h"
#include "sl1mmu.h"
#include "sl1reg.h"

#include "sl1instr.h"
#include "sl1disasm.h"
#include "sl1decoder.h"

#include "PerfGlobalOpt.h"

static SL1Instr sl1instr;
static SL1Decoder sl1decoder;
static Address addr;
static Memory mem(addr);
static ProcessStatus<SL1Instr> status;
static SL1MMU mmu(addr, status);
#ifndef MULTI_THREAD
static SL1Register regCore(status,THREAD_ID_CORE);
static SL1Register regBB(status,THREAD_ID_BB);
static SL1Disasm sl1disasmCore(mmu, regCore, THREAD_ID_CORE);
static SL1Disasm sl1disasmBB(mmu, regBB, THREAD_ID_BB);
#else
static SL1Register regThread0(status,THREAD_ID_0);
static SL1Register regThread1(status,THREAD_ID_1);
static SL1Register regThread2(status,THREAD_ID_2);
static SL1Register regThread3(status,THREAD_ID_3);
static SL1Disasm sl1disasmThread0(mmu, regThread0, THREAD_ID_0);
static SL1Disasm sl1disasmThread1(mmu, regThread1, THREAD_ID_1);
static SL1Disasm sl1disasmThread2(mmu, regThread2, THREAD_ID_2);
static SL1Disasm sl1disasmThread3(mmu, regThread3, THREAD_ID_3);
#endif

SL1Disasm *getDisasm(InstrItem *item){
	switch(item->_threadId) {
#ifndef MULTI_THREAD		
	case THREAD_ID_CORE:
		return &sl1disasmCore;
	case THREAD_ID_BB:
	    return &sl1disasmBB;
#else
	case THREAD_ID_0:
	    return &sl1disasmThread0;
	case THREAD_ID_1:
	    return &sl1disasmThread1;
	case THREAD_ID_2:
	    return &sl1disasmThread2;
	case THREAD_ID_3:
	    return &sl1disasmThread3;
#endif	           
	default:
		IsTrue((0), ("Invalid thread id %d in getDisasm()\n",  item->_threadId));
	}
	return NULL;
}


PerfIWin::PerfIWin()
{
	setIWinSizeLmt(DEFAULT_IWIN_LMT);
}

PerfIWin::~PerfIWin()
{
}

int PerfIWin::add(InstrItem *item, int cyc)
{
	IsTrue((item!=NULL), ("Invalid instruction item\n"));
	_iwinList.push_back(item);
	
	// let's decode here
	sl1instr.init();
	item->_desRegs.clear();
	item->_srcRegs.clear();
	sl1decoder.decode(&sl1instr, item->_pc, item->_raw, item->_brMeta);
	sl1instr.getRegList(&item->_desRegs, &item->_srcRegs);
	item->_instrGrp = sl1instr.group();
	item->_instrId = sl1instr.id();
	item->_fetchCyc = cyc;
	item->_instrStatus = PS_UNDEF;
	item->_hazardStatus = HS_NONE;
	if(item->_instrGrp==EIG_br || item->_instrGrp==EIG_br16)
		item->setBrPc(sl1instr.offset());
//	item->_latency = getInstrLatency(item->_instrGrp, item->_instrId);

	// Define latency for MAC group instructions
	//if (item->instrGrp() == EIG_c3_mac)
		//item->_latency = PSIM_SL1::g_macLatency;

	pair<int,int> rwPortPair;
	rwPortPair = sl1instr.gprRegPort();
//	item->_readPorts = item->_srcRegs.size();
//	item->_writePorts = item->_desRegs.size();
	item->_readPorts = rwPortPair.first;
	item->_writePorts = rwPortPair.second;
	item->_execUnit = EXEC_UNIT_UNDEFINED; // Will be allocated when issuing

	strncpy(item->_disasmBuf, getDisasm(item)->disasm(item->_pc, &sl1instr, item->_brMeta), MAX_DISASM_BUF_LEN-1);
	char *p = strchr(item->_disasmBuf, '\n');
	if (p)
		*p = '\0';
	item->_disasmBuf[MAX_DISASM_BUF_LEN-1] = '\0';
	if (PSIM_SL1::g_dumpFetchBuffer) {	
		printf("  %4s TRACE %08d: %s", getThreadName(item->_threadId), item->perfId(), item->_disasmBuf, item->isWordEnd());
		RegListIter rlIter = item->_desRegs.begin();
		printf("wRegs:");
		for (;rlIter != item->_desRegs.end(); rlIter++) {
			printf("%d ", *rlIter);
		}
		printf("rRegs:");
		for (rlIter = item->_srcRegs.begin(); rlIter != item->_srcRegs.end(); rlIter++) {
			printf("%d ", *rlIter);
		}
		printf("\n");
	}
	if(!(rwPortPair.first == item->_readPorts)) printf("Read ports not equal:%d -> old:%d\n", rwPortPair.first, item->_readPorts);
	if(!(rwPortPair.second == item->_writePorts)) printf ("Write ports not equal\n");
	return 0;
}

void PerfIWin::preUpdate(void) 
{
}
