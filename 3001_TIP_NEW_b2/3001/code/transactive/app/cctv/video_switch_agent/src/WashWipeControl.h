#if !defined(WashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_)
#define WashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/WashWipeControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Handles washing and wiping control for a Camera.
  * Implements the IWashWipeControl interface.
  * Uses the IPDWashWipeControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/WashWipeControlCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDWashWipeControl.h"     // PD => ProtocolDependent
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"

#include "core/corba/src/ServantBase.h"

// Forward declarations.

namespace TA_IRS_App
{
    class Camera;
}

namespace TA_IRS_App
{
    class WashWipeControl : public virtual POA_TA_Base_Bus::WashWipeControlCorbaDef,
                            public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * WashWipeControl
          *
          * Constructor that creates the IPDWashWipeControl interface.
          *
          * @param      const Camera& camera
          *             The Camera object that this interface 
          *             supplies the wash and wipe controls for.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the protocol dependent initialisation fails.
          */

        WashWipeControl( Camera& camera );

        /**
          * ~WashWipeControl
          *
          * Standard destructor.
          */

        virtual ~WashWipeControl() {};

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
          * @param      TA_Visual::EWashWipeFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::EWashWipeFeature feature );

        ///////////////////////////////////////////////////////////////////////
        //
        // WashWipeControlCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////
        //
        // Wash Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * startWashing
          *
          * Starts washing the Camera.
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
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

    	virtual void startWashing( const char* sessionId );

        /**
          * stopWashing
          *
          * Stops washing the Camera
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
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

    	virtual void stopWashing( const char* sessionId );
        
        ///////////////////////////////////////////////////////////////////////
        //
        // Wipe Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * startWiping
          *
          * Starts wiping the Camera.
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
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

    	virtual void startWiping( const char* sessionId );

        /**
          * stopWiping
          *
          * Stops wiping the Camera.
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
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

    	virtual void stopWiping( const char* sessionId );


        /**
          * isWiperOn
          *
          * Returns the current state of the wiper.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @return     bool
          *             The wiper status.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
          */

    	virtual bool isWiperOn( const char* sessionId );


        /**
          * isWasherOn
          *
          * Returns the current state of the washer.
          *
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @return     bool
          *             The washer status.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
          */

    	virtual bool isWasherOn( const char* sessionId );
    
    private:
        //
        // Disable copy constructor and assignment operator.
        //

        WashWipeControl( const WashWipeControl& theWashWipeControl );
        WashWipeControl& operator=( const WashWipeControl& theWashWipeControl );

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
          * Checks that this particular Camera and the protocol support the
          * given feature.  If not, an exception is thrown.
          *
          * @param      TA_Visual::EWashWipeFeature feature
          *             The feature to verify.
          *
          * @exception  NotSupportedException
          *             Thrown if the Camera or the protocol does not support this feature.
          */
 
        void verifyFeatureIsSupported( TA_Visual::EWashWipeFeature feature );

        //
        // The Camera object that this object supplies the wash and wipe controls for.
        //

        Camera& m_camera;
        
        //
        // Object that handles the protocol dependent (PD/pd) parts of the functions
        // managing Camera washing and wiping.  
        // This is a smart pointer so it will automatically be cleaned up.
        //

        std::auto_ptr< IPDWashWipeControl > m_pdWashWipeControl;

        //
        // Determines if users have the rights to perform certain actions.
        //

        UserPrivilegeVerifier m_userPrivilegeVerifier;

        //
        // static constants
        //

        static const std::string IS_WIPER_ON;
        static const std::string IS_WASHER_ON;

    }; // WashWipeControl

} // TA_IRS_App
    
#endif // !defined(WashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_)
