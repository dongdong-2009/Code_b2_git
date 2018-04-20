/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodForceCallTermination.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodForceCallTermination_H)
#define RadioMethodForceCallTermination_H

#include "RadioMethodCallReference.h"


namespace TA_IRS_App
{
	/**
	* Forces termination of the given call
	*/
	class RadioMethodForceCallTermination : public RadioMethodCallReference
	{
		public:

            /**
             * Constructor
             *
             * @param sessionRef    the usual session reference
             * @param voiceCallRef  a CallReference
             */
			RadioMethodForceCallTermination(RadioSessionReference sessionRef, unsigned long callRef);
	};
};

#endif // !defined(RadioMethodForceCallTermination_H)
