/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDeleteSubscriber.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation for radio RadioMethodDeleteSubscriber API call
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioMethodDeleteSubscriber.cpp
//  Implementation of the Class RadioMethodDeleteSubscriber
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 15:15:18
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#include "RadioMethodDeleteSubscriber.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"


#define DELETE_SUBSCRIBER_METHOD_PARAMETER_LENGTH  4
#define SUBSCRIBER_REFERENCE_START                 0

using namespace TA_IRS_App;

/**
* Constructor
* @param    sessionRef
*/
RadioMethodDeleteSubscriber::RadioMethodDeleteSubscriber(RadioSessionReference sessionRef,RadioSubscriberReference subscriberRef) 
                        : RadioMethod(DELETE_SUBSCRIBER_METHOD_PARAMETER_LENGTH,METHOD_DELETE_SUBSCRIBER,sessionRef)
{
	FUNCTION_ENTRY("Constructor");
	setParameterLong(SUBSCRIBER_REFERENCE_START,subscriberRef);
	FUNCTION_EXIT;
}


RadioMethodDeleteSubscriber::~RadioMethodDeleteSubscriber()
{
	FUNCTION_ENTRY("Destructor");
	FUNCTION_EXIT;
}


