/* /////////////////////////////// "PerfRegDep_SL1.h" /////////////////////////////////
 *  File: PerfRegDep_SL1.h
 *  Descriptions:  Define register dependency checking methods for SL1.
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#ifndef PERFREGDEP_H_
#define PERFREGDEP_H_

typedef struct {
	INT request; 
	INT lock;
} RegItem;

typedef struct {
	ES_REG_STATUS status;
//	INT cycle;
	InstrItem *instrItem;	// Instruction defining this register
} RegStatus;

class RegSetInfo {
public:
	static INT SL1RegIndex2SetMap[SL1_ALL_REGS_SIZE];	// reg index: 0..n / type: GPR/ACC/AR/CTRL
	
public:
	int _readPorts[REGCLASS_MAX];
	int _maxReadPorts[REGCLASS_MAX];
	int _writePorts[REGCLASS_MAX];
	int _maxWritePorts[REGCLASS_MAX];
	int _readPortsReq[REGCLASS_MAX];
	int _writePortsReq[REGCLASS_MAX];
	
	RegItem _regDefine[SL1_ALL_REGS_SIZE];  // Define table at current cycle for instructions waiting for issue
	RegItem _regUse[SL1_ALL_REGS_SIZE];     // Use table at current cycle for instructions waiting for issue
	RegStatus _regStatus[SL1_ALL_REGS_SIZE];// Define table for issued instructions
	inline bool checkCondMovePairWAW(InstrItem *curInstrItem, InstrItem *prevInstrItem);

public:
	static void initTables(void);
	
	inline void updateRegSet(void);
	inline void updateFixedReg(int index);
	inline void updateRegPort(void);
	inline void clearPorts(void);
	inline void lockPorts(void);
	void lockReg(InstrItem *item);
	HAZARD_STATUS checkRegReadHazard(INT threadId, INT regIndex, INT perfId);
	HAZARD_STATUS checkRegWriteHazard(INT threadId, INT regIndex, INT perfId);
	HAZARD_STATUS checkRegHazard(InstrItem *item);
};

/*
 * Reset register use-define table and decrement one cylce for registers being used
 */
 
inline void RegSetInfo::updateFixedReg(int index)
{
	_regStatus[index].status = RS_READY;
}

inline void RegSetInfo::updateRegSet(void)
{
	INT j;
	for (j=0; j<SL1_ALL_REGS_SIZE; j++) {
		_regDefine[j].request = 0;
		_regDefine[j].lock   = 0;
		_regUse[j].request   = 0;
		_regUse[j].lock      = 0;
		if (RS_READY != _regStatus[j].status) {
//			if (_regStatus[j].cycle > 0) {
//				_regStatus[j].cycle -= 1;
//				if (0 == _regStatus[j].cycle) { 
//					_regStatus[j].status = RS_READY;
////					printf("instr %d pipeline stage %d\n", _regStatus[j].instrItem->perfId(), 
////			_regStatus[j].instrItem->instrStage());
//				}
//			}
			InstrItem *item = _regStatus[j].instrItem;
			if(item->instrId()!=c3fft_id && item->instrId()!=c3viterbi_id && item->instrId()!=c3traceback_id)
			{
			// Register bypass occurs in EX stage for most instructions
			// For MAC instructions, register bypass(mac) occurs in MEM stage
			// Below code are to be cleaned after MAC dependency confirmed!!!!!!!!!!!!!!
				if ((item->instrGrp()!=EIG_c3_mac) && (item->instrStage() == PS_MEM)) {
				// Because register status update occurs before instruction dispatch,
				// the stage has to be MEM for register bypassing of EX stage
						_regStatus[j].status = RS_READY;
//					printf("instr %d: EX state-register status changed from %d to ready\n",
//						item->perfId(), j, _regStatus[j].status);
				}
				else
				if ((item->instrGrp() == EIG_c3_mac) && (item->instrStage() == PS_WB)) {
						_regStatus[j].status = RS_READY;
//						printf("instr %d: MEM stage-MAC status changed from %d to ready\n",
//							item->perfId(), j, _regStatus[j].status);
				} 
				else 
				if (item->instrStage() == PS_WB) {
						_regStatus[j].status = RS_READY;
//						printf("instr %d: WB stage-register status changed from %d to ready\n",
//						item->perfId(), j, _regStatus[j].status);
				}	
			
			}
		}
	}
}

inline void RegSetInfo::updateRegPort(void)
{
	int i;
	for (i=0; i<REGCLASS_MAX; i++) {
		_readPorts[i] = 0;
		_writePorts[i] = 0;
	}
}

inline void RegSetInfo::clearPorts(void)
{
	_readPortsReq[REGCLASS_GPR] = 0;
	_writePortsReq[REGCLASS_GPR] = 0;
}

inline void RegSetInfo::lockPorts(void)
{
	_readPorts[REGCLASS_GPR] += _readPortsReq[REGCLASS_GPR];
	_writePorts[REGCLASS_GPR] += _writePortsReq[REGCLASS_GPR];
}

inline ES_REG_STATUS getRegAccessType(INT instrGrp, INT instrId)
{
	switch (instrGrp) {
		case EIG_ls:
			return RS_MEM_WB;
//		case EIG_alu:
//			switch (instrId) {
//				case mvfc_id:
//				case mvfc16_id:
//				return RS_MEM_WB;
//			}
	}
	return RS_NORMAL;
}

inline INT getRegType(INT regIdx)
{
	IsTrue(((regIdx>=0)&&(regIdx<SL1_ALL_REGS_SIZE)), ("Invalid register index:%d\n", regIdx));
	return RegSetInfo::SL1RegIndex2SetMap[regIdx];
}

#endif /*PERFREGDEP_H_*/
