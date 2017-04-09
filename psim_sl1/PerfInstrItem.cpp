/* /////////////////////////////// "PerfInstrItem.cpp" /////////////////////////////////
 *  File: PerfInstrItem.cpp
 * 
 *  Descriptions:   Implementation for simulation execution
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Created: 2006/11/13
 *  Author: Lingyun Tuo
 */
 
#include <stdio.h>

#include "defs.h"
#include "sl1defs.h"
#include "PerfDefs.h"
#include "PerfInstrItem.h"

void InstrItem::dump()
{
	printf("thread=%d perfId=%d %08x %08x %x offset=%08x\n", 
		_threadId, _perfId, _pc, _raw, _brMeta, _dataAddr);
}

