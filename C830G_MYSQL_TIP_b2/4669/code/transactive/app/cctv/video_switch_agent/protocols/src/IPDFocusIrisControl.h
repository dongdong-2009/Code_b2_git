#if !defined(IPDFocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_)
#define IPDFocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/src/IPDFocusIrisControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the focus and iris control for a Camera.
  * This interface is used by the FocusIrisControl class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"

namespace TA_IRS_App
{
    class IPDFocusIrisControl
    {
    public:
        /**
          * ~IPDFocusIrisControl
          *
          * Standard destructor.
          */

        virtual ~IPDFocusIrisControl() {};
        
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the FocusIrisControl for this Camera.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::CameraCorbaDef::CameraState& state
          *             The new state of the Camera.
          */

        virtual void updateState( const TA_Base_Bus::CameraCorbaDef::CameraState& state ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Focus Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * isAutoFocusOn
          *
          * Identifies if the Camera is in auto focus mode.
          *
          * @return     bool
          *             True if the Camera is in auto focus mode.
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual bool isAutoFocusOn() const = 0;

        /**
          * setAutoFocusOn
          *
          * Turns auto focus on (or off).
          *
          * @param      bool isAutoFocusOn
          *             True  => turn auto focus on.
          *             False => turn auto focus off.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void setAutoFocusOn( bool isAutoFocusOn ) = 0;

        /**
          * focusNear
          *
          * Starts focusing the Camera nearer.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void focusNear() = 0;

        /**
          * focusFar
          *
          * Starts focusing the Camera farther away.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void focusFar() = 0;

        /**
          * focusHalt
          *
          * Stops changing the focus.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void focusHalt() = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Iris Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * isAutoIrisOn
          *
          * Identifies if the Camera is in auto iris mode.
          *
          * @return     bool
          *             True if the Camera is in auto iris mode.
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual bool isAutoIrisOn() const = 0;

        /**
          * setAutoIrisOn
          *
          * Turns auto iris on (or off).
          *
          * @param      bool isAutoIrisOn
          *             True  => turn auto iris on.
          *             False => turn auto iris off.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void setAutoIrisOn( bool isAutoIrisOn ) = 0;

        /**
          * openIris
          *
          * Starts opening the Camera iris.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void openIris() = 0;

        /**
          * closeIris
          *
          * Starts closing the Camera iris.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void closeIris() = 0;

        /**
          * irisHalt
          *
          * Stops changing the iris width.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void irisHalt() = 0;

        /**
          * isIrisPeakWhiteInversionOn
          *
          * Identifies if IrisPeakWhiteInversion is turned on.
          *
          * @return     bool
          *             True if peak white inversion is on.
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual bool isIrisPeakWhiteInversionOn() const = 0;

        /**
          * setIrisPeakWhiteInversionOn
          *
          * Turns IrisPeakWhiteInversion on (or off).
          *
          * @param      bool isIrisPeakWhiteInversionOn
          *             True  => turn on iris peak white inversion.
          *             False => turn off iris peak white inversion.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void setIrisPeakWhiteInversionOn( bool isIrisPeakWhiteInversionOn ) = 0;
            
        ///////////////////////////////////////////////////////////////////////
        //
        // Supporting Methods
        //
        ///////////////////////////////////////////////////////////////////////
        
        /**
          * isFeatureSupported
          *
          * Returns true if the feature is supported by the protocol.
          * This DOES NOT test if this Camera supports the feature only if the
          * protocol this Camera uses supports it.
          *
          * @return     bool
          *             True if the feature is supported.
          *             False otherwise, even if the feature is not recognised.
          * 
          * @param      TA_Visual::EFocusIrisFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::EFocusIrisFeature feature ) = 0;

    }; // IPDFocusIrisControl

} // TA_IRS_App
    
#endif // !defined(IPDFocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_)
