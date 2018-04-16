/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/SoundPlayerException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

