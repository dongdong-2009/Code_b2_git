/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ias/ias_agent/src/StatusReportProcessor.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  StatusReportProcessor.cpp
//  Implementation of the Class StatusReportProcessor
//  Created on:      04-Mar-2005 08:24:15 AM
///////////////////////////////////////////////////////////

#include "StatusReportProcessor.h"


StatusReportProcessor::StatusReportProcessor(){

}



StatusReportProcessor::~StatusReportProcessor(){

}


/**
 * @param taDataPointAccess    ItaIasAgentSummary cast into an ItaIasAgentSpecific
 * for access to DataPoint control functionality.
 * @param outgoingMessageSender    The IOutgoingMessageSender instance used to
 * send the IasMessageAcknowledgement packet to the required IAS Server.
 * 
 */
StatusReportProcessor::StatusReportProcessor(ItaIasAgentSpecific& taDataPointAccess, IOutgoingMessageSender& outgoingMessageSender){

}


/**
 * Called by the IasAgent to process the next IasStatusReport received from the
 * IAS servers.  After the datapoints are updated, an IasStatusAcknowledgement
 * message is created and sent back to the respective IAS server.
 * 
 * If the IasInvalidMessageException is thrown in the IasMessage construction
 * process, a debug warning message should be logged.
 * @param message
 * 
 */
void StatusReportProcessor::processMessage(IasStatusReport& message){

}


