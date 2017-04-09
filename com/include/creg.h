/*
 *  File: creg.h
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 */
#ifndef COREREG_H_
#define COREREG_H_

#include "defs.h"
#include "cregdefs.h"
#include "status.h"
#include "regmapitem.h"
#include <string>

#ifndef _REG0_WRITE_WARN
#define _REG0_WRITE_WARN 0
#endif

#if _REG0_WRITE_WARN
#define REG0_WRITE_WARN DevWarn("Register: write to GPR 0.");
#else
#define REG0_WRITE_WARN
#endif

/*
 * Tuo: Three macros: DECLARE_REGISTER_AS_WORD, RA_JA_AS_WORD, _RAREG_TO_31 shall be removed after a while.
 * 
 */
template <class INSTRClass>
class CoreRegister {
	protected:
	ProcessStatus<INSTRClass>& _status;
	WORD _gpr[REG_GPR_SIZE];

	WORD _spBase;

	WORD _ja;
	WORD _ra;
	UWORD _loop_cnt[REG_LOOP_CUR_SIZE];
	
//FE internal registers, not access for execution unit
    ADDR _loopStartPC[REG_LOOP_CUR_SIZE];
	ADDR _loopEndPC[REG_LOOP_CUR_SIZE];
	UINT _loopExecCnt[REG_LOOP_CUR_SIZE];
	INT _loopCurIndex;
	
	protected:
	ProcessStatus<INSTRClass>& status(void) { return _status; }
	
	public:
	CoreRegister(ProcessStatus<INSTRClass>& status);
	INT createNameMap(RegMap& regmap);	// create register name map and return map size
	char* getRegName(UINT32 index, UINT32 regbase);
	char* getRegName(UINT32 norm_index);

	
	void clear();
	void clearFEReg();
	WORD getSP(void){
		return _gpr[INDEX_REG_SP];
	}
	void setSP(WORD data){
		LOG_REG_WRITE(INDEX_REG_SP, EBASE_GPR, data)
		_gpr[INDEX_REG_SP] = data;
    }	
    ADDR spBase(void) { return _spBase; }
    void spBase(ADDR s) { _spBase = s; }
	void setRetrun(WORD data) { _gpr[INDEX_REG_V0] = data; }
	
	WORD getGPR(UINT32 index) { 
		AppFatal((index<REG_GPR_SIZE), ("Register: invalid gpr index (%d) @0x%08x.", index, status().pre_pc()));
		return _gpr[index];
	}
	
	void setGPR(UINT32 index, WORD data) {
		if(index==0){
			REG0_WRITE_WARN
		 	data = 0;
		}
		AppFatal((index<REG_GPR_SIZE), ("Register: invalid gpr index (%d) @0x%08x.", index, status().pre_pc()));
		LOG_REG_WRITE(index, EBASE_GPR, data)
		_gpr[index] = data;
	}

	WORD getRA(void) { 
		return _ra;
	}	
	void setRA(WORD data) { 
		LOG_REG_WRITE(ECR_RA, EBASE_CTRL, data)
		_ra = data;
	}	
	WORD getJA(void) { 
		return _ja;
	}	
	void setJA(WORD data) { 
		LOG_REG_WRITE(ECR_JA, EBASE_CTRL, data)
		_ja = data;
	}		
	UWORD getLOOP_CNT(UINT32 index) { 
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Register: invalid loop cnt index (%d) @0x%08x.", index, status().pre_pc()));
		return _loop_cnt[index];
	}
	void setLOOP_CNT(UINT32 index, UWORD data) { 
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Register: invalid loop cnt index (%d) @0x%08x.", index, status().pre_pc()));
		LOG_REG_WRITE(index, EBASE_LOOP_CNT, data)
		_loop_cnt[index] = data;
	}	
	
	/*for loop in cexec.h */
	INT loopCurIndex(void){
		return _loopCurIndex;
	}
	void loopCurIndex(INT index) { 
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
		_loopCurIndex = index;
	}	
	UINT loopExecCnt(INT index){
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
		return _loopExecCnt[index];
	}
	void loopExecCnt(INT index,UINT count) { 
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
		_loopExecCnt[index] = count;
	}
    ADDR loopStartPC(UINT index) { 
    	AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
    	return _loopStartPC[index]; 
    }
	void loopStartPC(UINT index,ADDR addr) { 
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
		_loopStartPC[index] = addr; 
	}
	ADDR loopEndPC(UINT index) {
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index));
		return _loopEndPC[index]; 
	}
	void loopEndPC(UINT index,ADDR addr) {
		AppFatal((index<REG_LOOP_CUR_SIZE), ("Exec: Invalid loop index (%d)", index)); 
		_loopEndPC[index] = addr;
	}		
	WORD getFEReg(UINT32 index) { 
		AppFatal((index<EBASE_FE_REGS_SIZE), ("Register: invalid FE reg index (%d) @0x%08x.", index, status().pre_pc()));
		if(index>=0&&index<REG_LOOP_CUR_SIZE) {
			return loopExecCnt(index);
		}
		else if(index>=REG_LOOP_CUR_SIZE&&index<(REG_LOOP_CUR_SIZE*2)) {
			return loopStartPC(index-REG_LOOP_CUR_SIZE);
		}
		else if(index>=(REG_LOOP_CUR_SIZE*2)&&index<EBASE_FE_REGS_SIZE-1) {
			return loopEndPC(index-(REG_LOOP_CUR_SIZE*2));
		}		
		else if(index==EBASE_FE_REGS_SIZE-1) {
			return loopCurIndex();
		}
		else {
			AppFatal((0), ("Register: invalid FE reg index (%d) @0x%08x.", index, status().pre_pc()));
		}
		return -1;
	}
	void setFEReg(UINT32 index, WORD data){ 
		AppFatal((index<EBASE_FE_REGS_SIZE), ("Register: invalid FE reg index (%d) @0x%08x.", index, status().pre_pc()));
		if(index>=0&&index<REG_LOOP_CUR_SIZE) {
			loopExecCnt(index, data);
		}
		else if(index>=REG_LOOP_CUR_SIZE&&index<(REG_LOOP_CUR_SIZE*2)) {
			loopStartPC(index-REG_LOOP_CUR_SIZE, data);
		}
		else if(index>=(REG_LOOP_CUR_SIZE*2)&&index<EBASE_FE_REGS_SIZE-1) {
			loopEndPC(index-(REG_LOOP_CUR_SIZE*2), data);
		}		
		else if(index==EBASE_FE_REGS_SIZE-1) {
			loopCurIndex(data);
		}
		else {
			AppFatal((0), ("Register: invalid FE reg index (%d) @0x%08x.", index, status().pre_pc()));
		}
	}
	
	WORD getCTRL(UINT32 index) { 
		AppFatal((index<REG_ALL_CTRL_SIZE), ("Register: invalid ctrl reg index (%d) @0x%08x.", index, status().pre_pc()));
		if(index==ECR_JA ) {
			return getJA();
		}
		else if(index==ECR_RA ) {
			return getRA();
		}		
		else if(index>=ECR_LOOP_CNT&&index<ECR_INSTR_CNT) {
			return getLOOP_CNT(index-ECR_LOOP_CNT);
		}
		else {
			AppFatal((0), ("Register: invalid ctrl reg index (%d) @0x%08x.", index, status().pre_pc()));
		}
		return -1;
	}
	void setCTRL(UINT32 index, WORD data) { 
		AppFatal((index<REG_ALL_CTRL_SIZE), ("Register: invalid ctrl reg index (%d) @0x%08x.", index, status().pre_pc()));
		if(index==ECR_JA ) {
			setJA(data);
		}
		else if(index==ECR_RA ) {
			setRA(data);
		}		
		else if(index>=ECR_LOOP_CNT&&index<ECR_INSTR_CNT) {
			setLOOP_CNT(index-ECR_LOOP_CNT, data);
		}
		else {
			AppFatal((0), ("Register: invalid ctrl reg index (%d) @0x%08x.", index, status().pre_pc()));
		}
	}

	INT getRegIndexByName(const STRING regName);
	INT setRegByName(const STRING regName, DWORD value);
	INT getRegisterNames(char *buf, int len);
	bool isValidRegName(const STRING regName);
//	INT getRegWidth(void) { return (_mode==ERM_32)?INT32_HEX_WIDTH:INT16_HEX_WIDTH; }
//	INT getRegMask(void) { return (_mode==ERM_32)?WORD_DATA_MASK:HWORD_DATA_MASK; }
	void dumpRegs(FILE* out, const char* regSetName);	
	void dumpRegs2File(FILE* out, const char* regSetName);	
	INT initRegs(FILE* in,  const char* regSetName);
};


const STRING reg_name_gpr[REG_GPR_SIZE] = {
	"zero", "r1", "r2", "r3", "r4", "r5", "r6", "r7", 
	"r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", 
	"r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", 
	"r24", "r25", "r26", "r27", "gp", "sp", "r30", "r31"
};

const STRING reg_name_ctrl[REG_ALL_CTRL_SIZE] = {
	"ja", "ra", 
	"lcnt0", "lcnt1", "lcnt2", "lcnt3",
};

const STRING reg_name_fe[EBASE_FE_REGS_SIZE] = {
	"felcnt0", "felcnt1", "felcnt2", "felcnt3",
	"felstart0", "felstart1", "felstart2", "felstart3",
	"felend0", "felend1", "felend2", "felend3",
	"felcuridx",
};

template<class INSTRClass>
CoreRegister<INSTRClass>::CoreRegister(ProcessStatus<INSTRClass>& status) : _status(status) {
	clear();
}

template <class INSTRClass>
INT CoreRegister<INSTRClass>::initRegs(FILE* in, const char* regSetName) {
	INT line = 0;
	if(strcasecmp(regSetName, "all")==0||strcasecmp(regSetName, "core")==0) {
		WORD data = 0;
		char str[50];
		INT i;

		for(i = 0; i<REG_GPR_SIZE; i++) {
			line++;
			if(fscanf(in, "%x%[^\n]", &data, str)!=EOF) {
				setGPR(i, data);
			}
			else {
				return (-line);
			}
		}

		line++;
		if(fscanf(in, "%x%[^\n]", &data, str)!=EOF) {
			setJA(data);
		}
		else {
			return (-line);
		}	
		line++;
		if(fscanf(in, "%x%[^\n]", &data, str)!=EOF) {
			setRA(data);
		}
		else {
			return (-line);
		}			
		for(i = 0; i<REG_LOOP_CUR_SIZE; i++) {
			line++;
			if(fscanf(in, "%x%[^\n]", &data, str)!=EOF) {
				setLOOP_CNT(i, data);
			}
			else {
				return (-line);
			}	
		}	
	}
	return line;	
}

/*
 * We don't check register name length. So we have to make sure register name length must be < REG_NAME_SIZE.
 * Register table layout:
 * 		gpr 0.31
 * 		hi 32
 * 		lo 33
 * 		ja 34
 * 		ra 35
 * 		loop_cnt 36..43
 * 		loop_offset 44..51
 * 		loop_cur	52..59
 * 		acc			60..63
 * 		ar			68..71
 */

template <class INSTRClass> 
INT CoreRegister<INSTRClass>::createNameMap(RegMap& regmap)
{
	INT i;
	for (i=0; i<REG_GPR_SIZE; i++) {
		regmap[reg_name_gpr[i]] = new RegMapItem(reg_name_gpr[i], WORD_BYTE, (BYTE*) &_gpr[i]);
	}

	regmap[reg_name_ctrl[0]] = new RegMapItem(reg_name_ctrl[0], WORD_BYTE, (BYTE*) &_ja);
	regmap[reg_name_ctrl[1]] = new RegMapItem(reg_name_ctrl[1], WORD_BYTE, (BYTE*) &_ra);

	for(i = 0; i<REG_LOOP_CUR_SIZE; i++) {
		regmap[reg_name_ctrl[i+REG_CTRL_SIZE]] = new RegMapItem(reg_name_ctrl[i+REG_CTRL_SIZE], WORD_BYTE, (BYTE*) &_loop_cnt[i]);
		regmap[reg_name_fe[i]] = new RegMapItem(reg_name_fe[i], WORD_BYTE, (BYTE*) &_loopStartPC[i]);
		regmap[reg_name_fe[i+REG_LOOP_CUR_SIZE]] = new RegMapItem(reg_name_fe[i+REG_LOOP_CUR_SIZE], WORD_BYTE, (BYTE*) &_loopEndPC[i]);
		regmap[reg_name_fe[i+(2*REG_LOOP_CUR_SIZE)]] = new RegMapItem(reg_name_fe[i+(2*REG_LOOP_CUR_SIZE)], WORD_BYTE, (BYTE*) &_loopExecCnt[i]);
	}
	regmap[reg_name_fe[(3*REG_LOOP_CUR_SIZE)]] = new RegMapItem(reg_name_fe[(3*REG_LOOP_CUR_SIZE)], WORD_BYTE, (BYTE*) &_loopCurIndex);

	// acc
	// ar.
	return regmap.size();
}

template <class INSTRClass> 
void CoreRegister<INSTRClass>::clear() {
	INT i;
	for(i = 0; i<REG_GPR_SIZE; i++) {
		 _gpr[i] = 0;
	}
	_spBase = 0xffffffff;
	_ja = 0;
	_ra = 0;
	for(i = 0; i<REG_LOOP_CUR_SIZE; i++) {
		 _loop_cnt[i] = 0;
	}
	clearFEReg();
}

template <class INSTRClass> 
void CoreRegister<INSTRClass>::clearFEReg() {
	_loopCurIndex = -1;	
	for(INT i = 0; i<REG_LOOP_CUR_SIZE; i++) {
		 _loopStartPC[i] = 0;
		 _loopEndPC[i] = 0;
		 _loopExecCnt[i] = 0;
	}		
}

/*
 * For shell.
 * Very inefficient. Need to improve later.
 * Overriding method must call parent first.
 * Return -1 if no register found, 0 if set. Overriding method could use
 * this to check.
 * Only support integer register
 */
template <class INSTRClass> 
INT CoreRegister<INSTRClass>::setRegByName(const STRING regName, const DWORD value)
{
	INT i;
	// gpr
	for (i=0; i<REG_GPR_SIZE; i++) {
		if (strcmp(reg_name_gpr[i], regName) == 0) {
			setGPR(i, value);
			return 0;
		}
	}
	// ctrl
	for (i=0; i<REG_ALL_CTRL_SIZE; i++) {
		if (strcmp(reg_name_ctrl[i], regName) == 0) {
			setCTRL(i, value);
			return 0;
		}
	}
	// fe_internal
	for (i=0; i<EBASE_FE_REGS_SIZE; i++) {
		if (strcmp(reg_name_fe[i], regName) == 0) {
			setFEReg(i, value);
			return 0;
		}
	}	
	return -1;	
}

template <class INSTRClass> 
bool CoreRegister<INSTRClass>::isValidRegName(const STRING regName)
{
	INT i;
	// gpr
	for (i=0; i<REG_GPR_SIZE; i++) {
		if (strcmp(reg_name_gpr[i], regName) == 0) {
			return true;
		}
	}
	// ctrl
	for (i=0; i<REG_ALL_CTRL_SIZE; i++) {
		if (strcmp(reg_name_ctrl[i], regName) == 0) {
			return true;
		}
	}
	//fe
	for (i=0; i<EBASE_FE_REGS_SIZE; i++) {
		if (strcmp(reg_name_fe[i], regName) == 0) {
			return true;
		}
	}	
	return false;	
}

template <class INSTRClass>
void CoreRegister<INSTRClass>::dumpRegs2File(FILE* out, const char* regSetName) {
	if(strcasecmp(regSetName, "all")==0||strcasecmp(regSetName, "core")==0) {
		for (INT i=0; i<REG_GPR_SIZE; i++) {
			fprintf(out, "%0*x \\\\ %s\n", 8, getGPR(i), reg_name_gpr[i]);
		}
		fprintf(out, "%0*x \\\\ %s\n", 8/*getRegWidth()*/, getJA(), reg_name_ctrl[ECR_JA]);
		fprintf(out, "%0*x \\\\ %s\n", 8/*getRegWidth()*/, getRA(), reg_name_ctrl[ECR_RA]);
		for (INT i=0; i<REG_LOOP_CUR_SIZE; i++) {
			fprintf(out, "%0*x \\\\ %s\n", 8/*getRegWidth()*/, getLOOP_CNT(i), reg_name_ctrl[i+REG_CTRL_SIZE]);
		}	
		for (INT i=0; i<EBASE_FE_REGS_SIZE; i++) {
			fprintf(out, "%0*x \\\\ %s\n", 8/*getRegWidth()*/, getFEReg(i), reg_name_fe[i]);
		}
	}	
} 

template <class INSTRClass> 
INT CoreRegister<INSTRClass>::getRegisterNames(char *buf, int len)
{
	int cc = strlen(buf);
	for (INT i=0; i<REG_GPR_SIZE; i++) {
		cc += sprintf(buf+cc, "%s ", reg_name_gpr[i]);
		if (i % 8 == 7)
			cc += sprintf(buf+cc, "\n");
	}
//	cc += sprintf(buf+cc, "%s ", reg_name_ctrl[ECR_HI]);
	cc += sprintf(buf+cc, "%s ", reg_name_ctrl[ECR_JA]);
	cc += sprintf(buf+cc, "%s ", reg_name_ctrl[ECR_RA]);
	cc += sprintf(buf+cc, "\n");
	for (INT i=0; i<REG_LOOP_CUR_SIZE; i++) {
		cc += sprintf(buf+cc, "%s ", reg_name_ctrl[i+REG_CTRL_SIZE]);
	}
	for (INT i=0; i<EBASE_FE_REGS_SIZE; i++) {
		cc += sprintf(buf+cc, "%s ", reg_name_fe[i]);
	}	
	buf[cc] = '\0';
	if (cc >= len)
		return -1;
	return 0;	
}

template <class INSTRClass> 
void CoreRegister<INSTRClass>::dumpRegs(FILE* out, const char* regSetName)
{
	if(strcasecmp(regSetName, "all")==0||strcasecmp(regSetName, "core")) {
		for (INT i=0; i<REG_GPR_SIZE; i++) {
			fprintf(out, "%5s=%0*x ", reg_name_gpr[i], 8, getGPR(i));
			if (i%REG_PRINT_PER_LINE == (REG_PRINT_PER_LINE-1))
				fprintf(out, "\n");
		}
	
		// register ja
		fprintf(out, "%5s=%0*x ", reg_name_ctrl[ECR_JA], 8, getJA());
		
		// register ra
		fprintf(out, "%5s=%0*x ", reg_name_ctrl[ECR_RA], 8, getRA());
		fprintf(out, "\n");
		for (INT i=0; i<REG_LOOP_CUR_SIZE; i++) {
			fprintf(out, "%5s=%0*x ", reg_name_ctrl[i+REG_CTRL_SIZE], 8/*getRegWidth()*/, getLOOP_CNT(i));
		}	
		fprintf(out, "\nFE internal Reg:\n");
		for (INT i=0; i<EBASE_FE_REGS_SIZE; i++) {
			fprintf(out, "%9s=%08x ", reg_name_fe[i], getFEReg(i));
			if (i%REG_LOOP_CUR_SIZE == (REG_LOOP_CUR_SIZE-1))
				fprintf(out, "\n");			
		}	
		fprintf(out, "\n");	
	}
}


template <class INSTRClass> 
INT CoreRegister<INSTRClass>::getRegIndexByName(const STRING regName){
	int i;
	for (i=0; i<REG_GPR_SIZE; i++) {
		if (strcmp(reg_name_gpr[i], regName)==0)
			return i;
	}
	for (i=0; i<REG_ALL_CTRL_SIZE; i++) {
		if (strcmp(reg_name_ctrl[i], regName)==0)
			return i;
	}	
	for (i=0; i<EBASE_FE_REGS_SIZE; i++) {
		if (strcmp(reg_name_fe[i], regName)==0)
			return i;
	}		
	return -1;	// not found
}



#endif /*COREREG_H_*/
