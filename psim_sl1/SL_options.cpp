/* /////////////////////////////// "SL_getopt.cpp" /////////////////////////////////
 *  File: SL_getopt.cpp
 * 
 * 	Copyright (c) 2006 Beijing SimpLight Nanoelectornics, Ltd.
 * 	All rights reserved.
 *
 *  Modified: 2006/11/08
 */

/*
The option system features:
.Easy to use. Nothing more than define a variable, its type and its option name.
	The variable is for internal program.
	The option name is for external use of program users
	The type is used to stream the input from users given either in the command line
	or environment to the variable defined in the program.
.Provide default values.
.A central place to group all the global options that can be changed outside during runtime.
.Be able to dump all the values.
.Builtin usage dump.

Author: Tuo Lingyun
Date: Dec 2002
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "SL_getopt.h"

#define tprintf7 printf
#define stricmp strcasecmp

static int nargc=0;
static char **nargv;
static bool bConfigFileProcessed=false;
static struct option *GlobalOptTable;
static void (*pUsage)(char *);
static void (*pVersion)();

// If it doesn't need private copy of the option value, set to false;
static bool bOwnCopy = true;

char *optionTypeNames[] =
{
	"bool",
	"string",
	"integer",
	"X_integer",
	"float",
	"enumerate",
	NULL
};

struct StrToIntStruct StrToBoolTable[] =
{
	{"0",     0},
	{"1",     1},
	{"true",  1},
	{"false", 0},
	{"yes",   1},
	{"no",    0},
	{NULL}
};


void UseOwnCopy(bool b)
{
	bOwnCopy = b;
}

int DumpOptionsValue(option *opts)
{
	option *cur;

	printf("\nDump options table:\n");
	printf("\tname\ttype\tcurrent value\tdefault value\n");

	for (cur=&opts[0]; cur; cur=GetNextOption(cur))
	{
		fseek(stdout, 0, SEEK_END);
		printf("\t%s\t%s\t", cur->name, optionTypeNames[cur->type]);
		if (!cur->pVal)
		{
			printf("~~~~\t");
		}
		else switch(cur->type)
		{
		case OptionType_X:
		    printf("0x%x\t", *(int*)(cur->pVal));
		    break;
		case OptionType_INT:
		case OptionType_ENUM:
			printf("%d\t", *(int*)(cur->pVal));
			break;
		case OptionType_BOOL:
			printf("%s\t", (*(bool*)cur->pVal)?"true":"false");
			break;
		case OptionType_FLOAT:
			printf("%f\t", *(float*)(cur->pVal));
			break;
		case OptionType_STR:
			if (cur->pVal)
				printf("%s\t", (char*)(cur->pVal));
			else
				printf("\t");
			break;
		default:
			printf("Option system error: Invalid type %d defined for option %s\n", cur->type, cur->name);
			return -1;
		}
		if (cur->pValDef)
			printf("%s\n", cur->pValDef);
	}
	printf("\n");
	return 0;
}

int CountDisplayPos(char *str)
{
	int cc=0;
	for (char *p=str; *p; p++)
	{
		if (*p!='\t')
			cc++;
		else 
		{
			cc += 8;
			cc = cc&(~7);
		}
	}
	return cc;
}

int FormatWrapper(char *str, int lineLimit, char *lineLead)
{
	tprintf7("%s",str);
	return 0;
	
	int cc;
	int leadCount=CountDisplayPos(lineLead);

	char *p=str;
	cc = 0;
	do
	{
		for (int i=0; i<cc; i++)
			tprintf7(" ");

		for (; *p && cc<lineLimit; p++)
		{
			tprintf7("%c", *p);
			if (*p=='\n')
			{
				cc=0;
				p++;
				break;
			}
			else if (*p=='\t')
			{
				cc += 8;
				cc = cc&(~7);
			}
			else
				cc++;

			if (cc>=lineLimit)
			{
				cc=0;
				p++;
				tprintf7("\n");
				break;
			}
		}
		cc = leadCount;
	} while (*p);

	return 0;
}

int DumpOptionDescriptions(option *opts)
{
	option *cur;
	char buf[4096];

	fprintf(stdout, "Options:\n");
	for (cur=&opts[0]; cur; cur=GetNextOption(cur))
	{
		if (cur->pValDef==NULL || strlen(cur->pValDef)==0)
			sprintf(buf, "    \t%s.\n",	cur->descriptions);
		else
			sprintf(buf, "    \t%s. Default:%s.\n", cur->descriptions, cur->pValDef);
		FormatWrapper(buf, 75, "        ");
	}
	return 0;
}

/*

  Intel compiler won't allow below macro.

#define printf NULL
*/
int ConvertStrToX(const char *valStr)
{
	int len = strlen(valStr);
	int num[len];
	int i=0,j;
	if(valStr[1]=='x')
	   i=2;
	for(j=i;j<len;j++){
	   switch(valStr[j]){
	      case 'F':
	      case 'f':
	        num[j]=15;
	        break;
	      case 'E':
	      case 'e':
	        num[j]=14;
	        break;
	      case 'D':
	      case 'd':
	        num[j]=13;
	        break;
	      case 'C':
	      case 'c':
	        num[j]=12;
	        break;
	      case 'B':
	      case 'b':
	        num[j]=11;
	        break;
	      case 'A':
	      case 'a':
	        num[j]=10;
	        break;
	      case '9':
	        num[j]=9;
	        break;
	      case '8':
	        num[j]=8;
	        break;
	      case '7':
	        num[j]=7;
	        break;
	      case '6':
	        num[j]=6;
	        break;
	      case '5':
	        num[j]=5;
	        break;
	      case '4':
	        num[j]=4;
	        break;
	      case '3':
	        num[j]=3;
	        break;  
	      case '2':
	        num[j]=2;
	        break;
	      case '1':
	        num[j]=1;
	        break;
	      case '0':
	        num[j]=0;
	        break;   
	      default:
	         fprintf(stderr, "invalid arguement %s\n", valStr);
			 exit(-1);  
	   }
	}
	int res=0,s=1;
	for(j=len-1;j>=i;j--)
	{
	   res += num[j]*s;
	   s = s*16;
	}
	return res;                 
	        
}
int ConvertStrToInt(const char *optName, char *valStr, struct StrToIntStruct *convTable)
{
	int i;
	for (i=0; convTable[i].name; i++)
	{
		if (stricmp(valStr,convTable[i].name)==0)
		{
			return convTable[i].val;
		}
	}
	// impossible
	tprintf7("Option: Invalid value '%s' specified for option -%s\n", valStr, optName);
	exit(-1);//return 0;
}

int SetOptionValue(option *pFound, char *valStr)
{
//	printf("SetOptionValue:%s --> %s\n", pFound->name, valStr);

	if (!valStr)
	{
		fprintf(stderr, "missing value for option %s\n", pFound->name);
		exit(-1);
	}
		
	if (!pFound->pVal)
	{
		if (bOwnCopy)
			pFound->pVal = strdup(valStr);
		else
			pFound->pVal = valStr;
	}
	else switch (pFound->type)
	{
	case OptionType_X:
	    *(int*)(pFound->pVal) = ConvertStrToX(valStr);
	    break;
	case OptionType_INT:
		*(int*)(pFound->pVal) = atoi(valStr);
		break;
	case OptionType_BOOL:
		*(bool*)(pFound->pVal) = ConvertStrToInt(pFound->name, valStr, StrToBoolTable)?true:false;
		break;
	case OptionType_ENUM:
		*(int*)(pFound->pVal) = ConvertStrToInt(pFound->name, valStr, pFound->conversionTable);
		break;
	case OptionType_FLOAT:
		*(float*)(pFound->pVal) = (float)atof(valStr);
		break;
	case OptionType_STR:
		if (pFound->has_arg == multiple_arguments) {
			char *p = (char*)pFound->pVal;
			if (p[0])
				strcat(p, ";");
			strcat(p, valStr);
//			printf("multiple arguments for option %s is [%s]\n", pFound->name, (char*)pFound->pVal);
		} else {
			strcpy((char*)pFound->pVal, valStr);
		}
		
		break;
	default:
		fprintf(stderr, "Option system error: Invalid type %d defined for option %s\n", pFound->type, valStr);
		return OPT_ERROR_INVALID_TYPE;
	}
	return OPT_PARSE_OK;
}

int ParseAnOption(option *pFound, char *optionArg)
{
//	printf("ParseAnOption: %s --> %s\n", pFound->name, optionArg);
	
	if ((optionArg==NULL) || (*optionArg==0))
	{
		if (pFound->has_arg == required_argument)	// must have argument
		{
			fprintf(stderr, "%s: missing arguments\n", pFound->name);
			exit(-1);
		}
		else if (pFound->has_arg == optional_argument)	// optionally have argument
		{
			// We don't have an argument to set. So we just return.
			return SetOptionValue(pFound, "true");
		}
		else
		{
			// This option must be a bool. Try to set the bool variable to true.
			return SetOptionValue(pFound, "true");
		}
	}
	else
	{
		return SetOptionValue(pFound, optionArg);
	}

	return OPT_PARSE_OK;
}

int ParseAnOptionByName(char *optionName, char *optionArg, option *optTable)
{
	// discard at most two leading hypens in option name
	if (*optionName=='-')
	{
		optionName++;
		if (*optionName=='-')
			optionName++;
	}

	option *cur;
	// find the item in option table
	for (cur=&optTable[0]; cur; cur=GetNextOption(cur))
	{
		if (strcmp(optionName, cur->name)==0)
		{
			return ParseAnOption(cur, optionArg);
		}
	}
	fprintf(stderr, "Invalid option %s\n", optionName);
	return OPT_ERROR_PARA_NOT_FOUND;
}

void *GetOptionByName(char *optionName)
{
	option *cur;
	// find the item in option table
	for (cur=&GlobalOptTable[0]; cur; cur=GetNextOption(cur))
	{
		if (strcmp(optionName, cur->name)==0)
		{
			if (cur->pVal&&((char*)cur->pVal)[0])
				return cur->pVal;
			else
				return cur->pValDef;
		}
	}
	fprintf(stderr, "GetOptionByName:Invalid option %s\n", optionName);
	return NULL;
}

bool FindOption(char *optionName)
{
	option *cur;
	// find the item in option table
	for (cur=&GlobalOptTable[0]; cur; cur=GetNextOption(cur))
	{
		if (strcmp(optionName, cur->name)==0)
		{
			return true;
		}
	}
	return false;
}

int ParseAnOptionById(int id, char *optionArg, option *optTable)
{
	option *cur;
	// find the item in option table
	for (cur=&optTable[0]; cur; cur=GetNextOption(cur))
	{
		if (cur->val == id)
		{
			return ParseAnOption(cur, optionArg);
		}
	}
	return OPT_ERROR_PARA_NOT_FOUND;
}

/************************************************************************
*	Name:			ParseFile(char *filename)
*	Descriptions:	Parse a configuration file with section defined
*      Configuration file looks like:
*          # comment
*          [section1]
*          para1=val1
*          [section2]
*          para2=val2
*	Parameters:	
*				file	[in] A text file that specifies system parameters
*				optTable	[in] The systen parameter table
*	Return value:
*				0	Parsing complete successfully
*               <0  error occurred
*	Remarks:
*          A parameter defined in the configuration file within a group
*          must be defined in the parameter(option) table in below format:
*          "sectioname:parametername".
**************************************************************************/
int ParseFile(char *filename, struct option *optTable)
{
	char curSection[128];
	char buf[256];
	char paraName[128], paraValue[128];
	FILE *fp;
	char *p, *q;

	if ((fp=fopen(filename, "rt"))==NULL)
	{
		tprintf7("Can't open configuration file %s\n", filename);
		return OPT_ERROR_CONFIG_FILE_NOT_FOUND;
	}

	curSection[0] = '\0';
	while(!feof(fp))
	{
		if (fgets(buf, 256, fp) == NULL)
			break;
		buf[255] = '\0';
		p = strchr(buf, '\n');
		if (p)
			*p = '\0';
		p = strchr(buf, '\r');
		if (p)
			*p = '\0';

		// Discard leading spaces
		p = buf;
		while(strchr(" \t", *p) && *p)
			p ++;
		if (!*p)
			continue;	// discard blank line
		if (*p == '#' || *p == ';')
			continue;	// skip comments

		if (*p == '[')
		{
			p++;
			q = strchr(p, ']');
			if (!q)
			{
				tprintf7("Illegal section name definition:%s\n", buf);
				fclose(fp);
				return OPT_ERROR_ILLEGAL_SECTION_DEF;
			}
			else
			{
				// get current section name
				*q = '\0';
				if (*p)
					sscanf(p, "%s", curSection);
				else
					curSection[0] = '\0';	// allow anonymous section
				printf("Current section name:[%s]\n", curSection);
			}
		}
		else
		{
			// Find the parameter value
			q = strchr(p, '=');
			if (!q)
			{
				tprintf7("Illegal parameter definition:%s\n", buf);
				fclose(fp);
				return OPT_ERROR_ILLEGAL_PARA_DEF;
			}
			else
			{
				*q = '\0';
				sscanf(p, "%s", paraName);
				q++;
				if (*q)
					strcpy(paraValue, q);
				else
					paraValue[0] = '\0';

				char longName[256];
				if (curSection[0])
				{
					strcpy(longName, curSection);
					strcat(longName, ":");
					strcat(longName, paraName);
				}
				else
				{
					strcpy(longName, paraName);
				}
				printf("parameter:[%s]=[%s]\n", longName, paraValue);
				int retval;
				if ((retval=ParseAnOptionByName(longName, paraValue, optTable))!=0)
				{
					printf("parse option error\n");
				}
			}
		}
	}
	fclose(fp);
	return OPT_PARSE_OK;
}

int IsConfigFileProcessed(void)
{
	return bConfigFileProcessed;
}

int LinkOptionTable(struct option *optTable)
{
	if (optTable == NULL)
		return 0;
	// Find the tail of the global table
	int i;
	for (i=0; GlobalOptTable[i].name!=NULL && GlobalOptTable[i].has_arg!=OPT_LINK; i++)
	{
	}
	// Link it
	GlobalOptTable[i].name    = (char *)optTable;
	GlobalOptTable[i].has_arg = OPT_LINK;
	return 0;
}

int ParseSysConfigFile(char *cfgFile)
{
	if (!GlobalOptTable)
	{
		tprintf7("GlobalOptTable has not been initialized:%s %d\n", __FILE__, __LINE__);
		exit(-1);
	}
	return ParseFile(cfgFile, GlobalOptTable);
}

void InitArgs(void (*pUsageFunc)(char *), void (*pVersionFunc)(), struct option *optTable)
{
	pUsage = pUsageFunc;
	pVersion = pVersionFunc;
	GlobalOptTable = optTable;
}

int ParseInternalArguments(int argc, char *argv[], struct option *optTable)
{
	int c;

	if (argc >= 1 )
	{
		// save for later use
		nargc = argc;
		nargv = argv;
	}
#undef argc
#undef argv

	// reset option index
	optind = 1;

	while ((c = SL_getopt_long(nargc, nargv, "v", GlobalOptTable, NULL)) != -1)
	{
//	printf("long option id:%d\n", c);
		switch(c)
		{
		case HELPOPTIONID:
			if (pUsage)
			{
				(*pUsage)(nargv[0]);
				exit(-1);
			}
			break;
		case 'v':
			if (pVersion)
			{
				(*pVersion)();
				exit(-1);
			}
			break;
		case '?':
			if (pUsage)
			{
				(*pUsage)(nargv[0]);
				exit(-1);
			}
			break;
		default:
			if (ParseAnOptionById(c, optarg, GlobalOptTable) != OPT_PARSE_OK)
			{
				fprintf(stderr, "Can't parse option %s\n", nargv[optind]);
				exit (-1);
			}
			break;
		}
	}

	if (argc==1 && pUsage)
		(*pUsage)(nargv[0]);

	return 0;
}

