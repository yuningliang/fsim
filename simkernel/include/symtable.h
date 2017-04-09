#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include "defs.h"
#include "sym.h"
#include <utility>

class ELF_object;

struct eq_name {
	bool operator() (const char* s1, const char* s2) const
  	{
    	return strcmp(s1, s2) == 0;
	}
};

class SymTable {
	public:
	typedef __gnu_cxx::hash_map<char* const, Sym*, __gnu_cxx::hash<STRING>, eq_name> NameMap;
	typedef __gnu_cxx::hash_map<ADDR, Sym*, __gnu_cxx::hash<ADDR> > ValueMap;
	typedef NameMap::iterator NameMapIter;
	typedef ValueMap::iterator ValueMapIter;	
	typedef std::vector<Sym* > SymEntry;
	typedef SymEntry::iterator SymEntryIter;
	
	typedef hash_map<ADDR,ADDR> IFMap;
	typedef IFMap::iterator IFMapIter; //IF->Ignore and Focus


	private:
	SymEntry _table;
	NameMap _nmap;
	ValueMap _funcmap;	
	IFMap _Imap;
	ADDR* _IFAddrBegin;
	ADDR* _IFAddrEnd;
	UINT _IFAddrNum;
	STRING _IFFuncbuf;

	
	public:
	SymTable(void) {_IFAddrNum=0;_Imap.clear();_nmap.clear();}
	void initTable(ELF_object* obj);
	void clearTable(void) {
		_table.clear();
		_nmap.clear();
		_funcmap.clear();
		_Imap.clear();
	}
	void addSym(Sym* sym, unsigned elf_sym_info=0);
	SymEntry* getTable(void) { return &_table; }
	Sym* getSym(STRING name);
	Sym* getSym(ADDR val); //return the 1st sym with the value "val"
	Sym* getSym(ADDR val, SYM_TYPE type);	//return the 1st sym with the value "val" and type "type"
	Sym* getFunc(ADDR val);	//return the 1st sym with the value "val" and type SYM_FUNC
	ADDR getValue(const STRING name);
	STRING getName(ADDR val); //return the 1st Name with the value "val"
	STRING getName(ADDR val, SYM_TYPE type);	//return the 1st Name with the value "val" and type "type"
	STRING getFuncName(ADDR val);	//return the 1st Name with the value "val" and type SYM_FUNC	
	pair<STRING, INT> getFuncOffset(ADDR val);
	pair<STRING, ADDR> getFuncStart(ADDR val);
	void setValue(Sym* sym, ADDR val) { sym->value(val); }
	BOOL nameToIgnore(STRING);
	BOOL checkIgnoreFocusName(STRING,STRING);
	IFMap getIgnoreMap(void){return _Imap;}
	UINT getIFAddrNum(void){return _IFAddrNum;}
	ADDR getIFaddrBegin(int i){return _IFAddrBegin[i];}
	ADDR getIFaddrEnd(int i){return _IFAddrEnd[i];}
	void setIgnoreFocusAddr(STRING name);
};

#endif /*SYMTABLE_H_*/
