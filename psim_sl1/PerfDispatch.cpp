/* /////////////////////////////// "PerfDispatch.cpp" /////////////////////////////////
 *  File: PerfDispath.cpp
 * 
 *  Descriptions:   Implementation for issuing instructions from IWin
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */

#include "defs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfIWin.h"
#include "PerfDispatch.h"
#include "PerfModel_SL1.h"
#include "PerfGlobalOpt.h"
#include "PerfCache.h"

#define PLOG(x, y) { if (x) {y;} }

PerfDispatch::PerfDispatch()
{
	_threadId = -1;
	_issueStatus = ISSUE_IDLE;
	_maxIssued = 1;
	_curIssued = 0;
	_incOrder = true;
	_emptyFE = 0;
	_totalIssued = 0;
	_totalNops = 0;
	_totalNop16s = 0;
	_curCycle = 0;
	
	_jrStalledCnt = 0;
	_brStalledCnt = 0;
	_brInstrMissCnt = 0;
	_dcStalledCnt = 0;
	_brInstrCnt = 0;
	_LastBrPc = 0;
	_LastBrinstrPc = 0;
	_lsInstr = 0;
	#ifdef FLUSH_PIPELINE
	_issue_stop = false;
	#endif
	
	p_cache = new PerfCache();
	int i;
	for (i=0; i<HS_MAX; i++) _hazardCnt[i] = 0;
}

PerfDispatch::~PerfDispatch()
{
}

/*
 * The instruction issued will be put in another list: _issuedList.
 * Return:
 * 		ISSUE_NORMAL: No more to issued
 * 		ISSUE_CONT: More instructions may be issued.
 */
ISSUE_STATUS PerfDispatch::issue(UINT cyc, UINT maxAllowed_)
{
	ISimpleList::iterator iwinIter;
	
	HAZARD_STATUS hazardStatus;
	int nIssued = 0;
	//static int nLSInstr=0;

	if (issueStatus() == ISSUE_STOPPED) {
		_percallIssued = 0;
		return issueStatus();
	}
	#ifdef FLUSH_PIPELINE
	if(_issue_stop==true){
		issueStatus(ISSUE_STOPPED);
		_percallIssued = 0;
		return issueStatus();
	}
	#endif
			
	IsTrue(((_iwinList.size() + _issuedList.size())<EXTRA_PAGE_SIZE),
		("Size of extra cyclic instruction cache is too small. You must increase EXTRA_PAGE_SIZE to %d\n", _iwinList.size() + _issuedList.size()));

	if (_iwinList.size() > 0)	
		_curCycle = cyc;
	else {
		_emptyFE ++;
		_percallIssued = 0;
		issueStatus(ISSUE_STOPPED);
	}
		
	for (iwinIter = _iwinList.begin(); iwinIter != _iwinList.end();) {
		InstrItem& item = **iwinIter;
		ISimpleList::iterator curIter = iwinIter;
		iwinIter++;

//		if (item.perfId() >= 12402000)	Psim_EnableDumpAll();
		IsTrue((item.instrStage() == PS_UNDEF), ("instruction status must be NOT_READY in iwin"));

		// check pipeline status
		if (PerfModelSL1::getInstance().pipeLineStalled(item.threadId())) {
//			if(PSIM_SL1::g_dumpHazardDetails)
//				printf("thread %d Pipeline stalled\n", item.threadId());
			PLOG((PSIM_SL1::g_dumpHazardDetails),
				({printf("thread %d Issuing stalled: %d cycs\n", 
					item.threadId(),
					PerfModelSL1::getInstance().pipeLineLatency(item.threadId()));}));
			issueStatus(ISSUE_STOPPED);
			break;
		}

		// check if it's a nop instruction
		if (isNop(item.instrId())) {
//			delete *curIter;
			_iwinList.erase(curIter);
			_totalNops++;
			if (isNop16(item.instrId()))
				_totalNop16s++;
			continue;		
		}

//		//************To be modified
//		if (_curIssued >= 1)
//		{
//			issueStatus = ISSUE_CONT;
//			break;
//		}
		// check if issue limit for this thread is reached. Note this check takes priority over next check.
		if (_curIssued >= _maxIssued) {
			_hazardCnt[HS_MAX_ISSUE]++;
			if(PSIM_SL1::g_dumpHazardDetails) {
				printf("  %4s Instr %d: Hazard found: HS_MAX_ISSUE!!!\n", 
					getThreadName(item.threadId()), item.perfId());
			}
			issueStatus(ISSUE_STOPPED);
			break;
		}
				
		// check structure hazard
		if ((hazardStatus=PerfModelSL1::getInstance().checkHazard(&item)) != HS_NONE) {
			if(PSIM_SL1::g_dumpHazardDetails) {
				printf("  %4s Instr %d: Hazard found: %s!!!\n", 
					getThreadName(item.threadId()), item.perfId(), 
					getHazardName(hazardStatus));
			}
			
			_hazardCnt[hazardStatus] ++;
			// inorder execution will break here
			if (incOrder()) {
				issueStatus(ISSUE_STOPPED);
				break;	// no more to issue
			} else
				continue;
		}

		item.instrStage(PS_ID);
		_issuedList.push_back(&item);
		_iwinList.erase(curIter);		
		_curIssued ++;
		nIssued++;
		if(isLoadStore(item.instrGrp()))
		{
			if(PSIM_SL1::g_externalmemLow!=0 && PSIM_SL1::g_externalmemHigh!=0 && PSIM_SL1::g_externalmemLatency!=0)
			{
				if(item.getDataAddr()>=PSIM_SL1::g_externalmemLow && item.getDataAddr()<PSIM_SL1::g_externalmemHigh)//extern memory
				{
					item.latency(PSIM_SL1::g_externalmemLatency);
					_dcStalledCnt +=PSIM_SL1::g_externalmemLatency;
					if(PSIM_SL1::g_dumpHazardDetails)
						printf("instr %d add external memory load/store latency %d\n", item.perfId(), item.latency());
				}
			} 
		// check dcache miss. dcache miss simulation only for CORE thread
			else if ((item.threadId() == THREAD_ID_CORE) && (isLoad(item.instrGrp(), item.instrId()))) {
		  //if (isLoad(item.instrGrp(), item.instrId())) {    
			  	/*if ((PSIM_SL1::g_dcacheMissRatio > 0) && (PSIM_SL1::g_dcacheMissPenalty>0)) {
					if ((nLSInstr%100)<PSIM_SL1::g_dcacheMissRatio) {
						item.latency(PSIM_SL1::g_dcacheMissPenalty);
						_dcStalledCnt += PSIM_SL1::g_dcacheMissPenalty;*/
				INT dcacheMissCyc = p_cache->checkDCache(item.getDataAddr());
				if(dcacheMissCyc)
				{
					#ifdef FLUSH_PIPELINE
					_issue_stop = true;
					#endif
					item.latency(dcacheMissCyc);
					if (PSIM_SL1::g_verbose>4)
						printf("instr %d add dcache miss latency %d\n", item.perfId(), item.latency());
				}
				_lsInstr++;
			}				
		}
		if(isMacA(item.instrId()))
		{
			if(PSIM_SL1::g_membankLatency)
				if(checkMemBank(item.getAr1(),item.getAr2())==true)
				{
				    item.latency(PSIM_SL1::g_membankLatency);
				}	        			   
		}
		if(isFixed(item.instrId()))
		{
			RegListIter regIter;
			if(item.instrId()==c3fft_id)
			{
				item.latency(PSIM_SL1::g_fftLatency[item.brMeta()&FFTPOINTMASK]);
				regIter = item.getDesRegs()->begin();
				PerfModelSL1::getInstance().fftDes(*regIter,item.threadId());
			}
			else if(item.instrId()==c3viterbi_id)
			{
				regIter = item.getDesRegs()->begin();
				PerfModelSL1::getInstance().viterbiDes(*regIter,item.threadId());
				item.latency(PSIM_SL1::g_viterbiLatency);
			}
			else //c3traceback_id
			{
				regIter = item.getDesRegs()->begin();
				PerfModelSL1::getInstance().tracebackDes(*regIter,item.threadId());
				item.latency(PSIM_SL1::g_tracebackLatency);
				
			}
		}
		
		// branch, jump/return latency
		BranchListIter iter;
		if(_LastBrPc)
		{
			  iter = _branchlist.find(_LastBrinstrPc);			
			if(item.pc()!=_LastBrPc)
			{
				PerfModelSL1::getInstance().pipeLineLatency(_threadId, PSIM_SL1::g_brPenalty);
				// stall pipeline: stop issuing next instruction
			    PerfModelSL1::getInstance().pipeLineStalled(_threadId, true);
				// record
				_brStalledCnt += PSIM_SL1::g_brPenalty;
				_brInstrMissCnt++;
				if(PSIM_SL1::g_dumpHazardDetails)
			    	printf("  %4s Instr %d: Branch miss penalty: %d cycs!!!\n", 
					getThreadName(item.threadId()), item.perfId(), 
					PSIM_SL1::g_brPenalty);
				iter->second.miss_count++;
			}
			else
				iter->second.taken_count++;
			_LastBrinstrPc=0;			
			_LastBrPc=0;			
		}
		if (isBranch(item.instrGrp()) && (PSIM_SL1::g_brPenalty>0)) {
				iter = _branchlist.find(item.pc());
				if(iter==_branchlist.end())
				{
					_branchlist[item.pc()].taken_count=0;
					_branchlist[item.pc()].miss_count=0;
				}
				
			    if(iwinIter != _iwinList.end())
			    {
			    	InstrItem& item_next = **iwinIter;
			    	if(item_next.pc()!=item.getBrPc()) //predict miss
			    	{
						PerfModelSL1::getInstance().pipeLineLatency(_threadId, PSIM_SL1::g_brPenalty);
				// stall pipeline: stop issuing next instruction
						PerfModelSL1::getInstance().pipeLineStalled(_threadId, true);
				// record
						_brStalledCnt += PSIM_SL1::g_brPenalty;
						_brInstrMissCnt++;
						if(PSIM_SL1::g_dumpHazardDetails)
					   	printf("  %4s Instr %d: Branch miss penalty: %d cycs!!!\n", 
						getThreadName(item.threadId()), item.perfId(), 
						PSIM_SL1::g_brPenalty);				
						_branchlist[item.pc()].miss_count++;							 
			    	}
			    	else
			    	 	_branchlist[item.pc()].taken_count++;
			    }			    	
			    else
			    {
			        _LastBrPc = item.getBrPc();
			    	_LastBrinstrPc=item.pc();
			    }
			    _brInstrCnt++;	    	
		}
		if (item.brMeta()) {
//			IsTrue((isBranch(item.instrGrp()) || isJump(item.instrGrp())), 
//				("Error: non-zero meta for non-jump/branch instruction %s for \n", item.disasm()));
			
		} else if (isJump(item.instrGrp()) && (PSIM_SL1::g_jrPenalty>0)) {
			PerfModelSL1::getInstance().pipeLineLatency(_threadId, PSIM_SL1::g_jrPenalty);
			// stall pipeline: stop issuing next instruction
			PerfModelSL1::getInstance().pipeLineStalled(_threadId, true);
			// record
			_jrStalledCnt += PSIM_SL1::g_jrPenalty;
		}

		// check if issue limit for this time is reached
		if (maxAllowed_&&(nIssued >= maxAllowed_)) {
//			_hazardCnt[HS_MAX_ISSUE]++;
			if(PSIM_SL1::g_dumpHazardDetails) {
				printf("  %4s Instr %d: reach issue limit for a turn!!!\n", 
					getThreadName(item.threadId()), item.perfId());
			}
			issueStatus(ISSUE_CONT);
			break;
		} 					
	}
	if (PSIM_SL1::g_dumpPipe) dumpPipeLine(stdout);

	_totalIssued += nIssued;
	_percallIssued = nIssued;
	return issueStatus();
}

int PerfDispatch::update()
{
	InstrListIter issueIter;
	
	setCurIssued(0);
	issueStatus(ISSUE_IDLE);
	
	for (issueIter = _issuedList.begin(); issueIter != _issuedList.end();) {
		InstrItem &item = **issueIter;
		InstrListIter curIter = issueIter;
		issueIter ++;
		
//		printf("update instr %d using eu %d at stage %d\n", item.perfId(), item.execUnit(), item.instrStage());
		
		int ownerId = PerfModelSL1::getInstance().getInstrPerfIdInEU(item.execUnit(), 
				item.instrStage());
		IsTrue((ownerId == item.perfId()), 
			("Impossible: instr id not matched:%d eu-owner %d\n", item.perfId(), ownerId)); 
			
		switch (item.instrStage()) {
		case PS_FE:
			if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
				item.instrStage(PS_ID);
			} else {
				IsTrue((0), ("Impossible: exec unit can't be stalled in FE stage\n"));
			}
			break;
		case PS_ID:
			if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage()))
				item.instrStage(PS_EX);
			else {
				// Do not change state
				if (PSIM_SL1::g_dumpPipe) 
					printf("%4s instr %d: PIPELINE stalled due to exec unit is stalled by instr %d. \n",
						getThreadName(item.threadId()), 
						item.perfId(), 
						PerfModelSL1::getInstance().getInstrPerfIdInEU(item.execUnit(),
							item.instrStage()));
				// stall pipeline
				PerfModelSL1::getInstance().pipeLineStalled(item.threadId(), true);
				
				return 0;
			}
			
			break;
		case PS_EX:
//			if (isMAC(item.instrId())) {
//				IsTrue((item.latency() == 0), 
//					("Impossible:MAC instruction has non execution latency\n"));
//				item.latency(1);
//			}
			if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
				item.instrStage(PS_MEM);
			} else {
				IsTrue((0), ("Impossible: exec unit can't be stalled in EX stage\n"));
			}
			break;
		case PS_MEM:
			if (item.latency() && isLoadStore(item.instrGrp())) {	// memory latency will stall current instruction
				if (PSIM_SL1::g_dumpPipe) 
					printf("instr %d stall pipeline cycs: %d on mem latency\n", item.perfId(), item.latency());
				item.decLatency();
				// stall the pipeline.Dcache miss will stall current instr itself
				bool stalled;
				if (item.latency() || PerfModelSL1::getInstance().pipeLineLatency(_threadId))
					stalled = true;
				else
					stalled = false; 
				PerfModelSL1::getInstance().pipeLineStalled(_threadId, stalled);
				return 0;
			}
			
			if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
				item.instrStage(PS_WB);
			} else {
				IsTrue((0), ("Impossible: exec unit can't be stalled by another thread in MEM stage\n"));
			}
			break;
		case PS_WB:
			if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
			} else {
				IsTrue((0), ("Impossible: exec unit can't be stalled by another thread in WB stage\n"));
			}
			setLastRetiredId(item.threadId(), item.perfId());
//			delete *curIter;
			_issuedList.erase(curIter);
			break;
		default:
			IsTrue((0), ("Instr %d: Invalid status %d in issued list\n", item.perfId(), item.instrStage()));
			break;
		}
	}
	
	// branch/jump instrs will only stall succeeding instrs
	if (PerfModelSL1::getInstance().pipeLineLatency(_threadId)) {
		IsTrue((PerfModelSL1::getInstance().pipeLineStalled(_threadId) == true), 
			("Impossible: Thread %d Pipeline not stalled with latency not zero\n", _threadId));
		// succeeding instructions will not be updated
		PerfModelSL1::getInstance().decPipeLineLatency(_threadId);
		if (PerfModelSL1::getInstance().pipeLineLatency(_threadId) == 0) {
			PerfModelSL1::getInstance().pipeLineStalled(_threadId, false);
		}
//		printf("pipeline latency :%d\n", PerfModelSL1::getInstance().pipeLineLatency(_threadId));
	} else {
		if (PerfModelSL1::getInstance().pipeLineStalled(_threadId)) {
			PerfModelSL1::getInstance().pipeLineStalled(_threadId, false);
		}
	}
	return 0;
}

/*
 * Update all the items in the same stage in different threads together.
 * The instrution list is in such an order that older instructions are in the head and newer instructions
 * are in the tail.
 */
int PerfDispatch::updateParallel(PerfDispatch *disp, int num)
{
	// update stage by stage
	int stage;
	InstrListIter iter[num];
	InstrList* iList[num];
	int i;
	bool stallOneThread[SL1_MAX_THREADS];
	
	for (i=0; i<num; i++) {
		disp[i].setCurIssued(0);
		disp[i].issueStatus(ISSUE_IDLE);
	}
	
	// get the instruction list and iterators
	for (i=0; i<num; i++) {
		iList[i] = &disp[i].getIssuedList();
		iter[i] = iList[i]->begin();
	}
	for(i = 0;i<num;i++)
	   stallOneThread[i] = false;
	for (stage = PS_WB; stage >= PS_FE; stage--) {
		// process all the list together
		for (i=0; i<num; i++) {
			//stallOneThread = false;
			// Skip items whose stage has already been processed
			while ( (iter[i] != iList[i]->end()) && ((*iter[i])->instrStage() > stage) ) 
				iter[i]++;

			// Empty list, process next thread
			if (iter[i] == iList[i]->end()) continue;
			
			// No item is in current stage, process next thread
			if ((*iter[i])->instrStage() != stage) continue;
//printf("UpdateP instr %d\n", (*iter[i])->perfId());
			// Update all items 
			while( !stallOneThread[i] && (iter[i] != iList[i]->end()) && ((*iter[i])->instrStage() == stage)) {
				InstrItem &item = **iter[i];
				InstrListIter curIter = iter[i];
				iter[i] ++;
				
		//		printf("update instr %d using eu %d at stage %d\n", item.perfId(), item.execUnit(), item.instrStage());
				{
					int ownerId = PerfModelSL1::getInstance().getInstrPerfIdInEU(item.execUnit(), 
							item.instrStage());
					IsTrue((ownerId == item.perfId()), 
						("Impossible: instr id not matched:%d eu-owner %d\n", item.perfId(), ownerId)); 
					
					switch (item.instrStage()) {
					case PS_FE:
						if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
							item.instrStage(PS_ID);
						} else {
							IsTrue((0), ("Impossible: exec unit can't be stalled in FE stage\n"));
						}
						break;
					case PS_ID:
						if (isFixed(item.instrId()))
						{
							if(item.latency())
							{							
								//INT fixlatency = PerfModelSL1::getInstance().fixPipeLineLatency();
								switch(item.instrId()){
									case c3fft_id:
										PerfModelSL1::getInstance().fftLatency(item.latency());
									 	break;
									case c3viterbi_id:
										PerfModelSL1::getInstance().viterbiLatency(item.latency());
										break;
									case c3traceback_id:
										PerfModelSL1::getInstance().tracebackLatency(item.latency());
								}
								PerfModelSL1::getInstance().addFixPipeLineLatency(item.latency());									
							}
						}
						if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage()))
						{
							item.instrStage(PS_EX);
							#ifdef FLUSH_PIPELINE
		    				if (item.latency() && isLoadStore(item.instrGrp()))
		    				{
		    					PerfModelSL1::getInstance().abondonEU(item.execUnit());
		    					printf("EU not useful for Dcache miss\n");
		    				}
		    				#endif
						}
						else {
						// Do not change state
							if (PSIM_SL1::g_dumpPipe) 
								printf("%4s instr %d: PIPELINE stalled due to exec unit is stalled by instr %d. \n",
									getThreadName(item.threadId()), 
									item.perfId(), 
									PerfModelSL1::getInstance().getInstrPerfIdInEU(item.execUnit(),
										item.instrStage()));
						// stall pipeline
							PerfModelSL1::getInstance().pipeLineStalled(item.threadId(), true);
						
							stallOneThread[i] = true;
							break;
						}
						break;
					case PS_EX:
						if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
							item.instrStage(PS_MEM);
							#ifdef NO_ACC_CROSS
							RegListIter regIter;
							if(item.instrGrp()==EIG_c3_mac)
							{
								for (regIter=item.getDesRegs()->begin();
		 						regIter != item.getDesRegs()->end();
		 						regIter ++)
		 						{
		 							INT regIndex = *regIter;
		 							if(regIndex>=EBASE_ACC)
		 							{
		 								regIndex = regIndex-EBASE_ACC;
		 								PerfModelSL1::getInstance().setPipeAcc(item.threadId(),regIndex,false);
		 								PerfModelSL1::getInstance().setPipeAccEu(item.threadId(),regIndex,EXEC_UNIT_UNDEFINED);
		 							}
		 						}	
							}
							#endif
						} else {
						//IsTrue((0), ("Impossible: exec unit can't be stalled in EX stage\n"));
							printf("instr%d stall in ex tage\n",item.perfId());
						}
						break;
					case PS_MEM:
						if (item.latency() && isLoadStore(item.instrGrp())) {	// memory latency will stall current instruction
							if (PSIM_SL1::g_dumpPipe) 
								printf("instr %d stall pipeline cycs: %d on mem latency\n", item.perfId(), item.latency());
						// stall the pipeline.Dcache miss will stall current instr itself
							PerfModelSL1::getInstance().pipeLineLatency(i,item.latency()+1);
					    	PerfModelSL1::getInstance().pipeLineStalled(i, true);
							stallOneThread[i] = true;
							item.decLatency();
							if(item.latency()==0)
							{
								#ifdef FLUSH_PIPELINE
								disp[item.threadId()].set_issue_stop(false);
								PerfModelSL1::getInstance().restoreEU(item.execUnit());
								#endif
							}
							break;
						}
						if(isMacA(item.instrId()))
						{
							if(item.latency()!=0)
							{
							 	if (PSIM_SL1::g_dumpPipe)
							    	printf("instr %d stalled pipeline cycs: %d due to same mem bank read\n",item.perfId(),item.latency());						
							 	PerfModelSL1::getInstance().pipeLineLatency(i,item.latency()+1);
							 	PerfModelSL1::getInstance().pipeLineStalled(i, true);
							 	stallOneThread[i] = true;
							 	item.decLatency();
							 	break;
							}						
						}					
						if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
							item.instrStage(PS_WB);
						} else {
							IsTrue((0), ("Impossible: exec unit can't be stalled by another thread in MEM stage\n"));
						}
						break;
					case PS_WB:
						if (PerfModelSL1::getInstance().updateEUStage(item.perfId(), item.execUnit(), item.instrStage())) {
						} else {
							IsTrue((0), ("Impossible: exec unit can't be stalled by another thread in WB stage\n"));
						}
						setLastRetiredId(item.threadId(), item.perfId());
		//			delete *curIter;
						iList[i]->erase(curIter);
						break;
					default:
						IsTrue((0), ("Instr %d: Invalid status %d in issued list\n", item.perfId(), item.instrStage()));
						break;
					}	// switch
				}//if
			} // while
		}	// list loop
	}	// stage loop
	
	// decrement pipeline latency counter
	// branch/jump instrs will only stall succeeding instrs
	for (i=0; i<num; i++) {
		if (PerfModelSL1::getInstance().pipeLineLatency(i)) {
			IsTrue((PerfModelSL1::getInstance().pipeLineStalled(i) == true), 
				("Impossible: Thread %d Pipeline not stalled with latency not zero\n", i));
			// succeeding instructions will not be updated
			PerfModelSL1::getInstance().decPipeLineLatency(i);
			if (PerfModelSL1::getInstance().pipeLineLatency(i) == 0) {
				PerfModelSL1::getInstance().pipeLineStalled(i, false);
			}
	//		printf("pipeline latency :%d\n", PerfModelSL1::getInstance().pipeLineLatency(_threadId));
		} else {
			if (PerfModelSL1::getInstance().pipeLineStalled(i)) {
				PerfModelSL1::getInstance().pipeLineStalled(i, false);
			}
		}
		
	}
	if(PerfModelSL1::getInstance().fixPipeLineLatency())
	{
		if(PerfModelSL1::getInstance().fftLatency() && PerfModelSL1::getInstance().fftActive()==true)
		{
			PerfModelSL1::getInstance().decFftLatency();
			PerfModelSL1::getInstance().decFixPipLineLatency();
		}
		else if(PerfModelSL1::getInstance().viterbiLatency() && PerfModelSL1::getInstance().viterbiActive()==true)
		{
			PerfModelSL1::getInstance().decViterbiLatency();
			PerfModelSL1::getInstance().decFixPipLineLatency();
		}
		else if(PerfModelSL1::getInstance().tracebackLatency() && PerfModelSL1::getInstance().tracebackActive()==true)
		{
			PerfModelSL1::getInstance().decTracebacLatency();
			PerfModelSL1::getInstance().decFixPipLineLatency();
		}	
		/*
		 * else
		 * {
		 * IsTrue((0),("Invalid fixed func engine type cause fixed func engine stalled %d,%d,%d,%d\n"));
		 * } 
		 */
			
	}			
	return 0;
}

void PerfDispatch::dumpIWin(FILE *fp)
{
	if (!_iwinList.size())
		return;
	fprintf(fp, "-------%4s iWin: %d instrs -------\n", 
		getThreadName(_threadId),
		_iwinList.size());
	ISimpleList::iterator iter;
	int cc=0;
	for (iter=_iwinList.begin(); iter!=_iwinList.end(); iter++) {
		InstrItem &item = **iter; 
		fprintf(fp, "  iwin%02d  %08d WAIT %s\n", cc, item.perfId(), item.disasm());
		cc++;  
	}
}

void PerfDispatch::dumpPipeLine(FILE *fp)
{
	if (!_issuedList.size())
		return;
	fprintf(fp, "-------%4s pipeline: %d instrs -------\n", 
		getThreadName(_threadId),
		_issuedList.size());
	InstrListIter iter;
	int cc=0;
	for (iter=_issuedList.begin(); iter!=_issuedList.end(); iter++) {
		InstrItem &item = **iter; 
		fprintf(fp, "  PIPE%02d  %08d %4s %s\n", cc, item.perfId(), 
			getStageName(item.instrStage()), item.disasm());
		cc++;  
	}
}

void PerfDispatch::dumpStats(FILE *fp)
{
	fprintf(fp, "%5s - Total issued: %d in %d cycs, issue rate %.2f/cyc\n", 
		getThreadName(_threadId), 
		_totalIssued,
		_curCycle,
		(_curCycle>0)?((float)_totalIssued/_curCycle) : 0.0f);
	int i;
	for (i=0; i<HS_MAX; i++) {
		if (_hazardCnt[i] == 0)
			continue;
		fprintf(fp, "    %5s: %s %d times, ave %.2f/cyc\n",
			getThreadName(_threadId),
			getHazardName((HAZARD_STATUS)i),
			_hazardCnt[i], 
			(_hazardCnt[i]>0)?(float)_hazardCnt[i]/_curCycle : 0.0f);
	}
	fprintf(fp, "     FE empty: %d\n", _emptyFE);
	
	fprintf(fp, "     NOP:%d, NOP32:%d, NOP16:%d\n", _totalNops, _totalNops-_totalNop16s, _totalNop16s);
	fprintf(fp, "     Latency--Jump: %d  Branch: %d D$: %d\n", _jrStalledCnt, _brStalledCnt, p_cache->dcacheMissed());
	fprintf(fp, "     Load times: %d\n",_lsInstr);
	if(PSIM_SL1::g_dumpBranchInfo==true)
	{
    	fprintf(fp, "     Branch summary: \n"); 
    	fprintf(fp, "     Branch instr cnt: %d  Branch miss cnt: %d\n",_brInstrCnt, _brInstrMissCnt);    
    	BranchListIter iter;    
    	for(iter = _branchlist.begin();iter!=_branchlist.end();iter++)
    	{
    		fprintf(fp,"     Branch instruction pc:0x%x,taken time:%d,miss time:%d\n",iter->first,iter->second.taken_count,
    	    	   iter->second.miss_count);
    	}
	}	    
}
