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

#include <algorithm>

#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/BvsStage.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/DisplayItemTasks.h"

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
#include "core/threads/src/ThreadedWorker.h"

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
typedef TA_IRS_App::SwitchAgentCommunicator::BVSStageToNamedObject BVSStageToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::MonitorToNamedObject MonitorToNamedObject;
typedef TA_IRS_App::SwitchAgentCommunicator::OutputToNamedObject OutputToNamedObject;

using CORBA::String_var;


namespace TA_IRS_App
{

	static const std::string OCC_WALL_MONITOR = "WALL";
	static const std::string OCC_ODS_MONITOR = "ODS";

	DisplayItemManager* DisplayItemManager::m_theInstance = NULL;
    TA_Base_Core::ReEntrantThreadLockable DisplayItemManager::m_singletonLock;


    DisplayItemManager& DisplayItemManager::getInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance == NULL )
        {
            m_theInstance = new DisplayItemManager();
        }

        return *m_theInstance;
    }

	
    void DisplayItemManager::removeInstance()
    {
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance != NULL )
        {
            delete m_theInstance;
            m_theInstance = NULL;
        }
    }


    DisplayItemManager::DisplayItemManager()
      : m_cameras(),
        m_camerasByPkey(),
        m_camerasByAddress(),
        m_quads(),
        m_quadsByPkey(),
        m_sequences(),
        m_sequencesByPkey(),
        m_bvsStages(),
        m_bvsStagesByPkey(),
        m_bvsStagesByLocation(),
        m_bvsStagesByStageId(),
        m_monitors(),
        m_monitorsByPkey(),
		m_locationkey(),
		m_locationType(),
		m_isConfigItems( false ),
        m_callbackWindow( NULL )
    {
    }


    DisplayItemManager::~DisplayItemManager()
    {
        terminateAndWait();

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
        deleteAllItems();
    }


	void DisplayItemManager::initialiseDisplayItems( CWnd* callbackWindow )
	{
        m_callbackWindow = callbackWindow;

        start();
    }

    
    void DisplayItemManager::run()
    {
        if ( true == m_isConfigItems )
		{
			return;
		}

        TA_Base_Core::IConsole* console;

		try
        {
            // Determine the local agent for the physical Location of this Console Session.
            std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
            console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
			m_locationkey = console->getLocation();

		    TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( m_locationkey );
			m_locationType = location->getLocationType();
			
            delete console;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            delete console;

			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << e.what();
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }
        catch( const TA_Base_Core::DataException& e )
        {
            delete console;
			
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << e.what();
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }

        // just in case
        deleteAllItems();

        try
        {
            loadItems();
        }
        catch( ... )
        {
        }

        // fallback in case agent isnt running
        if ( ( 0 == m_cameras.size() ) &&
             ( 0 == m_quads.size() ) &&
             ( 0 == m_sequences.size() ) &&
             ( 0 == m_bvsStages.size() ) &&
             ( 0 == m_monitors.size() ) )
        {
            loadItemsFromDatabase();
        }
		

        LOG_GENERIC( SourceInfo, DebugUtil::DebugNorm,
                     "Loaded %d Cameras, %d Quads, %d Sequences, %d Stages, %d Monitors",
                     m_cameras.size(),
                     m_quads.size(),
                     m_sequences.size(),
                     m_bvsStages.size(),
                     m_monitors.size() );

		// subscribes for comms messages and online updates, 
		subscribeToMessages();

        m_isConfigItems = true;

        m_callbackWindow->PostMessage( TA_INIT_CCTV );
	}


    void DisplayItemManager::terminate()
    {
    }


	void DisplayItemManager::loadItems()
	{
		// load cameras from database first
		loadCamerasFromDatabase();

        // the next 3 can be done in parallel
        // create worker threads and start them
        TA_Base_Core::ThreadedWorker quadWorker( *this );
        quadWorker.start();
        
        TA_Base_Core::ThreadedWorker sequenceWorker( *this );
        sequenceWorker.start();
        
        TA_Base_Core::ThreadedWorker stageWorker( *this );
        stageWorker.start();

        // create the barrier
        TA_Base_Core::SingleThreadBarrier barrier( 3 );

        TA_Base_Core::IWorkItemPtr quadTask( new LoadQuadFromAgentTask( *this, barrier ) );
        quadWorker.executeWorkItem( quadTask );
        
        TA_Base_Core::IWorkItemPtr sequenceTask( new LoadSequenceFromAgentTask( *this, barrier ) );
        sequenceWorker.executeWorkItem( sequenceTask );
        
        TA_Base_Core::IWorkItemPtr stageTask( new LoadStageFromAgentTask( *this, barrier ) );
        stageWorker.executeWorkItem( stageTask );

        // wait on the barrier
        barrier.wait();

        // stop all the threads
        quadWorker.terminateAndWait();
        sequenceWorker.terminateAndWait();
        stageWorker.terminateAndWait();

        // all prerequisites done, load monitors
        loadMonitorsFromAgent();
	}

	void DisplayItemManager::loadItemsFromAgent()
	{
        // first before anything else the cameras are needed
        loadCamerasFromAgent();
        
        // the next 3 can be done in parallel
        // create worker threads and start them
        TA_Base_Core::ThreadedWorker quadWorker( *this );
        quadWorker.start();
        
        TA_Base_Core::ThreadedWorker sequenceWorker( *this );
        sequenceWorker.start();
        
        TA_Base_Core::ThreadedWorker stageWorker( *this );
        stageWorker.start();

        // create the barrier
        TA_Base_Core::SingleThreadBarrier barrier( 3 );

        TA_Base_Core::IWorkItemPtr quadTask( new LoadQuadFromAgentTask( *this, barrier ) );
        quadWorker.executeWorkItem( quadTask );
        
        TA_Base_Core::IWorkItemPtr sequenceTask( new LoadSequenceFromAgentTask( *this, barrier ) );
        sequenceWorker.executeWorkItem( sequenceTask );
        
        TA_Base_Core::IWorkItemPtr stageTask( new LoadStageFromAgentTask( *this, barrier ) );
        stageWorker.executeWorkItem( stageTask );

        // wait on the barrier
        barrier.wait();

        // stop all the threads
        quadWorker.terminateAndWait();
        sequenceWorker.terminateAndWait();
        stageWorker.terminateAndWait();

        // all prerequisites done, load monitors
        loadMonitorsFromAgent();
	}


    void DisplayItemManager::loadItemsFromDatabase()
    {
        // everything here can be done in parallel

        // create 5 worker threads and start them
        TA_Base_Core::ThreadedWorker cameraWorker( *this );
        cameraWorker.start();

        TA_Base_Core::ThreadedWorker quadWorker( *this );
        quadWorker.start();
        
        TA_Base_Core::ThreadedWorker sequenceWorker( *this );
        sequenceWorker.start();
        
        TA_Base_Core::ThreadedWorker stageWorker( *this );
        stageWorker.start();
        
        TA_Base_Core::ThreadedWorker monitorWorker( *this );
        monitorWorker.start();
        
        // create the barrier
        TA_Base_Core::SingleThreadBarrier barrier( 5 );

        // assign work to all the threads
        TA_Base_Core::IWorkItemPtr cameraTask( new LoadCameraTask( *this, barrier ) );
        cameraWorker.executeWorkItem( cameraTask );
        
        TA_Base_Core::IWorkItemPtr quadTask( new LoadQuadTask( *this, barrier ) );
        quadWorker.executeWorkItem( quadTask );
        
        TA_Base_Core::IWorkItemPtr sequenceTask( new LoadSequenceTask( *this, barrier ) );
        sequenceWorker.executeWorkItem( sequenceTask );
        
        TA_Base_Core::IWorkItemPtr stageTask( new LoadStageTask( *this, barrier ) );
        stageWorker.executeWorkItem( stageTask );
        
        TA_Base_Core::IWorkItemPtr monitorTask( new LoadMonitorTask( *this, barrier ) );
        monitorWorker.executeWorkItem( monitorTask );

        // wait on the barrier
        barrier.wait();

        // stop all the threads
        cameraWorker.terminateAndWait();
        quadWorker.terminateAndWait();
        sequenceWorker.terminateAndWait();
        stageWorker.terminateAndWait();
        monitorWorker.terminateAndWait();
    }

    
    void DisplayItemManager::loadCamerasFromAgent()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading cameras from agent" );

        time_t startTime = time( NULL );

        try
        {
            CamToNamedObject cameras = SwitchAgentCommunicator::getInstance().getCameraObjects();

            // find the quad in m_quads, which already assigned from db when manager start
			for ( CamToNamedObject::iterator namedObjectIter = cameras.begin();
                  cameras.end() != namedObjectIter; ++namedObjectIter)
            {
                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
				std::string name;
                CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin
                std::string group;
                CORBA_CALL_RETURN( group, ( *( namedObjectIter->second ) ), getVideoInputGroupName, () );//limin
                std::string address;
                CORBA_CALL_RETURN( address, ( *( namedObjectIter->second ) ), getAddress, () );//limin

                // create the local object
                Camera* newCamera = createCamera( key, name, desc, group, address );
            }

            m_callbackWindow->PostMessage( TA_INIT_CAMERAS );
        }
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while loading cameras: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading cameras from agent. Elapsed time %d seconds",
                     elapsedTime );
    }

	
    void DisplayItemManager::loadQuadsFromAgent()
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading quads from agent" );

        time_t startTime = time( NULL );

        try
        {
            QuadToNamedObject quads = SwitchAgentCommunicator::getInstance().getQuadObjects();

            // find the quad in m_quads, which already assigned from db when manager start
			for ( QuadToNamedObject::iterator namedObjectIter = quads.begin();
                  quads.end() != namedObjectIter; ++namedObjectIter)
            {

                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
                std::string name;
                CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin
                std::string group;
                CORBA_CALL_RETURN( group, ( *( namedObjectIter->second ) ), getVideoInputGroupName, () );//limin
                std::string address;
                CORBA_CALL_RETURN( address, ( *( namedObjectIter->second ) ), getAddress, () );//limin

                // create the local object
                Quad* newQuad = createQuad( key, name, desc, group );

                TA_Base_Bus::QuadCorbaDef::QuadState state;
                CORBA_CALL_RETURN( state, ( *( namedObjectIter->second ) ), getQuadState, () );//limin
				newQuad->setTopLeft( getCamera(state.topLeftVideoInputKey) );
				newQuad->setTopRight( getCamera(state.topRightVideoInputKey) );
				newQuad->setBottomLeft( getCamera(state.bottomLeftVideoInputKey) );
				newQuad->setBottomRight( getCamera(state.bottomRightVideoInputKey) );
            }

            m_callbackWindow->PostMessage( TA_INIT_QUADS );
        }
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while loading quads: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading quads from agent. Elapsed time %d seconds",
                     elapsedTime );
	}


	void DisplayItemManager::loadSequencesFromAgent()
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading sequences from agent" );

        time_t startTime = time( NULL );

        try
        {
            SequenceToNamedObject sequences = SwitchAgentCommunicator::getInstance().getSequenceObjects();

            // find the quad in m_quads, which already assigned from db when manager start
            for ( SequenceToNamedObject::iterator namedObjectIter = sequences.begin();
                  sequences.end() != namedObjectIter; ++namedObjectIter)
            {
                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
				std::string name;
                CORBA_CALL_RETURN( name, ( *(namedObjectIter->second) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin
                std::string group;
                CORBA_CALL_RETURN( group, ( *( namedObjectIter->second ) ), getVideoInputGroupName, () );//limin
                bool readOnly;
                CORBA_CALL_RETURN( readOnly, ( * ( namedObjectIter->second ) ), isReadOnly, () );//limin
                std::string sequenceDescription;
                CORBA_CALL_RETURN( sequenceDescription, ( *( namedObjectIter->second ) ), getSequenceDescription, () );//limin

                Sequence* newSequence = createSequence( key, name, desc, group, readOnly, sequenceDescription );

                // set dwell time
				TA_Base_Bus::SequenceCorbaDef::SequenceConfig_var config;
                CORBA_CALL_RETURN( config, ( *( namedObjectIter->second ) ), getSequenceConfig, () );//limin
				newSequence->setDwellTime(config->dwellTime);
				
				// set camera sequence
				std::list<Camera*> cameras;
				for (unsigned int i = 0; i < config->inputKeySequence.length(); i++)
				{
					// get the camera by the key
					Camera* cam = getCamera(config->inputKeySequence[i]);
					if (cam != NULL)
					{
						cameras.push_back(cam);
					}
					else
					{
                        std::string name;
                        CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Could not load camera with key %d for sequence %s",
                            config->inputKeySequence[i], name.c_str() );//TD14337 yezh++
					}
				}
				newSequence->setCameras(cameras);
			}// end of sequences

            m_callbackWindow->PostMessage( TA_INIT_SEQUENCES );
		}
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while loading sequences: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading sequences from agent. Elapsed time %d seconds",
                     elapsedTime );
	}


	void DisplayItemManager::loadStagesFromAgent()
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading stages from agent" );

        time_t startTime = time( NULL );

        try
        {
            // load the location/cctv zone mapping
            TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap locationToCctvZoneMap;
            TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap cctvZoneToLocationMap;

            // load the configuration
            TA_IRS_Bus::AtsZoneUtils::getCctvConfiguration( locationToCctvZoneMap, cctvZoneToLocationMap );

            BVSStageToNamedObject stages = SwitchAgentCommunicator::getInstance().getStageObjects();

            // find the quad in m_quads, which already assigned from db when manager start
            for ( BVSStageToNamedObject::iterator namedObjectIter = stages.begin();
                  stages.end() != namedObjectIter; ++namedObjectIter)
            {
                // get all details
                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
                std::string name;
                CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin
                std::string group;
                CORBA_CALL_RETURN( group, ( *( namedObjectIter->second ) ), getVideoInputGroupName, () );//limin
                std::string address;
                CORBA_CALL_RETURN( address, ( *( namedObjectIter->second ) ), getAddress, () );//limin

                BvsStage* newStage = createStage( key, name, desc, group, address, cctvZoneToLocationMap );

                TA_Base_Bus::BVSStageCorbaDef::BVSStageState state;//limin
                CORBA_CALL_RETURN( state, ( *( namedObjectIter->second ) ), getBVSStageState, () );//limin

				newStage->setActiveTrain( state.activeTrainID );
				newStage->setPreConflict( state.preConflict );
				newStage->setNumActiveMonitors( state.numActiveMonitors );
				
                if(state.activeTrainID == 0)
				{
					newStage->setActiveVideoSource( BvsStage::None, 0, 0, 0, 0 );
				}
				else
				{
					switch( state.switchMode )
					{
					    case TA_Base_Bus::BVSStageCorbaDef::Camera:
						    newStage->setActiveVideoSource( BvsStage::Single, state.item1, 0, 0, 0 );
						    break;

					    case TA_Base_Bus::BVSStageCorbaDef::Seq:
						    newStage->setActiveVideoSource( BvsStage::Sequence, state.item1, 0, 0, 0 );
						    break;

					    case TA_Base_Bus::BVSStageCorbaDef::Quad:
						    newStage->setActiveVideoSource( BvsStage::Quad, state.item1, state.item2, state.item3, state.item4 );
						    break;
					}
				}

            } // end of stages

            // for each location, need to map to a bvs stage
            for ( TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap::iterator locationToCctvZoneMapIter = locationToCctvZoneMap.begin();
                  locationToCctvZoneMap.end() != locationToCctvZoneMapIter; ++locationToCctvZoneMapIter )
            {
                // find the BVS stage for the given zone
                std::map<unsigned long, BvsStage*>::iterator findIter = m_bvsStagesByStageId.find( locationToCctvZoneMapIter->second );

                if ( m_bvsStagesByStageId.end() != findIter)
                {
                    m_bvsStagesByLocation[ locationToCctvZoneMapIter->first ] = findIter->second;
                }
            }

            m_callbackWindow->PostMessage( TA_INIT_STAGES );
		}
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
	        std::string exceptionMsg( "Exception thrown while loading quads: " );
		        exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading stages from agent. Elapsed time %d seconds",
                     elapsedTime );
    }


    void DisplayItemManager::loadMonitorsFromAgent()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Initialising monitor state" );

        time_t startTime = time( NULL );

        try
        {
            MonitorToNamedObject consoleMonitors = SwitchAgentCommunicator::getInstance().getConsoleMonitorObjects();

            for ( MonitorToNamedObject::iterator namedObjectIter = consoleMonitors.begin();
                  consoleMonitors.end() != namedObjectIter; ++namedObjectIter )
            {
                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
                std::string name;
                CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin

                Monitor* newMonitor = createMonitor( key, name, desc, true );

                TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState_var outputState;
                CORBA_CALL_RETURN( outputState, ( *( namedObjectIter->second ) ), getVideoOutputState, () );

                if ( outputState->isInAlarmMode )
                {
                    // get the alarm stack
                    newMonitor->setAlarmStackItems( outputState->alarmStack );
                }
                else
                {
                    // get the input
                    // can be a sequence, camera, quad etc
                    newMonitor->assignInput( getInput( outputState->currentVideoInputKey ) );
                }
            }
        }
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while loading quads: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        // go through the common monitors
        try
        {
            OutputToNamedObject commonMonitors = SwitchAgentCommunicator::getInstance().getCommonMonitorObjects();

            for ( OutputToNamedObject::iterator namedObjectIter = commonMonitors.begin();
                  commonMonitors.end() != namedObjectIter; ++namedObjectIter )
            {
                unsigned long key;
                CORBA_CALL_RETURN( key, ( *( namedObjectIter->second ) ), getKey, () );//limin
                std::string name;
                CORBA_CALL_RETURN( name, ( *( namedObjectIter->second ) ), getName, () );//limin
                std::string desc;
                CORBA_CALL_RETURN( desc, ( *( namedObjectIter->second ) ), getDescription, () );//limin

                Monitor* newMonitor = createMonitor( key, name, desc, false );

                TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState_var outputState;
                CORBA_CALL_RETURN( outputState, ( *( namedObjectIter->second ) ), getVideoOutputState, () );//limin
			
                if ( outputState->isInAlarmMode )
                {
                    // get the alarm stack
                    newMonitor->setAlarmStackItems( outputState->alarmStack );
                }
                else
                {
                    // get the input
                    // can be a sequence, camera, quad etc
                    newMonitor->assignInput( getInput( outputState->currentVideoInputKey ) );
                }
            }

            m_callbackWindow->PostMessage( TA_INIT_MONITORS );
        }
        catch (TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while loading quads: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished initialising monitor state. Elapsed time %d seconds",
                     elapsedTime );
    }



    void DisplayItemManager::loadCamerasFromDatabase()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading cameras" );

        time_t startTime = time( NULL );

        try
        {
			IEntityDataList cameraEntities	= EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Camera::getStaticType(), m_locationkey);

			IEntityDataList::iterator it;
			
			for (it = cameraEntities.begin(); it != cameraEntities.end(); it++)
            {
				TA_Base_Core::Camera* cameraEntity = dynamic_cast<TA_Base_Core::Camera*>(*it); 
				
				// get all details
                unsigned long key = cameraEntity->getKey();
				std::string name = cameraEntity->getName();
                std::string desc = cameraEntity->getDescription();
                std::string group = cameraEntity->getVideoInputGroupName();
                std::string address = cameraEntity->getAddress();

                delete cameraEntity;

                createCamera( key, name, desc, group, address );
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading cameras. Elapsed time %d seconds",
                     elapsedTime );

        m_callbackWindow->PostMessage( TA_INIT_CAMERAS );
    }


    void DisplayItemManager::loadQuadsFromDatabase()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading quads" );

        time_t startTime = time( NULL );

        try
        {
			IEntityDataList quadEntities;
			quadEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Quad::getStaticType(), m_locationkey);

			IEntityDataList::iterator it;

            for (it = quadEntities.begin(); it != quadEntities.end(); it++)
            {
				TA_Base_Core::Quad* quadEntity = dynamic_cast<TA_Base_Core::Quad*>(*it); 

                // get all details
                unsigned long key = quadEntity->getKey();
				std::string name = quadEntity->getName();
                std::string desc = quadEntity->getDescription();
                std::string group = quadEntity->getVideoInputGroupName();

                delete quadEntity;

                // create the local object
                createQuad( key, name, desc, group );
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading quads. Elapsed time %d seconds",
                     elapsedTime );

        m_callbackWindow->PostMessage( TA_INIT_QUADS );
    }


    void DisplayItemManager::loadSequencesFromDatabase()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading sequences" );

        time_t startTime = time( NULL );

        try
        {
			IEntityDataList sequenceEntities;
			sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), m_locationkey);

			IEntityDataList::iterator it;

            for (it = sequenceEntities.begin(); it != sequenceEntities.end(); it++)
            {
				TA_Base_Core::Sequence* sequenceEntity = dynamic_cast<TA_Base_Core::Sequence*>(*it); 

                // get all details
                unsigned long key = sequenceEntity->getKey();
				std::string name = sequenceEntity->getName();
                std::string desc = sequenceEntity->getDescription();
                std::string group = sequenceEntity->getVideoInputGroupName();
                bool readOnly = sequenceEntity->isReadOnly();
                std::string sequenceDescription = sequenceEntity->getSequenceDescription();

                delete sequenceEntity;

                createSequence( key, name, desc, group, readOnly, sequenceDescription );
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading sequences. Elapsed time %d seconds",
                     elapsedTime );

        m_callbackWindow->PostMessage( TA_INIT_SEQUENCES );
    }


    void DisplayItemManager::loadStagesFromDatabase()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading stages" );

        time_t startTime = time( NULL );

        try
        {
            // load the location/cctv zone mapping
            TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap locationToCctvZoneMap;
            TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap cctvZoneToLocationMap;

            // load the configuration
            TA_IRS_Bus::AtsZoneUtils::getCctvConfiguration( locationToCctvZoneMap, cctvZoneToLocationMap );


			IEntityDataList stageEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::BVSStage::getStaticType(), m_locationkey);
			IEntityDataList::iterator it;

            for (it = stageEntities.begin(); it != stageEntities.end(); it++)
            {
				TA_Base_Core::BVSStage* stageEntity = dynamic_cast<TA_Base_Core::BVSStage*>(*it); 

                // get all details
                unsigned long key = stageEntity->getKey();
				std::string name = stageEntity->getName();
                std::string desc = stageEntity->getDescription();
                std::string group = stageEntity->getVideoInputGroupName();
                std::string address = stageEntity->getAddress();

                delete stageEntity;

                createStage( key, name, desc, group, address, cctvZoneToLocationMap );
            }

            // for each location, need to map to a bvs stage
            for ( TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap::iterator locationToCctvZoneMapIter = locationToCctvZoneMap.begin();
            locationToCctvZoneMap.end() != locationToCctvZoneMapIter; ++locationToCctvZoneMapIter )
            {
                // find the BVS stage for the given zone
                std::map<unsigned long, BvsStage*>::iterator findIter = m_bvsStagesByStageId.find( locationToCctvZoneMapIter->second );

                if ( m_bvsStagesByStageId.end() != findIter)
                {
                    m_bvsStagesByLocation[ locationToCctvZoneMapIter->first ] = findIter->second;
                }
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading stages. Elapsed time %d seconds",
                     elapsedTime );

        m_callbackWindow->PostMessage( TA_INIT_STAGES );
    }


    void DisplayItemManager::loadMonitorsFromDatabase()
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loading monitors" );

        time_t startTime = time( NULL );

        try
        {
			IEntityDataList monitorEntities;
			monitorEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), m_locationkey);
			IEntityDataList::iterator it;
		    //get local console key
            TA_Base_Core::IConsole* console = 0;
            unsigned long consoleKey( 0 );
            try
            {
	            // Determine the local agent for the physical Location of this Console Session.
	            std::string sessionID = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );
	            console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
	            consoleKey = console->getKey();
	            delete console;
	            console = 0;
            }
	        catch( const TA_Base_Core::DatabaseException& e )
	        {
	            delete console;
	            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
	            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", 
	                "The console name could not be determined." );
	        }
	        catch( const TA_Base_Core::DataException& e )
	        {
	            delete console;
	            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
	            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", 
	                "The Corba name for the Video Switch Agent could not be determined." );
	        }
            for (it = monitorEntities.begin(); it != monitorEntities.end(); it++)
            {
				TA_Base_Core::VideoMonitor* monitorEntity = dynamic_cast<TA_Base_Core::VideoMonitor*>(*it); 

                // get all details
                unsigned long key = monitorEntity->getKey();
				std::string name = monitorEntity->getName();
                std::string desc = monitorEntity->getDescription();
				
                bool isConsoleMonitor = ( consoleKey != 0 ) && ( consoleKey == monitorEntity->getConsoleKey() );

                delete monitorEntity;

                createMonitor( key, name, desc, isConsoleMonitor );
            }
        }
        catch (const TA_Base_Core::DatabaseException& e)
        {
            // object resolution etc
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what() );
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what() );
        }

        time_t elapsedTime = time( NULL ) - startTime;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Finished loading monitors. Elapsed time %d seconds",
                     elapsedTime );

        m_callbackWindow->PostMessage( TA_INIT_MONITORS );
    }


    Camera* DisplayItemManager::createCamera( unsigned long key, 
                                              const std::string& name,
                                              const std::string& description,
                                              const std::string& group,
                                              const std::string& address )
    {
        // create the local object
        Camera* camPtr = new Camera( key, name, description, address, group );

        // add it to the maps
        m_cameras[name] = camPtr;
        m_camerasByPkey[key] = camPtr;
        m_camerasByAddress[address] = camPtr;

        return camPtr;
    }
    
    
    Quad* DisplayItemManager::createQuad( unsigned long key, 
                                          const std::string& name,
                                          const std::string& description,
                                          const std::string& group )
    {
        // create the local object
        Quad* quadPtr = new Quad( key, name, description, group);

        // add it to the maps
        m_quads[name] = quadPtr;
        m_quadsByPkey[key] = quadPtr;

        return quadPtr;
    }


    Sequence* DisplayItemManager::createSequence( unsigned long key,
                                                  const std::string& name,
                                                  const std::string& description,
                                                  const std::string& group,
                                                  bool readOnly,
                                                  const std::string& sequenceDescription )
    {
        // create the local object
        Sequence* seqPtr = new Sequence( key, name, description, group );
        
        seqPtr->setReadOnly( readOnly );
		seqPtr->setSequenceDescription(sequenceDescription);

        // add it to the maps
        m_sequences[name] = seqPtr;
        m_sequencesByPkey[key] = seqPtr;

		return seqPtr;
    }


    BvsStage* DisplayItemManager::createStage( unsigned long key, 
                                               const std::string& name,
                                               const std::string& description,
                                               const std::string& group,
                                               const std::string& address,
                                               const TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap& cctvZoneToLocationMap )
    {
        unsigned long stageId = 0;
        std::istringstream conversionStream( address );
        conversionStream >> stageId;
        TA_ASSERT ( 0 != stageId, "Invalid BVS Stage configuration, address is not a stage ID" );

		// find the locations this stage covers
        TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap::const_iterator cctvZoneToLocationMapIter = cctvZoneToLocationMap.find( stageId );

        std::set<unsigned long> locations;

        if ( cctvZoneToLocationMap.end() != cctvZoneToLocationMapIter )
        {
            locations = cctvZoneToLocationMapIter->second;
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Incorrectly configured stage %s, covers no locations",
                         name.c_str() );
        }

        // create the local object
        BvsStage* stagePtr = new BvsStage( key, stageId, name, description, group, locations );

        // add it to the maps
        m_bvsStages[name] = stagePtr;
        m_bvsStagesByPkey[key] = stagePtr;
        m_bvsStagesByStageId[stageId] = stagePtr;

        return stagePtr;
    }


    Monitor* DisplayItemManager::createMonitor( unsigned long key, 
                                                const std::string& name,
                                                const std::string& description,
                                                bool isConsoleMonitor )
    {
        // create the local object
        Monitor* monPtr = new Monitor( key, name, description, isConsoleMonitor );

        m_monitors[ name ] = monPtr;
        m_monitorsByPkey[ key ] = monPtr;
		
        if ( true == isConsoleMonitor )
        {
            m_consoleMonitors.push_back( monPtr->getName() );
        }

        return monPtr;
    }

    
	void DisplayItemManager::subscribeToMessages()
    {
        unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str() , NULL, 10 );

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::VisualComms::VideoOutputStateUpdate, 
                                                                                    this, 0, 0, locationKey, NULL);
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::VisualComms::QuadStateUpdate, 
                                                                                    this, 0, 0, locationKey, NULL);
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::VisualComms::SequenceConfigUpdate, 
                                                                                    this, 0, 0, locationKey, NULL);
        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::VisualComms::BvsStageStateUpdate, 
                                                                                    this, 0, 0, locationKey, NULL);

        
        // No subscriptions for TA_Base_Core::VisualComms::RecordingUnitStateUpdate
        // there are no more recording units here
        
        // online update messages for sequence descriptions
        std::vector< unsigned long > sequenceKeys;
        for (std::map<std::string, Sequence*>::iterator iter = m_sequences.begin();
        iter != m_sequences.end(); iter++)
        {
            sequenceKeys.push_back( iter->second->getKey() );
        }
        TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests( TA_Base_Core::ENTITY, *this, sequenceKeys );
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


    Camera* DisplayItemManager::getCameraByAddress(std::string address)
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


    BvsStage* DisplayItemManager::getBvsStage(std::string name)
    {
        // find and return the item in the map
        std::map<std::string, BvsStage*>::iterator findIter = m_bvsStages.find(name);
        if (findIter != m_bvsStages.end())
        {
            return (findIter->second);
        }

        // not found
        return NULL;
    }


    BvsStage* DisplayItemManager::getBvsStage(unsigned long pkey)
    {
        // find and return the item in the map
        std::map<unsigned long, BvsStage*>::iterator findIter = m_bvsStagesByPkey.find(pkey);
        if (findIter != m_bvsStagesByPkey.end())
        {
            return (findIter->second);
        }

        // not found
        return NULL;
    }


    BvsStage* DisplayItemManager::getBvsStageForLocation(unsigned long locationKey)
    {
        // find and return the item in the map
        std::map<unsigned long, BvsStage*>::iterator findIter = m_bvsStagesByLocation.find(locationKey);
        if (findIter != m_bvsStagesByLocation.end())
        {
            return (findIter->second);
        }

        // not found
        return NULL;
    }


    BvsStage* DisplayItemManager::getBvsStageById( unsigned long stageId )
    {
        // find and return the item in the map
        std::map<unsigned long, BvsStage*>::iterator findIter = m_bvsStagesByStageId.find( stageId );
        if ( findIter != m_bvsStagesByStageId.end() )
        {
            return ( findIter->second );
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

        input = getBvsStage(name);
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

        input = getBvsStage(pkey);
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


    std::map<std::string, BvsStage*> DisplayItemManager::getAllStages()
    {
        return m_bvsStages;
    }


    std::map<std::string, Monitor*> DisplayItemManager::getAllMonitors()
    {
        return m_monitors;
    }


    bool DisplayItemManager::isCurrentlyActiveTrain(unsigned char trainId)
    {
        for (std::map<std::string, BvsStage*>::iterator iter = m_bvsStages.begin();
        iter != m_bvsStages.end(); iter++)
        {
            // if an active train id matches this train
            if (iter->second->getActiveTrain() == trainId)
            {
                return true;
            }
        }

        return false;
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

        for (std::map<std::string, BvsStage*>::iterator stageIter = m_bvsStages.begin();
        stageIter != m_bvsStages.end(); stageIter++)
        {
            delete stageIter->second;
            stageIter->second = NULL;
        }
        m_bvsStages.clear();
        m_bvsStagesByPkey.clear();
        m_bvsStagesByLocation.clear();

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


    void DisplayItemManager::processUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        // we only register for updates to sequences
        Sequence* sequence = getSequence(updateEvent.getKey());
        if ( sequence != NULL )
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
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Adding/Modifying sequences online is not supported.");
            }

            // tell the GUI to update
            notifyGUIofDisplayItemUpdate(sequence);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Online Update message received for unrecognised sequence %d",
                        updateEvent.getKey() );
        }
    }


    void DisplayItemManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
	    // Extract the stateUpdateInfo from the State Update Message    
        TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState* videoOutputState = 0;
        if ( 0 != ( message.messageState >>= videoOutputState ) )   // Is this a VideoOutput state update message?
		{
            unsigned long outputKey = message.assocEntityKey;
            processVideoOutputStateMessage( outputKey, *videoOutputState );
            return;
		}

        TA_Base_Bus::QuadCorbaDef::QuadState* quadState = 0;
        if ( 0 != ( message.messageState >>= quadState ) )   // Is this a Quad state update message?
		{
            unsigned long quadKey = message.assocEntityKey;
            processQuadStateMessage( quadKey, *quadState );
            return;
        }

        TA_Base_Bus::SequenceCorbaDef::SequenceConfig* sequenceConfig = 0;
        if ( 0 != ( message.messageState >>= sequenceConfig ) )   // Is this a Sequence Config update message?
		{
            unsigned long sequenceKey = message.assocEntityKey;
            processSequenceConfigMessage( sequenceKey, *sequenceConfig );			
            return;
		}


        TA_Base_Bus::BVSStageCorbaDef::BVSStageState* stageConfig = 0;
        if ( 0 != ( message.messageState >>= stageConfig ) )   // Is this a BVS Stage update message?
		{
            unsigned long stageKey = message.assocEntityKey;
            processStageStateMessage( stageKey, *stageConfig );			
            return;
		}
        
        
        
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
            "Unrecognised state update message received." );
    }


    void DisplayItemManager::processVideoOutputStateMessage( unsigned long outputKey, const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& videoOutputState )
    {
        // get the monitor
        VideoOutput* output = getOutput(outputKey);

        if ( output != NULL )
        {
            if (videoOutputState.isInAlarmMode)
            {
                // get the alarm stack
                output->setAlarmStackItems(videoOutputState.alarmStack);
            }
            else
            {
                // clear the alarm stack
                output->clearAlarmStack();

                // get the input
                // can be a sequence, camera, quad etc
                output->assignInput( getInput(videoOutputState.currentVideoInputKey) );
            }

            // tell the GUI to update
            notifyGUIofDisplayItemUpdate(output);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Update message received for unrecognised video output %d",
                        outputKey );
        }
    }


    void DisplayItemManager::processQuadStateMessage( unsigned long quadKey, const TA_Base_Bus::QuadCorbaDef::QuadState& quadState )
    {
        // get the monitor
        Quad* quad = getQuad(quadKey);

        if ( quad != NULL )
        {
            // set assignments
            quad->setTopLeft( getCamera(quadState.topLeftVideoInputKey) );
            quad->setTopRight( getCamera(quadState.topRightVideoInputKey) );
            quad->setBottomLeft( getCamera(quadState.bottomLeftVideoInputKey) );
            quad->setBottomRight( getCamera(quadState.bottomRightVideoInputKey) );

            // tell the GUI to update
            notifyGUIofDisplayItemUpdate(quad);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Update message received for unrecognised quad %d",
                        quadKey );
        }
    }


    void DisplayItemManager::processSequenceConfigMessage( unsigned long sequenceKey, const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig )
    {
        // get the monitor
        Sequence* sequence = getSequence(sequenceKey);

        if ( sequence != NULL )
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
                                sequenceConfig.inputKeySequence[i], sequence->getName().c_str());//TD14337 yezh++
                }
            }
            sequence->setCameras(cameras);

            // tell the GUI to update
            notifyGUIofDisplayItemUpdate(sequence);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Update message received for unrecognised sequence %d",
                        sequenceKey );
        }
    }


    void DisplayItemManager::processStageStateMessage( unsigned long stageKey, const TA_Base_Bus::BVSStageCorbaDef::BVSStageState& stageConfig )
    {
        // get the stage
        BvsStage* stage = getBvsStage(stageKey);

        if ( stage != NULL )
        {
            stage->setActiveTrain(stageConfig.activeTrainID);
			stage->setPreConflict(stageConfig.preConflict);
			if(stageConfig.activeTrainID == 0)
			{
				stage->setActiveVideoSource(BvsStage::None, 0, 0, 0, 0);
            }
			else
			{
				switch(stageConfig.switchMode)
				{
					case TA_Base_Bus::BVSStageCorbaDef::Camera:
						stage->setActiveVideoSource(BvsStage::Single, stageConfig.item1, 0, 0, 0);
						break;
					case TA_Base_Bus::BVSStageCorbaDef::Seq:
						stage->setActiveVideoSource(BvsStage::Sequence, stageConfig.item1, 0, 0, 0);
						break;
					case TA_Base_Bus::BVSStageCorbaDef::Quad:
						stage->setActiveVideoSource(BvsStage::Quad, stageConfig.item1, stageConfig.item2, stageConfig.item3, stageConfig.item4);
						break;
				}
			}
			stage->setNumActiveMonitors(stageConfig.numActiveMonitors);
			
			// tell the GUI to update
            notifyGUIofDisplayItemUpdate(stage);
        }
        else
        {
            // update received for unknown output device
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Update message received for unrecognised stage %d",
                        stageKey );
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
        std::list<CWnd*>::iterator findIter = std::find( list.begin(),
                                                         list.end(),
                                                         window );

        // if found, delete it
        if ( findIter != list.end() )
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


    void DisplayItemManager::notifyStageListeners(BvsStage* modifiedItem)
    {
        postMessageToWindows(stageListeners, WM_STAGE_UPDATED, modifiedItem);

        // check monitors for sequence assignments
        for (std::map<std::string, Monitor*>::iterator iter = m_monitors.begin();
        iter != m_monitors.end(); iter++)
        {
            if (iter->second->getInput() == modifiedItem)
            {
                notifyMonitorListeners(iter->second);
            }
			else if (iter->second->isInAlarmMode())
			{
				if(iter->second->isBvsStageInAlarmStack(modifiedItem))
				{
					iter->second->updateBvsStageInAlarmStack(modifiedItem);
					notifyMonitorListeners(iter->second);
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
            if ( *iter != NULL )
            {
                (*iter)->PostMessage( message, NULL, reinterpret_cast<LPARAM>(itemPtr) );
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

            BvsStage* stage = dynamic_cast<BvsStage*>(input);
            if (stage != NULL)
            {
                notifyStageListeners(stage);
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

	unsigned long DisplayItemManager::getLocationKey()
	{
		return m_locationkey;
	}


    void DisplayItemManager::workerFree()
    {
    }

} // TA_IRS_App

