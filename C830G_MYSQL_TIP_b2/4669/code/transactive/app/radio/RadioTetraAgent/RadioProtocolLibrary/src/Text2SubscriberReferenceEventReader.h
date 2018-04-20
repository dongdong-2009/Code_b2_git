/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/Text2SubscriberReferenceEventReader.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of New Reference Event Reader
  *
  */
///////////////////////////////////////////////////////////
//
//  Text2SubscriberReferenceEventReader.h
//  Implementation of the Class Text2SubscriberReferenceEventReader
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 16:38:14
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(Text2SubscriberReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)
#define Text2SubscriberReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
	class Text2SubscriberReferenceEventReader : public RadioEventReader
	{
	public:
		typedef unsigned long RadioReference;

		Text2SubscriberReferenceEventReader(RadioEvent& event);
		virtual ~Text2SubscriberReferenceEventReader();

	public:

        const std::string getTextReference() const;
        const RadioReference getSubscriberReference() const;
        

    private:
        static const unsigned int TEXT_2_SUBSCRIBER_REFERENCE_EVENT_LENGTH;
        static const unsigned int TEXT_REFERENCE_PARAMETER_START_POSITION;
        static const unsigned int SUBSCRIBER_REFERENCE_PARAMETER_POSITION;
	};
};

#endif // !defined(Text2SubscriberReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)
