/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DeleteReferenceEventReader.h $
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
//  DeleteReferenceEventReader.h
//  Implementation of the Class DeleteReferenceEventReader
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 16:38:14
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(DeleteReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)
#define DeleteReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
	class DeleteReferenceEventReader : public RadioEventReader
	{
	public:
		typedef unsigned long RadioReference;

		DeleteReferenceEventReader(RadioEvent& event);
		virtual ~DeleteReferenceEventReader();

	public:
		enum EReferenceType 
        {                      
           CALL_REFERENCE, 
		   SDS_REFERENCE, 
		   DATA_REFERENCE, 
		   SUBSCRIBER_REFERENCE,
           AUDIO_REFERENCE,
           CAD_REFERENCE,
           MONITOR_REFERENCE,
           SEARCH_REFERENCE 
        };

		const RadioReference getReference() const;
        const EReferenceType getReferenceType() const;
        

    private:
        static const unsigned int DELETE_REFERENCE_EVENT_LENGTH;
        static const unsigned int DELETE_REFERENCE_TYPE_START;
        static const unsigned int REFERENCE_TYPE_PARAMETER_POSITION;
	};
};

#endif // !defined(DeleteReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)
