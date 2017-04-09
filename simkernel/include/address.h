#ifndef MEMDEFS_H_
#define MEMDEFS_H_

#include "defs.h"

//address range flag
#define RANGE_FLAG_NORMAL 0x0
#define RANGE_FLAG_HANDLE 0x1
#define RANGE_FLAG_READONLY 0x2

//framebuffer
#define FB_CTRL_SIZE	(32)
#define FB_LCD_WIDTH	320
#define FB_LCD_HEIGHT	240
#define FB_SIZE		(320*240*INT32_BYTE)
#define FB_CTRL_BASE	0x81000000
#define FB_BASE_START (FB_CTRL_BASE+FB_CTRL_SIZE)

//fb control register address 
#define FB_POU_READ_CTRL_OFFSET (0)
#define FB_BASE_OFFSET (4)
#define FB_WIDTH_OFFSET (8)
#define FB_HEIGHT_OFFSET (12)
#define FB_DRAWABLE_WIDTH_OFFSET (16)
#define FB_DRAWABLE_HEIGHT_OFFSET (20)
#define FB_LCD_ON_OFFSET (24)
#define FB_POU_READ_CTRL (FB_CTRL_BASE+FB_POU_READ_CTRL_OFFSET)
#define FB_BASE (FB_CTRL_BASE+FB_BASE_OFFSET)
#define FB_WIDTH (FB_CTRL_BASE+FB_WIDTH_OFFSET)
#define FB_HEIGHT (FB_CTRL_BASE+FB_HEIGHT_OFFSET)
#define FB_DRAWABLE_WIDTH (FB_CTRL_BASE+FB_DRAWABLE_WIDTH_OFFSET)
#define FB_DRAWABLE_HEIGHT (FB_CTRL_BASE+FB_DRAWABLE_HEIGHT_OFFSET)
#define FB_LCD_ON (FB_CTRL_BASE+FB_LCD_ON_OFFSET)

#define PAGE_LOG_SIZE		11	// 2K
#define PAGE_SIZE			(1 << PAGE_LOG_SIZE)	
#define PAGE_OFFSET_MASK 	(PAGE_SIZE - 1)
#define PAGE_ADDR_MASK 		(~PAGE_OFFSET_MASK)
#define INSTR_PAGE_SIZE	   (PAGE_SIZE/MIN_BYTES_PER_INSTR)
#define INSTR_OFFSET_MASK   (INSTR_PAGE_SIZE-1)
#define MAX_PAGE			(1 << ((TARG_WORD_SIZE * 8) - PAGE_LOG_SIZE))
#define ILLEGAL_PAGE_NUM	(MAX_PAGE+1)

class Address {
	private:
	
	public:
	UINT getPageNum(const ADDR addr) {
		return (addr >> PAGE_LOG_SIZE);
	}
	UINT getPageOffset(const ADDR addr){
		return (addr & (ADDR) PAGE_OFFSET_MASK);
	}
	UINT getInstrOffset(const ADDR addr){
		return ((addr>>MIN_BITS_ADDR_SHIFT) & (ADDR) INSTR_OFFSET_MASK);
	}	
	ADDR getPageAddr(const ADDR addr) {
		return (addr & PAGE_ADDR_MASK);
	}
	UINT getPageSize(ADDR addr) {
		return (UINT) PAGE_SIZE;
	}		
};

#endif /*MEMDEFS_H_*/
