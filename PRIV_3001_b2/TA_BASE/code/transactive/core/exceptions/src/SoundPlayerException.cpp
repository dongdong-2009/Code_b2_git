/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/SoundPlayerException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include "SoundPlayerException.h"

namespace TA_Base_Core
{
	SoundPlayerException::SoundPlayerException()
	{

	}

	SoundPlayerException::SoundPlayerException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}
		

	SoundPlayerException::~SoundPlayerException()
	{

	}
}

