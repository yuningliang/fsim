
#include "defs.h"
#include "tclapi.h"
#include "memory.h"
#include "sl2mmu.h"
#include "sl2instr.h"
#include "sl2reg.h"
#include "sl2exec.h"
#include "sl2fetch.h"
#include "sl2decoder.h"
#include "sl2disasm.h"
#include "status.h"
#include "sl2machine.h"
#include "shell.h"

int main(int argc, char *argv[]) {
	Address addr;
	ProcessStatus<SL2Instr> status;
	SL2MMU mmu(addr, status);
	SL2Register reg01(status);
	SL2Register reg02(status);
	SL2Exec exec01(mmu, reg01, status);
	SL2Exec exec02(mmu, reg02, status);
	SL2Fetch fetch(mmu, addr, status);
	SL2Disasm disasm01(mmu, reg01, THREAD_ID_MAIN_0);
	SL2Disasm disasm02(mmu, reg02, THREAD_ID_MAIN_1);
	SL2Machine machine(mmu, fetch, status);
	Profiler<SL2Instr, SL2Machine> profiler(status, machine);
	SL2Decoder decoder;
	fprintf(stdout, "EBASE_GPR = %d\n", EBASE_GPR);
	fprintf(stdout, "EBASE_CTRL = %d\n", EBASE_CTRL);
	fprintf(stdout, "EBASE_LOOP_CNT = %d\n", EBASE_LOOP_CNT);
	fprintf(stdout, "EBASE_SIMD = %d\n", EBASE_SIMD);
	fprintf(stdout, "EBASE_C2_CTRL = %d\n", EBASE_C2_CTRL);
	fprintf(stdout, "EBASE_C2_SPEC = %d\n", EBASE_C2_SPEC);
	fprintf(stdout, "EBASE_C2_ACC = %d\n", EBASE_C2_ACC);
	fprintf(stdout, "EBASE_C2_INTERNAL = %d\n", EBASE_C2_INTERNAL);
	fprintf(stdout, "EBASE_SCALAR = %d\n", EBASE_SCALAR);

	if(argc>=2) {
		FILE* fp = fopen(argv[1], "r");
		UINT raw;
		UINT meta;
		char str[50];
		pair<INT, INT> ports;
		if(fp!=NULL) {
			while(fscanf(fp, "%x %d%[^\n]", &raw, &meta, str)!=EOF) {
				RegList destList;
				RegList srcList;
				RegListIter iter;	
				SL2Instr instr;		
				decoder.decode(&instr, 0, raw, meta);	
				fprintf(stdout, "instr= 0x%08x (%d)\n", raw, instr.group());		
				fprintf(stdout, "%s", disasm01.disasm(0, &instr, meta));
				instr.getRegList(&destList, &srcList);
				iter = destList.begin();
				fprintf(stdout, "Dest: ");
				while(iter!=destList.end()) {
					fprintf(stdout, "%d ", *iter);
					iter++;
				}
				iter = srcList.begin();
				fprintf(stdout, "\nSrc: ");
				while(iter!=srcList.end()) {
					fprintf(stdout, "%d ", *iter);
					iter++;
				}	
				fprintf(stdout, "\n");
				ports = instr.gprRegPort();
				fprintf(stdout, "GPR ports r=%d, w=%d\n", ports.first, ports.second);
				ports = instr.simdRegPort();
				fprintf(stdout, "SIMD ports r=%d, w=%d\n", ports.first, ports.second);								
				fprintf(stdout, "\n");
				if(instr.macro()==1&&instr.macroCnt()>=0) {
					SL2InstrListIter miter = instr.getMacro()->begin();
					SL2InstrListIter miter_end = instr.getMacro()->end();
					int i = 0;
					printf("Macro:\n");
					while(miter!=miter_end) {
				    	fprintf(stdout, "(%x) %s", (*miter)->id(), disasm01.disasm(i, *miter, 0));
						(*miter)->getRegList(&destList, &srcList);
						iter = destList.begin();
						fprintf(stdout, "Dest: ");
						while(iter!=destList.end()) {
							fprintf(stdout, "%d ", *iter);
							iter++;
						}
						iter = srcList.begin();
						fprintf(stdout, "\nSrc: ");
						while(iter!=srcList.end()) {
							fprintf(stdout, "%d ", *iter);
							iter++;
						}	
						fprintf(stdout, "\n");
						ports = (*miter)->gprRegPort();
						fprintf(stdout, "GPR ports r=%d, w=%d\n", ports.first, ports.second);
						ports = (*miter)->simdRegPort();
						fprintf(stdout, "SIMD ports r=%d, w=%d\n", ports.first, ports.second);								
						fprintf(stdout, "\n");				    	
						miter++;
						i++;
					}
					
				}
			}	
		}
		fclose(fp);
	}
}
