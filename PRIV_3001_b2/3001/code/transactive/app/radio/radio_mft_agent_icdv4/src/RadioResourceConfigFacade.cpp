/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 11:47:25 $
 * Last modified by:  $Author: builder $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioResourceConfigFacade.h"
#include "app/radio/radio_mft_agent_icdv4/src/IMcdbRefreshCallback.h"

RadioResourceConfigFacade::RadioResourceConfigFacade() :
    m_MCDB(NULL), m_eventImpl(NULL)
{
    FUNCTION_ENTRY("RadioResourceConfigFacade");
	m_retrievedDatabase = false;
    FUNCTION_EXIT;
}


RadioResourceConfigFacade::~RadioResourceConfigFacade()
{
    FUNCTION_ENTRY("~RadioResourceConfigFacade");
    m_MCDB->terminateAndWait();
    FUNCTION_EXIT;
}

void RadioResourceConfigFacade::notify()
{
    m_eventImpl->onMcdbRefreshComplete();
}
void RadioResourceConfigFacade::setIAPIRequest_ptr(MCC7500SDK::IAPIRequest_ptr &ptr)
{
m_MCDB=new MDB(this,ptr);
}
void RadioResourceConfigFacade::setEventImpl(ICDV4::IMcdbRefreshCallback* pImpl)
{
    m_eventImpl = pImpl;
}

void RadioResourceConfigFacade::startRetrieveDB()
{
	if (m_retrievedDatabase)
	{
		return;
	}
	else
	{
		m_retrievedDatabase = true;
	}
	if(m_MCDB==NULL)
		return;
    m_MCDB->start();
}

bool RadioResourceConfigFacade::getRadioResource(std::vector<RadioResource>& radioResource)
{
    return m_MCDB->getRadioResource(radioResource);
}

