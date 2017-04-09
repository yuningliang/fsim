#include "accesslist.h"

AccessList::AccessList() {
	_list = NULL;
}

struct LayoutItem* AccessList::_newItem(UINT type, ADDR start, ADDR end, InterfaceHandler* object) {
	struct LayoutItem* item = new struct LayoutItem;
	item->type = type;
	item->start = start;
	item->end = end;
	item->object = object;
	item->next = NULL;
	return item;
}

void AccessList::insert(UINT type, ADDR start, ADDR end, InterfaceHandler* object) {
	struct LayoutItem* node = _list;
	struct LayoutItem* item = _newItem(type, start, end, object);
	if(node==NULL) {
		_list = item;
	}
	else{
		if(start<node->start) {
			SimFatal((end<node->start), ("AccessList: Address overlapped (end=%x, nextNode start=%x).", end, node->start));
			 item->next = node;
			_list = item;
			return;
		}
		else {
			struct LayoutItem* preNode = node;
			while(node!=NULL) {
				if(start<node->start) {
					SimFatal((preNode==node||start>preNode->end), ("AccessList: Address overlapped (start=%x, preNode end=%x).", start, preNode->end)); 
					SimFatal((end<node->start), ("AccessList: Address overlapped (end=%x, nextNode start=%x).", end, node->start)); 
					item->next = node;
					preNode->next = item;
					return;
				}
				preNode = node;
				node = node->next;
			}
			preNode->next = item;			
		}
	}
}

void AccessList::remove(InterfaceHandler* object) {
	struct LayoutItem* node = _list;
	struct LayoutItem* preNode = node;
	while(node!=NULL) {
		if(object==node->object) {
			preNode->next = node->next;
			delete node;
			return;
		}
		preNode = node;
		node = node->next;
	}	
}

UINT AccessList::getType(ADDR addr) {
	struct LayoutItem* node = _list;
	while(node!=NULL) {
		if(addr>=node->start&&addr<node->end) {
			return node->type;
		}
		node = node->next;
	}
	return 0;	
}

ADDR AccessList::getAddr(ADDR addr, UINT type) {
	struct LayoutItem* node = _list;
	while(node!=NULL) {
		if(type==node->type) {
			ADDR realAddr = node->start + addr;
//			SimFatal((realAddr<=node->end), ("AccessList: Address out of range (end=%x, realAddr=%x).", node->end, realAddr));
			return realAddr;
		}
		node = node->next;
	}
	SimFatal((0), ("AccessList: Address type not register (type=%d (%s)).", type, range_idx_name[type]));
	return 0;	
}

pair<UINT, InterfaceHandler*> AccessList::search(ADDR addr) {
	struct LayoutItem* node = _list;
	pair<UINT, InterfaceHandler*> ret(0, NULL);
	while(node!=NULL) {
		if(addr>=node->start&&addr<node->end) {
			ret.first = node->type;
			ret.second = node->object;
			return ret;
		}
		node = node->next;
	}
	return ret;	
}

void AccessList::printList() {
	struct LayoutItem* node = _list;
	INT i = 0;
	while(node!=NULL) {
		fprintf(stdout, "%d: %s %08x-%08x, %x\n"
					, i
					, range_idx_name[node->type]
					, node->start
					, node->end
					, node->object);
		i++;
		node = node->next;
	}

}

