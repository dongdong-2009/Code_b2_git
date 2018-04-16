#include "LibrarySyncFailedProcessor.h"
#include "WorkThread.h"
#include "StatusMonitor.h"
#include "MessageLibrarySynchroniser.h"
#include "StringConverter.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

namespace TA_IRS_App
{
    using TA_Base_Core::DebugUtil;
    LibrarySyncFailedProcessor::LibrarySyncFailedProcessor(ProcessorType type, unsigned short repeatInterval)
        : m_processThread(NULL),
          m_type(type),
		  m_processRepeatInterval(repeatInterval),
		  m_sessionID("INTERNAL")
    {
        m_processThread =
            new WorkThread(boost::bind(&LibrarySyncFailedProcessor::processFailedStations, this));
        switch(type)
        {
            case STIS_SYNCHRONISE:
                processFailedStation = boost::bind(&LibrarySyncFailedProcessor::processSTISSynchronise, this, _1);
                getCDBVersion = boost::bind(&StatusMonitor::getNextISCSStationLibraryVersion_central, StatusMonitor::getInstance());
				getStationVersion = boost::bind(&LibrarySyncFailedProcessor::getNextSTISVersion, this, _1);
                break;
            case STIS_UPGRADE:
                processFailedStation = boost::bind(&LibrarySyncFailedProcessor::processSTISUpgrade, this, _1);
                getCDBVersion = boost::bind(&StatusMonitor::getCurrentISCSStationLibraryVersion_central, StatusMonitor::getInstance());
				getStationVersion = boost::bind(&LibrarySyncFailedProcessor::getCurrentSTISVersion, this, _1);
                break;
            case TTIS_SYNCHRONISE:
                processFailedStation = boost::bind(&LibrarySyncFailedProcessor::processTTISSynchronise, this, _1);
                getCDBVersion = boost::bind(&StatusMonitor::getNextISCSTrainLibraryVersion_central, StatusMonitor::getInstance());
				getStationVersion = boost::bind(&LibrarySyncFailedProcessor::getNextTTISVersion, this, _1);
                break;
            case TTIS_UPGRADE:
                processFailedStation = boost::bind(&LibrarySyncFailedProcessor::processTTISUpgrade, this, _1);
                getCDBVersion = boost::bind(&StatusMonitor::getCurrentISCSTrainLibraryVersion_central, StatusMonitor::getInstance());
				getStationVersion = boost::bind(&LibrarySyncFailedProcessor::getCurrentTTISVersion, this, _1);
                break;
            default:
                TA_ASSERT(false, "Unknown processor type");
        }
		// Init corbaobject for all stations
        TA_Base_Core::CorbaNameList allTisAgents =
			TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfType( TA_Base_Core::STISEntityData::getStaticType(),false );
		unsigned long occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
		BOOST_FOREACH(TA_Base_Core::CorbaName& i, allTisAgents)
		{
			if(occLocationKey != i.getLocationKey())
			{
				m_stisCorbaObjects.push_back(StisNamedObjectPtr(new StisNamedObject(i)));
			}
		}
    }

    LibrarySyncFailedProcessor::~LibrarySyncFailedProcessor(void)
    {
		m_processThread->terminateAndWait();
		delete m_processThread;
    }

    void LibrarySyncFailedProcessor::startProcess()
    {
        if(STIS_SYNCHRONISE == m_type || TTIS_SYNCHRONISE == m_type)
        {
            try
            {
                unsigned long cdbVersion = getCDBVersion();
                std::vector<unsigned char> libraryFile;
                MessageLibrarySynchroniser::getLibraryFileFromDB(libraryFile,
                                                                 cdbVersion,
                                                                 STIS_SYNCHRONISE == m_type ? STATION_LIBRARY : TRAIN_LIBRARY);
                m_libraryFile = StringConverter::vectorCharToCorbaCharSequence(libraryFile);
                if(m_libraryFile.length() == 0)
                {
                    throw TA_Base_Core::TransactiveException("Failed get library content");
                }
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exception occur when start %s processor", processTypeToString(m_type).c_str());
                return;
            }
        }
        m_processThread->start();
    }

    void LibrarySyncFailedProcessor::stopProcess()
    {
        m_processThread->terminateAndWait();
    }

	std::string LibrarySyncFailedProcessor::processTypeToString(ProcessorType type)
	{
		switch(type)
		{
		case STIS_SYNCHRONISE:
			return "STIS_SYNCHRONISE";
		case STIS_UPGRADE:
			return "STIS_UPGRADE";
		case TTIS_SYNCHRONISE:
			return "TTIS_SYNCHRONISE";
		case TTIS_UPGRADE:
			return "TTIS_UPGRADE";
		default:
			return "UNKNOWN";
		}
	}

    void LibrarySyncFailedProcessor::processFailedStations()
    {
        unsigned long cdbVersion = getCDBVersion();
        std::vector<StisNamedObjectPtr> failedStations;
		BOOST_FOREACH(StisNamedObjectPtr& i, m_stisCorbaObjects)
        {
            unsigned long value = 0;
			if(m_processThread->shouldRunning())
			{
	            try
	            {
	                value = getStationVersion(*(i.get()));
	            }
	            catch(...)
	            {
	            }
	            if(value != cdbVersion)
	            {
	                failedStations.push_back(i);
	            }
			}
        }
        std::vector<StisNamedObjectPtr>::iterator it = failedStations.begin();
        while(m_processThread->shouldRunning() && !failedStations.empty())
        {
            if(processFailedStation(*(it->get())))
            {
                it = failedStations.erase(it);
                if(failedStations.empty())
                {
                    return;
                }
            }
			else if(++it == failedStations.end())
			{
				it = failedStations.begin();
			}
            m_processThread->sleep(m_processRepeatInterval);
        }
    }

    bool LibrarySyncFailedProcessor::processSTISSynchronise(StisNamedObject& stisCorbaObject)
    {
        bool isSuccess = false;
        try
        {
            unsigned long version = getNextSTISVersion(stisCorbaObject);
			unsigned long nextCDBVersion = getCDBVersion();
            if(version == nextCDBVersion)
            {
                isSuccess = true;
                return isSuccess;
            }
            CORBA_CALL_RETURN(isSuccess,
                              stisCorbaObject,
                              saveSTISLibraryToLocalDB,
                              (nextCDBVersion,
                               m_libraryFile));
        }
        catch(...)
        {
        }
        return isSuccess;
    }

    bool LibrarySyncFailedProcessor::processTTISSynchronise(StisNamedObject& stisCorbaObject)
    {
        bool isSuccess = false;
        try
        {
            unsigned long version = getNextTTISVersion(stisCorbaObject);
			unsigned long nextCDBVersion = getCDBVersion();
            if(version == nextCDBVersion)
            {
                isSuccess = true;
                return isSuccess;
            }
            CORBA_CALL_RETURN(isSuccess,
                              stisCorbaObject,
                              saveTTISLibraryToLocalDB,
                              (nextCDBVersion,
                               m_libraryFile));
        }
        catch(...)
        {
        }
        return isSuccess;
    }

    bool LibrarySyncFailedProcessor::processSTISUpgrade(StisNamedObject& stisCorbaObject)
    {
        bool isSucess = false;
        try
        {
            unsigned long currentVersion = getCurrentSTISVersion(stisCorbaObject);
			unsigned long currentCDBVersion = getCDBVersion();
            if(currentVersion == currentCDBVersion)
            {
                isSucess = true;
                return isSucess;
            }
			// Check if next version is equal to cdb version
			unsigned long nextVersion = getNextSTISVersion(stisCorbaObject);
			if(nextVersion != currentCDBVersion)
			{
				isSucess = false;
				return isSucess;
			}
			// If station next version equal to current CDB version, do update operation
            CORBA_CALL(stisCorbaObject,
                       upgradePredefinedStationMessageLibrary,
                       (currentCDBVersion, m_sessionID.c_str()));
            isSucess = true;
        }
        catch(...)
        {
        }
        return isSucess;
    }

    bool LibrarySyncFailedProcessor::processTTISUpgrade(StisNamedObject& stisCorbaObject)
    {
        bool isSucess = false;
        try
        {
            unsigned long currentVersion = getCurrentTTISVersion(stisCorbaObject);
			unsigned long currentCDBVersion = getCDBVersion();
            if(currentVersion == currentCDBVersion)
            {
                isSucess = true;
                return isSucess;
            }
			// Check if next version is equal to cdb version
			unsigned long nextVersion = getNextTTISVersion(stisCorbaObject);
			if(nextVersion != currentCDBVersion)
			{
				isSucess = false;
				return isSucess;
			}
			// If station next version equal to current CDB version, do update operation
            CORBA_CALL(stisCorbaObject,
                       upgradePredefinedTrainMessageLibrary,
                       (currentCDBVersion, m_sessionID.c_str()));
            isSucess = true;
        }
        catch(...)
        {
        }
        return isSucess;
    }

	unsigned long LibrarySyncFailedProcessor::getNextSTISVersion(StisNamedObject& stisCorbaObject)
	{
        unsigned long version = 0;
        CORBA_CALL_RETURN(version, stisCorbaObject, getNextSTISMessageLibraryVersion, ());
		return version;
	}

	unsigned long LibrarySyncFailedProcessor::getCurrentSTISVersion(StisNamedObject& stisCorbaObject)
	{
        unsigned long version = 0;
        CORBA_CALL_RETURN(version, stisCorbaObject, getCurrentSTISMessageLibraryVersion, ());
		return version;
	}

	unsigned long LibrarySyncFailedProcessor::getNextTTISVersion(StisNamedObject& stisCorbaObject)
	{
        unsigned long version = 0;
        CORBA_CALL_RETURN(version, stisCorbaObject, getNextTTISMessageLibraryVersion, ());
		return version;
	}

	unsigned long LibrarySyncFailedProcessor::getCurrentTTISVersion(StisNamedObject& stisCorbaObject)
	{
        unsigned long version = 0;
        CORBA_CALL_RETURN(version, stisCorbaObject, getCurrentTTISMessageLibraryVersion, ());
		return version;
	}

	void LibrarySyncFailedProcessor::setSessionId(std::string sessionId)
	{
		m_sessionID = sessionId;
	}
}
