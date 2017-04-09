/*
 *  File: cdisasm.h
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 */
#ifndef COREDISASM_H_
#define COREDISASM_H_

#include "defs.h"
#include "cregdefs.h"
#include "messg.h"
#include "creg.h"
#include "symtable.h"

#define DIASM_BUF_SIZE 512

#ifndef _LOG_LOOP
#define _LOG_LOOP 0
#endif 

#if _LOG_LOOP
#define RET_STRBUF(pc) strbuf(pc);
#else
#define RET_STRBUF(pc) strbuf();
#endif

template <class MMUClass, class REGClass, class INSTRClass>
class CoreDisasm {
	private:
	MMUClass& _mmu;
	REGClass& _reg;
	SymTable* _symtable;
	UINT _thread_id;
	
	protected:	
	char _strbuf[DIASM_BUF_SIZE];
	
	protected:
	MMUClass& mmu(void) { return _mmu; }
	REGClass& reg(void) { return _reg; }
	STRING getSymName(ADDR addr) {
		return (_symtable==NULL?NULL:symtable()->getName(addr));
	}
	pair<STRING, INT> getSymOffset(ADDR addr) {
		pair<STRING, INT> ret(NULL, 0);
		if(_symtable!=NULL) {
			ret = symtable()->getFuncOffset(addr);
		}
		return ret;
	}
		
	public:
	SymTable* symtable(void) { return _symtable; }
	STRING strbuf(ADDR pc) {
		INT loopCurIndex = reg().loopCurIndex();
		if(loopCurIndex>=0) {
			if(pc==reg().loopStartPC(loopCurIndex)){
				char tmp[DIASM_BUF_SIZE];
			    sprintf(tmp,"%s\nLoop %d Start @0x%08x (count = %d, End pc = 0x%08x)\n", 
			    strbuf(),
			    loopCurIndex,
			    reg().loopStartPC(loopCurIndex),
			    reg().loopExecCnt(loopCurIndex),
			    reg().loopEndPC(loopCurIndex));
			    strcpy(_strbuf, tmp);
			}
		}
		return _strbuf;
	}
	STRING strbuf(void) {
		return _strbuf;
	}
	CoreDisasm(MMUClass& mmu, REGClass& reg, UINT th_id);
	void symtable(SymTable* s) { _symtable = s; }
	
	
//Instruction disassembler function

	STRING disasmCore01_01 (ADDR pc, INSTRClass* instr, UINT meta); //, add.i, setlt.i, setltu.i
	STRING disasmCore01_02 (ADDR pc, INSTRClass* instr, UINT meta); //, stb, sth, stw
	STRING disasmCore01_03 (ADDR pc, INSTRClass* instr, UINT meta); //, ldb, ldh, ldub, lduh, ldw
	STRING disasmCore01_04 (ADDR pc, INSTRClass* instr, UINT meta); //, jp, jp.lnk
	STRING disasmCore01_05 (ADDR pc, INSTRClass* instr, UINT meta); //, mvup.i
	STRING disasmCore01_06 (ADDR pc, INSTRClass* instr, UINT meta); //, and.i, nor.i, or.i, xor.i
	STRING disasmCore01_07 (ADDR pc, INSTRClass* instr, UINT meta); //, br.eq, br.ne
	STRING disasmCore02_01 (ADDR pc, INSTRClass* instr, UINT meta); //, add16, and16, mv16, or16, shll16, sub16, xor16
	STRING disasmCore02_02 (ADDR pc, INSTRClass* instr, UINT meta); //, mv16.i
	STRING disasmCore02_03 (ADDR pc, INSTRClass* instr, UINT meta); //, br16.eqz, br16.nez
	STRING disasmCore02_05 (ADDR pc, INSTRClass* instr, UINT meta); //, add16.i, and16.i, or16.i, shra16.i, shll16.i, shrl16.i, sub16.i, xor16.i
	STRING disasmCore02_06 (ADDR pc, INSTRClass* instr, UINT meta); //, mvfc16
	STRING disasmCore02_07 (ADDR pc, INSTRClass* instr, UINT meta); //, mvtc16
	STRING disasmCore02_08 (ADDR pc, INSTRClass* instr, UINT meta); //, ldw16
	STRING disasmCore02_09 (ADDR pc, INSTRClass* instr, UINT meta); //, stw16
	STRING disasmCore02_10 (ADDR pc, INSTRClass* instr, UINT meta); //, pop16
	STRING disasmCore02_11 (ADDR pc, INSTRClass* instr, UINT meta); //, push16
	STRING disasmCore03_01 (ADDR pc, INSTRClass* instr, UINT meta); //, nop16, jr16, jr16.lnk
	STRING disasmCore03_02 (ADDR pc, INSTRClass* instr, UINT meta); //, abs16, inv16, ldub16.rs, lduh16.rs
	STRING disasmCore03_03 (ADDR pc, INSTRClass* instr, UINT meta); //, add16.sp
	STRING disasmCore03_04 (ADDR pc, INSTRClass* instr, UINT meta); //, rete16, ret16
	STRING disasmCore04_01 (ADDR pc, INSTRClass* instr, UINT meta); //, nop
	STRING disasmCore04_02 (ADDR pc, INSTRClass* instr, UINT meta); //, add, and, nor, or, setlt, setltu, shll, shra, shrl, sub, xor
	STRING disasmCore04_03 (ADDR pc, INSTRClass* instr, UINT meta); //, depb, extrb, extrbu
	STRING disasmCore04_04 (ADDR pc, INSTRClass* instr, UINT meta); //, mc.r.eq, mc.r.ne, mc.r.gt, mc.r.ge, mc.r.lt, mc.r.le, mc.zc.eq, mc.zc.ne, mc.zc.gt, mc.zc.ge, mc.zc.lt, mc.zc.le, mc.z.eq, mc.z.ne, mc.z.gt, mc.z.ge, mc.z.lt, mc.z.le, mc.zn.eq, mc.zn.ne, mc.zn.gt, mc.zn.ge, mc.zn.lt, mc.zn.le
	STRING disasmCore04_05 (ADDR pc, INSTRClass* instr, UINT meta); //, mc.abs
	STRING disasmCore04_06 (ADDR pc, INSTRClass* instr, UINT meta); //, mvfc
	STRING disasmCore04_08 (ADDR pc, INSTRClass* instr, UINT meta); //, mvtc
	STRING disasmCore04_10 (ADDR pc, INSTRClass* instr, UINT meta); //, shll.i, shra.i, shrl.i
	STRING disasmCore05_01 (ADDR pc, INSTRClass* instr, UINT meta); //, br.gez, br.gtz, br.lez, br.ltz, dcflush
	STRING disasmCore05_02 (ADDR pc, INSTRClass* instr, UINT meta); //, ret, rete
	STRING disasmCore05_03 (ADDR pc, INSTRClass* instr, UINT meta); //, syscall
	STRING disasmCore05_04 (ADDR pc, INSTRClass* instr, UINT meta); //, loop
	STRING disasmCore05_05 (ADDR pc, INSTRClass* instr, UINT meta); //, jr, jr.lnk
	STRING disasmCore05_06 (ADDR pc, INSTRClass* instr, UINT meta); //mvtlc.i
	STRING disasmUndefined (ADDR pc, INSTRClass* instr, UINT meta); //, G0, G1, G2, C1, C3, C4, break
};

template <class MMUClass, class REGClass, class INSTRClass>
CoreDisasm< MMUClass, REGClass, INSTRClass>::CoreDisasm(MMUClass& mmu, REGClass& reg, UINT th_id) : _mmu(mmu), _reg(reg) {
	_symtable = new SymTable;
	_thread_id = th_id;
}

//, add.i, setlt.i, setltu.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_01 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,%d <0x%x,0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      instr->imm16(),
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()));
	return RET_STRBUF(pc);
}

//, stb, sth, stw
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_02 (ADDR pc, INSTRClass* instr, UINT meta) {
    ADDR base = reg().getGPR(instr->rs1());
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,%d <0x%x,0x%x,0x%08x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rs0(),
	      instr->rs1(),
	      instr->offset(),
	      reg().getGPR(instr->rs0()),
	      base,
	      base + instr->offset());
     return RET_STRBUF(pc);
}

//, ldb, ldh, ldub, lduh, ldw
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_03 (ADDR pc, INSTRClass* instr, UINT meta) {
    ADDR base = reg().getGPR(instr->rs1());
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,%d <0x%x,0x%x,0x%08x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      instr->offset(),
	      reg().getGPR(instr->rd()),
	      base,
	      base + instr->offset());

	return RET_STRBUF(pc);
}

//, jp, jp.lnk
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_04 (ADDR pc, INSTRClass* instr, UINT meta) {
	ADDR target = (pc & 0xf0000000) | (instr->instr_index()<<OFFSET_SHIFT_JP);
	const STRING name = getSymName(target);
	int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %d <0x%08x>",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->instr_index(),
	      target);
	if (name)
     	sprintf(strbuf() + cc, "<%s>\n", name);
     else
     	sprintf(strbuf() + cc, "\n");
     return RET_STRBUF(pc); 
}

//, mvup.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_05 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->imm16(),
	      reg().getGPR(instr->rd()));
	return RET_STRBUF(pc);
}

//, and.i, nor.i, or.i, xor.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_06 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d, r%d,%d <0x%x,0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      instr->imm16(),
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()) );
	return RET_STRBUF(pc);
}

//, br.eq, br.ne
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore01_07 (ADDR pc, INSTRClass* instr, UINT meta) {
 	ADDR target = pc+(instr->offset()<<OFFSET_SHIFT_BR);
 	const STRING name = getSymName(target);
    int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,%d <0x%x,0x%x,0x%08x>",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      instr->offset(),
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()),
	      target);
	if (name)
     	sprintf(strbuf() + cc, "<%s>\n", name);
     else
     	sprintf(strbuf() + cc, "\n");      
	return RET_STRBUF(pc);
}

//, add16, and16, mv16, or16, shll16, sub16, xor16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_01 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d <0x%x,0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()));
	return RET_STRBUF(pc);
}

//, mv16.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_02 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->imm5() & 0x1f,
	      reg().getGPR(instr->rd()));
	return RET_STRBUF(pc);
}

//, br16.eqz, br16.nez
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_03 (ADDR pc, INSTRClass* instr, UINT meta) {
    ADDR target = pc+(instr->offset()<<OFFSET_SHIFT_BR_16);
    const STRING name = getSymName(target);
    int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x, 0x%x>",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rs0(),
	      instr->offset(),
	      reg().getGPR(instr->rs0()),
	      target);
	if (name)
     	sprintf(strbuf() + cc, "<%s>\n", name);
    else
     	sprintf(strbuf() + cc, "\n");     
	return RET_STRBUF(pc);
}

//, add16.i, and16.i, or16.i, shra16.i, shll16.i, shrl16.i, sub16.i, xor16.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_05 (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->imm5(),
	      reg().getGPR(instr->rd()));
	return RET_STRBUF(pc);   
}

//, mvfc16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_06 (ADDR pc, INSTRClass* instr, UINT meta) {
    AppFatal((instr->cs1() < ECR_MAX), ("invalid control register name(%d), pc@0x%x", instr->cs1(), pc));
//    printf("%s\n",reg_name_ctrl[instr->cs1()]);
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%s <0x%x,0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            instr->rd(),
            reg_name_ctrl[instr->cs1()],
            reg().getGPR(instr->rd()) ,
            reg().getCTRL(instr->cs1()));
	return RET_STRBUF(pc);	
}

//, mvtc16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_07 (ADDR pc, INSTRClass* instr, UINT meta) {
	AppFatal((instr->cd() < ECR_MAX), ("invalid control register name(%d),pc@0x%x", instr->cs1(), pc));
//    printf("%s\n",reg_name_ctrl[instr->cs1()]);
	sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %s,r%d <0x%x,0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            reg_name_ctrl[instr->cd()],
            instr->rs1(),
            reg().getCTRL(instr->cd()),
            reg().getGPR(instr->rs1()) );
	return RET_STRBUF(pc);	
}

//, ldw16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_08 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d <0x%x, 0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      instr->rs1(),
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()));
	return RET_STRBUF(pc);
}

//, stw16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_09 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d <0x%x, 0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rs0(),
	      instr->rs1(),
	      reg().getGPR(instr->rs0()),
	      reg().getGPR(instr->rs1()));
	return RET_STRBUF(pc);
}

//, pop16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_10 (ADDR pc, INSTRClass* instr, UINT meta) {
    ADDR offset = instr->imm4() & 0x1f;
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x, 0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      offset,
	      reg().getGPR(instr->rd()),
	      reg().getGPR(instr->rs1()) + (offset<<2));
	return RET_STRBUF(pc);
}

//, push16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore02_11 (ADDR pc, INSTRClass* instr, UINT meta) {
    ADDR offset = instr->imm4() & 0x1f;
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x, 0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rs0(),
	      offset,
	      reg().getGPR(instr->rs0()),
	      reg().getGPR(instr->rs1()) + (offset<<2));
	return RET_STRBUF(pc);
}

//, nop16, jr16, jr16.lnk
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore03_01 (ADDR pc, INSTRClass* instr, UINT meta) {
	ADDR target = reg().getJA();
	const STRING name = getSymName(target);
    	int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s <0x%08x>",
			pc,
             		instr->rawbits(),
             		instr->mn(),
             		target);
    	if (name)
     		sprintf(strbuf() + cc, "<%s>\n", name);
	else
     		sprintf(strbuf() + cc, "\n");                
	return RET_STRBUF(pc); 
}

//, abs16, inv16, ldub16.rs, lduh16.rs
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore03_02 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d <0x%x>\n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->rd(),
	      reg().getGPR(instr->rd()));
	return RET_STRBUF(pc);
}

//, add16.sp
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore03_03 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %d \n",
          pc,
	      instr->rawbits(),
	      instr->mn(),
	      instr->imm5());
	return RET_STRBUF(pc);
}

//, rete16, ret16
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore03_04 (ADDR pc, INSTRClass* instr, UINT meta) {
	ADDR target = reg().getRA();
	pair<STRING, INT> func_off = getSymOffset(target);
    int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s",
            pc,
            instr->rawbits(),
            instr->mn());
    if (func_off.first) {
		if(func_off.second>=0) {
     		sprintf(strbuf() + cc, " <0x%x> <%s+%d>\n", target, func_off.first, func_off.second);
		}
		else {
			sprintf(strbuf() + cc, " <0x%x> <%s-%d>\n", target, func_off.first, func_off.second);
		}
    }
    else {
     	sprintf(strbuf() + cc, " <0x%x>\n", target);
    }                
    return RET_STRBUF(pc);   
}

//, nop
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_01 (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(),"0x%08x: 0x%08x  %-5s\n",
          pc,
	      instr->rawbits(),
	      instr->mn());
	return RET_STRBUF(pc); 
}

//, add, and, nor, or, setlt, setltu, shll, shra, shrl, sub, xor
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_02 (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(), 
      "0x%08x: 0x%08x  %-5s r%d,r%d,r%d <0x%x,0x%x,0x%x>\n", 
             pc,
             instr->rawbits(),
             instr->mn(),
             instr->rd(),
             instr->rs1(),
             instr->rs2(),
             reg().getGPR(instr->rd()) ,
             reg().getGPR(instr->rs1()) ,
             reg().getGPR(instr->rs2()) );
             return RET_STRBUF(pc);
}

//, depb, extrb, extrbu
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_03 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,%d,%d <0x%x,0x%x>\n", 
             pc,
             instr->rawbits(),
             instr->mn(),
             instr->rd(),
             instr->rs1(),
             instr->pos(),
             instr->width(),
             reg().getGPR(instr->rd()),
             reg().getGPR(instr->rs1()));
             return RET_STRBUF(pc);
}

//, mc.r.eq, mc.r.ne, mc.r.gt, mc.r.ge, mc.r.lt, mc.r.le, mc.zc.eq, mc.zc.ne, mc.zc.gt, mc.zc.ge, mc.zc.lt, mc.zc.le, mc.z.eq, mc.z.ne, mc.z.gt, mc.z.ge, mc.z.lt, mc.z.le, mc.zn.eq, mc.zn.ne, mc.zn.gt, mc.zn.ge, mc.zn.lt, mc.zn.le
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_04 (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d,r%d,%d <0x%x,0x%x,0x%x>\n",
        	pc,
        	instr->rawbits(),
        	instr->mn(),
        	instr->rd(),
        	instr->rs1(),
			instr->rs2(),
			instr->unsigned_flag(),
        	reg().getGPR(instr->rd()) ,
        	reg().getGPR(instr->rs1()),
			reg().getGPR(instr->rs2()));
	return RET_STRBUF(pc); 
}

//, mc.abs
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_05 (ADDR pc, INSTRClass* instr, UINT meta) {
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,r%d <0x%x,0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            instr->rd(),
            instr->rs1(),
            reg().getGPR(instr->rd()) ,
            reg().getGPR(instr->rs1()));
            return RET_STRBUF(pc); 
}

//, mvfc
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_06 (ADDR pc, INSTRClass* instr, UINT meta) {
    AppFatal((instr->cs1() < ECR_MAX), ("invalid control register name(%d),pc@0x%x", instr->cs1(), pc));
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%s <0x%x,0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            instr->rd(),
            reg_name_ctrl[instr->cs1()],
            reg().getGPR(instr->rd()) ,
            reg().getCTRL(instr->cs1()));
	return RET_STRBUF(pc);		
    
}

//, mvtc
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_08 (ADDR pc, INSTRClass* instr, UINT meta) {
    AppFatal((instr->cd() < ECR_MAX), ("invalid control register name(%d),pc@0x%x", instr->cs1(), pc));
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %s,r%d <0x%x,0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            reg_name_ctrl[instr->cd()],
            instr->rs1(),
            reg().getCTRL(instr->cd()),
            reg().getGPR(instr->rs1()) );
            return RET_STRBUF(pc);	
}

//, shll.i, shra.i, shrl.i
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore04_10 (ADDR pc, INSTRClass* instr, UINT meta) {
     sprintf(strbuf(),"0x%08x: 0x%08x  %-5s  r%d,r%d,%d <0x%x,0x%x>\n", 
             pc,
             instr->rawbits(),
             instr->mn(),
             instr->rd(),
             instr->rs1(),
             instr->imm16(),
             reg().getGPR(instr->rd()),
             reg().getGPR(instr->rs1()));
	return RET_STRBUF(pc);
}

//, br.gez, br.gtz, br.lez, br.ltz, dcflush
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_01 (ADDR pc, INSTRClass* instr, UINT meta) {
	ADDR target = pc+(instr->offset()<<OFFSET_SHIFT_BR);
	const STRING name = getSymName(target);
    int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s r%d,%d <0x%x,0x%08x>",
             pc,
             instr->rawbits(),
             instr->mn(),
             instr->rs1(),
             instr->imm16(),
             reg().getGPR(instr->rs1()),
             target);
     if (name)
     	sprintf(strbuf() + cc, "<%s>\n", name);
     else
     	sprintf(strbuf() + cc, "\n");
     return RET_STRBUF(pc);
}

//, ret, rete
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_02 (ADDR pc, INSTRClass* instr, UINT meta) {
     ADDR target = reg().getRA();
     pair<STRING, INT> func_off = getSymOffset(target);
     int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s",
             pc,
             instr->rawbits(),
             instr->mn());
     if (func_off.first) {
		if(func_off.second>=0) {
     		sprintf(strbuf() + cc, " <0x%x> <%s+%d>\n", target, func_off.first, func_off.second);
		}
		else {
			sprintf(strbuf() + cc, " <0x%x> <%s-%d>\n", target, func_off.first, func_off.second);
		}
     }
     else {
     	sprintf(strbuf() + cc, " <0x%x>\n", target);
     }                
     return RET_STRBUF(pc);     
}

//, syscall
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_03 (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %d\n",
	         pc,
             instr->rawbits(),
             instr->mn(),
             instr->sysc_id());
	return RET_STRBUF(pc); 
}

//, loop
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_04 (ADDR pc, INSTRClass* instr, UINT meta) {
	 sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %d,%d\n",
	       pc,
           instr->rawbits(),
           instr->mn(),
           instr->loop_index(),
	       instr->instr_offset() );
	return RET_STRBUF(pc);
}

//, jr, jr.lnk
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_05 (ADDR pc, INSTRClass* instr, UINT meta) {
     ADDR target = reg().getJA();
     const STRING name = getSymName(target);
     int cc = sprintf(strbuf(),"0x%08x: 0x%08x  %-5s <0x%08x >",
             pc,
             instr->rawbits(),
             instr->mn(),
             target);
     if (name)
     	sprintf(strbuf() + cc, "<%s>\n", name);
     else
     	sprintf(strbuf() + cc, "\n");                
     return RET_STRBUF(pc);  
}


template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmCore05_06 (ADDR pc, INSTRClass* instr, UINT meta) {
	AppFatal((instr->cd() < ECR_MAX), ("invalid control register name(%d),pc@0x%x", instr->cs1(), pc));
    sprintf(strbuf(),"0x%08x: 0x%08x  %-5s %s,%d <0x%x>\n",
            pc,
            instr->rawbits(),
            instr->mn(),
            reg_name_ctrl[instr->cd()],
            instr->imm10(),
            reg().getCTRL(instr->cd()));
            return RET_STRBUF(pc);	

}

//, G0, G1, G2, C1, C3, C4, break
template <class MMUClass, class REGClass, class INSTRClass>
STRING CoreDisasm< MMUClass, REGClass, INSTRClass>::disasmUndefined (ADDR pc, INSTRClass* instr, UINT meta) {
	sprintf(strbuf(),"unknown instruction\n");
	return RET_STRBUF(pc);
}


#endif /*COREDISASM_H_*/
