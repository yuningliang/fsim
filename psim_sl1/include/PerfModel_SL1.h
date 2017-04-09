/* /////////////////////////////// "PerfModel_SL1.h" /////////////////////////////////
 *  File: PerfModel_SL1.h
 *  Descriptions:  Define SL1-dependent parameters for simulation
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#ifndef PERFMODEL_SL1_H_
#define PERFMODEL_SL1_H_

#include "singleton.h"
#include "sl1defs.h"
#include "sl1regdefs.h"
#include "sl1instrid.h"
#include "sl1instrtable.h"
#include "PerfRegDep_SL1.h"
#include "regdefs.h"

#define FFTPOINTMASK 7
#define FFTPOLLPOSION 3
#define POLL_ENABLE 1
#define POLL_DISABLE 0
#define SL1_MAX_RSC           32
#define SL1_MAX_RSC_NAME      16
#define SL1_MAX_RSC_TYPE_NAME 10
#define SL1_RSC_MAX_TYPES     4
#define DSP_DARAM_MASK 0x00008000
#define DSP_DARAM_MASK_LOW 0xc
#define GET_MEM_BANK(addr) ((addr&DSP_DARAM_MASK)>>13 | ((addr & DSP_DARAM_MASK_LOW)>>2))

class InstrItem;

typedef struct {
	HAZARD_STATUS hazardStatus;
	char *hazardStatusName;
}HAZARD_STATUS_ITEM;

typedef struct {
	UINT eig;
	UINT latency;
}INSTR_LATENCY_ITEM;

class CPerfModelSL1
{
public:
	// Tables for dumping and quick lookup
	static HAZARD_STATUS_ITEM SL1HazardStatusNameMap[];
	static STRING stageNames[];
	static STRING threadNames[SL1_MAX_THREADS];
	static INSTR_LATENCY_ITEM SL1InstrLatencyTable[];
	static UINT _lastRetiredId[SL1_MAX_THREADS];
	static UINT _euHazardCnt[SL1_MAX_THREADS][EIG_max];
	
private:
	RegSetInfo _regSetInfo[SL1_MAX_THREADS];	// one register set per thread
	
	int _execUnitStatus[SL1_MAX_RSC][PS_LAST]; // >=0: instr perf id; -1: IDLE
                                      // one set of execution units for all threads
    int _execUnitUsageCnt[SL1_MAX_RSC][SL1_MAX_THREADS];
    int _execUnitNumber;
    
    int _pipeLineLatency[SL1_MAX_THREADS];
    int _fixPipeLineLatency;//for fixed func engine 
    int _fftLatency;
    int _viterbiLatency;
    int _tracebackLatency;

    int _fftDes[SL1_MAX_THREADS];
    int _viterbiDes[SL1_MAX_THREADS];
    int _tracebackDes[SL1_MAX_THREADS];
    bool _fftActive;
    bool _viterbiActive;
    bool _tracebackActive;

    #ifdef NO_ACC_CROSS
    bool _pipe_acc[SL1_MAX_THREADS][REG_ACC_CUR_SIZE];
    unsigned int _pipe_acc_eu[SL1_MAX_THREADS][REG_ACC_CUR_SIZE];
    #endif
    
	InstrItem *_prevInstrItem;	// for quick check
	InstrItem *_curInstrItem;
	
	int initInstrRscMap(void);
	int initInstrPairLookupTable(void);
	
	bool _pipeLineStalled[SL1_MAX_THREADS];
	inline void updateExecUnits();
	
public:
	CPerfModelSL1();
	~CPerfModelSL1();
	
	#ifdef FLUSH_PIPELINE
	void abondonEU(int eu);
	void restoreEU(int eu);
	#endif
	void pipeLineStalled(int threadId_, bool stall) { _pipeLineStalled[threadId_] = stall; }
	bool pipeLineStalled(int threadId_) { return _pipeLineStalled[threadId_]; }
	void pipeLineLatency(int threadId_, int latency_) {_pipeLineLatency[threadId_] = latency_;}
	int  pipeLineLatency(int threadId_) { return _pipeLineLatency[threadId_]; }
	void decPipeLineLatency(int threadId_) { _pipeLineLatency[threadId_]--; }
	
	int fixPipeLineLatency(void) { return _fixPipeLineLatency;}
	void addFixPipeLineLatency(int latency) { _fixPipeLineLatency += latency;}
	void decFixPipLineLatency(void) { _fixPipeLineLatency--; }
	
	
	int fftLatency(void) {return _fftLatency;}
	void fftLatency(int latency) { _fftLatency = latency;}
	void decFftLatency(void) { _fftLatency--; }
	bool fftActive(void) { return _fftActive; }
	void fftActive(bool flag) { _fftActive = flag; }
	
	int viterbiLatency(void) { return _viterbiLatency; }
	void viterbiLatency(int latency) { _viterbiLatency = latency; }
	void decViterbiLatency(void) { _viterbiLatency--; }
	bool viterbiActive(void) { return _viterbiActive; }
	void viterbiActive(bool flag) { _viterbiActive = flag; }
	
	int tracebackLatency(void) {return _tracebackLatency;}
	void tracebackLatency(int latency) { _tracebackLatency = latency;}
	void decTracebacLatency(void) { _tracebackLatency--; }
	bool tracebackActive(void) { return _tracebackActive; }
	void tracebackActive(bool flag) { _tracebackActive = flag; }
	

	int  fftDes(int threadId_) { return _fftDes[threadId_]; }
	void fftDes(int i, int threadId_) { _fftDes[threadId_] = i; }
	
	int  viterbiDes(int threadId_) { return _viterbiDes[threadId_]; }
	void viterbiDes(int i, int threadId_) { _viterbiDes[threadId_] = i; }
	
	int  tracebackDes(int threadId_) { return _tracebackDes[threadId_]; }
	void tracebackDes(int i, int threadId_) {_tracebackDes[threadId_] = i; }

	
	bool updateEUStage(int perfId, int eu, int eu_stage);	// false: stalled; true: EU change to next state
	int  getInstrPerfIdInEU(int eu, int eu_status);
	#ifdef NO_ACC_CROSS
	void setPipeAcc(int thread_id,int regindex,bool a){ _pipe_acc[thread_id][regindex] = a;}
	void setPipeAccEu(int thread_id,int regindex,unsigned int eu) {_pipe_acc_eu[thread_id][regindex] = eu;}
	#endif
	
	void update();
	
	HAZARD_STATUS checkHazard(InstrItem *item);
	
	void dump(FILE *out);
	void dumpConfig(FILE *out);
	void dumpStatistics(FILE *out);
	
};
#ifdef FLUSH_PIPELINE
inline void CPerfModelSL1::abondonEU(int eu)
{
	_execUnitStatus[eu][PS_ID] = -2;
}

inline void CPerfModelSL1::restoreEU(int eu)
{
	_execUnitStatus[eu][PS_ID] = PS_UNDEF;
}
#endif
inline bool CPerfModelSL1::updateEUStage(int perfId, int eu, int eu_stage)
{
//	printf("updateEUStage instr %d eu %d eu_status %d", perfId, eu, eu_stage);
	IsTrue(((eu>=0) && (eu<_execUnitNumber)), ("Invalid exec unit index:%d\n", eu));
	IsTrue((eu_stage>=0 && eu_stage<PS_LAST),("Invalid eu status:%d\n", eu_stage));
	IsTrue((PS_UNDEF != eu_stage),("Exec unit already idle\n"));
	IsTrue((_execUnitStatus[eu][eu_stage] == perfId), ("Impossible: update EU state by wrong instruction\n"));
	if (PS_WB == eu_stage) {
		_execUnitStatus[eu][PS_WB] = PS_UNDEF;
	} else if (_execUnitStatus[eu][eu_stage+1] != PS_UNDEF) {
		printf("Failed to put eu %d@%d(instr %d) to next stage. It's still occupied by instr %d\n",
			eu, eu_stage, perfId,  _execUnitStatus[eu][eu_stage+1]);
		return false;
	} else {
		_execUnitStatus[eu][eu_stage+1] = perfId;
		_execUnitStatus[eu][eu_stage] = PS_UNDEF;
	}
	return true;
}

inline int CPerfModelSL1::getInstrPerfIdInEU(int eu, int eu_stage) {
	IsTrue(((eu>=0) && (eu<_execUnitNumber)), ("Invalid exec unit index:%d (0..%d)\n", eu, _execUnitNumber));
	IsTrue((eu_stage>=0 && eu_stage<PS_LAST),("Invalid eu status:%d\n", eu_stage));
//	printf("eu:%d eu_status:%d\n", eu, eu_stage);
	return _execUnitStatus[eu][eu_stage];
}

inline STRING getHazardName(HAZARD_STATUS hs)
{
	return CPerfModelSL1::SL1HazardStatusNameMap[hs+1].hazardStatusName;
}

inline char *getStageName(UINT status)
{
	IsTrue((status <= PS_WB), ("getStateName():Invalid status %d", status));
	return CPerfModelSL1::stageNames[status];
}

inline void setLastRetiredId(INT threadId, UINT perfId)
{
	CPerfModelSL1::_lastRetiredId[threadId] = perfId;
}

inline UINT getLastRetiredId(INT threadId)
{
	return CPerfModelSL1::_lastRetiredId[threadId];
}

inline STRING getThreadName(INT threadId)
{
	return CPerfModelSL1::threadNames[threadId];
}

inline UINT getInstrLatency(UINT instrGrp, UINT _instrId)
{
	IsTrue( (instrGrp<EIG_misc),("Invalid instruction group id:%d\n", instrGrp));
	return CPerfModelSL1::SL1InstrLatencyTable[instrGrp].latency;
}

inline bool isLoadStore(UINT instrGrp)
{
	return ((instrGrp == EIG_ls) || (instrGrp == EIG_ls16)); 
}

inline bool isMacA(UINT instrId)
{
	switch(instrId) {
		case c3saadda_id:
		case c3saddha_id:
		case c3maca_id:
		case c3samulha_id:
		case c3sasubha_id:
		//case c3saaddha_id:
		//case c3mulha_id:
		case c3macna_id:
		case c3mulaa_id:
		case c3dmaca_id:
		case c3dmacna_id:
	    case c3dmulaa_id:
	    case c3dmulana_id:
	    case c3sasuba_id:
	    	return true;
		default:
			return false;
	}
}

inline bool isFixed(int instrId)
{
	switch(instrId){
		case c3fft_id:
		case c3viterbi_id:
		case c3traceback_id:
			return true;
		default:
			return false;
	}
}

inline bool checkMemBank(ADDR ar1,ADDR ar2)
{
	ADDR bank1,bank2;
	if(ar1>=DSP_DARAM_BASE && ar1<DSP_DARAM_BASE+(DSP_DARAM_SPACE>>1))
		if(ar2>=DSP_DARAM_BASE && ar2<DSP_DARAM_BASE+(DSP_DARAM_SPACE>>1)) //64k daram
		{
			bank1 = GET_MEM_BANK(ar1);
			bank2 = GET_MEM_BANK(ar2);
			if(bank1==bank2)
				return true;			
		}
    return false;
}
inline bool isLoad(UINT instrGrp, UINT instrId)
{
	if ((instrGrp != EIG_ls) && (instrGrp != EIG_ls16))
		return false;
	switch(instrId) {
	case ldb_id:
	case ldh_id:
	case ldub_id:
	case lduh_id:
	case ldw_id:
	case c3ld_id:
	case ldw16_id:
	case ldub16rs_id:
	case lduh16rs_id:
		return true;
	}
	return false;
}

inline bool isBranch(UINT instrGrp)
{
	return ((instrGrp == EIG_br) || (instrGrp == EIG_br16)); 
}

inline bool isJump(UINT instrGrp)
{
	return ((instrGrp == EIG_j) || (instrGrp == EIG_j16)); 
}

inline bool isMAC(UINT instrGrp)	// 2 cycles
{
	// All MAC related instructions will take 2 cyles
	return (EIG_c3_mac == instrGrp);
}

inline bool isNop(UINT instrId)
{
	return ( (instrId == nop16_id) || (instrId == nop_id) );
}

inline bool isNop16(UINT instrId)
{
	return (instrId == nop16_id);
}

typedef Singleton<CPerfModelSL1> PerfModelSL1;

#endif /*PERFMODEL_SL1_H_*/
