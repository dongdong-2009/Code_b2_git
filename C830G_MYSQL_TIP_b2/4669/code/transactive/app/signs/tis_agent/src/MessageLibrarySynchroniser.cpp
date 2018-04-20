/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibrarySynchroniser.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/06/25 12:09:17 $
  * Last modified by:  $Author: builder $
  *
  */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include "app/signs/tis_agent/src/MessageLibrarySynchroniser.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/message/types/TISAlarms_MessageTypes.h"

#include "core/message/types/TISAudit_MessageTypes.h"
#include "app/signs/tis_agent/src/ILocationStore.h"

namespace TA_IRS_App
{

    // the retry period for the thread (10s)
    const unsigned long MessageLibrarySynchroniser::THREAD_SLEEP_TIME = 10000;

	const unsigned long MessageLibrarySynchroniser::ALLLOCATON_LOCATION_KEY = 0;
	const unsigned long MessageLibrarySynchroniser::OCC_LOCATION_KEY = 1;
	const unsigned long MessageLibrarySynchroniser::TDS_LOCATION_KEY = 2;

	void MessageLibrarySynchroniser::run()
	{
		FUNCTION_ENTRY("run()");
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibrarySynchroniser is only available at the OCC" );

        // Set this to true when the thread is first started.  From then on,
		// only attempt to synchronise, if initiateSynchronisation() is called

		m_librariesSynchronised = true;

        // get a list of agents to synchronise
        TA_Base_Core::CorbaNameList agentsThatFailedToSynchronise = createListOfAgentsToSynchronise();

        // keep track of the agents that failed
        TA_Base_Core::CorbaNameList agentsToSynchronise;

		//hongran++ TD17510
		bool isFirstTime = true;
		//++hongran TD17510

        
		while( m_shouldBeRunning )
		{
		
            // Repeat until the timer expires, or the libraries are synchronised
		    // timeout is stored in the database in minutes - convert to milliseconds
			if( !m_librariesSynchronised )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Checking library versions...");

				// Call each station's reportNewMessageLibraryVersion()
				// If the station's new _database_ library version and the ISCS OCC _database_ new version
				// are the same, then this station is ready to go

				StisNamedObject *stisNamedObject = NULL;

				//hongran++ TD17510
				if(isFirstTime)
				{
					agentsToSynchronise = createListOfAgentsToSynchronise();
					isFirstTime = false;
				}
				else
				{
					agentsToSynchronise = agentsThatFailedToSynchronise;
				}
				//++hongran TD17510
				
				
                // new round of attempts, clear the list of failed agents
                agentsThatFailedToSynchronise.clear();

                // for each agent in the list of agents to synchronise
                for (TA_Base_Core::CorbaNameList::iterator agentNameIter = agentsToSynchronise.begin();
				     agentNameIter != agentsToSynchronise.end(); agentNameIter++ )
				{
                    // get the named object for the agent to synchronise
					stisNamedObject = new StisNamedObject( *agentNameIter );

                    // flag to tell if the library is synchronised for this agent
                    bool received = false;

					switch ( m_libraryType )
					{
					case TA_IRS_App::STATION_LIBRARY:
			
						try
						{
							std::stringstream msg;
							msg << "Checking predefined Station message library at ";
							msg << agentNameIter->getAgentName();
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
							CORBA_CALL_RETURN( received,
							                   (*stisNamedObject),
							                   reportStationLibraryVersionReceived,
							                   ( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central() ) );
							if( received )
							{
								std::stringstream msg;
								msg << "Successfully established Station message library version at ";
								msg << agentNameIter->getAgentName();
								std::string msgString;
								msgString = msg.str();

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								 msgString.c_str() );
							}
							else
							{
								std::stringstream msg;
								msg << "Failed to established Station message library version at ";
								msg << agentNameIter->getAgentName();
								std::string msgString;
								msgString = msg.str();

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								 msgString.c_str() );

							}

						}
						catch ( const TA_Base_Core::ObjectResolutionException& ore )
						{
							std::stringstream msg;
							msg << "Could not resolve the TIS Agent: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to establish the Station predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
						catch ( const CORBA::Exception& ce )
						{
							std::stringstream msg;
							msg << "Caught a CORBA exception while attempting to resolve: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to establish the Station predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
					
					
						break;

					case TA_IRS_App::TRAIN_LIBRARY:

						try
						{
							std::stringstream msg;
							msg << "Checking predefined Train message library at ";
							msg << agentNameIter->getAgentName();
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );

							CORBA_CALL_RETURN( received,
							                   (*stisNamedObject),
							                   reportTrainLibraryVersionReceived,
							                   ( StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central() ) );
                            if( received )
							{
								std::stringstream msg;
								msg << "Successfully established train message library version at ";
								msg << agentNameIter->getAgentName();
								std::string msgString;
								msgString = msg.str();

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
								 msgString.c_str() );
							}

						}
						catch ( const TA_Base_Core::ObjectResolutionException& ore )
						{
							std::stringstream msg;
							msg << "Could not resolve the TIS Agent: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to establish the Train predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
						catch ( const CORBA::Exception& ce )
						{
							std::stringstream msg;
							msg << "Caught a CORBA exception while attempting to resolve: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to establish the Train predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
					
						break;
					}

                    // if the agent failed to synchronise
                    if (!received)
                    {
                        // add it to the list of failed agents
                        agentsThatFailedToSynchronise.push_back(*agentNameIter);
                    }

                    // clean up memory
                    delete stisNamedObject;
                    stisNamedObject = NULL;
				}//for

                // check if all agents have been synchronised
                // the list of failed agents will be empty if all succeeded
				if( agentsThatFailedToSynchronise.empty() )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Successfully synchronised all stations");

                    // set the flag that means its done
					m_librariesSynchronised = true;

					//hongran++ TD17510
					isFirstTime = true;
					//++hongran TD17510

                    // no need to clear the failed list - its empty
                    // plus it will be cleared before the next synchronisation

                    // update the status
					synchronisationComplete();

				}
                // not synchronised
                else
                {
                    // check for the timeout condition
				    if( m_timer.elapsedTime() >= (m_stationSynchTimeout*60) )
				    {
					    // Even if the libraries are not synchronised by this point ( we got here because the synch timed out)
				        // we probably want to notify the STISManager to go ahead anyway, because
				        // a station is probably down.

                        // set the flag that means its done
					    m_librariesSynchronised = true;

						//hongran++ TD17510
						isFirstTime = true;
						//++hongran TD17510

                         // update the status
				        synchronisationTimedOut(agentsThatFailedToSynchronise);

                        // reset the list of failed agents
                        agentsThatFailedToSynchronise.clear();

                    }
                }

            } // if ( !m_librariesSynchronised )

			this->sleep( THREAD_SLEEP_TIME );
        } // while

		FUNCTION_EXIT;
	}

	void MessageLibrarySynchroniser::terminate()
	{
		FUNCTION_ENTRY("terminate");
		
		m_shouldBeRunning = false;
		
		FUNCTION_EXIT;
	}


	MessageLibrarySynchroniser::MessageLibrarySynchroniser( int stationSynchTimeout, TA_IRS_App::EMessageLibraryType type ) : m_shouldBeRunning( true )
	{
		m_stationSynchTimeout = stationSynchTimeout;
		m_libraryType = type;
	}

	MessageLibrarySynchroniser::~MessageLibrarySynchroniser()
	{

	}

	void MessageLibrarySynchroniser::notifyLibrariesSynchronised()
	{

	}

	void MessageLibrarySynchroniser::initiateSynchronisation()
	{
		m_librariesSynchronised = false;

		m_timer.restart();
	}


    TA_Base_Core::CorbaNameList MessageLibrarySynchroniser::createListOfAgentsToSynchronise()
    {
        // get all configured tis agents
        TA_Base_Core::CorbaNameList allTisAgents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType( TA_Base_Core::STISEntityData::getStaticType(),false );

        // get the current location key
        // this will be used to remove the occ tis agent (this agent)
        // from the list of agents to synchronise
       
		//hongran++ TD17500
		// I removed the following code. Before, occ agent will not be informed when version dps updated. But now for DDB, OCC agent should 
		// also be informed to read the DB and upate the dp values.

/*      TA_Base_Core::CorbaNameList::iterator iter;
        for (iter = allTisAgents.begin(); iter != allTisAgents.end();)
        {
            // if its the occ agent
            if (iter->getLocationKey() == ALLLOCATON_LOCATION_KEY || 
				iter->getLocationKey() == OCC_LOCATION_KEY || 
				iter->getLocationKey() == TDS_LOCATION_KEY)
            {
				iter = allTisAgents.erase(iter);
            }
			else
			{
				++iter;
			}
        }*/
		//++hongran TD17500
        return allTisAgents;
    }


    void MessageLibrarySynchroniser::synchronisationComplete()
    {
        // notify station libraries synchronised.
		// The StatusMonitor will update its member variable
		// and will allow the 'Upgrade ISCS' button to 
		// be enabled on the TTISManager (after querying the OCC TIS agent via the STIS Manager)

		// The GUI needs to check that 1) the new library version is different to the current library version,
		// and 2) the station library versions are synchronised.  If so, the Upgrade ISCS button can be enabled.

		// if the library versions are the same, and 'libraries synchronised' is set to true, the 'Upgrade'
		// button should not be enabled.

        // update the status

		if( m_libraryType == TA_IRS_App::STATION_LIBRARY )
		{
			StatusMonitor::getInstance()->setStationLibrariesSynchronised( true );

			STISManager::getInstance()->messageLibrarySynchronised( TA_IRS_App::STATION_LIBRARY );

			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			versionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			TA_Base_Core::NameValuePair nvp("Version", versionStream.str() );

			dp.push_back( &nvp );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::StationLibraryPendingUpgrade, dp );


			//TES831 Events not logged!
			TA_Base_Core::DescriptionParameters desc;

			TA_Base_Core::NameValuePair versionVP("version", versionStream.str() );
			TA_Base_Core::NameValuePair failedVP("Failed on:", "" );
			TA_Base_Core::NameValuePair listVP("Failed station list", "" );
			
			desc.push_back(&versionVP);
			desc.push_back(&failedVP);
			desc.push_back(&listVP);
			
			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISLibrarySynchronised,desc,"");
			//TES831 Events not logged!

			// TODO: Remove when the STISManager GUI has the 'Upgrade ISCS' button
			// STISManager::getInstance()->upgradePredefinedStationMessageLibrary( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion(), "INTERNAL" );
		}
		else if( m_libraryType == TA_IRS_App::TRAIN_LIBRARY )
		{
			StatusMonitor::getInstance()->setTrainLibrariesSynchronised( true );

			STISManager::getInstance()->messageLibrarySynchronised( TA_IRS_App::TRAIN_LIBRARY );

			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			versionStream << StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

			TA_Base_Core::NameValuePair nvp("Version", versionStream.str() );

			dp.push_back( &nvp );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::TrainLibraryPendingUpgrade, dp );


			//TES831 Events not logged!
			TA_Base_Core::DescriptionParameters desc;

			std::stringstream libraryVersionStream;

			libraryVersionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			TA_Base_Core::NameValuePair versionVP("version", libraryVersionStream.str() );
			TA_Base_Core::NameValuePair failedVP("Failed on:", "" );
			TA_Base_Core::NameValuePair listVP("Failed station list", "" );
			
			desc.push_back(&versionVP);
			desc.push_back(&failedVP);
			desc.push_back(&listVP);
			
			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::TTISLibrarySynchronised,desc,"");
			//TES831 Events not logged!

		}

    }


    void MessageLibrarySynchroniser::synchronisationTimedOut(const TA_Base_Core::CorbaNameList& agentsThatFailed)
    {
        // notify station libraries synchronised.
		// The StatusMonitor will update its member variable
		// and will allow the 'Upgrade ISCS' button to 
		// be enabled on the TTISManager (after querying the OCC TIS agent via the STIS Manager)

		// The GUI needs to check that 1) the new library version is different to the current library version,
		// and 2) the station library versions are synchronised.  If so, the Upgrade ISCS button can be enabled.

		// if the library versions are the same, and 'libraries synchronised' is set to true, the 'Upgrade'
		// button should not be enabled.

        // this should also submit a different alarm - outlining the agents that failed to synchronise


        // create a list of the failed stations
        std::stringstream message;
		std::stringstream alarmLocationList;

        message << "The following locations failed to synchronise: ";
       

        for (TA_Base_Core::CorbaNameList::const_iterator iter = agentsThatFailed.begin();
        iter != agentsThatFailed.end(); iter++)
        {
            try
            {
                TA_Base_Core::ILocation* location = NULL;
                //wenbo++,CL-21314
                location = ILocationStore::getInstance()->getLocationByKey(iter->getLocationKey());
                //++wenbo
                
                if (location != NULL)
                {
                    message << location->getName() << " (" << iter->getAgentName() << ") ";
					alarmLocationList << " " << location->getName();
                }
            }
            catch (...)
            {
                // database exception or a data exception
                message << iter->getAgentName() << " ";
            }
        }

		if( m_libraryType == TA_IRS_App::STATION_LIBRARY )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while attempting to synchronise the predefined STIS message library - considering the synchronisation complete - possible station failure");
            LOGMORE( SourceInfo, message.str().c_str() );

			StatusMonitor::getInstance()->setStationLibrariesSynchronised( true );

			STISManager::getInstance()->messageLibrarySynchronised( TA_IRS_App::STATION_LIBRARY );


			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			versionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			TA_Base_Core::NameValuePair functionVP("Function", "synchronise" );
			TA_Base_Core::NameValuePair versionVP("Version", versionStream.str() );
			TA_Base_Core::NameValuePair typeVP("Type", "Station" );
			TA_Base_Core::NameValuePair listVP("StationList", alarmLocationList.str() );

			dp.push_back( &functionVP );
			dp.push_back( &versionVP );
			dp.push_back( &typeVP );
			dp.push_back( &listVP );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::PredefinedMessageLibraryError, dp );


            // TODO - submit appropriate audit message

			//TES831 Events not logged!
			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair versionDP("version", versionStream.str() );
			TA_Base_Core::NameValuePair failedDP("Failed on:", "Failed on:" );
			TA_Base_Core::NameValuePair listDP("Failed station list",alarmLocationList.str() );

			desc.push_back( &versionDP );
			desc.push_back( &failedDP );
			desc.push_back( &listDP );

			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISLibrarySynchronised,
				desc, 
				""); // event key - not required 
			//TES831 Events not logged!


            // TODO: Remove when the STISManager GUI has the 'Upgrade ISCS' button
			//STISManager::getInstance()->upgradePredefinedStationMessageLibrary( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion(), "INTERNAL" );
		}
		else if( m_libraryType == TA_IRS_App::TRAIN_LIBRARY )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while attempting to synchronise the predefined TTIS message library - considering the synchronisation complete - possible station failure");
            LOGMORE( SourceInfo, message.str().c_str() );

			StatusMonitor::getInstance()->setTrainLibrariesSynchronised( true );

			STISManager::getInstance()->messageLibrarySynchronised( TA_IRS_App::TRAIN_LIBRARY );

			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			versionStream << StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

			TA_Base_Core::NameValuePair functionVP("Function", "synchronise" );
			TA_Base_Core::NameValuePair versionVP("Version", versionStream.str() );
			TA_Base_Core::NameValuePair typeVP("Type", "Train" );
			TA_Base_Core::NameValuePair listVP("StationList", alarmLocationList.str() );

			dp.push_back( &functionVP );
			dp.push_back( &versionVP );
			dp.push_back( &typeVP );
			dp.push_back( &listVP );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::PredefinedMessageLibraryError, dp );

			//TES831 Events not logged!
			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair versionDP("version", versionStream.str() );
			TA_Base_Core::NameValuePair failedDP("Failed on:", "Failed on:" );
			TA_Base_Core::NameValuePair listDP("Failed station list",alarmLocationList.str() );

			desc.push_back( &versionDP );
			desc.push_back( &failedDP );
			desc.push_back( &listDP );

			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::TTISLibrarySynchronised,
				desc, 
				"");
			//TES831 Events not logged!

		}
    }


}  // namespace TA_IRS_App