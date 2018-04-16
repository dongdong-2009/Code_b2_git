/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/AbstractIasMessage.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  AbstractIasMessage.cpp
//  Implementation of the Class AbstractIasMessage
//  Created on:      04-Mar-2005 08:23:55 AM
///////////////////////////////////////////////////////////

#include "AbstractIasMessage.h"


AbstractIasMessage::AbstractIasMessage(){

}



AbstractIasMessage::~AbstractIasMessage(){

}


/**
 * This constructor is used to construct the message using explicit fields.
 * 
 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
 * 
 * @param transactionNumber
 * @param repeatNumber
 * @param hostId
 * @param systemCode
 * @param dateTime    Number of seconds since dd mm 1970.
 * @param receiveParty
 * 
 */
AbstractIasMessage::AbstractIasMessage(int transactionNumber, int repeatNumber, int hostId, const std::string& systemCode, time_t dateTime, int receiveParty){

}


int AbstractIasMessage::getTransactionNumber(){

}


int AbstractIasMessage::getRepeatNumber(){

}


void AbstractIasMessage::incrementRepeatNumber(){

}


int AbstractIasMessage::getHostId(){

}


time_t AbstractIasMessage::getDateTime(){

}


int AbstractIasMessage::getReceiveParty(){

}


