#ifndef MESSAGELIBRARYUPGRADER_H_INCLUDED

#define MESSAGELIBRARYUPGRADER_H_INCLUDED



/**

  * The source code in this file is the property of

  * Ripple Systems and is not for redistribution in any form.

  *

  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibraryUpgrader.h $

  * @author:   Robin Ashcroft

  * @version:  $Revision: #1 $

  *

  * Last modification: $DateTime: 2008/11/28 16:26:01 $

  * Last modified by:  $Author: builder $

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



		void initiateUpgrade(const std::string& sessionID);



		void upgradeOcc();



	private:



        // the retry period for the thread

        static const unsigned long THREAD_SLEEP_TIME;



		static const unsigned long OCC_LOCATION_KEY;

		static const unsigned long TDS_LOCATION_KEY;

		static const unsigned long ALLLOCATON_LOCATION_KEY;



        

        /** 

          * createListOfAgentsToUpgrade

          *

          * Get all configured Tis agents from the database,

          * (minus the OCC agent).

          *

          * @return A vector of corba names for tis agents to upgrade

          */

        TA_Base_Core::CorbaNameList createListOfAgentsToUpgrade();





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

          * @param agentsThatFailed     The corba names of the agents that failed.

          */

		

		void upgradeTimedOut(const TA_Base_Core::CorbaNameList& agentsThatFailed);



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