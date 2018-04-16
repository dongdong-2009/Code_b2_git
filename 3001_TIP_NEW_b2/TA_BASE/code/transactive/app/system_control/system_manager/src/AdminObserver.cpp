/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_manager/src/AdminObserver.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * AdminObserver is an abstract class. Each object that needs to register 
  * with the System Manager as an observer must inherit from this class and 
  * implement the OnNewData method.
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "SystemManager.h"
#include "AdminObserver.h"
#include "AdminManager.h"

namespace TA_Base_App
{

    AdminObserver::AdminObserver()
    {

    }

    AdminObserver::~AdminObserver()
    {

    }

    void AdminObserver::registerObserver()
    {
        AdminManager::getInstance().registerObserver(this);
    }

    void AdminObserver::unregisterObserver()
    {
        AdminManager::getInstance().unregisterObserver(this);
    }

} // namespace TA_Base_App
