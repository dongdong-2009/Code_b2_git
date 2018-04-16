#pragma once

#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"

namespace TA_IRS_App
{
	class CCTVConfiguration
	{
	public:
		static CCTVConfiguration& getinstance();
		void setDefaultDwellTime(unsigned short dwellTime);
		void setCCTVSubsystemKey(unsigned long cctvSubsystemKey);
		void setConsole(TA_Base_Core::IConsole* console);
		void setLocation(TA_Base_Core::ILocation* location);

		unsigned short getDefaultDwellTime();
		unsigned long getCCTVSubsystemKey();
		unsigned long getLocationKey();
		TA_Base_Core::ILocation::ELocationType getLocationType();
		unsigned long getConsoleKey();
		TA_Base_Core::ILocation* getLocation();
		TA_Base_Core::IConsole* getConsole();

	private:
		CCTVConfiguration();
		TA_Base_Core::PrimitiveWrapper<unsigned short> m_defaultDwellTime;
		TA_Base_Core::PrimitiveWrapper<unsigned long> m_cctvSubsystemKey;
		TA_Base_Core::PrimitiveWrapper<TA_Base_Core::IConsole*> m_console;
		TA_Base_Core::PrimitiveWrapper<TA_Base_Core::ILocation*> m_location;
		static CCTVConfiguration s_instance;
	};
}
