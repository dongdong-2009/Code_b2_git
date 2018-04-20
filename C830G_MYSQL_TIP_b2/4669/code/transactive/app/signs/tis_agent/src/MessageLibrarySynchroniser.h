#ifndef MESSAGELIBRARYSYNCHRONISER_H_INCLUDED
#define MESSAGELIBRARYSYNCHRONISER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibrarySynchroniser.h $
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

	class MessageLibrarySynchroniser : public TA_Base_Core::Thread
	{

	public:

		/**
		  * constructor
		  *
		  * @return     nothing.
		  *
		  */
		MessageLibrarySynchroniser( int stationSynchTimeout, TA_IRS_App::EMessageLibraryType type );
		
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

	private:

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
        void synchronisationTimedOut(const TA_Base_Core::CorbaNameList& agentsThatFailed);


		int m_stationSynchTimeout;

		bool m_shouldBeRunning;

		bool m_librariesSynchronised;

		TA_IRS_App::EMessageLibraryType m_libraryType;

		TA_Base_Core::Timer m_timer;
	};

}

#endif // MESSAGELIBRARYSYNCHRONISER_H_INCLUDED