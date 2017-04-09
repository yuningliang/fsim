/* /////////////////////////////// "PerfDispatch.h" /////////////////////////////////
 *  File: PerfDispatch.h
 *  Descriptions:  Header file for issuing instructions from IWin
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
#ifndef PERFDISPATCH_H_
#define PERFDISPATCH_H_

#include "PerfIWin.h"
class PerfCache;
struct branch_take_not{
	UINT taken_count;
	UINT miss_count;
 };

class PerfDispatch : public PerfIWin
{
private:

	typedef hash_map<ADDR,struct branch_take_not> BranchList;
	typedef BranchList::iterator BranchListIter;
	BranchList _branchlist;
	
	PerfCache *p_cache;
	int _threadId;
	ISSUE_STATUS _issueStatus;
	int _maxIssued;
	int _curIssued;
	int _percallIssued;
	bool _incOrder;
	#ifdef FLUSH_PIPELINE
	bool _issue_stop;
	#endif
	UINT _hazardCnt[HS_MAX];
	UINT _emptyFE;
	int _curCycle;
	int _jrStalledCnt;
	int _brStalledCnt;
	int _brInstrCnt;
	int _brInstrMissCnt;
	int _dcStalledCnt;	// d$ miss
	int _totalIssued;
	int _totalNops;
	int _totalNop16s;
	int _lsInstr;
	ADDR _LastBrPc;
	ADDR _LastBrinstrPc;
public:
	PerfDispatch();
	~PerfDispatch();
	
	void setThreadId(int threadId_) { _threadId = threadId_; }
	void setMaxIssue(int maxIssued_) { _maxIssued = maxIssued_; }
	int  getMaxIssue(void) { return _maxIssued; }
	int  getCurIssued(void) { return _curIssued; }
	int getPercallIssued(void) {return _percallIssued;}
	void setCurIssued(int val_) { _curIssued = val_; }
	void incOrder(bool incOrder_) { _incOrder = incOrder_; }
	bool incOrder(void) { return _incOrder; }
	PerfCache* dcache() { return p_cache;}
	#ifdef FLUSH_PIPELINE
	void set_issue_stop(bool flag){_issue_stop = flag;}
	#endif

	ISSUE_STATUS issueStatus(void) { return _issueStatus; }
	void issueStatus(ISSUE_STATUS issueStatus_) { _issueStatus = issueStatus_; }
	
	ISSUE_STATUS issue(UINT cycle, UINT maxAllowed_=0);
	int update(void);
	static int updateParallel(PerfDispatch *disp, int num);
	
	void dumpPipeLine(FILE *fp);
	void dumpIWin(FILE *fp);
	void dumpStats(FILE *fp);
};

#endif /*PERFDISPATCH_H_*/
