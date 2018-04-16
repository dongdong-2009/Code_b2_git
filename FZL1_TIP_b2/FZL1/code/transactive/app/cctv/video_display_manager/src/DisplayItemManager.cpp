/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * This singleton is required to keep a reference to all display
  * items. It maps each type of display item by name and provides access to
  * the items that are shared throughout the GUI - their state is updated
  * via comms messages and the display refreshed to reflect this.
  */

#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/BarrieredTask.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDAIImpl.h"
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
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"
#include "core/synchronisation/src/SingleThreadBarrier.h"
#include "core/threads/src/ThreadedWorker.h"
#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include <sstream>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::IEntityData;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::EntityAccessFactory;

/*using TA_IRS_App::SwitchAgentCommunicator;
typedef TA_IRS_App::SwitchAgentCommunicator::CamToNamedObject CamToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::SequenceToNamedObject SequenceToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::MonitorToNamedObject MonitorToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::OutputToNamedObject OutputToNamedObject;
*/
using CORBA::String_var;


namespace TA_IRS_App
{

    static const std::string OCC_WALL_MONITOR = "WALL";
    static const std::string OCC_ODS_MONITOR = "ODS";
    typedef TA_Base_Core::IEntityData* EntityDataPtr;

    bool CompareEntity(const EntityDataPtr& entity1, const EntityDataPtr& entity2)
    {
        return (entity1->getKey() < entity2->getKey());
    }

    DisplayItemManager& DisplayItemManager::getInstance()
    {
		return *(ACE_Singleton<DisplayItemManager, ACE_Recursive_Thread_Mutex>::instance());
    }

	void DisplayItemManager::removeInstance()
	{
		ACE_Singleton<DisplayItemManager, ACE_Recursive_Thread_Mutex>::close();
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
          m_callbackWindow(NULL),
		  m_localMonitor(NULL),
		  m_localConsoleKey(0),
		  m_cctvSubSystemKey(-1)
    {
        m_videoDisplayManagerDAI = new VideoDisplayManagerDAIImpl();
    }


    DisplayItemManager::~DisplayItemManager()
    {
		if(m_itemLoadingThread.get() != NULL)
		{
			m_itemLoadingThread->terminateAndWait();
		}
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
        deleteAllItems();
    }


    void DisplayItemManager::initialiseDisplayItems(CWnd* callbackWindow)
    {
        m_callbackWindow = callbackWindow;
		m_itemLoadingThread = std::auto_ptr<ThreadedMemberFunc>(new ThreadedMemberFunc(boost::bind(&DisplayItemManager::initItems, this)));
    }


    void DisplayItemManager::setVideoDisplayManagerDAI(IVideoDisplayManagerDAI* videoDisplayManagerDAI)
    {
        m_videoDisplayManagerDAI = videoDisplayManagerDAI;
    }

    void DisplayItemManager::initItems()
    {
        try
        {
            // Determine the local agent for the physical Location of this Console Session.
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
			m_localConsoleKey = console->getKey();
            m_locationkey = console->getLocation();
            TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(m_locationkey);
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
        loadItemsFromDatabase();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm,
                    "Loaded %d Cameras, %d Sequences, %d Monitors",
                    m_cameras.size(),
                    m_sequences.size(),
                    m_monitors.size());
        // subscribes for comms messages and online updates,
        subscribeToMessages();
        m_callbackWindow->PostMessage(TA_INIT_CCTV);
    }

    void DisplayItemManager::loadItemsFromDatabase()
    {
        // everything here can be done in parallel
        // first load cameras from database
        loadCamerasFromDatabase();
        TA_Base_Core::ThreadedWorker sequenceWorker(*this);
        sequenceWorker.start();
        TA_Base_Core::ThreadedWorker monitorWorker(*this);
        monitorWorker.start();
        TA_Base_Core::ThreadedWorker stationWorker(*this);
        stationWorker.start();
        // create the barrier
        TA_Base_Core::SingleThreadBarrier barrier(3);
		TA_Base_Core::IWorkItemPtr sequenceTask(new BarrieredTask(boost::bind(&DisplayItemManager::loadSequencesFromDatabase, this), barrier));
        sequenceWorker.executeWorkItem(sequenceTask);
        TA_Base_Core::IWorkItemPtr monitorTask(new BarrieredTask(boost::bind(&DisplayItemManager::loadMonitorsFromDatabase, this), barrier));
        monitorWorker.executeWorkItem(monitorTask);
        TA_Base_Core::IWorkItemPtr stationTask(new BarrieredTask(boost::bind(&DisplayItemManager::loadStationsFromDatabase, this), barrier));
        stationWorker.executeWorkItem(stationTask);
        // wait on the barrier
        barrier.wait();
        // stop all the threads
        sequenceWorker.terminateAndWait();
        monitorWorker.terminateAndWait();
		stationWorker.terminateAndWait();
        LoadSequenceRelationFromDatabase();
    }


    void DisplayItemManager::loadCamerasFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading cameras");
        time_t startTime = time(NULL);
		try
		{
            IEntityDataList cameraEntities;
            if(TA_Base_Core::ILocation::STATION == m_locationType)
			{
                cameraEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Camera::getStaticType(), m_locationkey);
			}
			else // Should be OCC
			{
				cameraEntities = EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::Camera::getStaticType(), false, true, true);
			}
			BOOST_FOREACH(IEntityData* i, cameraEntities)
			{
				std::auto_ptr<TA_Base_Core::Camera> cameraEntity(dynamic_cast<TA_Base_Core::Camera*>(i));
				TA_ASSERT(cameraEntity.get() != NULL, "");
                unsigned long key = cameraEntity->getKey();
                std::string name = cameraEntity->getName();
                std::string desc = cameraEntity->getDescription();
                std::string group = cameraEntity->getVideoInputGroupName();
                std::string address = cameraEntity->getAddress();
                unsigned long locationid = cameraEntity->getLocation();
                bool isMovementSupported = cameraEntity->isMovementSupported();
                createCamera(key, name, desc, group, address , locationid, isMovementSupported);
				if(-1 == m_cctvSubSystemKey)
				{
					m_cctvSubSystemKey = cameraEntity->getSubsystem();
				}
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
            if(TA_Base_Core::ILocation::STATION == m_locationType)
			{
                sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), m_locationkey);
			}
			else // Should be OCC
			{
				sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfType(TA_Base_Core::Sequence::getStaticType() ,false, true, true);
			}
            std::sort(sequenceEntities.begin(), sequenceEntities.end(), TA_IRS_App::CompareEntity);
			BOOST_FOREACH(IEntityData* i, sequenceEntities)
            {
                std::auto_ptr<TA_Base_Core::Sequence> sequenceEntity(dynamic_cast<TA_Base_Core::Sequence*>(i));
				TA_ASSERT(sequenceEntity.get() != NULL, "");
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
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Finished loading sequences. Elapsed time %d seconds",
                    elapsedTime);
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
			BOOST_FOREACH(Camera* i, cameras)
            {
                cameraKeys.push_back(i->getKey());
            }
            TA_IRS_Core::SequenceHelper sequenceHelper;
            sequenceHelper.setSequenceRelationShip(parentKey, dwellTime, description, cameraKeys);
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
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Finished loading sequences. Elapsed time %d seconds",
                    elapsedTime);
    }

    void DisplayItemManager::loadMonitorsFromDatabase()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading monitors");
        time_t startTime = time(NULL);
        try
        {
            IEntityDataList monitorEntities;
            monitorEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), m_locationkey);
            std::sort(monitorEntities.begin(), monitorEntities.end(), TA_IRS_App::CompareEntity);
            Monitor* quadMonitors[4] = {NULL, NULL, NULL, NULL};
            for(IEntityDataList::iterator it = monitorEntities.begin(); it != monitorEntities.end(); ++it)
            {
                std::auto_ptr<TA_Base_Core::VideoMonitor> monitorEntity(dynamic_cast<TA_Base_Core::VideoMonitor*>(*it));
                // get all details
                unsigned long key = monitorEntity->getKey();
                std::string name = monitorEntity->getName();
                std::string desc = monitorEntity->getDescription();
                unsigned long consoleKey = monitorEntity->getConsoleKey();
                int displayOrder = monitorEntity->getDisplayOrder();
				std::string address = monitorEntity->getAddress();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Monitor - entitykey:%lu, name:%s, consolekey:%lu, displayorder:%d, address:%s",
                    key, name.c_str(), consoleKey, displayOrder, address.c_str());

				// Station only location monitor and 4 virtual quad monitor,
				// from database only load console monitor for local monitor
				if( TA_Base_Core::ILocation::STATION == m_locationType )
				{
					if( 0 == displayOrder)
					{	//Local monitor
						if(m_localMonitor == NULL)
						{	//only read the first entity if the entity is not configed consolekey
	                        m_localMonitor =
                                createMonitor(key, name, desc, true, m_localConsoleKey, displayOrder, address, false);
						}
						else if(m_localConsoleKey == consoleKey)
						{	//There are duplicate data configed in db which their displayOrder is 0, so just keep the entity which configed consoleKey
                            deleteMonitor(m_localMonitor);
	                        m_localMonitor =
                                createMonitor(key, name, desc, true, consoleKey, displayOrder, address ,false);
						}
					}
                    else if(1 <= displayOrder && displayOrder <= 4)
                    {	//Quad
                        if(quadMonitors[displayOrder-1] == NULL)
                        {	//only read the first entity if the entity is not configed consolekey
    						quadMonitors[displayOrder-1] =
                                createMonitor(key, name, desc, false, consoleKey, displayOrder, address, true);
                        }
                        else if(m_localConsoleKey == consoleKey)
                        {	//There are duplicate data configed in db which their displayOrder is 0, so just keep the entity which configed consoleKey
                            deleteMonitor(quadMonitors[displayOrder-1]);
                            quadMonitors[displayOrder-1] =
                                createMonitor(key, name, desc, false, consoleKey, displayOrder, address, true);
                        }
                    }
				}
				else // OCC
				{
					if( 0 == displayOrder && consoleKey == m_localConsoleKey && m_localMonitor == NULL)		//If db have duplicate item for local monitor, only read the first record
					{
						m_localMonitor = createMonitor(key, name, desc, true, consoleKey, displayOrder, address, false);
					}
					else if(1 <= displayOrder && displayOrder <= 4 && consoleKey == m_localConsoleKey && quadMonitors[displayOrder-1] == NULL)
					{
						// quad in occ must config consolekey
						createMonitor(key, name, desc, false, consoleKey, displayOrder, address, true);
					}
					else if(1 <= displayOrder && displayOrder <= 24  && consoleKey == 0)
					{
						// OPS monitor not need consolekey value
	                    createMonitor(key, name, desc, false, 0, displayOrder, address, false);
					}
				}
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
        TA_IRS_Core::SequenceHelper::sequence_to_input_map sequences;
        std::vector<unsigned long> sequenceKeys;
		BOOST_FOREACH(Sequence* i, m_seqVec)
        {
            sequenceKeys.push_back(i->getKey());
        }
        TA_IRS_Core::SequenceHelper sequenceHelper;
        sequenceHelper.getSequenceRelationShipInSequences(sequenceKeys, sequences);
		BOOST_FOREACH(Sequence* i, m_seqVec)
        {
            if(sequences.find(i->getKey()) == sequences.end())
            {
                continue;
            }
            TA_IRS_Core::SequenceHelper::sequence_input& sequenceInput = sequences[i->getKey()];
            std::list<Camera*> cameras;
			BOOST_FOREACH(unsigned long j, sequenceInput.m_inputKeys)
            {
                // get the camera by the key
                Camera* cam = getCamera(j);
                if(cam != NULL)
                {
                    cameras.push_back(cam);
                }
            }
            i->setDwellTime(sequenceInput.m_dwellTime);
            i->setCameras(cameras);
            if(!sequenceInput.m_descriptions.empty())
            {
                i->setSequenceDescription(sequenceInput.m_descriptions);
            }
        }
    }

    void DisplayItemManager::loadEntityRelationship(unsigned long parentKey, unsigned long& pkey, std::string& description, int* ptype)
    {
        TA_IRS_Core::SequenceHelper sequenceHelper;
        sequenceHelper.getRelationShipFromParentKey(parentKey, pkey, description, ptype);
    }

    void DisplayItemManager::saveEntityRelationship(unsigned long parentKey, unsigned long pkey, const std::string& description, int type)
    {
        TA_IRS_Core::SequenceHelper sequenceHelper;
        sequenceHelper.setRelationShipOfParentKey(parentKey, pkey, description, type);
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
											   const std::string& address,
											   bool isQuadMon)
    {
        // create the local object
        Monitor* monitor = new Monitor(key, name, description, isConsoleMonitor, consoleKey, order, address, isQuadMon);
        m_monitors[ name ] = monitor;
        m_monitorsByPkey[ key ] = monitor;
        return monitor;
    }


    void DisplayItemManager::subscribeToMessages()
    {
        unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::VideoOutputStateUpdate,
                                                                                        this, 0, 0, locationKey, NULL);

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(TA_Base_Core::VisualComms::SequenceConfigUpdate,
                                                                                        this, 0, 0, locationKey, NULL);

        std::vector< unsigned long > sequenceKeys;
        for(std::map<std::string, Sequence*>::iterator it = m_sequences.begin(); it != m_sequences.end(); ++it)
        {
            sequenceKeys.push_back(it->second->getKey());
        }
        TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests(TA_Base_Core::ENTITY, *this, sequenceKeys);
    }


    Camera* DisplayItemManager::getCamera(std::string name)
    {
        std::map<std::string, Camera*>::iterator it = m_cameras.find(name);
		return it != m_cameras.end() ? it->second : NULL;
    }


    Camera* DisplayItemManager::getCamera(unsigned long pkey)
    {
        std::map<unsigned long, Camera*>::iterator it = m_camerasByPkey.find(pkey);
		return it != m_camerasByPkey.end() ? it->second : NULL;
    }

    Camera* DisplayItemManager::getCameraByAddress(const std::string& address)
    {
        std::map<std::string, Camera*>::iterator it = m_camerasByAddress.find(address);
		return it != m_camerasByAddress.end() ? it->second : NULL;
    }

    Sequence* DisplayItemManager::getSequence(std::string name)
    {
        std::map<std::string, Sequence*>::iterator it = m_sequences.find(name);
		return it != m_sequences.end() ? it->second : NULL;
    }


    Sequence* DisplayItemManager::getSequence(unsigned long pkey)
    {
        std::map<unsigned long, Sequence*>::iterator it = m_sequencesByPkey.find(pkey);
		return it != m_sequencesByPkey.end() ? it->second : NULL;
    }

    VideoInput* DisplayItemManager::getInput(std::string name)
    {
		VideoInput* input = NULL;
		return ((input = getCamera(name)) != NULL
			    || (input = getSequence(name)) != NULL) ? input : NULL;
    }


    VideoInput* DisplayItemManager::getInput(unsigned long pkey)
    {
        VideoInput* input = getCamera(pkey);
		return ((input = getCamera(pkey)) != NULL
			    || (input = getSequence(pkey)) != NULL) ? input : NULL;
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
        std::map<std::string, Monitor*>::iterator it = m_monitors.find(name);
		return it != m_monitors.end() ? it->second : NULL;
    }


    Monitor* DisplayItemManager::getMonitor(unsigned long pkey)
    {
        std::map<unsigned long, Monitor*>::iterator it = m_monitorsByPkey.find(pkey);
		return it != m_monitorsByPkey.end() ? it->second : NULL;
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
		// Delete monitors
		BOOST_FOREACH(NameToMonitorMap::value_type& i, m_monitors)
		{
			delete i.second;
		}
        m_monitors.clear();
        m_monitorsByPkey.clear();
		// Delete sequences
		BOOST_FOREACH(NameToSequenceMap::value_type& i, m_sequences)
		{
			delete i.second;
		}
        m_sequences.clear();
        m_sequencesByPkey.clear();
		// Delete cameras;
		BOOST_FOREACH(NameToCameraMap::value_type& i, m_cameras)
		{
			delete i.second;
		}
        m_cameras.clear();
        m_camerasByPkey.clear();
        m_camerasByAddress.clear();
    }


    void DisplayItemManager::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        Sequence* sequence = getSequence(updateEvent.getKey());
        if(sequence != NULL)
        {
            if(updateEvent.getModifications() == TA_Base_Core::Update)
            {
                //SwitchAgentCommunicator::getInstance().updateSequence(sequence);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Adding/Modifying sequences online is not supported.");
            }
            notifyGUIofDisplayItemUpdate(sequence);
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				        "Online Update message received for unrecognised sequence %d", updateEvent.getKey());
        }
    }


    void DisplayItemManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        // Extract the stateUpdateInfo from the State Update Message
        TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
        TA_Base_Bus::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
		const char* displayParameter = 0;
        if(0 != (message.messageState >>= videoOutputState))
        {
            unsigned long outputKey = message.assocEntityKey;
            processVideoOutputStateMessage(outputKey, *videoOutputState);
        }
        else if(0 != (message.messageState >>= sequenceConfig))
        {
            unsigned long sequenceKey = message.assocEntityKey;
            processSequenceConfigMessage(sequenceKey, *sequenceConfig);
        }
		else if(m_localConsoleKey == message.assocEntityKey && 0 != (message.messageState >>= displayParameter))
		{
			TA_Base_Core::RunParams::getInstance().set(RPARAM_DISPLAYMONITOR, displayParameter);
		}
    }


	bool DisplayItemManager::isVirtualMonitor(VideoOutput* output)
	{
		Monitor* monitor = dynamic_cast<Monitor*>(output);
		return (monitor->isVirtualQuadMonitor() || m_localMonitor == monitor);
	}

    void DisplayItemManager::processVideoOutputStateMessage(unsigned long outputKey, const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& videoOutputState)
    {
        VideoOutput* output = getOutput(outputKey);
		if(isVirtualMonitor(output))
		{
			return;
		}
        if(output != NULL)
        {
            output->assignInput(getInput(videoOutputState.currentVideoInputKey));
            notifyGUIofDisplayItemUpdate(output);
        }
        else
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				        "Update message received for unrecognised video output %d", outputKey);
        }
    }


    void DisplayItemManager::processSequenceConfigMessage(unsigned long sequenceKey, const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig)
    {
        // get the monitor
        Sequence* sequence = getSequence(sequenceKey);
        if(sequence != NULL)
        {
            sequence->setDwellTime(sequenceConfig.dwellTime);
            std::list<Camera*> cameras;
            for(unsigned int i = 0; i < sequenceConfig.inputKeySequence.length(); ++i)
            {
                // get the camera by the key
                Camera* cam = getCamera(sequenceConfig.inputKeySequence[i]);
                if(cam != NULL)
                {
                    cameras.push_back(cam);
                }
                else
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                                "Could not load camera with key %d for sequence %s",
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
        if(findIter != list.end())
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
		BOOST_FOREACH(NameToMonitorMap::value_type& i, m_monitors)
        {
            if(i.second->getInput() == modifiedItem)
            {
                notifyMonitorListeners(i.second);
            }
        }
    }

    void DisplayItemManager::notifyCameraListeners(Camera* modifiedItem)
    {
        postMessageToWindows(cameraListeners, WM_CAMERA_UPDATED, modifiedItem);
        // check monitors for camera assignments
		BOOST_FOREACH(NameToMonitorMap::value_type& i, m_monitors)
        {
            if(i.second->getInput() == modifiedItem)
            {
                notifyMonitorListeners(i.second);
            }
        }
        // check sequences for assignments
		BOOST_FOREACH(NameToSequenceMap::value_type& i, m_sequences)
        {
            std::list<Camera*> cameras = i.second->getCameras();
			BOOST_FOREACH(Camera* j, cameras)
            {
                if(j == modifiedItem)
                {
                    notifySequenceListeners(i.second);
                }
            }
        }
    }


    void DisplayItemManager::postMessageToWindows(const std::list<CWnd*>& windows, int message, DisplayItem* itemPtr)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);
		BOOST_FOREACH(CWnd* i, windows)
        {
			TA_ASSERT(i != NULL, "");
            i->PostMessage(message, NULL, reinterpret_cast<LPARAM>(itemPtr));
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
            Camera* camera = NULL;
            Sequence* seq = NULL;
			if((camera = dynamic_cast<Camera*>(input)) != NULL)
            {
                notifyCameraListeners(camera);
            }
			else if((seq = dynamic_cast<Sequence*>(input)) != NULL)
            {
                notifySequenceListeners(seq);
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

    void DisplayItemManager ::loadStations()
    {
		m_stations = m_videoDisplayManagerDAI->getAllLocations();
    }

    void DisplayItemManager::getGraphicfullPath(unsigned long stationId , std::string& fullPath, unsigned long level)
    {
		TA_ASSERT(m_cctvSubSystemKey != -1, "Should init cctv subsystemkey value first");
		UINT64 key = stationId;
		key <<= 32;
		key += level;
		if(m_schematicPathCache.find(key) == m_schematicPathCache.end())
		{
		    try
		    {
		        fullPath = TA_Base_Core::SchematicDisplayMapAccessFactory::getInstance().getSchematicDisplay( stationId, m_cctvSubSystemKey, level);
				m_schematicPathCache.insert(std::make_pair(key, fullPath));
		    }
		    catch ( TA_Base_Core::DataException& expWhat )
		    {
		        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", expWhat.what());
		    }
		    catch (TA_Base_Core::DatabaseException& expWhat)
		    {
		        LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", expWhat.what());
		    }
		}
		else
		{
			fullPath = m_schematicPathCache[key];
		}
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "the full path of diagram is %s " , fullPath.c_str());
    }

    void DisplayItemManager::initGraphicFullPath()
    {
        //TODO
    }

    void DisplayItemManager::setVideoWallMonitorKeys(std::vector<unsigned long> keys)
    {
        m_videoWallMonitorKeys = keys;
    }

    std::vector<TA_Base_Core::ILocation*>& DisplayItemManager ::getStations()
    {
        return m_stations;
    }

    void DisplayItemManager::loadStationsFromDatabase()
    {
        loadStations();
        m_callbackWindow->PostMessage(TA_INIT_STATION);
    }

    void DisplayItemManager::getSequenceByLocation( unsigned long locationId , std::vector< VideoInput*>& sequences)
    {
		BOOST_FOREACH(Sequence* i, m_seqVec)
		{
			if(i->getLocationId() == locationId)
			{
				sequences.push_back(i);
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

	Monitor* DisplayItemManager::getLocalMonitor()
	{
		return m_localMonitor;
	}

    void DisplayItemManager::deleteMonitor( Monitor* monitor )
    {
        m_monitors.erase(monitor->getName());
        m_monitorsByPkey.erase(monitor->getKey());
        delete monitor;
    }

    unsigned long DisplayItemManager::getCctvSubystemKey()
    {
        return m_cctvSubSystemKey;
    }

} // TA_IRS_App

