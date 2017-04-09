/*
 *  File: cdecoder.h
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 */
#ifndef COREDECODER_H_
#define COREDECODER_H_

#include "defs.h"
#include "cregdefs.h"
#include "cinstr.h"
#include "cinstrid.h"
#include "simdefs.h"

//#define _DEBUG_DECODE 1

#if _DEBUG_DECODE
#define DEBUG_DECODE printf
#else
#define DEBUG_DECODE(x,...) ((void)1)
#endif

#define DECODER_UNDEF_INSTR 0
#define DECODER_CORE_INSTR 0x1
#define DECODER_C1_INSTR 0x2
#define DECODER_C2_INSTR 0x4
#define DECODER_C3_INSTR 0x8

#define CORE_INSTR16_GR(op) ((op&0x20)>0)
#define CORE_TOP_GR(op) (op>(UBYTE)EGROUP_C4)
#define C2_INSTR_GR(op) (op>=(UBYTE)EGROUP_C2)
#define IS_CORE_INSTR16(gr) (gr==EGROUP_TOP_16||gr==EGROUP_G2)

template <class INSTRClass>
class CoreDecoder {
	
	private:
	UINT _mode;
	BOOL _assertInvalidInstr;
	
	public:
	CoreDecoder(BOOL b) { 
		_assertInvalidInstr = b;
		_mode = 0xffffffff; 
		//default accept all instructions
	} 	
	UINT32 decode (INSTRClass* instr, ADDR pc, WORD raw);
	UINT mode(void) { return _mode; }
	void setMode(UINT m) { _mode |= m; }
	void clearMode(UINT m) { _mode &= (~m); }

	BOOL assertInvalidInstr(void) { return _assertInvalidInstr;}
	void assertInvalidInstr(BOOL b) { _assertInvalidInstr = b;}

	UBYTE getGroupID(UBYTE op) {
		if(CORE_TOP_GR(op)) {
			if(C2_INSTR_GR(op)) {
				return EGROUP_C2;
			}
			else if(CORE_INSTR16_GR(op)) {
				return EGROUP_TOP_16;
			}
			return EGROUP_TOP;
		}
		return op;
	}
	
	UINT32 decodeInvalid (INSTRClass* instr, UINT meta);
	
//Instruction decoding function

	UINT32 decodeCore01_01 (INSTRClass* instr, UINT meta); //, add.i, setlt.i, setltu.i
	UINT32 decodeCore01_02 (INSTRClass* instr, UINT meta); //, stb, sth, stw
	UINT32 decodeCore01_03 (INSTRClass* instr, UINT meta); //, ldb, ldh, ldub, lduh, ldw
	UINT32 decodeCore01_04 (INSTRClass* instr, UINT meta); //, jp, jp.lnk
	UINT32 decodeCore01_05 (INSTRClass* instr, UINT meta); //, mvup.i
	UINT32 decodeCore01_06 (INSTRClass* instr, UINT meta); //, and.i, nor.i, or.i, xor.i
	UINT32 decodeCore01_07 (INSTRClass* instr, UINT meta); //, br.eq, br.ne
	UINT32 decodeCore02_01 (INSTRClass* instr, UINT meta); //, add16, and16, mv16, or16, shll16, sub16, xor16
	UINT32 decodeCore02_02 (INSTRClass* instr, UINT meta); //, mv16.i
	UINT32 decodeCore02_03 (INSTRClass* instr, UINT meta); //, br16.eqz, br16.nez
	UINT32 decodeCore02_05 (INSTRClass* instr, UINT meta); //, add16.i, and16.i, or16.i, shra16.i, shll16.i, shrl16.i, sub16.i, xor16.i
	UINT32 decodeCore02_06 (INSTRClass* instr, UINT meta); //, mvfc16
	UINT32 decodeCore02_07 (INSTRClass* instr, UINT meta); //, mvtc16
	UINT32 decodeCore02_08 (INSTRClass* instr, UINT meta); //, ldw16
	UINT32 decodeCore02_09 (INSTRClass* instr, UINT meta); //, stw16
	UINT32 decodeCore02_10 (INSTRClass* instr, UINT meta); //, pop16
	UINT32 decodeCore02_11 (INSTRClass* instr, UINT meta); //, push16
	UINT32 decodeCore03_01 (INSTRClass* instr, UINT meta); //, nop16, jr16, jr16.lnk
	UINT32 decodeCore03_02 (INSTRClass* instr, UINT meta); //, abs16, inv16, ldub16.rs, lduh16.rs
	UINT32 decodeCore03_03 (INSTRClass* instr, UINT meta); //, add16.sp
	UINT32 decodeCore03_04 (INSTRClass* instr, UINT meta); //, rete16, ret16
	UINT32 decodeCore04_01 (INSTRClass* instr, UINT meta); //, nop
	UINT32 decodeCore04_02 (INSTRClass* instr, UINT meta); //, add, and, nor, or, setlt, setltu, shll, shra, shrl, sub, xor
	UINT32 decodeCore04_03 (INSTRClass* instr, UINT meta); //, depb, extrb, extrbu
	UINT32 decodeCore04_04 (INSTRClass* instr, UINT meta); //, mc.r.eq, mc.r.ne, mc.r.gt, mc.r.ge, mc.r.lt, mc.r.le, mc.zc.eq, mc.zc.ne, mc.zc.gt, mc.zc.ge, mc.zc.lt, mc.zc.le, mc.z.eq, mc.z.ne, mc.z.gt, mc.z.ge, mc.z.lt, mc.z.le, mc.zn.eq, mc.zn.ne, mc.zn.gt, mc.zn.ge, mc.zn.lt, mc.zn.le
	UINT32 decodeCore04_05 (INSTRClass* instr, UINT meta); //, mc.abs
	UINT32 decodeCore04_06 (INSTRClass* instr, UINT meta); //, mvfc
	UINT32 decodeCore04_08 (INSTRClass* instr, UINT meta); //, mvtc
	UINT32 decodeCore04_10 (INSTRClass* instr, UINT meta); //, shll.i, shra.i, shrl.i
	UINT32 decodeCore05_01 (INSTRClass* instr, UINT meta); //, br.gez, br.gtz, br.lez, br.ltz, dcflush
	UINT32 decodeCore05_02 (INSTRClass* instr, UINT meta); //, ret, rete
	UINT32 decodeCore05_03 (INSTRClass* instr, UINT meta); //, syscall
	UINT32 decodeCore05_04 (INSTRClass* instr, UINT meta); //, loop
	UINT32 decodeCore05_05 (INSTRClass* instr, UINT meta); //, jr, jr.lnk
	UINT32 decodeCore05_06 (INSTRClass* instr, UINT meta); //mvtlc.i
	UINT32 decodeUndefined (INSTRClass* instr, UINT meta); //, G0, G1, G2, C1, C3, C4, break
	
};

template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decode(INSTRClass* instr,  ADDR pc, WORD raw) {
	return 0;
}

//, add.i, setlt.i, setltu.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_01 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed imm : 16;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	simm = rb.bits.imm;
	instr->imm16(simm);
	return INT32_BYTE;	
}

//, stb, sth, stw
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_02 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed offset : 16;
			unsigned rs1 : 5;
			unsigned rs0 : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rs0(rb.bits.rs0);
	instr->rs1(rb.bits.rs1);
	simm = rb.bits.offset;
	instr->offset(simm);
	return INT32_BYTE;	
}

//, ldb, ldh, ldub, lduh, ldw
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_03 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed offset : 16;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	simm = rb.bits.offset;
	instr->offset(simm);
	return INT32_BYTE;
}

//, jp, jp.lnk
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_04 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned instr_index : 26;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.instr_index;
	instr->instr_index(uimm);
	return INT32_BYTE;	
}

//, mvup.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_05 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed imm16 : 16;
			unsigned na : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	simm = rb.bits.imm16;
	instr->imm16(simm);
	return INT32_BYTE;	
}

//, and.i, nor.i, or.i, xor.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_06 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned imm16 : 16;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.imm16;
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	instr->imm16(uimm);
	return INT32_BYTE;	
}

//, br.eq, br.ne
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore01_07 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed offset : 16;
			unsigned rs1 : 5;
			unsigned rs0 : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rs0(rb.bits.rs0);
	instr->rs1(rb.bits.rs1);
	simm = rb.bits.offset;
	instr->offset(simm);
	return INT32_BYTE;	
}

//, add16, and16, mv16, or16, shll16, sub16, xor16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_01 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rs1(rb.bits.rs1);
	instr->rd_rs(rb.bits.rd); //rd will be read as well
	return INT16_BYTE;	
}

//, mv16.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_02 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned imm5 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd); 
	uimm = rb.bits.imm5;
	instr->imm5(rb.bits.imm5);	
	return INT16_BYTE;	
}

//, br16.eqz, br16.nez
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_03 (INSTRClass* instr, UINT meta) {
	 union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned offset : 5;
			unsigned rs0 : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rs0(rb.bits.rs0);
	instr->offset(rb.bits.offset);
	return INT16_BYTE;	
}

//, add16.i, and16.i, or16.i, shra16.i, shll16.i, shrl16.i, sub16.i, xor16.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_05 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned imm5 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.imm5;
	instr->rd_rs(rb.bits.rd); //rd will be read as well
	instr->imm5(uimm);	
	return INT16_BYTE;	
}

//, mvfc16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_06 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned cs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->cs1(rb.bits.cs1);	
	return INT16_BYTE;	
}

//, mvtc16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_07 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned rs1 : 5;
			unsigned cd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->cd(rb.bits.cd); 
	instr->rs1(rb.bits.rs1);
	return INT16_BYTE;	
}

//, ldw16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_08 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd); 
	instr->rs1(rb.bits.rs1);
	return INT16_BYTE;	
}

//, stw16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_09 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned rs1 : 5;
			unsigned rs0 : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rs0(rb.bits.rs0); 
	instr->rs1(rb.bits.rs1);	
	return INT16_BYTE;	
}

//, pop16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_10 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned uimm : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.uimm;
	instr->rd(rb.bits.rd);
	instr->rs1(INDEX_REG_SP);	
	instr->imm5(uimm);	
	return INT16_BYTE;	
}

//, push16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore02_11 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned uimm : 5;
			unsigned rs0 : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	instr->rs1(INDEX_REG_SP);	
    instr->rs0(rb.bits.rs0);
	uimm = rb.bits.uimm;
	instr->imm5(rb.bits.uimm);	
	return INT16_BYTE;	
}

//, nop16, jr16, jr16.lnk
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore03_01 (INSTRClass* instr, UINT meta) {
	switch(instr->id()) {
		case jr16_id: 
			instr->cs1(ECR_JA);
			break;
		case jr16lnk_id: 
			instr->cs1(ECR_JA);
			instr->cd(ECR_RA);
			break;
		default:
			break;
	}		
	return INT16_BYTE;	
}

//, abs16, inv16, ldub16.rs, lduh16.rs
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore03_02 (INSTRClass* instr, UINT meta) {
    union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned op : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rd_rs(rb.bits.rd); 
	return INT16_BYTE;		
}

//, add16.sp
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore03_03 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned na : 16;
			unsigned op : 5;
			signed imm5 : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	simm = rb.bits.imm5;
	instr->imm5(simm); //rd will be read as well
	instr->rd_rs(INDEX_REG_SP);
	return INT16_BYTE;	
}

//, rete16, ret16
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore03_04 (INSTRClass* instr, UINT meta) {
	switch(instr->id()) {
		case ret16_id:
			instr->cs1(ECR_RA);
			break;
		default:
			break;
	}		
	return INT16_BYTE;	
}

//, nop
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_01 (INSTRClass* instr, UINT meta) {
	return INT32_BYTE;
}

//, add, and, nor, or, setlt, setltu, shll, shra, shrl, sub, xor
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_02 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned na : 5;
			unsigned rs2 : 5;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	instr->rs2(rb.bits.rs2);
//	printf("Entering CoreDecoder::decodeCore04_02\n");
	return INT32_BYTE;	
}

//, depb, extrb, extrbu
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_03 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned width : 5;
			unsigned pos : 5;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	instr->pos(rb.bits.pos);
	instr->width(rb.bits.width);
	return INT32_BYTE;		
}

//, mc.r.eq, mc.r.ne, mc.r.gt, mc.r.ge, mc.r.lt, mc.r.le, mc.zc.eq, mc.zc.ne, mc.zc.gt, mc.zc.ge, mc.zc.lt, mc.zc.le, mc.z.eq, mc.z.ne, mc.z.gt, mc.z.ge, mc.z.lt, mc.z.le, mc.zn.eq, mc.zn.ne, mc.zn.gt, mc.zn.ge, mc.zn.lt, mc.zn.le
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_04 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned unsigned_flag : 1;
			unsigned na : 4;
			unsigned rs2 : 5;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->unsigned_flag(rb.bits.unsigned_flag);
	instr->rs2(rb.bits.rs2);
	instr->rs1(rb.bits.rs1);
	instr->rd(rb.bits.rd);
	return INT32_BYTE;	
}

//, mc.abs
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_05 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned na : 10;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rs1(rb.bits.rs1);
	instr->rd(rb.bits.rd);
	return INT32_BYTE;	
}

//, mvfc
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_06 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned na : 10;
			unsigned cs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->cs1(rb.bits.cs1);
	instr->rd(rb.bits.rd);
	return INT32_BYTE;	
}

//, mvtc
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_08 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned na : 10;
			unsigned rs1 : 5;
			unsigned cd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	instr->rs1(rb.bits.rs1);
	instr->cd(rb.bits.cd);
	return INT32_BYTE;	
}

//, shll.i, shra.i, shrl.i
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore04_10 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned op : 6;
			unsigned na : 5;
			unsigned imm5 : 5;
			unsigned rs1 : 5;
			unsigned rd : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	instr->rd(rb.bits.rd);
	instr->rs1(rb.bits.rs1);
	uimm = rb.bits.imm5;
	instr->imm16(uimm);
	return INT32_BYTE;	
}

//, br.gez, br.gtz, br.lez, br.ltz, dcflush
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_01 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			signed offset : 16;
			unsigned rs1 : 5;
			unsigned op : 5;
			unsigned top :6;
		} bits;
	}rb;
	INT simm;
	rb.rawbits = instr->rawbits();
	instr->rs1(rb.bits.rs1);
	simm = rb.bits.offset;
	instr->offset(simm);
	return INT32_BYTE;	
}

//, ret, rete
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_02 (INSTRClass* instr, UINT meta) {
	switch(instr->id()) {
		case ret_id:
			instr->cs1(ECR_RA);
			break;
		default:
			break;
	}
	return INT32_BYTE;	
}

//, syscall
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_03 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned sysc_id : 20;
			unsigned na : 1;
			unsigned op : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.sysc_id;
	instr->sysc_id(uimm);
	return INT32_BYTE;		
}

//, loop
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_04 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned instr_offset : 16;
			unsigned na : 1;
			unsigned loop_index : 4;
			unsigned lv2 : 5;
			unsigned top :6;
		} bits;
	}rb;
	UINT uimm;
	rb.rawbits = instr->rawbits();
	uimm = rb.bits.instr_offset;
	instr->instr_offset(uimm);
	instr->loop_index(rb.bits.loop_index);
	return INT32_BYTE;
}

//, jr, jr.lnk
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_05 (INSTRClass* instr, UINT meta) {
	switch(instr->id()) {
		case jr_id: 
			instr->cs1(ECR_JA);
			break;
		case jrlnk_id: 
			instr->cs1(ECR_JA);
			instr->cd(ECR_RA);
			break;
		default:
			break;
	}
	return INT32_BYTE;	
}

template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeCore05_06 (INSTRClass* instr, UINT meta) {
	union {
		WORD rawbits;
		struct {
			unsigned opcode : 6;
			unsigned uimm10 : 10;
			unsigned na : 5;
			unsigned cd : 5;
			unsigned top :6;
		} bits;
	}rb;
	rb.rawbits = instr->rawbits();
	UINT uimm = rb.bits.uimm10;
	instr->imm10(uimm);
	instr->cd(rb.bits.cd);
	return INT32_BYTE;
}
//, G0, G1, G2, C1, C3, C4, break
template <class INSTRClass>
UINT32 CoreDecoder<INSTRClass>::decodeUndefined (INSTRClass* instr, UINT meta) {
	return 0;
}


#endif /*COREDECODER_H_*/
