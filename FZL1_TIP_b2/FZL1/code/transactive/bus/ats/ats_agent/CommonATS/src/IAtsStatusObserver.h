/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/IAtsStatusObserver.h $
  * @author:   Adam Radics
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/03/30 11:17:43 $
  * Last modified by:  $Author: grace.koh $
  *
  * Observer for ATS server status.
  */
#ifndef IATS_STATUS_OBSERVER_H
#define IATS_STATUS_OBSERVER_H

namespace TA_IRS_Bus
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
