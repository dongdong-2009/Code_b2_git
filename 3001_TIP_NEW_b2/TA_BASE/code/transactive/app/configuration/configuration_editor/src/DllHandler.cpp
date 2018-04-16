/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/configuration/configuration_editor/src/DllHandler.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $	
  *
  * This implements the IDLLHandler so it can be passed to the SingletonHelper to allow it
  * to initialise all singletons.
  */

#include "app/configuration/configuration_editor/src/DllHandler.h"

#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_App
{
    TA_Base_Core::RunParams::ParamVector DllHandler::getRunParams() const
    {
        TA_Base_Core::RunParams::ParamVector allParams;
        TA_Base_Core::RunParams::getInstance().getAll(allParams);
        return allParams;
    }


    TA_Base_Core::CorbaUtil& DllHandler::getCorbaUtil() const
    {
        return TA_Base_Core::CorbaUtil::getInstance();
    }


} // TA_Base_App

