/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodSelect.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Select method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodSelect.h
//  Implementation of the Class RadioMethodSelect
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodSelect__INCLUDED_)
#define RadioMethodSelect__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodSelect : public RadioMethodCallReference 
	{
		public:
			RadioMethodSelect(RadioSessionReference sessionRef, RadioCallReference callRef);
			virtual ~RadioMethodSelect();

		public:

		private:
			RadioMethodSelect(const RadioMethodSelect& theRadioMethodSelect);
	};
};
#endif // !defined(RadioMethodSelect__INCLUDED_)
