#ifndef EVENTHANLDER_H_
#define EVENTHANLDER_H_

#include "cmmu.h"
#ifndef _CONTENT_SWITCH_DEBUG
	#define _CONTENT_SWITCH_DEBUG 0
#endif

#if _CONTENT_SWITCH_DEBUG
#define CONTENT_SWITCH_DEBUG printf
#else
#define CONTENT_SWITCH_DEBUG(x,...) ((void)1)
#endif

#ifndef _SYSTEM_SIMULATION
	#define _SYSTEM_SIMULATION 0
#endif
#if _SYSTEM_SIMULATION
	#include "system.h"
	#include "simconfig.h"
	#define SYS_ENABLE_CLOCK(enable) system()->enableClock(enable);
	#define SYSTRAP(id, thid) system()->trap(thid, id);
	#define SYSTRAPEND(thid) system()->clearTrap(thid);
	#define SYSGETHANDLER(thid) system()->getHandlerAddr(thid);
	#define SYSGETPICPC(thid) (thid==0?SimConfig::PIC_CORE_SAVE_PC:SimConfig::PIC_DSP_SAVE_PC)
	#define SYSGETRESETPC(thid) system()->getStartPC(thid);
	#define SYS_ILLEGAL_INS System::TRAP_EXCEPT
	#define SYS_INVALID_ADDR System::TRAP_EXCEPT
	#define SYS_BREAK System::TRAP_BREAK
	#define SYS_SYSCALL System::TRAP_SYSCALL
	#define SYS_GET_STATUS(thid) (system()->getStatus(thid))
	#define SYS_GET_RESET(thid) (system()->getReset(thid))
	#define SYS_IS_NON_NORMAL (system()->getStatus()!=System::STATUS_NORMAL)
	#define DEVICE_TRIGGER \
		system()->clock(); \
		if(SYS_IS_NON_NORMAL) status().event(EVENT_INTERRUPT);
#else
	class System;
	#define SYS_ENABLE_CLOCK(enable) 
	#define SYSTRAP(id, thid) 
	#define SYSTRAPEND(thid) 
	#define SYSGETHANDLER(thid) pc;
	#define SYSGETPICPC(thid) 0
	#define SYSGETRESETPC(thid) 0;	
	#define SYS_ILLEGAL_INS 0
	#define SYS_INVALID_ADDR 0
	#define SYS_BREAK 0
	#define SYS_SYSCALL 0
	#define SYS_GET_STATUS(thid) 0
	#define SYS_GET_RESET(thid) 0
	#define SYS_IS_NON_NORMAL (FALSE)	
	#define DEVICE_TRIGGER
#endif

template<class MMUClass, class REGClass, class MACHINEClass>
class EventHandler {
	private:
	MMUClass& _mmu;
	System* _system;
	MACHINEClass* _machine;
	
	public:
	EventHandler(MMUClass& m, System* s, MACHINEClass* mach);
	EventHandler(MMUClass& m, MACHINEClass* mach);
	EventHandler(MMUClass& m);
	MMUClass& mmu (void) { return _mmu; }
	System* system (void) { return _system; }
	MACHINEClass* machine(void) { return _machine; }
	void system (System* s) { _system = s; }
	void contentSave(REGClass& reg, ADDR pc);
	ADDR contentRestore(REGClass& reg);
	ADDR interruptHandler(REGClass& reg, ADDR pc, UINT thid);
	void exceptionHandler(UINT flag, UINT thid);
	ADDR returnHandler(REGClass& reg, UINT thid);		
	void sysBlackDoor(UINT flag);
};

template<class MMUClass, class REGClass, class MACHINEClass>
EventHandler<MMUClass, REGClass, MACHINEClass>::EventHandler(MMUClass& m, System* s, MACHINEClass* mach) : _mmu(m) {
	_system = s;
	_machine = mach;
}

template<class MMUClass, class REGClass, class MACHINEClass>
EventHandler<MMUClass, REGClass, MACHINEClass>::EventHandler(MMUClass& m, MACHINEClass* mach) : _mmu(m) {
	_machine = mach;
};

template<class MMUClass, class REGClass, class MACHINEClass>
EventHandler<MMUClass, REGClass, MACHINEClass>::EventHandler(MMUClass& m) : _mmu(m) {};

template<class MMUClass, class REGClass, class MACHINEClass>
void EventHandler<MMUClass, REGClass, MACHINEClass>::contentSave(REGClass& reg, ADDR pc) {
	SimFatal((system()!=NULL), ("Machine error: system handler is NULL.\n"));
	ADDR picAddr = SYSGETPICPC(mmu().curthread()); //get the address of pic
	UINT j;
	mmu().setWord(picAddr, pc);
	picAddr += WORD_BYTE;
	CONTENT_SWITCH_DEBUG("ContentSave started: ");
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		mmu().setWord(picAddr, reg.loopExecCnt(j));
		CONTENT_SWITCH_DEBUG("%d ", reg.loopExecCnt(j));
		picAddr += WORD_BYTE;
	}
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		mmu().setWord(picAddr, reg.loopStartPC(j));
		picAddr += WORD_BYTE;
	}
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		mmu().setWord(picAddr, reg.loopEndPC(j));
		picAddr += WORD_BYTE;
	}	
	CONTENT_SWITCH_DEBUG(", cur_index=%d\n", reg.loopCurIndex());
	reg.clearFEReg();
}

template<class MMUClass, class REGClass, class MACHINEClass>
ADDR EventHandler<MMUClass, REGClass, MACHINEClass>::contentRestore(REGClass& reg) {
	SimFatal((system()!=NULL), ("Machine error: system handler is NULL.\n"));
	ADDR picAddr = SYSGETPICPC(mmu().curthread());//get the address of pic
	INT loopCurIndex = -1;
	ADDR nextPC = mmu().getWord(picAddr);
	UINT j;
	picAddr += WORD_BYTE;
	CONTENT_SWITCH_DEBUG("ContentRestore started: ");
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		INT cnt = mmu().getWord(picAddr);
		if(cnt>0) {
			loopCurIndex++;
		}
		picAddr += WORD_BYTE;
		reg.loopExecCnt(j, cnt);
		CONTENT_SWITCH_DEBUG("%d ", cnt);
	}	
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		reg.loopStartPC(j, mmu().getWord(picAddr));
		picAddr += WORD_BYTE;
	}	
	for(j = 0; j<REG_LOOP_CUR_SIZE; j++) {
		reg.loopEndPC(j, mmu().getWord(picAddr));
		picAddr += WORD_BYTE;
	}			
	reg.loopCurIndex(loopCurIndex);
	CONTENT_SWITCH_DEBUG(", cur_index=%d\n", loopCurIndex);
	return 	nextPC;
}

template<class MMUClass, class REGClass, class MACHINEClass>
ADDR EventHandler<MMUClass, REGClass, MACHINEClass>::returnHandler(REGClass& reg, UINT thid) {
	mmu().curthread(thid);
	ADDR pc = contentRestore(reg);
	SYSTRAPEND(thid)
	return pc;
}

template<class MMUClass, class REGClass, class MACHINEClass>
ADDR EventHandler<MMUClass, REGClass, MACHINEClass>::interruptHandler(REGClass& reg, ADDR pc, UINT thid) {
	ADDR ret_pc = pc;
//	Debug_Break("debug");
	if(SYS_GET_STATUS(thid)) {
		mmu().curthread(thid);
		contentSave(reg, pc);
		ret_pc = SYSGETHANDLER(thid);
	}
	
	/* check reset interrupt for all thread */
	if(machine()!=NULL) {
		for(INT i = 0; i<MAX_THREAD; i++) {
			INT signal = SYS_GET_RESET(i);
			if(signal>0) {
				// enable thread i
				ADDR startpc = SYSGETRESETPC(i);
				mmu().curthread(i);
				machine()->setPC(i, mmu().getWord(startpc));
				machine()->enablePC(i);
				mmu().curthread(thid);
			}
			else if(signal<0) {
				machine()->disablePC(i);
			}
		}
	}

	return ret_pc;
}

template<class MMUClass, class REGClass, class MACHINEClass>
void EventHandler<MMUClass, REGClass, MACHINEClass>::exceptionHandler(UINT flag, UINT thid) {
	if((flag&EVENT_ILLEGAL_INS)>0) {
		SYSTRAP(SYS_ILLEGAL_INS, thid)
	}
	if((flag&EVENT_INVALID_ADDR)>0) {
		SYSTRAP(SYS_INVALID_ADDR, thid)
	}
	if((flag&EVENT_BREAK)>0) {
		SYSTRAP(SYS_BREAK, thid)
	}
	if((flag&EVENT_SYSCALL)>0) {
		SYSTRAP(SYS_SYSCALL, thid)
	}			
//	printf("%d, %d\n", (flag&EVENT_BREAK), (flag&EVENT_SYSCALL));
}

template<class MMUClass, class REGClass, class MACHINEClass>
void EventHandler<MMUClass, REGClass, MACHINEClass>::sysBlackDoor(UINT flag) {
	if((flag&EVENT_SUSPENSE_SYS)>0){
		SYS_ENABLE_CLOCK(TRUE) 
	}
	else if((flag&EVENT_RESUME_SYS)>0) {
		SYS_ENABLE_CLOCK(FALSE) 
	}
}

#endif /*EVENTHANLDER_H_*/
