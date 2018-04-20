/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodCeaseTX.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Demand TX method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodDemandTX.h
//  Implementation of the Class RadioMethodDemandTX
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodCeaseTX__INCLUDED_)
#define RadioMethodCeaseTX__INCLUDED_

#include "RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodCeaseTX : public RadioMethod 
	{
		public:
			RadioMethodCeaseTX(RadioSessionReference sessionRef);
			virtual ~RadioMethodCeaseTX();

		public:

		private:
			RadioMethodCeaseTX(const RadioMethodCeaseTX& theMethod);
	};
};
#endif // !defined(RadioMethodCeaseTX__INCLUDED_)
