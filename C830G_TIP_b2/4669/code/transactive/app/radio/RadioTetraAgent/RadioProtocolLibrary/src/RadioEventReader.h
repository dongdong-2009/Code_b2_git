/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioEventReader.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/09/23 18:48:34 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  RadioEventReader.h
//  Implementation of the Class RadioEventReader
//  Generated by Enterprise Architect
//  Created on:      04-Nov-2003 13:29:05
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(RadioEventReader_FD972C79_38BC_45b5_9F6E_E8C2F7EBDE44__INCLUDED_)
#define RadioEventReader_FD972C79_38BC_45b5_9F6E_E8C2F7EBDE44__INCLUDED_

#include "RadioProtocol.h"
#include <string>

/**
* This class is used to read the contents of the radio event. It is important as
* it has to ensure that the parameters are correctly converted from the
* transmitted little endian for to the appropriate form for the platform. This
* class is an abstract version used to derive all the actual events.
* @version 1.0
* @generator Enterprise Architect
* @created 04-Nov-2003 13:29:05
*/

namespace TA_IRS_App
{
	class RadioEvent;

	typedef unsigned long   EventType;
	typedef unsigned long   EventLengthType;

	class RadioEventReader
	{
		public:
			RadioEventReader(RadioEvent& event, EventType eventType,EventLengthType parameterLength);

            /**
             * Constructor
             *
             * Used to construct a RadioEventReader where the length of the message is variable 
             * (i.e. contains a LONGSTRING)
             */
            RadioEventReader(RadioEvent& event, EventType eventType);
			virtual ~RadioEventReader();

			const RadioEvent& getEvent() const { return m_event; };
            bool isStatusOK();

		protected:

			short getParameterShort(unsigned int position) const;
			unsigned long getParameterLong(unsigned int position) const;
			std::string getParameterShortString(unsigned int position) const;
			unsigned char getParameterChar(unsigned int position) const;

		private:

			RadioEvent&   m_event;

	};
};

#endif // !defined(RadioEventReader_FD972C79_38BC_45b5_9F6E_E8C2F7EBDE44__INCLUDED_)
