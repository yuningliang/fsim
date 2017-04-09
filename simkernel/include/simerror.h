#ifndef SIMERROR_H_
#define SIMERROR_H_

#include "defs.h"
#include "simdefs.h"

class SimException {
	public:
	
	protected:
	char _prefix[MAX_EXCEPTION_PREFIX];
	BOOL _isFatal;
	char _target[MAX_MESSAGE_LENGTH];
	
	protected:
	void prefix(STRING p) { strcpy(_prefix, p); }
	
	public:
	SimException(STRING msg, STRING dataType, UINT data) {
		prefix("Exception");
		sprintf(_target, "%s (%s=%x)", msg, dataType, data);
		_isFatal = FALSE;
	}
	SimException(STRING msg, STRING dataType, INT data) {
		prefix("Exception");
		sprintf(_target, "%s (%s=%d)", msg, dataType, data);
		_isFatal = FALSE;
	}	
	SimException(STRING msg, STRING dataType, STRING data) {
		prefix("Exception");
		sprintf(_target, "%s (%s=%s)", msg, dataType, data);
		_isFatal = FALSE;
	}
	BOOL isFatal(void) { return _isFatal; }
	void toString(FILE* out) { fprintf(out, "%s: %s\n", _prefix, _target); }
};

class BreakPointInvalidAddrException : public SimException {
	public:
	BreakPointInvalidAddrException(STRING msg, ADDR addr) : SimException(msg, "address", (UINT) addr) { 
		prefix("BreakPoint");
	}
};

class BreakPointInvalidIndexException : public SimException {
	public:
	BreakPointInvalidIndexException(STRING msg, INT index) : SimException(msg, "index", (INT) index) { 
		prefix("BreakPoint");
	}
};

class ProfilerException : public SimException {
	public:
	ProfilerException(STRING msg, STRING reason) : SimException(msg, "reason", reason) { 
		prefix("Profiler");
	}
};

#endif /*SIMERROR_H_*/
