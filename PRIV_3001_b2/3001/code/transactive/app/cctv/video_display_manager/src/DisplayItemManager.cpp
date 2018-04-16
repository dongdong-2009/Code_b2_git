/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/DisplayItemManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This singleton is required to keep a reference to all display
  * items. It maps each type of display item by name and provides access to
  * the items that are shared throughout the GUI - their state is updated
  * via comms messages and the display refreshed to reflect this.
  */

#include <algorithm>

#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/BvsStage.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/DisplayItemTasks.h"

#include "bus/cctv/video_switch_agent/idl/src/SwitchManagerCorbaDef.h"
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
#include "core/data_access_interface/entity_access/src/BvsStage.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/synchronisation/src/SingleThreadBarrier.h"
#include <boost/tokenizer.hpp>

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::EntityAccessFactory;

using TA_IRS_App::SwitchAgentCommunicator;

using CORBA::String_var;


namespace TA_IRS_App
{

    static const std::string OCC_WALL_MONITOR = "WALL";
    static const std::string OCC_ODS_MONITOR = "ODS";

    DisplayItemManager* DisplayItemManager::m_theInstance = NULL;
    TA_Base_Core::ReEntrantThreadLockable DisplayItemManager::m_singletonLock;


    DisplayItemManager& DisplayItemManager::getInstance()
    {
        ThreadGuard guard(m_singletonLock);
        if(m_theInstance == NULL)
        {
            m_theInstance = new DisplayItemManager();
        }
        return *m_theInstance;
    }


    void DisplayItemManager::removeInstance()
    {
        ThreadGuard guard(m_singletonLock);
        if(m_theInstance != NULL)
        {
            delete m_theInstance;
            m_theInstance = NULL;
        }
    }


    DisplayItemManager::DisplayItemManager()
        : m_cameras(),
          m_camerasByPkey(),
          m_camerasByAddress(),
          m_sequences(),
          m_sequencesByPkey(),
          m_monitors(),
          m_monitorsByPkey(),
          m_locationkey(),
          m_locationType(),
          m_callbackWindow(NULL)
    {
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
        try
        {
            // Determine the local agent for the physical Location of this Console Session.
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
            m_locationkey = console->getLocation();
            TA_Base_Core::ILocation* location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(m_locationkey);
            m_locationType = location->getLocationType();
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }
        catch(const TA_Base_Core::DataException& e)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }
        // just in case
        deleteAllItems();
        time_t startTime = time(NULL);
        try
        {
            loadItemsFromAgent();
        }
        catch(...)
        {
        }
        if((0 == m_cameras.size()) &&
                (0 == m_sequences.size()) &&
                (0 == m_monitors.size()))
        {
			loadItemsFromDatabase();
        }
        time_t elapsedTime = time(NULL) - startTime;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm,
                    "Loaded %d Cameras,  %d Sequences, %d Monitors, caused %d second",
                    m_cameras.size(),
                    m_sequences.size(),
                    m_monitors.size(),
					elapsedTime);
        // subscribes for comms messages and online updates,
        subscribeToMessages();
        m_callbackWindow->PostMessage(TA_INIT_CCTV);
    }


    void DisplayItemManager::terminate()
    {
    }


    void DisplayItemManager::loadItemsFromAgent()
	{
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId));
		// Camera
		TA_Base_Bus::SwitchManagerCorbaDef::VideoObjectsInfo_var objectsInfo;
        CORBA_CALL_RETURN(objectsInfo, TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(console->getKey()), getVideoObjectsInfo, ());
		TA_Base_Bus::SwitchManagerCorbaDef::CameraInfoSequence& camerasInfo = objectsInfo->camerasInfo;
		for(unsigned long i = 0; i < camerasInfo.length(); ++i)
		{
			
			createCamera(camerasInfo[i].key,
				camerasInfo[i].name.in(),
				camerasInfo[i].description.in(),
				camerasInfo[i].groupName.in(),
				camerasInfo[i].address.in());
		}
		m_callbackWindow->PostMessage(TA_INIT_CAMERAS);
		// Sequence
		TA_Base_Bus::SwitchManagerCorbaDef::SequenceInfoSequence& sequencesInfo = objectsInfo->sequencesInfo;
		for(unsigned long i = 0; i < sequencesInfo.length(); ++i)
		{
			Sequence* newSequence = createSequence(sequencesInfo[i].key,
					sequencesInfo[i].name.in(),
					sequencesInfo[i].description.in(),
					sequencesInfo[i].groupName.in(),
					sequencesInfo[i].isReadOnly,
					sequencesInfo[i].sequenceDescription.in());
			TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig = sequencesInfo[i].sequenceConfig;
			newSequence->setDwellTime(sequenceConfig.dwellTime);
			std::list<Camera*> cameras;
			for(unsigned long j = 0; j < sequenceConfig.inputKeySequence.length(); ++j)
			{
				Camera* cam = getCamera(sequenceConfig.inputKeySequence[j]);
				if(cam != NULL)
				{
					cameras.push_back(cam);
				}
				else
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load camera with key %d for sequence %s",
						sequenceConfig.inputKeySequence[j], sequencesInfo[i].name.in());
				}
			}
			newSequence->setCameras(cameras);
		}
		m_callbackWindow->PostMessage(TA_INIT_SEQUENCES);
		// Monitor
		TA_Base_Bus::SwitchManagerCorbaDef::MonitorInfoSequence& monitorsInfo = objectsInfo->monitorsInfo;
		for(unsigned long i = 0; i < monitorsInfo.length(); ++i)
		{
			Monitor* newMonitor = createMonitor(monitorsInfo[i].key,
				monitorsInfo[i].name.in(),
				monitorsInfo[i].description.in(),
				monitorsInfo[i].address.in(),
				monitorsInfo[i].displayOrder,
				monitorsInfo[i].consoleKey,
				monitorsInfo[i].isOverall);
			TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState outputState = monitorsInfo[i].outputState;
			newMonitor->setTriggeringAlarm(outputState.isInAlarmMode);
			newMonitor->setShowingAckAlarm(outputState.isShowingAckAlarm);
			if(outputState.isInAlarmMode)
			{
				newMonitor->setAlarmStackItems(outputState.alarmStack);
			}
			else if(outputState.isQuadInput )
			{
				Camera* cameraW,*cameraX,*cameraY,*cameraZ;
				cameraW = dynamic_cast<Camera*>(getInput(outputState.cameras.cameraW));
				cameraX = dynamic_cast<Camera*>(getInput(outputState.cameras.cameraX));
				cameraY = dynamic_cast<Camera*>(getInput(outputState.cameras.cameraY));
				cameraZ = dynamic_cast<Camera*>(getInput(outputState.cameras.cameraZ));
				newMonitor->assignQuad(cameraW,cameraX,cameraY,cameraZ);
			}
			else
			{
				newMonitor->assignInput( getInput( outputState.currentVideoInputKey ) );
			}
		}
		m_callbackWindow->PostMessage(TA_INIT_MONITORS);
	}

    void DisplayItemManager::loadItemsFromDatabase()
    {
		// Should not running in GUI thread
		loadCamerasFromDatabase();
		loadSequencesFromDatabase();
		loadMonitorsFromDatabase();
    }

    void DisplayItemManager::loadCamerasFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Loading cameras");
        time_t startTime = time(NULL);
        try
        {
            IEntityDataList cameraEntities  = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Camera::getStaticType(), m_locationkey);
            for(IEntityDataList::iterator it = cameraEntities.begin(); it != cameraEntities.end(); ++it)
            {
                TA_Base_Core::Camera* cameraEntity = dynamic_cast<TA_Base_Core::Camera*>(*it);
                // get all details
                unsigned long key = cameraEntity->getKey();
                std::string name = cameraEntity->getName();
                std::string desc = cameraEntity->getDescription();
                std::string group = cameraEntity->getVideoInputGroupName();
                std::string address = cameraEntity->getAddress();
                delete cameraEntity;
                createCamera(key, name, desc, group, address);
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        time_t elapsedTime = time(NULL) - startTime;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading cameras. Elapsed time %d seconds", elapsedTime);
        m_callbackWindow->PostMessage(TA_INIT_CAMERAS);
    }

    void DisplayItemManager::loadSequencesFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading sequences");
        time_t startTime = time(NULL);
        try
        {
            IEntityDataList sequenceEntities;
            sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), m_locationkey);
            IEntityDataList::iterator it;
            for(it = sequenceEntities.begin(); it != sequenceEntities.end(); it++)
            {
                std::auto_ptr<TA_Base_Core::Sequence> sequenceEntity(dynamic_cast<TA_Base_Core::Sequence*>(*it));
                // get all details
                unsigned long key = sequenceEntity->getKey();
                std::string name = sequenceEntity->getName();
                std::string desc = sequenceEntity->getDescription();
                std::string group = sequenceEntity->getVideoInputGroupName();
                bool readOnly = sequenceEntity->isReadOnly();
                std::string sequenceDescription = sequenceEntity->getSequenceDescription();
                Sequence* seq = createSequence(key, name, desc, group, readOnly, sequenceDescription);
                // get dwell time from database
                TA_Base_Core::EntityHelper* help = sequenceEntity->getHelper();
                std::string temp = help->getParameter("SequenceDwellTime");
                seq->setDwellTime(atoi(temp.c_str()));
                // get cameras config
                typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
                boost::char_separator<char> sep(",");
				std::string cameraList = help->getParameter("AttachedCameras");
                tokenizer tokens(cameraList, sep);
                std::list<Camera*> cameras;
                for(tokenizer::iterator it2 = tokens.begin(); it2 != tokens.end(); ++it2)
				{
					Camera* cam = getCamera(*it2);
					if(NULL != cam)
					{
						cameras.push_back(cam);
					}
					else
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load camera %s for sequence %s", (*it2).c_str(), name.c_str());
					}
				}
                seq->setCameras(cameras);
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        time_t elapsedTime = time(NULL) - startTime;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading sequences. Elapsed time %d seconds", elapsedTime);
        m_callbackWindow->PostMessage(TA_INIT_SEQUENCES);
    }

    void DisplayItemManager::loadMonitorsFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Loading monitors");
        time_t startTime = time(NULL);
        try
        {
            IEntityDataList monitorEntities;
            monitorEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), m_locationkey);
            for(IEntityDataList::iterator it = monitorEntities.begin(); it != monitorEntities.end(); ++it)
            {
                std::auto_ptr<TA_Base_Core::VideoMonitor> monitorEntity(dynamic_cast<TA_Base_Core::VideoMonitor*>(*it));
                // get all details
                unsigned long key = monitorEntity->getKey();
                std::string name = monitorEntity->getName();
                std::string desc = monitorEntity->getDescription();
                std::string addr = monitorEntity->getAddress();
                unsigned int displayOrder = monitorEntity->getDisplayOrder();
                unsigned long ConsoleKey = monitorEntity->getConsoleKey() ;
                bool bIsOverallMonitor = monitorEntity->isOverallMonitor();
                createMonitor(key, name, desc, addr, displayOrder, ConsoleKey, bIsOverallMonitor);
            }
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        time_t elapsedTime = time(NULL) - startTime;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Finished loading monitors. Elapsed time %d seconds",
                    elapsedTime);
        m_callbackWindow->PostMessage(TA_INIT_MONITORS);
    }


    Camera* DisplayItemManager::createCamera(unsigned long key,
                                             const std::string& name,
                                             const std::string& description,
                                             const std::string& group,
                                             const std::string& address)
    {
        // create the local object
        Camera* camPtr = new Camera(key, name, description, address, group);
        m_cameras[name] = camPtr;
        m_camerasByPkey[key] = camPtr;
        m_camerasByAddress[address] = camPtr;
        return camPtr;
    }

    Sequence* DisplayItemManager::createSequence(unsigned long key,
                                                 const std::string& name,
                                                 const std::string& description,
                                                 const std::string& group,
                                                 bool readOnly,
                                                 const std::string& sequenceDescription)
    {
        // create the local object
        Sequence* seqPtr = new Sequence(key, name, description, group);
        seqPtr->setReadOnly(readOnly);
        seqPtr->setSequenceDescription(sequenceDescription);
        // add it to the maps
        m_sequences[name] = seqPtr;
        m_sequencesByPkey[key] = seqPtr;
        return seqPtr;
    }

    Monitor* DisplayItemManager::createMonitor(unsigned long key,
                                               const std::string& name,
                                               const std::string& description,
                                               const std::string& addr,
                                               unsigned int displayOrder,
                                               unsigned long consoleKey,
                                               bool bIsOverallMonitor)
    {
        // create the local object
        Monitor* monPtr = new Monitor(key, name, description, addr, displayOrder, consoleKey, bIsOverallMonitor);
        m_monitors[ name ] = monPtr;
        m_monitorsByPkey[ key ] = monPtr;
        if(monPtr->isConsoleMonitor())
        {
            m_consoleMonitors.push_back(monPtr->getName());
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "createMonitor( %ld,%s,%s,%s,%d,%ld,%d)",
                    key, name.c_str(), description.c_str(), addr.c_str(), displayOrder, consoleKey, bIsOverallMonitor);
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
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::BvsStageStateUpdate,
                                                                                        this, 0, 0, locationKey, NULL);
        std::vector< unsigned long > sequenceKeys;
        for(std::map<std::string, Sequence*>::iterator iter = m_sequences.begin();
                iter != m_sequences.end(); iter++)
        {
            sequenceKeys.push_back(iter->second->getKey());
        }
        TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests(TA_Base_Core::ENTITY, *this, sequenceKeys);
    }


    Camera* DisplayItemManager::getCamera(const std::string& name)
    {
        // find and return the item in the map
        Camera* cam = NULL;
        std::map<std::string, Camera*>::iterator findIter = m_cameras.find(name);
        if(findIter != m_cameras.end())
        {
            cam = findIter->second;
        }
        return cam;
    }


    Camera* DisplayItemManager::getCamera(unsigned long pkey)
    {
        // find and return the item in the map
        Camera* cam = NULL;
        std::map<unsigned long, Camera*>::iterator findIter = m_camerasByPkey.find(pkey);
        if(findIter != m_camerasByPkey.end())
        {
            cam = findIter->second;
        }
        return cam;
    }


    Camera* DisplayItemManager::getCameraByAddress(std::string address)
    {
        // find and return the item in the map
        Camera* cam = NULL;
        std::map<std::string, Camera*>::iterator findIter = m_camerasByAddress.find(address);
        if(findIter != m_camerasByAddress.end())
        {
            cam = findIter->second;
        }
        return cam;
    }

    Sequence* DisplayItemManager::getSequence(std::string name)
    {
        Sequence* seq = NULL;
        std::map<std::string, Sequence*>::iterator findIter = m_sequences.find(name);
        if(findIter != m_sequences.end())
        {
            seq = findIter->second;
        }
        return seq;
    }


    Sequence* DisplayItemManager::getSequence(unsigned long pkey)
    {
        Sequence* seq = NULL;
        std::map<unsigned long, Sequence*>::iterator findIter = m_sequencesByPkey.find(pkey);
        if(findIter != m_sequencesByPkey.end())
        {
            seq = findIter->second;
        }
        return seq;
    }

    VideoInput* DisplayItemManager::getInput(std::string name)
    {
        VideoInput* input = NULL;
        input = getCamera(name);
        if(NULL == input)
        {
            input = getSequence(name);
        }
        return input;
    }


    VideoInput* DisplayItemManager::getInput(unsigned long pkey)
    {
        VideoInput* input = NULL;
        input = getCamera(pkey);
        if(NULL == input)
        {
            input = getSequence(pkey);
        }
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
        Monitor* mon = NULL;
        std::map<std::string, Monitor*>::iterator findIter = m_monitors.find(name);
        if(findIter != m_monitors.end())
        {
            mon = findIter->second;
        }
        return mon;
    }


    Monitor* DisplayItemManager::getMonitor(unsigned long pkey)
    {
        Monitor* mon = NULL;
        std::map<unsigned long, Monitor*>::iterator findIter = m_monitorsByPkey.find(pkey);
        if(findIter != m_monitorsByPkey.end())
        {
            mon = findIter->second;
        }
        return mon;
    }

    std::vector<std::string> DisplayItemManager::getConsoleMonitors()
    {
        return m_consoleMonitors;
    }

    std::map<std::string, Camera*> DisplayItemManager::getAllCameras()
    {
        return m_cameras;
    }

    std::map<std::string, Sequence*> DisplayItemManager::getAllSequences()
    {
        return m_sequences;
    }

    std::map<std::string, Monitor*> DisplayItemManager::getAllMonitors()
    {
        return m_monitors;
    }

    void DisplayItemManager::deleteAllItems()
    {
        for(std::map<std::string, Monitor*>::iterator monIter = m_monitors.begin(); monIter != m_monitors.end(); ++monIter)
        {
            delete monIter->second;
            monIter->second = NULL;
        }
        m_monitors.clear();
        m_monitorsByPkey.clear();
        for(std::map<std::string, Sequence*>::iterator seqIter = m_sequences.begin(); seqIter != m_sequences.end(); ++seqIter)
        {
            delete seqIter->second;
            seqIter->second = NULL;
        }
        m_sequences.clear();
        m_sequencesByPkey.clear();
        for(std::map<std::string, Camera*>::iterator camIter = m_cameras.begin(); camIter != m_cameras.end(); ++camIter)
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
        if(sequence != NULL)
        {
            // the only thing to bother updating is the description
            // online adding/deleting of sequences is not supported
            if(updateEvent.getModifications() == TA_Base_Core::Update)
            {
                SwitchAgentCommunicator::getInstance().updateSequence(sequence);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Adding/Modifying sequences online is not supported.");
            }
            notifyGUIofDisplayItemUpdate(sequence);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Online Update message received for unrecognised sequence %d",
				updateEvent.getKey());
        }
    }


    void DisplayItemManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        // Extract the stateUpdateInfo from the State Update Message
        TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
        if(0 != (message.messageState >>= videoOutputState))        // Is this a VideoOutput state update message?
        {
            unsigned long outputKey = message.assocEntityKey;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "processVideoOutputStateMessage.[%lu]", outputKey);
            processVideoOutputStateMessage(outputKey, *videoOutputState);
            return;
        }
        TA_Base_Bus::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
        if(0 != (message.messageState >>= sequenceConfig))        // Is this a Sequence Config update message?
        {
            unsigned long sequenceKey = message.assocEntityKey;
            processSequenceConfigMessage(sequenceKey, *sequenceConfig);
            return;
        }
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Unrecognised state update message received.");
    }


    void DisplayItemManager::processVideoOutputStateMessage(unsigned long outputKey, const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& videoOutputState)
    {
        // get the monitor
        VideoOutput* output = getOutput(outputKey);
        if(output != NULL)
        {
            output->setTriggeringAlarm(videoOutputState.isInAlarmMode);
            output->setShowingAckAlarm(videoOutputState.isShowingAckAlarm);
            if(videoOutputState.isInAlarmMode || videoOutputState.isShowingAckAlarm)
            {
                output->setAlarmStackItems(videoOutputState.alarmStack);
            }
            else
            {
                // clear the alarm stack
                output->clearAlarmStack();
                // get the input
                // can be a sequence, camera, quad etc
                if(videoOutputState.isQuadInput)
                {
                    Camera* cameraW, *cameraX, *cameraY, *cameraZ;
                    cameraW = dynamic_cast<Camera*>(getInput(videoOutputState.cameras.cameraW));
                    cameraX = dynamic_cast<Camera*>(getInput(videoOutputState.cameras.cameraX));
                    cameraY = dynamic_cast<Camera*>(getInput(videoOutputState.cameras.cameraY));
                    cameraZ = dynamic_cast<Camera*>(getInput(videoOutputState.cameras.cameraZ));
                    output->assignQuad(cameraW, cameraX, cameraY, cameraZ);
                }
                else
				{
                    output->assignInput(getInput(videoOutputState.currentVideoInputKey));
				}
            }
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

    void DisplayItemManager::processSequenceConfigMessage(unsigned long sequenceKey, const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig)
    {
        // get the monitor
        Sequence* sequence = getSequence(sequenceKey);
        if(sequence != NULL)
        {
            std::string temp = sequenceConfig.description;
            sequence->setSequenceDescription(temp);
            sequence->setDwellTime(sequenceConfig.dwellTime);
            std::list<Camera*> cameras;
            for(unsigned int i = 0; i < sequenceConfig.inputKeySequence.length(); i++)
            {
                // get the camera by the key
                Camera* cam = getCamera(sequenceConfig.inputKeySequence[i]);
                if(cam != NULL)
                {
                    cameras.push_back(cam);
                }
                else
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not load camera with key %d for sequence %s",
						sequenceConfig.inputKeySequence[i], sequence->getName().c_str());
                }
            }
            sequence->setCameras(cameras);
            // tell the GUI to update
            notifyGUIofDisplayItemUpdate(sequence);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Update message received for unrecognised sequence %d",
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

    void DisplayItemManager::registerForCameraChanges(CWnd* window)
    {
        registerWindow(cameraListeners, window);
    }
    void DisplayItemManager::deregisterForMonitorChanges(CWnd* window)
    {
        deregisterWindow(monitorListeners, window);
    }

    void DisplayItemManager::deregisterForSequenceChanges(CWnd* window)
    {
        deregisterWindow(sequenceListeners, window);
    }

    void DisplayItemManager::deregisterForCameraChanges(CWnd* window)
    {
        deregisterWindow(cameraListeners, window);
    }

    void DisplayItemManager::registerWindow(std::list<CWnd*>& list, CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);
        if(window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);
            list.push_back(window);
        }
    }


    void DisplayItemManager::deregisterWindow(std::list<CWnd*>& list, CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);
		list.erase(std::remove(list.begin(), list.end(), window), list.end());
    }




    void DisplayItemManager::notifyMonitorListeners(Monitor* modifiedItem)
    {
        postMessageToWindows(monitorListeners, WM_MONITOR_UPDATED, modifiedItem);
    }


    void DisplayItemManager::notifySequenceListeners(Sequence* modifiedItem)
    {
        postMessageToWindows(sequenceListeners, WM_SEQUENCE_UPDATED, modifiedItem);
        // check monitors for sequence assignments
        for(std::map<std::string, Monitor*>::iterator iter = m_monitors.begin(); iter != m_monitors.end(); ++iter)
        {
            if(iter->second->getInput() == modifiedItem)
            {
                notifyMonitorListeners(iter->second);
            }
        }
    }

    void DisplayItemManager::notifyCameraListeners(Camera* modifiedItem)
    {
        postMessageToWindows(cameraListeners, WM_CAMERA_UPDATED, modifiedItem);
        // check monitors for camera assignments
        for(std::map<std::string, Monitor*>::iterator monIter = m_monitors.begin(); monIter != m_monitors.end(); ++monIter)
        {
            if(monIter->second->getInput() == modifiedItem)
            {
                notifyMonitorListeners(monIter->second);
            }
        }
        for(std::map<std::string, Sequence*>::iterator seqIter = m_sequences.begin(); seqIter != m_sequences.end(); ++seqIter)
        {
            std::list<Camera*> cameras = seqIter->second->getCameras();
            for(std::list<Camera*>::iterator camIter = cameras.begin(); camIter != cameras.end(); ++camIter)
            {
                if(*camIter == modifiedItem)
                {
                    notifySequenceListeners(seqIter->second);
                }
            }
        }
    }

    void DisplayItemManager::postMessageToWindows(const std::list<CWnd*>& windows, int message, DisplayItem* itemPtr)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);
        for(std::list<CWnd*>::const_iterator iter = windows.begin();
                iter != windows.end(); iter++)
        {
            // if the window is not null and is a window
            if(*iter != NULL)
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
        if(input != NULL)
        {
            Sequence* seq = dynamic_cast<Sequence*>(input);
            if(seq != NULL)
            {
                notifySequenceListeners(seq);
            }
            else
            {
                Camera* camera = dynamic_cast<Camera*>(input);
                if(camera != NULL)
                {
                    notifyCameraListeners(camera);
                }
            }
        }
        else if(output != NULL)
        {
            Monitor* monitor = dynamic_cast<Monitor*>(output);
            if(monitor != NULL)
            {
                notifyMonitorListeners(monitor);
            }
        }
    }

    unsigned long DisplayItemManager::getLocationKey()
    {
        return m_locationkey;
    }
} // TA_IRS_App

