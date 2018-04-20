/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodMonitorCall.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(RadioMethodMonitorCall_H)
#define RadioMethodMonitorCall_H

#include "RadioMethodCallReference.h"


namespace TA_IRS_App
{
	/**
	* Monitor the given subscriber.
	*/
	class RadioMethodMonitorCall : public RadioMethodCallReference
	{
		public:

            /**
             * Constructor
             *
             * @param sessionRef    the usual session reference
             * @param callRef       a CallReference
             */
			RadioMethodMonitorCall(RadioSessionReference sessionRef, unsigned long callRef);
	};
};

#endif // !defined(RadioMethodMonitorCall_H)