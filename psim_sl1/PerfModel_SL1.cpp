/* /////////////////////////////// "PerfModel_SL1.cpp" /////////////////////////////////
 *  File: PerfModel_SL1.cpp
 * 
 *  Descriptions:   Implementation SL1-dependent methods for simulation
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
 
#include "defs.h"
#include "cregdefs.h"
#include "sl1regdefs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"
#include "PerfModel_SL1.h"
#include "PerfGlobalOpt.h"
#include <string.h>

using namespace PSIM_SL1;

INSTR_LATENCY_ITEM CPerfModelSL1::SL1InstrLatencyTable[] = {
	{EIG_gr,     DEFAULT_INSTR_STAGES},
	{EIG_alu,    DEFAULT_INSTR_STAGES},
	{EIG_alu16,  DEFAULT_INSTR_STAGES},
	{EIG_ls,     DEFAULT_INSTR_STAGES},
	{EIG_ls16,   DEFAULT_INSTR_STAGES},
	{EIG_br,     DEFAULT_INSTR_STAGES},
	{EIG_br16,   DEFAULT_INSTR_STAGES},
	{EIG_j,      DEFAULT_INSTR_STAGES},
	{EIG_j16,    DEFAULT_INSTR_STAGES},	
	{EIG_c3_mac, DEFAULT_INSTR_STAGES},
	{EIG_c3_ffe, DEFAULT_INSTR_STAGES},
	{EIG_c3_viterbi,DEFAULT_INSTR_STAGES},
	{EIG_c3_traceback,DEFAULT_INSTR_STAGES},
	{EIG_misc,   DEFAULT_INSTR_STAGES},
};

typedef struct {
	UINT rscAttr;	// No use
	char rscName[SL1_MAX_RSC_NAME];
	char rscTypes[SL1_RSC_MAX_TYPES][SL1_MAX_RSC_TYPE_NAME];
}RSC_ITEM;

// Original execution units
RSC_ITEM SL1RscTable[SL1_MAX_RSC] = {	
#ifndef MULTI_THREAD
    {RISC_LS,     "LS",     {"ls"}},
    {RISC_ALU_BR, "ALU",    {"alu", "br"}},
	{RISC_ALU_LS, "ALU_LS", {"alu", "ls", "br"}},	
#else	
	{RISC_ALU_BR0, "ALU",    {"alu", "br"}},
	{RISC_ALU_BR1, "ALU1",    {"alu", "br"}},
	{RISC_ALU_BR2, "ALU2",    {"alu","br"}},
	//{RISC_ALU_BR3, "ALU3",    {"alu","br"}},
	//{RISC_ALU_LS0, "ALU_LS0", {"alu", "ls", "br"}},
	//{RISC_ALU_LS1, "ALU_LS1", {"alu", "ls", "br"}},	
	{RISC_LS0, "LS0", {"ls"}},
	{RISC_LS1, "LS1", {"ls"}},
#endif	
	{CISC_MAC0,   "MAC0",   {"mac"}},
	//{CISC_MAC1,   "MAC1",   {"mac"}},
#ifdef MULTI_THREAD
	{CISC_MAC1,   "MAC1",   {"mac"}},
	//{CISC_MAC2,   "CISC_MAC1",   {"mac"}},
#endif
	{CISC_FFT,    "FFT",    {"fft"}},
	{CISC_VITERBI, "VITERBI", {"viterbi"}},
	{CISC_TRACEBACK, "TRACEBACK", {"traceback"}},
	{EXEC_UNIT_UNDEFINED}
};

// Separate LS from ALU_LS
RSC_ITEM SL1RscTable_1[SL1_MAX_RSC] = {
// case 2: separate LS
#ifndef MULTI_THREAD
	{RISC_ALU_BR, "ALU_BR0",    {"alu", "br"}},
	{RISC_ALU_LS, "ALU_BR1",    {"alu", "br"}},
#else
    {RISC_ALU_BR0, "ALU_BR0",    {"alu", "br"}},
	{RISC_ALU_LS0, "ALU_BR1",    {"alu", "br"}},	
#endif
	{LS0, "LS0", {"ls"}},
	{LS1, "LS1", {"ls"}},
	{CISC_MAC0,   "MAC0",   {"mac"}},
	{CISC_MAC1,   "MAC1",   {"mac"}},
	{CISC_FFT,    "FFT",    {"fft"}},
	{EXEC_UNIT_UNDEFINED}
};

// Separate BR from ALU
RSC_ITEM SL1RscTable_2[SL1_MAX_RSC] = {
	{RISC_LS,     "LS",     {"ls"}},
#ifndef MULTI_THREAD	
	{RISC_ALU_BR, "ALU",    {"alu"}},
	{RISC_ALU_LS, "ALU_LS", {"alu", "ls"}},
#else
    {RISC_ALU_BR0, "ALU",    {"alu"}},
	{RISC_ALU_LS0, "ALU_LS", {"alu", "ls"}},
#endif	
	{BR0, "BR0", {"br"}},
	{BR1, "BR1", {"br"}},
	{CISC_MAC0,   "MAC0",   {"mac"}},
//	{CISC_MAC1,   "CISC_MAC1",   {"mac"}},
	{CISC_FFT,    "FFT",    {"fft"}},
	{EXEC_UNIT_UNDEFINED}
};

// Separate both LS and BR from ALU
RSC_ITEM SL1RscTable_3[SL1_MAX_RSC] = {
// case 3: separate BR
#ifndef MULTI_THREAD
    {RISC_ALU_BR, "ALU",    {"alu"}},
	{RISC_ALU_LS, "ALU_LS", {"alu"}},
#else
	{RISC_ALU_BR0, "ALU",    {"alu"}},
	{RISC_ALU_LS0, "ALU_LS", {"alu"}},
#endif	
	{LS0, "LS0", {"ls"}},
	{LS1, "LS1", {"ls"}},
	{BR0, "BR0", {"br"}},
	{BR1, "BR1", {"br"}},
	{CISC_MAC0,   "MAC0",   {"mac"}},
//	{CISC_MAC1,   "CISC_MAC1",   {"mac"}},
	{CISC_FFT,    "FFT",    {"fft"}},
	{EXEC_UNIT_UNDEFINED}
};

/*
 * Assume one instruction group needs only one execution resource
 */
typedef struct {
	UINT eig;
	char *eigName;
	UINT nResources;	// number of resources can be used for this instruction group
	UINT rscTbl[8];	// Index to SL1RscTable, resource allocation order is from the head
	char rscTypeRequired[10];	// case insensitive
}INSTR_RSC_MAP_ITEM;

// The table will be rebuilt by looking up SL1RscTable
INSTR_RSC_MAP_ITEM SL1InstrRscMap[] = {
	{EIG_gr,     "EIG_gr",     0, {0}, "null"},
	{EIG_alu,    "EIG_alu",    0, {0}, "alu"},
	{EIG_alu16,  "EIG_alu16",  0, {0}, "alu"},
	{EIG_ls,     "EIG_ls",     0, {0}, "ls"},
	{EIG_ls16,   "EIG_ls16",   0, {0}, "ls"},
	{EIG_br,     "EIG_br",     0, {0}, "br"},
	{EIG_br16,   "EIG_br16",   0, {0}, "br"},
	{EIG_j,      "EIG_j",      0, {0}, "br"},
	{EIG_j16,    "EIG_j16",    0, {0}, "br"},	
	{EIG_c3_mac, "EIG_c3_mac", 0, {0}, "mac"},
	{EIG_c3_ffe, "EIG_c3_ffe", 0, {0}, "fft"},
	{EIG_c3_viterbi,"EIG_c3_viterbi", 0, {0}, "viterbi"},
	{EIG_c3_traceback,"EIG_c3_traceback", 0, {0}, "traceback"},
	{EIG_misc,   "EIG_misc",   0, {0}, "null"},	// dcflush, icflush: They don't need any resource
};

#define SL1_INSTR_RSC_MAP_LENGTH (sizeof(SL1InstrRscMap) / sizeof(INSTR_RSC_MAP_ITEM))

// different threads may use different resource
INSTR_RSC_MAP_ITEM SL1InstrRscMapList[SL1_MAX_THREADS][SL1_INSTR_RSC_MAP_LENGTH];

HAZARD_STATUS_ITEM CPerfModelSL1::SL1HazardStatusNameMap[] = {
	{ HS_UNDEFINED,        "HS_UNDEFINED"},
	{ HS_NONE,             "HS_NONE"},
	{ HS_ENGINE_NOT_READY, "HS_ENGINE_NOT_READY"},
	{ HS_ENGINE_FULL,      "HS_ENGINE_FULL"},
	{ HS_INSTR_ORDER,      "HS_INSTR_ORDER"},
	{ HS_BR_STALL,         "HS_BR_STALL"},
	{ HS_READ_PORT_JAM,    "READ_PORT_JAM"},
	{ HS_WRITE_PORT_JAM,   "WRITE_PORT_JAM"},
	{ HS_REG_WAW,          "HS_REG_WAW"},
	{ HS_REG_RAW,          "HS_REG_RAW"},
	{ HS_REG_WAR,          "HS_REG_WAR"},
	{ HS_IWIN_FULL,        "HS_IWIN_FULL"},
	{ HS_MAX_ISSUE,        "HS_MAX_ISSUE"},
};

#ifndef MULTI_THREAD
STRING CPerfModelSL1::threadNames[SL1_MAX_THREADS] = {
	"Core",	"BB"
};
#else
STRING CPerfModelSL1::threadNames[SL1_MAX_THREADS] = {
	"thread0", "thread1", "thread2", "thread3"
};
#endif
STRING CPerfModelSL1::stageNames[] = {
	"IDLE", // PS_UNDEF
	"FE  ",	// PS_FE = 1
	"ID  ",	// PS_ID = 2
	"EX  ",	// PS_ID = 3
	"MEM ",	// PS_MEM = 4
	"WB  "	// PS_WB  = 5
};

UINT CPerfModelSL1::_lastRetiredId[SL1_MAX_THREADS];
UINT CPerfModelSL1::_euHazardCnt[SL1_MAX_THREADS][EIG_max];

#define CHECK_RESOURCE_MASK(mask, rscindex)  (mask&(1<<rscindex))

/*
 *	Can't be called twice.
 */
int CPerfModelSL1::initInstrRscMap(void)
{
	int i, j, k, cc;
	// select execution units type
	printf("eu set type:%d\n", PSIM_SL1::g_euSetType);
	IsTrue( ((PSIM_SL1::g_euSetType>=0) || (PSIM_SL1::g_euSetType<=EUSET_SEPARATED_ALL)) ,
		("Invalid execution unit set specified: %d. It must be one of 0, 1, 2 or 3.\n",
			PSIM_SL1::g_euSetType));
	switch (PSIM_SL1::g_euSetType) {
		case EUSET_SEPARATED_LS:
			memcpy(SL1RscTable, SL1RscTable_1, sizeof(SL1RscTable_1));
		break;
		case EUSET_SEPARATED_BR:
			memcpy(SL1RscTable, SL1RscTable_2, sizeof(SL1RscTable_2));
		break;
		case EUSET_SEPARATED_ALL:
			memcpy(SL1RscTable, SL1RscTable_3, sizeof(SL1RscTable_3));
		break;
		case EUSET_ORIGINAL:
		break;
		case EUSET_NONE:
			SL1RscTable[0].rscAttr = EXEC_UNIT_UNDEFINED;
		break;
		default:
			IsTrue((0), ("Impossible\n"));
		break;
	}
	// Check if resource is added from commandline
	if (g_execResources[0]) {
		for (j=0; SL1RscTable[j].rscAttr != EXEC_UNIT_UNDEFINED && j<SL1_MAX_RSC; j++) ;
		char *p = strtok(g_execResources,";");
		while (p) {
			if (j>=SL1_MAX_RSC-1) {
				fprintf(stderr, "Can't add execution resource due to too many execution resources already in table:%d\n", j);
				exit(-1);
			}
			char *q = strchr(p, ',');
			if (!q) {
				fprintf(stderr, "Invalid parameter for execution resource:%s\n", p);
				exit(-1);
			}
			if ((q-p >= SL1_MAX_RSC_NAME) || (q-p==1)) {
				*q = '\0';
				fprintf(stderr, "Invalid resource name:%s\n", p);
				exit(-1);
			}
			strncpy(SL1RscTable[j].rscName, p, q-p); 
			SL1RscTable[j].rscName[q-p] = '\0';
			
			int rscTypeNum = 0;
			char *r=q+1;
			while(1) {
				q = r;
				r = strchr(q, ',');
				if (r) {
					*r = '\0';
					r++;
				}
				if ((strlen(q)==0)) {
					break;
				}
				if (strlen(q) > SL1_MAX_RSC_TYPE_NAME-1 ) {
					fprintf(stderr, "Resource type name is too long:%s\n", q);
					exit(-1);
				}
				printf("%d resouce type %s\n", rscTypeNum, q);
				strcpy(SL1RscTable[j].rscTypes[rscTypeNum], q);
				SL1RscTable[j].rscTypes[rscTypeNum+1][0] = '\0';	// Truncate

				rscTypeNum ++;
				IsTrue((rscTypeNum<SL1_RSC_MAX_TYPES),("Too many resource types for %s\n", SL1RscTable[j].rscName)); 
				if (!r) {
					break;
				}
			}
			if (!rscTypeNum) {
				fprintf(stderr, "Resource type name missing:%s\n", p);
				exit(-1);
			}
			
			SL1RscTable[j].rscAttr = EXEC_UNIT_ADDED;
			SL1RscTable[j+1].rscAttr = EXEC_UNIT_UNDEFINED;
			j++;
			p = strtok(NULL, ";");
		}
	}
//	exit(0);
	
	// rebuild the indices from instr grp to exec unit
	for (i=0; i<=EIG_misc; i++) {
		cc=0;
		for (j=0; SL1RscTable[j].rscAttr != EXEC_UNIT_UNDEFINED && j<SL1_MAX_RSC; j++) {
			for (k=0; k<SL1_RSC_MAX_TYPES && SL1RscTable[j].rscTypes[k][0]; k++) {
				if (strcasecmp(SL1RscTable[j].rscTypes[k], SL1InstrRscMap[i].rscTypeRequired) == 0) {
					SL1InstrRscMap[i].rscTbl[cc++] = j;
					break;
				}
			}
		}
		SL1InstrRscMap[i].nResources = cc;
	}
	for (j=0; SL1RscTable[j].rscAttr != EXEC_UNIT_UNDEFINED && j<SL1_MAX_RSC; j++);
	_execUnitNumber = j;
	
	// Get the resource mask and build the instrgrp-to-resource map for each thread
	unsigned umask[SL1_MAX_THREADS];
#ifndef MULTI_THREAD
	umask[THREAD_ID_CORE] = PSIM_SL1::g_coreRscMask;
	umask[THREAD_ID_BB] = PSIM_SL1::g_bbRscMask;
	printf("resource mask:core-%#08x BB-%#08x\n", umask[THREAD_ID_CORE], umask[THREAD_ID_BB]);
#else	
	umask[THREAD_ID_0] = PSIM_SL1::g_thread0RscMask;
	umask[THREAD_ID_1] = PSIM_SL1::g_thread1RscMask;
	umask[THREAD_ID_2] = PSIM_SL1::g_thread2RscMask;
	umask[THREAD_ID_3] = PSIM_SL1::g_thread3RscMask;	
	printf("resource mask:thread0-%#08x thread1-%#08x thread2-%#08x thread3-%#08x\n", 
	        umask[THREAD_ID_0], umask[THREAD_ID_1], umask[THREAD_ID_2], umask[THREAD_ID_3]);
#endif	        
	for (i=0; i<SL1_MAX_THREADS; i++) {
		memcpy(SL1InstrRscMapList[i], SL1InstrRscMap, sizeof(SL1InstrRscMap));
		for (j=0; j<=EIG_misc; j++) {
			int m=0; // point to start of resource indices table
			for (k=0, m=0; k<SL1InstrRscMapList[i][j].nResources; k++) {
				if (CHECK_RESOURCE_MASK(umask[i], SL1InstrRscMapList[i][j].rscTbl[k])) {
					SL1InstrRscMapList[i][j].rscTbl[m++] = SL1InstrRscMapList[i][j].rscTbl[k];
				}
			}
			SL1InstrRscMapList[i][j].nResources = m;
		}
	}
	#ifdef NO_ACC_CROSS
	for(j=0;j<SL1_MAX_THREADS;j++)
		for(i=0;i<REG_ACC_CUR_SIZE;i++)
		{
			_pipe_acc[j][i] = false;
			_pipe_acc_eu[j][i] = EXEC_UNIT_UNDEFINED;
		}
	#endif
	return 0;
}

CPerfModelSL1::CPerfModelSL1()
{
	_prevInstrItem = NULL;
	_execUnitNumber = 0;
	
	// Assume tables can be accessed by using instruction group as array index
	UINT i, j;
	for (i=0; i<sizeof(SL1InstrLatencyTable)/sizeof(INSTR_LATENCY_ITEM); i++) {
		IsTrue((i==SL1InstrLatencyTable[i].eig), ("SL1InstrLatencyTable: incompatible table\n"));
	}
	for (i=0; i<sizeof(SL1InstrRscMap)/sizeof(INSTR_RSC_MAP_ITEM); i++) {
		IsTrue((i==SL1InstrRscMap[i].eig), ("SL1InstrRscMap: incompatible table\n"));
	}
	for (i=0; i<sizeof(SL1HazardStatusNameMap)/sizeof(HAZARD_STATUS_ITEM); i++) {
		IsTrue((i==SL1HazardStatusNameMap[i].hazardStatus+1), ("SL1HazardStatusNameMap: incompatible table\n"));
	}
	// check if the instruction group has been changed
	IsTrue(((sizeof(SL1InstrRscMap) / sizeof(INSTR_RSC_MAP_ITEM)) == EIG_misc+1), ("Invalid instruction group\n"));
	for (i=0; i<=EIG_misc; i++) {
		IsTrue((SL1InstrRscMap[i].eig == i), 
			("Instruction group changed. You need to reconstruct the table 'SL1InstrRscMap'\n"));
	}
	
	memset(_execUnitUsageCnt, 0, sizeof(_execUnitUsageCnt));
	
	memset(_regSetInfo, 0, sizeof(_regSetInfo));
	// Default to set ports for all register files to don't care
	for (i=0; i<SL1_MAX_THREADS; i++) {
		for (j=0; j<REGCLASS_MAX; j++) {
			_regSetInfo[i]._maxReadPorts[j] = DEFAULT_DONT_CARE_PORT;
			_regSetInfo[i]._maxWritePorts[j] = DEFAULT_DONT_CARE_PORT;
		}
	}
	
	// max ports for core gprs
#ifndef MULTI_THREAD
    if (g_maxGprReadPorts[THREAD_ID_CORE] == 0)
		_regSetInfo[THREAD_ID_CORE]._maxReadPorts[REGCLASS_GPR] = MAX_CORE_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_CORE]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_CORE];
	
	if (g_maxGprWritePorts[THREAD_ID_CORE] == 0)
		_regSetInfo[THREAD_ID_CORE]._maxWritePorts[REGCLASS_GPR] = MAX_CORE_GPR_WRITE_PORTS;
	else
		_regSetInfo[THREAD_ID_CORE]._maxWritePorts[REGCLASS_GPR] = g_maxGprWritePorts[THREAD_ID_CORE];
	
	// max ports for bb gprs
	if (g_maxGprReadPorts[THREAD_ID_BB] == 0)
		_regSetInfo[THREAD_ID_BB]._maxReadPorts[REGCLASS_GPR]  = MAX_BB_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_BB]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_BB];
	
	if (g_maxGprWritePorts[THREAD_ID_BB] == 0)
		_regSetInfo[THREAD_ID_BB]._maxWritePorts[REGCLASS_GPR] = MAX_BB_GPR_WRITE_PORTS;
	else
		_regSetInfo[THREAD_ID_BB]._maxWritePorts[REGCLASS_GPR] = g_maxGprWritePorts[THREAD_ID_BB];
#else	
	if (g_maxGprReadPorts[THREAD_ID_0] == 0)
		_regSetInfo[THREAD_ID_0]._maxReadPorts[REGCLASS_GPR] = MAX_THREAD0_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_0]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_0];
	
	if (g_maxGprWritePorts[THREAD_ID_0] == 0)
		_regSetInfo[THREAD_ID_0]._maxWritePorts[REGCLASS_GPR] = MAX_THREAD0_GPR_WRITE_PORTS;
	else
		_regSetInfo[THREAD_ID_0]._maxWritePorts[REGCLASS_GPR] = g_maxGprWritePorts[THREAD_ID_0];
	
	if (g_maxGprReadPorts[THREAD_ID_1] == 0)
		_regSetInfo[THREAD_ID_1]._maxReadPorts[REGCLASS_GPR]  = MAX_THREAD1_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_1]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_1];
	
	if (g_maxGprWritePorts[THREAD_ID_1] == 0)
		_regSetInfo[THREAD_ID_1]._maxWritePorts[REGCLASS_GPR] = MAX_THREAD1_GPR_WRITE_PORTS;
	else
		_regSetInfo[THREAD_ID_1]._maxWritePorts[REGCLASS_GPR] = g_maxGprWritePorts[THREAD_ID_1];
		
    if (g_maxGprReadPorts[THREAD_ID_2] == 0)
		_regSetInfo[THREAD_ID_2]._maxReadPorts[REGCLASS_GPR]  = MAX_THREAD2_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_2]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_2];
		
	if (g_maxGprReadPorts[THREAD_ID_3] == 0)
		_regSetInfo[THREAD_ID_3]._maxReadPorts[REGCLASS_GPR]  = MAX_THREAD3_GPR_READ_PORTS;
	else
		_regSetInfo[THREAD_ID_3]._maxReadPorts[REGCLASS_GPR]  = g_maxGprReadPorts[THREAD_ID_3];	
#endif
	initInstrRscMap();

	for (i=0; i<SL1_MAX_RSC; i++) {
		int j;
		for (j=0; j<PS_LAST; j++)
			_execUnitStatus[i][j] = PS_UNDEF;
	}

	for (i=0; i<SL1_MAX_THREADS; i++) {
		_pipeLineLatency[i] = 0;
	}
	_fixPipeLineLatency = 0;
	_fftLatency = 0;
	_viterbiLatency = 0;
	_tracebackLatency = 0;
	_fftActive = false;
	_viterbiActive = false;
	_tracebackActive = false;
	for(i=0; i<SL1_MAX_THREADS; i++)
	{
		_fftDes[i] = 0;
		_viterbiDes[i] = 0;
		_tracebackDes[i] = 0;	
	}
	for (i=0; i<SL1_MAX_THREADS; i++) {
		for (j=0; j<SL1_ALL_REGS_SIZE; j++) {
			_regSetInfo[i]._regStatus[i].status = RS_READY;
//			_regSetInfo[i]._regStatus[i].cycle = 0;
			_regSetInfo[i]._regStatus[i].instrItem = NULL;
		}
	}
	RegSetInfo::initTables();
}

CPerfModelSL1::~CPerfModelSL1()
{
}

/*
 * Reset temporary register info strutures.
 * It will be called before hazard check for every instructions at every cycle. Please don't put any
 * one time initialization here.
 */
void CPerfModelSL1::update(void)
{
	int i;
	updateExecUnits();
	for (i=0; i<SL1_MAX_THREADS; i++) {
		_regSetInfo[i].updateRegSet();
		_regSetInfo[i].updateRegPort();				
	}
	if(!viterbiLatency() && viterbiActive()==true)
	{
		
		for(i=0;i<SL1_MAX_THREADS;i++)
		{
			if(_viterbiDes[i])
			{
				_regSetInfo[i].updateFixedReg(_viterbiDes[i]);
				_viterbiDes[i]=0;
			}
		}
		viterbiActive(false);
	}
	else if(!fftLatency() && fftActive()==true)
	{
		for(i=0;i<SL1_MAX_THREADS;i++)
		{
			if(_fftDes[i])
			{
				_regSetInfo[i].updateFixedReg(_fftDes[i]);
				_fftDes[i]=0;
			}
		}
		fftActive(false);
		
	}
	else if(!tracebackLatency() && tracebackActive()==true)
	{
		for(i=0;i<SL1_MAX_THREADS;i++)
		{
			if(_tracebackDes[i])
			{
				_regSetInfo[i].updateFixedReg(_tracebackDes[i]);
				_tracebackDes[i]=0;
			}
				
		}
	    tracebackActive(false);
	}
	if(fixPipeLineLatency() && tracebackActive()==false && viterbiActive()==false 
		&& fftActive()==false)
	{
		if(viterbiLatency())
		{
			viterbiActive(true);
		}
		else if(fftLatency())
		{
			fftActive(true);
		}
	 	else if(tracebackLatency())
		{
			tracebackActive(true);
		}	
	}
}

/*
 * Reset all execution units to unused.
 */
void CPerfModelSL1::updateExecUnits(void)
{
//	int i;
//	for (i=0; i<SL1_MAX_RSC; i++) {
//		_execUnitStatus[i] = UNIT_IDLE;
//	}
}

HAZARD_STATUS CPerfModelSL1::checkHazard(InstrItem *item)
{
	int threadId =item->threadId();

	HAZARD_STATUS hazardStatus = HS_NONE;
	_curInstrItem = item;
	
	// check execution units
	IsTrue((item->instrGrp()<=EIG_misc), ("checkHazard():Invalid instruction group %d\n", item->instrGrp()));
	int i;
	UINT execUnit = EXEC_UNIT_UNDEFINED;
	for (i=0;i<SL1InstrRscMapList[threadId][item->instrGrp()].nResources;i++) {
		UINT rscIdx = SL1InstrRscMapList[threadId][item->instrGrp()].rscTbl[i];
		if(isFixed(item->instrId())) //fixedfunc instr can not be issued in the same cyc
		{
			UINT fftIdx = SL1InstrRscMapList[threadId][EIG_c3_ffe].rscTbl[0];
			UINT viterbiIdx = SL1InstrRscMapList[threadId][EIG_c3_viterbi].rscTbl[0];
			UINT trbackIdx = SL1InstrRscMapList[threadId][EIG_c3_traceback].rscTbl[0];
			if(_execUnitStatus[fftIdx][PS_ID] !=PS_UNDEF || _execUnitStatus[viterbiIdx][PS_ID] !=PS_UNDEF
			|| _execUnitStatus[trbackIdx][PS_ID] !=PS_UNDEF){
				if (PSIM_SL1::g_dumpHazardDetails)
				{
					printf("fixedfunc instr can not be issued in the same cyc\n");
				}	
				break;
			}
			if(item->instrId()==c3fft_id && fftLatency())
			{
				if (PSIM_SL1::g_dumpHazardDetails)
					printf("%4s: fft fixed func engine is used now,instr %d stalled pipeline\n",getThreadName(threadId),item->perfId());
				break;				
			}
			if(item->instrId()==c3viterbi_id && viterbiLatency())
			{
				if (PSIM_SL1::g_dumpHazardDetails)
					printf("%4s: viterbi fixed func engine is used now,instr %d stalled pipeline\n",getThreadName(threadId),item->perfId());
				break;
			}
			if(item->instrId()==c3traceback_id && tracebackLatency())
			{
				if (PSIM_SL1::g_dumpHazardDetails)
						printf("%4s: traceback fixed func engine is used now,instr %d stalled pipeline\n",getThreadName(threadId),item->perfId());
				break;
			}
		}
		if (_execUnitStatus[rscIdx][PS_ID] == PS_UNDEF) {
#ifndef MULTI_THREAD			
			if (PSIM_SL1::g_coreNoALULS && (item->threadId() == THREAD_ID_CORE) && (SL1RscTable[rscIdx].rscAttr == RISC_ALU_LS)) continue;
			if (PSIM_SL1::g_bbNoALU && (item->threadId() == THREAD_ID_BB) && (SL1RscTable[rscIdx].rscAttr == RISC_ALU_BR)) continue;
#endif	
			execUnit = SL1InstrRscMapList[threadId][item->instrGrp()].rscTbl[i];
            #ifdef NO_ACC_CROSS
        	RegListIter regIter;
            if(item->instrGrp()==EIG_c3_mac)
            {
            	for (regIter=item->getDesRegs()->begin();
		 		regIter != item->getDesRegs()->end();
		 		regIter ++) 
		 		{	
		 			INT regIndex = *regIter;
		 			if(regIndex>=EBASE_ACC) //ACC reg need check execunit;
		 			{
		 				regIndex = regIndex-EBASE_ACC;
		 				if(execUnit != _pipe_acc_eu[item->threadId()][regIndex] && _pipe_acc[item->threadId()][regIndex]==true)
		 				{
		 			   		execUnit = EXEC_UNIT_UNDEFINED;
		 			   		break;
		 				}
		 			}
		 		}
		 		if(execUnit==EXEC_UNIT_UNDEFINED)
		 		   continue;
            }
            #endif			
			break;
		}
	}
	if (EXEC_UNIT_UNDEFINED == execUnit) {
		if (PSIM_SL1::g_dumpHazardDetails)
			printf("%4s Instr %d: no execution unit available\n", getThreadName(threadId), item->perfId());
		hazardStatus = HS_ENGINE_FULL;
		IsTrue((item->instrGrp()>=0 && item->instrGrp() < EIG_max),("Invalid group id:%d\n", item->instrGrp()));
		_euHazardCnt[item->threadId()][item->instrGrp()] ++;
	}

	_regSetInfo[threadId].clearPorts();
	
	if (HS_NONE == hazardStatus) {
		// check registers and ports hazard
		hazardStatus=_regSetInfo[threadId].checkRegHazard(item);
	}
	
	// No hazard, lock the resource immediately
	if (HS_NONE == hazardStatus) {
	    #ifdef NO_ACC_CROSS
		RegListIter regIter;
		//if(MacNum>1){
		if(item->instrGrp()==EIG_c3_mac){
			for (regIter=item->getDesRegs()->begin();
		 	regIter != item->getDesRegs()->end();
		 	regIter ++) 
		 	{
		 		INT regIndex = *regIter;
		 		if(regIndex>=EBASE_ACC) //ACC reg need check execunit;
		 		{
		 			regIndex = regIndex-EBASE_ACC;
		 			_pipe_acc[item->threadId()][regIndex] = true;
		 			_pipe_acc_eu[item->threadId()][regIndex] = execUnit;
		 		}
		 	}
		}
		#endif
		_regSetInfo[threadId].lockPorts();
		_execUnitStatus[execUnit][PS_ID] = (int)item->perfId();
		_execUnitUsageCnt[execUnit][item->threadId()] ++;
		item->execUnit(execUnit);
//		printf("instr %d alloc eu:%s\n", item->perfId(), SL1RscTable[execUnit].rscName);
		_regSetInfo[threadId].lockReg(item);
	}
	
	_prevInstrItem = item;
	return hazardStatus;
}

void CPerfModelSL1::dump(FILE *out)
{
	INT i, j;
//	fprintf(out, "---use-define table---\n");
//	for (i=0; i<SL1_MAX_THREADS; i++) {
//		for (j=0; j<SL1_ALL_REGS_SIZE; j++) {
//			if (_regSetInfo[i]._regStatus[j].status == RS_READY)
//				continue;	// ignore dump on those unused registers
//			fprintf(out, "\t%4s - reg %02d - def(%d %d) - use(%d %d) - stage(%-6s %d)\n",
//				getThreadName(i), j,
//				_regSetInfo[i]._regDefine[j].request, _regSetInfo[i]._regDefine[j].lock,
//				_regSetInfo[i]._regUse[j].request, _regSetInfo[i]._regUse[j].lock,
//				(RS_NORMAL == _regSetInfo[i]._regStatus[j].status)?"normal":"wb",
//				_regSetInfo[i]._regStatus[j].instrItem->instrStage()); 
//		}
//	}
//	
//	fprintf(out, "---ports table---\n");
//	for (i=0; i<SL1_MAX_THREADS; i++) {
//		fprintf(out, "\t%4s - rports req:%d - max:%d\n",
//			getThreadName(i), _regSetInfo[i]._readPorts[REGCLASS_GPR], _regSetInfo[i]._maxReadPorts[REGCLASS_GPR]);
//		fprintf(out, "\t%4s - wports req:%d - max:%d\n",
//			getThreadName(i), _regSetInfo[i]._writePorts[REGCLASS_GPR], _regSetInfo[i]._maxWritePorts[REGCLASS_GPR]);
//	}
	
	fprintf(out, "---exec unit status ---\n");
	for (i=0; i<SL1_MAX_RSC && (CISC_FFT != SL1RscTable[i].rscAttr);	i++) {
		//if(SL1RscTable[i].rscAttr!=CISC_FFT && SL1RscTable[i].rscAttr!=CISC_VITERBI && SL1RscTable[i].rscAttr != CISC_TRACEBACK)
		fprintf(out, "\tEX%d - %s - ID=%d EX=%d MEM=%d WB=%d\n", i, SL1RscTable[i].rscName, 
				_execUnitStatus[i][PS_ID],
				_execUnitStatus[i][PS_EX],
				_execUnitStatus[i][PS_MEM],
				_execUnitStatus[i][PS_WB]);
	}
	for (i;i<SL1_MAX_RSC && ( EXEC_UNIT_UNDEFINED != SL1RscTable[i].rscAttr);	i++) {
		if(SL1RscTable[i].rscAttr==CISC_FFT)
		{
			if(fftActive()==true)
				fprintf(out, "\tEX%d - %s fft calculate will finished in %d cycs.\n",i,SL1RscTable[i].rscName,
					fftLatency());
			else
				fprintf(out, "\tEx%d - %s %s\n",i,SL1RscTable[i].rscName,
					(fftLatency()==0)? "not used by any thread.":"waiting for other fixed func engine finished.");
		}
		else if(SL1RscTable[i].rscAttr==CISC_VITERBI)
		{
			if(viterbiActive()==true)
				fprintf(out, "\tEX%d - %s viterbi calculate will finished in %d cycs.\n",i,SL1RscTable[i].rscName,
					viterbiLatency());
			else
				fprintf(out, "\tEx%d - %s %s\n",i,SL1RscTable[i].rscName,
					(viterbiLatency()==0)? "not used by any thread.":"waiting for other fixed func engine finished.");
		}
		else if(SL1RscTable[i].rscAttr==CISC_TRACEBACK)
		{
			if(tracebackActive()==true)
				fprintf(out, "\tEX%d - %s traceback calculate will finished in %d cycs.\n",i,SL1RscTable[i].rscName,
					tracebackLatency());
			else
				fprintf(out, "\tEx%d - %s %s\n",i,SL1RscTable[i].rscName,
					(tracebackLatency()==0)? "not used by any thread.":"waiting for other fixed func engine finished.");
		}
		else
			IsTrue((0),("Invalid fixed func engine type for dump\n"));
	}
}

STRING RegClassName[] = 
{
	"REGCLASS_GPR",
	"REGCLASS_CTRL",
	"REGCLASS_ACC",
	"REGCLASS_AR",
	"REGCLASS_HI",
	"REGCLASS_FFT",
	"REGCLASS_VITERBI",
	"REGCLASS_TRBACK"
};

void CPerfModelSL1::dumpConfig(FILE *out)
{
	INT i,j,k;
	// resources
	// Dump resource table
	fprintf(out, "Execution Resources: (resource name, function type):\n");
	for (i=0; SL1RscTable[i].rscAttr != EXEC_UNIT_UNDEFINED && i<SL1_MAX_RSC; i++) {
		fprintf(out, "\t%d: %12s -- types: ", i, SL1RscTable[i].rscName);
		for (j=0; SL1RscTable[i].rscTypes[j][0] && j<SL1_RSC_MAX_TYPES; j++)
			fprintf(out, "%4s ", SL1RscTable[i].rscTypes[j]);
		fprintf(out, "\n");
	}
	
	// resource map: what kind of resource is required to execute an instruction
	fprintf(out, "Instruction-to-resource map:\n");
	for (i=0; i<=EIG_misc; i++) {
		fprintf(out, "\tInstrGrp %02d %10s:", i, SL1InstrRscMap[i].eigName);
		fprintf(out, "  %d:", SL1InstrRscMap[i].nResources);  
		for (k=0; k<SL1InstrRscMap[i].nResources; k++) {
			fprintf(out, "%12s ", SL1RscTable[SL1InstrRscMap[i].rscTbl[k]].rscName); 
		}
		fprintf(out, "\n");
	}

	for (j=0; j<SL1_MAX_THREADS; j++) {
		fprintf(out, "Thread %s Instruction-to-resource map:\n", getThreadName(j));
		for (i=0; i<=EIG_misc; i++) {
			fprintf(out, "\tInstrGrp %02d %10s:", i, SL1InstrRscMapList[j][i].eigName);
			fprintf(out, "  %d:", SL1InstrRscMapList[j][i].nResources);  
			for (k=0; k<SL1InstrRscMapList[j][i].nResources; k++) {
				fprintf(out, "%12s ", SL1RscTable[SL1InstrRscMapList[j][i].rscTbl[k]].rscName); 
			}
			fprintf(out, "\n");
		}
	}

	for (j=0; j<SL1_MAX_THREADS; j++) {
		// registers per thread
		fprintf(out, "%4s: %d registers %d register classes\n", 
			getThreadName(j), SL1_ALL_REGS_SIZE, REGCLASS_MAX);
//		for (i=0; i<REGCLASS_MAX; i++)
//			fprintf(out, "\tRegClass %d name: %s\n", i, RegClassName[i]);
		// ports per regclass per thread
//		fprintf(out, "%s: register ports per regclass\n", getThreadName(j));
		for (i=0; i<REGCLASS_MAX; i++) {
			if (_regSetInfo[j]._maxReadPorts[i] != DEFAULT_DONT_CARE_PORT) {
				fprintf(out, "\t RegClass %13s rports:%9d wports: %9d\n", 
					RegClassName[i], 
					_regSetInfo[j]._maxReadPorts[i], 
					_regSetInfo[j]._maxWritePorts[i]);
			} else {
				fprintf(out, "\t RegClass %13s rports:DONT CARE wports: DONT CARE\n", 
					RegClassName[i]);
			}
		}
	}
}

void CPerfModelSL1::dumpStatistics(FILE *out)
{
	int i, j;
	for (i=0; i<_execUnitNumber; i++) {
		for (j=0; j<SL1_MAX_THREADS; j++) {
			if (_execUnitUsageCnt[i][j]) {
			fprintf(out, "\t%12s used by %4s: %8d times\n", 
				SL1RscTable[i].rscName,
				getThreadName(j),
				_execUnitUsageCnt[i][j]);
			}
		}
	}
	fprintf(out, "---hazard count by instr group ---\n");
	for (i=0; i<SL1_MAX_THREADS; i++) {
		for (j=0; j<sizeof(SL1InstrRscMap)/sizeof(INSTR_RSC_MAP_ITEM); j++) {
			if (_euHazardCnt[i][SL1InstrRscMap[j].eig]) {
			fprintf(out, "\t%4s - %s: %d\n", getThreadName(i),
				SL1InstrRscMap[j].eigName,
				_euHazardCnt[i][SL1InstrRscMap[j].eig]); 
			}
		}
	}
}
