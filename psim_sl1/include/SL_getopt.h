/* getopt.h */
/* Declarations for getopt.
   Copyright (C) 1989-1994, 1996-1999, 2001 Free Software 
   Foundation, Inc. This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute 
   it and/or modify it under the terms of the GNU Lesser 
   General Public License as published by the Free Software 
   Foundation; either version 2.1 of the License, or 
   (at your option) any later version.

   The GNU C Library is distributed in the hope that it will 
   be useful, but WITHOUT ANY WARRANTY; without even the 
   implied warranty of MERCHANTABILITY or FITNESS FOR A 
   PARTICULAR PURPOSE.  See the GNU Lesser General Public 
   License for more details.

   You should have received a copy of the GNU Lesser General 
   Public License along with the GNU C Library; if not, write 
   to the Free Software Foundation, Inc., 59 Temple Place,
   Suite 330, Boston, MA 02111-1307 USA.  */

    

#ifndef _SL_GETOPT_H

# define _SL_GETOPT_H 1

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

/* Describe the long-named options requested by the application.
   The LONG_OPTIONS argument to getopt_long or getopt_long_only is a vector
   of `struct option' terminated by an element containing a name which is
   zero.

   The field `has_arg' is:
   no_argument          (or 0) if the option does not take an argument,
   required_argument    (or 1) if the option requires an argument,
   optional_argument    (or 2) if the option takes an optional argument.

   If the field `flag' is not NULL, it points to a variable that is set
   to the value given in the field `val' when the option is found, but
   left unchanged if the option is not found.

   To have a long-named option do something other than set an `int' to
   a compiled-in constant, such as set a value from `optarg', set the
   option's `flag' field to zero and its `val' field to a nonzero
   value (the equivalent single-letter option character, if there is
   one).  For long options that have a zero `flag' field, `getopt'
   returns the contents of the `val' field.  */

struct option
{
	char *name;
	/* has_arg can't be an enum because some compilers complain about
	 type mismatches in all the code that assumes it is an int.  */
	int has_arg;
	int *flag;
	int val;
	int type;
	void *pVal;			//pointer to a buffer that holds the option value
	char *pValDef;	    //default value for this option
	struct StrToIntStruct *conversionTable;	// only string-to-integer type needs this
	char *descriptions;
};

/* Names for the values of the `has_arg' field of `struct option'.  */

# define no_argument            0
# define required_argument      1
# define optional_argument      2
# define multiple_arguments     3
# define OPT_LINK 0x8888

enum OptionIds
{
	TSR_PARAM_CONFIG_FILE=10000,
	HELPOPTIONID,
	EXT_OPTID_BASE=8000,
};


// Below section for enhancement
#define OptionType_BOOL     0
#define OptionType_STR		1
#define OptionType_INT		2
#define OptionType_X        3
#define OptionType_FLOAT	4
#define OptionType_ENUM		5

struct StrToIntStruct
{
	char *name;
	int val;
};

/*
Return code for parsing options
*/
enum {
	OPT_PARSE_OK=0,
	OPT_ERROR_PARA_NOT_FOUND=-1,
	OPT_ERROR_INVALID_TYPE=-2,
	OPT_ERROR_ILLEGAL_SECTION_DEF=-3,
	OPT_ERROR_ILLEGAL_PARA_DEF=-4,
	OPT_ERROR_ILLEGAL_PARA_VALUE=-5,
	OPT_ERROR_CONFIG_FILE_NOT_FOUND=-6,
};

extern int SL_getopt_long (int ___argc, char *const *___argv,
                        const char *__shortopts,
                        const struct option *__longopts, 
						const struct option **__longind);

extern int SL_getopt_long_only (int ___argc, char *const *___argv,
                             const char *__shortopts,
                             const struct option *__longopts, 
							 const struct option **__longind);
							 //int *__longind);

/* Internal only.  Users should not call this directly.  */
extern int SL_getopt_internal (int ___argc, char *const *___argv,
                             const char *__shortopts,
                             const struct option *__longopts, int *__longind,
                             int __long_only);
struct option *GetNextOption(struct option *cur);
int ParseFile(char *filename, struct option *optTable);
int ParseAnOptionByName(char *optionName, char *optionArg, struct option *optTable);
int ParseAnOptionById(int id, char *optionArg, struct option *optTable);
int DumpOptionsValue(struct option *opts);
int DumpOptionDescriptions(struct option *opts);
void *GetOptionByName(char *optionName);
bool FindOption(char *optionName);
int ParseInternalArguments(int argc, char *argv[], struct option *optTable);
int  IsConfigFileProcessed(void);
int  ParseSysConfigFile(char *cfgFile);
int LinkOptionTable(struct option *optTable);
void UseOwnCopy(bool b);


void InitArgs(void (*pUsageFunc)(char *), void (*pVersionFunc)(), struct option *optTable);

#endif /* getopt.h */
