/*
	Copyright (C) 2005, 2006.  Free Software Foundation, Inc.

	This program is free software; you can redistribute it and/or modify it
	under the terms of version 2 of the GNU General Public License as
	published by the Free Software Foundation.

	This program is distributed in the hope that it would be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	Further, this software is distributed without any warranty that it is
	free of the rightful claim of any third person regarding infringement
	or the like.  Any license provided herein, whether implied or
	otherwise, applies only to this software file.  Patent licenses, if
	any, provided herein do not apply to combinations of this program with
	other software, or any other product whatsoever.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write the Free Software Foundation, Inc., 59
	Temple Place - Suite 330, Boston MA 02111-1307, USA.
*/


#ifndef PERFCACHE_H_
#define PERFCACHE_H_

#include "PerfDefs.h"

#define INSTR_CACHE_LINE_LOG_SIZE       5 //32 byte
#define INSTR_CACHE_LINE_SIZE           (1<<INSTR_CACHE_LINE_LOG_SIZE) //8 words
#define INSTR_CACHE_LOG_SIZE            14 //16k byte, 2-way associative
#define INSTR_CACHE_WAY                 2 //2-way associative
#define INSTR_CACHE_SIZE                (1<<INSTR_CACHE_LOG_SIZE) //in byte
#define INSTR_CACHE_INDEX_MASK          0xff8
#define INSTR_CACHE_INDEX_SHIFT_VALUE   3
#define INSTR_CACHE_TAG_MASK            0x7ffff000
#define INSTR_CACHE_TAG_SHIFT_VALUE     12
#define INSTR_CACHE_ENTYR_NUMBER        512
//#define INSTR_CACHE_MASK                (INSTR_CACHE_SIZE-1)
//#define INSTR_CACHE_OFFSET_MASK         (~INSTR_CACHE_MASK)
//#define DATA_CACHE_LINE_LOG_SIZE        0
//#define DATA_CACHE_LINE_SIZE            (1<<DATA_CACHE_LINE_LOG_SIZE)
//#define DATA_CACHE_LOG_SIZE             0
//#define DATA_CACHE_SIZE                 (1<<DATA_CACHE_LOG_SIZE)
//#define DATA_CACHE_MASK                 (DATA_CACHE_SIZE-1)
//#define DATA_CACHE_OFFSET_MASK          (~DATA_CACHE_MASK)
//#define DEFAULT_ICACHE_MISS_CYC         25
//#define DEFAULT_ICACHE_MISS_CYC       8

#define DATA_CACHE_LINE_LOG_SIZE        5
#define DATA_CACHE_LINE_SIZE            (1<<DATA_CACHE_LINE_LOG_SIZE) // 32 bytes
#define DATA_CACHE_LOG_SIZE             14
#define DATA_CACHE_SIZE                 (1<<DATA_CACHE_LOG_SIZE) //16k bytes
#define DATA_CACHE_WAY                  2

#define DATA_CACHE_INDEX_MASK           0x1FE0
#define DATA_CACHE_INDEX_SHIFT_VALUE    5
#define DATA_CACHE_TAG_MASK             0xFFFFe000
#define DATA_CACHE_TAG_SHIFT_VALUE      13
#define DATA_CACHE_ENTYR_NUMBER         256
           


#define DEFAULT_DCACHE_MISS_CYC         35
#define CACHE_INIT_FILL                 0

typedef struct icache_Item {UINT tag[INSTR_CACHE_WAY]; UINT rep_index;} ICacheItem;
typedef struct dcache_Item {UINT tag[DATA_CACHE_WAY]; UINT rep_index;} DCacheItem;

class PerfCache {
	private:
		ICacheItem _icache[INSTR_CACHE_ENTYR_NUMBER];
		DCacheItem _dcache[DATA_CACHE_ENTYR_NUMBER];
		BOOL _incICachePenalty;
		BOOL _incDCachePenalty;
		BOOL _incCache;
		ADDR _lastICacheAccess;
		UINT _icacheMissCyc;
		UINT _dcacheMissCyc;
		UINT _icacheMissed;
		UINT _dcacheMissed;
		UINT _icacheAccess;
		UINT _dcacheAccess;		
	
	private:
		BOOL _hasTag(UINT tag, UINT index);
		BOOL _dCacheHasTag(UINT index,UINT tag);
		
	public:
		PerfCache();
		void init();
		INT checkICache(ADDR);
		INT checkDCache(ADDR);
		BOOL incICachePenalty() { return _incICachePenalty; }
		void incICachePenalty(BOOL t) { _incICachePenalty = t; }
		BOOL incDCachePenalty() { return _incDCachePenalty; }
		void incDCachePenalty(BOOL t) { _incDCachePenalty = t; }
		BOOL incCache() { return _incCache; }
		void incCache(BOOL t) { _incCache = t; }
		UINT icacheMissCyc() { return (incICachePenalty()==TRUE?_icacheMissCyc:0); }
		void icacheMissCyc(UINT c) { _icacheMissCyc = c; }
		
		UINT dcacheMissCyc() { return (incDCachePenalty()==TRUE?_dcacheMissCyc:0); }		
		void dcacheMissCyc(UINT c) { _dcacheMissCyc = c; }
		UINT icacheAccess() { return _icacheAccess; }
		UINT dcacheAccess() { return _dcacheAccess; }		
		UINT icacheMissed() { return _icacheMissed; }
		UINT dcacheMissed() { return _dcacheMissed; }		
		//ADDR getTag(ADDR addr) { return addr&INSTR_CACHE_OFFSET_MASK; }
		//UINT getIndex(ADDR addr) { return (UINT)((addr&INSTR_CACHE_MASK)>>INSTR_CACHE_LINE_LOG_SIZE); }
		ADDR getRepIndex(UINT index) { return _icache[index].rep_index; }
		std::pair<ADDR, ADDR> getLastTag(ADDR addr);
		std::pair<UINT, ADDR> getRepTag(UINT index);
};

inline BOOL PerfCache::_hasTag(UINT tag, UINT index) {
	BOOL t = FALSE;
	for(INT j = 0; j < INSTR_CACHE_WAY; j++) {
		if(_icache[index].tag[j]==tag) {
			t = TRUE;
			_icache[index].rep_index = (++j)%INSTR_CACHE_WAY;
			break;
		}
	}
	if(t==FALSE) {
		UINT repl_index = _icache[index].rep_index;
		_icache[index].tag[repl_index] = tag;
		_icache[index+1].tag[repl_index] = tag;
		_icache[index].rep_index = (++repl_index)%INSTR_CACHE_WAY; //rep_index only change on one ray		
	}
	
	return t;
}

inline BOOL PerfCache::_dCacheHasTag(UINT index,UINT tag)
{
	BOOL t = FALSE;
	INT i;
	for(i=0;i<DATA_CACHE_WAY;i++)
	{
		if(_dcache[index].tag[i]==tag)
		{
			t = TRUE;
			_dcache[index].rep_index = (++i)%DATA_CACHE_WAY;
			break;
		}
	}
	if(t==FALSE)
	{
		UINT rep_index = _dcache[index].rep_index;
		_dcache[index].tag[rep_index] = tag;
		 _dcache[index].rep_index = (++rep_index)%DATA_CACHE_WAY;
	}
	return t;
}

#endif /*PERFCACHE_H_*/
