/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/scada_historian/src/ScadaHistorianAgentProcess.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include <string>
#include "app/history/scada_historian/src/ScadaHistorianAgentProcess.h"
#include "app/history/scada_historian/src/DataPointCache.h"
#include "app/history/scada_historian/src/HistoryConfig.h"
#include "app/history/scada_historian/src/DataPointStateUpdateSubscriber.h"
#include "app/history/scada_historian/src/DatapointOnlineUpdateSubscriber.h"
#include "app/history/scada_historian/src/DatapointLoaderThread.h"
#include "app/history/scada_historian/src/ProcessTimerThread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataRecordingAgentException.h"
#include "core/exceptions/src/GenericAgentException.h"
#include "core/message/types/DataRecordingAgentAlarms_MessageTypes.h"


using TA_Core::RunParams;
using TA_Core::DebugUtil;
using TA_Core::DataException;
using TA_Core::DataRecordingAgentException;
using TA_Core::GenericAgentException;
using TA_Core::ScadaHistoryAgentEntityData;
using TA_Core::IEntityData;
using TA_Core::DataPointEntityData;


namespace TA_App
{

    //
    // Constructor
    //
    ScadaHistorianAgentProcess::ScadaHistorianAgentProcess()
    : m_datapointLoaderThread(NULL),
	  m_processTimerThread(NULL),
      m_dataPointOnlineUpdateSubscriber(NULL),
	  m_semaphore(0)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "constructor");

        try
        {
            //
            // Retrieve the entity key and type key of the agent and store as
            // run params so they can be retrieved later
            //
            IEntityData* agentData = TA_Core::EntityAccessFactory::getInstance().getEntity(TA_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
            unsigned long entitykey = agentData->getKey();
            unsigned long typekey = agentData->getTypeKey();

            char buffer[500];
            sprintf(buffer, "%lu", entitykey);
            TA_Core::RunParams::getInstance().set("EntityKey", buffer);
            sprintf(buffer, "%lu", typekey);
            TA_Core::RunParams::getInstance().set("TypeKey", buffer);

            checkEntityParameters(agentData);

	        //
            // Close all Data Recording Agent alarms
            //
            TA_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(TA_Core::DataRecordingAgentAlarms::MaxTrendsPerLocationReached,
                atoi(RunParams::getInstance().get("EntityKey").c_str()), 0);
			TA_Core::AlarmHelperManager::getInstance().getAlarmHelper().cleanUp();
		    // This will throw an exception if the datapoints
		    // could not be loaded. Let the exception be
		    // caught higher up.
		    //
            m_datapointLoaderThread = new DatapointLoaderThread(this);

		    m_processTimerThread = new ProcessTimerThread;

		    //
		    // Subscriber to online updates of datapoints
		    //
            m_dataPointOnlineUpdateSubscriber = new DataPointOnlineUpdateSubscriber(this);
        }
        catch(...)
        {
            if(m_datapointLoaderThread != NULL)
            {
                delete m_datapointLoaderThread;
                m_datapointLoaderThread = NULL;
            }

            if(m_processTimerThread != NULL)
            {
                delete m_processTimerThread;
                m_processTimerThread = NULL;
            }

            if(m_dataPointOnlineUpdateSubscriber != NULL)
            {
                delete m_dataPointOnlineUpdateSubscriber;
                m_dataPointOnlineUpdateSubscriber = NULL;
            }
        }
    }


    //
    // Destructor
    //
    ScadaHistorianAgentProcess::~ScadaHistorianAgentProcess()
    {
        try
        {
            if(m_datapointLoaderThread != NULL)
		    {
			    delete m_datapointLoaderThread;
			    m_datapointLoaderThread = NULL;
		    }

            if(m_processTimerThread != NULL)
		    {
			    delete m_processTimerThread;
			    m_processTimerThread = NULL;
		    }

		    std::vector<DataPointStateUpdateSubscriber*>::iterator it = m_dataPointStateUpdateSubscribers.begin();
            for(; it != m_dataPointStateUpdateSubscribers.end(); ++it)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Deleting state update subscriber");
                delete *it;
                m_dataPointStateUpdateSubscribers.erase(it);
                *it = NULL;
            }

            if(m_dataPointOnlineUpdateSubscriber != NULL)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Deleting online update subscriber");
                //m_dataPointOnlineUpdateSubscriber->removeAllInterestedKeys();
                delete m_dataPointOnlineUpdateSubscriber;
                m_dataPointOnlineUpdateSubscriber = NULL;
            }
        }
        catch(...)
        {
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "destructor");
    }


    //
    // run
    //
    void ScadaHistorianAgentProcess::run()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "run");

		//
		// Load the datapoints into the cache
		//
        m_datapointLoaderThread->start();

		//
		// Give the loader some time to insert the datapoints into
		// the cache and then start the process thread
		//
		int sleepInMilli = 2000;
		sleep(sleepInMilli);
		m_processTimerThread->start();

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Just waiting for events...");

        // PW 3374: Use a semaphore instead of looping continually to check
        // termination flag. Termination flag has been removed.
        m_semaphore.wait();

		LOG (SourceInfo, DebugUtil::FunctionExit, "run");
    }


    //
    // terminate
    //
    void ScadaHistorianAgentProcess::terminate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "terminate");

        if(m_datapointLoaderThread != NULL)
		{
			m_datapointLoaderThread->terminateAndWait();
		}

        //
        // Because the process timer thread is not started immediately in run(),
        // we want to make sure that it has actually started before attempting
        // to terminate it. Otherwise this thread will continue running while
        // the agent is shutting down. An example of when this could happen is
        // if the agent is terminated immediately after it has been started.
        //
		if(m_processTimerThread != NULL)
		{
            while(!m_processTimerThread->isThreadStarted())
            {
                sleep(100);
            }

			m_processTimerThread->terminateAndWait();
		}

        // PW 3374: Use a semaphore instead of looping continually to check
        // termination flag. Termination flag has been removed.
        m_semaphore.post();

		LOG ( SourceInfo, DebugUtil::FunctionExit, "terminate");
    }


    //
    // addHistoryConfig
    //
    void ScadaHistorianAgentProcess::addHistoryConfig(HistoryConfig* historyConfig)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "addHistoryConfig");

        m_processTimerThread->addHistoryConfig(historyConfig);

        LOG(SourceInfo, DebugUtil::FunctionExit, "addHistoryConfig");
    }


    //
    // addDataPointSubscriber
    //
    void ScadaHistorianAgentProcess::addDataPointSubscriber(unsigned long entitykey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "addDataPointSubscriber");

        //
		// Both Control and Monitor need to create a subscriber
		// they can update their caches if a datapoint changes
		// state or value
		//
        DataPointStateUpdateSubscriber* dpSubscriber = new DataPointStateUpdateSubscriber(this, entitykey);
        m_dataPointStateUpdateSubscribers.push_back(dpSubscriber);

        LOG(SourceInfo, DebugUtil::FunctionExit, "addDataPointSubscriber");
    }


    //
    // removeDataPointSubscriber
    //
    void ScadaHistorianAgentProcess::removeDataPointSubscriber(unsigned long entitykey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "removeDataPointSubscriber");

        std::vector<DataPointStateUpdateSubscriber*>::iterator it = m_dataPointStateUpdateSubscribers.begin();
        for(; it != m_dataPointStateUpdateSubscribers.end(); ++it)
        {
            if((*it)->getEntitykey() == entitykey)
            {
                delete *it;
                m_dataPointStateUpdateSubscribers.erase(it);
                *it = NULL;
                break;
            }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "removeDataPointSubscriber");
    }


    //
    // processUpdatedDataPoint
    //
    void ScadaHistorianAgentProcess::processUpdatedDataPoint(unsigned long entitykey, const TA_Bus::DataPointCorbaDef::ClientDataPointState dpState)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processUpdatedDataPoint");

        DataPointData dpData(entitykey, dpState);
        DataPointCache::getInstance().updateDataPointCache(dpData);

        LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdatedDataPoint");
    }


    //
    // processUpdateCfgEvent
    //
    void ScadaHistorianAgentProcess::processUpdateCfgEvent(unsigned long entitykey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processUpdateCfgEvent");

        IEntityData* entityData = TA_Core::EntityAccessFactory::getInstance().getEntity(entitykey);
        entityData->invalidate();

        DataPointEntityData* dpData = dynamic_cast <DataPointEntityData*> (entityData);
        bool trendingEnabled = dpData->getEnableTrending();

        //
        // If trending is enabled and the datapoint doesn't exist in the cache,
        // load the datapoint. Once loaded in cache, it will remain in the cache
        // until the entity is deleted.
        //
        if(!DataPointCache::getInstance().dataPointExists(entitykey))
        {
            if(trendingEnabled)
            {
                unsigned long stationKey;
                std::string stationName;
                try
                {
                    m_datapointLoaderThread->loadStationNameAndKey(entitykey, stationName, stationKey);
                }
                catch(DataRecordingAgentException& ex)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                        "There was a problem loading data point: %lu for trending. "
                        "Is an associated Station configured? Details: %s",
                        ex.what());
                    return;
                }

                DatapointLoaderThread::DataPoints datapoint;
                datapoint.pKey = uLongToString(dpData->getKey());
                datapoint.name = dpData->getName();
                //datapoint.typeName = dpData->getType();
                //datapoint.subsystemName = dpData->getSubsystemName();
                datapoint.agentName = dpData->getAgentName();

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Storing for resolving.", entitykey);

                m_datapointLoaderThread->storeDatapointForResolving(datapoint, stationKey, stationName);
            }
            else
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Removing from resolving.", entitykey);

                //
                // The datapoint has been loaded but it has not been resolved and
                // inserted into the cache yet. Remove it from the list of datapoints
                // to resolve.
                //
                try
                {
                    m_datapointLoaderThread->removeDatapointForResolving(entitykey);
                }
                catch(DataRecordingAgentException& ex)
                {
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                        "There was a problem removing data point: %lu from trending. Details: %s",
                        ex.what());
                    return;
                }
            }
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Updating cache.", entitykey);

            try
            {
                m_datapointLoaderThread->datapointUpdateTrendingEnabled(entitykey, trendingEnabled);
            }
            catch(DataRecordingAgentException& ex)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                    "There was a problem updating cache for datapoint: %lu. Details: %s",
                    ex.what());
                return;
            }

            //
            // It's already in the cache, so just update the trending status. We don't care
            // about the other parameters at this stage.
            //
            DataPointCache::getInstance().dataCollectionOn(entitykey, trendingEnabled);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdateCfgEvent");
    }


    //
    // processCreateCfgEvent
    //
    void ScadaHistorianAgentProcess::processCreateCfgEvent(unsigned long entitykey, IEntityData* entityData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processCreateCfgEvent");

        DataPointEntityData* dpData = dynamic_cast <DataPointEntityData*> (entityData);
        bool trendingEnabled = dpData->getEnableTrending();

        //
        // Only deal with this new data point if it has trending enabled.
        // If it has been enabled, we want to store the datapoint into the cache
        // so we can sample it.
        //
        if(trendingEnabled)
        {
            unsigned long stationKey;
            std::string stationName;

            try
            {
                m_datapointLoaderThread->loadStationNameAndKey(entitykey, stationName, stationKey);
            }
            catch(DataRecordingAgentException& ex)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                        "There was a problem loading data point: %lu for trending. "
                        "Is an associated Station configured? Details: %s",
                        ex.what());
                return;
            }

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Inserting into cache.", entitykey);

            //
            // Don't need to check if the maximum datapoints per station has been reached
            // because storeDatapointForResolving() will check this and send an alarm
            // if the max has been reached.
            //

            DatapointLoaderThread::DataPoints datapoint;
            datapoint.pKey = uLongToString(dpData->getKey());
            datapoint.name = dpData->getName();
            datapoint.agentName = dpData->getAgentName();

            m_datapointLoaderThread->storeDatapointForResolving(datapoint, stationKey, stationName);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "processCreateCfgEvent");
    }


    //
    // processDeleteCfgEvent
    //
    void ScadaHistorianAgentProcess::processDeleteCfgEvent(unsigned long entitykey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processDeleteCfgEvent");

        if(!DataPointCache::getInstance().dataPointExists(entitykey))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Removing from resolving.", entitykey);

            //
            // The datapoint has been loaded but it has not been resolved and
            // inserted into the cache yet. Remove it from the list of datapoints
            // to resolve.
            //
            try
            {
                m_datapointLoaderThread->removeDatapointForResolving(entitykey);
            }
            catch(DataRecordingAgentException& ex)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                    "There was a problem removing data point: %lu from trending. Details: %s",
                    ex.what());
                return;
            }
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
                    "Online update for datapoint: %lu. Removing from cache.", entitykey);

            try
            {
                m_datapointLoaderThread->datapointUpdateTrendingEnabled(entitykey, false);
            }
            catch(DataRecordingAgentException& ex)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                    "There was a problem removing data point: %lu from trending. Details: %s",
                    ex.what());
                return;
            }

            //
            // Remove the datapoint from the cache
            //
            DataPointCache::getInstance().remove(entitykey);
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "processDeleteCfgEvent");
    }


    //
    // setToControl
    //
    void ScadaHistorianAgentProcess::setToControl(bool isControl)
    {
        m_processTimerThread->setToControl(isControl);
    }


    //
    // checkEntityParameters
    //
    void ScadaHistorianAgentProcess::checkEntityParameters(IEntityData* entityData)
    {
        //
        // MaxTrendDatapointsPerStation
        //
        int maxTrendDatapointsPerStation;
        try
        {
            char buffer[500];
            maxTrendDatapointsPerStation = (dynamic_cast<ScadaHistoryAgentEntityData*>(entityData))->getMaxTrendDatapointsPerStation();

            if(maxTrendDatapointsPerStation > 0)
            {
                sprintf(buffer, "%d", maxTrendDatapointsPerStation);
                TA_Core::RunParams::getInstance().set(MAX_TREND_DPS_PER_STATION_CFG, buffer);

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm,
                    "Setting MaxTrendDatapointsPerStation to: %d", maxTrendDatapointsPerStation);
            }
            else
            {
                sprintf(buffer, "Invalid configuration parameter: %s. Must be greater than 0.",
                    MAX_TREND_DPS_PER_STATION_CFG);
                TA_THROW(GenericAgentException(GenericAgentException::CONFIGURATION_ERROR, buffer));

            }
        }
        catch(DataException& ex)
        {
            //
            // There was a problem retrieving the value, set to default
            //
            maxTrendDatapointsPerStation = DEFAULT_MAX_TREND_DPS_PER_STATION_CFG;

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An exception was caught "
                "while retrieving %s.\nDetails: %s\n%s set to default value of: %d",
                MAX_TREND_DPS_PER_STATION_CFG, ex.what(), MAX_TREND_DPS_PER_STATION_CFG, DEFAULT_MAX_TREND_DPS_PER_STATION_CFG);
        }
    }


    //
    // uLongToString
    //
    std::string ScadaHistorianAgentProcess::uLongToString(unsigned long ulong)
    {
        char buffer[500];
        sprintf(buffer, "%lu", ulong);
        return std::string(buffer);
    }

} // TA_App










