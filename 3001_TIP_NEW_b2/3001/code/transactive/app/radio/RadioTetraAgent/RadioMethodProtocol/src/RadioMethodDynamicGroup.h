

#ifndef RADIO_METHOD_DYNAMIC_GROUP_H
#define RADIO_METHOD_DYNAMIC_GROUP_H

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodDynamicGroup : public RadioMethod
	{
	public:
		RadioMethodDynamicGroup(unsigned long ssi, std::string targetTalkgroupAlias, bool dgnaAction);
		virtual ~RadioMethodDynamicGroup();
	public:
		std::string getISSI(unsigned long ssi);
	};
};
#endif // end of RADIO_METHOD_DYNAMIC_GROUP_H