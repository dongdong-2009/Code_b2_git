/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/DatapointOnlineUpdateSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "app/history/scada_historian/src/ScadaHistorianAgentProcess.h"
#include "app/history/scada_historian/src/DatapointOnlineUpdateSubscriber.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "app/history/scada_historian/src/DatapointLoaderThread.h"


using TA_Core::DebugUtil;
using TA_Core::OnlineUpdateListener;

const TA_Core::EObjectType OBJECT_TYPE = TA_Core::ENTITY;

const std::string TRENDING_ENABLED_ENTITY_PARAM("EnableTrending");
const std::string SUBSYSTEM("Subsystem");
const std::string NAME("Name");


namespace TA_App
{
    //
	// DataPointOnlineUpdateSubscriber
	//
	DataPointOnlineUpdateSubscriber::DataPointOnlineUpdateSubscriber(ScadaHistorianAgentProcess* parent)
		: m_parentProcess(parent) 
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "DataPointOnlineUpdateSubscriber" );

        TA_ASSERT(parent != NULL, "Null pointer");

		//
		// Create the message subscriber and subscribe the subscriber with the 
        // MessageSubscriptionManager by message context
		//
        std::vector<IEntityData*> dataPointData = TA_Core::EntityAccessFactory::getInstance().getEntitiesOfType(DATAPOINT_TYPENAME);
        std::vector<IEntityData*>::iterator it = dataPointData.begin();
        for(; it != dataPointData.end(); ++it)
        {
            //OnlineUpdateListener::getInstance().addKey(OBJECT_TYPE, (*it)->getKey());
            OnlineUpdateListener::getInstance().registerInterest(OBJECT_TYPE, *this, (*it)->getKey());
        }

		LOG ( SourceInfo, DebugUtil::FunctionExit, "DataPointOnlineUpdateSubscriber" );
	}


	//
	// ~DataPointOnlineUpdateSubscriber
	//
	DataPointOnlineUpdateSubscriber::~DataPointOnlineUpdateSubscriber()
	{
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "~DataPointOnlineUpdateSubscriber" );

		OnlineUpdateListener::getInstance().deregisterAllInterests(OBJECT_TYPE, *this);

		LOG ( SourceInfo, DebugUtil::FunctionExit, "~DataPointOnlineUpdateSubscriber" );
	}


    void DataPointOnlineUpdateSubscriber::addInterestedKey(unsigned int entitykey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "addInterestedKey" );

        //OnlineUpdateListener::getInstance().addKey(OBJECT_TYPE, entitykey);
        OnlineUpdateListener::getInstance().registerInterest(OBJECT_TYPE, *this, entitykey);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "addInterestedKey" );
    }


    void DataPointOnlineUpdateSubscriber::removeInterestedKey(unsigned int entitykey)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "removeInterestedKey" );

        //OnlineUpdateListener::getInstance().removeKey(OBJECT_TYPE, entitykey);
        OnlineUpdateListener::getInstance().deregisterInterest(OBJECT_TYPE, *this, entitykey);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "removeInterestedKey" );
    }


    void DataPointOnlineUpdateSubscriber::removeAllInterestedKeys()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "removeAllInterestedKeys" );

		OnlineUpdateListener::getInstance().deregisterAllInterests(OBJECT_TYPE, *this);

		LOG ( SourceInfo, DebugUtil::FunctionExit, "removeAllInterestedKeys" );
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
        // The only parameters we are interested in are 'EnableTrending', the name,
        // and subsystem of the entity. If it's anything else, just ignore it.
        //
        std::vector<std::string>::const_iterator it = params.begin();
        for(; it != params.end(); ++it)
        {
            if(*it == TRENDING_ENABLED_ENTITY_PARAM ||
                *it == SUBSYSTEM ||
                *it == NAME)
            {
                break;
            }
        }

        if(it != params.end())
        {
            m_parentProcess->processUpdateCfgEvent(updateEvent.getKey());       
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
            return;
        }

        m_parentProcess->processCreateCfgEvent(entitykey, entityData);

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

} // TA_App


