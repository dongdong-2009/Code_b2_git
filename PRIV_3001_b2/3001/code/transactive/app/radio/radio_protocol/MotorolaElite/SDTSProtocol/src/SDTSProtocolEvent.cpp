/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $ Haipeng
  * 1: added the throw in the BuildMsg() function
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App {
    
    const int SDTSProtocolEvent::ADAPTATION_LENGTH = 2;

	SDTSProtocolEvent::SDTSProtocolEvent() : rawMsgInitialised(false)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}

	SDTSProtocolEvent::~SDTSProtocolEvent()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	void SDTSProtocolEvent::buildMsg()
	{
		FUNCTION_ENTRY("buildMsg");
		FUNCTION_EXIT;
		TA_THROW(TA_Base_Core::TransactiveException("Do not call the base function!"));
	}

	std::vector<unsigned char> SDTSProtocolEvent::getRawMessage()
	{
		FUNCTION_ENTRY("vector<unsigned char> SDTSProtocolEvent::getRawMessage");

		if ( !rawMsgInitialised )
		{
			buildMsg();
		}

		FUNCTION_EXIT;		
		return rawMsg;
	}

	void SDTSProtocolEvent::setRawMessage(const std::vector<unsigned char>& data)
	{
		FUNCTION_ENTRY("setRawMessage");

		rawMsgInitialised = true;
		rawMsg = data;

		FUNCTION_EXIT;
	}

	unsigned int SDTSProtocolEvent::getAdaptationLength()
	{
	    FUNCTION_ENTRY("getAdaptationLength");
		FUNCTION_EXIT;
		return adaptionLength;
	}

	void SDTSProtocolEvent::setAdaptationLength(unsigned int nAdaptation)
	{
		FUNCTION_ENTRY("setAdaptationLength");

		adaptionLength = nAdaptation;

		FUNCTION_EXIT;
	}
}
