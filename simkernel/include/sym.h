#ifndef SYM_H_
#define SYM_H_

#include "defs.h"
#include "simdefs.h"

const STRING SYM_TYPE_NAME[] = {
	"ABS",
	"FUNC",
	"SDATA",
	"DATA",
	"SBSS",
	"BSS",
	"SECTION",
	"ILLEGAL",
	"USER",
};

class Sym
{
	private:
	char*    _name;
	SYM_TYPE _type;
	ADDR     _value; // 32 bit machine, TARG_WORD_SIZE == 4
	UINT32   _size; 
	
	public:
	void name(char* s)          { _name = s; }
	char* name(void)            { return _name; }
	void type(const SYM_TYPE t) { _type = t; }
	SYM_TYPE type(void)         { return _type; }
	void value(ADDR v)            { _value = (ADDR) v; }
	ADDR value(void)              { return _value; }
	void size(const UINT32 sz)  { _size = sz; }
	UINT32 size(void)           { return _size; }
	STRING typeName(void) { return SYM_TYPE_NAME[(INT)type()];}
	void dump(FILE *out) {
		fprintf(out, "symbol name:%s type:%s addr:%#08x size:%d\n", name(), typeName(), value(), size());
	}
};

#endif /*SYM_H_*/
