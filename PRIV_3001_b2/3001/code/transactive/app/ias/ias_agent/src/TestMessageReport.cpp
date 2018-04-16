/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/TestMessageReport.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  TestMessageReport.cpp
//  Implementation of the Class TestMessageReport
//  Created on:      04-Mar-2005 08:24:16 AM
///////////////////////////////////////////////////////////

#include "TestMessageReport.h"


TestMessageReport::TestMessageReport(){

}



TestMessageReport::~TestMessageReport(){

}


/**
 * This constructor is used to construct a Test Message Report using a raw stream
 * of bytes.  When the message is successfully interpreted, the bytes must be
 * removed from the original stream.  Ensure that upon completion, all member
 * variables from AbstactIasMessage are set.
 * 
 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
 * 
 * Throws IasInvalidMessageException if the packet size is not the right size (37
 * bytes).
 * @param rawData
 * 
 */
void TestMessageReport::TestMessageReport(std::vector<unsigned char>& rawData){

}


/**
 * Return "#A71"
 */
const std::string TestMessageReport::getMessageId(){

}


bool TestMessageReport::hasConnectionFault(){

}


