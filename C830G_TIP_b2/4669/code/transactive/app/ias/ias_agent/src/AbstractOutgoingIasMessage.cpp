/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ias/ias_agent/src/AbstractOutgoingIasMessage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
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


