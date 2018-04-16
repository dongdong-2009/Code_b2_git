/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:        $Source: /cvs/1TS-9999-T/code/transactive/core/threads/src/Attic/IThreadPoolWorkerCallback.h,v $
  * @author:    Adam Radics
  * @version:    $Revision: 1.1.2.1 $
  *
  * Last modification:    $Date: 2006/03/15 06:50:46 $
  * Last modified by:    $Author: darrens $
  *
  */

#ifndef IDisplayItemLoader_H
#define IDisplayItemLoader_H

namespace TA_IRS_App
{

    class IDisplayItemLoader
    {
        public:

            virtual ~IDisplayItemLoader() {};
            /**
             * Loading methods to load display items in parallel
             */
            virtual void loadCamerasFromDatabase() = 0;
            virtual void loadQuadsFromDatabase() = 0;
            virtual void loadSequencesFromDatabase() = 0;
            virtual void loadStagesFromDatabase() = 0;
            virtual void loadMonitorsFromDatabase() = 0;
            virtual void loadStationsFromDatabase() = 0;

            virtual void loadCamerasFromAgent() = 0;
            virtual void loadQuadsFromAgent() = 0;
            virtual void loadSequencesFromAgent() = 0;
            virtual void loadStagesFromAgent() = 0;
            virtual void loadMonitorsFromAgent() = 0;
            virtual void loadStationsFromAgent() = 0;

    };

}

#endif
