#ifndef ACCESSLIST_H_
#define ACCESSLIST_H_

#include "simdefs.h"

extern const STRING range_idx_name[];

class InterfaceHandler;

struct LayoutItem {
	UINT type;
	ADDR start;
	ADDR end;
	InterfaceHandler* object;
	struct LayoutItem* next;
};

class AccessList {
	private:
	struct LayoutItem* _list;
	
	private:
	struct LayoutItem* _newItem(UINT type, ADDR start, ADDR end, InterfaceHandler* object);
		
	public:
	AccessList(void);
	void insert(UINT type, ADDR start, ADDR end, InterfaceHandler* object);
	void remove(InterfaceHandler* object);
	pair<UINT, InterfaceHandler*> search(ADDR addr);
	ADDR getAddr(ADDR addr, UINT type);
	UINT getType(ADDR addr);
	void printList(void);
};

#endif /*ACCESSLIST_H_*/
