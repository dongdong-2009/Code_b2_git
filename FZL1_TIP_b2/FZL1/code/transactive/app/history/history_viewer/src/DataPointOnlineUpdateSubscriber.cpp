#if !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_CPP)
#define DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_CPP

/**
  * DataPointOnlineUpdateSubscriber.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/DataPointOnlineUpdateSubscriber.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class listens for datapoint online updates.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/DatapointOnlineUpdateSubscriber.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/FunctionEntry.h"

#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

const std::string DATAPOINT_TYPENAME("DataPoint");

const std::string ENTITY_NAME("Name");
const std::string DESCRIPTION("Description");
const std::string DATAPOINT_TYPE_ENTITY_PARAM("DataPointType");
const std::string ENG_LIMIT_LOW_ENTITY_PARAM("EngineeringLimitLow");
const std::string ENG_LIMIT_HIGH_ENTITY_PARAM("EngineeringLimitHigh");
const std::string ENG_UNITS_ENTITY_PARAM("EngineeringUnits");
const std::string TRENDING_ENABLED_ENTITY_PARAM("EnableTrending");

const std::string ADDRESS("Address");
const std::string PARENT("Parent");
const std::string LOCATION("Location");
const std::string SUBSYSTEM("Subsystem");

const TA_Core::EObjectType OBJECT_TYPE = TA_Core::ENTITY;

using TA_Core::DebugUtil;
using TA_Core::OnlineUpdateListener;

namespace TA_App
{
    //
	// DataPointOnlineUpdateSubscriber
	//
	DataPointOnlineUpdateSubscriber::DataPointOnlineUpdateSubscriber(CHistoryViewerDlg* parent)
		: m_parentProcess(parent),
          m_interestRegisterThread(NULL)
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "DataPointOnlineUpdateSubscriber" );

        TA_ASSERT(parent != NULL, "Null pointer");
    
        //
        // It takes a little while to register for online updates for all datapoints, so 
        // do this in another thread to speed up initialisation and display of the main dialog
        //
	    m_interestRegisterThread = new DataPointInterestRegisterThread(this);
        m_interestRegisterThread->start();

		LOG ( SourceInfo, DebugUtil::FunctionExit, "DataPointOnlineUpdateSubscriber" );
	}


	//
	// ~DataPointOnlineUpdateSubscriber
	//
	DataPointOnlineUpdateSubscriber::~DataPointOnlineUpdateSubscriber()
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "~DataPointOnlineUpdateSubscriber" );

        if(m_interestRegisterThread != NULL)
        {
            m_interestRegisterThread->terminateAndWait();
            delete m_interestRegisterThread;
            m_interestRegisterThread = NULL;
        }

        OnlineUpdateListener::getInstance().deregisterAllInterests(OBJECT_TYPE, *this);

		LOG ( SourceInfo, DebugUtil::FunctionExit, "~DataPointOnlineUpdateSubscriber" );
	}


    //
	// addInterestedKey
	//
    void DataPointOnlineUpdateSubscriber::addInterestedKey(unsigned int entitykey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "addInterestedKey" );

        OnlineUpdateListener::getInstance().registerInterest(OBJECT_TYPE, *this, entitykey);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "addInterestedKey" );
    }


    //
	// removeInterestedKey
	//
    void DataPointOnlineUpdateSubscriber::removeInterestedKey(unsigned int entitykey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "removeInterestedKey" );

        OnlineUpdateListener::getInstance().deregisterInterest(OBJECT_TYPE, *this, entitykey);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "removeInterestedKey" );
    }


    //
	// receiveSpecialisedMessage
	//
    void DataPointOnlineUpdateSubscriber::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

        switch(updateEvent.getModifications())
        {
            case TA_Core::Update:   handleUpdateEvent(updateEvent);
                                    break;

            case TA_Core::Create:   handleCreateEvent(updateEvent);
                                    break;

            case TA_Core::Delete:   handleDeleteEvent(updateEvent);
                                    break;
        
            default:                // Shouldn't go here
                                    break;
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
    }


    //
	// handleUpdateEvent
	//
    void DataPointOnlineUpdateSubscriber::handleUpdateEvent(const ConfigUpdateDetails& updateEvent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "handleUpdateEvent" );

        const std::vector< std::string> params = updateEvent.getChangedParams();

        //
        // The only parameter we are interested in is 'EnableTrending'. If it's anything else,
        // just ignore it.
        //
        std::vector<std::string>::const_iterator it = params.begin();
        for(; it != params.end(); ++it)
        {
            if(*it == TRENDING_ENABLED_ENTITY_PARAM || *it == ENTITY_NAME ||
                *it == DESCRIPTION || *it == DATAPOINT_TYPE_ENTITY_PARAM ||
                *it == ENG_UNITS_ENTITY_PARAM || *it == ENG_LIMIT_LOW_ENTITY_PARAM || 
                *it == ENG_LIMIT_HIGH_ENTITY_PARAM ||
                *it == ADDRESS || *it == PARENT || *it == LOCATION || *it == SUBSYSTEM)
            {
                break;
            }
        }

        if(it != params.end())
        {
            unsigned long entitykey = updateEvent.getKey();
            IEntityData* entityData = TA_Core::EntityAccessFactory::getInstance().getEntity(entitykey);
            m_parentProcess->processUpdateCfgEvent(entityData);
            
            if(entityData != NULL)
            {
                delete entityData;
            }
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "handleUpdateEvent" );
    }


    //
	// handleCreateEvent
	//
    void DataPointOnlineUpdateSubscriber::handleCreateEvent(const ConfigUpdateDetails& updateEvent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "handleCreateEvent" );

        //
        // Check if it is a new datapoint entity. If it isn't, just ignore it.
        //
        unsigned long entitykey = updateEvent.getKey();
        IEntityData* entityData = TA_Core::EntityAccessFactory::getInstance().getEntity(entitykey);
        if(entityData->getType() != DATAPOINT_TYPENAME)
        {
            if(entityData != NULL)
            {
                delete entityData;
            }
            return;
        }

        m_parentProcess->processCreateCfgEvent(entitykey, entityData);

        if(entityData != NULL)
        {
            delete entityData;
        }

        //
        // We're interested in update events for this datapoint entity, so add the key
        //
        addInterestedKey(entitykey);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "handleCreateEvent" );
    }


    //
	// handleDeleteEvent
	//
    void DataPointOnlineUpdateSubscriber::handleDeleteEvent(const ConfigUpdateDetails& updateEvent)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "handleDeleteEvent" );

        m_parentProcess->processDeleteCfgEvent(updateEvent.getKey());

        //
        // The entity has been deleted so we're no longer interested in this key
        //
        removeInterestedKey(updateEvent.getKey());

        LOG ( SourceInfo, DebugUtil::FunctionExit, "handleDeleteEvent" );
    }


    //
	// run
	//
    void DataPointInterestRegisterThread::run()
    {
        FUNCTION_ENTRY("run");

        //
		// Create the message subscriber and subscribe the subscriber with the 
        // MessageSubscriptionManager by message context
		//
        std::vector<IEntityData*> dataPointData = TA_Core::EntityAccessFactory::getInstance().getEntitiesOfType(DATAPOINT_TYPENAME);
        std::vector<IEntityData*>::iterator it = dataPointData.begin();
        for(; (it != dataPointData.end() && !m_terminated); ++it)
        {
            //OnlineUpdateListener::getInstance().addKey(OBJECT_TYPE, (*it)->getKey());
            OnlineUpdateListener::getInstance().registerInterest(OBJECT_TYPE, *m_parentProcess, (*it)->getKey());

            if((*it) != NULL)
            {
                delete (*it);
            }
        }

        m_terminated = true;

        FUNCTION_EXIT;
    }


    //
	// terminate
	//
    void DataPointInterestRegisterThread::terminate()
    {
        FUNCTION_ENTRY("terminate");

        if(!m_terminated)
        {
            m_terminated = true;
        }

        FUNCTION_EXIT;
    }

} // TA_App

#endif // !defined(DATAPOINT_ONLINE_UPDATE_SUBSCRIBER_CPP)
