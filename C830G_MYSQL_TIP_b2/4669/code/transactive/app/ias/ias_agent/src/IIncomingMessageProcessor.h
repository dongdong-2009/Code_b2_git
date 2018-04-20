/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ias/ias_agent/src/IIncomingMessageProcessor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  IIncomingMessageProcessor.h
//  Implementation of the Interface IIncomingMessageProcessor
//  Created on:      04-Mar-2005 08:24:11 AM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////

#if !defined(IIncomingMessageProcessor_73BE4116_DBA2_4195_9EC4_AE73BCE93AC6__INCLUDED_)
#define IIncomingMessageProcessor_73BE4116_DBA2_4195_9EC4_AE73BCE93AC6__INCLUDED_

/**
 * Simple interface to split dependencies between business logic classes and
 * protocol classes.
 * @author Sean Liew
 * @version 1.0
 * @created 04-Mar-2005 08:24:12 AM
 */
class IIncomingMessageProcessor
{

public:
	/**
	 * Notify the class to process a received IasStatusReport packet.
	 * @param theIasStatusReport
	 * 
	 */
	virtual void processIasStatusReport(const IasStatusReport& theIasStatusReport) =0;
	/**
	 * Notify the class to process a received TestMessageReport packet.
	 * @param testMessageReport
	 * 
	 */
	virtual void processTestMessageReport(const TestMessageReport& testMessageReport) =0;

};
#endif // !defined(IIncomingMessageProcessor_73BE4116_DBA2_4195_9EC4_AE73BCE93AC6__INCLUDED_)