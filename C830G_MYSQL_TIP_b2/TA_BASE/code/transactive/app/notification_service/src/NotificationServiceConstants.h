#ifndef NOTIFICATION_SERVICE_CONSTANTS_H
#define NOTIFICATION_SERVICE_CONSTANTS_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/notification_service/src/NotificationServiceConstants.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * Constant values for the NotificationServiceWrapper
 */


namespace TA_Base_App
{
	// Timeout for resolving the EventChannelFactory (secs)
	const int RESOLVE_CHANNELFACTORY_TIMEOUT	= 20;

	// path & name of the omniNotify config file
	#define OMNI_NOTIFY_CONFIG_FILE "NotifyConfigFile"
}

#endif // NOTIFICATION_SERVICE_CONSTANTS_H
