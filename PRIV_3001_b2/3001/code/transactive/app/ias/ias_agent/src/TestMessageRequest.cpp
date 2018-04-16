/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/TestMessageRequest.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  TestMessageRequest.cpp
//  Implementation of the Class TestMessageRequest
//  Created on:      04-Mar-2005 08:24:17 AM
///////////////////////////////////////////////////////////

#include "TestMessageRequest.h"


TestMessageRequest::TestMessageRequest(){

}



TestMessageRequest::~TestMessageRequest(){

}


/**
 * Construct by calling AbstractOutgoingIasMessage()
 * - Use m_messageId for messageId.
 * - Use 0 for repeatNumber.
 * 
 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
 * 
 * @param transactionNumber
 * @param myHostId
 * @param receiveParty
 * 
 */
TestMessageRequest::TestMessageRequest(int transactionNumber, int myHostId, int receiveParty){

}


/**
 * Return "#M71"
 */
const std::string TestMessageRequest::getMessageId(){

}


bool TestMessageRequest::isMatchingResponseFound(){

}


/**
 * @param value
 * 
 */
void TestMessageRequest::setIsMatchingReponseFound(bool value){

}


/**
 * This pure virtual method must be implemented by derived classes so that a raw
 * byte stream can be produced ready for sending through a socket stream.
 */
std::vector<unsigned char> TestMessageRequest::getRawData(){

}


