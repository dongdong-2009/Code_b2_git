/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/TA_BASE/transactive/core/data_access_interface/user_settings/src/UserSettingsAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  *
  */
#pragma warning (disable:4786)
#include <string>
#include "PidDataAccessFactory.h"
#include "PidSettingsData.h"
#include "core\exceptions\src\DataException.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    PidDataAccessFactory::~PidDataAccessFactory()
    {

    }


    IPidSettings* PidDataAccessFactory::getUserSettings(std::string sessionId ,unsigned long appType)
    {

        // Create the UserSettings object to represent this object.
		IPidSettings* userSettings = new PidSettingsData(sessionId, appType);

        // Return the userSettings pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return userSettings;
    }
} //end namespace TA_Base_Core


