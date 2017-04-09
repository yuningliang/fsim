/* /////////////////////////////// "PerfDefs.h" /////////////////////////////////
 *  File: PerfDefs.h
 *  Descriptions:  Definitions for performance simulation
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
#ifndef PERFDEFS_H_
#define PERFDEFS_H_

#define MAX_DISASM_BUF_LEN          64

#define MAX_PREREAD_BUF				400000	//in kbytes

#ifndef MULTI_THREAD
#define SL1_MAX_THREADS       2
#else
#define SL1_MAX_THREADS       4
#define THREAD_EXEC_MASK SL1_MAX_THREADS-1
#endif

#define DEFAULT_FETCH_BUF_LMT       4
#define FETCH_NO_STALL              (-1)

#define OFFSET_SHIFT_BR_PSIM 1

enum BRANCH_META {
	BRANCH_PREDICTION_RIGHT = 0,
	BRANCH_PREDICTION_WRONG = -1,
	BRANCH_PREDICTION_NONE =1
};

#define DEFAULT_CORE_IWIN_SIZE		4
#define DEFAULT_BB_IWIN_SIZE        4
#define DEFAULT_IWIN_LMT            8

#define DEFAULT_MAX_ISSUE           2
#define DEFAULT_MAX_CORE_ISSUE      2
#define DEFAULT_MAX_BB_ISSUE        3
#define DEFAULT_MAX_TOTAL_ISSUE     4
#define DEFAULT_MAX_ISSUE_PER_CALL  256	// Just a wild #

// Default ports number setting. Can be overridden from configuration.
#define DEFAULT_DONT_CARE_PORT		1000

#ifndef MULTI_THREAD
#define MAX_CORE_GPR_READ_PORTS     4
#define MAX_CORE_GPR_WRITE_PORTS    2
#define MAX_BB_GPR_READ_PORTS       6
#define MAX_BB_GPR_WRITE_PORTS      3
#else
#define MAX_THREAD0_GPR_READ_PORTS 6
#define MAX_THREAD0_GPR_WRITE_PORTS 3
#define MAX_THREAD1_GPR_READ_PORTS 6
#define MAX_THREAD1_GPR_WRITE_PORTS 3
#define MAX_THREAD2_GPR_READ_PORTS 6
#define MAX_THREAD2_GPR_WRITE_PORTS 3
#define MAX_THREAD3_GPR_READ_PORTS 6
#define MAX_THREAD3_GPR_WRITE_PORTS 3
#endif

#define DEFAULT_INSTR_STAGES        4	// IS-EX-MEM-WB without IF
#define PIPE_NORMAL_CYCLES          DEFAULT_INSTR_STAGES
#define PIPE_WB_READY_CYCLES        2

#define EXTRA_PAGE_SIZE 128	// Must not be less than the total number of instructions in PIPE and IWIN

// register set for a single thread
enum REG_CLASS {
	RS_UNDEFINED = -1,
	REGCLASS_GPR=0,
	REGCLASS_CTRL,
	REGCLASS_ACC,
	REGCLASS_AR,
	REGCLASS_HI,
	REGCLASS_FFT,
	REGCLASS_VITERBI,
	REGCLASS_TRBACK,
	REGCLASS_MAX,	// Keep this as the last one
};

enum EXEC_UNIT {
	RISC_LS = 0,
#ifndef MULTI_THREAD
    RISC_ALU_LS,
	RISC_ALU_BR,
#else	
	RISC_ALU_LS0,
	RISC_ALU_LS1,
	RISC_ALU_BR0,
	RISC_ALU_BR1,
	RISC_ALU_BR2,
	RISC_ALU_BR3,
	RISC_LS0,
	RISC_LS1,
#endif	
	ALU0,
	ALU1,
	LS0,
	LS1,
	BR0,
	BR1,
	CISC_MAC0,
	CISC_MAC1,
	CISC_MAC2,
	CISC_FFT,
	CISC_VITERBI,
	CISC_TRACEBACK,
	EXEC_UNIT_MAX,	// Please keep this be the last one
	EXEC_UNIT_ADDED,	// Only for use with resource added from command line
	EXEC_UNIT_UNDEFINED=(unsigned)-1
};

enum EXEC_UNIT_SET {
EUSET_ORIGINAL     = 0,
EUSET_SEPARATED_LS = 1,
EUSET_SEPARATED_BR = 2,
EUSET_SEPARATED_ALL= 3,
EUSET_NONE=4,
};

/*
 * Should be modified.
 */
enum ES_REG_STATUS {
	RS_READY = 0,
	RS_NORMAL = 1, //RaW with ALU operations 
	RS_MEM_WB = 2, //RaW with memory access
	RS_RET_WB = 3, //RaW on return register "RA"
	RS_MAC_WB = 4,
};

enum PIPELINE_STAGE {
	PS_UNDEF = 0,
	PS_FE   = 1,
	PS_ID   = 2,
	PS_EX   = 3,
	PS_MEM  = 4,
	PS_WB   = 5,
	PS_LAST = 6
};

enum HAZARD_STATUS {
	HS_UNDEFINED = -1,
	HS_NONE = 0,
	HS_ENGINE_NOT_READY,
	HS_ENGINE_FULL,
	HS_INSTR_ORDER,
	HS_BR_STALL, 
	HS_READ_PORT_JAM,
	HS_WRITE_PORT_JAM,
	HS_REG_WAW,
	HS_REG_RAW,
	HS_REG_WAR,
	HS_IWIN_FULL,
	HS_MAX_ISSUE,
	HS_MAX
};

enum ISSUE_STATUS {
	ISSUE_IDLE = 0,
	ISSUE_CONT = 1,
	ISSUE_STOPPED = 2,
};

#endif /*PERFDEFS_H_*/
