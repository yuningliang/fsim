#ifndef STATUS_H_
#define STATUS_H_

#include "defs.h"
#include "simdefs.h"
#include "cregdefs.h"

#define MAX_EXT_META 512
#define MAX_DEST_REG 5
#define MAX_MEM_BUS 2
#define BR_MISS 0xffffffff
#define BR_HIT	0

#define CHECK_MEM_BREAK(addr, tag) status().checkMemBreak(addr, tag);
#define CHECK_INSTR_BREAK(addr, tag) \
	if(tag!=ET_NORMAL) { \
		if((tag&ET_INSTR_SKIP)>0) { \
			mmu().clearBreak(addr, ET_INSTR_SKIP); \
		} \
		else { \
			status().checkInstrBreak(tag); \
		} \
	}

#if _PROFILER_BUILD
#define LOG_EXTMETA(s, d) status().extMeta(s, d);
#define LOG_META(m) status().meta(m);
#define LOG_BR(taken, offset) status().br_log(taken, offset);
#define LOG_FIX(n) status().fixedfunc_log(n);
#define LOG_SRAM_WRITE(addr, data, busIndex, size) status().setSRamDataWrite(addr, data, busIndex, size);
#define LOG_SRAM_READ(addr, data, busIndex, size) status().setSRamDataRead(addr, data, busIndex, size);
#define LOG_CBUS_WRITE(addr, data, size) status().setCBusDataWrite(addr, data, size);
#define LOG_CBUS_READ(addr, data, size) status().setCBusDataRead(addr, data, size);
#define LOG_REG_WRITE(index, regbase, data) status().setWriteRegData(index, regbase, data, FALSE);
#define LOG_REG16_WRITE(index, regbase, data) status().setWriteRegData(index, regbase, data, TRUE);
#else
#define LOG_EXTMETA(s, d)
#define LOG_META(m)
#define LOG_BR(taken, offset)
#define LOG_FIX(m)
#define LOG_SRAM_WRITE(addr, data, busIndex, size)
#define LOG_SRAM_READ(addr, data, busIndex, size)
#define LOG_CBUS_WRITE(addr, data, size)
#define LOG_CBUS_READ(addr, data, size)
#define LOG_REG_WRITE(index, regbase, data)
#define LOG_REG16_WRITE(index, regbase, data)
#endif

enum EEXT_META {
	EMETA_INVAILD = -1,
	EMETA_NONE = 0,
	EMETA_OFFSET = 1,
};

enum EMEM_TYPE {
	EMEM_INVAILD = -1,
	EMEM_CBUS_READ = 0,
	EMEM_CBUS_WRITE = 1,
	EMEM_SRAM0_READ = 2,
	EMEM_SRAM0_WRITE = 3,	
	EMEM_SRAM1_READ = 4,
	EMEM_SRAM1_WRITE = 5,
};

struct MemData {
	ADDR addr;
	WORD data;
	INT size;
	EMEM_TYPE type;
};

struct RegData {
	UINT index;
	UINT indexBase;
	WORD data;
	BOOL enable16;
};	

template <class INSTRClass>
class ProcessStatus {
	private:
	//profiler logging
	BYTE _writeRegCnt;
	BYTE _memCnt;
	struct RegData _writeRegData[MAX_DEST_REG];
	struct MemData _memData[MAX_MEM_BUS];
	BYTE _thread_id;
	BOOL _br_taken;
	ADDR _br_offset;
	UINT _meta;
	INT _extMetaIndex;
	char _extMeta[MAX_EXT_META];
	INSTRClass* _instr;
	BOOL _activate;
	UINT _event;
	UINT _switch_thid;
	
	//Status logging
	ADDR _pre_pc;
	ADDR _post_pc;
	UINT _state;
	UINT _last_state;
	ADDR _breakAddr;
	
	//sp write logging
	WORD _spValue;
	BOOL _spWrite;

	public:
	ProcessStatus() { 
		_post_pc = 0;
		_switch_thid = 0;
		clear(0); 
		_state = ESTATUS_NORMAL;
		_last_state = ESTATUS_NORMAL;
		_activate = FALSE;
		_event = EVENT_NONE;
	}
	UINT state(void) { return _state; }
	void state(UINT16 s) { _state |= s; }
	UINT last_state(void) { return _last_state; }
	void last_state(UINT16 s) { _last_state = s; }
	
	UINT event(void) { return _event; }
	void event(UINT e) { 
		LOCK_STATUS
		_event |= e; 
		UNLOCK_STATUS
	}
	void startThread(UINT id) { 
		LOCK_STATUS
		_event |= EVENT_START_THREAD; 
		_switch_thid = id;
		UNLOCK_STATUS
	}		
	void switchThread(UINT id) { 
		LOCK_STATUS
		_event |= EVENT_SWITCH_THREAD; 
		_switch_thid = id;
		UNLOCK_STATUS
	}	
	void clearEvent(void) { 
		LOCK_STATUS
		_event = EVENT_NONE;
		UNLOCK_STATUS 
	}
	void clearEvent(UINT e) {
		LOCK_STATUS 
		_event &= (~e);
		UNLOCK_STATUS 
	}
	
	void setState(UINT s) { _state = s; }
	UINT meta(void) { return _meta; }
	void meta(UINT m) { _meta = m; }
	void extMeta(STRING str, UINT data) {
		_extMetaIndex += sprintf(_extMeta+_extMetaIndex,"%s=0x%x ", str, data);
	}		
	STRING extMeta(void) {
		return _extMeta;
	}	
	UINT breakAddr(void) { return _breakAddr; }
	void breakAddr(ADDR a) { _breakAddr = a; }		
	BYTE thread_id(void) { return _thread_id; }
	void thread_id(BYTE id) { _thread_id = id; }	
	
	void logExec(ADDR pre, ADDR post, INSTRClass* i) {
		pre_pc(pre);
		post_pc(post);	
		instr(i);
	}
	
	void checkInstrBreak(UBYTE tag) { 
		if(_activate==TRUE) {
			if((tag&ET_INSTR_SKIP)==0&&(tag&ET_INSTR)>0) {
				state(ESTATUS_INSTR_BREAK);
			}		
		}
	}
	void checkMemBreak(ADDR addr, UBYTE tag) { 
		if(_activate==TRUE) {
			if((tag&ET_RB)>0) {
				state(ESTATUS_MEM_RBREAK);
				_breakAddr = addr;
			}
			if((tag&ET_WB)>0) {
				state(ESTATUS_MEM_WBREAK);
				_breakAddr = addr;
			}
		}
	}	
	void resume(void) {
		_activate = TRUE;
	}
	void suspense(void) {
		_activate = FALSE;
	}	
	void enable(BYTE id) {
		clear(id);
		_activate = TRUE;
	}
	void disable() {
		_activate = FALSE;
	}	
	
	void clear(BYTE id) {
		_state = ESTATUS_NORMAL;
		_breakAddr = 0;
		_pre_pc = _post_pc;
		_post_pc = 0;
		_thread_id = id;
		_instr = NULL;
		_event = EVENT_NONE;
#if _PROFILER_BUILD	
		_spValue = 0;
		_spWrite = FALSE;
		_writeRegCnt = -1;
		_memCnt = -1;
		_br_taken = FALSE;
		_br_offset = 0;
		_meta = 0;
		_extMetaIndex = 0;
		_extMeta[0] = '\0';	
#endif		
	}
	
	INT memCnt(void) { return (_memCnt+1); }
	INT writeRegCnt(void) { return (_writeRegCnt+1); }
	
	struct RegData* getWriteRegData(INT index) {
		SimFatal((index>=0&&index<MAX_DEST_REG), ("Status: invalid getRegData index (%d).", index));
		return &_writeRegData[index];
	}

	struct MemData* getMemData(INT index) {
		SimFatal((index>=0&&index<MAX_MEM_BUS), ("Status: invalid getMemData index (%d).", index));
		return &_memData[index];		
	}
	
	void setWriteRegData(UINT index, UINT indexBase, WORD data, BOOL enable16) {
		if(_activate==TRUE) {
			_writeRegCnt++;
			SimFatal((_writeRegCnt<MAX_DEST_REG), ("Status: _writeRegCnt>=MAX_DEST_REG (%d), %d.", _writeRegCnt, indexBase));
			_writeRegData[_writeRegCnt].index = index;
			_writeRegData[_writeRegCnt].indexBase = indexBase;
			_writeRegData[_writeRegCnt].data = data;
			_writeRegData[_writeRegCnt].enable16 = enable16;
			if(index==INDEX_REG_SP&&indexBase==EBASE_GPR) {
				_spValue = data;
				_spWrite = TRUE;
			}
		}
	}
	void setCBusDataRead(ADDR addr, WORD data, INT size) {
		if(_activate==TRUE) {
			_memCnt++;
			SimFatal((_memCnt<MAX_MEM_BUS), ("Status: _memCnt>=MAX_MEM_BUS (%d).", _memCnt));
			_memData[_memCnt].addr = addr;
			_memData[_memCnt].data = data;
			_memData[_memCnt].size = size;
			_memData[_memCnt].type = EMEM_CBUS_READ;
		}
	}	
	void setCBusDataWrite(ADDR addr, WORD data, INT size) {
		if(_activate==TRUE) {
			_memCnt++;
			SimFatal((_memCnt<MAX_MEM_BUS), ("Status: _memCnt>=MAX_MEM_BUS (%d).", _memCnt));
			_memData[_memCnt].addr = addr;
			_memData[_memCnt].data = data;
			_memData[_memCnt].size = size;
			_memData[_memCnt].type = EMEM_CBUS_WRITE;
		}
	}	
	void setSRamDataRead(ADDR addr, WORD data, INT busIndex, INT size) {
		if(_activate==TRUE) {
			_memCnt++;
			SimFatal((_memCnt<MAX_MEM_BUS), ("Status: _memCnt>=MAX_MEM_BUS (%d).", _memCnt));
			SimFatal((busIndex==1||busIndex==2), ("Status: Invaild sram bus index (%d).", busIndex));
			_memData[_memCnt].addr = addr;
			_memData[_memCnt].data = data;
			_memData[_memCnt].size = size;
			_memData[_memCnt].type = (busIndex==1)?EMEM_SRAM0_READ:EMEM_SRAM1_READ;
		}
	}		
	void setSRamDataWrite(ADDR addr, WORD data, INT busIndex, INT size) {
		if(_activate==TRUE) {
			_memCnt++;
			SimFatal((_memCnt<MAX_MEM_BUS), ("Status: _memCnt>=MAX_MEM_BUS (%d).", _memCnt));
			SimFatal((busIndex==1||busIndex==2), ("Status: Invaild sram bus index (%d).", busIndex));
			_memData[_memCnt].addr = addr;
			_memData[_memCnt].data = data;
			_memData[_memCnt].size = size;
			_memData[_memCnt].type = (busIndex==1)?EMEM_SRAM0_WRITE:EMEM_SRAM1_WRITE;
		}
	}		

	void br_log(BOOL taken, ADDR offset) {
		_br_taken = taken;
		_br_offset = offset;
		meta(taken);
		/*
		if(taken) {
			if(offset>=0) 
				meta(BR_MISS);
			else
				meta(BR_HIT);
		}
		else {
			if(offset>=0) 
				meta(BR_HIT);
			else
				meta(BR_MISS);			
		}	
		*/	
	}
	
	void fixedfunc_log(UINT8 number)
	{
		meta(number);
	}	
	
	BOOL br_taken(void) { return _br_taken; }
	ADDR br_offset(void) { return _br_offset; }
	ADDR pre_pc(void) { return _pre_pc; }
	void pre_pc(ADDR addr) { _pre_pc = addr; }
	ADDR post_pc(void) { return _post_pc; }
	ADDR* post_pc_ptr(void) { 
		return &_post_pc; 
	}
	void post_pc(ADDR addr) { _post_pc = addr; }
	UINT switch_thid(void) { return _switch_thid; }
	UINT start_thid(void) { return _switch_thid; }
	INSTRClass* instr(void) { return _instr; }
	void instr(INSTRClass* in) { _instr = in; }
	WORD spValue(void) { return _spValue; }
	BOOL spWrite(void) { return	_spWrite; }	
};

#endif /*STATUS_H_*/
