/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/IasStatusAcknowledgement.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  IasStatusAcknowledgement.cpp
//  Implementation of the Class IasStatusAcknowledgement
//  Created on:      04-Mar-2005 08:24:10 AM
///////////////////////////////////////////////////////////

#include "IasStatusAcknowledgement.h"


IasStatusAcknowledgement::IasStatusAcknowledgement(){

}



IasStatusAcknowledgement::~IasStatusAcknowledgement(){

}


/**
 * Construct by calling AbstractOutgoingIasMessage()
 * - Use m_messageId for messageId.
 * - Use iasStatusReport for transactionNumber and repeatNumber.
 * 
 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
 * 
 * @param iasStatusReport    The recently received IasStatusReport packet to
 * obtain the transactionNumber and repeatNumber from.
 * @param myHostId
 * @param receiveParty
 * 
 */
IasStatusAcknowledgement::IasStatusAcknowledgement(IasStatusReport& iasStatusReport, int myHostId, int receiveParty){

}


/**
 * Return "#A99"
 */
const std::string IasStatusAcknowledgement::getMessageId(){

}


/**
 * This pure virtual method must be implemented by derived classes so that a raw
 * byte stream can be produced ready for sending through a socket stream.
 */
std::vector<unsigned char> IasStatusAcknowledgement::getRawData(){

}


