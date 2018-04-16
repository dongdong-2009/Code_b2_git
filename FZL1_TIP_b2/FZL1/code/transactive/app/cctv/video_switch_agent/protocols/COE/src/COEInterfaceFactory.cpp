/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEInterfaceFactory.cpp $
  * @author:   Rob Young
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
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
    PDInterfaceFactory::PDInterfaceFactory()
    {
    }
    //
    // ~PDInterfaceFactory
    //
    PDInterfaceFactory::~PDInterfaceFactory()
    {
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Protocol Dependent Object Creation Methods For VideoInputs
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCommunicationsHandler
    //
    IPDCommunicationsHandler* PDInterfaceFactory::createCommunicationsHandler(TA_Base_Core::VideoSwitchAgentPtr videoSwitchAgentData)
    {
        std::auto_ptr<COECommunicationsHandler> handler(new COECommunicationsHandler(videoSwitchAgentData));
        COESwitchManager::getInstance().addCommunicationsHander(*handler);
        return handler.release();
    }


    //
    // createVideoInput
    //
    IPDVideoInput* PDInterfaceFactory::createVideoInput(TA_Base_Core::VideoInputPtr videoInputData)
    {
        return new COEVideoInput(videoInputData);
    }


    //
    // createCamera
    //
    IPDCamera* PDInterfaceFactory::createCamera(TA_Base_Core::CameraPtr cameraData)
    {
        return new COECamera(cameraData);
    }


    // createSequence
    //
    IPDSequence* PDInterfaceFactory::createSequence(TA_Base_Core::SequencePtr sequenceData)
    {
        return new COESequence(sequenceData);
    }


    //
    // createVideoOutput
    //
    IPDVideoOutput* PDInterfaceFactory::createVideoOutput(TA_Base_Core::VideoOutputPtr videoOutputData)
    {
        return new COEVideoOutput(videoOutputData);
    }

} // TA_IRS_App
