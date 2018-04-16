/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioServerAgent/src/RadioServerAgentStatusManager.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

//#include "bus/radio/RadioBus/src/radiotypes.h"

#include "app/radio/RadioServerAgent/src/RadioServerAgentStatusManager.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/data_access_interface/radio/src/RadioPredefinedTextMessageFactory.h"
#include "core/data_access_interface/radio/src/IRadioPredefinedTextMessage.h"


using namespace TA_IRS_Core;

RadioServerAgentStatusManager::RadioServerAgentStatusManager()
{
	FUNCTION_ENTRY("constructor");
	/**
	 *	Has yet to kwnow what is needed;
	 */ 
	FUNCTION_EXIT;
}


RadioServerAgentStatusManager::~RadioServerAgentStatusManager()
{
	FUNCTION_ENTRY("destructor");
	/**
	 *	Has yet to kwnow what is needed;
	 */ 
	FUNCTION_EXIT;
}

