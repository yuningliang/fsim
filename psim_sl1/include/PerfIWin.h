/* /////////////////////////////// "PerfIWin.h" /////////////////////////////////
 *  File: PerfIWin.h
 *  Descriptions:  Define instruction container class for issuing instructions.
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/11
 *  Author: Lingyun Tuo
 */

#ifndef PERFIWIN_H_
#define PERFIWIN_H_

class PerfIWin
{
protected:
	ISimpleList _iwinList;	// instructions waiting for issuing
	InstrList _issuedList;	// instructions issued and in pipeline

public:
	PerfIWin();
	~PerfIWin();
	
	int dispatch(int cyc);
	int add(InstrItem *item, int cyc);
	void preUpdate(void);
	
	void setIWinSizeLmt(int lmt) { _iwinList.setMaxWords(lmt); }
	bool isIWinFull() { return _iwinList.isFull();}
	int IWinSize() {return _iwinList.size();}
	InstrList &getIssuedList() { return _issuedList; }
	PerfIWin *getIWin(void) { return this; }
	bool empty() { return _iwinList.empty() && _issuedList.empty(); }
};

#endif /*PERFIWIN_H_*/
