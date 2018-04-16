/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.cpp $
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

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSHelper.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
//#include "app/cctv/video_switch_agent/protocols/MODBUS/src/StateValidationCycle.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
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

	TA_Base_Core::ReEntrantThreadLockable MODBUSSwitchManager::m_singletonLock;
	MODBUSSwitchManager* MODBUSSwitchManager::m_theClassInstance = NULL;

    const std::string MODBUSSwitchManager::SUPPORTED_DATAPOINT_TYPE  = "Boolean";

    //
    // Constructor
    //
    MODBUSSwitchManager::MODBUSSwitchManager() 
        : m_commsHandler( 0 ),
//          m_stateValidationCycle( new StateValidationCycle() ),
          m_dataPointFactory( NULL )
    {
    }



    //
    // Destructor (only gets called when removeInstance() is called by client code)
    //
    MODBUSSwitchManager::~MODBUSSwitchManager() 
    {
		/*
        delete m_stateValidationCycle;
        m_stateValidationCycle = 0;
		*/
        m_dataPointFactory = 0;
    }


    //
    // getInstance
    //
    MODBUSSwitchManager& MODBUSSwitchManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( 0 == m_theClassInstance )
		{
			// Create the one & only object.

			m_theClassInstance = new MODBUSSwitchManager();
		}

		return *m_theClassInstance;
    }



    //
    // removeInstance
    //
    void MODBUSSwitchManager::removeInstance()
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
    MODBUSRecordingUnit* MODBUSSwitchManager::getRecordingUnit( unsigned long entityKey )
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
    MODBUSRecordingUnit* MODBUSSwitchManager::getRecordingUnitFromId( unsigned short id )
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
    MODBUSCamera* MODBUSSwitchManager::getCamera( unsigned long entityKey )
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
    MODBUSCamera* MODBUSSwitchManager::getCameraFromLocationAndId( unsigned short location, unsigned short id )
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
    MODBUSQuad* MODBUSSwitchManager::getQuad( unsigned long entityKey )
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
    MODBUSQuad* MODBUSSwitchManager::getQuadFromId( unsigned short id )
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
  /*  MODBUSBVSStage* MODBUSSwitchManager::getBVSStage( unsigned long entityKey )
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
    MODBUSBVSStage* MODBUSSwitchManager::getBVSStageFromId( unsigned short id )
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
    MODBUSSequence* MODBUSSwitchManager::getSequence( unsigned long entityKey )
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
    MODBUSSequence* MODBUSSwitchManager::getSequenceFromId( unsigned short id )
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
    MODBUSVideoOutput* MODBUSSwitchManager::getVideoOutput( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() == it )
        {
            return 0;
        }
        return it->second;
    }


	MODBUSVideoOutput* MODBUSSwitchManager::getVideoOutPutByDisplayOrderAndConsole( int displayOrder, unsigned long consoleKey )
	{
		if(m_consoleToDisplayOrderToOutput.find(consoleKey) == m_consoleToDisplayOrderToOutput.end())
		{
			return 0;
		}
		if(m_consoleToDisplayOrderToOutput[consoleKey].find(displayOrder) == m_consoleToDisplayOrderToOutput[consoleKey].end())
		{
			return 0;
		}
		return m_consoleToDisplayOrderToOutput[consoleKey][displayOrder];
	}

	//
    // getAllVideoOutputsWithAssignment
    //
    std::vector<MODBUSVideoOutput*> MODBUSSwitchManager::getAllVideoOutputsWithAssignment( unsigned long entityKey )
    {
        VideoOutputMap::iterator it;
		std::vector<MODBUSVideoOutput*> videoOutputs;
        for(it=m_videoOutputs.begin(); it!=m_videoOutputs.end(); it++)
        {
			MODBUSVideoOutput* output = (*it).second;
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
    MODBUSVideoOutput* MODBUSSwitchManager::getVideoOutputFromId( unsigned short id )
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
    DataPoint* MODBUSSwitchManager::getDataPointFromId( unsigned short id )
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
    DataPoint* MODBUSSwitchManager::getDataPointFromKey( unsigned long pkey )
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
    TA_Base_Bus::IEntity* MODBUSSwitchManager::staticCreateDataPoint( TA_Base_Core::IEntityDataPtr entityData )
    {
        return getInstance().createDataPoint( entityData );
    }


    //
    // createDataPoint
    //
    TA_Base_Bus::IEntity* MODBUSSwitchManager::createDataPoint( TA_Base_Core::IEntityDataPtr entityData )
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

            unsigned short dpAddress = TA_MODBUS::getIdFromAddress( dpEntityData->getAddress() );
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
    void MODBUSSwitchManager::addRecordingUnit( MODBUSRecordingUnit& recordingUnit )
    {
        unsigned long entityKey = recordingUnit.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_recordingUnits.end() != m_recordingUnits.find( entityKey ) )
        {
            std::stringstream error;
            error << "A MODBUSRecordingUnit with the entityKey " << entityKey << " already exists. ";
            error << "The new MODBUSRecordingUnit will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        // This is the only object with this entityKey, so add it to the map.
        unsigned short recordingUnitId = TA_MODBUS::getIdFromAddress( recordingUnit.getAddress() );
        m_recordingUnits[ entityKey ] = &recordingUnit;
        m_idToRecordingUnit[ recordingUnitId ] = &recordingUnit;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added MODBUSRecordingUnit with entityKey %ul", entityKey );
    }


    //
    // removeRecordingUnit
    //
    void MODBUSSwitchManager::removeRecordingUnit( unsigned long entityKey )
    {
        RecordingUnitMap::iterator it = m_recordingUnits.find( entityKey );
        if ( m_recordingUnits.end() != it )
        {
            m_idToRecordingUnit.erase( TA_MODBUS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_recordingUnits.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed MODBUSRecordingUnit with entityKey %ul", entityKey );
    }


    //
    // addCamera
    //
    void MODBUSSwitchManager::addCamera( MODBUSCamera& camera )
    {
        unsigned long entityKey = camera.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_cameras.end() != m_cameras.find( entityKey ) )
        {
            std::stringstream error;
            error << "A MODBUSCamera with the entityKey " << entityKey << " already exists. ";
            error << "The new MODBUSCamera will not be added.";
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
            "Added MODBUSCamera with entityKey %ul", entityKey );
    }


    //
    // removeCamera
    //
    void MODBUSSwitchManager::removeCamera( unsigned long entityKey )
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
            "Removed MODBUSCamera with entityKey %ul", entityKey );
    }


    //
    // addQuad
    //
    void MODBUSSwitchManager::addQuad( MODBUSQuad& quad )
    {
        unsigned long entityKey = quad.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_quads.end() != m_quads.find( entityKey ) )
        {
            std::stringstream error;
            error << "A MODBUSQuad with the entityKey " << entityKey << " already exists. ";
            error << "The new MODBUSQuad will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        m_quads[ entityKey ] = &quad;
        m_inputTypes[ entityKey ] = QUAD;
		/*
        unsigned short quadId = TA_MODBUS::getIdFromAddress( quad.getAddress() );
        m_idToQuad[ quadId ] = &quad;
		*/
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added MODBUSQuad with entityKey %ul", entityKey );
    }


    //
    // removeQuad
    //
    void MODBUSSwitchManager::removeQuad( unsigned long entityKey )
    {
        QuadMap::iterator it = m_quads.find( entityKey );
        if ( m_quads.end() != it )
        {
            //m_idToQuad.erase( TA_MODBUS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_quads.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed MODBUSQuad with entityKey %ul", entityKey );
    }


    //
    // addBVSStage
    //
  /*  void MODBUSSwitchManager::addBVSStage( MODBUSBVSStage& bvsStage )
    {
        unsigned long entityKey = bvsStage.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_bvsStages.end() != m_bvsStages.find( entityKey ) )
        {
            std::stringstream error;
            error << "A MODBUSBVSStage with the entityKey " << entityKey << " already exists. ";
            error << "The new MODBUSBVSStage will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        

        // This is the only object with this entityKey, so add it to the maps.
        unsigned short bvsStageId = TA_MODBUS::getIdFromAddress( bvsStage.getAddress() );
        m_bvsStages[ entityKey ] = &bvsStage;
        m_idToBVSStage[ bvsStageId ] = &bvsStage;
        m_inputTypes[ entityKey ] = BVS_STAGE;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added MODBUSBVSStage with entityKey %ul", entityKey );
    }


    //
    // removeBVSStage
    //
    void MODBUSSwitchManager::removeBVSStage( unsigned long entityKey )
    {
        BVSStageMap::iterator it = m_bvsStages.find( entityKey );
        if ( m_bvsStages.end() != it )
        {
            m_idToBVSStage.erase( TA_MODBUS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_bvsStages.erase( it );
        }

        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed MODBUSBVSStage with entityKey %ul", entityKey );
    }
*/

    //
    // addSequence
    //
    void MODBUSSwitchManager::addSequence( MODBUSSequence& sequence )
    {
        unsigned long entityKey = sequence.getKey();
        
        // If an object with this entityKey already exists - throw an exception.
        
        if ( m_sequences.end() != m_sequences.find( entityKey ) )
        {
            std::stringstream error;
            error << "A MODBUSSequence with the entityKey " << entityKey << " already exists. ";
            error << "The new MODBUSSequence will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }
        
        // This is the only object with this entityKey, so add it to the map.
        m_sequences[ entityKey ] = &sequence;
        m_inputTypes[ entityKey ] = SEQUENCE;
		/*
        unsigned short sequenceId = TA_MODBUS::getIdFromAddress( sequence.getAddress() );
        m_idToSequence[ sequenceId ] = &sequence;
		*/
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added MODBUSSequence with entityKey %ul", entityKey );
    }


    //
    // removeSequence
    //
    void MODBUSSwitchManager::removeSequence( unsigned long entityKey )
    {
        SequenceMap::iterator it = m_sequences.find( entityKey );
        if ( m_sequences.end() != it )
        {
            //m_idToSequence.erase( TA_MODBUS::getIdFromAddress( ( *it ).second->getAddress() ) );
            m_sequences.erase( it );
        }
        m_inputTypes.erase( entityKey );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed MODBUSSequence with entityKey %ul", entityKey );
    }


    //
    // addVideoOutput
    //
    void MODBUSSwitchManager::addVideoOutput( MODBUSVideoOutput& videoOutput )
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
        unsigned short videoOutputId = TA_MODBUS::getIdFromAddress( videoOutput.getAddress() );
        m_videoOutputs[ entityKey ] = &videoOutput;
        m_idToVideoOutput[ videoOutputId ] = &videoOutput;
		int displayOrder = videoOutput.getDisplayOrder();
		const unsigned long consoleKey = videoOutput.getConsoleKey();
		if(displayOrder >= 0)
		{
			m_consoleToDisplayOrderToOutput[consoleKey][displayOrder] = &videoOutput;
		}

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Added VideoOutput with entityKey %ul", entityKey );
    }


    //
    // removeVideoOutput
    //
    void MODBUSSwitchManager::removeVideoOutput( unsigned long entityKey )
    {
        VideoOutputMap::iterator it = m_videoOutputs.find( entityKey );
        if ( m_videoOutputs.end() != it )
        {
            m_videoOutputs.erase( it );
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
            "Removed VideoOutput with entityKey %ul", entityKey );
    }


    MODBUSSwitchManager::EMODBUSInputType MODBUSSwitchManager::getInputType( unsigned long entityKey )
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
    MODBUSCommunicationsHandler* MODBUSSwitchManager::getCommunicationsHandler()
    {
        if ( 0 == m_commsHandler )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_DOES_NOT_EXIST,
                "The MODBUSCommunicationsHandler does not exist. " ) );
        }
        return m_commsHandler;
    }


    //
    // addCommunicationsHander
    //
    void MODBUSSwitchManager::addCommunicationsHander( MODBUSCommunicationsHandler& commsHandler )
    {
        if ( 0 != m_commsHandler )
        {
            std::stringstream error;
            error << "The MODBUSCommunicationsHandler already exists. ";
            error << "The new MODBUSCommunicationsHandler will not be added.";
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::OBJECT_ALREADY_EXISTS,
                error.str().c_str() ) );
        }

        m_commsHandler = &commsHandler;

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Added MODBUSCommunicationsHandler" );
    }


    //
    // removeCommunicationsHander
    //
    void MODBUSSwitchManager::removeCommunicationsHander()
    {
        // Don't worry about deleting this object - that will be handled elsewhere.

        m_commsHandler = 0;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Removed MODBUSCommunicationsHandler." );
    }


    //
    // getStateValidationCycle
    //
	/*
    StateValidationCycle& MODBUSSwitchManager::getStateValidationCycle()
    {
        return *m_stateValidationCycle;
    }
	*/


	//TD16761 Mintao++
	std::map< unsigned short, TA_Base_Bus::DataPoint* > MODBUSSwitchManager::getAllDataPoints()
    {
        return m_idToDataPoint;
    }

    void MODBUSSwitchManager::registerDataPointFactory(TA_Base_Bus::DataPointFactory* dataPointFactory)
    {
        m_dataPointFactory = dataPointFactory;
    }
    

} // TA_IRS_App

