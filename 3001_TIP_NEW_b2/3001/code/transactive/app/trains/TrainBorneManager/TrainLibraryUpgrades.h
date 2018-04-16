/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/TrainLibraryUpgrades.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * Messages are sent to the given window, they are:
  * WM_UPDATE_DOWNLOADLIST (wParam is a TrainDownloadStatus*, it must be deleted when used)
  */


#if !defined(TrainLibraryUpgrades_H)
#define TrainLibraryUpgrades_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AbstractTrainDownloads.h"

#include "bus/trains/TrainSelectionComponent/src/TrainList.h"


namespace TA_IRS_App
{


    class TrainLibraryUpgrades : public AbstractTrainDownloads
    {

    public:

        /**
          * TrainLibraryUpgrades
          *
          * Constructor
          *
          * @param window   the window to receive the update messages.
          */
        TrainLibraryUpgrades(CWnd* window);

        virtual ~TrainLibraryUpgrades();

		virtual void startDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool autoRetry);



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
            return LibraryUpgrade;
        };

    };


} // TA_IRS_App

#endif // !defined
