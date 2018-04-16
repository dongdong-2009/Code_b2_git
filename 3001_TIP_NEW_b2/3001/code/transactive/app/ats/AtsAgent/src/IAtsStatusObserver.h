/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsAgent/src/IAtsStatusObserver.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
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
