
// DoubleAtsFailureWorkItem.cpp: implementation of the DoubleAtsFailureWorkItem class.
//
//////////////////////////////////////////////////////////////////////

#include "app/ats/AtsAgent/src/DoubleAtsFailureWorkItem.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
	DoubleAtsFailureWorkItem::DoubleAtsFailureWorkItem(TA_IRS_Bus::TrainOperationModeCorbaProxyPtr observer, bool AtsFailure)
		: m_observer(observer), doubleAtsFailure(AtsFailure)
	{
		FUNCTION_ENTRY("DoubleAtsFailureWorkItem");
		FUNCTION_EXIT;
	}

	DoubleAtsFailureWorkItem::~DoubleAtsFailureWorkItem()
	{
		FUNCTION_ENTRY("~DoubleAtsFailureWorkItem");
		FUNCTION_EXIT;
	}

	void DoubleAtsFailureWorkItem::executeWorkItem()
	{
		FUNCTION_ENTRY("executeWorkItem");

		// call m_observer 
		m_observer->inDoubleAtsFailure(doubleAtsFailure);

		FUNCTION_EXIT;
	}

}
