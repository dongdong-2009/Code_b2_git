/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.cpp $
  * @author:  Alexis Laredo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEvent.h"


namespace TA_IRS_App 
{

    SDSEvent::SDSEvent() : m_event(NULL)
	{
		FUNCTION_ENTRY( "Constructor" );


		FUNCTION_EXIT;
	}

	SDSEvent::~SDSEvent()
	{
		FUNCTION_ENTRY( "Destructor" );

		if (NULL != m_event) {
			delete m_event;
			m_event = NULL;
		}

		FUNCTION_EXIT;
	}


    SDTSProtocolEvent* SDSEvent::getProtocolEvent()
    {
        return m_event;
    }

    void SDSEvent::setProtocolEvent(SDTSProtocolEvent* event)
    {
        m_event = event;
    }

}
