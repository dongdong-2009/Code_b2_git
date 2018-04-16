#include "core/data_access_interface/bas/src/Mode.h"
#include "core/data_access_interface/bas/src/ModeHelper.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{
	Mode::Mode(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper(new ModeHelper(row, data))
	{
		FUNCTION_ENTRY("Mode()");
		FUNCTION_EXIT;
	}

	Mode::~Mode(void)
	{
		FUNCTION_ENTRY("~Mode()");
		FUNCTION_EXIT;
	}

	unsigned long Mode::getKey()
	{
		FUNCTION_ENTRY("getKey");
		FUNCTION_EXIT;
		return m_helper->getKey();
	}

	std::string Mode::getName()
	{
		FUNCTION_ENTRY("getName");
		FUNCTION_EXIT;
		return m_helper->getName();
	}

	unsigned int Mode::getValue()
	{
		FUNCTION_ENTRY("getValue");
		FUNCTION_EXIT;
		return m_helper->getValue();
	}

	std::string Mode::getDescription()
	{
		FUNCTION_ENTRY("getDescription");
		FUNCTION_EXIT;
		return m_helper->getDescription();
	}

	unsigned int Mode::getLocationKey()
	{
		FUNCTION_ENTRY("getLocationKey");
		FUNCTION_EXIT;
		return m_helper->getLocationKey();
	}

	unsigned int Mode::getSubsystemKey()
	{
		FUNCTION_ENTRY("getSubsystemKey");
		FUNCTION_EXIT;
		return m_helper->getSubsystemKey();
	}

	unsigned int Mode::getTypeKey()
	{
		FUNCTION_ENTRY("getTypeKey");
		FUNCTION_EXIT;
		return m_helper->getTypeKey();
	}

	void Mode::invalidate()
	{
		FUNCTION_ENTRY("invalidate");		
		m_helper->invalidate();
		FUNCTION_EXIT;
	}

	std::string Mode::getLocationName()
	{
		return m_helper->getLocationName();
	}

	std::string Mode::getSubsystemName()
	{
		return m_helper->getSubsystemName();
	}

	std::string Mode::getTypeName()
	{
		return m_helper->getTypeName();
	}
}
