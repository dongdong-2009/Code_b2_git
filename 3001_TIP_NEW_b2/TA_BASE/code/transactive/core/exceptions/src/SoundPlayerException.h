/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/SoundPlayerException.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This file outlines a simple exception class for use with
  * SoundPlayer classes. It is derived from TransActiveException.
  */


#ifndef SoundPlayerException_H 
#define SoundPlayerException_H 

#include "TransactiveException.h"
#include <string>																

namespace TA_Base_Core
{
	class SoundPlayerException : public TransactiveException  
	{
	public:
		SoundPlayerException();
		SoundPlayerException(const std::string& msg);
		virtual ~SoundPlayerException();
	};
}

#endif // SoundPlayerException_H 

