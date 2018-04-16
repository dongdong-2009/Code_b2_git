#ifndef MESSAGELIBRARYSYNCHRONISER_H_INCLUDED
#define MESSAGELIBRARYSYNCHRONISER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/MessageLibrarySynchroniser.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "bus/signs_4669/TisManagerCorbaDef/src/ISTISManagerCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/utilities/src/Timer.h"
#include "WorkThreadPool.h"
#include <ace/Atomic_Op.h>
#include <vector>


typedef TA_Base_Core::NamedObject<TA_Base_Bus::ISTISManagerCorbaDef,
        TA_Base_Bus::ISTISManagerCorbaDef_ptr,
        TA_Base_Bus::ISTISManagerCorbaDef_var> StisNamedObject;

namespace TA_IRS_App
{

    class LibraryDBSynchroniser : public TA_Base_Core::Thread
    {
        public:
            LibraryDBSynchroniser(TA_IRS_App::EMessageLibraryType libraryType);
            ~LibraryDBSynchroniser();
            virtual void run();
            virtual void terminate();
        private:
            void synchroniseStis(StisNamedObject* target,
                                 const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& libraryContent);
            void synchroniseTtis(StisNamedObject* target,
                                 const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& libraryContent);
            bool m_shouldBeRunning;
            TA_IRS_App::EMessageLibraryType m_libraryType;
            WorkThreadPool m_threadPool;
			TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType m_corbaFileContent;
    };


    class MessageLibrarySynchroniser : public TA_Base_Core::Thread
    {

        public:

            /**
              * constructor
              *
              * @return     nothing.
              *
              */
            MessageLibrarySynchroniser(int stationSynchTimeout, TA_IRS_App::EMessageLibraryType type);

            /**
              * StatusMonitor
              *
              * Destructor
              *
              * @return     none.
              *
              */
            virtual ~MessageLibrarySynchroniser();



            // Other methods

            virtual void run();

            virtual void terminate();

            void notifyLibrariesSynchronised();

            void initiateSynchronisation();
            //throw exception
            static void getLibraryFileFromDB(std::vector<unsigned char>& dbFileContent, unsigned long version, EMessageLibraryType type);

        private:

			void checkStisLibraryVersion(StisNamedObject* target);
			void checkTtisLibraryVersion(StisNamedObject* target);

            WorkThreadPool m_threadPool;
			ACE_Atomic_Op<ACE_Thread_Mutex, int> m_notSyncStationsCount;
            // the retry period for the thread
            static const unsigned long THREAD_SLEEP_TIME;

            static const unsigned long OCC_LOCATION_KEY;
            static const unsigned long TDS_LOCATION_KEY;
            static const unsigned long ALLLOCATON_LOCATION_KEY;

            /**
              * createListOfAgentsToSynchronise
              *
              * Get all configured Tis agents from the database,
              * (minus the OCC agent).
              *
              * @return A vector of corba names for tis agents to synchronise
              */
            TA_Base_Core::CorbaNameList createListOfAgentsToSynchronise();


            /**
              * synchronisationComplete
              *
              * The synchronisation completed successfully.
              * Do what is necessary to reflect that.
              *
              */
            void synchronisationComplete();


            /**
              * synchronisationTimedOut
              *
              * The synchronisation failed. A number of agents didnt synchronise.
              * All the rest are okay, so synchronisation is complete, this will raise
              * an alarm that will give details of the failed agents.
              *
              * @param agentsThatFailed     The corba names of the agents that failed.
              */
            void synchronisationTimedOut();


            int m_stationSynchTimeout;

            bool m_shouldBeRunning;

            bool m_librariesSynchronised;

            TA_IRS_App::EMessageLibraryType m_libraryType;

            TA_Base_Core::Timer m_timer;

            LibraryDBSynchroniser m_dbSynchroniser;
    };
}

#endif // MESSAGELIBRARYSYNCHRONISER_H_INCLUDED