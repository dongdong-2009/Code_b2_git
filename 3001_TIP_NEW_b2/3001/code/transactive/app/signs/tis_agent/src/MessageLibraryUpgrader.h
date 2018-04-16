#ifndef MESSAGELIBRARYUPGRADER_H_INCLUDED
#define MESSAGELIBRARYUPGRADER_H_INCLUDED
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/MessageLibraryUpgrader.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/03/28 15:52:59 $
  * Last modified by:  $Author: huang.wenbo $
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


typedef TA_Base_Core::NamedObject<TA_Base_Bus::ISTISManagerCorbaDef,
TA_Base_Bus::ISTISManagerCorbaDef_ptr,
TA_Base_Bus::ISTISManagerCorbaDef_var> StisNamedObject;
namespace TA_IRS_App
{
    class MessageLibraryUpgrader : public TA_Base_Core::Thread
    {
        public:
            /**
              * constructor
              *
              * @return     nothing.
              *
              */
            MessageLibraryUpgrader( int stationUpgradeTimeout, TA_IRS_App::EMessageLibraryType type );
            /**
            * StatusMonitor
            *
            * Destructor
            *
            * @return     none.
            *
            */
            virtual ~MessageLibraryUpgrader();
            // Other methods
            virtual void run();
            virtual void terminate();
            void initiateUpgrade( const std::string& sessionID );
            void upgradeOcc();
        private:
            // the retry period for the thread

            static const unsigned long THREAD_SLEEP_TIME;
            static const unsigned long OCC_LOCATION_KEY;
            static const unsigned long TDS_LOCATION_KEY;
            static const unsigned long ALLLOCATON_LOCATION_KEY;

            /**
              * upgradeComplete
              *
              * The synchronisation completed successfully.
              * Do what is necessary to reflect that.
              *
              */
            void upgradeComplete();
            /**
              * upgradeTimedOut
              *
              * The complete upgrade failed. A number of agents didnt upgrade.
              * This will raise an alarm that will give details of the failed agents.
              *
              */
            void upgradeTimedOut();

			TA_Base_Core::CorbaNameList createListOfAgentsToUpgrade();
            int m_stationUpgradeTimeout;
            bool m_shouldBeRunning;
            bool m_librariesUpgraded;
            bool m_occUpgraded;
            TA_IRS_App::EMessageLibraryType m_libraryType;
            TA_Base_Core::ReEntrantThreadLockable m_upgradeLock;
            TA_Base_Core::Timer m_timer;
            std::string m_sessionID;
    };
}
#endif // MESSAGELIBRARYSYNCHRONISER_H_INCLUDED