#include "app/cctv/video_display_manager/src/Configuration.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

	CCTVConfiguration CCTVConfiguration::s_instance;

	CCTVConfiguration& CCTVConfiguration::getinstance()
	{
		return s_instance;
	}

	void CCTVConfiguration::setDefaultDwellTime( unsigned short dwellTime )
	{
		m_defaultDwellTime.setValue(dwellTime);
	}

	void CCTVConfiguration::setCCTVSubsystemKey( unsigned long cctvSubsystemKey )
	{
		m_cctvSubsystemKey.setValue(cctvSubsystemKey);
	}

	void CCTVConfiguration::setConsole( TA_Base_Core::IConsole* console )
	{
		m_console.setValue(console);
	}

	void CCTVConfiguration::setLocation( TA_Base_Core::ILocation* location )
	{
		m_location.setValue(location);
	}

	unsigned short CCTVConfiguration::getDefaultDwellTime()
	{
		TA_ASSERT(m_defaultDwellTime.hasBeenSet(), "");
		return m_defaultDwellTime.getValue();
	}

	unsigned long CCTVConfiguration::getCCTVSubsystemKey()
	{
		TA_ASSERT(m_cctvSubsystemKey.hasBeenSet(), "");
		return m_cctvSubsystemKey.getValue();
	}

	unsigned long CCTVConfiguration::getLocationKey()
	{
		return getLocation()->getKey();
	}

	TA_Base_Core::ILocation::ELocationType CCTVConfiguration::getLocationType()
	{
		return getLocation()->getLocationType();
	}

	unsigned long CCTVConfiguration::getConsoleKey()
	{
		return getConsole()->getKey();
	}

	TA_Base_Core::ILocation* CCTVConfiguration::getLocation()
	{
		TA_ASSERT(m_location.hasBeenSet(), "");
		return m_location.getValue();
	}

	TA_Base_Core::IConsole* CCTVConfiguration::getConsole()
	{
		TA_ASSERT(m_console.hasBeenSet(), "");
		return m_console.getValue();
	}

	CCTVConfiguration::CCTVConfiguration()
	{
	}



}