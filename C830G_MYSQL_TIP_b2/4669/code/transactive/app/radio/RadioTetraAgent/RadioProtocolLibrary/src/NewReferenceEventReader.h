/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/NewReferenceEventReader.h $
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
//  NewReferenceEventReader.h
//  Implementation of the Class NewReferenceEventReader
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 16:38:14
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////


#if !defined(NewReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)
#define NewReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
	class NewReferenceEventReader : public RadioEventReader
	{
	public:
		typedef unsigned long RadioReference;

		NewReferenceEventReader(RadioEvent& event);
		virtual ~NewReferenceEventReader();

	public:
		RadioReference getReference();
	};
};

#endif // !defined(NewReferenceEventReader_3A523F52_FC8F_4435_B2D9_A175B85C6AEF__INCLUDED_)