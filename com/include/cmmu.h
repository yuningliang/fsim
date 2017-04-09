#ifndef CMMU_H_
#define CMMU_H_

#include "defs.h"
#include "address.h"
#include "memory.h"
#include "status.h"

class InterfaceHandler;

class CoreMMU {
	protected:
	Address& _address;
	Memory* _memory;
		
	ADDR _dataStart;
	ADDR _dataEnd;	
	ADDR _allocStart;
	ADDR _allocLimit;
	ADDR _textStart;
	ADDR _textEnd;
	UINT _curthread;
	
	INT _mmap_cal;
	
	struct freelist{
		ADDR start; //free space start
	    INT page_num; //free space page number 
	    INT flag;
	    ADDR end;   //free space end;
	    INT next;
	    INT prev;
	}_alloc[MAX_ALLOC_NUM];
	
	UINT _searchPos;
				
	protected:
	

	public:
	CoreMMU(Address& addr);
	~CoreMMU() {};
	void init(void);
	Memory* memory(void) { return _memory; }
	void memory(Memory* memory) { _memory = memory; }
	
	struct freelist *alloc(INT i){return &_alloc[i];}
	void searchPos(INT i){_searchPos=i;}
	INT searchPos(void){return _searchPos;}

	ADDR textStart(void) { return _textStart; }
	ADDR textEnd(void) { return _textEnd; }	
	ADDR dataStart(void) { return _dataStart; }
	ADDR dataEnd(void) { return _dataEnd; }
	void dataEnd(ADDR end) { _dataEnd = end; }
	ADDR allocStart(void) { return _allocStart; }
	void allocStart(ADDR start) {_allocStart = start;}
	ADDR allocLimit(void) { return _allocLimit;}
	
	UINT mmap_cal(void) {return _mmap_cal;}
	void add_mmap_cal(void){_mmap_cal++;}	
	void showTextRange(void) {
		fprintf(stdout, "Text Start = %x, Text End = %x\n", _textStart, _textEnd);
	}
	virtual void setTextRange(ADDR start, ADDR end) {
		_textStart = start;
		_textEnd = end;
	}
	virtual void setDataRange(ADDR start, ADDR end) {
		_dataStart = start;
		_dataEnd = end;
	}
	void setAllocRange(ADDR allocStart, UINT allocLimit) {
		_allocStart=allocStart;
		_allocLimit=allocLimit;
		if ((UINT)(_allocLimit/ADD_PAGE_SIZE) > MAX_ALLOC_PAGE) {
			_alloc[0].page_num = MAX_ALLOC_PAGE;
			if (_allocLimit != (UINT)-1) {
				fprintf(stderr, "Free memory too large and wasted. Max allowed size:0x%x08x bytes\n",
					MAX_ALLOC_PAGE * ADD_PAGE_SIZE);
			}
		} else {
			_alloc[0].page_num = _allocLimit/ADD_PAGE_SIZE;
		}
		_alloc[0].end = _allocStart+_alloc[0].page_num*ADD_PAGE_SIZE;
    	//printf("page num:%d max:%d\n", _alloc[0].page_num, MAX_ALLOC_PAGE);
	}
	
	UINT checkTextRange(ADDR pc) {
		if(pc>=_textStart&&pc<_textEnd) return ET_NORMAL;
		else return ET_ERR;
	}

	UINT checkDataRange(ADDR addr) {
		if(addr>=_dataStart&&addr<_dataEnd) return ET_NORMAL;
		else return ET_ERR;
	}

	ADDR checkTextStart(ADDR pc) {
		return (pc>=_textStart?pc:_textStart);
	}

	ADDR checkTextEnd(ADDR pc) {
		return (pc<_textEnd?pc:_textEnd);
	}

	void readBlock(const ADDR, BYTE*, INT) ;
	void writeBlock(const ADDR, const BYTE*, INT);
	void initBlock(const ADDR, BYTE, INT);	
	void initFile(FILE*, const ADDR);	
	void dumpFile(FILE*, const ADDR, INT);	
	INT getStrlen(const ADDR addr);
	STRING getString(const ADDR addr);
	void setString(const ADDR addr, const STRING str);
	UBYTE* getPageTagPtr(ADDR pageStartAddr) ;
	
	//override function
	//used by kernal aand execution
	UBYTE writeByte(const ADDR, const BYTE);
	UBYTE writeHword(const ADDR, const HWORD);
	UBYTE writeWord(const ADDR, const WORD);
	pair<BYTE, UBYTE> readByte(const ADDR);
	pair<HWORD, UBYTE> readHword(const ADDR);
	pair<WORD, UBYTE> readWord(const ADDR);		
	WORD readInstr(const ADDR);
		
	BOOL setBreak(const ADDR addr, UINT tag);
	BOOL clearBreak(const ADDR addr, UINT tag);	
	BOOL clearMem(void);
	BOOL initMem(void);

	virtual UINT readHandler(const ADDR addr, const UBYTE tag) = 0;	
	virtual UINT writeHandler(const ADDR addr, const UBYTE tag) = 0;
	
	virtual UINT curthread(void) { return _curthread; }
	virtual void curthread(UINT ct) { _curthread = ct; }

	virtual pair<ADDR, ADDR> cBufAddrReg(INT id) = 0;
	virtual void cBufAddrReg(ADDR start, ADDR end, INT id) = 0;

	//setSRAMBase assume start address is SRAM base address
	//without any checking and trigger event, kernel internal use only
	virtual void setSRAMBaseWord(const ADDR addr, WORD data) = 0;	
	virtual void setSRAMBaseHword(const ADDR addr, HWORD data) = 0;	
	virtual void setSRAMBaseByte(const ADDR addr, BYTE data) = 0;	
	
	//get/set pair, without any checking and trigger event, kernel internal use only
	virtual void setByte(const ADDR, const BYTE); 
	virtual BYTE getByte(const ADDR);
	virtual void setHword(const ADDR, const HWORD); 
	virtual HWORD getHword(const ADDR); 
	virtual void setWord(const ADDR, const WORD); 
	virtual WORD getWord(const ADDR); 
	virtual void setSRAMByte(const ADDR, const BYTE) = 0; 
	virtual void setSRAMHword(const ADDR, const HWORD) = 0;  
	virtual void setSRAMWord(const ADDR, const WORD) = 0;  	
	
	virtual void registerDevice(InterfaceHandler* object, ADDR start, size_t size) = 0;
	virtual void registerMemoryRange(UINT type, ADDR start, size_t size, bool readOnly) = 0;
};

INLINE UBYTE* CoreMMU::getPageTagPtr(ADDR pageStartAddr) {
	// get the tag of memory (text area)
	return memory()->getPageTagPtr(pageStartAddr); 
}

INLINE UBYTE CoreMMU::writeByte(const ADDR addr, const BYTE byte) {
	return memory()->writeByte(addr, byte);
}

INLINE UBYTE CoreMMU::writeHword(const ADDR addr, const HWORD hword) {
	return memory()->writeHword(addr, hword);
}

INLINE UBYTE CoreMMU::writeWord(const ADDR addr, const WORD word) {
	return memory()->writeWord(addr, word);
}

INLINE pair<BYTE, UBYTE> CoreMMU::readByte(const ADDR addr) {
	return memory()->readByte(addr);
}

INLINE pair<HWORD, UBYTE> CoreMMU::readHword(const ADDR addr) {
	return memory()->readHword(addr);
}

INLINE pair<WORD, UBYTE> CoreMMU::readWord(const ADDR addr) {
	return memory()->readWord(addr);
}

	
INLINE WORD CoreMMU::readInstr(const ADDR pc) {
//	AppFatal((checkTextRange(pc)==ET_NORMAL), ("Instr Fetch: access non-text area @0x%08x.", pc));
	pair<WORD, UBYTE> raw = memory()->readInstr(pc);
	return raw.first;
}

INLINE void CoreMMU::setByte(const ADDR addr, BYTE byte) {
	memory()->setByte(addr, byte);
}

	
INLINE void CoreMMU::setHword(const ADDR addr, HWORD hword) {
	memory()->setHword(addr, hword);
}

	
INLINE void CoreMMU::setWord(const ADDR addr, WORD word) {
	memory()->setWord(addr, word);
}

	
INLINE BYTE CoreMMU::getByte(const ADDR addr) {
	return memory()->getByte(addr);
}

	
INLINE HWORD CoreMMU::getHword(const ADDR addr) {
	return memory()->getHword(addr);
}

	
INLINE WORD CoreMMU::getWord(const ADDR addr) {
	return memory()->getWord(addr);
}

#endif /*CMMU_H_*/
