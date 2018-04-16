/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ias/ias_agent/src/StatusReportProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  StatusReportProcessor.h
//  Implementation of the Class StatusReportProcessor
//  Created on:      04-Mar-2005 08:24:15 AM
///////////////////////////////////////////////////////////

#if !defined(StatusReportProcessor_04E4E939_A4CB_4867_A533_2324DAEAD78F__INCLUDED_)
#define StatusReportProcessor_04E4E939_A4CB_4867_A533_2324DAEAD78F__INCLUDED_

#include "IOutgoingMessageSender.h"

/**
 * The StatusReportProcessor class is driven by the MessageProcessor thread to
 * process IasStatusReport messages received from the sockets.  The result is the
 * update of the station/depot IAS datapoint values and qualities.  To conclude
 * the operation, a IasStatusAcknowledgement message is sent back to the IAS
 * server.
 * @version 1.0
 * @created 04-Mar-2005 08:24:15 AM
 */
class StatusReportProcessor
{

public:
	StatusReportProcessor();
	virtual ~StatusReportProcessor();

public:
	/**
	 * @param taDataPointAccess    ItaIasAgentSummary cast into an ItaIasAgentSpecific
	 * for access to DataPoint control functionality.
	 * @param outgoingMessageSender    The IOutgoingMessageSender instance used to
	 * send the IasMessageAcknowledgement packet to the required IAS Server.
	 * 
	 */
	StatusReportProcessor(ItaIasAgentSpecific& taDataPointAccess, IOutgoingMessageSender& outgoingMessageSender);
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
	void processMessage(IasStatusReport& message);

protected:
	/**
	 * ItaIasAgentSummary cast into an ItaIasAgentSpecific for access to DataPoint
	 * control functionality.
	 */
	ItaIasAgentSpecific& m_taDataPointAccess;
	/**
	 * The IOutgoingMessageSender instance used to send the IasMessageAcknowledgement
	 * packet to the required IAS Server.
	 */
	IOutgoingMessageSender& m_outgoingMessageSender;

};
#endif // !defined(StatusReportProcessor_04E4E939_A4CB_4867_A533_2324DAEAD78F__INCLUDED_)