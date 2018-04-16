/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/IOutgoingMessageSender.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  IOutgoingMessageSender.h
//  Implementation of the Interface IOutgoingMessageSender
//  Created on:      04-Mar-2005 08:24:12 AM
//  Original author: Sean Liew
///////////////////////////////////////////////////////////

#if !defined(IOutgoingMessageSender_31F40A2F_FD5A_497d_A99A_18F035874535__INCLUDED_)
#define IOutgoingMessageSender_31F40A2F_FD5A_497d_A99A_18F035874535__INCLUDED_

/**
 * Simple interface to split dependencies between business logic classes and
 * protocol classes.
 * @author Sean Liew
 * @version 1.0
 * @created 04-Mar-2005 08:24:12 AM
 */
class IOutgoingMessageSender
{

public:
	/**
	 * Notify the class to send an AbstractOutgoingIasMessage packet.
	 * @param outgoingMessage
	 * 
	 */
	virtual void sendMessage(AbstractOutgoingIasMessage& outgoingMessage) =0;

};
#endif // !defined(IOutgoingMessageSender_31F40A2F_FD5A_497d_A99A_18F035874535__INCLUDED_)