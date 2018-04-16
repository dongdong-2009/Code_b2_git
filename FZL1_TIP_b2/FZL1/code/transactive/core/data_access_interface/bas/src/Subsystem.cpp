#include "core/data_access_interface/bas/src/Subsystem.h"
#include "core/data_access_interface/bas/src/SubsystemHelper.h"
#include "core/utilities/src/DebugUtil.h"
namespace TA_IRS_Core
{
	Subsystem::Subsystem(const unsigned long row, TA_Base_Core::IData& data)
		: m_helper(new SubsystemHelper(row, data))
	{
		FUNCTION_ENTRY("Subsystem()");
		FUNCTION_EXIT;
	}

	Subsystem::~Subsystem()
	{
		FUNCTION_ENTRY("~Subsystem()");
		FUNCTION_EXIT;
	}

	unsigned long Subsystem::getKey()
	{
		FUNCTION_ENTRY("getKey");
		FUNCTION_EXIT;
		return m_helper->getKey();
	}

	std::string Subsystem::getName()
	{
		FUNCTION_ENTRY("getName");
		FUNCTION_EXIT;
		return m_helper->getName();
	}

	std::string Subsystem::getAssetName()
	{
		FUNCTION_ENTRY("getAssetName");
		FUNCTION_EXIT;
		return m_helper->getAssetName();
	}

	bool Subsystem::getIsSystem()
	{
		FUNCTION_ENTRY("getIsSystem");
		FUNCTION_EXIT;
		return m_helper->getIsSystem();
	}
	bool Subsystem::getIsMerge()
	{
		FUNCTION_ENTRY("getIsMerge");
		FUNCTION_EXIT;
		return m_helper->getIsMerge();
    }

    unsigned long Subsystem::getParentKey()
    {
        FUNCTION_ENTRY("getMergeName");
        FUNCTION_EXIT;
        return m_helper->getParentKey();
    }

	std::string Subsystem::getMergeName()
	{
		FUNCTION_ENTRY("getMergeName");
		FUNCTION_EXIT;
		return m_helper->getMergeName();
	}

	unsigned long Subsystem::getOrderID()
	{
		FUNCTION_ENTRY("getOrderID");
		FUNCTION_EXIT;
		return m_helper->getOrderID();
	}

	void Subsystem::invalidate()
	{
		FUNCTION_ENTRY("invalidate");
		FUNCTION_EXIT;
		m_helper->invalidate();
	}
}
