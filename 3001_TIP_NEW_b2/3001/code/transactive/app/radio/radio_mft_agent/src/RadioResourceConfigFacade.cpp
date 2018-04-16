/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_mft_agent/src/RadioResourceConfigFacade.cpp $
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
#include "app/radio/radio_mft_agent/src/RadioResourceConfigFacade.h"
//#include "app/radio/radio_mft_agent/src/RadioMFTAgentEventImpl.h"
#include "app/radio/radio_mft_agent/src/IMcdbRefreshCallback.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentHelper.h"

#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"

const short RadioResourceConfigFacade::DB_INIT_ISCS_MODE = 0;
const short RadioResourceConfigFacade::DB_INIT_RADIO_MODE = 1;
const short RadioResourceConfigFacade::DB_SYNC_RADIO_MODE = 2;
const short RadioResourceConfigFacade::DB_UPDATE_MODE = 3;

RadioResourceConfigFacade::RadioResourceConfigFacade() :
    m_MCDB(NULL), m_eventImpl(NULL)
{
    FUNCTION_ENTRY("RadioResourceConfigFacade");
	//m_retrievedDatabase = false;
	m_opMode = DB_INIT_ISCS_MODE;
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
	//Make sure that the MCC Connection status is OK before processing DB Refresh Complete
	if ( false == The_StatusManager::instance()->getMccConnectionStatus() )
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MCC Connection status is not OK, DB Refresh Complete will not be called.");
		return;
	}

    m_eventImpl->onMcdbRefreshComplete(m_opMode);
}
void RadioResourceConfigFacade::setIAPIRequest_ptr(MCC7500SDK::IAPIRequest_ptr& ptr, TA_Base_Core::ReEntrantThreadLockable & apiLock)
{
	MDB::MDBQueryTimeout mdbQueryTimeoutSettings;
	mdbQueryTimeoutSettings.MaxRetries = RadioMFTAgentHelper::getMaxRadioRetries();
	mdbQueryTimeoutSettings.TalkGroupQueryTimeout = RadioMFTAgentHelper::getMaxTalkgroupQueryTimeout();
	mdbQueryTimeoutSettings.RadioUserQueryTimeout = RadioMFTAgentHelper::getMaxRadioUserQueryTimeout();
	mdbQueryTimeoutSettings.DatabaseMCCQueryTimeout = RadioMFTAgentHelper::getDatabaseMCCQueryTimeout();

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MaxRetries :%d, MaxTalkgroupQueryTimeout :%d, MaxRadioUserQueryTimeout :%d",
		   mdbQueryTimeoutSettings.MaxRetries, mdbQueryTimeoutSettings.TalkGroupQueryTimeout, mdbQueryTimeoutSettings.RadioUserQueryTimeout );
	
	m_MCDB=new MDB(this,ptr, apiLock, mdbQueryTimeoutSettings);
}
void RadioResourceConfigFacade::setEventImpl(IMcdbRefreshCallback* pImpl)
{
    m_eventImpl = pImpl;
}

void RadioResourceConfigFacade::startRetrieveDB(short mode)
{
	//if (m_retrievedDatabase)
	//{
	//	return;
	//}
	//else
	//{
	//	m_retrievedDatabase = true;
	//}
	m_opMode = mode;
	if (DB_INIT_ISCS_MODE == m_opMode) {
		m_eventImpl->onMcdbRefreshComplete(m_opMode);
	} else {
		if(m_MCDB==NULL)
			return;
		m_MCDB->terminateAndWait();
		m_MCDB->start();
	}
}

bool RadioResourceConfigFacade::getRadioResource(std::vector<RadioResource>& radioResource)
{
    return m_MCDB->getRadioResource(radioResource);
}

void RadioResourceConfigFacade::reconnectMCC()
{
	//Call MCC reconnection here
	The_RadioSystemStatus::instance()->restartConnection();

}