/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEInterfaceFactory.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This class is a factory that creates protocol dependent objects
  * for the Video Switch Agent.  For all functions returning new objects
  * in this class, it is the *CALLER'S* responsibility to clean up the object.
  *
  * When implementing a protocol library for the Video Switch Agent,
  * a cpp implementation file of this class should be created within
  * the library.
  */

#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECamera.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COECommunicationsHandler.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COEVideoOutput.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
    //
    // PDInterfaceFactory
    //
    PDInterfaceFactory::PDInterfaceFactory() {};

    
    //
    // ~PDInterfaceFactory
    //
    PDInterfaceFactory::~PDInterfaceFactory() {};


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoInputs
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCommunicationsHandler
    //
    IPDCommunicationsHandler* PDInterfaceFactory::createCommunicationsHandler( TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData )
    {
        COECommunicationsHandler* handler = new COECommunicationsHandler( videoSwitchAgentData );
        if ( 0 == handler )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDCommunicationsHandler." ) );
        }
        try
        {
            COESwitchManager::getInstance().addCommunicationsHander( *handler );
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& )
        {
            delete handler;
            handler = 0;
            throw;
        }

        return handler;
    }
    
   
    //
    // createVideoInput
    //
    IPDVideoInput* PDInterfaceFactory::createVideoInput( TA_Base_Core::VideoInputPtr videoInputData)
    {
        COEVideoInput* input = new COEVideoInput( videoInputData );
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
    bool PDInterfaceFactory::isSuperimposedTextControlSupported()
    {
        return false;
    }

    
    //
    // createSuperimposedTextControl
    //
    IPDSuperimposedTextControl* PDInterfaceFactory::createSuperimposedTextControl( TA_Base_Core::VideoInputPtr videoInputData )
    {
        TA_ASSERT( isSuperimposedTextControlSupported(), "Superimposed text control is not supported by the COE protocol." );
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
    IPDCamera* PDInterfaceFactory::createCamera( TA_Base_Core::CameraPtr cameraData)
    {
        COECamera* camera = new COECamera( cameraData );
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
    bool PDInterfaceFactory::isMovementControlSupported()
    {
        return false;
    }


    //
    // createMovementControl
    //
    IPDMovementControl* PDInterfaceFactory::createMovementControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isMovementControlSupported(), "Movement control is not supported by the COE protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDMovementControl interface is not required." );
        return 0;
    }

    //
    // isFocusIrisControlSupported
    //
    bool PDInterfaceFactory::isFocusIrisControlSupported()
    {
        return false;
    }


    //
    // createFocusIrisControl
    //
    IPDFocusIrisControl* PDInterfaceFactory::createFocusIrisControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isFocusIrisControlSupported(), "Focus and iris control is not supported by the COE protocol." );
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDFocusIrisControl interface is not required." );
        return 0;
    }


    //
    // isWashWipeControlSupported
    //
    bool PDInterfaceFactory::isWashWipeControlSupported()
    {
        return false;
    }


    //
    // createWashWipeControl
    //
    IPDWashWipeControl* PDInterfaceFactory::createWashWipeControl( TA_Base_Core::CameraPtr cameraData )
    {
        TA_ASSERT( isWashWipeControlSupported(), "Wash and wipe control is not supported by the COE protocol." );
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Returning 0 because the IPDWashWipeControl interface is not required." );
        return 0;
    }
    // createSequence
    //
    IPDSequence* PDInterfaceFactory::createSequence( TA_Base_Core::SequencePtr sequenceData)
    {
        COESequence* sequence = new COESequence( sequenceData );
        if ( 0 == sequence )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDSequence." ) );
        }
        return sequence;
    }


    //
    // createVideoOutput
    //
    IPDVideoOutput* PDInterfaceFactory::createVideoOutput( TA_Base_Core::VideoOutputPtr videoOutputData)
    {
        COEVideoOutput* output = new COEVideoOutput( videoOutputData );
        if ( 0 == output )
        {
            TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException( 
                TA_Base_Core::VideoSwitchAgentInternalException::CREATION_FAILED, "Failed to create IPDVideoOutput." ) );
        }
        return output;
    }

} // TA_IRS_App
