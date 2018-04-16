/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ias/ias_agent/src/AbstractOutgoingIasMessage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  AbstractOutgoingIasMessage.cpp
//  Implementation of the Class AbstractOutgoingIasMessage
//  Created on:      04-Mar-2005 08:23:56 AM
///////////////////////////////////////////////////////////

#include "AbstractOutgoingIasMessage.h"


AbstractOutgoingIasMessage::AbstractOutgoingIasMessage(){

}



AbstractOutgoingIasMessage::~AbstractOutgoingIasMessage(){

}


/**
 * Construct by calling AbstractIasMessage()
 * - Use the current time for the dateTime parameter.
 * - USe "COM" for the systemCode parameter.
 * @param transactionNumber
 * @param repeatNumber
 * @param myHostId
 * @param receiveParty
 * 
 */
AbstractOutgoingIasMessage::AbstractOutgoingIasMessage(int transactionNumber, int repeatNumber, int myHostId, int receiveParty){

}


