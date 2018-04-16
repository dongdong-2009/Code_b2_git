/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/SoundInitException.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * This exception is raised when there is an error
  * initialising the sound card.
  *
  */

#include "SoundInitException.h"


namespace TA_Base_Core
{
	SoundInitException::SoundInitException()
	{

	}

	SoundInitException::SoundInitException(const std::string& msg) 
		: TransactiveException(msg)
	{

	}
		

	SoundInitException::~SoundInitException()
	{

	}
}

