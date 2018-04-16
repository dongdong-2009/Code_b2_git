/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/datapoint_library/src/NotifyEventProcessor.cpp $
  * @author  J. Welton
  * @version $Revision: #2 $
  * Last modification : $DateTime: 2012/02/06 16:15:14 $
  * Last modified by : $Author: haijun.li $
  *
  *
  *	The NotifyEventProcessor thread is responsible 
  * for generating events and submitting
  * them.
  *
  *
  */

#include <ctime>

#include "bus/scada/datapoint_library/src/NotifyEventProcessor.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{

	NotifyEventProcessor::NotifyEventProcessor()
	{
        FUNCTION_ENTRY("Constructor");

        start();

        FUNCTION_EXIT;
	}


	NotifyEventProcessor::~NotifyEventProcessor()
	{
        FUNCTION_ENTRY("Destructor");

        terminateAndWait();

        FUNCTION_EXIT;
	}

	void NotifyEventProcessor::processEvent( NotifyEventRequest * newEvent )
	{
        FUNCTION_ENTRY("processEvent");

		// construct the DescriptionParameters list from the new event
		DescriptionParameters descriptionParameters;
		NameValuePair pair1 ( "AuditMessage", newEvent->getMessage() );
		descriptionParameters.push_back ( &pair1 );

		// get the audit message sender
		AuditMessageSender * auditMessageSender =
		MessagePublicationManager::getInstance().getAuditMessageSender( newEvent->getMessageType() );

		// send the new audit message type
		auditMessageSender->sendAuditMessage ( newEvent->getMessageType(),	// message type
											 newEvent->getEntityKey(),		// entity key
											 descriptionParameters,			// description
											 "",							// details
											 newEvent->getSessionID(),		// session ID
											 "",							// alarm ID
											 "",							// incidentkey
											 "" );							// parent event ID

		// clean up
		delete auditMessageSender;
		auditMessageSender = 0;

        FUNCTION_EXIT;
	}

}

