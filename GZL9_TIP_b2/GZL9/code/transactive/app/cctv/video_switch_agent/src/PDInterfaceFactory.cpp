#include "core/data_access_interface/src/ILocation.h"

extern TA_Base_Core::ILocation* gLocalLocation;

namespace TA_IRS_App
{
	class PDInterfaceFactory;

	extern PDInterfaceFactory* gIMOSFactory;
	extern PDInterfaceFactory* gMODBUSFactory;

	PDInterfaceFactory* factory = NULL;

	void initPDInterfaceFactory()
	{
		if(gLocalLocation->getLocationType() == TA_Base_Core::ILocation::DPT)
		{
			factory = gIMOSFactory;
		}
		else
		{
			factory = gMODBUSFactory;
		}
	}

	PDInterfaceFactory& getPDInterfaceFactory()
	{
		return *factory;
	}
}