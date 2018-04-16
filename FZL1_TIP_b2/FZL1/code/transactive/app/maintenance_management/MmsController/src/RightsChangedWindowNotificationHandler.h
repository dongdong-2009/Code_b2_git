/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/maintenance_management/MmsController/src/RightsChangedWindowNotificationHandler.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Helper class to handle posting of notifications (WM_PA_RIGHTS_CHANGE) to windows
  * when there is a change in rights
  *
  */

#pragma once
//Mintao ++
//TD14772
#include "app/maintenance_management/MmsController/src/IRightsChangeListener.h"
//Mintao ++
//TD14772
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{
    class RightsChangedWindowNotificationHandler
    :
    public IRightsChangeListener
    {
    public:
    
        /**
         * RightsChangedWindowNotificationHandler (constructor)
         *
         * Constructs a new observer which will post messages to target
         * window whenever if receives a onCachedMapChanged call
         *
         * @param hWnd the window to receive the update notifications
         *
         */
        RightsChangedWindowNotificationHandler(HWND hWnd);
    
        /**
         * ~CachedMapChangedWindowNotificationHandler (destructor)
         *
         * deregisters this observer from the rights changed registry
         *
         */
        virtual ~RightsChangedWindowNotificationHandler();
        
        /**
         * onRightsChanged
         *
         * Interface implementation, @see IRightsChangeListener::onRightsChanged
         *
         */
        virtual void onRightsChanged();
    
    private:
    
        HWND    m_notificationWnd;
        TA_Base_Core::NonReEntrantThreadLockable m_lock;
    };

} // end namespace TA_IRS_App
    