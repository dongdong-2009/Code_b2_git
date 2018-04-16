/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/ats/AtsAgent/src/IAtsStatusObserver.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
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
