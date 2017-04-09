#include "cmmu.h"

CoreMMU::CoreMMU(Address& addr) : _address(addr) {
	int i;
	_alloc[0].flag = LIST_USED;
	_alloc[0].start = 0;
	_alloc[0].page_num=_allocLimit;
    _alloc[0].next = MAX_ALLOC_NUM;
	_alloc[0].prev=MAX_ALLOC_NUM;
	_searchPos=0;
	for(i=1;i<MAX_ALLOC_NUM;i++){
		_alloc[i].flag=LIST_NOT_INIT;
		_alloc[i].next=MAX_ALLOC_NUM;
		_alloc[i].prev=MAX_ALLOC_NUM;
		}
}

void CoreMMU::init(void){
	int i;
	_alloc[0].flag = LIST_USED;
	_alloc[0].start = 0;
	_alloc[0].page_num=_allocLimit;
    _alloc[0].next = MAX_ALLOC_NUM;
	_alloc[0].prev=MAX_ALLOC_NUM;
	_searchPos=0;
	for(i=1;i<MAX_ALLOC_NUM;i++){
		_alloc[i].flag=LIST_NOT_INIT;
		_alloc[i].next=MAX_ALLOC_NUM;
		_alloc[i].prev=MAX_ALLOC_NUM;
		}
}

void CoreMMU::initFile(FILE* fp, const ADDR start) { 
	WORD data;
	ADDR addr = start;
	AppFatal((fp!=NULL), ("MMU: initFile with a NULL file pointer."));
	AppFatal(((start&WORD_ALIGN_MASK)==0), ("MMU error: unaligned word address %x.\n", start));
	char str[128];
	INT i = 0;
	BYTE inByte;
	BOOL isBin = FALSE;
	while(fread(&inByte, sizeof(BYTE), 1, fp)>0&&i<200) {
		if(inByte==0) {
			isBin = TRUE;
			break;
		}
		i++;
	}
	if(isBin==FALSE) {
		UINT len = 0;
		char tmp[512];
		i = 0;
		rewind(fp);
		while(fscanf(fp, "%s%[^\n]", str, tmp)!=EOF&&i<50) {
			if(len!=0&&len!=strlen(str)) {
				isBin = TRUE;
				break;
			}
			else {
				len = strlen(str);
			}
			i++;
		}	
	}
	rewind(fp);
	if(isBin==FALSE) {
		while(fscanf(fp, "%x%[^\n]", &data, str)!=EOF) {
			setWord(addr, data);
			addr+=INT32_BYTE;
		}
	}
	else {
		while(fread(&inByte, sizeof(BYTE), 1, fp)>0) {
			setByte(addr, inByte);
			addr+=INT8_BYTE;
		}		
	}
}

void CoreMMU::dumpFile(FILE* fp, const ADDR start, INT size) { 
	ADDR addr = start;
	AppFatal((fp!=NULL), ("MMU: dumpFile with a NULL file pointer."));
	AppFatal(((start&WORD_ALIGN_MASK)==0), ("MMU error: unaligned word address %x.\n", start));
	char tmp_str[1];
	while(size>0) {
#if 0
		fprintf(fp, "%08x\n", getWord(addr));
		addr+=INT32_BYTE;
		size--;
#else
		///*** Just work for test use current, should be deleted
		///*** Here size is BYTE
		tmp_str[0] = getByte(addr);
		fwrite(tmp_str, 1, 1, fp);
		addr+=INT8_BYTE;
		size--;		
#endif
	}
}


INT CoreMMU::getStrlen(const ADDR addr) { 
	ADDR a = addr;
	INT len = 0;
	while(getByte(a)!='\0') {
		len++;
		a++;
	}	
	return len; 
}

STRING CoreMMU::getString(const ADDR addr) {
	INT len = getStrlen(addr);
	STRING buf = new char[len+1];
	readBlock(addr, (BYTE*) buf, len);
	buf[len]=0;
	return buf; 
}

void CoreMMU::setString(const ADDR addr, const STRING str) { 
	INT len = strlen(str);
	writeBlock(addr, (BYTE*) str, len);	
}

	
void CoreMMU::readBlock(const ADDR start_addr, BYTE* block, int size) {
	ADDR addr = start_addr;
	ADDR end_addr = start_addr + size;
	while(addr<end_addr) {	
		*block = getByte(addr);
		addr++;
		block++;
	}
}

	
void CoreMMU::writeBlock(const ADDR start_addr, const BYTE* block, int size) {
	ADDR addr = start_addr;
	ADDR end_addr = start_addr + size;
	while(addr<end_addr) {	
		setByte(addr, *block);
		addr++;
		block++;
	}
}

	
void CoreMMU::initBlock(const ADDR start_addr, BYTE value, int size) {
	ADDR addr = start_addr;
	ADDR end_addr = start_addr + size;
	while(addr<end_addr) {	
		setByte(addr, value);
		addr++;
	}
}


	
BOOL CoreMMU::setBreak(const ADDR addr, UINT tag) {
	memory()->setBreak(addr, tag);
	return TRUE;
}

	
BOOL CoreMMU::clearBreak(const ADDR addr, UINT tag) {
	memory()->clearBreak(addr, tag);
	return TRUE;
}

	
BOOL CoreMMU::initMem(void) {
	memory()->init();
	return TRUE;
}

BOOL CoreMMU::clearMem(void) {
	memory()->clear();
	return TRUE;
}

