/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/SwitchManager.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Manages access to the entities (Cameras, VideoMonitors, 
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * mplements the SwitchManager interface.
  *
  * Design Note: This used to have access to entities by name.  However, names
  * can be changed, so we use entity keys to identify entities instead.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoOutputGroup.h"
#include "app/cctv/video_switch_agent/src/VideoMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/Quad.h"
//#include "app/cctv/video_switch_agent/src/BVSStage.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/RecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDStageModeInterface.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"

#include <algorithm>
#include <sstream>

namespace TA_IRS_App
{
    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable SwitchManager::m_singletonLock;
	SwitchManager* SwitchManager::m_theClassInstance = NULL;


    //
    // getInstance
    //
    SwitchManager& SwitchManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( NULL == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new SwitchManager();
		}

		return *m_theClassInstance;
    }


    //
    // activate
    //
    void SwitchManager::activate()
    {
        TA_ASSERT( AgentModeMonitor::getInstance().isInControlMode(), "The agent is not in Control mode." );
           
        // The SwitchManager handles access to the agent, so activate it using the agent's name.

        activateServantWithName( TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ) );
    }


    ///////////////////////////////////////////////////////////////////
    //
    // SwitchManagerCorbaDef Methods
    //
    ///////////////////////////////////////////////////////////////////

    //
    // getVideoOutputGroups
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputGroupSequence* SwitchManager::getVideoOutputGroups()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputGroupSequence > videoOutputGroups = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputGroupSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputGroupSequence() );
        if ( 0 == videoOutputGroups.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoOutputGroupSequence." );
            return 0;
        }

        videoOutputGroups->length( m_videoOutputGroups.size() );
        
        unsigned int index( 0 );
        VideoOutputGroupMap::iterator it = m_videoOutputGroups.begin();
        for( ; it != m_videoOutputGroups.end(); it++ )
        {
            (*videoOutputGroups)[ index ] = it->second->_this();
            index++;
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoOutputGroups.release();
    }


    //
    // getVideoOutputGroup
    //
    TA_Base_Bus_GZL9::VideoOutputGroupCorbaDef_ptr SwitchManager::getVideoOutputGroup( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        VideoOutputGroupMap::iterator it = m_videoOutputGroups.find( entityKey );
        if ( m_videoOutputGroups.end() == it )
        {
            std::stringstream error;
            error << "The VideoOutputGroup with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getVideoMonitors
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getVideoMonitors()
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"getVideoMonitor." );
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence() );
        if ( 0 == videoMonitors.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoMonitorSequence." );
            return 0;
        }
        videoMonitors->length( m_videoMonitors.size() );

        unsigned int index( 0 );
        VideoMonitorMap::iterator it = m_videoMonitors.begin();
        for( ; it != m_videoMonitors.end(); it++ )
        {
            (*videoMonitors)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoMonitors.release();
    }


    //
    // getVideoMonitor
    //
    TA_Base_Bus_GZL9::VideoMonitorCorbaDef_ptr SwitchManager::getVideoMonitor( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        VideoMonitorMap::iterator it = m_videoMonitors.find( entityKey );
        if ( m_videoMonitors.end() == it )
        {
            std::stringstream error;
            error << "The VideoMonitor with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getAllConsoleMonitors
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getAllConsoleMonitors()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence() );
        if ( 0 == videoMonitors.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoMonitorSequence." );
            return 0;
        }

        unsigned int size( 1 );        
        VideoMonitorMap::iterator it = m_videoMonitors.begin();
        for( ; it != m_videoMonitors.end(); it++ )
        {
            if ( 0 != it->second->getConsoleKey() && it->second->getDisplayOrder() == 0 )   // consoleKey of 0 indicates not console monitor.
            {
                ( *videoMonitors ).length( size );
                ( *videoMonitors )[ (size - 1) ] = it->second->_this();
                size++;
            }
        }
    
        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoMonitors.release();
    }


    //
    // getConsoleMonitors
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence* SwitchManager::getConsoleMonitors( 
        unsigned long consoleKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence > videoMonitors = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence() );
        if ( 0 == videoMonitors.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoMonitorSequence." );
            return 0;
        }

        unsigned int size( 1 );
        VideoMonitorMap::iterator it = m_videoMonitors.begin();
        for( ; it != m_videoMonitors.end(); it++ )
        {
			const unsigned long key = ( *it ).second->getConsoleKey() ;
			const int displayOrder = (*it).second->getDisplayOrder();
            if ( key == consoleKey  && displayOrder == 0)
            {
                videoMonitors->length( size );
                ( *videoMonitors )[ (size - 1) ] = it->second->_this();
                size++;
            }
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoMonitors.release();
    }


    //
    // getRecordingUnits
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence* SwitchManager::getRecordingUnits()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence > recordingUnits = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence() );
        if ( 0 == recordingUnits.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new RecordingUnitSequence." );
            return 0;
        }

        recordingUnits->length( m_recordingUnits.size() );
        unsigned int index( 0 );
        RecordingUnitMap::iterator it = m_recordingUnits.begin();
        for( ; it != m_recordingUnits.end(); it++ )
        {
            (*recordingUnits)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return recordingUnits.release();
    }


    //
    // getConsoleRecordingUnits
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence* SwitchManager::getConsoleRecordingUnits( 
        unsigned long consoleKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence > recordingUnits = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::RecordingUnitSequence() );
        if ( 0 == recordingUnits.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new RecordingUnitSequence." );
            return 0;
        }

        unsigned int size( 1 );
        RecordingUnitMap::iterator it = m_recordingUnits.begin();
        for( ; it != m_recordingUnits.end(); it++ )
        {
            if ( consoleKey == ( *it ).second->getConsoleKey() )
            {
                recordingUnits->length( size );
                ( *recordingUnits )[ (size - 1) ] = it->second->_this();
                size++;
            }
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return recordingUnits.release();
    }


    //
    // getRecordingUnit
    //
    TA_Base_Bus_GZL9::RecordingUnitCorbaDef_ptr SwitchManager::getRecordingUnit( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() == it )
        {
            std::stringstream error;
            error << "The RecordingUnit with the identifier " << entityKey;
            error << " is unknown to the  Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getCameras
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::CameraSequence* SwitchManager::getCameras()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::CameraSequence > cameras = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::CameraSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::CameraSequence() );
        if ( 0 == cameras.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new CameraSequence." );
            return 0;
        }
        cameras->length( m_cameras.size() );

        unsigned int index( 0 );
        CameraMap::iterator it = m_cameras.begin();
        for( ; it != m_cameras.end(); it++ )
        {
            (*cameras)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return cameras.release();
    }


    //
    // getCamera
    //
    TA_Base_Bus_GZL9::CameraCorbaDef_ptr SwitchManager::getCamera( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        CameraMap::iterator it = m_cameras.find( entityKey );
        if ( m_cameras.end() == it )
        {
            std::stringstream error;
            error << "The Camera with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getQuads
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::QuadSequence* SwitchManager::getQuads()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::QuadSequence > quads = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::QuadSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::QuadSequence() );
        if ( 0 == quads.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new QuadSequence." );
            return 0;
        }
        quads->length( m_quads.size() );

        unsigned int index( 0 );
        QuadMap::iterator it = m_quads.begin();
        for( ; it != m_quads.end(); it++ )
        {
            (*quads)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return quads.release();
    }


    //
    // getQuad
    //
    TA_Base_Bus_GZL9::QuadCorbaDef_ptr SwitchManager::getQuad( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() == it )
        {
            std::stringstream error;
            error << "The Quad with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getBVSStages
    //
	/*
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::BVSStageSequence* SwitchManager::getBVSStages()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::BVSStageSequence > bvsStages = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::BVSStageSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::BVSStageSequence() );
        if ( 0 == bvsStages.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new BVSStageSequence." );
            return 0;
        }

        bvsStages->length( m_bvsStages.size() );

        unsigned int index( 0 );
        BVSStageMap::iterator it = m_bvsStages.begin();
        for( ; it != m_bvsStages.end(); it++ )
        {
            (*bvsStages)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return bvsStages.release();
    }*/


    //
    // getBVSStage
    //
  /*  TA_Base_Bus_GZL9::BVSStageCorbaDef_ptr SwitchManager::getBVSStage( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() == it )
        {
            std::stringstream error;
            error << "The BVSStage with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }*/


    //
    // getSequences
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::SequenceSequence* SwitchManager::getSequences()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::SequenceSequence > sequences = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::SequenceSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::SequenceSequence() );
        if ( 0 == sequences.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new SequenceSequence." );
            return 0;
        }
        sequences->length( m_sequences.size() );

        unsigned int index( 0 );
        SequenceMap::iterator it = m_sequences.begin();
        for( ; it != m_sequences.end(); it++ )
        {
            (*sequences)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return sequences.release();
    }


    //
    // getSequence
    //
    TA_Base_Bus_GZL9::SequenceCorbaDef_ptr SwitchManager::getSequence( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() == it )
        {
            std::stringstream error;
            error << "The Sequence with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getVideoOutputs
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputSequence* SwitchManager::getVideoOutputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputSequence > videoOutputs = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoOutputSequence() );
        if ( 0 == videoOutputs.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoOutputSequence." );
            return 0;
        }
        videoOutputs->length( m_videoOutputs.size() );

        unsigned int index( 0 );
        VideoOutputMap::iterator it = m_videoOutputs.begin();
        for( ; it != m_videoOutputs.end(); it++ )
        {
            (*videoOutputs)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoOutputs.release();
    }


    //
    // getVideoOutput
    //
    TA_Base_Bus_GZL9::VideoOutputCorbaDef_ptr SwitchManager::getVideoOutput( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() == it )
        {
            std::stringstream error;
            error << "The VideoOutput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }


    //
    // getVideoInputs
    //
    TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoInputSequence* SwitchManager::getVideoInputs()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // This will be cleaned up by the ORB once we return. If anything unexpected happens
        // in the meantime, we have to clean it up ourselves, so make it an auto_ptr.
 
        std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoInputSequence > videoInputs = 
            std::auto_ptr< TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoInputSequence >(
            new TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoInputSequence() );
        if ( 0 == videoInputs.get() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                "Failed to create new VideoInputSequence." );
            return 0;
        }
        videoInputs->length( m_videoInputs.size() );

        unsigned int index( 0 );
        VideoInputMap::iterator it = m_videoInputs.begin();
        for( ; it != m_videoInputs.end(); it++ )
        {
            (*videoInputs)[index++] = it->second->_this();
        }

        // Release the pointer to the ORB, so it becomes responsible for cleaning up the sequence.

        return videoInputs.release();
    }


    //
    // getVideoInput
    //
    TA_Base_Bus_GZL9::VideoInputCorbaDef_ptr SwitchManager::getVideoInput( unsigned long entityKey )
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        VideoInputMap::iterator it = m_videoInputs.find( entityKey );
        if ( m_videoInputs.end() == it )
        {
            std::stringstream error;
            error << "The VideoInput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        return it->second->_this();
    }

	
	//
	//acknowledgeAlarm
	//
    void SwitchManager::acknowledgeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail, CORBA::ULong consoleId)
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		if(m_switchAlarmInterface != NULL)
		{
			try
			{
				// TD15853 - changed parameters to acknowledgeAlarm()
				std::ostringstream alarmStr;
				alarmStr << "[ACK] Alarm ID = " << pAlarmDetail.alarmID 
					     << " Type = " << pAlarmDetail.messageTypeKey 
						 << " EntitKey = " << pAlarmDetail.assocEntityKey;

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, alarmStr.str().c_str());

				//TD18661
				//std::ostringstream alarmTypeKey;
				//alarmTypeKey << pAlarmDetail.messageTypeKey;
				std::string alarmID = pAlarmDetail.alarmID.in();
				m_switchAlarmInterface->acknowledgeAlarm(alarmID.c_str()/*, pAlarmDetail.assocEntityKey*/, consoleId);
				//TD18661
			}
			catch(...)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException("error acknowledging alarm in switch");
			}

		}
		else
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No Interface found to acknowledge alarm");
		}
	}

	
	//
	//acknowledgeCameraAlarm
	//
    void SwitchManager::acknowledgeTrainCameraAlarm( unsigned long locationId, unsigned long trainId, const TA_Base_Bus_GZL9::SwitchManagerCorbaDef::TrainCameraSequence& cameraIds, unsigned long consoleId )
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        if (cameraIds.length() == 0)
        {
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No cameras to add to alarm stack");
        }

		if(m_switchAlarmInterface != NULL)
		{
			try
			{
                std::vector<unsigned long> cameras;
                for (unsigned int i = 0; i < cameraIds.length(); ++i)
                {
                    cameras.push_back( cameraIds[i] );
                }

				m_switchAlarmInterface->acknowledgeTrainCameraAlarm(locationId, trainId, cameras, consoleId);
			}
			catch(...)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException("error acknowledging alarm in switch");
			}

		}
		else
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No Interface found to acknowledge alarm");
		}
	}

	
	//
	//acknowledgeAlarm
	//
    void SwitchManager::acknowledgeAlarmWithEvent( unsigned long stationId, unsigned long eventId, unsigned long consoleId)
	{
		if(m_switchAlarmInterface != NULL)
		{
			try
			{
				m_switchAlarmInterface->acknowledgeAlarm(stationId, eventId, consoleId);
			}
			catch(...)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException("error acknowledging alarm in switch");
			}

		}
		else
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No Interface found to acknowledge alarm");
		}
	}

	
	//
	//closeAlarm
	//
    void SwitchManager::closeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail)
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		if(m_switchAlarmInterface != NULL)
		{
			try
			{
				// TD15853 - changed parameters to closeAlarm()
				std::ostringstream alarmStr;
				alarmStr << "[CLOSE] Alarm ID = " << pAlarmDetail.alarmID 
					     << " Type = " << pAlarmDetail.messageTypeKey
						 << " EntitKey = " << pAlarmDetail.assocEntityKey;

				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, alarmStr.str().c_str());

				//TD18661
				//std::ostringstream alarmTypeKey;
				//alarmTypeKey << pAlarmDetail.messageTypeKey;
				std::string alarmID = pAlarmDetail.alarmID.in();
				m_switchAlarmInterface->closeAlarm(alarmID.c_str()/*, pAlarmDetail.assocEntityKey*/);
				//TD18661
			}
			catch (TA_Base_Core::VideoSwitchAgentInternalException& e) // TD16201
			{
				std::ostringstream err;
				err << "Close Alarm Exception: " << e.what(); 
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, err.str().c_str());
			}
			catch(...)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException("error closing alarm in switch");
			}

		}
		else
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No Interface found to acknowledge alarm");
		}
	}

    //
	//closeAlarm
	//
    void SwitchManager::closeAlarmWithEvent( unsigned long stationId, unsigned long eventId )
	{
		if(m_switchAlarmInterface != NULL)
		{
			try
			{
				m_switchAlarmInterface->closeAlarm( stationId, eventId);
			}
			catch(...)
			{
				throw TA_Base_Bus_GZL9::VideoSwitchAgentException("error closing alarm in switch");
			}

		}
		else
		{
			throw TA_Base_Bus_GZL9::VideoSwitchAgentException("No Interface found to close alarm");
		}
	}

    void SwitchManager::clearAllMonitorsWithInput( unsigned long inputEntityKey, const char* sessionId )
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		TA_Base_Bus_GZL9::SwitchManagerCorbaDef::VideoMonitorSequence* monitors = getVideoMonitors();
		
		if(monitors!=NULL)
		{
			for(unsigned int i=0; i<monitors->length(); i++)
			{
				(*monitors)[i]->requestSwitchOfInput(0, sessionId, true);
			}
		}
	}

	void SwitchManager::setAlarmInterface(IPDAlarmInterface* alarmInterface)
	{
		m_switchAlarmInterface = alarmInterface;
	}

	IPDAlarmInterface* SwitchManager::getAlarmInterface()
	{
		return m_switchAlarmInterface;
	}

//++tianxiang Merge ECP++
    void SwitchManager::setStageModeInterface(IPDStageModeInterface* stageModeInterface)
    {
        m_stageModeInterface = stageModeInterface;
    }
    IPDStageModeInterface* SwitchManager::getStageModeInterface()
    {
        return m_stageModeInterface;
    }
//++tianxiang Merge ECP++
    ///////////////////////////////////////////////////////////////////
    //
    // Add/Remove Entity Methods
    //
    ///////////////////////////////////////////////////////////////////


    //
    // addVideoOutputGroup
    //
    void SwitchManager::addVideoOutputGroup( VideoOutputGroup& videoOutputGroup )
    {
        unsigned long entityKey = videoOutputGroup.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_videoOutputGroups.end() != m_videoOutputGroups.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoOutputGroup with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoOutputGroup will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_videoOutputGroups[ entityKey ] = &videoOutputGroup;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added VideoOutputGroup with entityKey %ul", entityKey );
    }


    //
    // removeVideoOutputGroup
    //
    void SwitchManager::removeVideoOutputGroup( unsigned long entityKey )
    {
        m_videoOutputGroups.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed VideoOutputGroup with entityKey %ul", entityKey );
    }


    //
    // addVideoMonitor
    //
    void SwitchManager::addVideoMonitor( VideoMonitor& videoMonitor )
    {
        unsigned long entityKey = videoMonitor.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_videoMonitors.end() != m_videoMonitors.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoMonitor with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoMonitor will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        if ( m_videoOutputs.end() != m_videoOutputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoOutput with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoMonitor will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_videoMonitors[ entityKey ] = &videoMonitor;
        m_videoOutputs[ entityKey ] = &videoMonitor;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added VideoMonitor with entityKey %ul", entityKey );

    }


    //
    // removeVideoMonitor
    //
    void SwitchManager::removeVideoMonitor( unsigned long entityKey )
    {
        m_videoMonitors.erase( entityKey );
		m_videoOutputs.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed VideoMonitor with entityKey %ul", entityKey );
    }


    //
    // addRecordingUnit
    //
    void SwitchManager::addRecordingUnit( RecordingUnit& recordingUnit )
    {
        unsigned long entityKey = recordingUnit.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_recordingUnits.end() != m_recordingUnits.find( entityKey ) )
        {
            std::stringstream error;
            error << "A RecordingUnit with the entityKey " << entityKey << " already exists. ";
            error << "The new RecordingUnit will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the map.

        m_recordingUnits[ entityKey ] = &recordingUnit;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added RecordingUnit with entityKey %ul", entityKey );
    }


    //
    // removeRecordingUnit
    //
    void SwitchManager::removeRecordingUnit( unsigned long entityKey )
    {
        m_recordingUnits.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed RecordingUnit with entityKey %ul", entityKey );
    }


    //
    // addCamera
    //
    void SwitchManager::addCamera( Camera& camera )
    {
        unsigned long entityKey = camera.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_cameras.end() != m_cameras.find( entityKey ) )
        {
            std::stringstream error;
            error << "A Camera with the entityKey " << entityKey << " already exists. ";
            error << "The new Camera will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        if ( m_videoInputs.end() != m_videoInputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new Camera will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_cameras[ entityKey ] = &camera;
        m_videoInputs[ entityKey ] = &camera;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added Camera with entityKey %ul", entityKey );
    }


    //
    // removeCamera
    //
    void SwitchManager::removeCamera( unsigned long entityKey )
    {
        m_cameras.erase( entityKey );
        m_videoInputs.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed Camera with entityKey %ul", entityKey );
    }


    //
    //
    //
    void SwitchManager::addQuad( Quad& quad )
    {
        unsigned long entityKey = quad.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_quads.end() != m_quads.find( entityKey ) )
        {
            std::stringstream error;
            error << "A Quad with the entityKey " << entityKey << " already exists. ";
            error << "The new Quad will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        if ( m_videoInputs.end() != m_videoInputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new Quad will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_quads[ entityKey ] = &quad;
        m_videoInputs[ entityKey ] = &quad;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added Quad with entityKey %ul", entityKey );
    }


    //
    // removeQuad
    //
    void SwitchManager::removeQuad( unsigned long entityKey )
    {
        m_quads.erase( entityKey );
        m_videoInputs.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed Quad with entityKey %ul", entityKey );
    }


    //
    // addBVSStage
    //
    /*void SwitchManager::addBVSStage( BVSStage& quad )
    {
        unsigned long entityKey = quad.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_bvsStages.end() != m_bvsStages.find( entityKey ) )
        {
            std::stringstream error;
            error << "A BVSStage with the entityKey " << entityKey << " already exists. ";
            error << "The new BVSStage will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        if ( m_videoInputs.end() != m_videoInputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new BVSStage will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_bvsStages[ entityKey ] = &quad;
        m_videoInputs[ entityKey ] = &quad;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added BVSStage with entityKey %ul", entityKey );
    }*/


    //
    // removeBVSStage
    //
  /*  void SwitchManager::removeBVSStage( unsigned long entityKey )
    {
        m_bvsStages.erase( entityKey );
        m_videoInputs.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed BVSStage with entityKey %ul", entityKey );
    }
*/

    //
    // addSequence
    //
    void SwitchManager::addSequence( Sequence& sequence )
    {
        unsigned long entityKey = sequence.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_sequences.end() != m_sequences.find( entityKey ) )
        {
            std::stringstream error;
            error << "A Sequence with the entityKey " << entityKey << " already exists. ";
            error << "The new Sequence will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        if ( m_videoInputs.end() != m_videoInputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoInput with the entityKey " << entityKey << " already exists. ";
            error << "The new Sequence will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.

        m_sequences[ entityKey ] = &sequence;
        m_videoInputs[ entityKey ] = &sequence;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added Sequence with entityKey %ul", entityKey );
    }


    //
    // removeSequence
    //
    void SwitchManager::removeSequence( unsigned long entityKey )
    {
        m_sequences.erase( entityKey );
        m_videoInputs.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed Sequence with entityKey %ul", entityKey );
    }

    
    ///////////////////////////////////////////////////////////////////
    //
    // Helper Methods
    //
    ///////////////////////////////////////////////////////////////////


    //
    // getVideoOutputInternal
    //
    VideoOutput& SwitchManager::getVideoOutputInternal( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() == it )
        {
            std::stringstream error;
            error << "The VideoOutput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }


    //
    // getVideoInputInternal
    //
    VideoInput& SwitchManager::getVideoInputInternal( unsigned long entityKey )
    {
        VideoInputMap::iterator it = m_videoInputs.find( entityKey );
        if ( m_videoInputs.end() == it )
        {
            std::stringstream error;
            error << "The VideoInput with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }


    //
    // getCameraInternal
    //
    Camera& SwitchManager::getCameraInternal( unsigned long entityKey )
    {
        CameraMap::iterator it = m_cameras.find( entityKey );
        if ( m_cameras.end() == it )
        {
            std::stringstream error;
            error << "The Camera with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }


    //
    // getQuadInternal
    //
    Quad& SwitchManager::getQuadInternal( unsigned long entityKey )
    {
        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() == it )
        {
            std::stringstream error;
            error << "The Quad with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_OUTPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }


    //
    // getSequenceInternal
    //
    Sequence& SwitchManager::getSequenceInternal( unsigned long entityKey )
    {
        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() == it )
        {
            std::stringstream error;
            error << "The Sequence with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_INPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }


    //
    // getRecordingUnitInternal
    //
    RecordingUnit& SwitchManager::getRecordingUnitInternal( unsigned long entityKey )
    {
        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() == it )
        {
            std::stringstream error;
            error << "The RecordingUnit with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_INPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }

    //
    // getRecordingUnitInternal
    //
    /*BVSStage& SwitchManager::getBVSStageInternal( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() == it )
        {
            std::stringstream error;
            error << "The BVS Stage with the identifier " << entityKey;
            error << " is unknown to the Video Switch Agent.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::UNKNOWN_INPUT,
                error.str().c_str() ) );
        }
        return *(it->second);
    }*/


    //
    // SwitchManager
    //
    SwitchManager::SwitchManager()
        : m_switchAlarmInterface(NULL),
          m_stageModeInterface(NULL),
		  m_PDInterfaceFactory(NULL)
    {
        // Activate this CORBA servant so it can accept incoming requests.

        // activateServant();
    }


    SwitchManager::~SwitchManager()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );
        
        SwitchManager::m_theClassInstance = NULL;
    }
//++tianxiang Merge ECP++
    void SwitchManager::setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch)
    {
        FUNCTION_ENTRY( "setStageSwitchMode" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        //limin TODO, when set value to m_stageModeInterface ?
        if ( m_stageModeInterface != NULL )
        {
            m_stageModeInterface->setStageSwitchMode( originatingStage, destinationStage, allowSwitch );
        }

        FUNCTION_EXIT;
    }

	//++tianxiang Merge ECP++

} // TA_IRS_App
