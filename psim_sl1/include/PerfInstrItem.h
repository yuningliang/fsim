/* /////////////////////////////// "PerfInstrItem.h" /////////////////////////////////
 *  File: PerfInstrItem.h
 *  Descriptions:  Define instrution item for performance simulation
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
#ifndef PERFINSTRITEM_H_
#define PERFINSTRITEM_H_

#include "defs.h"
#include <list>
#include <deque>

typedef std::list<UINT16> RegList;
typedef std::list<UINT16>::iterator RegListIter;
//typedef std::deque<UINT16> RegList;
//typedef std::deque<UINT16>::iterator RegListIter;

class PerfModel_SL1;

class InstrItem;

typedef std::list<InstrItem*> InstrList;
typedef std::list<InstrItem*>::iterator InstrListIter;

class InstrItem {
public:
	ADDR _pc; 
	UINT _threadId;	// THREAD_ID_CORE/THREAD_ID_BB 
	WORD _raw;
	UINT _instrId; 
	UINT _instrGrp;
	BRANCH_META  _brMeta;  
	ADDR _dataAddr;
	ADDR _ar1;
	ADDR _ar2;
	UINT _perfId; 
	UINT _fetchCyc;
	ADDR _InstrBrPc;

	PIPELINE_STAGE _instrStatus;
	HAZARD_STATUS _hazardStatus;
	UINT  _isWordEnd;	// For detecting 16bit instr pair, 1st 16bit:false; 2nd 16bit:true.

	UINT _latency;      // latency penalty: branch, jump/return
	UINT _readPorts;	// read ports used
	UINT _writePorts;	// write ports used
	RegList _desRegs;
	RegList _srcRegs;
	UINT _execUnit;	// allocated execution unit
//	InstrVector _depInstrs;	// the instructions whose target registers will be read by current instruction
	
	char _disasmBuf[MAX_DISASM_BUF_LEN];

public:
	inline InstrItem(UINT threadId_, ADDR pc_, WORD raw_, BRANCH_META meta_, UINT perfId_, ADDR dataAddr_,ADDR ar1_,ADDR ar2_){
		init(threadId_, pc_, raw_, meta_, perfId_, dataAddr_,ar1_,ar2_);
	}
	inline InstrItem(){_perfId=(unsigned)-1;}
//	~InstrItem();
	
	inline void init(UINT threadId_, ADDR pc_, WORD raw_, BRANCH_META meta_, UINT perfId_, ADDR dataAddr_,ADDR ar1_,ADDR ar2_){
		_pc = pc_;
		_threadId = threadId_;
		_raw = raw_;
		_brMeta = meta_;
		_perfId = perfId_ * SL1_MAX_THREADS + threadId_;
		_isWordEnd = true;
		_latency = 0;
		_execUnit = EXEC_UNIT_UNDEFINED;
		_dataAddr = dataAddr_;
		_ar1 = ar1_;
		_ar2 = ar2_;
	}
	
	PIPELINE_STAGE instrStage(void) { return _instrStatus; }
	void instrStage(PIPELINE_STAGE instrStatus_) { _instrStatus = instrStatus_; }
	UINT perfId(void) { return _perfId; }
	void perfId(UINT perfId_) { _perfId = perfId_; }
	RegList *getSrcRegs(void) { return &_srcRegs; }
	RegList *getDesRegs(void) { return &_desRegs; }
	UINT threadId(void) { return _threadId; }
	UINT instrGrp(void) { return _instrGrp; }
	UINT instrId(void)  { return _instrId;  }
	void isWordEnd(bool we) { _isWordEnd = we; }
	bool isWordEnd(void) { return _isWordEnd; }
	UINT latency(void) { return _latency; }
	void latency(UINT cyc) { _latency = cyc; }
	void decLatency(void) { _latency--;}
	void execUnit(UINT eu_) {_execUnit = eu_;}
	UINT execUnit(void) {return _execUnit; }
	void setBrPc(INT offset) {_InstrBrPc = _pc+(offset<<OFFSET_SHIFT_BR_PSIM);}
	ADDR getBrPc(void) {return _InstrBrPc;}
    ADDR getDataAddr(void) { return _dataAddr;}
    ADDR getAr1(void) {return _ar1;}
    ADDR getAr2(void) {return _ar2;}
	ADDR pc(void) { return _pc; }
	BRANCH_META brMeta(void) { return _brMeta; }
	
	char* disasm(void) { return _disasmBuf; }
	
	void dump(void);
};

/*
 * This class is a wrapper to keep track # of 32-bit words because
 * of 16bit instructions. The list also has an upper limit on number of words
 * in the list.
 */
#define _INTSTR_CONTAINER_CLASS std::list<InstrItem*>
//class ISimpleList:public InstrList

class ISimpleList:public _INTSTR_CONTAINER_CLASS
{
private:
	int _wordCnt;
	int _maxWords;
	
public:
	ISimpleList():_INTSTR_CONTAINER_CLASS() { _wordCnt=_maxWords=0; }
	
	// return the number of words in the list instead of number of instructions
	inline int wordCnt(void) { return _wordCnt; }
	
	inline int getMaxWords(void) { return _maxWords; }
	inline void setMaxWords(int wordLimit_) { 
		IsTrue((wordLimit_>=0),("Invalid word number limit:%d\n", wordLimit_));
		_maxWords = wordLimit_; 
	} 
	inline bool isFull(void) { return (_wordCnt >= _maxWords); }
	
	void push_back(InstrItem* item_){
		_INTSTR_CONTAINER_CLASS::push_back(item_);
		if (item_->isWordEnd()) _wordCnt++;
	}
	
	void erase(iterator/*InstrListIter*/ pos_) {
		if ((*pos_)->isWordEnd())	_wordCnt--;
		_INTSTR_CONTAINER_CLASS::erase(pos_);
	}
	
	inline bool empty(void) { return _INTSTR_CONTAINER_CLASS::empty(); }
	inline int size(void) { return _INTSTR_CONTAINER_CLASS::size(); }
	inline iterator begin(void) { return _INTSTR_CONTAINER_CLASS::begin(); }
	inline iterator end(void) { return _INTSTR_CONTAINER_CLASS::end(); }
	inline InstrItem* front() { return _INTSTR_CONTAINER_CLASS::front(); }

	inline void pop_front() {
		if (_INTSTR_CONTAINER_CLASS::empty())
			return;
		if (_INTSTR_CONTAINER_CLASS::front()->isWordEnd()) _wordCnt--;
		_INTSTR_CONTAINER_CLASS::pop_front();
	}
};

#endif /*PERFINSTRITEM_H_*/
