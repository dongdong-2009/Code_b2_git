/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DisconnectEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Disconnect Event Reader
  *
  */
///////////////////////////////////////////////////////////
//
//  DisconnectEventReader.h
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


#if !defined(DisconnectEventReader_INCLUDED_)
#define DisconnectEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
	class DisconnectEventReader : public RadioEventReader
	{
		public:
			typedef unsigned long RadioReference;

			DisconnectEventReader(RadioEvent& event);
			virtual ~DisconnectEventReader();

		public:

			const RadioReference getCallReference() const;
			const std::string getReason() const;
    
		private:
			static const unsigned int DISCONNECT_REFERENCE_EVENT_LENGTH;
			static const unsigned int CALL_REFERENCE_START;
			static const unsigned int REASON_START;
	};
};

#endif // !defined(DisconnectEventReader_INCLUDED_)