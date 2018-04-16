/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.CPP $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"

namespace TA_IRS_App
{

    ConsoleStateManager::ConsoleStateManager()
    :m_clientID(0)
    {
        FUNCTION_ENTRY( "Constructor" );
        FUNCTION_EXIT;
    }

    ConsoleStateManager::~ConsoleStateManager ()
    {
        FUNCTION_ENTRY( "Destructor" );

        FUNCTION_EXIT;
    }
	void ConsoleStateManager::setClientID(unsigned char id)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"ClientID: %u",id);
		m_clientID=id;
	}
	unsigned char ConsoleStateManager::getClientID()
	{
		return m_clientID;
	}
}

