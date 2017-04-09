#ifndef REGMAPITEM_H_
#define REGMAPITEM_H_

#include "defs.h"

class RegMapItem {
	private:
	STRING _name;
	UINT _size;
	BYTE* _ptr;
	
	public:
	RegMapItem(STRING n, UINT s, BYTE* p) {
		_name = n;
		_size = s;
		_ptr = p;
	}
	pair<DWORD, UINT> getValue(void) {
		pair<DWORD, UINT> ret(0, _size);
		switch (_size) {
			case HWORD_BYTE:
				ret.first = *(HWORD*) _ptr;
				break;
			case WORD_BYTE:
				ret.first = *(WORD*) _ptr;
				break;		
			case DWORD_BYTE:
				ret.first = *(DWORD*) _ptr;
				break;
			default:
				SimFatal((0), ("RegMapItem error: unknown data size (%d).", _size));
		}
		return ret;
	}
	
	BYTE *getPtr(void)   {	return _ptr;	}
	STRING getName(void) {	return _name;	}
	UINT   getSize(void) {	return _size;	}
	
	void setValue(DWORD data) {
		switch (_size) {
			case HWORD_BYTE:
				*(HWORD*) _ptr = (HWORD) data;
				break;
			case WORD_BYTE:
				*(WORD*) _ptr = (WORD) data;
				break;		
			case DWORD_BYTE:
				*(DWORD*) _ptr = data;
				break;
			default:
				SimFatal((0), ("RegMapItem error: unknown data size (%d).", _size));
		}

	}
	
};

struct eq_reg_name {
	bool operator() (const char* s1, const char* s2) const
  	{
    	return strcmp(s1, s2) == 0;
	}
};

typedef __gnu_cxx::hash_map<char* const, RegMapItem*, __gnu_cxx::hash<STRING>, eq_reg_name> RegMap;
typedef RegMap::iterator RegMapIter;

#endif /*REGMAP_H_*/
