/*
 *  File: cexec.h
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 */
#ifndef COREEXEC_H_
#define COREEXEC_H_

#include "defs.h"
#include "messg.h"
#include "status.h"
#include "cregdefs.h"
#include "vrt.h"

#ifdef _SYSTEM_SIMULATION
#define _THROW_EXEC_EXCEPTION _SYSTEM_SIMULATION
#else
#define _THROW_EXEC_EXCEPTION 0
#endif

#if _THROW_EXEC_EXCEPTION
#define EXEC_EXCEPTION(ex_type, cond, arg) status().event(ex_type);
#else
#define EXEC_EXCEPTION(ex_type, cond, arg) AppFatal(cond, arg);
#endif

template <class MMUClass, class REGClass, class INSTRClass>
class CoreExec {
	private:
	ADDR _pc;
	MMUClass& _mmu;
	REGClass& _reg;	
	ProcessStatus<INSTRClass>& _status;
	
	protected:
	ADDR loopMode(ADDR curAddr, ADDR retAddr);
			
	public:
	CoreExec(MMUClass& mmu, REGClass& reg, ProcessStatus<INSTRClass>& status);	
	ProcessStatus<INSTRClass>& status(void) { return _status; }
	REGClass& reg(void) { return _reg; }
	MMUClass& mmu(void) { return _mmu; }
	void setExecMode(INT modeID, WORD value);
	ADDR pc(void) { return _pc; }	
	void pc(ADDR p) { _pc = p; }
	void *pcAddr(void) { return (void*)&_pc; }		
	BOOL isInstr16(WORD raw) {
		union {
			WORD rawbits;
			struct {
				unsigned lv2 : 6;
				unsigned na : 10;
				unsigned lv2s : 5;
				unsigned lv2g1 : 5;
				unsigned top :6;
			} bits;
		}rb;
		rb.rawbits = raw;
		if((rb.bits.top==0x2)||(rb.bits.top>=0x20&&rb.bits.top<=0x37)) {
			return TRUE;
		}
		else {
			return FALSE;
		}
	}
	
	
			
	//Execution function
	ADDR execAbs16 (INSTRClass* instr);
	ADDR execAdd (INSTRClass* instr);
	ADDR execAdd16 (INSTRClass* instr);
	ADDR execAdd16i (INSTRClass* instr);
	ADDR execAdd16sp (INSTRClass* instr);
	ADDR execAddi (INSTRClass* instr);
	ADDR execAnd (INSTRClass* instr);
	ADDR execAnd16 (INSTRClass* instr);
	ADDR execAnd16i (INSTRClass* instr);
	ADDR execAndi (INSTRClass* instr);
	ADDR execBreak (INSTRClass* instr);
	ADDR execBreak16 (INSTRClass* instr);
	ADDR execBreq (INSTRClass* instr);
	ADDR execBrgez (INSTRClass* instr);
	ADDR execBrgtz (INSTRClass* instr);
	ADDR execBrlez (INSTRClass* instr);
	ADDR execBrltz (INSTRClass* instr);
	ADDR execBrne (INSTRClass* instr);
	ADDR execDcflush (INSTRClass* instr);
	ADDR execDepb (INSTRClass* instr);
	ADDR execExtrb (INSTRClass* instr);
	ADDR execExtrbu (INSTRClass* instr);
	ADDR execIcflush (INSTRClass* instr);
	ADDR execIllegal (INSTRClass* instr);
	ADDR execInv16 (INSTRClass* instr);	
	ADDR execInvalid (INSTRClass* instr);
	ADDR execJp (INSTRClass* instr);
	ADDR execJplnk (INSTRClass* instr);
	ADDR execJr (INSTRClass* instr);
	ADDR execJr16 (INSTRClass* instr);
	ADDR execJr16lnk (INSTRClass* instr);
	ADDR execJrlnk (INSTRClass* instr);
	ADDR execLdb (INSTRClass* instr);
	ADDR execLdh (INSTRClass* instr);
	ADDR execLdlnk (INSTRClass* instr);
	ADDR execLdub (INSTRClass* instr);
	ADDR execLdub16rs (INSTRClass* instr);
	ADDR execLduh (INSTRClass* instr);
	ADDR execLduh16rs (INSTRClass* instr);
	ADDR execLdw (INSTRClass* instr);
	ADDR execLdw16 (INSTRClass* instr);
	ADDR execLoop (INSTRClass* instr);	
	ADDR execMcabs (INSTRClass* instr);
	ADDR execMcreq (INSTRClass* instr);
	ADDR execMcrge (INSTRClass* instr);
	ADDR execMcrgt (INSTRClass* instr);
	ADDR execMcrle (INSTRClass* instr);
	ADDR execMcrlt (INSTRClass* instr);
	ADDR execMcrne (INSTRClass* instr);
	ADDR execMczceq (INSTRClass* instr);
	ADDR execMczcge (INSTRClass* instr);
	ADDR execMczcgt (INSTRClass* instr);
	ADDR execMczcle (INSTRClass* instr);
	ADDR execMczclt (INSTRClass* instr);
	ADDR execMczcne (INSTRClass* instr);
	ADDR execMczeq (INSTRClass* instr);
	ADDR execMczge (INSTRClass* instr);
	ADDR execMczgt (INSTRClass* instr);
	ADDR execMczle (INSTRClass* instr);
	ADDR execMczlt (INSTRClass* instr);
	ADDR execMczne (INSTRClass* instr);
	ADDR execMczneq (INSTRClass* instr);
	ADDR execMcznge (INSTRClass* instr);
	ADDR execMczngt (INSTRClass* instr);
	ADDR execMcznle (INSTRClass* instr);
	ADDR execMcznlt (INSTRClass* instr);
	ADDR execMcznne (INSTRClass* instr);
	ADDR execMv16 (INSTRClass* instr);
	ADDR execMv16i (INSTRClass* instr);
	ADDR execMvfc (INSTRClass* instr);
	ADDR execMvfc16 (INSTRClass* instr);
	ADDR execMvtc (INSTRClass* instr);
	ADDR execMvtc16 (INSTRClass* instr);
	ADDR execMvupi (INSTRClass* instr);
	ADDR execNop (INSTRClass* instr);
	ADDR execNop16 (INSTRClass* instr);
	ADDR execNor (INSTRClass* instr);
	ADDR execNori (INSTRClass* instr);
	ADDR execOr (INSTRClass* instr);
	ADDR execOr16 (INSTRClass* instr);
	ADDR execOr16i (INSTRClass* instr);
	ADDR execOri (INSTRClass* instr);
	ADDR execPop16 (INSTRClass* instr);
	ADDR execPush16 (INSTRClass* instr);
	ADDR execRet (INSTRClass* instr);
	ADDR execRet16 (INSTRClass* instr);
	ADDR execRete (INSTRClass* instr);
	ADDR execRete16 (INSTRClass* instr);
	ADDR execSetlt (INSTRClass* instr);
	ADDR execSetlti (INSTRClass* instr);
	ADDR execSetltu (INSTRClass* instr);
	ADDR execSetltui (INSTRClass* instr);
	ADDR execShll (INSTRClass* instr);
	ADDR execShll16 (INSTRClass* instr);
	ADDR execShll16i (INSTRClass* instr);
	ADDR execShlli (INSTRClass* instr);
	ADDR execShra (INSTRClass* instr);
	ADDR execShra16 (INSTRClass* instr);
	ADDR execShra16i (INSTRClass* instr);
	ADDR execShrai (INSTRClass* instr);
	ADDR execShrl (INSTRClass* instr);
	ADDR execShrl16 (INSTRClass* instr);
	ADDR execShrl16i (INSTRClass* instr);
	ADDR execShrli (INSTRClass* instr);
	ADDR execStb (INSTRClass* instr);
	ADDR execStc (INSTRClass* instr);
	ADDR execSth (INSTRClass* instr);
	ADDR execStw (INSTRClass* instr);
	ADDR execStw16 (INSTRClass* instr);
	ADDR execSub (INSTRClass* instr);
	ADDR execSub16 (INSTRClass* instr);
	ADDR execSub16i (INSTRClass* instr);
	ADDR execSyscall (INSTRClass* instr);
	ADDR execXor (INSTRClass* instr);
	ADDR execXor16 (INSTRClass* instr);
	ADDR execXor16i (INSTRClass* instr);
	ADDR execXori (INSTRClass* instr);
	ADDR execBr16eqz (INSTRClass* instr);
	ADDR execBr16nez (INSTRClass* instr);	
	ADDR execMvtci(INSTRClass* instr);	
};

template <class MMUClass, class REGClass, class INSTRClass>
CoreExec<MMUClass, REGClass, INSTRClass>::CoreExec(MMUClass& mmu, REGClass& reg, ProcessStatus<INSTRClass>& status) 
									: _mmu(mmu), _reg(reg), _status(status) {
}

template <class MMUClass, class REGClass, class INSTRClass>
void CoreExec<MMUClass, REGClass, INSTRClass>::setExecMode(INT modeID, WORD value) {
	fprintf(stdout, "Exec: the execution unit doesn't support any configurable option.\n");
}

template <class MMUClass, class REGClass, class INSTRClass>
INLINE ADDR CoreExec<MMUClass, REGClass, INSTRClass>::loopMode (ADDR curAddr, ADDR retAddr) {
	INT loopCurIndex = reg().loopCurIndex();	
    if(loopCurIndex>=0) {
		AppFatal((loopCurIndex<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop stack level (%d)", loopCurIndex));
//   		if(curAddr==reg().loopEndPC(loopCurIndex)) {
		ADDR endpc = reg().loopEndPC(loopCurIndex);
   		if(curAddr== endpc|| ((curAddr+INSTR16) == endpc && (curAddr+INSTR32==retAddr))) {
   			UINT cnt = reg().loopExecCnt(loopCurIndex) - 1;
   			reg().loopExecCnt(loopCurIndex, cnt);
   			if(cnt>0) {
   				retAddr = reg().loopStartPC(loopCurIndex);
   			}
   			else {
   				loopCurIndex--;  				
   				while(loopCurIndex>=0)	{
//  					if(curAddr==(reg().loopEndPC(loopCurIndex))){
					endpc = reg().loopEndPC(loopCurIndex);
   					if(curAddr==endpc || ((curAddr+INSTR16) == endpc && (curAddr+INSTR32==retAddr))) {
   						cnt = reg().loopExecCnt(loopCurIndex) - 1;
   						reg().loopExecCnt(loopCurIndex, cnt);
   						AppFatal((cnt<0xffff), ("Exec: Invalid loop count (index=%d, @0x%08x)", loopCurIndex, curAddr));
   				  		if(cnt>0) {
   				       		retAddr = reg().loopStartPC(loopCurIndex);
   				       		break;
   				  		}
   				  		else {
   				  			loopCurIndex--;  
   				  		}
   				    }
   				    else {
   				    	break;
   				    }
   				}
   			}
   		}
   		reg().loopCurIndex(loopCurIndex);
   }
   return retAddr;      
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAbs16 (INSTRClass* instr) {
   WORD src_rd = reg().getGPR(instr->rd());
   WORD dest = (src_rd<0)? -src_rd : src_rd;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();      
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAdd (INSTRClass* instr) {
	 UWORD src1,src2,dest;
     src1 = reg().getGPR(instr->rs1());
     src2 = reg().getGPR(instr->rs2());
     dest = src1+src2;
     reg().setGPR(instr->rd(),dest);
     
     return pc()+instr->instrsize();     
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAdd16 (INSTRClass* instr) {
     UWORD src1 = reg().getGPR(instr->rs1());
     UWORD src_rd = reg().getGPR(instr->rd());
     src_rd = src_rd+src1;
     reg().setGPR(instr->rd(),src_rd);
     return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAdd16i (INSTRClass* instr) {
     UINT uimm = instr->imm5() & 0x1f;
     WORD src_rd = reg().getGPR(instr->rd());
     src_rd = src_rd+uimm;
     reg().setGPR(instr->rd(),src_rd);
     return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAdd16sp (INSTRClass* instr) {
     ADDR ad1 = reg().getSP();
     INT simm = instr->imm5();
     ad1 += simm<<2;
     reg().setSP(ad1);
     return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAddi (INSTRClass* instr) {
    UWORD src1 = reg().getGPR(instr->rs1());
    WORD simm = instr->imm16();
    UWORD dest = src1+simm;
    reg().setGPR(instr->rd(),dest);
    return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAnd (INSTRClass* instr) {
    UWORD src1,src2,dest;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = src1&src2;
    reg().setGPR(instr->rd(),dest);
    return pc()+instr->instrsize();     
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAnd16 (INSTRClass* instr) {
    WORD src1 = reg().getGPR(instr->rs1());
    WORD src_rd = reg().getGPR(instr->rd());
    src_rd = src_rd & src1;
    reg().setGPR(instr->rd(),src_rd);
    return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAndi (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   UINT uimm = instr->imm16() & 0xffff;
   WORD dest = src1&uimm;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execAnd16i (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rd());
   UINT uimm = instr->imm5() & 0x1f;
   WORD dest = src1&uimm;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBreak (INSTRClass* instr) {
	EXEC_EXCEPTION(EVENT_BREAK, (0), ("Exec: unsupported sw exception"));
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBreak16 (INSTRClass* instr) {
	EXEC_EXCEPTION(EVENT_BREAK, (0), ("Exec: unsupported sw exception"));
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBreq (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs0());
   WORD src2 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1==src2)? offset:size;
   LOG_BR((src1==src2),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBrgez (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1 >=0)? offset:size;
   LOG_BR((src1>=0),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBrgtz (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1 >0)? offset:size;
   LOG_BR((src1>0),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBrlez (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1<=0)? offset:size;
   LOG_BR((src1<=0),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBrltz (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1 <0)? offset:size;
   LOG_BR((src1<0),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBrne (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs0());
   WORD src2 = reg().getGPR(instr->rs1());
   ADDR target = pc();
   WORD offset = instr->offset()<<OFFSET_SHIFT_BR;
   UINT size = instr->instrsize();
   target += (src1!=src2)? offset:size;
   LOG_BR((src1!=src2),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execDcflush (INSTRClass* instr) {
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execDepb (INSTRClass* instr) {
   WORD src = reg().getGPR(instr->rs1());
   WORD dest = reg().getGPR(instr->rd());
   UINT pos = instr->pos() & 0x1f;
   UINT width = instr->width() & 0x1f;
   AppFatal((width-1<=pos), ("depb: invalid position(%d) or width(%d), pc@0x%x", pos, width, pc()));
   //return wrong situation
   UINT maskup = 0xffffffff;
   UINT maskdown = 0xffffffff;
   UINT mask = maskup<<width;
   src = src & (~mask);
   src = src<<(pos-width+1);
   maskup = maskup>>(31-pos);
   maskdown = maskdown<<(pos-width+1);
   mask = maskup & maskdown;
   dest = dest & (~mask);
   dest = dest | src;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();    
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execExtrb (INSTRClass* instr) {
   WORD src = reg().getGPR(instr->rs1());
   UINT pos = instr->pos() & 0x1f;
   UINT width = instr->width() & 0x1f;
   UINT32 mask = 0xffffffff;
   if ((width-1)>pos)
      AppFatal((0), ("extrb: invalid position(%d) or width(%d), pc@0x%x", pos, width, pc()));
   src >>= (pos-width+1);   
   mask <<= width;
   src &= (~mask);
	//sign ext.
   if((src&(mask>>1))>0)
		src |= mask;	
	reg().setGPR(instr->rd(), src);
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execExtrbu (INSTRClass* instr) {
    INT32 rs = reg().getGPR(instr->rs1());
	INT32 pos = instr->pos() & 0x1f;	
	INT32 width = instr->width() & 0x1f;	
	UINT32 mask = 0xffffffff;
	if((width-1)>pos){
		AppFatal((0), ("Extrbu: invalid position(%d) or width(%d), pc@0x%x", pos, width, pc()));
	}	
	rs >>= (pos-width+1);
	if(width<(TARG_WORD_SIZE*8))
		mask <<= width;
	else
		mask = 0;
	rs &= (~mask);
	reg().setGPR(instr->rd(),rs);
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execIcflush (INSTRClass* instr) {
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execInvalid (INSTRClass* instr) {
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJp (INSTRClass* instr) {
    ADDR offset = ((pc() + instr->instrsize()) & 0xf8000000) | (instr->instr_index()<<OFFSET_SHIFT_JP);
    return offset;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJplnk (INSTRClass* instr) {
    ADDR offset = ((pc() + instr->instrsize()) & 0xf8000000) | (instr->instr_index()<<OFFSET_SHIFT_JP);
    ADDR ra = pc() + instr->instrsize();
    reg().setRA(ra);
    return offset;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJr (INSTRClass* instr) {
    ADDR target = reg().getJA();
    return target;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJr16 (INSTRClass* instr) {
    ADDR target = reg().getJA();
    return target;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJr16lnk (INSTRClass* instr) {
    ADDR target = reg().getJA();
    ADDR ra = pc() + instr->instrsize();
    reg().setRA(ra);
    return target;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execJrlnk (INSTRClass* instr) {
    ADDR target = reg().getJA();
    ADDR ra = pc() + instr->instrsize();
    reg().setRA(ra);
    return target;
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdb (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1()) + instr->offset();
	LOG_EXTMETA("offset", addr)
	WORD data = mmu().readByte(addr);
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdh (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1()) + instr->offset();
	LOG_EXTMETA("offset", addr)
	WORD data = mmu().readHword(addr);
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdlnk (INSTRClass* instr) {
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdub (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1()) + instr->offset();
	LOG_EXTMETA("offset", addr)
	WORD data = mmu().readByte(addr) & 0xff;
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdub16rs (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rd());
	WORD data = mmu().readByte(addr) & 0xff;
	LOG_EXTMETA("offset", addr)
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLduh (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1()) + instr->offset();;
	WORD data = mmu().readHword(addr) & 0xffff;
	LOG_EXTMETA("offset", addr)
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLduh16rs  (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rd());
	WORD data = mmu().readHword(addr) & 0xffff;
	LOG_EXTMETA("offset", addr)
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdw (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1()) + instr->offset();
	WORD data = mmu().readWord(addr);
	LOG_EXTMETA("offset", addr)
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize();
    
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLdw16 (INSTRClass* instr) {
    ADDR addr = reg().getGPR(instr->rs1());
	WORD data = mmu().readWord(addr);
	LOG_EXTMETA("offset", addr)
	reg().setGPR(instr->rd(),data); // sign-extended
	return pc() + instr->instrsize();
    
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execLoop (INSTRClass* instr) {
	ADDR loopStart = pc()+instr->instrsize();
	AppFatal((instr->loop_index()<REG_LOOP_CUR_SIZE),("invalid loop index(%d)\n",instr->loop_index()));
	UINT index = instr->loop_index();
	UINT cnt = reg().getLOOP_CNT(index);
	ADDR endPC = (instr->instr_offset()<<OFFSET_SHIFT_BR)+pc();
	if(cnt>0) { //if cnt=0, loop is effect is ignored, pc = endpc+2
		AppFatal((instr->instr_offset()>0), ("Exec: Invalid instr offset (empty loop?)"));
		INT loopCurIndex = reg().loopCurIndex() + 1;
		
		//in hw, the end pc always set at the final half word
		//end pc = n if n is a 32bits instr
		//end pc = n+2 if n is a 16bits instr
	
		AppFatal((loopCurIndex>=0&&loopCurIndex<REG_LOOP_CUR_SIZE), ("Exec: InvalidFE loop level (%d)", loopCurIndex));
		reg().loopExecCnt(loopCurIndex, cnt);
		reg().loopStartPC(loopCurIndex, loopStart);
		reg().loopEndPC(loopCurIndex, endPC);
		reg().loopCurIndex(loopCurIndex);
		return loopStart;
	}
	else {
		return (endPC + INT16_BYTE);
	}
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcabs (INSTRClass* instr) {
    WORD src,dest;
	src = reg().getGPR(instr->rs1());
    dest = (src<0)? -src : src;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();    
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcreq (INSTRClass* instr) {
	WORD src1,src2,dest;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = (src1 == src2)? src1:src2;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcrge (INSTRClass* instr) {
    WORD src1,src2,dest;
  
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 >= (unsigned)src2)? src1:src2;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 >= src2)? src1:src2;
	}
	reg().setGPR(instr->rd(),dest);
  
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcrgt (INSTRClass* instr) {
    WORD src1,src2,dest;
   
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 > (unsigned)src2)? src1:src2;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 > src2)? src1:src2;
	}
	reg().setGPR(instr->rd(),dest);
 
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcrle (INSTRClass* instr) {
    WORD src1,src2,dest;

	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 <= (unsigned)src2)? src1:src2;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 <= src2)? src1:src2;
	}
	reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcrlt (INSTRClass* instr) {
    WORD src1,src2,dest;
   
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 < (unsigned)src2)? src1:src2;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 < src2)? src1:src2;
	}
	reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcrne (INSTRClass* instr) {
   WORD src1,src2,dest;

    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = (src1 != src2)? src1:src2;
    reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();

}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczceq (INSTRClass* instr) {
	WORD src1,src2,dest;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = (src1 == 0)? src2:0;
    reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczcge (INSTRClass* instr) {
    WORD src1,src2,dest;
   
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 >=0 )? src2:0;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 >= 0)? src2:0;
	}
	reg().setGPR(instr->rd(),dest);
 
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczcgt (INSTRClass* instr) {
    WORD src1,src2,dest;
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 >0 )? src2:0;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 > 0)? src2:0;
	}
	reg().setGPR(instr->rd(),dest);
 
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczcle (INSTRClass* instr) {
    WORD src1,src2,dest;
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 <=0 )? src2:0;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 <= 0)? src2:0;
	}
	reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczclt (INSTRClass* instr) {
    WORD src1,src2,dest;
	if(instr->unsigned_flag()==1)
	{
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = ((unsigned)src1 < 0 )? src2:0;
	}
	else
	{    
		src1 = reg().getGPR(instr->rs1());
    	src2 = reg().getGPR(instr->rs2());
    	dest = (src1 < 0)? src2:0;
	}
	reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczcne (INSTRClass* instr) {
    WORD src1,src2,dest;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = (src1 != 0)? src2:0;
    reg().setGPR(instr->rd(),dest);

    return pc() + instr->instrsize();   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczeq (INSTRClass* instr) {
    WORD src1,src2;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    if(src1==0)
		reg().setGPR(instr->rd(),src2); 

   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczge (INSTRClass* instr) {
	WORD src1,src2;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
   		src1 = reg().getGPR(instr->rs1());
   		if((unsigned)src1>=0)
 			reg().setGPR(instr->rd(),src2);
	}
	else
	{
   		src1 = reg().getGPR(instr->rs1());
   		if(src1>=0)
 			reg().setGPR(instr->rd(),src2);
	}  
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczgt (INSTRClass* instr) {
	WORD src1,src2;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
   		src1 = reg().getGPR(instr->rs1());
   		if((unsigned)src1>0)
 			reg().setGPR(instr->rd(),src2);
	}
	else
	{
   		src1 = reg().getGPR(instr->rs1());
   		if(src1>0)
 			reg().setGPR(instr->rd(),src2);
	}   

	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczle (INSTRClass* instr) {
	WORD src1,src2;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
   		src1 = reg().getGPR(instr->rs1());
   		if((unsigned)src1<=0)
 			reg().setGPR(instr->rd(),src2);
	}
	else
	{
   		src1 = reg().getGPR(instr->rs1());
   		if(src1<=0)
 			reg().setGPR(instr->rd(),src2);
	}

	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczlt (INSTRClass* instr) {
    WORD src1,src2;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
   		src1 = reg().getGPR(instr->rs1());
   		if((unsigned)src1<0)
 			reg().setGPR(instr->rd(),src2);
	}
	else
	{
   		src1 = reg().getGPR(instr->rs1());
   		if(src1<0)
 			reg().setGPR(instr->rd(),src2);
	}   

   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczne (INSTRClass* instr) {
    WORD src1,src2;
    src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    if(src1!=0)
		reg().setGPR(instr->rd(),src2); 
   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczneq (INSTRClass* instr) {
	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = (src1 == 0)? src2:-src2;
	reg().setGPR(instr->rd(),dest);

   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcznge (INSTRClass* instr) {
   	WORD src1,src2,dest;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = ((unsigned)src1 >= 0)? src2:-src2;
	}
	else
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = (src1 >= 0)? src2:-src2;
	}    
	reg().setGPR(instr->rd(),dest);

   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMczngt (INSTRClass* instr) {
   	WORD src1,src2,dest;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = ((unsigned)src1 > 0)? src2:-src2;
	}
	else
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = (src1 > 0)? src2:-src2;
	}    
	reg().setGPR(instr->rd(),dest);

   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcznle (INSTRClass* instr) {
   	WORD src1,src2,dest;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = ((unsigned)src1 <= 0)? src2:-src2;
	}
	else
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = (src1 <= 0)? src2:-src2;
	}    
	reg().setGPR(instr->rd(),dest);

   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcznlt (INSTRClass* instr) {
    WORD src1,src2,dest;
	src2 = reg().getGPR(instr->rs2());
	if(instr->unsigned_flag()==1)	
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = ((unsigned)src1 < 0)? src2:-src2;
	}
	else
	{
		src1 = reg().getGPR(instr->rs1());
   		dest = (src1 < 0)? src2:-src2;
	}    
	reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMcznne (INSTRClass* instr) {
	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = (src1 != 0)? src2:-src2;
	reg().setGPR(instr->rd(),dest);
   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMv16 (INSTRClass* instr) {
   WORD src = reg().getGPR(instr->rs1());
   reg().setGPR(instr->rd(),src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMv16i (INSTRClass* instr) {
   WORD simm = instr->imm5();
   reg().setGPR(instr->rd(),simm);
   return pc() + instr->instrsize();  
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvfc (INSTRClass* instr) {
   UWORD cs = reg().getCTRL(instr->cs1());
   reg().setGPR(instr->rd(),cs);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvfc16 (INSTRClass* instr) {
   UWORD cs = reg().getCTRL(instr->cs1());
   reg().setGPR(instr->rd(),cs);
   return pc() + instr->instrsize();   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvtc (INSTRClass* instr) {
   UWORD src = reg().getGPR(instr->rs1());
   reg().setCTRL(instr->cd(),src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvtc16 (INSTRClass* instr) {
   UWORD src = reg().getGPR(instr->rs1());
   reg().setCTRL(instr->cd(),src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvtci (INSTRClass* instr) {
	UWORD uimm = instr->imm10();
	reg().setCTRL(instr->cd(),uimm);
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execMvupi (INSTRClass* instr) {
   WORD simm = instr->imm16();
   simm = simm << 16;
   reg().setGPR(instr->rd(),simm);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execNop (INSTRClass* instr) {
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execNop16 (INSTRClass* instr) {
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execNor (INSTRClass* instr) {
	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = ~(src1 | src2);
	reg().setGPR(instr->rd(),dest);

   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execInv16 (INSTRClass* instr) {
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = ~(src_rd | 0);
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();  
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execNori (INSTRClass* instr) {
   UINT uimm = instr->imm16() & 0xffff;
   WORD src = reg().getGPR(instr->rs1());
   WORD dest = ~(src | uimm);
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execOr (INSTRClass* instr) {
	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = src1 | src2;
	reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execOr16 (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd | src1;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execOri (INSTRClass* instr) {
   UINT uimm = instr->imm16() & 0xffff;
   WORD src = reg().getGPR(instr->rs1());
   WORD dest = src | uimm;
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execOr16i (INSTRClass* instr) {
   UINT uimm = instr->imm5() & 0x1f;
   WORD src = reg().getGPR(instr->rd());
   WORD dest = src | uimm;
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execPop16 (INSTRClass* instr) {
	ADDR ad1 = reg().getSP();
	ad1 += (instr->imm5() & 0x1f)<<2;
	WORD src = mmu().readWord(ad1);
	LOG_EXTMETA("offset", ad1)
	reg().setGPR(instr->rd(),src);
	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execPush16 (INSTRClass* instr) {
   WORD src;
   ADDR ad1 = reg().getSP();
   ad1 += (instr->imm5() & 0x1f)<<2;
   src = reg().getGPR(instr->rs0());   		
   LOG_EXTMETA("offset", ad1)
   mmu().writeWord(ad1,src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execRet (INSTRClass* instr) {
    ADDR ad = reg().getRA();
    pc(ad);
    return pc();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execRet16 (INSTRClass* instr) {
    ADDR ad = reg().getRA();
    pc(ad);
    return pc();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execRete16 (INSTRClass* instr) {
	status().event(EVENT_ERETURN);
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execRete (INSTRClass* instr) {
	status().event(EVENT_ERETURN);
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSetlt (INSTRClass* instr) {
    WORD src1,src2,dest;
   	src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());
    dest = (src1 < src2)? 1:0;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSetlti (INSTRClass* instr) {
    WORD src1 = reg().getGPR(instr->rs1());
    WORD simm = instr->imm16();
    WORD dest = (src1 < simm)? 1:0;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSetltu (INSTRClass* instr) {
    UINT src1,src2;
    WORD dest;
   	src1 = reg().getGPR(instr->rs1());
    src2 = reg().getGPR(instr->rs2());	
    dest = (src1 < src2)? 1:0;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSetltui (INSTRClass* instr) {
    UINT src1 = reg().getGPR(instr->rs1());
    UINT uimm = instr->imm16();
    WORD dest = (src1 < uimm)? 1:0;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShll (INSTRClass* instr) {
	WORD src1,dest;
	UINT src2 = reg().getGPR(instr->rs2());
   	src1 = reg().getGPR(instr->rs1());
    dest = src1 << src2;
    reg().setGPR(instr->rd(),dest);
    return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShll16 (INSTRClass* instr) {
   WORD src_rd = reg().getGPR(instr->rd());
   UINT src2 = reg().getGPR(instr->rs1());
   src_rd = src_rd << src2;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShll16i (INSTRClass* instr) {
   UINT uimm = instr->imm5();
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd << uimm;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShlli (INSTRClass* instr) {
   UINT uimm = instr->imm16();
   WORD src = reg().getGPR(instr->rs1());
   WORD dest = src << uimm;
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShra (INSTRClass* instr) {
   	WORD src1,dest;
   	UINT src2 = reg().getGPR(instr->rs2());
	src1 = reg().getGPR(instr->rs1());
   	dest = src1 >> src2;
   	reg().setGPR(instr->rd(),dest);
 	return pc() + instr->instrsize();  
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShra16 (INSTRClass* instr) {
   WORD src_rd = reg().getGPR(instr->rd());
   UINT src2 = reg().getGPR(instr->rs1());
   src_rd = src_rd >> src2;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShra16i (INSTRClass* instr){
   UINT uimm = instr->imm5();
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd >> uimm;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
   	
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShrai (INSTRClass* instr) {
   UINT uimm = instr->imm16();
   WORD src = reg().getGPR(instr->rs1());
   WORD dest = src >> uimm;
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShrl (INSTRClass* instr) {
	UINT src1,dest;
	UINT src2 = reg().getGPR(instr->rs2());
	src1 = reg().getGPR(instr->rs1());
	dest = src1 >> src2;
	reg().setGPR(instr->rd(),dest);
	return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShrl16 (INSTRClass* instr) {
   UINT src_rd = reg().getGPR(instr->rd());
   UINT src2 = reg().getGPR(instr->rs1());
   src_rd = src_rd >> src2;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShrl16i (INSTRClass* instr) {
   UINT uimm = instr->imm5();
   UINT src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd >> uimm;
   reg().setGPR(instr->rd(),src_rd);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execShrli (INSTRClass* instr) {
   UINT uimm = instr->imm16();
   UINT src = reg().getGPR(instr->rs1());
   UINT dest = src >> uimm;
   reg().setGPR(instr->rd(),dest);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execStb (INSTRClass* instr) {
   ADDR ad1 = reg().getGPR(instr->rs1()) + instr->offset();
   BYTE src = reg().getGPR(instr->rs0());
   LOG_EXTMETA("offset", ad1)
   mmu().writeByte(ad1,src);
   return pc() + instr->instrsize();   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execStc (INSTRClass* instr) {
	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSth (INSTRClass* instr) {
   ADDR ad1 = reg().getGPR(instr->rs1()) + instr->offset();
   HWORD src = reg().getGPR(instr->rs0());
   LOG_EXTMETA("offset", ad1)
   mmu().writeHword(ad1,src);
   return pc() + instr->instrsize(); 
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execStw (INSTRClass* instr) {
   ADDR ad1 = reg().getGPR(instr->rs1()) + instr->offset();
   WORD src = reg().getGPR(instr->rs0());
   LOG_EXTMETA("offset", ad1)
   mmu().writeWord(ad1,src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execStw16 (INSTRClass* instr) {
   ADDR ad1 = reg().getGPR(instr->rs1());
   WORD src = reg().getGPR(instr->rs0());
   LOG_EXTMETA("offset", ad1)
   mmu().writeWord(ad1,src);
   return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSub (INSTRClass* instr) {
   	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = src1-src2;
	reg().setGPR(instr->rd(),dest);
   	return pc() + instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSub16 (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd-src1;
   reg().setGPR(instr->rd(),src_rd);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSub16i (INSTRClass* instr) {
   UINT uimm = instr->imm5() & 0x1f;
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd-uimm;
   reg().setGPR(instr->rd(),src_rd);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execSyscall (INSTRClass* instr) {
	VRuntime<MMUClass, REGClass, INSTRClass> vrt(mmu(), reg(), status());
	UINT ret = vrt.syscall(instr->sysc_id());
	status().state(ret);
//	printf("System call invoked!");

	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execXor (INSTRClass* instr) {
	WORD src1,src2,dest;
	src1 = reg().getGPR(instr->rs1());
	src2 = reg().getGPR(instr->rs2());
	dest = src1 ^ src2;
	reg().setGPR(instr->rd(),dest);
   	return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execXor16 (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   WORD src_rd = reg().getGPR(instr->rd());
   src_rd = src_rd ^ src1;
   reg().setGPR(instr->rd(),src_rd);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execXori (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rs1());
   UINT uimm = instr->imm16() & 0xffff;
   WORD dest = src1^uimm;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execXor16i (INSTRClass* instr) {
   WORD src1 = reg().getGPR(instr->rd());
   UINT uimm = instr->imm5() & 0x1f;
   WORD dest = src1^uimm;
   reg().setGPR(instr->rd(),dest);
   return pc()+instr->instrsize();
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBr16eqz (INSTRClass* instr) {
   WORD src0 = reg().getGPR(instr->rs0());
   ADDR target = pc();
   WORD offset = (instr->offset()&0x1f)<<OFFSET_SHIFT_BR_16;
   UINT size = instr->instrsize();
   target += (src0==0)? offset:size;
   LOG_BR((src0==0),offset);  
   return target;   
}

template <class MMUClass, class REGClass, class INSTRClass>
ADDR CoreExec<MMUClass, REGClass, INSTRClass>::execBr16nez (INSTRClass* instr) {
   WORD src0 = reg().getGPR(instr->rs0());
   ADDR target = pc();
   WORD offset = (instr->offset() & 0x1f)<<OFFSET_SHIFT_BR_16;
   UINT size = instr->instrsize();
   target += (src0!=0)? offset:size;
   LOG_BR((src0!=0),offset);  
   return target;   
}

#endif /*COREEXEC_H_*/
