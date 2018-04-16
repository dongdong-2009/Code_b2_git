/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainScheduleUpgrades.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */


#if !defined(TrainScheduleUpgrades_H)
#define TrainScheduleUpgrades_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/signs/ttis_manager/src/AbstractTrainDownloads.h"

#include "bus/trains/TrainSelectionComponent/src/TrainList.h"


namespace TA_IRS_App
{


    class TrainScheduleUpgrades : public AbstractTrainDownloads
    {

    public:

        /**
          * TrainScheduleUpgrades
          *
          * Constructor
          *
          * @param window   the window to receive the update messages.
          */
        TrainScheduleUpgrades(CWnd* window);

        virtual ~TrainScheduleUpgrades();


    protected:


        /**
         * This will perform the actual download/upgrade request to the TIS agent.
         * It will return success or failure to submit the operation, and optionally an error code. Further status is updated by incoming messages.
         * 
         * @return true if the request was submitted, false if there was an error encountered
         * 
         * @param trainList    The list of trains to perform the actual request on
         * @param errorString    Optionally this will contain an error string if the download fails
         */
        virtual bool processRequests(const TA_Base_Core::TrainList& trainList, std::string& errorString);


        /**
          * getDownloadType
          *
          * Subclasses will return what type of train download they do.
          *
          * @return A download/upgrade type
          */
        inline virtual TrainDownloadType getDownloadType()
        {
            return ScheduleUpgrade;
        };

    };


} // TA_IRS_App

#endif // !defined
