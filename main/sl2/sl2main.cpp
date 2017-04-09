
#include "defs.h"
#include "sl2shell.h"
#include "tclapi.h"
#include "longjmp.h"
#include "properties.h"
#include "btree.h"
#include "sl2defs.h"
#include "sl2mmu.h"
#include "sl2instr.h"
#include "sl2reg.h"
#include "sl2exec.h"
#include "sl2fetch.h"
#include "sl2decoder.h"
#include "sl2disasm.h"
#include "sl2threadctrl.h"
#include "status.h"
#include "sl2machine.h"


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
	SL2Shell *shell = new SL2Shell();
	tcl_shell_ptr = shell;
#ifdef NEED_LONG_JMP
	sim_shell_ptr = shell;
#endif		
	shell->mainRunTCL(argc, argv);
}
