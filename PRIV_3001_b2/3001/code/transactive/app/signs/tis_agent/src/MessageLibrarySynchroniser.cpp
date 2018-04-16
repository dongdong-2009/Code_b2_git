/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/MessageLibrarySynchroniser.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include "app/signs/tis_agent/src/MessageLibrarySynchroniser.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "app/signs/tis_agent/src/ILocationStore.h"
#include "StringConverter.h"
#include <vector>
#include <string>
#include <boost/bind.hpp>

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
        TA_ASSERT(STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                  "MessageLibrarySynchroniser is only available at the OCC");

        // Set this to true when the thread is first started.  From then on,
        // only attempt to synchronise, if initiateSynchronisation() is called
        m_librariesSynchronised = true;

        while(m_shouldBeRunning)
        {
            //////////////////////////////////////////////////////////////////////
            // Checking library version process
            /////////////////////////////////////////////////////////////////////
            // Repeat until the timer expires, or the libraries are synchronised
            // timeout is stored in the database in minutes - convert to milliseconds
            if(!m_librariesSynchronised)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Checking library versions...");
                // Start threads synchronise db
                m_dbSynchroniser.start();

                TA_Base_Core::CorbaNameList agentsToSynchronise;
                try
                {
                    agentsToSynchronise = createListOfAgentsToSynchronise();
                }
                catch(const TA_Base_Core::DataException& e)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
                }
                if(!agentsToSynchronise.empty())
                {
                    m_threadPool.start(agentsToSynchronise.size());
                }
                // Start threads check if db synchronise successfully
                m_notSyncStationsCount = agentsToSynchronise.size();
                for(TA_Base_Core::CorbaNameList::iterator it = agentsToSynchronise.begin();
                    it != agentsToSynchronise.end();
                    ++it)
                {
                    m_threadPool.run(boost::bind(STATION_LIBRARY == m_libraryType ?
                                                 &MessageLibrarySynchroniser::checkStisLibraryVersion : &MessageLibrarySynchroniser::checkTtisLibraryVersion,
                                                 this,
                                                 new StisNamedObject(*it)));
                }

                while(m_shouldBeRunning && m_notSyncStationsCount > 0 && m_timer.elapsedTime() < m_stationSynchTimeout * 60)
                {
                    TA_Base_Core::Thread::sleep(1000);
                }
                if(m_notSyncStationsCount <= 0)
                {
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                        "Successfully synchronised all stations");

                    // set the flag that means its done
                    m_librariesSynchronised = true;
                    // update the status
                    synchronisationComplete();
                }
                else if(m_timer.elapsedTime() >= m_stationSynchTimeout * 60)
                {
                    // Even if the libraries are not synchronised by this point ( we got here because the synch timed out)
                    // we probably want to notify the STISManager to go ahead anyway, because
                    // a station is probably down.

                    // set the flag that means its done
                    m_librariesSynchronised = true;
                    // update the status
                    synchronisationTimedOut();
                }
                // Terminate synchronise threads
                m_dbSynchroniser.terminateAndWait();
                m_threadPool.stop();
            } // if ( !m_librariesSynchronised )
            this->sleep(THREAD_SLEEP_TIME);
        } // while

        FUNCTION_EXIT;
    }

    void MessageLibrarySynchroniser::terminate()
    {
        FUNCTION_ENTRY("terminate");

        m_shouldBeRunning = false;
        // Terminate synchronise threads
        m_dbSynchroniser.terminateAndWait();
        m_threadPool.stop();
        FUNCTION_EXIT;
    }


    MessageLibrarySynchroniser::MessageLibrarySynchroniser(int stationSynchTimeout, TA_IRS_App::EMessageLibraryType type)
        : m_shouldBeRunning(true),
          m_dbSynchroniser(type)
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
        TA_Base_Core::CorbaNameList allTisAgents = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType(TA_Base_Core::STISEntityData::getStaticType(), false);

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

        if(m_libraryType == TA_IRS_App::STATION_LIBRARY)
        {
            StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);

            STISManager::getInstance()->messageLibrarySynchronised(TA_IRS_App::STATION_LIBRARY);

            TA_Base_Core::DescriptionParameters dp;

            std::stringstream versionStream;
            versionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

            TA_Base_Core::NameValuePair nvp("Version", versionStream.str());

            dp.push_back(&nvp);

            STISManager::getInstance()->submitAlarm(TA_Base_Core::TISAlarms::StationLibraryPendingUpgrade, dp);


            //TES831 Events not logged!
            TA_Base_Core::DescriptionParameters desc;

            TA_Base_Core::NameValuePair versionVP("version", versionStream.str());
            TA_Base_Core::NameValuePair failedVP("Failed on:", "");
            TA_Base_Core::NameValuePair listVP("Failed station list", "");

            desc.push_back(&versionVP);
            desc.push_back(&failedVP);
            desc.push_back(&listVP);

            //TES831 Events not logged!

            // TODO: Remove when the STISManager GUI has the 'Upgrade ISCS' button
            // STISManager::getInstance()->upgradePredefinedStationMessageLibrary( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion(), "INTERNAL" );
        }
        else if(m_libraryType == TA_IRS_App::TRAIN_LIBRARY)
        {
            StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);

            STISManager::getInstance()->messageLibrarySynchronised(TA_IRS_App::TRAIN_LIBRARY);

            TA_Base_Core::DescriptionParameters dp;

            std::stringstream versionStream;
            versionStream << StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

            TA_Base_Core::NameValuePair nvp("Version", versionStream.str());

            dp.push_back(&nvp);

            STISManager::getInstance()->submitAlarm(TA_Base_Core::TISAlarms::TrainLibraryPendingUpgrade, dp);


            //TES831 Events not logged!
            TA_Base_Core::DescriptionParameters desc;

            std::stringstream libraryVersionStream;

            libraryVersionStream << StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();

            TA_Base_Core::NameValuePair versionVP("version", libraryVersionStream.str());
            TA_Base_Core::NameValuePair failedVP("Failed on:", "");
            TA_Base_Core::NameValuePair listVP("Failed station list", "");

            desc.push_back(&versionVP);
            desc.push_back(&failedVP);
            desc.push_back(&listVP);

        }
    }


    void MessageLibrarySynchroniser::synchronisationTimedOut()
    {
        if(m_libraryType == TA_IRS_App::STATION_LIBRARY)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "Timed out while attempting to synchronise the predefined STIS message library - considering the synchronisation complete - possible station failure");
            StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);
            STISManager::getInstance()->messageLibrarySynchronised(TA_IRS_App::STATION_LIBRARY);
        }
        else if(m_libraryType == TA_IRS_App::TRAIN_LIBRARY)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "Timed out while attempting to synchronise the predefined TTIS message library - considering the synchronisation complete - possible station failure");
            StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);
            STISManager::getInstance()->messageLibrarySynchronised(TA_IRS_App::TRAIN_LIBRARY);
        }
    }

    void MessageLibrarySynchroniser::getLibraryFileFromDB(std::vector<unsigned char>& dbFileContent, unsigned long version, EMessageLibraryType type)
    {
        FUNCTION_ENTRY("getLibraryFileFromDB");

        std::string libraryType = ((STATION_LIBRARY == type) ? "STIS" : "TTIS");
        std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary> predefinedMessageLibrary;
        predefinedMessageLibrary = std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary>
                                   (TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(version, libraryType));
        if(NULL == predefinedMessageLibrary.get())
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not get the %s predefined message library, VERSION[%d]", libraryType.c_str(), version);
            FUNCTION_EXIT;
            return;
        }
        predefinedMessageLibrary->getLibraryFileContent(dbFileContent);

        FUNCTION_EXIT;
    }

    void MessageLibrarySynchroniser::checkStisLibraryVersion(StisNamedObject* target)
    {
        bool received = false;
        bool offline = false;
        while(m_shouldBeRunning && !received && m_timer.elapsedTime() < (m_stationSynchTimeout * 60))
        {
            try
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Checking predefined station message library at:%s", target->getAgentName().c_str());
                CORBA_CALL_RETURN(received,
                                  (*target),
                                  reportStationLibraryVersionReceived,
                                  (StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central()));
                received ?
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully established station message library at:%s", target->getAgentName().c_str())
                : LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to established station message library at:%s", target->getAgentName().c_str());
            }
            catch(const TA_Base_Core::ObjectResolutionException&)
            {
                offline = true;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not resolve the TisAgent:%s", target->getAgentName().c_str());
            }
            catch(const CORBA::Exception&)
            {
                offline = true;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught CORBA exception TisAgent:%s", target->getAgentName().c_str());
            }
            if(received || offline)
            {
                m_notSyncStationsCount--;
                break;
            }
            TA_Base_Core::Thread::sleep(1000);
        }
        delete target;
    }

    void MessageLibrarySynchroniser::checkTtisLibraryVersion(StisNamedObject* target)
    {
        bool received = false;
        bool offline = false;
        while(m_shouldBeRunning && !received && m_timer.elapsedTime() < (m_stationSynchTimeout * 60))
        {
            try
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Checking predefined train message library at:%s", target->getAgentName().c_str());
                CORBA_CALL_RETURN(received,
                                  (*target),
                                  reportTrainLibraryVersionReceived,
                                  (StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central()));
                received ?
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Successfully established station message library at:%s", target->getAgentName().c_str())
                : LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to established station message library at:%s", target->getAgentName().c_str());
            }
            catch(const TA_Base_Core::ObjectResolutionException&)
            {
                offline = true;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not resolve the TisAgent:%s", target->getAgentName().c_str());
            }
            catch(const CORBA::Exception&)
            {
                offline = true;
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught CORBA exception TisAgent:%s", target->getAgentName().c_str());
            }
            if(received || offline)
            {
                m_notSyncStationsCount--;
                break;
            }
            TA_Base_Core::Thread::sleep(1000);
        }
        delete target;
    }

    //////////////////////////////////////////////////////////////////////////
    //////////////LibraryDBSynchroniser
    //////////////////////////////////////////////////////////////////////////

    LibraryDBSynchroniser::LibraryDBSynchroniser(TA_IRS_App::EMessageLibraryType libraryType)
        : m_libraryType(libraryType),
          m_shouldBeRunning(false)
    {

    }

    void LibraryDBSynchroniser::run()
    {
        m_shouldBeRunning = true;

        TA_Base_Core::CorbaNameList agentsDbToSynchronise;

        std::vector<unsigned char> dbFileContent;
        try
        {
            unsigned long version = (STATION_LIBRARY == m_libraryType ?
                                     StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central()
                                     : StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central());
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Get %s library from DB, VERSION[%d]",
                        STATION_LIBRARY == m_libraryType ? "STIS" : "TTIS", version);
            MessageLibrarySynchroniser::getLibraryFileFromDB(dbFileContent, version, m_libraryType);
            m_corbaFileContent = StringConverter::vectorCharToCorbaCharSequence(dbFileContent);
        }
        catch(TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", e.what());
            m_shouldBeRunning = false;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "While attempting to retrieve the current Predefined Message Library from the Database");
            m_shouldBeRunning = false;
        }
        if(!m_shouldBeRunning)
        {
            return;
        }

        try
        {
            agentsDbToSynchronise = TA_Base_Core::EntityAccessFactory::
                                    getInstance().getCorbaNamesOfType(TA_Base_Core::STISEntityData::getStaticType(), false);
        }
        catch(TA_Base_Core::DataException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", e.what());
        }

        if(agentsDbToSynchronise.empty())
        {
            return;
        }
        m_threadPool.start(agentsDbToSynchronise.size());
        for(TA_Base_Core::CorbaNameList::iterator it = agentsDbToSynchronise.begin();
            it != agentsDbToSynchronise.end();
            ++it)
        {
            m_threadPool.run(boost::bind(STATION_LIBRARY == m_libraryType ?
                                         &LibraryDBSynchroniser::synchroniseStis : &LibraryDBSynchroniser::synchroniseTtis,
                                         this,
                                         new StisNamedObject(*it),
                                         m_corbaFileContent));
        }
    }

    void LibraryDBSynchroniser::terminate()
    {
        m_shouldBeRunning = false;
        m_threadPool.stop();
    }

    LibraryDBSynchroniser::~LibraryDBSynchroniser()
    {
    }

    void LibraryDBSynchroniser::synchroniseStis(StisNamedObject* target,
                                                const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& libraryContent)
    {
        bool isSuccessStart = false;
        int retry = 0;
        while(!isSuccessStart && m_shouldBeRunning && retry++ < 3)
        {
            try
            {

                CORBA_CALL_RETURN(isSuccessStart,
                                  (*target),
                                  saveSTISLibraryToLocalDB,
                                  (StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central(),
                                   libraryContent));
            }
            catch(const TA_Base_Core::ObjectResolutionException& ore)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what());
            }
            catch(const CORBA::Exception&)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", "");
            }
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "STIS library db synchronise at %s %s", target->getAgentName().c_str(), isSuccessStart ? "successful" : "failed");
        }
        delete target;
    }

    void LibraryDBSynchroniser::synchroniseTtis(StisNamedObject* target,
                                                const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& libraryContent)
    {
        bool isSuccessStart = false;
        int retry = 0;
        while(!isSuccessStart && m_shouldBeRunning && retry++ < 3)
        {
            try
            {

                CORBA_CALL_RETURN(isSuccessStart,
                                  (*target),
                                  saveTTISLibraryToLocalDB,
                                  (StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central(),
                                   libraryContent));
            }
            catch(const TA_Base_Core::ObjectResolutionException& ore)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what());
            }
            catch(const CORBA::Exception&)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", "");
            }
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                        "TTIS library db synchronise at %s %s", target->getAgentName().c_str(), isSuccessStart ? "successful" : "failed");
        }
        delete target;
    }

}  // namespace TA_IRS_App