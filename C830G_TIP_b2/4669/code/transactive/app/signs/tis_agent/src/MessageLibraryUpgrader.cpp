/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibraryUpgrader.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/06/25 12:09:17 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/MessageLibraryUpgrader.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"

#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"

#include "core/message/src/NameValuePair.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "app/signs/tis_agent/src/ILocationStore.h" //CL-21314,wenbo


namespace TA_IRS_App
{

    // the retry period for the thread (10s)
    const unsigned long MessageLibraryUpgrader::THREAD_SLEEP_TIME = 10000;

	const unsigned long MessageLibraryUpgrader::ALLLOCATON_LOCATION_KEY = 0;
	const unsigned long MessageLibraryUpgrader::OCC_LOCATION_KEY = 1;
	const unsigned long MessageLibraryUpgrader::TDS_LOCATION_KEY = 2;

	void MessageLibraryUpgrader::run()
	{
		FUNCTION_ENTRY("run()");
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryUpgrader is only available at the OCC" );

        // Set this to true when the thread is first started.  From then on,
		// only attempt to synchronise, if initiateSynchronisation() is called

		m_librariesUpgraded = true;
		m_occUpgraded = false;

		//hongran++ TD17510
		bool isFirstTime = true;
		//++hongran TD17510
		
        // get a list of agents to synchronise
        TA_Base_Core::CorbaNameList agentsThatFailedToUpgrade = createListOfAgentsToUpgrade();

        // keep track of the agents that failed
        TA_Base_Core::CorbaNameList agentsToUpgrade;

		// Set this to false to upgrade the Occ when atleast one agent gets upgraded
		// Set it to true when the Occ gets Upgraded.
		
		while( m_shouldBeRunning )
		{
			// Repeat until the timer expires, or the libraries are upgraded
		    // timeout is stored in the database in minutes - convert to milliseconds
			if( !m_librariesUpgraded )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to upgrade library version...");

				// Call each station's upgradePredefinedXXXMessageLibraryVersion()

				StisNamedObject *stisNamedObject = NULL;

				agentsToUpgrade = agentsThatFailedToUpgrade;
				//hongran++ TD17510
				if(isFirstTime)
				{
					agentsToUpgrade = createListOfAgentsToUpgrade();
					isFirstTime = false;
				}
				else
				{
					agentsToUpgrade = agentsThatFailedToUpgrade;
				}
				//++hongran TD17510

                // new round of attempts, clear the list of failed agents
                agentsThatFailedToUpgrade.clear();

                // for each agent in the list of agents to synchronise
                for (TA_Base_Core::CorbaNameList::iterator agentNameIter = agentsToUpgrade.begin();
				     agentNameIter != agentsToUpgrade.end(); agentNameIter++ )
				{
                    // get the named object for the agent to synchronise
					stisNamedObject = new StisNamedObject( *agentNameIter );

                    // flag to tell if the library is synchronised for this agent
                    bool upgradeSuccessful = false;

					switch ( m_libraryType )
					{
					case TA_IRS_App::STATION_LIBRARY:
			
						try
						{
							std::stringstream msg;
							msg << "Attempting to upgrade Station message library at ";
							msg << agentNameIter->getAgentName();
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );

							// TODO - send the session ID
							if( 0 != stisNamedObject )
							{
								CORBA_CALL( (*stisNamedObject),
								            upgradePredefinedStationMessageLibrary,
								            ( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central(), "INTERNAL" ) );
							}
							else
							{
								std::string errorMsg;
								errorMsg += "Unable to create named object for ";
								errorMsg += agentNameIter->getAgentName();

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									errorMsg.c_str() );
							}


							upgradeSuccessful = true;

							msg.clear();
							msg << "successfully upgraded station message library on TIS Agent: ";
							msg << agentNameIter->getAgentName();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msg.str().c_str() );

						}
						catch ( const TA_Base_Core::ObjectResolutionException& )
						{
							std::stringstream msg;
							msg << "Could not resolve the TIS Agent: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to upgrade the Station predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
						catch ( const CORBA::Exception& )
						{
							std::stringstream msg;
							msg << "Caught a CORBA exception while attempting to resolve: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to upgrade the Station predefined message library version.";
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
							msg << "Attempting to upgrade Train message library at ";
							msg << agentNameIter->getAgentName();
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );

							if( 0 != stisNamedObject )
							{
								CORBA_CALL( (*stisNamedObject),
								            upgradePredefinedTrainMessageLibrary,
								            ( StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central(), "INTERNAL" ) );
							}
							else
							{
								std::string errorMsg;
								errorMsg += "Unable to create named object for ";
								errorMsg += agentNameIter->getAgentName();

								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
									errorMsg.c_str() );
							}


							upgradeSuccessful = true;

							msg.clear();
							msg << "successfully upgraded train message library on TIS Agent: ";
							msg << agentNameIter->getAgentName();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msg.str().c_str() );

						}
						catch ( const TA_Base_Core::ObjectResolutionException& )
						{
							std::stringstream msg;
							msg << "Could not resolve the TIS Agent: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to upgrade the Train predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
						catch ( const CORBA::Exception& )
						{
							std::stringstream msg;
							msg << "Caught a CORBA exception while attempting to resolve: ";
							msg << agentNameIter->getAgentName();
							msg << " while attempting to upgrade the Train predefined message library version.";
							std::string msgString;
							msgString = msg.str();

							LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
							 msgString.c_str() );
						}
					
						break;
					}

                    // if the agent failed to synchronise
                    if (!upgradeSuccessful)
                    {
                        // add it to the list of failed agents
                        agentsThatFailedToUpgrade.push_back(*agentNameIter);
                    }
					
					if(!m_occUpgraded)
					{
                        int agentsUpgraded = (agentsToUpgrade.size() 
                            - agentsThatFailedToUpgrade.size());
                        
                        if( agentsUpgraded >0 )					
						{
							upgradeOcc();
							m_occUpgraded = true;
						}
					}

                    // clean up memory
                    delete stisNamedObject;
                    stisNamedObject = NULL;
				}
                
				
				// check if all agents have been synchronised
                // the list of failed agents will be empty if all succeeded
				if( agentsThatFailedToUpgrade.empty() )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Successfully upgraded predefined message library at all stations");

                    // set the flag that means its done
					m_librariesUpgraded = true;

					//hongran++ TD17510
					isFirstTime = true;
					//++hongran TD17510
					
                    // no need to clear the failed list - its empty
                    // plus it will be cleared before the next synchronisation

                    // update the status
					upgradeComplete();

				}
                // not synchronised
                else
                {
                    // check for the timeout condition
				    if( m_timer.elapsedTime() >= (m_stationUpgradeTimeout*60) )
				    {
					    // Even if the libraries are not upgraded by this point ( we got here because the upgrade timed out)
				        // we probably want to notify the STISManager to go ahead anyway, because
				        // a station is probably down.

                        // set the flag that means its done
					    m_librariesUpgraded = true;

						//hongran++ TD17510
						isFirstTime = true;
						//++hongran TD17510

                        // update the status
				        upgradeTimedOut(agentsThatFailedToUpgrade);

                        // reset the list of failed agents
                        agentsThatFailedToUpgrade.clear();

                    }
                }

            } // if ( !m_librariesUpgraded )

			this->sleep( THREAD_SLEEP_TIME );
        } // while

		// keep track of the agents that failed
        //TA_Base_Core::CorbaNameList agentsThatFailedToUpgrade;

		FUNCTION_EXIT;
	}

	void MessageLibraryUpgrader::terminate()
	{
		FUNCTION_ENTRY("terminate");
		
		m_shouldBeRunning = false;
		
		FUNCTION_EXIT;
	}


	MessageLibraryUpgrader::MessageLibraryUpgrader( int stationUpgradeTimeout, TA_IRS_App::EMessageLibraryType type ) : m_shouldBeRunning( true )
	{
		m_stationUpgradeTimeout = stationUpgradeTimeout;
		m_libraryType = type;
	}

	MessageLibraryUpgrader::~MessageLibraryUpgrader()
	{

	}

	void MessageLibraryUpgrader::initiateUpgrade(const std::string& sessionID)
	{
		TA_Base_Core::ThreadGuard guard( m_upgradeLock );

		m_timer.restart();

		m_occUpgraded = false;
		m_librariesUpgraded = false;
		m_sessionID = sessionID;
	}


    TA_Base_Core::CorbaNameList MessageLibraryUpgrader::createListOfAgentsToUpgrade()
    {
        // get all configured tis agents
        TA_Base_Core::CorbaNameList allTisAgents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType( TA_Base_Core::STISEntityData::getStaticType(),false );

        // get the current location key
        // this will be used to remove the occ tis agent (this agent)
        // from the list of agents to synchronise
        
        TA_Base_Core::CorbaNameList::iterator iter;
        for (iter = allTisAgents.begin(); iter != allTisAgents.end(); )
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
        }

        return allTisAgents;
    }


    void MessageLibraryUpgrader::upgradeComplete()
    {
		FUNCTION_ENTRY("upgradeComplete()");

		int nowPreviousMessageLibraryVersion;
		int newLibraryVersion;
        std::stringstream versionStream;
		TA_Base_Core::AuditMessageSender* messageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
		TA_Base_Core::DescriptionParameters desc;
        TA_Base_Core::NameValuePair versionVP("version", "" );
		TA_Base_Core::NameValuePair failedVP("Failed on:", "");
		TA_Base_Core::NameValuePair listVP("Failed station list", "" );

		// update the status
		switch ( m_libraryType )
		{
		case TA_IRS_App::STATION_LIBRARY:
			nowPreviousMessageLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
			newLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			// delete the previous message library and messages from the OCC database
            TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
                "STIS", nowPreviousMessageLibraryVersion, newLibraryVersion );

			versionStream << newLibraryVersion;			
			versionVP.value = versionStream.str();
			
			desc.push_back(&versionVP);
			desc.push_back(&failedVP);
			desc.push_back(&listVP);
			
			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISISCSLibraryUpgrade,
                desc, m_sessionID.c_str());

            break;

		case TA_IRS_App::TRAIN_LIBRARY:
            nowPreviousMessageLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
			newLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

            // delete the previous message library and messages from the OCC database
            TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
                "TTIS", nowPreviousMessageLibraryVersion, newLibraryVersion );

			versionStream << newLibraryVersion;
            versionVP.value = versionStream.str();

			desc.push_back(&versionVP);
			desc.push_back(&failedVP);
			desc.push_back(&listVP);

			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::TTISISCSLibraryUpgrade,
                desc, m_sessionID.c_str()); 
            
            break;
		}

		FUNCTION_EXIT;
    }


    void MessageLibraryUpgrader::upgradeTimedOut(const TA_Base_Core::CorbaNameList& agentsThatFailed)
    {
		FUNCTION_ENTRY("upgradeTimedOut()");

        // create a list of the failed stations
        std::stringstream message;

		std::stringstream alarmLocationList;

        message << "The following locations failed to synchronise: ";


        for ( TA_Base_Core::CorbaNameList::const_iterator iter = agentsThatFailed.begin();
                iter != agentsThatFailed.end(); iter++ )
        {
            try
            {
                TA_Base_Core::ILocation* location = NULL;
                //wenbo++,CL-21314
                location = ILocationStore::getInstance()->getLocationByKey( iter->getLocationKey() );
                //++wenbo

                if ( location != NULL )
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
					"Timed out while attempting to upgrade the predefined STIS message library - considering the upgrade complete - possible station failure");
            LOGMORE( SourceInfo, message.str().c_str() );

			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::STATION_LIBRARY );

			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			versionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			TA_Base_Core::NameValuePair functionVP("Function", "upgrade" );
			TA_Base_Core::NameValuePair versionVP("Version", versionStream.str() );
			TA_Base_Core::NameValuePair typeVP("Type", "Station" );
			TA_Base_Core::NameValuePair listVP("StationList", alarmLocationList.str() );

			dp.push_back( &functionVP );
			dp.push_back( &versionVP );
			dp.push_back( &typeVP );
			dp.push_back( &listVP );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::PredefinedMessageLibraryError, dp );

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Operator has attempted to upgrade the STIS library version ");
		
			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair versionDP("version", versionStream.str() );
			TA_Base_Core::NameValuePair failedDP("Failed on:", "Failed on:" );
			TA_Base_Core::NameValuePair listDP("Failed station list",alarmLocationList.str() );
			//TA_Base_Core::NameValuePair listDP("StationList", alarmLocationList.str() );

			desc.push_back( &versionDP );
			desc.push_back( &failedDP );
			desc.push_back( &listDP );

			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISISCSLibraryUpgrade,
				desc, 
				m_sessionID.c_str()); // event key - not required 

		}
		else if( m_libraryType == TA_IRS_App::TRAIN_LIBRARY )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while attempting to upgrade the predefined TTIS message library - considering the upgrade complete - possible station failure");
            LOGMORE( SourceInfo, message.str().c_str() );

			// TD 16480
			// int newLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();
			// TD 16480

			// set the current library version for this agent (OCC) 
	        // StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( newLibraryVersion );

			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::TRAIN_LIBRARY );
 
			TA_Base_Core::DescriptionParameters dp;

			std::stringstream versionStream;
			// TD 16480
			// versionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();
			versionStream << StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
			// TD 16480

			TA_Base_Core::NameValuePair functionVP("Function", "upgrade" );
			TA_Base_Core::NameValuePair versionVP("Version", versionStream.str() );
			TA_Base_Core::NameValuePair typeVP("Type", "Train" );
			TA_Base_Core::NameValuePair listVP("StationList", alarmLocationList.str() );

			dp.push_back( &functionVP );
			dp.push_back( &versionVP );
			dp.push_back( &typeVP );
			dp.push_back( &listVP );

			STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::PredefinedMessageLibraryError, dp );

			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair versionDP("version", versionStream.str() );
			TA_Base_Core::NameValuePair failedDP("Failed on:", "Failed on:" );
			TA_Base_Core::NameValuePair listDP("Failed station list",alarmLocationList.str());
			//TA_Base_Core::NameValuePair listDP("StationList", alarmLocationList.str() );

			desc.push_back( &versionDP );
			desc.push_back( &failedDP );
			desc.push_back( &listDP );

			STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::TTISISCSLibraryUpgrade,
				desc, 
				m_sessionID.c_str()); // event key - not required 
		}

    }


	void MessageLibraryUpgrader::upgradeOcc()
	{	
		FUNCTION_ENTRY("upgradeOcc()");
		int nowPreviousMessageLibraryVersion;
		int newLibraryVersion;

		// update the status
		switch ( m_libraryType )
		{
		case TA_IRS_App::STATION_LIBRARY:

			// get the occ local version
			nowPreviousMessageLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();

			newLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

			// set the current library version for this agent (OCC local) 
			StatusMonitor::getInstance()->setCurrentISCSStationLibraryVersion_central( newLibraryVersion );

			// Attempt to contact the STIS server first and tell it to upgrade
			try
			{
				STISManager::getInstance()->submitUpgradePredefinedMessageRequest( newLibraryVersion );
			}
			catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			{
				FUNCTION_EXIT;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "STIS Server not available when attempting to upgrade STIS message library");
			}
			catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
			{
				FUNCTION_EXIT;
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Communications failure with STIS Server when attempting to upgrade STIS message library");
			}

			// Let the STISManager servant know that the upgrade is complete so clients can initiate new
			// upgrades when possible
			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::STATION_LIBRARY );

			try
			{
				TA_Base_Core::IPredefinedMessageLibrary* oldLibrary = 
					TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( nowPreviousMessageLibraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS );

				if( oldLibrary )
				{
					oldLibrary->deleteThisLibrary();
				}

				delete oldLibrary;
			}
			catch( const TA_Base_Core::DataException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Caught a data exception while attempting to delete the predefined message library");
			}
			catch( const TA_Base_Core::DatabaseException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Caught a database exception while attempting to delete the predefined message library");
			}

			break;

		case TA_IRS_App::TRAIN_LIBRARY:

			nowPreviousMessageLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();

			newLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

			// TODO - submit audit message

			// set the current library version for this agent (OCC) 
			StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion_central( newLibraryVersion );

			// Let the STISManager servant know that the upgrade is complete so clients can initiate new
			// upgrades when possible
			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::TRAIN_LIBRARY );

			try
			{
				TA_Base_Core::IPredefinedMessageLibrary* oldLibrary = 
					TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( nowPreviousMessageLibraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS );

				if( oldLibrary )
				{
					oldLibrary->deleteThisLibrary();
				}

				delete oldLibrary;
			}
			catch( const TA_Base_Core::DataException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Caught a data exception while attempting to delete the predefined message library");
			}
			catch( const TA_Base_Core::DatabaseException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Caught a database exception while attempting to delete the predefined message library");
			}
				break;
		}
		
	}

}  // namespace TA_IRS_App