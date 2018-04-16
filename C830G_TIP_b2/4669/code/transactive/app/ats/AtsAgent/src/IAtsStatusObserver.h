/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/ats/AtsAgent/src/IAtsStatusObserver.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last mofified by:  $Author: builder $
  *
  * Observer for ATS server status.
  */
#ifndef IATS_STATUS_OBSERVER_H
#define IATS_STATUS_OBSERVER_H

namespace TA_IRS_App
{
    class IAtsStatusObserver
    {
    public:

        /**
         * Called when the ATS server health status changes
         * from good to bad or bad to good.
         *
         * @param atsOk true if at least one ATS server is healthy
         */
        virtual void atsServerStatusChanged(bool atsOk) = 0;

    };
}

#endif // !defined(IATS_STATUS_OBSERVER_H)
