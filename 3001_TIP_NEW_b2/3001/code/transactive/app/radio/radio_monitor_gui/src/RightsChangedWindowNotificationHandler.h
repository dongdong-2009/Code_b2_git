/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RightsChangedWindowNotificationHandler.h $
  * @author Jade Lee
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Helper class to handle posting of notifications (WM_PA_RIGHTS_CHANGE) to windows
  * when there is a change in rights
  *
  */

#pragma once

#include "app/radio/radio_monitor_gui/src/IRightsChangeListener.h"
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
    
