/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/LoginEventReader.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  DisconnectEventReader.cpp
//  Implementation of the Class DisconnectEventReader
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 16:38:14
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#include "LoginEventReader.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	const unsigned int LoginEventReader::LOGIN_EVENT_LENGTH =  32;
	const unsigned int LoginEventReader::DGNA_POSITION =  0;


    LoginEventReader::LoginEventReader(RadioEvent& event)
		   : RadioEventReader(event,EVENT_LOGIN,LOGIN_EVENT_LENGTH)
    {
    }

    LoginEventReader::~LoginEventReader()
    {
    }

    const std::string LoginEventReader::getDGNA() const
    {
		return getParameterShortString(DGNA_POSITION);
    }
     

}


