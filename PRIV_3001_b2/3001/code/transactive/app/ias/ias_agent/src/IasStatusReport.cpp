/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ias/ias_agent/src/IasStatusReport.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
///////////////////////////////////////////////////////////
//  IasStatusReport.cpp
//  Implementation of the Class IasStatusReport
//  Created on:      04-Mar-2005 08:24:10 AM
///////////////////////////////////////////////////////////

#include "IasStatusReport.h"


IasStatusReport::IasStatusReport(){

}



IasStatusReport::~IasStatusReport(){

}


/**
 * This constructor is used to construct an IAS Status Report using a raw stream
 * of bytes.  When the message is successfully interpreted, the bytes must be
 * removed from the original stream.  Ensure that upon completion, all member
 * variables from AbstactIasMessage are set.
 * 
 * Throws IasInvalidMessageException if there is a failure interpreting the inputs.
 * 
 * Throws IasInvalidMessageException if the packet size is not the right size (40
 * bytes).
 * @param rawData
 * 
 */
void IasStatusReport::IasStatusReport(std::vector<unsigned char>& rawData){

}


/**
 * Return "#A70"
 */
const std::string IasStatusReport::getMessageId(){

}


bool IasStatusReport::hasFault(){

}


bool IasStatusReport::hasIntruderAlarm(){

}


std::string IasStatusReport::getStation(){

}


