/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSInterfaceFactory.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a factory that creates protocol dependent objects
  * for the Video Switch Agent.  For all functions returning new objects
  * in this class, it is the *CALLER'S* responsibility to clean up the object.
  *
  * When implementing a protocol library for the Video Switch Agent,
  * a cpp implementation file of this class should be created within
  * the library.
  */

#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSPDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCamera.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/MODBUS/src/MODBUSSwitchManager.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	PDInterfaceFactory* gMODBUSFactory = new MODBUSPDInterfaceFactory();
    //
    // PDInterfaceFactory
    //
    MODBUSPDInterfaceFactory::MODBUSPDInterfaceFactory() {};

    
    //
    // ~PDInterfaceFactory
    //
    MODBUSPDInterfaceFactory::~MODBUSPDInterfaceFactory() {};


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoInputs
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCommunicationsHandler
    //
    IPDCommunicationsHandler* MODBUSPDInterfaceFactory::createCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData )
    {
        MODBUSCommunicationsHandler* handler = NULL;
        try
        {
	        handler = new MODBUSCommunicationsHandler( videoSwitchAgentData );
			const TA_Base_Core::ILocation::ELocationType locationType =
				TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(videoSwitchAgentData->getLocation());
			if(TA_Base_Core::ILocation::OCC == locationType)
			{
				handler->setControlPrivilege(255);
			}
			else
			{
				handler->setControlPrivilege(0);
			}
            MODBUSSwitchManager::getInstance().addCommunicationsHander( *handler );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& )
        {
            delete handler;
            throw;
        }

        return handler;
    }
    
   
    //
    // createVideoInput
    //
    IPDVideoInput* MODBUSPDInterfaceFactory::createVideoInput( TA_Base_Core::VideoInputPtr videoInputData )
    {
        MODBUSVideoInput* input = new MODBUSVideoInput( videoInputData );
        if ( 0 == input )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoInput." ) );
        }
        return input;
    }

    
    //
    // isSuperimposedTextControlSupported
    //
    bool MODBUSPDInterfaceFactory::isSuperimposedTextControlSupported()
    {
        return false;
    }

    
    //
    // createSuperimposedTextControl
    //
    IPDSuperimposedTextControl* MODBUSPDInterfaceFactory::createSuperimposedTextControl( TA_Base_Core::VideoInputPtr videoInputData )
    {
        TA_ASSERT( isSuperimposedTextControlSupported(), "Superimposed text control is not supported by the MODBUS protocol." );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDSuperimposedTextControl interface is not required." );
        return 0;
    }

    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For Cameras
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCamera
    //
    IPDCamera* MODBUSPDInterfaceFactory::createCamera( TA_Base_Core::CameraPtr cameraData )
    {
        MODBUSCamera* camera = new MODBUSCamera( cameraData );
        if ( 0 == camera )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDCamera." ) );
        }
        return camera;
    }
    

    //
    // isMovementControlSupported
    //
    bool MODBUSPDInterfaceFactory::isMovementControlSupported()
    {
        return false;
    }


    //
    // createMovementControl
    //
    IPDMovementControl* MODBUSPDInterfaceFactory::createMovementControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isMovementControlSupported(), "Movement control is not supported by the MODBUS protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDMovementControl interface is not required." );
        return 0;
    }

   
    //
    // isFocusIrisControlSupported
    //
    bool MODBUSPDInterfaceFactory::isFocusIrisControlSupported()
    {
        return false;
    }


    //
    // createFocusIrisControl
    //
    IPDFocusIrisControl* MODBUSPDInterfaceFactory::createFocusIrisControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isFocusIrisControlSupported(), "Focus and iris control is not supported by the MODBUS protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDFocusIrisControl interface is not required." );
        return 0;
    }


    //
    // isWashWipeControlSupported
    //
    bool MODBUSPDInterfaceFactory::isWashWipeControlSupported()
    {
        return false;
    }


    //
    // createWashWipeControl
    //
    IPDWashWipeControl* MODBUSPDInterfaceFactory::createWashWipeControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isWashWipeControlSupported(), "Wash and wipe control is not supported by the MODBUS protocol." );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDWashWipeControl interface is not required." );
        return 0;
    }


    //
    // createSequence
    //
    IPDSequence* MODBUSPDInterfaceFactory::createSequence( TA_Base_Core::SequencePtr sequenceData )
    {
		try
		{
	        return new MODBUSSequence( sequenceData );
		}
		catch (...)
		{
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDSequence." ) );
		}
    }


    //
    // createQuad
    //
    IPDQuad* MODBUSPDInterfaceFactory::createQuad( TA_Base_Core::QuadPtr quadData )
    {
        MODBUSQuad* quad = new MODBUSQuad( quadData );
        if ( 0 == quad )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDQuad." ) );
        }
        return quad;
    }


    //
    // createBVSStage
    //
	/*
    IPDBVSStage* MODBUSPDInterfaceFactory::createBVSStage( TA_Base_Core::BVSStagePtr BVSStageData )
    {
        MODBUSBVSStage* bvsStage = new MODBUSBVSStage( BVSStageData );
        if ( 0 == bvsStage )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDBVSStage." ) );
        }
        return bvsStage;
    }*/


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoOutputs
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // createVideoOutput
    //
    IPDVideoOutput* MODBUSPDInterfaceFactory::createVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData )
    {
        MODBUSVideoOutput* output = new MODBUSVideoOutput( videoOutputData );
        if ( 0 == output )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoOutput." ) );
        }
        return output;
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoOutputs
    //
    ///////////////////////////////////////////////////////////////////////

    
    //
    // createRecordingUnit
    //
    IPDRecordingUnit* MODBUSPDInterfaceFactory::createRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData )
    {
        MODBUSRecordingUnit* recordingUnit = new MODBUSRecordingUnit( recordingUnitData );
        if ( 0 == recordingUnit )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDRecordingUnit." ) );
        }
        return recordingUnit;
    }



} // TA_IRS_App
