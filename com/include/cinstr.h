#ifndef INSTR_H_
#define INSTR_H_

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include "defs.h"
#include <list>

#define CORE_ID(id) ((_id&0xffffff00)==0)
#define C2_ID(id) ((_id&0xffffff00)==0x200)
#define C3_ID(id) ((_id&0xffffff00)==0x300)

#ifndef _INCLUDE_REG_DETAIL
#define _INCLUDE_REG_DETAIL 1
#endif

#if _INCLUDE_REG_DETAIL
	#define REG_DETAIL(reg, type, base) \
		reg.status = type;		\
		reg.regbase = base;
#else
	#define REG_DETAIL(reg, type, base);
#endif

typedef std::list<UINT16> RegList;
typedef std::list<UINT16>::iterator RegListIter;

extern STRING Instr_Group_Name[];

enum EGROUP_ID {
	EGROUP_G0 = 0,
	EGROUP_G1 = 1,
	EGROUP_G2 = 2,
	EGROUP_C3 = 4,
	EGROUP_C4 = 5,
	EGROUP_C2 = 0x38,
	EGROUP_TOP = 0x10,
	EGROUP_TOP_16 = 0x20,
	EGROUP_INVALID = 0xff
};

enum EREG_STATUS {
	ERS_UNDEF = 0,
	ERS_READ = 0x1,
	ERS_WRITE = 0x2,
	ERS_RW = 0x3,
	ERS_NO_HAZARD = 0x4,
};

class CoreInstr {
	
	public:	
	struct Reg {
		EREG_STATUS status;
		UINT32 regbase;
		UINT32 index;
	};
	
	protected:
	//instruction info from instr table
	STRING _mn;
	UINT16 _id;
		
	//runtime info
	WORD _rawbits;
	WORD _meta;	
	UINT _instrsize;
	ADDR _pc;
	
	//bit field
	struct Reg _ctrl1;
	struct Reg _ctrl2;
	struct Reg _reg0;
	struct Reg _reg1;
	struct Reg _reg2;
	
	WORD _imm_b16;  //>=16bits, imm16, offset, instr_index
	WORD _imm_s16;  //<16bits, imm10, imm5, imm4
	UBYTE _modf15_11;  // pos
	UBYTE _modf10_6;  // width
	UBYTE _modf6_6; //unsigned_flag
	UBYTE _modf20_17; //loop_index
			
	public:	
	CoreInstr() {
		init();
	}
		
	void init() {
		_mn = NULL;
		_id = 0;
		_rawbits = 0;
		_pc = 0;
#if _INCLUDE_REG_DETAIL
		_ctrl1.status = ERS_UNDEF;
		_ctrl2.status = ERS_UNDEF;
		_reg0.status = ERS_UNDEF;
		_reg1.status = ERS_UNDEF;
		_reg2.status = ERS_UNDEF;
#endif
	}
	
	ADDR pc(void) { return _pc; }
	void pc(ADDR pc) { _pc = pc; }
	WORD rawbits(void) { return _rawbits; }
	STRING mn(void) { return _mn; }
	UINT16 id(void) { return _id; }
	WORD meta(void) { return _meta; }
	void meta(WORD m) { _meta = m; }
	WORD instrsize(void) { return _instrsize; }
	void instrsize(UINT s) { _instrsize = s; }	
	void getRegList(RegList* destList, RegList* srcList) {
	}
	
	BOOL isCore(void) { return CORE_ID(_id); }
	BOOL isC2(void) { return C2_ID(_id); }
	BOOL isC3(void) { return C3_ID(_id); }
	
	UINT32 rd(void) { return _reg0.index; }
	void rd(UINT32 r) {  //write only
		REG_DETAIL(_reg0, ERS_WRITE, EBASE_GPR)
		_reg0.index = r; 
	}
	void rd_rs(UINT32 r) {  //read and write (16bit instruction)
		REG_DETAIL(_reg0, ERS_RW, EBASE_GPR)
		_reg0.index = r; 
	}
		
	UINT32 cd(void) { return _ctrl1.index; }
	void cd(UINT32 r) { 
		REG_DETAIL(_ctrl1, ERS_WRITE, EBASE_CTRL)
		_ctrl1.index = r; 
	}
	//rs0
	UINT32 rs0(void) { return _reg0.index; }
	void rs0(UINT32 r) { 
		REG_DETAIL(_reg0, ERS_READ, EBASE_GPR)
		_reg0.index = r; 
	}
	//rs1
	UINT32 rs1(void) { return _reg1.index; }
	void rs1(UINT32 r) { 
		REG_DETAIL(_reg1, ERS_READ, EBASE_GPR)
		_reg1.index = r; 
	}
	
	UINT32 rs2(void) { return _reg2.index; }
	void rs2(UINT32 r)   { 
		REG_DETAIL(_reg2, ERS_READ, EBASE_GPR)
		_reg2.index = r; 
	}	
	UINT32 cs1(void) { return _ctrl1.index; }
	void cs1(UINT32 r) { 
		REG_DETAIL(_ctrl1, ERS_READ, EBASE_CTRL)
		_ctrl1.index = r; 
	}


	//_imm_b16
	WORD imm16(void) { return _imm_b16; }
	void imm16(WORD imm) { _imm_b16 = imm; }
	WORD offset(void) { return _imm_b16; }
	void offset(WORD off) { _imm_b16 = off; }
	WORD instr_index(void) { return _imm_b16; }
	void instr_index(WORD index) { _imm_b16 = index; }
	UINT sysc_id(void) { return _imm_b16; }
	void sysc_id(UINT id) { _imm_b16 = id; }
	UINT instr_offset(void) { return _imm_b16; }
	void instr_offset(UINT cnt) { _imm_b16 = cnt; }
		
	// _imm_s16
	WORD imm10(void) { return _imm_s16; }
	void imm10(WORD imm) { _imm_s16 = imm; }
	WORD imm4(void) { return _imm_s16; }
	void imm4(WORD imm) { _imm_s16 = imm; }
	WORD imm5(void) { return _imm_s16; }
	void imm5(WORD imm) { _imm_s16 = imm; }	
		
	//	_modf15_11
	UBYTE pos(void) { return _modf15_11; }
	void pos(UBYTE p) { _modf15_11 = p; }

	
	//_modf10_6
	UBYTE width(void) { return _modf10_6; }
	void width(UBYTE w) { _modf10_6 = w; }	

	//modf7_7
	UBYTE unsigned_flag(void) { return _modf6_6;}
	void unsigned_flag(UBYTE f){ _modf6_6 = f;}		

	//_modf20_17
	UBYTE loop_index(void) { return _modf20_17;}
	void loop_index(UBYTE i) { _modf20_17 = i;}					
};

#endif /*INSTRUCTION_H_*/


#endif /*INSTR_H_*/
