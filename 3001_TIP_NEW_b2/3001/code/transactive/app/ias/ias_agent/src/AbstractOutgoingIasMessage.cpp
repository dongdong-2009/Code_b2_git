/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/AbstractOutgoingIasMessage.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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


