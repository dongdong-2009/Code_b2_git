/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.cpp $
  * @author:   Rob Young 
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/08/30 17:38:58 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Manages access to the entities (Cameras, VideoMonitors, 
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSHelper.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/StateValidationCycle.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

namespace TA_IRS_App
{

    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable IMOSSwitchManager::m_singletonLock;
	IMOSSwitchManager* IMOSSwitchManager::m_theClassInstance = NULL;

    //
    // Constructor
    //
    IMOSSwitchManager::IMOSSwitchManager() 
        : m_commsHandler( 0 )
//          m_stateValidationCycle( new StateValidationCycle() ),
    {
    }



    //
    // Destructor (only gets called when removeInstance() is called by client code)
    //
    IMOSSwitchManager::~IMOSSwitchManager() 
    {
		/*
        delete m_stateValidationCycle;
        m_stateValidationCycle = 0;
		*/
    }


    //
    // getInstance
    //
    IMOSSwitchManager& IMOSSwitchManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( 0 == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new IMOSSwitchManager();
		}

		return *m_theClassInstance;
    }



    //
    // removeInstance
    //
    void IMOSSwitchManager::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		if ( 0 != m_theClassInstance )
		{
			// Delete the one & only object.

			delete m_theClassInstance;
            m_theClassInstance = 0;
		}
    }


    //
    // getRecordingUnit
    //
    IMOSRecordingUnit* IMOSSwitchManager::getRecordingUnit( unsigned long entityKey )
    {
        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getRecordingUnitFromId
    //
    IMOSRecordingUnit* IMOSSwitchManager::getRecordingUnitFromId( unsigned short id )
    {
        IdToRecordingUnit::iterator idToRecordingUnitIt = m_idToRecordingUnit.find( id );
        if ( m_idToRecordingUnit.end() != idToRecordingUnitIt )
        {
            return ( *idToRecordingUnitIt ).second;
        }

        return 0;
    }


    //
    // getCamera
    //
    IMOSCamera* IMOSSwitchManager::getCamera( unsigned long entityKey )
    {
        CameraMap::iterator it = m_cameras.find( entityKey );
        if ( m_cameras.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getCameraFromLocationAndId
    //
    IMOSCamera* IMOSSwitchManager::getCameraFromLocationAndId( unsigned short location, unsigned short id )
    {
		/*
        LocationAndIdToCamera::iterator locAndIdToCamIt = m_locationAndIdToCamera.find( location );
        if ( m_locationAndIdToCamera.end() != locAndIdToCamIt )
        {
            IdToCamera::iterator idToCamIt = ( *locAndIdToCamIt ).second.find( id );
            if ( ( *locAndIdToCamIt ).second.end() != idToCamIt )
            {
                return ( *idToCamIt ).second;
            }
        }
		*/

        return 0;
    }


    //
    // getQuad
    //
    IMOSQuad* IMOSSwitchManager::getQuad( unsigned long entityKey )
    {
        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getQuadFromId
    //
    IMOSQuad* IMOSSwitchManager::getQuadFromId( unsigned short id )
    {
		/*
        IdToQuad::iterator idToQuadIt = m_idToQuad.find( id );
        if ( m_idToQuad.end() != idToQuadIt )
        {
            return ( *idToQuadIt ).second;
        }
		*/
        return 0;
    }


    //
    // getBVSStage
    //
  /*  IMOSBVSStage* IMOSSwitchManager::getBVSStage( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() == it )
        {
            return 0;
        }
        return it->second;
    }*/


    //
    // getBVSStageFromId
    //
	/*
    IMOSBVSStage* IMOSSwitchManager::getBVSStageFromId( unsigned short id )
    {
        IdToBVSStage::iterator idToBVSStageIt = m_idToBVSStage.find( id );
        if ( m_idToBVSStage.end() != idToBVSStageIt )
        {
            return ( *idToBVSStageIt ).second;
        }

        return 0;
    }*/


    //
    // getSequence
    //
    IMOSSequence* IMOSSwitchManager::getSequence( unsigned long entityKey )
    {
        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getSequenceFromId
    //
    IMOSSequence* IMOSSwitchManager::getSequenceFromId( unsigned short id )
    {
		/*
        IdToSequence::iterator idToSequenceIt = m_idToSequence.find( id );
        if ( m_idToSequence.end() != idToSequenceIt )
        {
            return ( *idToSequenceIt ).second;
        }
		*/
        return 0;
    }


    //
    // getVideoOutput
    //
    IMOSVideoOutput* IMOSSwitchManager::getVideoOutput( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getAllVideoOutputsWithAssignment
    //
    std::vector<IMOSVideoOutput*> IMOSSwitchManager::getAllVideoOutputsWithAssignment( unsigned long entityKey )
    {
        VideoOutputMap::iterator it;
		std::vector<IMOSVideoOutput*> videoOutputs;
        for(it=m_videoOutputs.begin(); it!=m_videoOutputs.end(); it++)
        {
			IMOSVideoOutput* output = (*it).second;
			if(output->getCurrentVideoInputKey() == entityKey)
			{
				videoOutputs.push_back(output);
			}
			/*else if(output->isBVSStageInAlarmStack(entityKey))
			{
				videoOutputs.push_back(output);
			}*/
		}
        return videoOutputs;
    }
	

    //
    // getVideoOutputFromId
    //
    IMOSVideoOutput* IMOSSwitchManager::getVideoOutputFromId( unsigned short id )
    {
        IdToVideoOutput::iterator idToVideoOutputIt = m_idToVideoOutput.find( id );
        if ( m_idToVideoOutput.end() != idToVideoOutputIt )
        {
            return ( *idToVideoOutputIt ).second;
        }

        return 0;
    }



    ///////////////////////////////////////////////////////////////////
    //
    // Add/Remove Entity Methods
    //
    ///////////////////////////////////////////////////////////////////



    //
    // addRecordingUnit
    //
    void IMOSSwitchManager::addRecordingUnit( IMOSRecordingUnit& recordingUnit )
    {
        unsigned long entityKey = recordingUnit.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_recordingUnits.end() != m_recordingUnits.find( entityKey ) )
        {
            std::stringstream error;
            error << "A IMOSRecordingUnit with the entityKey " << entityKey << " already exists. ";
            error << "The new IMOSRecordingUnit will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.
        unsigned short recordingUnitId = TA_IMOS::getIdFromAddress( recordingUnit.getAddress() );
        m_recordingUnits[ entityKey ] = &recordingUnit;
        m_idToRecordingUnit[ recordingUnitId ] = &recordingUnit;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added IMOSRecordingUnit with entityKey %ul", entityKey );
    }


    //
    // removeRecordingUnit
    //
    void IMOSSwitchManager::removeRecordingUnit( unsigned long entityKey )
    {
        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() != it )
        {
            m_idToRecordingUnit.erase( TA_IMOS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_recordingUnits.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed IMOSRecordingUnit with entityKey %ul", entityKey );
    }


    //
    // addCamera
    //
    void IMOSSwitchManager::addCamera( IMOSCamera& camera )
    {
        unsigned long entityKey = camera.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_cameras.end() != m_cameras.find( entityKey ) )
        {
            std::stringstream error;
            error << "A IMOSCamera with the entityKey " << entityKey << " already exists. ";
            error << "The new IMOSCamera will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the maps.
        m_cameras[ entityKey ] = &camera;
        m_inputTypes[ entityKey ] = CAMERA;
		/*
        std::pair< unsigned short, unsigned short > camAddr = camera.getParsedAddress();
        m_locationAndIdToCamera[ camAddr.first ][ camAddr.second ] = &camera;
		*/

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added IMOSCamera with entityKey %ul", entityKey );
    }


    //
    // removeCamera
    //
    void IMOSSwitchManager::removeCamera( unsigned long entityKey )
    {
        CameraMap::iterator it = m_cameras.find( entityKey );
        if ( m_cameras.end() != it )
        {
			/*
            std::pair< unsigned short, unsigned short > camAddr = ( *it ).second->getParsedAddress();
            m_locationAndIdToCamera[ camAddr.first ].erase( camAddr.second );
			*/
            m_cameras.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed IMOSCamera with entityKey %ul", entityKey );
    }


    //
    // addQuad
    //
    void IMOSSwitchManager::addQuad( IMOSQuad& quad )
    {
        unsigned long entityKey = quad.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_quads.end() != m_quads.find( entityKey ) )
        {
            std::stringstream error;
            error << "A IMOSQuad with the entityKey " << entityKey << " already exists. ";
            error << "The new IMOSQuad will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        m_quads[ entityKey ] = &quad;
        m_inputTypes[ entityKey ] = QUAD;
		/*
        unsigned short quadId = TA_IMOS::getIdFromAddress( quad.getAddress() );
        m_idToQuad[ quadId ] = &quad;
		*/
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added IMOSQuad with entityKey %ul", entityKey );
    }


    //
    // removeQuad
    //
    void IMOSSwitchManager::removeQuad( unsigned long entityKey )
    {
        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() != it )
        {
            //m_idToQuad.erase( TA_IMOS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_quads.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed IMOSQuad with entityKey %ul", entityKey );
    }


    //
    // addBVSStage
    //
  /*  void IMOSSwitchManager::addBVSStage( IMOSBVSStage& bvsStage )
    {
        unsigned long entityKey = bvsStage.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_bvsStages.end() != m_bvsStages.find( entityKey ) )
        {
            std::stringstream error;
            error << "A IMOSBVSStage with the entityKey " << entityKey << " already exists. ";
            error << "The new IMOSBVSStage will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        unsigned short bvsStageId = TA_IMOS::getIdFromAddress( bvsStage.getAddress() );
        m_bvsStages[ entityKey ] = &bvsStage;
        m_idToBVSStage[ bvsStageId ] = &bvsStage;
        m_inputTypes[ entityKey ] = BVS_STAGE;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added IMOSBVSStage with entityKey %ul", entityKey );
    }


    //
    // removeBVSStage
    //
    void IMOSSwitchManager::removeBVSStage( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() != it )
        {
            m_idToBVSStage.erase( TA_IMOS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_bvsStages.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed IMOSBVSStage with entityKey %ul", entityKey );
    }
*/

    //
    // addSequence
    //
    void IMOSSwitchManager::addSequence( IMOSSequence& sequence )
    {
        unsigned long entityKey = sequence.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_sequences.end() != m_sequences.find( entityKey ) )
        {
            std::stringstream error;
            error << "A IMOSSequence with the entityKey " << entityKey << " already exists. ";
            error << "The new IMOSSequence will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        // This is the only object with this entityKey, so add it to the map.
        m_sequences[ entityKey ] = &sequence;
        m_inputTypes[ entityKey ] = SEQUENCE;
		/*
        unsigned short sequenceId = TA_IMOS::getIdFromAddress( sequence.getAddress() );
        m_idToSequence[ sequenceId ] = &sequence;
		*/
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added IMOSSequence with entityKey %ul", entityKey );
    }


    //
    // removeSequence
    //
    void IMOSSwitchManager::removeSequence( unsigned long entityKey )
    {
        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() != it )
        {
            //m_idToSequence.erase( TA_IMOS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_sequences.erase( it );
        }
        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed IMOSSequence with entityKey %ul", entityKey );
    }


    //
    // addVideoOutput
    //
    void IMOSSwitchManager::addVideoOutput( IMOSVideoOutput& videoOutput )
    {
        unsigned long entityKey = videoOutput.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_videoOutputs.end() != m_videoOutputs.find( entityKey ) )
        {
            std::stringstream error;
            error << "A VideoOutput with the entityKey " << entityKey << " already exists. ";
            error << "The new VideoOutput will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.
        unsigned short videoOutputId = TA_IMOS::getIdFromAddress( videoOutput.getAddress() );
        m_videoOutputs[ entityKey ] = &videoOutput;
        m_idToVideoOutput[ videoOutputId ] = &videoOutput;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added VideoOutput with entityKey %ul", entityKey );
    }


    //
    // removeVideoOutput
    //
    void IMOSSwitchManager::removeVideoOutput( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() != it )
        {
            m_videoOutputs.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed VideoOutput with entityKey %ul", entityKey );
    }


    IMOSSwitchManager::EIMOSInputType IMOSSwitchManager::getInputType( unsigned long entityKey )
    {
        InputTypeMap::iterator it = m_inputTypes.find( entityKey );
        if ( it != m_inputTypes.end() )
        {
            return ( *it ).second;
        }
        else
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The InputType does not exist. " ) );
        }
    }

    //
    // getCommunicationsHandler
    //
    IMOSCommunicationsHandler* IMOSSwitchManager::getCommunicationsHandler()
    {
        if ( 0 == m_commsHandler )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The IMOSCommunicationsHandler does not exist. " ) );
        }
        return m_commsHandler;
    }


    //
    // addCommunicationsHander
    //
    void IMOSSwitchManager::addCommunicationsHander( IMOSCommunicationsHandler& commsHandler )
    {
        if ( 0 != m_commsHandler )
        {
            std::stringstream error;
            error << "The IMOSCommunicationsHandler already exists. ";
            error << "The new IMOSCommunicationsHandler will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        m_commsHandler = &commsHandler;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added IMOSCommunicationsHandler" );
    }


    //
    // removeCommunicationsHander
    //
    void IMOSSwitchManager::removeCommunicationsHander()
    {
        // Don't worry about deleting this object - that will be handled elsewhere.

        m_commsHandler = 0;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed IMOSCommunicationsHandler." );
    }


    //
    // getStateValidationCycle
    //
	/*
    StateValidationCycle& IMOSSwitchManager::getStateValidationCycle()
    {
        return *m_stateValidationCycle;
    }
	*/


} // TA_IRS_App

