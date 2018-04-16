/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSUnregisterSession.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/utilities/src/DebugUtil.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSUnregisterSession.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"


namespace TA_IRS_App {

	SDSUnregisterSession::SDSUnregisterSession(SDTSTransactionManager& transactionManager, 
                                               SDSQueueManager& queueManager) :
            m_transactionManager(transactionManager), m_queueManager(queueManager)
	{
		FUNCTION_ENTRY( "Constructor" );
		FUNCTION_EXIT;
	}


	SDSUnregisterSession::~SDSUnregisterSession()
	{
		FUNCTION_ENTRY( "Destructor" );
		FUNCTION_EXIT;
	}


	int SDSUnregisterSession::call()
	{
		FUNCTION_ENTRY("call");

		m_queueManager.disconnect();
		FUNCTION_EXIT;

		return 0;
	}
}
