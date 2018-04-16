/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/DisplayItemManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #10 $
  *
  * Last modification: $DateTime: 2013/11/18 21:38:42 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This singleton is required to keep a reference to all display
  * items. It maps each type of display item by name and provides access to
  * the items that are shared throughout the GUI - their state is updated
  * via comms messages and the display refreshed to reflect this.
  */

#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/DisplayItemTasks.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDAIImpl.h"
#include "app/cctv/video_display_manager/src/Configuration.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/data_access_interface/entity_access/src/Quad.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"
#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/threads/src/ThreadedWorker.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include <algorithm>
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <boost/lexical_cast.hpp>


using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::EntityAccessFactory;

using TA_IRS_App::SwitchAgentCommunicator;
typedef TA_IRS_App::SwitchAgentCommunicator::CamToNamedObject CamToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::QuadToNamedObject QuadToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::SequenceToNamedObject SequenceToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::MonitorToNamedObject MonitorToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::OutputToNamedObject OutputToNamedObject;

using CORBA::String_var;


namespace TA_IRS_App
{

static const std::string OCC_WALL_MONITOR = "WALL";
static const std::string OCC_ODS_MONITOR = "ODS";

DisplayItemManager* DisplayItemManager::m_theInstance = NULL;
TA_Base_Core::ReEntrantThreadLockable DisplayItemManager::m_singletonLock;

typedef TA_Base_Core::IEntityData* EntityDataPtr;
DisplayItemManager& DisplayItemManager::getInstance()
{
    ThreadGuard guard(m_singletonLock);
    if (m_theInstance == NULL)
    {
        m_theInstance = new DisplayItemManager();
    }
    return *m_theInstance;
}


void DisplayItemManager::removeInstance()
{
    ThreadGuard guard(m_singletonLock);
    if (m_theInstance != NULL)
    {
        delete m_theInstance;
        m_theInstance = NULL;
    }
}


DisplayItemManager::DisplayItemManager() :
        m_cameras(),
        m_camerasByPkey(),
        m_camerasByAddress(),
        m_quads(),
        m_quadsByPkey(),
        m_sequences(),
        m_sequencesByPkey(),
        m_monitors(),
        m_monitorsByPkey(),
        m_isConfigItems(false),
        m_callbackWindow(NULL),
        m_imosSingleMonitor(NULL)
{
    m_imosQuadMonitors.fill(NULL);
    m_allMonitorsArray.fill(NULL);
    m_videoDisplayManagerDAI = new VideoDisplayManagerDAIImpl();
}


DisplayItemManager::~DisplayItemManager()
{
    terminateAndWait();
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    deleteAllItems();
}


void DisplayItemManager::initialiseDisplayItems(CWnd* callbackWindow)
{
    m_callbackWindow = callbackWindow;
    start();
}


void DisplayItemManager::run()
{
    if (m_isConfigItems)
    {
        return;
    }
    // Determine the local agent for the physical Location of this Console Session.
    // just in case
    deleteAllItems();
    if ((0 == m_cameras.size()) &&
            (0 == m_quads.size()) &&
            (0 == m_sequences.size()) &&
            (0 == m_monitors.size()))
    {
        loadItemsFromDatabase();
    }
    LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm,
                "Loaded %d Cameras, %d Quads, %d Sequences, %d Monitors",// %d Stages,
                m_cameras.size(),
                m_quads.size(),
                m_sequences.size(),
                m_monitors.size());
    // subscribes for comms messages and online updates,
    subscribeToMessages();
    m_isConfigItems = true;
    m_callbackWindow->PostMessage(TA_INIT_CCTV);
}


void DisplayItemManager::terminate()
{
}



void DisplayItemManager::loadItemsFromDatabase()
{
    // everything here can be done in parallel
    // first load cameras from database
    loadCamerasFromDatabase();
    TA_Base_Core::ThreadedWorker quadWorker(*this);
    quadWorker.start();
    TA_Base_Core::ThreadedWorker sequenceWorker(*this);
    sequenceWorker.start();
    TA_Base_Core::ThreadedWorker stageWorker(*this);
    stageWorker.start();
    TA_Base_Core::ThreadedWorker monitorWorker(*this);
    monitorWorker.start();
    TA_Base_Core::ThreadedWorker stationWorker(*this);
    stationWorker.start();
    // create the barrier
    TA_Base_Core::SingleThreadBarrier barrier(5);
    TA_Base_Core::IWorkItemPtr quadTask(new LoadQuadTask(*this, barrier));
    quadWorker.executeWorkItem(quadTask);
    TA_Base_Core::IWorkItemPtr sequenceTask(new LoadSequenceTask(*this, barrier));
    sequenceWorker.executeWorkItem(sequenceTask);
    TA_Base_Core::IWorkItemPtr stageTask(new LoadStageTask(*this, barrier));
    stageWorker.executeWorkItem(stageTask);
    TA_Base_Core::IWorkItemPtr monitorTask(new LoadMonitorTask(*this, barrier));
    monitorWorker.executeWorkItem(monitorTask);
    TA_Base_Core::IWorkItemPtr stationTask(new LoadStationTask(*this, barrier));
    stationWorker.executeWorkItem(stationTask);
    // wait on the barrier
    barrier.wait();
    // stop all the threads
    quadWorker.terminateAndWait();
    sequenceWorker.terminateAndWait();
    stageWorker.terminateAndWait();
    monitorWorker.terminateAndWait();
    stationWorker.terminateAndWait();
    LoadSequenceRelationFromDatabase();
}


bool CompareEntity(const EntityDataPtr& entity1, const EntityDataPtr& entity2)
{
    return (entity1->getKey() < entity2->getKey());
}



/*
void DisplayItemManager::loadMonitorsFromAgent()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Initialising monitor state");
    time_t startTime = time(NULL);
    try
    {
        MonitorToNamedObject consoleMonitors = SwitchAgentCommunicator::getInstance().getConsoleMonitorObjects();
        for(MonitorToNamedObject::iterator namedObjectIter = consoleMonitors.begin();
                consoleMonitors.end() != namedObjectIter; ++namedObjectIter)
        {
            unsigned long key;
            CORBA_CALL_RETURN(key, (* (namedObjectIter->second)), getKey, ());        //limin
            std::string name;
            CORBA_CALL_RETURN(name, (* (namedObjectIter->second)), getName, ());        //limin
            std::string desc;
            CORBA_CALL_RETURN(desc, (* (namedObjectIter->second)), getDescription, ());        //limin
            Monitor* newMonitor = createMonitor(key, name, desc, true, 0, 0, "");
            TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState_var outputState;
            CORBA_CALL_RETURN(outputState, (* (namedObjectIter->second)), getVideoOutputState, ());
            if(outputState->isInAlarmMode)
            {
                // get the alarm stack
                newMonitor->setAlarmStackItems(outputState->alarmStack);
            }
            else
            {
                // get the input
                // can be a sequence, camera, quad etc
                newMonitor->assignInput(getInput(outputState->currentVideoInputKey));
            }
        }
    }
    catch(TransactiveException& te)
    {
        // object resolution etc
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what());
    }
    catch(const CORBA::Exception& cex)
    {
        std::string exceptionMsg("Exception thrown while loading quads: ");
        exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
    }
    // go through the common monitors
    try
    {
        MonitorToNamedObject commonMonitors = SwitchAgentCommunicator::getInstance().getCommonMonitorObjects();
        for(MonitorToNamedObject::iterator namedObjectIter = commonMonitors.begin();
                commonMonitors.end() != namedObjectIter; ++namedObjectIter)
        {
            unsigned long key;
            CORBA_CALL_RETURN(key, (* (namedObjectIter->second)), getKey, ());        //limin
            std::string name;
            CORBA_CALL_RETURN(name, (* (namedObjectIter->second)), getName, ());        //limin
            std::string desc;
            CORBA_CALL_RETURN(desc, (* (namedObjectIter->second)), getDescription, ());        //limin
            Monitor* newMonitor = createMonitor(key, name, desc, false, 0, 0, "");
            TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState_var outputState;
            CORBA_CALL_RETURN(outputState, (* (namedObjectIter->second)), getVideoOutputState, ());        //limin
            if(outputState->isInAlarmMode)
            {
                // get the alarm stack
                newMonitor->setAlarmStackItems(outputState->alarmStack);
            }
            else
            {
                // get the input
                // can be a sequence, camera, quad etc
                newMonitor->assignInput(getInput(outputState->currentVideoInputKey));
            }
        }
        m_callbackWindow->PostMessage(TA_INIT_MONITORS);
    }
    catch(TransactiveException& te)
    {
        // object resolution etc
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what());
    }
    catch(const CORBA::Exception& cex)
    {
        std::string exceptionMsg("Exception thrown while loading quads: ");
        exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
        LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
    }
    time_t elapsedTime = time(NULL) - startTime;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Finished initialising monitor state. Elapsed time %d seconds",
                elapsedTime);
}
*/



void DisplayItemManager::loadCamerasFromDatabase()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading cameras");
    time_t startTime = time(NULL);
	TA_Base_Core::ILocation::ELocationType localLocationType = CCTVConfiguration::getinstance().getLocationType();
	unsigned long localLocationKey = CCTVConfiguration::getinstance().getLocationKey();
    try
    {
        if (TA_Base_Core::ILocation::OCC == localLocationType)
        {
            std::vector<TA_Base_Core::ILocation*> locations = m_videoDisplayManagerDAI->getAllLocations();
            for (std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin(); it != locations.end(); it++)
            {
                TA_Base_Core::ILocation* loc = dynamic_cast<TA_Base_Core::ILocation*>(*it);
                const TA_Base_Core::ILocation::ELocationType locationType = loc->getLocationType();
                if (TA_Base_Core::ILocation::DPT == locationType)
                {
                    continue;
                }
                unsigned long locationKey = loc->getKey();
                IEntityDataList cameraEntities = m_videoDisplayManagerDAI->getCameras(locationKey);
                for (IEntityDataList::iterator it = cameraEntities.begin(); it != cameraEntities.end(); it++)
                {
                    std::auto_ptr<TA_Base_Core::Camera> cameraEntity(dynamic_cast<TA_Base_Core::Camera*>(*it));
                    // get all details
                    if (NULL != cameraEntity.get())
                    {
                        unsigned long key = cameraEntity->getKey();
                        std::string name = cameraEntity->getName();
                        std::string desc = cameraEntity->getDescription();
                        std::string group = cameraEntity->getVideoInputGroupName();
                        std::string address = cameraEntity->getAddress();
                        unsigned long locationid = cameraEntity->getLocation();
                        bool isMovementSupported = cameraEntity->isMovementSupported();
                        createCamera(key, name, desc, group, address , locationid, isMovementSupported);
                    }
                }
            }
        }
        else
        {
            IEntityDataList cameraEntities = m_videoDisplayManagerDAI->getCameras(localLocationKey);
            std::sort(cameraEntities.begin(), cameraEntities.end(), TA_IRS_App::CompareEntity);
            for (IEntityDataList::iterator it = cameraEntities.begin(); it != cameraEntities.end(); it++)
            {
                std::auto_ptr<TA_Base_Core::Camera> cameraEntity(dynamic_cast<TA_Base_Core::Camera*>(*it));
                // get all details
                if (NULL != cameraEntity.get())
                {
                    unsigned long key = cameraEntity->getKey();
                    std::string name = cameraEntity->getName();
                    std::string desc = cameraEntity->getDescription();
                    std::string group = cameraEntity->getVideoInputGroupName();
                    std::string address = cameraEntity->getAddress();
                    unsigned long locationid = cameraEntity->getLocation();
                    bool isMovementSupported = cameraEntity->isMovementSupported();
                    createCamera(key, name, desc, group, address , locationid, isMovementSupported);
                }
            }
        }
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        // object resolution etc
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
    }
    time_t elapsedTime = time(NULL) - startTime;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading cameras. Elapsed time %d seconds", elapsedTime);
    m_callbackWindow->PostMessage(TA_INIT_CAMERAS);
}


void DisplayItemManager::loadQuadsFromDatabase()
{
}


void DisplayItemManager::loadSequencesFromDatabase()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading sequences");
    time_t startTime = time(NULL);
	TA_Base_Core::ILocation::ELocationType localLocationType = CCTVConfiguration::getinstance().getLocationType();
	unsigned long localLocationKey = CCTVConfiguration::getinstance().getLocationKey();
    try
    {
        IEntityDataList sequenceEntities;
        if (TA_Base_Core::ILocation::OCC == localLocationType)
        {
            std::vector<TA_Base_Core::ILocation*> locations = m_videoDisplayManagerDAI->getAllLocations();
            for (std::vector<TA_Base_Core::ILocation*>::iterator it = locations.begin(); it != locations.end(); it++)
            {
                TA_Base_Core::ILocation* loc = dynamic_cast<TA_Base_Core::ILocation*>(*it);
                const TA_Base_Core::ILocation::ELocationType locType = loc->getLocationType();
                if (TA_Base_Core::ILocation::DPT == locType)
                {
                    continue;
                }
                unsigned long locationKey = loc->getKey();
                // sequenceEntities = m_videoDisplayManagerDAI->getSequences( getLocationKey() );
                // sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), m_locationkey);
                // sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::Sequence::getStaticType());
                // sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), locationkey);
                sequenceEntities = m_videoDisplayManagerDAI->getSequences(locationKey);
                std::sort(sequenceEntities.begin(), sequenceEntities.end(), TA_IRS_App::CompareEntity);
                for (IEntityDataList::iterator it = sequenceEntities.begin(); it != sequenceEntities.end(); it++)
                {
                    std::auto_ptr<TA_Base_Core::Sequence> sequenceEntity(dynamic_cast<TA_Base_Core::Sequence*>(*it));
                    // get all details
                    unsigned long key = sequenceEntity->getKey();
                    std::string name = sequenceEntity->getName();
                    std::string addr = sequenceEntity->getAddress();
                    std::string desc = sequenceEntity->getDescription();
                    std::string group = sequenceEntity->getVideoInputGroupName();
                    bool readOnly = sequenceEntity->isReadOnly();
                    std::string sequenceDescription = sequenceEntity->getSequenceDescription();
                    unsigned long locationId = sequenceEntity->getLocation();
                    createSequence(key, name, addr, desc, group, readOnly, sequenceDescription , locationId);
                }
            }
        }
        else
        {
            sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), localLocationKey);
            std::sort(sequenceEntities.begin(), sequenceEntities.end(), TA_IRS_App::CompareEntity);
            for (IEntityDataList::iterator it = sequenceEntities.begin(); it != sequenceEntities.end(); it++)
            {
                std::auto_ptr<TA_Base_Core::Sequence> sequenceEntity(dynamic_cast<TA_Base_Core::Sequence*>(*it));
                // get all details
                unsigned long key = sequenceEntity->getKey();
                std::string name = sequenceEntity->getName();
                std::string addr = sequenceEntity->getAddress();
                std::string desc = sequenceEntity->getDescription();
                std::string group = sequenceEntity->getVideoInputGroupName();
                bool readOnly = sequenceEntity->isReadOnly();
                std::string sequenceDescription = sequenceEntity->getSequenceDescription();
                unsigned long locationId = sequenceEntity->getLocation();
                createSequence(key, name, addr, desc, group, readOnly, sequenceDescription , locationId);
            }
        }
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        // object resolution etc
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
    }
    time_t elapsedTime = time(NULL) - startTime;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading sequences. Elapsed time %d seconds", elapsedTime);
    m_callbackWindow->PostMessage(TA_INIT_SEQUENCES);
}

void DisplayItemManager::saveSequencesRelationship(unsigned long parentKey)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Saving sequences");
    time_t startTime = time(NULL);
    try
    {
        std::list<Camera*> cameras = m_sequencesByPkey[parentKey]->getCameras();
        unsigned short dwellTime = m_sequencesByPkey[parentKey]->getDwellTime();
        std::string description = m_sequencesByPkey[parentKey]->getSequenceDescription();
        std::vector<unsigned long> cameraKeys;
        for (std::list<Camera*>::iterator it = cameras.begin(); it != cameras.end(); it++)
        {
            Camera* cam = (dynamic_cast<Camera*>(*it));
            cameraKeys.push_back(cam->getKey());
        }
        TA_IRS_Core::SequenceHelper sequenceHelper;
        sequenceHelper.setSequenceRelationShip(parentKey, dwellTime, description, cameraKeys);
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        // object resolution etc
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
    }
    time_t elapsedTime = time(NULL) - startTime;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading sequences. Elapsed time %d seconds", elapsedTime);
}

void DisplayItemManager::loadMonitorsFromDatabase()
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading monitors");
    time_t startTime = time(NULL);
	unsigned long localLocationKey = CCTVConfiguration::getinstance().getLocationKey();
    try
    {
        IEntityDataList monitorEntities =
            EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), localLocationKey);
        std::sort(monitorEntities.begin(), monitorEntities.end(), TA_IRS_App::CompareEntity);
		unsigned long localConsoleKey = CCTVConfiguration::getinstance().getConsoleKey();
        for (IEntityDataList::iterator it = monitorEntities.begin(); it != monitorEntities.end(); it++)
        {
            std::auto_ptr<TA_Base_Core::VideoMonitor> monitorEntity(dynamic_cast<TA_Base_Core::VideoMonitor*>(*it));
            // get all details
            unsigned long key = monitorEntity->getKey();
            unsigned long locationKey = monitorEntity->getLocation();
            std::string name = monitorEntity->getName();
            std::string desc = monitorEntity->getDescription();
            unsigned long consoleKey = monitorEntity->getConsoleKey();
            int displayOrder = monitorEntity->getDisplayOrder();
            std::string address = monitorEntity->getAddress();
			const bool isConsoleMonitor = (consoleKey == localConsoleKey && monitorEntity->getDisplayOrder() == 0);

            if (consoleKey != 0 && consoleKey != localConsoleKey) // Monitors belong to other MTFs
            {
                continue;
            }

			if(displayOrder == 0 && !isConsoleMonitor) // Configure error
			{
				continue;
			}

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Monitor being created, name=%s, entityKey=%lu, consoleKey=%lu, displayOrder=%d",
                        name.c_str(), key, consoleKey, displayOrder);

            Monitor* monitor = createMonitor(key, name, desc, isConsoleMonitor, consoleKey, displayOrder, address);
            m_allMonitorsArray[displayOrder] = monitor;

            if (isConsoleMonitor)
            {
                m_imosSingleMonitor = monitor;
            }

			if(displayOrder >= 1 && displayOrder <= 4)
			{
				m_imosQuadMonitors[displayOrder -1] = monitor;
			}
        }
    }
    catch (const TA_Base_Core::DatabaseException& e)
    {
        // object resolution etc
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
    }
    catch (const TA_Base_Core::DataException& e)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
    }
    time_t elapsedTime = time(NULL) - startTime;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Finished loading monitors. Elapsed time %d seconds",
                elapsedTime);
    m_callbackWindow->PostMessage(TA_INIT_MONITORS);
}


Camera* DisplayItemManager::createCamera(unsigned long key,
                                         const std::string& name,
                                         const std::string& description,
                                         const std::string& group,
                                         const std::string& address, unsigned long locationId, bool isMovementSupported)
{
    // create the local object
    Camera* camPtr = new Camera(key, name, description, address, group , locationId, isMovementSupported);
    // add it to the maps
    m_cameras[name] = camPtr;
    m_camerasByPkey[key] = camPtr;
    m_camerasByAddress[address] = camPtr;
    return camPtr;
}


Quad* DisplayItemManager::createQuad(unsigned long key,
                                     const std::string& name,
                                     const std::string& description,
                                     const std::string& group, unsigned long locationId)
{
    // create the local object
    Quad* quadPtr = new Quad(key, name, description, group, locationId);
    // add it to the maps
    m_quads[name] = quadPtr;
    m_quadsByPkey[key] = quadPtr;
    return quadPtr;
}


Sequence* DisplayItemManager::createSequence(unsigned long key,
                                             const std::string& name,
                                             const std::string& addr,
                                             const std::string& description,
                                             const std::string& group,
                                             bool readOnly,
                                             const std::string& sequenceDescription,
                                             unsigned long locationId)
{
    // create the local object
    Sequence* seqPtr = new Sequence(key, name, addr, description, group , locationId);
    seqPtr->setReadOnly(readOnly);
    seqPtr->setSequenceDescription(sequenceDescription);
    // add it to the maps
    m_sequences[name] = seqPtr;
    m_sequencesByPkey[key] = seqPtr;
    m_seqVec.push_back(seqPtr);
    return seqPtr;
}

void DisplayItemManager::LoadSequenceRelationFromDatabase()
{
    TA_IRS_Core::SequenceHelper sequenceHelper;
    unsigned short dwellTime;
    for (std::vector <Sequence *>::iterator it = m_seqVec.begin(); it != m_seqVec.end(); it++)
    {
        dwellTime = 0;
        Sequence * seqPtr = dynamic_cast<Sequence*>(*it);
        std::list<Camera*> cameras;
        std::vector<unsigned long> videoInputKeys;
        std::string description;
        sequenceHelper.getSequenceRelationShip(seqPtr->getKey() , dwellTime, description, videoInputKeys);
        for (unsigned int i = 0; i < videoInputKeys.size(); i++)
        {
            // get the camera by the key
            Camera* cam = getCamera(videoInputKeys[i]);
            if (cam != NULL)
            {
                cameras.push_back(cam);
            }
        }
        seqPtr->setDwellTime(dwellTime);
        seqPtr->setCameras(cameras);
        if (description != "")
        {
            //seqPtr->setDescription(description);
            seqPtr->setSequenceDescription(description);
        }
        m_sequences[seqPtr->getName()] = seqPtr;
        m_sequencesByPkey[seqPtr->getKey()] = seqPtr;
    }
}

void DisplayItemManager::loadEntityRelationship(unsigned long parentKey, unsigned long& pkey, int* ptype)
{
    TA_IRS_Core::SequenceHelper sequenceHelper;
    sequenceHelper.getRelationShipFromParentKey(parentKey, pkey, std::string(), ptype);
}

void DisplayItemManager::saveEntityRelationship(unsigned long parentKey, unsigned long pkey, int type)
{
    TA_IRS_Core::SequenceHelper sequenceHelper;
    sequenceHelper.setRelationShipOfParentKey(parentKey, pkey, "", type);
}

void DisplayItemManager::delEntityRelationship(unsigned long parentKey)
{
    TA_IRS_Core::SequenceHelper sequenceHelper;
    sequenceHelper.delRelationShipOfParentKey(parentKey);
}


Monitor* DisplayItemManager::createMonitor(unsigned long key,
                                           const std::string& name,
                                           const std::string& description,
                                           bool isConsoleMonitor,
                                           unsigned long consoleKey,
                                           int order,
                                           const std::string& address)
{
    // create the local object
    Monitor* monPtr = new Monitor(key, name, description, isConsoleMonitor,
                                  consoleKey, order, address);
    m_monitors[ name ] = monPtr;
    m_monitorsByPkey[ key ] = monPtr;
    if (isConsoleMonitor)
    {
        m_consoleMonitors.push_back(monPtr->getName());
    }
    return monPtr;
}


void DisplayItemManager::subscribeToMessages()
{
    unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::VideoOutputStateUpdate,
            this, 0, 0, locationKey, NULL);
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::QuadStateUpdate,
            this, 0, 0, locationKey, NULL);
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::SequenceConfigUpdate,
            this, 0, 0, locationKey, NULL);
    //     TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::VisualComms::BvsStageStateUpdate,
    //                                                                                this, 0, 0, locationKey, NULL);
    // No subscriptions for TA_Base_Core::VisualComms::RecordingUnitStateUpdate
    // there are no more recording units here
    // online update messages for sequence descriptions
    std::vector< unsigned long > sequenceKeys;
    for (std::map<std::string, Sequence*>::iterator iter = m_sequences.begin();
            iter != m_sequences.end(); iter++)
    {
        sequenceKeys.push_back(iter->second->getKey());
    }
    TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests(TA_Base_Core::ENTITY, *this, sequenceKeys);
}


Camera* DisplayItemManager::getCamera(std::string name)
{
    // find and return the item in the map
    std::map<std::string, Camera*>::iterator findIter = m_cameras.find(name);
    if (findIter != m_cameras.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Camera* DisplayItemManager::getCamera(unsigned long pkey)
{
    // find and return the item in the map
    std::map<unsigned long, Camera*>::iterator findIter = m_camerasByPkey.find(pkey);
    if (findIter != m_camerasByPkey.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Camera* DisplayItemManager::getCameraByAddress(const std::string& address)
{
    // find and return the item in the map
    std::map<std::string, Camera*>::iterator findIter = m_camerasByAddress.find(address);
    if (findIter != m_camerasByAddress.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Quad* DisplayItemManager::getQuad(std::string name)
{
    // find and return the item in the map
    std::map<std::string, Quad*>::iterator findIter = m_quads.find(name);
    if (findIter != m_quads.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Quad* DisplayItemManager::getQuad(unsigned long pkey)
{
    // find and return the item in the map
    std::map<unsigned long, Quad*>::iterator findIter = m_quadsByPkey.find(pkey);
    if (findIter != m_quadsByPkey.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Sequence* DisplayItemManager::getSequence(std::string name)
{
    // find and return the item in the map
    std::map<std::string, Sequence*>::iterator findIter = m_sequences.find(name);
    if (findIter != m_sequences.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Sequence* DisplayItemManager::getSequence(unsigned long pkey)
{
    // find and return the item in the map
    std::map<unsigned long, Sequence*>::iterator findIter = m_sequencesByPkey.find(pkey);
    if (findIter != m_sequencesByPkey.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}

VideoInput* DisplayItemManager::getInput(std::string name)
{
    VideoInput* input = getCamera(name);
    if (input != NULL)
    {
        return input;
    }
    input = getQuad(name);
    if (input != NULL)
    {
        return input;
    }
    input = getSequence(name);
    if (input != NULL)
    {
        return input;
    }
    //     input = getBvsStage(name);
    return input;
}


VideoInput* DisplayItemManager::getInput(unsigned long pkey)
{
    VideoInput* input = getCamera(pkey);
    if (input != NULL)
    {
        return input;
    }
    input = getQuad(pkey);
    if (input != NULL)
    {
        return input;
    }
    input = getSequence(pkey);
    if (input != NULL)
    {
        return input;
    }
    //     input = getBvsStage(pkey);
    return input;
}


VideoOutput* DisplayItemManager::getOutput(std::string name)
{
    return getMonitor(name);
}


VideoOutput* DisplayItemManager::getOutput(unsigned long pkey)
{
    return getMonitor(pkey);
}


Monitor* DisplayItemManager::getMonitor(std::string name)
{
    // find and return the item in the map
    std::map<std::string, Monitor*>::iterator findIter = m_monitors.find(name);
    if (findIter != m_monitors.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}


Monitor* DisplayItemManager::getMonitor(unsigned long pkey)
{
    // find and return the item in the map
    std::map<unsigned long, Monitor*>::iterator findIter = m_monitorsByPkey.find(pkey);
    if (findIter != m_monitorsByPkey.end())
    {
        return (findIter->second);
    }
    // not found
    return NULL;
}

std::vector<std::string> DisplayItemManager::getConsoleMonitors()  //TD 19471 lichao++
{
    return m_consoleMonitors;
}

std::map<std::string, Camera*> DisplayItemManager::getAllCameras()
{
    return m_cameras;
}


std::map<std::string, Quad*> DisplayItemManager::getAllQuads()
{
    return m_quads;
}


std::map<std::string, Sequence*> DisplayItemManager::getAllSequences()
{
    return m_sequences;
}


/*std::map<std::string, BvsStage*> DisplayItemManager::getAllStages()
{
    return m_bvsStages;
}*/


std::map<std::string, Monitor*> DisplayItemManager::getAllMonitors()
{
    return m_monitors;
}


void DisplayItemManager::deleteAllItems()
{
    for (std::map<std::string, Monitor*>::iterator monIter = m_monitors.begin();
            monIter != m_monitors.end(); monIter++)
    {
        delete monIter->second;
        monIter->second = NULL;
    }
    m_monitors.clear();
    m_monitorsByPkey.clear();
    for (std::map<std::string, Quad*>::iterator quadIter = m_quads.begin();
            quadIter != m_quads.end(); quadIter++)
    {
        delete quadIter->second;
        quadIter->second = NULL;
    }
    m_quads.clear();
    m_quadsByPkey.clear();
    for (std::map<std::string, Sequence*>::iterator seqIter = m_sequences.begin();
            seqIter != m_sequences.end(); seqIter++)
    {
        delete seqIter->second;
        seqIter->second = NULL;
    }
    m_sequences.clear();
    m_sequencesByPkey.clear();
    for (std::map<std::string, Camera*>::iterator camIter = m_cameras.begin();
            camIter != m_cameras.end(); camIter++)
    {
        delete camIter->second;
        camIter->second = NULL;
    }
    m_cameras.clear();
    m_camerasByPkey.clear();
    m_camerasByAddress.clear();
}


void DisplayItemManager::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    // we only register for updates to sequences
    Sequence* sequence = getSequence(updateEvent.getKey());
    if (sequence != NULL)
    {
        // the only thing to bother updating is the description
        // online adding/deleting of sequences is not supported
        if (updateEvent.getModifications() == TA_Base_Core::Update)
        {
            SwitchAgentCommunicator::getInstance().updateSequence(sequence);
        }
        else
        {
            // not supported
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Adding/Modifying sequences online is not supported.");
        }
        // tell the GUI to update
        notifyGUIofDisplayItemUpdate(sequence);
    }
    else
    {
        // update received for unknown output device
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Online Update message received for unrecognised sequence %d",
                    updateEvent.getKey());
    }
}


void DisplayItemManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    // Extract the stateUpdateInfo from the State Update Message
    TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
    if (0 != (message.messageState >>= videoOutputState))       // Is this a VideoOutput state update message?
    {
        unsigned long outputKey = message.assocEntityKey;
        processVideoOutputStateMessage(outputKey, *videoOutputState);
        return;
    }
    TA_Base_Bus_GZL9::QuadCorbaDef::QuadState* quadState = 0;
    if (0 != (message.messageState >>= quadState))       // Is this a Quad state update message?
    {
        unsigned long quadKey = message.assocEntityKey;
        processQuadStateMessage(quadKey, *quadState);
        return;
    }
    TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
    if (0 != (message.messageState >>= sequenceConfig))       // Is this a Sequence Config update message?
    {
        unsigned long sequenceKey = message.assocEntityKey;
        processSequenceConfigMessage(sequenceKey, *sequenceConfig);
        return;
    }
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Unrecognised state update message received.");
}


void DisplayItemManager::processVideoOutputStateMessage(unsigned long outputKey, const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& videoOutputState)
{
    // get the monitor
    VideoOutput* output = getOutput(outputKey);
    if (output != NULL)
    {
        output->assignInput(getInput(videoOutputState.currentVideoInputKey));
        // tell the GUI to update
        notifyGUIofDisplayItemUpdate(output);
    }
    else
    {
        // update received for unknown output device
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Update message received for unrecognised video output %d",
                    outputKey);
    }
}


void DisplayItemManager::processQuadStateMessage(unsigned long quadKey, const TA_Base_Bus_GZL9::QuadCorbaDef::QuadState& quadState)
{
    // get the monitor
    Quad* quad = getQuad(quadKey);
    if (quad != NULL)
    {
        // set assignments
        quad->setTopLeft(getCamera(quadState.topLeftVideoInputKey));
        quad->setTopRight(getCamera(quadState.topRightVideoInputKey));
        quad->setBottomLeft(getCamera(quadState.bottomLeftVideoInputKey));
        quad->setBottomRight(getCamera(quadState.bottomRightVideoInputKey));
        // tell the GUI to update
        notifyGUIofDisplayItemUpdate(quad);
    }
    else
    {
        // update received for unknown output device
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Update message received for unrecognised quad %d",
                    quadKey);
    }
}


void DisplayItemManager::processSequenceConfigMessage(unsigned long sequenceKey, const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig)
{
    // get the monitor
    Sequence* sequence = getSequence(sequenceKey);
    if (sequence != NULL)
    {
        sequence->setDwellTime(sequenceConfig.dwellTime);
        std::list<Camera*> cameras;
        for (unsigned int i = 0; i < sequenceConfig.inputKeySequence.length(); i++)
        {
            // get the camera by the key
            Camera* cam = getCamera(sequenceConfig.inputKeySequence[i]);
            if (cam != NULL)
            {
                cameras.push_back(cam);
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Could not load camera with key %d for sequence %s",
                            sequenceConfig.inputKeySequence[i], sequence->getName().c_str()); //TD14337 yezh++
            }
        }
        sequence->setCameras(cameras);
        // tell the GUI to update
        notifyGUIofDisplayItemUpdate(sequence);
    }
    else
    {
        // update received for unknown output device
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Update message received for unrecognised sequence %d",
                    sequenceKey);
    }
}

void DisplayItemManager::registerForMonitorChanges(CWnd* window)
{
    registerWindow(monitorListeners, window);
}

void DisplayItemManager::registerForSequenceChanges(CWnd* window)
{
    registerWindow(sequenceListeners, window);
}

void DisplayItemManager::registerForQuadChanges(CWnd* window)
{
    registerWindow(quadListeners, window);
}

void DisplayItemManager::registerForCameraChanges(CWnd* window)
{
    registerWindow(cameraListeners, window);
}

void DisplayItemManager::registerForStageChanges(CWnd* window)
{
    registerWindow(stageListeners, window);
}


void DisplayItemManager::deregisterForMonitorChanges(CWnd* window)
{
    deregisterWindow(monitorListeners, window);
}

void DisplayItemManager::deregisterForSequenceChanges(CWnd* window)
{
    deregisterWindow(sequenceListeners, window);
}

void DisplayItemManager::deregisterForQuadChanges(CWnd* window)
{
    deregisterWindow(quadListeners, window);
}

void DisplayItemManager::deregisterForCameraChanges(CWnd* window)
{
    deregisterWindow(cameraListeners, window);
}

void DisplayItemManager::deregisterForStageChanges(CWnd* window)
{
    deregisterWindow(stageListeners, window);
}


void DisplayItemManager::registerWindow(std::list<CWnd*>& list, CWnd* window)
{
    TA_Base_Core::ThreadGuard guard(m_windowListLock);
    if (window != NULL)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);
        // add it to the map
        list.push_back(window);
    }
}


void DisplayItemManager::deregisterWindow(std::list<CWnd*>& list, CWnd* window)
{
    TA_Base_Core::ThreadGuard guard(m_windowListLock);
    // find the entry
    std::list<CWnd*>::iterator findIter = std::find(list.begin(),
                                                    list.end(),
                                                    window);
    // if found, delete it
    if (findIter != list.end())
    {
        list.erase(findIter);
    }
}




void DisplayItemManager::notifyMonitorListeners(Monitor* modifiedItem)
{
    postMessageToWindows(monitorListeners, WM_MONITOR_UPDATED, modifiedItem);
    // no flow on effects
}


void DisplayItemManager::notifySequenceListeners(Sequence* modifiedItem)
{
    postMessageToWindows(sequenceListeners, WM_SEQUENCE_UPDATED, modifiedItem);
    // check monitors for sequence assignments
    for (std::map<std::string, Monitor*>::iterator iter = m_monitors.begin();
            iter != m_monitors.end(); iter++)
    {
        if (iter->second->getInput() == modifiedItem)
        {
            notifyMonitorListeners(iter->second);
        }
    }
}


void DisplayItemManager::notifyQuadListeners(Quad* modifiedItem)
{
    postMessageToWindows(quadListeners, WM_QUAD_UPDATED, modifiedItem);
    // check monitors for quad assignments
    for (std::map<std::string, Monitor*>::iterator iter = m_monitors.begin();
            iter != m_monitors.end(); iter++)
    {
        if (iter->second->getInput() == modifiedItem)
        {
            notifyMonitorListeners(iter->second);
        }
    }
}


void DisplayItemManager::notifyCameraListeners(Camera* modifiedItem)
{
    postMessageToWindows(cameraListeners, WM_CAMERA_UPDATED, modifiedItem);
    // check monitors for camera assignments
    for (std::map<std::string, Monitor*>::iterator monIter = m_monitors.begin();
            monIter != m_monitors.end(); monIter++)
    {
        if (monIter->second->getInput() == modifiedItem)
        {
            notifyMonitorListeners(monIter->second);
        }
    }
    // check quads for assignments
    for (std::map<std::string, Quad*>::iterator quadIter = m_quads.begin();
            quadIter != m_quads.end(); quadIter++)
    {
        std::vector<Camera*> cameras = quadIter->second->getCameras();
        for (std::vector<Camera*>::iterator camIter = cameras.begin();
                camIter != cameras.end(); camIter++)
        {
            if (*camIter == modifiedItem)
            {
                notifyQuadListeners(quadIter->second);
            }
        }
    }
    // check sequences for assignments
    for (std::map<std::string, Sequence*>::iterator seqIter = m_sequences.begin();
            seqIter != m_sequences.end(); seqIter++)
    {
        std::list<Camera*> cameras = seqIter->second->getCameras();
        for (std::list<Camera*>::iterator camIter = cameras.begin();
                camIter != cameras.end(); camIter++)
        {
            if (*camIter == modifiedItem)
            {
                notifySequenceListeners(seqIter->second);
            }
        }
    }
}


void DisplayItemManager::postMessageToWindows(const std::list<CWnd*>& windows, int message, DisplayItem* itemPtr)
{
    TA_Base_Core::ThreadGuard guard(m_windowListLock);
    for (std::list<CWnd*>::const_iterator iter = windows.begin();
            iter != windows.end(); iter++)
    {
        // if the window is not null and is a window
        if (*iter != NULL)
        {
            (*iter)->PostMessage(message, NULL, reinterpret_cast<LPARAM>(itemPtr));
        }
    }
}


void DisplayItemManager::notifyGUIofDisplayItemUpdate(DisplayItem* itemThatChanged)
{
    // work out its type
    // do a smart update based on its type
    VideoInput* input = dynamic_cast<VideoInput*>(itemThatChanged);
    VideoOutput* output = dynamic_cast<VideoOutput*>(itemThatChanged);
    if (input != NULL)
    {
        Sequence* seq = dynamic_cast<Sequence*>(input);
        if (seq != NULL)
        {
            notifySequenceListeners(seq);
            return;
        }
        Quad* quad = dynamic_cast<Quad*>(input);
        if (quad != NULL)
        {
            notifyQuadListeners(quad);
            return;
        }
        Camera* camera = dynamic_cast<Camera*>(input);
        if (camera != NULL)
        {
            notifyCameraListeners(camera);
            return;
        }
    }
    else if (output != NULL)
    {
        Monitor* monitor = dynamic_cast<Monitor*>(output);
        if (monitor != NULL)
        {
            notifyMonitorListeners(monitor);
            return;
        }
    }
}

void DisplayItemManager::workerFree()
{
}

void DisplayItemManager ::loadStations()
{
    std::vector<TA_Base_Core::ILocation*> stations = m_videoDisplayManagerDAI->getAllLocations();
    for (std::vector<TA_Base_Core::ILocation*>::iterator it = stations.begin(); it != stations.end(); ++it)
    {
        // filter out the list to only station locations.
        TA_Base_Core::ILocation* location = *it;
        m_stations.push_back(*it);
    }
}

void DisplayItemManager::getGraphicfullPath(unsigned long stationId , std::string& fullPath, unsigned long level)
{
    try
    {
		fullPath = TA_Base_Core::SchematicDisplayMapAccessFactory::getInstance().getSchematicDisplay(stationId,
			CCTVConfiguration::getinstance().getCCTVSubsystemKey(), level);
    }
    catch (TA_Base_Core::DataException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", expWhat.what());
    }
    catch (TA_Base_Core::DatabaseException& expWhat)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", expWhat.what());
    }
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "the full path of diagram is %s " , fullPath.c_str());
}

void DisplayItemManager::initGraphicFullPath()
{
    //TODO
}

void DisplayItemManager::setVideoWallMonitorKeys(std::vector<unsigned long> keys)
{
    m_videoWallMonitorKeys = keys;
}

std::vector<TA_Base_Core::ILocation*>& DisplayItemManager::getStations()
{
    return m_stations;
}

void DisplayItemManager::loadStationsFromDatabase()
{
    loadStations();
    m_callbackWindow->PostMessage(TA_INIT_STATION);
}

void DisplayItemManager::getSequenceByLocation(
    unsigned long locationId , std::vector< VideoInput*>& sequences)
{
    std::vector <Sequence*>::iterator it = m_seqVec.begin();
    for (it; it != m_seqVec.end(); it++)
    {
        VideoInput* seq = (VideoInput*) * it;
        if (seq->getLocationId() == locationId)
        {
            sequences.push_back(seq);
        }
    }
}

void DisplayItemManager::updateSequence(Sequence * sequence)
{
    Sequence* seq = getSequence(sequence->getKey());
    //seq->setDescription(sequence->getDescription());
    seq->setSequenceDescription(sequence->getSequenceDescription());
    seq->setDwellTime(sequence->getDwellTime());
    seq->setCameras(sequence->getCameras());
}

void DisplayItemManager::updateSequenceDesc(Sequence * sequence, std::string description)
{
    Sequence* seq = getSequence(sequence->getKey());
    //seq->setDescription(description);
    seq->setSequenceDescription(sequence->getSequenceDescription());
}

void DisplayItemManager::loadCamerasFromAgent()
{
}
void DisplayItemManager::loadQuadsFromAgent()
{
}
void DisplayItemManager::loadSequencesFromAgent()
{
}
void DisplayItemManager::loadStagesFromAgent()
{
}
void DisplayItemManager::loadMonitorsFromAgent()
{
}
void DisplayItemManager::loadStationsFromAgent()
{
}
void DisplayItemManager::loadStagesFromDatabase()
{
}

const VirtualQuadArray& DisplayItemManager::getIMOSQuadMonitors()
{
    return m_imosQuadMonitors;
}

Monitor* DisplayItemManager::getIMOSSingleMonitor()
{
    return m_imosSingleMonitor;
}

const AllMonitorsArray& DisplayItemManager::getAllMonitorsArray()
{
    return m_allMonitorsArray;
}

} // TA_IRS_App

