#ifndef PROPERTIES_H_
#define PROPERTIES_H_

#include <string>

#include "singleton.h"

class CProperties
{
public:
	bool _enPseudoSerial;
	CProperties()
	{
		_enPseudoSerial = false;
	}
	

	void enablePseudoSerial(bool en)
	{
		_enPseudoSerial = en;
	}
	bool enablePseudoSerial()
	{
		return _enPseudoSerial;
	}
};

typedef Singleton<CProperties> Properties;

#endif /* PROPERTIES_H_ */
