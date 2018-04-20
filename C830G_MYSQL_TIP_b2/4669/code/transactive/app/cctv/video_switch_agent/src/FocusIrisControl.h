#if !defined(FocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_)
#define FocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/FocusIrisControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/25 17:58:20 $
  * Last modified by:  $Author: builder $
  *
  * Handles the focus and iris control for a Camera.
  * Implements the IFocusIrisControl interface.
  * Uses the IPDFocusIrisControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/FocusIrisControlCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDFocusIrisControl.h"     // PD => ProtocolDependent
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"

#include "core/corba/src/ServantBase.h"

// Forward declarations

namespace TA_IRS_App
{
    class Camera;
}

namespace TA_IRS_App
{
    class FocusIrisControl : public virtual POA_TA_Base_Bus::FocusIrisControlCorbaDef,
                             public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * FocusIrisControl
          *
          * Constructor that creates the IPDFocusIrisControl interface.
          *
          * @param      const Camera& camera
          *             The Camera object that this interface 
          *             supplies the focus and iris controls for.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the protocol dependent initialisation fails.
          */

        FocusIrisControl( Camera& camera );

        /**
          * ~FocusIrisControl
          *
          * Standard destructor.
          */

        virtual ~FocusIrisControl() {};

        /**
          * isFeatureSupported
          *
          * Returns true if the feature is supported by this Camera and by 
          * the protocol.
          *
          * @return     bool
          *             True if the feature is supported.
          *             False otherwise, even if the feature is not recognised
          *             or if a database problem occurs.
          * 
          * @param      TA_Visual::EFocusIrisFeature feature
          *             The feature to check.
          */

        bool isFeatureSupported( TA_Visual::EFocusIrisFeature feature );
        
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Note: This method does nothing because AutoFocus, AutoIris
          * and IrisPeakWhiteInversion are not supported by the Pelco protocol.
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

        void updateState( const TA_Base_Bus::CameraCorbaDef::CameraState& state );

        ///////////////////////////////////////////////////////////////////////
        //
        // FocusIrisControlCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////

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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

    	virtual bool isAutoFocusOn();

        /**
          * setAutoFocusOn
          *
          * Turns auto focus on (or off).
          *
          * @param      bool isAutoFocusOn
          *             True  => turn auto focus on.
          *             False => turn auto focus off.
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          */

    	virtual void setAutoFocusOn( bool isAutoFocusOn, const char* sessionId );

        /**
          * focusNear
          *
          * Starts focusing the Camera nearer.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void focusNear( const char* sessionId );

        /**
          * focusFar
          *
          * Starts focusing the Camera farther away.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void focusFar( const char* sessionId );

        /**
          * focusHalt
          *
          * Stops changing the focus.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void focusHalt( const char* sessionId );

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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

    	virtual bool isAutoIrisOn();

        /**
          * setAutoIrisOn
          *
          * Turns auto iris on (or off).
          *
          * @param      bool isAutoIrisOn
          *             True  => turn auto iris on.
          *             False => turn auto iris off.
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          */

    	virtual void setAutoIrisOn( bool isAutoIrisOn, const char* sessionId );

        /**
          * openIris
          *
          * Starts opening the Camera iris.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void openIris( const char* sessionId );

        /**
          * closeIris
          *
          * Starts closing the Camera iris.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void closeIris( const char* sessionId );

        /**
          * irisHalt
          *
          * Stops changing the iris width.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The Camera address cannot be retrieved from the database.
          */

    	virtual void irisHalt( const char* sessionId );

        /**
          * isIrisPeakWhiteInversionOn
          *
          * Identifies if Peak White Inversion is turned on.
          *
          * @return     bool
          *             True if peak white inversion is on.
          *             False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

    	virtual bool isIrisPeakWhiteInversionOn();

        /**
          * setIrisPeakWhiteInversionOn
          *
          * Turns Peak White Inversion on (or off).
          *
          * @param      bool isIrisPeakWhiteInversionOn
          *             True =>
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          */

    	virtual void setIrisPeakWhiteInversionOn( bool isIrisPeakWhiteInversionOn, const char* sessionId );
    
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        FocusIrisControl( const FocusIrisControl& theFocusIrisControl );
        FocusIrisControl& operator=( const FocusIrisControl& theFocusIrisControl );
        
        /**
          * hasInitialised
          *
          * Identifies if the object was initialised successfully.
          * Only used to test if the precondition for the following methods are met.
          * 
          * @return     bool
          *             True if the object was initialised successfully.
          */

        bool hasInitialised() const;

        /**
          * verifyFeatureIsSupported
          *
          * Checks that the feature is supported by this Camera and by the protocol. 
          * If not, an exception is thrown.
          *
          * @param      TA_Visual::EFocusIrisFeature feature
          *             The feature to verify.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the Camera or the protocol does not support this feature.
          */
 
        void verifyFeatureIsSupported( TA_Visual::EFocusIrisFeature feature );

        //
        // The Camera object that this object supplies the focus and iris controls for.
        //

    	Camera& m_camera;
        
        //
        // Object that handles the protocol dependent (PD/pd) parts of the functions
        // managing Camera focus and iris control.  
        // This is a smart pointer so it will automatically be cleaned up.
        //

        std::auto_ptr< IPDFocusIrisControl > m_pdFocusIrisControl;

        //
        // Determines if users have the rights to perform certain actions.
        //

//         UserPrivilegeVerifier m_userPrivilegeVerifier;
        UserPrivilegeVerifier& m_userPrivilegeVerifier; // HuangQi++

        //
        // static constants
        //

        static const std::string IS_AUTO_FOCUS_ON;
        static const std::string SET_AUTO_FOCUS;
        static const std::string IS_AUTO_IRIS_ON;
        static const std::string SET_AUTO_IRIS;
        static const std::string IS_PEAK_WHITE_INVERTER_ON;
        static const std::string SET_PEAK_WHITE_INVERTER;

    }; // FocusIrisControl

} // TA_IRS_App
    
#endif // !defined(FocusIrisControl_5BC56742_3AA4_4284_9CC5_674B72EB3610__INCLUDED_)
