#ifndef DEFS_H_
#define DEFS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#if (__GNUC__ >= 3)
	#include <map>
	#include <ext/hash_map>
	using std::pair;
	using std::vector;
	using std::map;
	using __gnu_cxx::hash_map;
	using std::equal_to;
#else
	#include <hash_map>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _LANGUAGE_C
#ifndef inline
#ifdef Is_True_On
#define inline static
#else
#define inline static __inline
#endif
#endif
#endif

#if (defined(__GNUC__))
typedef int		BOOL;	/* Natural size Boolean value */
typedef signed int	INT;	/* The natural integer on the host */
typedef signed char	INT8;	/* Use the natural integer */
typedef signed char	CHAR;	/* Character */
typedef signed short	INT16;	/* Use the natural integer */
typedef signed int	INT32;	/* The natural integer matches */
typedef signed long long INT64;	/* The natural integer matches */
typedef signed long LONG;	/* The natural long matches */
typedef unsigned long	INTPTR;	/* Integer the same size as pointer*/
typedef unsigned int	UINT;	/* The natural integer on the host */
typedef unsigned char	UINT8;	/* Use the natural integer */
typedef unsigned short	UINT16;	/* Use the natural integer */
typedef unsigned int	UINT32;	/* The natural integer matches */
typedef unsigned long long	UINT64;	/* The natural integer matches */
typedef unsigned long ULONG;	/* The natural long matches */

typedef unsigned int 	ADDR;
typedef signed char		BYTE;	
typedef unsigned char	UBYTE;	
typedef signed int 		WORD;
typedef signed short 	HWORD;
typedef unsigned short 	UHWORD;
typedef unsigned int 	UWORD;
typedef signed long long DWORD;
typedef unsigned long long UDWORD;

/* thread id mapping */

#define THREAD_ID_0 0
#define THREAD_ID_1 1
#define THREAD_ID_2 2
#define THREAD_ID_3 3

/* Number of bytes of each datatype */
#define INT8_BYTE 1
#define INT16_BYTE 2
#define INT32_BYTE 4
#define HWORD_BYTE 2
#define WORD_BYTE 4
#define DWORD_BYTE 8

/* Number of hex width of each datatype in printf*/
#define INT8_HEX_WIDTH 2
#define INT16_HEX_WIDTH 4
#define INT32_HEX_WIDTH 8
#define INT64_HEX_WIDTH 16

/* Number of bytes of instruction*/
#define INSTR16 2
#define INSTR32 4

/* branch/jump offset base shift bits */
#define OFFSET_SHIFT_BR 1
#define OFFSET_SHIFT_JP 1
#define OFFSET_SHIFT_BR_16 1

/* Number of bits of each datatype */
#define INT8_BIT 8
#define INT16_BIT 16
#define INT24_BIT 24
#define INT32_BIT 32
#define HWORD_BIT 16
#define WORD_BIT 32
#define DOUBLEWORD_BIT 64

/* Provide some limits that match the above types */
#define	INT8_MAX	127		/* Max 8-bit int */
#define	INT8_MIN	(-127)		/* Min 8-bit int */
#define	UINT8_MAX	255u		/* Max 8-bit unsigned int */
#define	INT16_MAX	32767		/* Max 16-bit int */
#define	INT16_MIN	(-32768)	/* Min 16-bit int */
#define	UINT16_MAX	65535u		/* Max 16-bit unsigned int */
#define	INT32_MAX	2147483647	/* Max 32-bit int */
#define	INT32_MIN	(-2147483647-1)	/* Min 32-bit int */
#define	UINT32_MAX	4294967295u	/* Max 32-bit unsigned int */

#undef INLINE
#define INLINE inline

#define ERROR_RET -1
#define TARG_WORD_SIZE 4
#define EVEN_REG_MASK (0x1)
#define HWORD_ALIGN_MASK (0x1)
#define WORD_ALIGN_MASK (0x3)
#define HWORD_INSTR_MASK (0x2)
#define WORD_DATA_MASK (0xffffffff)
#define HWORD_DATA_MASK (0xffff)
#define UPPER_HWORD_DATA_MASK (0xffff0000)
#define BYTE_DATA_MASK (0xff)
#define MAX_INST_PER_WORD 2  // (16 bits or 32 bits)
#define MIN_BITS_ADDR_SHIFT 1
#define MIN_BYTES_PER_INSTR (1<<MIN_BITS_ADDR_SHIFT)

#endif // __GNUC__

typedef char *STRING;

/* Define standard values: */
#ifndef TRUE
#define TRUE	((BOOL) 1)
#endif
#ifndef FALSE
#define FALSE	((BOOL) 0)
#endif

/* Define standard functions: */
#ifdef MAXf
# undef MAX
#endif
#ifdef MIN
# undef MIN
#endif
#ifdef ABS
# undef ABS
#endif
#define MAX(a,b)	((a>=b)?a:b)
#define MIN(a,b)	((a<=b)?a:b)
#define ABS(a)	((a>=0)?a:-a)

inline INT Max(INT i, INT j)
{
  return MAX(i,j);
}
inline INT Min(INT i, INT j)
{
  return MIN(i,j);
}
inline INT Abs(INT i)
{
  return ABS(i);
}

#ifdef __cplusplus
}
#endif
/*VRuntime*/
/*page size*/
#define ADD_PAGE_SIZE 4096
#define PAGE_MASK_LOW 0xfffff000
#define PAGE_MASK_HIGH 0x00000fff
/*for mmap and munmap*/
#define MAX_ALLOC_NUM 25600 
#define MAX_ALLOC_PAGE 256000
#define LIST_USED 1
#define LIST_NOT_INIT 0

#include "messg.h"

#endif /*DEFS_H_*/
