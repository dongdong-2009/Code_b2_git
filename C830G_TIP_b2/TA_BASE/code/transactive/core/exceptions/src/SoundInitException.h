/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/SoundInitException.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This exception is raised when there is an error
  * initialising the sound card.
  *
  */


#if !defined(SOUNDINITEXCEPTION_H)
#define SOUNDINITEXCEPTION_H

#include "TransactiveException.h"
#include <string>																

namespace TA_Base_Core
{
	class SoundInitException : public TransactiveException  
	{
	public:
		SoundInitException();
		SoundInitException(const std::string& msg);
		virtual ~SoundInitException();
	};
}

#endif // SOUNDINITEXCEPTION_H

