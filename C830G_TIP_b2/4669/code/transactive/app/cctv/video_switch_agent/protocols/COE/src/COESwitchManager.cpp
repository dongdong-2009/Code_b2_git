/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.cpp $
  * @author:   Rob Young 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Manages access to the entities (Cameras, VideoMonitors, 
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/video_switch_agent/protocols/COE/src/COEHelper.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/StateValidationCycle.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "bus/scada/datapoint_library/src/DataPointFactory.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include <sstream>

namespace TA_IRS_App
{
    using TA_Base_Bus::DataPointFactory;
    using TA_Base_Bus::DataPoint;

    //
    // Initialise statics.
    //

	TA_Base_Core::ReEntrantThreadLockable COESwitchManager::m_singletonLock;
	COESwitchManager* COESwitchManager::m_theClassInstance = NULL;

    const std::string COESwitchManager::SUPPORTED_DATAPOINT_TYPE  = "Boolean";

    //
    // Constructor
    //
    COESwitchManager::COESwitchManager() 
        : m_commsHandler( 0 ),
          m_stateValidationCycle( new StateValidationCycle() ),
          m_dataPointFactory( NULL )
    {
    }



    //
    // Destructor (only gets called when removeInstance() is called by client code)
    //
    COESwitchManager::~COESwitchManager() 
    {
        delete m_stateValidationCycle;
        m_stateValidationCycle = 0;
        m_dataPointFactory = 0;
    }


    //
    // getInstance
    //
    COESwitchManager& COESwitchManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( 0 == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new COESwitchManager();
		}

		return *m_theClassInstance;
    }



    //
    // removeInstance
    //
    void COESwitchManager::removeInstance()
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
    COERecordingUnit* COESwitchManager::getRecordingUnit( unsigned long entityKey )
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
    COERecordingUnit* COESwitchManager::getRecordingUnitFromId( unsigned short id )
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
    COECamera* COESwitchManager::getCamera( unsigned long entityKey )
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
    COECamera* COESwitchManager::getCameraFromLocationAndId( unsigned short location, unsigned short id )
    {
        LocationAndIdToCamera::iterator locAndIdToCamIt = m_locationAndIdToCamera.find( location );
        if ( m_locationAndIdToCamera.end() != locAndIdToCamIt )
        {
            IdToCamera::iterator idToCamIt = ( *locAndIdToCamIt ).second.find( id );
            if ( ( *locAndIdToCamIt ).second.end() != idToCamIt )
            {
                return ( *idToCamIt ).second;
            }
        }

        return 0;
    }


    //
    // getQuad
    //
    COEQuad* COESwitchManager::getQuad( unsigned long entityKey )
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
    COEQuad* COESwitchManager::getQuadFromId( unsigned short id )
    {
        IdToQuad::iterator idToQuadIt = m_idToQuad.find( id );
        if ( m_idToQuad.end() != idToQuadIt )
        {
            return ( *idToQuadIt ).second;
        }

        return 0;
    }


    //
    // getBVSStage
    //
    COEBVSStage* COESwitchManager::getBVSStage( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() == it )
        {
            return 0;
        }
        return it->second;
    }


    //
    // getBVSStageFromId
    //
    COEBVSStage* COESwitchManager::getBVSStageFromId( unsigned short id )
    {
        IdToBVSStage::iterator idToBVSStageIt = m_idToBVSStage.find( id );
        if ( m_idToBVSStage.end() != idToBVSStageIt )
        {
            return ( *idToBVSStageIt ).second;
        }

        return 0;
    }


    //
    // getSequence
    //
    COESequence* COESwitchManager::getSequence( unsigned long entityKey )
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
    COESequence* COESwitchManager::getSequenceFromId( unsigned short id )
    {
        IdToSequence::iterator idToSequenceIt = m_idToSequence.find( id );
        if ( m_idToSequence.end() != idToSequenceIt )
        {
            return ( *idToSequenceIt ).second;
        }

        return 0;
    }


    //
    // getVideoOutput
    //
    COEVideoOutput* COESwitchManager::getVideoOutput( unsigned long entityKey )
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
    std::vector<COEVideoOutput*> COESwitchManager::getAllVideoOutputsWithAssignment( unsigned long entityKey )
    {
        VideoOutputMap::iterator it;
		std::vector<COEVideoOutput*> videoOutputs;
        for(it=m_videoOutputs.begin(); it!=m_videoOutputs.end(); it++)
        {
			COEVideoOutput* output = (*it).second;
			if(output->getCurrentVideoInputKey() == entityKey)
			{
				videoOutputs.push_back(output);
			}
			else if(output->isBVSStageInAlarmStack(entityKey))
			{
				videoOutputs.push_back(output);
			}
		}
        return videoOutputs;
    }
	

    //
    // getVideoOutputFromId
    //
    COEVideoOutput* COESwitchManager::getVideoOutputFromId( unsigned short id )
    {
        IdToVideoOutput::iterator idToVideoOutputIt = m_idToVideoOutput.find( id );
        if ( m_idToVideoOutput.end() != idToVideoOutputIt )
        {
            return ( *idToVideoOutputIt ).second;
        }

        return 0;
    }


    //
    // getDataPointFromId
    //
    DataPoint* COESwitchManager::getDataPointFromId( unsigned short id )
    {
        IdToDataPoint::iterator idToDataPointIt = m_idToDataPoint.find( id );
        if ( m_idToDataPoint.end() != idToDataPointIt )
        {
            return ( *idToDataPointIt ).second;
        }

        return 0;
    }


    //
    // getDataPointFromKey
    //
    DataPoint* COESwitchManager::getDataPointFromKey( unsigned long pkey )
    {
        IdToDataPoint::iterator idToDataPointIt;
        for(idToDataPointIt = m_idToDataPoint.begin(); idToDataPointIt != m_idToDataPoint.end(); idToDataPointIt++ )
        {
            if(( *idToDataPointIt ).second->getEntityKey() == pkey)
			{
				return ( *idToDataPointIt ).second;
			}
        }

        return 0;
    }


    ///////////////////////////////////////////////////////////////////
    //
    // Add/Remove Entity Methods
    //
    ///////////////////////////////////////////////////////////////////


    //
    // staticCreateDataPoint (static function pointer access to createDataPoint)
    //
    TA_Base_Bus::IEntity* COESwitchManager::staticCreateDataPoint( TA_Base_Core::IEntityDataPtr entityData )
    {
        return getInstance().createDataPoint( entityData );
    }


    //
    // createDataPoint
    //
    TA_Base_Bus::IEntity* COESwitchManager::createDataPoint( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT(m_dataPointFactory != NULL, "DataPointFactory is not set yet.");
        
        TA_Base_Core::DataPointEntityDataPtr dpEntityData = boost::dynamic_pointer_cast< TA_Base_Core::DataPointEntityData >( entityData );
		if ( 0 == dpEntityData.get())
		{
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                "The provided entity data could not be converted to DataPoint data." ) );
		}

        try
        {
            if ( SUPPORTED_DATAPOINT_TYPE != dpEntityData->getDataPointType() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "VideoSwitchAgent configured with DataPoint type it does not support." );
                return 0;
            }

            unsigned short dpAddress = TA_COE::getIdFromAddress( dpEntityData->getAddress() );
            if ( m_idToDataPoint.end() != m_idToDataPoint.find( dpAddress ) )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "VideoSwitchAgent configured with multiple DataPoints at same address :%d", dpAddress );
                return 0;
            }

            DataPoint* dataPoint = m_dataPointFactory->createDataPoint( dpEntityData );
            m_idToDataPoint[ dpAddress ] = dataPoint;
            return dataPoint;
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to retrieve DataPoint's configuration from database." );
            return 0;
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to retrieve DataPoint's configuration from database." );
            return 0;
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Could not convert DataPoint's configured address into unsigned short." );
            return 0;
        }
    }


    //
    // addRecordingUnit
    //
    void COESwitchManager::addRecordingUnit( COERecordingUnit& recordingUnit )
    {
        unsigned long entityKey = recordingUnit.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_recordingUnits.end() != m_recordingUnits.find( entityKey ) )
        {
            std::stringstream error;
            error << "A COERecordingUnit with the entityKey " << entityKey << " already exists. ";
            error << "The new COERecordingUnit will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.
        unsigned short recordingUnitId = TA_COE::getIdFromAddress( recordingUnit.getAddress() );
        m_recordingUnits[ entityKey ] = &recordingUnit;
        m_idToRecordingUnit[ recordingUnitId ] = &recordingUnit;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added COERecordingUnit with entityKey %ul", entityKey );
    }


    //
    // removeRecordingUnit
    //
    void COESwitchManager::removeRecordingUnit( unsigned long entityKey )
    {
        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() != it )
        {
            m_idToRecordingUnit.erase( TA_COE::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_recordingUnits.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed COERecordingUnit with entityKey %ul", entityKey );
    }


    //
    // addCamera
    //
    void COESwitchManager::addCamera( COECamera& camera )
    {
        unsigned long entityKey = camera.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_cameras.end() != m_cameras.find( entityKey ) )
        {
            std::stringstream error;
            error << "A COECamera with the entityKey " << entityKey << " already exists. ";
            error << "The new COECamera will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the maps.
        std::pair< unsigned short, unsigned short > camAddr = camera.getParsedAddress();
        m_cameras[ entityKey ] = &camera;
        m_inputTypes[ entityKey ] = CAMERA;
        m_locationAndIdToCamera[ camAddr.first ][ camAddr.second ] = &camera;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added COECamera with entityKey %ul", entityKey );
    }


    //
    // removeCamera
    //
    void COESwitchManager::removeCamera( unsigned long entityKey )
    {
        CameraMap::iterator it = m_cameras.find( entityKey );
        if ( m_cameras.end() != it )
        {
            std::pair< unsigned short, unsigned short > camAddr = ( *it ).second->getParsedAddress();
            m_locationAndIdToCamera[ camAddr.first ].erase( camAddr.second );
            m_cameras.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed COECamera with entityKey %ul", entityKey );
    }


    //
    // addQuad
    //
    void COESwitchManager::addQuad( COEQuad& quad )
    {
        unsigned long entityKey = quad.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_quads.end() != m_quads.find( entityKey ) )
        {
            std::stringstream error;
            error << "A COEQuad with the entityKey " << entityKey << " already exists. ";
            error << "The new COEQuad will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        unsigned short quadId = TA_COE::getIdFromAddress( quad.getAddress() );
        m_quads[ entityKey ] = &quad;
        m_idToQuad[ quadId ] = &quad;
        m_inputTypes[ entityKey ] = QUAD;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added COEQuad with entityKey %ul", entityKey );
    }


    //
    // removeQuad
    //
    void COESwitchManager::removeQuad( unsigned long entityKey )
    {
        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() != it )
        {
            m_idToQuad.erase( TA_COE::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_quads.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed COEQuad with entityKey %ul", entityKey );
    }


    //
    // addBVSStage
    //
    void COESwitchManager::addBVSStage( COEBVSStage& bvsStage )
    {
        unsigned long entityKey = bvsStage.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_bvsStages.end() != m_bvsStages.find( entityKey ) )
        {
            std::stringstream error;
            error << "A COEBVSStage with the entityKey " << entityKey << " already exists. ";
            error << "The new COEBVSStage will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        unsigned short bvsStageId = TA_COE::getIdFromAddress( bvsStage.getAddress() );
        m_bvsStages[ entityKey ] = &bvsStage;
        m_idToBVSStage[ bvsStageId ] = &bvsStage;
        m_inputTypes[ entityKey ] = BVS_STAGE;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added COEBVSStage with entityKey %ul", entityKey );
    }


    //
    // removeBVSStage
    //
    void COESwitchManager::removeBVSStage( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() != it )
        {
            m_idToBVSStage.erase( TA_COE::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_bvsStages.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed COEBVSStage with entityKey %ul", entityKey );
    }


    //
    // addSequence
    //
    void COESwitchManager::addSequence( COESequence& sequence )
    {
        unsigned long entityKey = sequence.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_sequences.end() != m_sequences.find( entityKey ) )
        {
            std::stringstream error;
            error << "A COESequence with the entityKey " << entityKey << " already exists. ";
            error << "The new COESequence will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        // This is the only object with this entityKey, so add it to the map.
        unsigned short sequenceId = TA_COE::getIdFromAddress( sequence.getAddress() );
        m_sequences[ entityKey ] = &sequence;
        m_idToSequence[ sequenceId ] = &sequence;
        m_inputTypes[ entityKey ] = SEQUENCE;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added COESequence with entityKey %ul", entityKey );
    }


    //
    // removeSequence
    //
    void COESwitchManager::removeSequence( unsigned long entityKey )
    {
        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() != it )
        {
            m_idToSequence.erase( TA_COE::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_sequences.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed COESequence with entityKey %ul", entityKey );
    }


    //
    // addVideoOutput
    //
    void COESwitchManager::addVideoOutput( COEVideoOutput& videoOutput )
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
        unsigned short videoOutputId = TA_COE::getIdFromAddress( videoOutput.getAddress() );
        m_videoOutputs[ entityKey ] = &videoOutput;
        m_idToVideoOutput[ videoOutputId ] = &videoOutput;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added VideoOutput with entityKey %ul", entityKey );
    }


    //
    // removeVideoOutput
    //
    void COESwitchManager::removeVideoOutput( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() != it )
        {
            m_videoOutputs.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed VideoOutput with entityKey %ul", entityKey );
    }


    COESwitchManager::ECOEInputType COESwitchManager::getInputType( unsigned long entityKey )
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
    COECommunicationsHandler* COESwitchManager::getCommunicationsHandler()
    {
        if ( 0 == m_commsHandler )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The COECommunicationsHandler does not exist. " ) );
        }
        return m_commsHandler;
    }


    //
    // addCommunicationsHander
    //
    void COESwitchManager::addCommunicationsHander( COECommunicationsHandler& commsHandler )
    {
        if ( 0 != m_commsHandler )
        {
            std::stringstream error;
            error << "The COECommunicationsHandler already exists. ";
            error << "The new COECommunicationsHandler will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        m_commsHandler = &commsHandler;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added COECommunicationsHandler" );
    }


    //
    // removeCommunicationsHander
    //
    void COESwitchManager::removeCommunicationsHander()
    {
        // Don't worry about deleting this object - that will be handled elsewhere.

        m_commsHandler = 0;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed COECommunicationsHandler." );
    }


    //
    // getStateValidationCycle
    //
    StateValidationCycle& COESwitchManager::getStateValidationCycle()
    {
        return *m_stateValidationCycle;
    }


	//TD16761 Mintao++
	std::map< unsigned short, TA_Base_Bus::DataPoint* > COESwitchManager::getAllDataPoints()
    {
        return m_idToDataPoint;
    }

    void COESwitchManager::registerDataPointFactory(TA_Base_Bus::DataPointFactory* dataPointFactory)
    {
        m_dataPointFactory = dataPointFactory;
    }
    

} // TA_IRS_App

