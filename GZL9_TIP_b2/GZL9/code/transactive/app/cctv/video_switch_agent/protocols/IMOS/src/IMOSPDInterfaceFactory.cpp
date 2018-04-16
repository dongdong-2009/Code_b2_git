/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSInterfaceFactory.cpp $
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

#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSPDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCamera.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSCommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSwitchManager.h"

#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	PDInterfaceFactory* gIMOSFactory = new IMOSPDInterfaceFactory();
    //
    // PDInterfaceFactory
    //
    IMOSPDInterfaceFactory::IMOSPDInterfaceFactory() {};

    
    //
    // ~PDInterfaceFactory
    //
    IMOSPDInterfaceFactory::~IMOSPDInterfaceFactory() {};


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoInputs
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCommunicationsHandler
    //
    IPDCommunicationsHandler* IMOSPDInterfaceFactory::createCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData )
    {
        IMOSCommunicationsHandler* handler = 0;
		try
		{
			handler = new IMOSCommunicationsHandler( videoSwitchAgentData );
            IMOSSwitchManager::getInstance().addCommunicationsHander( *handler );
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
    IPDVideoInput* IMOSPDInterfaceFactory::createVideoInput( TA_Base_Core::VideoInputPtr videoInputData )
    {
        IMOSVideoInput* input = new IMOSVideoInput( videoInputData );
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
    bool IMOSPDInterfaceFactory::isSuperimposedTextControlSupported()
    {
        return false;
    }

    
    //
    // createSuperimposedTextControl
    //
    IPDSuperimposedTextControl* IMOSPDInterfaceFactory::createSuperimposedTextControl( TA_Base_Core::VideoInputPtr videoInputData )
    {
        TA_ASSERT( isSuperimposedTextControlSupported(), "Superimposed text control is not supported by the IMOS protocol." );
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
    IPDCamera* IMOSPDInterfaceFactory::createCamera( TA_Base_Core::CameraPtr cameraData )
    {
        IMOSCamera* camera = new IMOSCamera( cameraData );
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
    bool IMOSPDInterfaceFactory::isMovementControlSupported()
    {
        return false;
    }


    //
    // createMovementControl
    //
    IPDMovementControl* IMOSPDInterfaceFactory::createMovementControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isMovementControlSupported(), "Movement control is not supported by the IMOS protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDMovementControl interface is not required." );
        return 0;
    }

   
    //
    // isFocusIrisControlSupported
    //
    bool IMOSPDInterfaceFactory::isFocusIrisControlSupported()
    {
        return false;
    }


    //
    // createFocusIrisControl
    //
    IPDFocusIrisControl* IMOSPDInterfaceFactory::createFocusIrisControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isFocusIrisControlSupported(), "Focus and iris control is not supported by the IMOS protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDFocusIrisControl interface is not required." );
        return 0;
    }


    //
    // isWashWipeControlSupported
    //
    bool IMOSPDInterfaceFactory::isWashWipeControlSupported()
    {
        return false;
    }


    //
    // createWashWipeControl
    //
    IPDWashWipeControl* IMOSPDInterfaceFactory::createWashWipeControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isWashWipeControlSupported(), "Wash and wipe control is not supported by the IMOS protocol." );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDWashWipeControl interface is not required." );
        return 0;
    }


    //
    // createSequence
    //
    IPDSequence* IMOSPDInterfaceFactory::createSequence( TA_Base_Core::SequencePtr sequenceData )
    {
        IMOSSequence* sequence = new IMOSSequence( sequenceData );
        if ( 0 == sequence )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDSequence." ) );
        }
        return sequence;
    }


    //
    // createQuad
    //
    IPDQuad* IMOSPDInterfaceFactory::createQuad( TA_Base_Core::QuadPtr quadData )
    {
        IMOSQuad* quad = new IMOSQuad( quadData );
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
    IPDBVSStage* PDInterfaceFactory::createBVSStage( TA_Base_Core::BVSStagePtr BVSStageData )
    {
        IMOSBVSStage* bvsStage = new IMOSBVSStage( BVSStageData );
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
    IPDVideoOutput* IMOSPDInterfaceFactory::createVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData )
    {
        IMOSVideoOutput* output = new IMOSVideoOutput( videoOutputData );
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
    IPDRecordingUnit* IMOSPDInterfaceFactory::createRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData )
    {
        IMOSRecordingUnit* recordingUnit = new IMOSRecordingUnit( recordingUnitData );
        if ( 0 == recordingUnit )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDRecordingUnit." ) );
        }
        return recordingUnit;
    }



} // TA_IRS_App
