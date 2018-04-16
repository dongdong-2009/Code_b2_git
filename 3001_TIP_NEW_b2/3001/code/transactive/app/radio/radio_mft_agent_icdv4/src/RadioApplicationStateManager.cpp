/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/radio/radio_mft_agent_icdv4/src/RadioApplicationStateManager.h"
#include "core/synchronisation/src/ThreadGuard.h"

RadioApplicationStateManager::RadioApplicationStateManager() : m_state(RA_UNKNOWN)
{

}


RadioApplicationStateManager::~RadioApplicationStateManager()
{

}

ApplicationState RadioApplicationStateManager::getApState()
{
    TA_Base_Core::ThreadGuard guard(m_lock);
    return m_state;
}

void RadioApplicationStateManager::setApState(ApplicationState state)
{
    TA_Base_Core::ThreadGuard guard(m_lock);
    m_state = state;
}

