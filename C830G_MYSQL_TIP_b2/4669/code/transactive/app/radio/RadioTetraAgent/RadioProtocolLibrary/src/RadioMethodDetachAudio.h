/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodDetachAudio.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API Detach Audio method.
  *
  */

///////////////////////////////////////////////////////////
//  RadioMethodDetachAudio.h
//  Implementation of the Class RadioMethodDetachAudio
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#if !defined(RadioMethodDetachAudio__INCLUDED_)
#define RadioMethodAttachAudio__INCLUDED_

#include "RadioMethodCallReference.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioMethodDetachAudio : public RadioMethodCallReference
	{
		public:
			RadioMethodDetachAudio(RadioSessionReference sessionRef, unsigned long callRef, unsigned long audioRef);
			virtual ~RadioMethodDetachAudio();

		public:

		private:
			RadioMethodDetachAudio(const RadioMethodDetachAudio& theMethod);
	};
};
#endif // !defined(RadioMethodAttachAudio__INCLUDED_)
