/*
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 */


#include "defs.h"
#include "sl1shell.h"
#include "tclapi.h"
#include "longjmp.h"
#include "properties.h"
#include "btree.h"
#include "sl1defs.h"
#include "memory.h"
#include "sl1address.h"
#include "sl1mmu.h"
#include "sl1instr.h"
#include "sl1reg.h"
#include "sl1exec.h"
#include "sl1fetch.h"
#include "sl1decoder.h"
#include "sl1disasm.h"
#include "status.h"
#include "sl1machine.h"

int main(int argc, char *argv[]) {
	Properties::getInstance().enablePseudoSerial(false);
	for (int i=1; i<argc; i++) {
 		if(strncmp(argv[i], "-pts", 4) == 0) {
 			/*
 			 * Use pseudo terminal for uart simulation
 			 */
 			 Properties::getInstance().enablePseudoSerial(true);
 		}
	}

	SL1Shell *shell = new SL1Shell();
	
	tcl_shell_ptr = shell;
#ifdef NEED_LONG_JMP
	sim_shell_ptr = shell;
#endif	
	shell->mainRunTCL(argc, argv);
}
