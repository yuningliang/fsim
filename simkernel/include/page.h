#ifndef PAGE_H_
#define PAGE_H_

#include "defs.h"
#include "simdefs.h"

class Page {
	private:
	BYTE* _data;
	UBYTE* _tag;
	INT _size;
	BYTE _defaultValue;
	
	private:
			
	public:
	Page(INT size, BYTE defaultValue) {
		_data = new BYTE[size];
		_tag = new UBYTE[size];
		_size = size;
		_defaultValue = defaultValue;
		initAll();
	}
	
	INT size(void) { return _size; }
	
	UBYTE* getPageTagPtr(void) { return &_tag[0]; }
	
	void initData() {
		for(INT i = 0; i<_size; i++) {
			_data[i] = _defaultValue;
		}		
	}	
	void initAll() {
		for(INT i = 0; i<_size; i++) {
			_data[i] = _defaultValue;
			_tag[i] = ET_NORMAL;
		}		
	}
	void initTag() {
		for(INT i = 0; i<_size; i++) {
			_tag[i] = ET_NORMAL;
		}		
	}	
	void setByteData(INT index, BYTE data) {
		_data[index] = data;
	}

	BYTE getByteData(INT index) {
		return _data[index];
	}
	void setHwordData(INT index, HWORD data) {
		*(UINT16*)&_data[index] = data;
	}

	HWORD getHwordData(INT index) {
		return (*(UINT16*)&_data[index]);
	}
	void setWordData(INT index, WORD data) {
		*(UINT32*)&_data[index] = data;
	}

	WORD getWordData(INT index) {
		return (*(UINT32*)&_data[index]);		
	}	
	void clearEntryTag(INT index, UBYTE tag) {
		_tag[index] &= (~tag);
	}

	void setByteEntryTag(INT index, UBYTE tag) {
		_tag[index] |= tag;
	}
	void setHWordEntryTag(INT index, UBYTE tag) {
		_tag[index]  |= tag;
		_tag[index+1] |= tag;;
	}
	void setWordEntryTag(INT index, UBYTE tag) {
		_tag[index] |= tag;
		_tag[index+1] |= tag;
		_tag[index+2] |= tag;
		_tag[index+3] |= tag;
	}
	
	BYTE getByteEntryTag(INT index) {
		return _tag[index];
	}
	BYTE getHWordEntryTag(INT index) {
		return (_tag[index]|_tag[index+1]);
	}
	BYTE getWordEntryTag(INT index) {
		return (_tag[index]|_tag[index+1]|_tag[index+2]|_tag[index+3]);
	}
};

#endif /*PAGE_H_*/
