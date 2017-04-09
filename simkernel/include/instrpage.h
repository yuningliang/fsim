#ifndef INSTRPAGE_H_
#define INSTRPAGE_H_

#include "defs.h"

template<class INSTRClass>
class InstrPage {
	private:
	INSTRClass* _entry;
	UBYTE* _tag;
	UINT _instrPageSize;
	
	public:
	InstrPage(UINT size, UBYTE* tag) {
		_entry = new INSTRClass[size];
		_tag = tag;
		for (UINT i=0; i<size; i++) {
			_entry[i].init();
		}
		_instrPageSize = size;
	}

	pair<INSTRClass*, UBYTE> getInstrEntry(UINT index) { 
//		printf("Entering getInstr: index:%#08x rawbits:%#08x\n", index, _entry[index].rawbits());
		pair<INSTRClass*, UBYTE> instr;
		UINT byte_index = index<<MIN_BITS_ADDR_SHIFT;
		instr.first = &_entry[index]; 
		instr.second = _tag[byte_index] | _tag[byte_index+1];
		return instr; 
	}

	INSTRClass* getInstr(UINT index) { 
		return &_entry[index];
	}
		
	void dump() {
		printf("-------Instruction page dump begin----------------------\n");
		for (UINT i=0; i<_instrPageSize; i++) {
			printf("index %#08x point %p: rawbits:%#08x\n", i, &_entry[i], _entry[i].rawbits());
		}
		printf("-------Instruction page dump end------------------------\n");
	}
};

#endif /*INSTRPAGE_H_*/
