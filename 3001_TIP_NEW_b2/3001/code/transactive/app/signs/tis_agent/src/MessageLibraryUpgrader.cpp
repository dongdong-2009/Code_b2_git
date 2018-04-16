/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/MessageLibraryUpgrader.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #10 $
  *
  * Last modification: $DateTime: 2015/07/06 20:50:43 $
  * Last modified by:  $Author: hongzhi.zhang $
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
#include "app/signs/tis_agent/src/ILocationStore.h"


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
		// Mark a new upgrade start
		bool isFirstTime = true;
		// The agent list of to be upgraded
		TA_Base_Core::CorbaNameList agentsToUpgrade;

		while( m_shouldBeRunning )
		{
			if( !m_librariesUpgraded )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to upgrade library version...");
				if(isFirstTime)
				{
					agentsToUpgrade = createListOfAgentsToUpgrade();
					isFirstTime = false;
					// submit audit message
		            TA_Base_Core::DescriptionParameters desc;
	                TA_Base_Core::NameValuePair blank("blank", " ");
				    desc.push_back(&blank);

					if( TA_IRS_App::STATION_LIBRARY == m_libraryType)
					{
					    STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISISCSLibraryUpgrade, desc, m_sessionID.c_str());
					}
					else // ttis
					{
					    STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::TTISISCSLibraryUpgrade, desc, m_sessionID.c_str());
					}
				}
                // for each agent in the list of agents to synchronise
				for (TA_Base_Core::CorbaNameList::iterator iter = agentsToUpgrade.begin();
				     iter != agentsToUpgrade.end();)
				{
					std::auto_ptr<StisNamedObject> namedObject(new StisNamedObject(*iter));
					std::string agentName = namedObject->getAgentName();
                    // flag to tell if the library is synchronised for this agent
                    bool upgradeSuccessful = false;
                    bool offline = false;
					switch ( m_libraryType )
					{
					case TA_IRS_App::STATION_LIBRARY:
			
						try
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to upgrade STIATION message library at:%s", agentName.c_str());
							CORBA_CALL( (*namedObject),
							            upgradePredefinedStationMessageLibrary,
							            ( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central(), m_sessionID.c_str() ) );
							upgradeSuccessful = true;
						}
						catch ( const TA_Base_Core::ObjectResolutionException& )
						{
						    offline = true;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resolve the Tis Agent:%s failed", agentName.c_str());
						}
						catch ( const CORBA::Exception& )
						{
						    offline = true;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught CORBA exception, Tis Agent:%s", agentName.c_str());
						}
						break;
					case TA_IRS_App::TRAIN_LIBRARY:

						try
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Attempting to upgrade Train message library at:%s", agentName.c_str());
							CORBA_CALL( (*namedObject),
							            upgradePredefinedTrainMessageLibrary,
							            ( StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central(), m_sessionID.c_str() ) );
							upgradeSuccessful = true;
						}
						catch ( const TA_Base_Core::ObjectResolutionException& )
						{
						    offline = true;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Resolve the Tis Agent:%s failed", agentName.c_str());
						}
						catch ( const CORBA::Exception& )
						{
						    offline = true;
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught CORBA exception, Tis Agent:%s", agentName.c_str());
						}
						break;
					}

                    // if the agent failed to synchronise
                    if (true == upgradeSuccessful || true == offline)
                    {
						iter = agentsToUpgrade.erase(iter);
						if(!m_occUpgraded && true == upgradeSuccessful) // anyone station update successful, update occ
						{
							try
							{
								upgradeOcc();
								m_occUpgraded = true;
							}
							catch (...)
							{
								LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
									"Exception catch while upgrade STIS library");
							}
						}
                    }
					else
					{
						++iter;
					}
				} // end for
				// check if all agents have been synchronised
				if( agentsToUpgrade.empty() )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"Successfully upgraded predefined message library at all stations");

                    // set the flag that means its done
					m_librariesUpgraded = true;
					isFirstTime = true;
                    // update the status
					try
					{
						upgradeComplete();
					}
					catch( const TA_Base_Core::DataException&e )
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
					}
					catch( const TA_Base_Core::DatabaseException&e )
					{
						LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", e.what());
					}
				}
                else //timeout
                {
				    if( m_timer.elapsedTime() >= (m_stationUpgradeTimeout*60) )
				    {
                        // set the flag that means its done
					    m_librariesUpgraded = true;
						isFirstTime = true;

                        if(!agentsToUpgrade.empty())
                        {
        		            TA_Base_Core::DescriptionParameters desc;
        	                TA_Base_Core::NameValuePair blank("blank", " ");
        				    desc.push_back(&blank);
                            std::stringstream stnListStream;
                            for(TA_Base_Core::CorbaNameList::iterator it = agentsToUpgrade.begin(); it != agentsToUpgrade.end(); ++it)
                            {
                                try
                                {
                                    stnListStream << ILocationStore::getInstance()->getLocationByKey(it->getLocationKey())->getDisplayName() << ",";
                                }
                                catch(...)
                                {
                                }
                            }
                            std::string stnList = stnListStream.str();
                            stnList = stnList.substr(0, stnList.find_last_of(','));
        	                TA_Base_Core::NameValuePair stn("STN", stnList);
                            desc.push_back(&stn);
        					if( TA_IRS_App::STATION_LIBRARY == m_libraryType)
        					{
        					    STISManager::getInstance()->submitAuditMessage( TA_Base_Core::TISAudit::STISLibraryUpgradeFailed, desc, m_sessionID.c_str());
        					}
                        }

				        upgradeTimedOut();
						agentsToUpgrade.clear();
                    }
                }

            } // if ( !m_librariesUpgraded )
			this->sleep( THREAD_SLEEP_TIME );
        } // while
		FUNCTION_EXIT;
	}

	void MessageLibraryUpgrader::terminate()
	{
		FUNCTION_ENTRY("terminate");
		
		m_shouldBeRunning = false;
		
		FUNCTION_EXIT;
	}


	MessageLibraryUpgrader::MessageLibraryUpgrader( int stationUpgradeTimeout, TA_IRS_App::EMessageLibraryType type )
		:m_shouldBeRunning( true ), m_stationUpgradeTimeout(stationUpgradeTimeout), m_libraryType(type),
		m_librariesUpgraded(true), m_occUpgraded(true), m_sessionID("INTERNAL")
	{
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

		unsigned long nowPreviousMessageLibraryVersion;
		unsigned long newLibraryVersion;
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
            break;

		case TA_IRS_App::TRAIN_LIBRARY:
            nowPreviousMessageLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
			newLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

            // delete the previous message library and messages from the OCC database
            TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
                "TTIS", nowPreviousMessageLibraryVersion, newLibraryVersion );
            break;
		}

		FUNCTION_EXIT;
    }


    void MessageLibraryUpgrader::upgradeTimedOut()
    {
		FUNCTION_ENTRY("upgradeTimedOut()");

		if( m_libraryType == TA_IRS_App::STATION_LIBRARY )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while attempting to upgrade the predefined STIS message library - considering the upgrade complete - possible station failure");
			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::STATION_LIBRARY );
		}
		else if( m_libraryType == TA_IRS_App::TRAIN_LIBRARY )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while attempting to upgrade the predefined TTIS message library - considering the upgrade complete - possible station failure");
			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::TRAIN_LIBRARY );
		}
    }


	void MessageLibraryUpgrader::upgradeOcc()
	{	
		FUNCTION_ENTRY("upgradeOcc()");
		unsigned long nowPreviousMessageLibraryVersion;
		unsigned long newLibraryVersion;

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
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "STIS Server not available when attempting to upgrade STIS message library");
				throw;
			}
			catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Communications failure with STIS Server when attempting to upgrade STIS message library");
				throw;
			}
            catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Time out when STIS Server reply upgrade message when attempting to upgrade STIS message library");
				throw;
			}

			// Let the STISManager servant know that the upgrade is complete so clients can initiate new
			// upgrades when possible
			STISManager::getInstance()->messageLibraryUpgraded( TA_IRS_App::STATION_LIBRARY );

			try
			{
				std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary>
					oldLibrary(TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( nowPreviousMessageLibraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS ));
				oldLibrary->deleteThisLibrary();
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
				std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary>
					oldLibrary(TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( nowPreviousMessageLibraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS ));
				oldLibrary->deleteThisLibrary();
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
