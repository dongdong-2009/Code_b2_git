#if !defined(MovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_)
#define MovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/MovementControl.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Controls pan, tilt, zoom and presets for a Camera.
  * Implements the IMovementControl interface. 
  * Uses the IPDMovementControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <string>
#include <vector>
#include <memory>
#include <utility>

#include "bus/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDMovementControl.h"     // PD => ProtocolDependent
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"

#include "core/corba/src/ServantBase.h"

// Forward declarations

namespace TA_IRS_App
{
    class Camera;
}

namespace TA_IRS_App
{
    class MovementControl : public virtual POA_TA_Base_Bus::MovementControlCorbaDef,
                            public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * MovementControl
          *
          * Constructor that creates the IPDMovementControl interface.
          *
          * @param      const Camera& camera
          *             The Camera object that this interface 
          *             supplies the movement controls for.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the protocol dependent initialisation fails.
          */

        MovementControl( Camera& camera );

        /**
          * ~MovementControl
          *
          * Standard destructor.
          */

        virtual ~MovementControl() {};

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
          * @param      TA_Visual::EMovementFeature feature
          *             The feature to check.
          */

        bool isFeatureSupported( TA_Visual::EMovementFeature feature );

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updatePresetName
          *
          * Updates the name of the given preset.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails (for example if the preset is invalid).  If there is a failure, 
          * it is simply logged.
          *
          * @param      unsigned short index
          *             The index of the preset to update.
          * @param      const std::string& name
          *             The new name for the preset.
          */

        void updatePresetName( unsigned short index, const std::string& name );
        
        /**
          * updateState
          *
          * Updates the active preset index.  
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
        // MovementControlCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////

	    /**
          * panAndTilt
          *
          * Pans and tilts the Camera in the given directions at the given speeds.
          * If proportional gain is not supported, the speed value will be ignored
          * and the Camera will pan and tilt at either its default speed or its 
          * slowest speed depending on the Camera type.
          *
          * @param      EPanDirection panDirection
          *             The direction to Pan (left, right or halt).
          * @param      unsigned short panSpeed
          *             The speed to pan the Camera.
          * @param      ETiltDirection tiltDirection
          *             The direction to Tilt (up, down or halt).
          * @param      unsigned short tiltSpeed
          *             The speed to tilt the Camera.
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. One or more speed or direction values are invalid.
          *             5. The agent is in Monitor mode.
          *             6. The user does not have sufficient privileges for this operation.
          *            	7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        void panAndTilt( TA_Base_Bus::MovementControlCorbaDef::EPanDirection panDirection,
                         unsigned short panSpeed,
                         TA_Base_Bus::MovementControlCorbaDef::ETiltDirection tiltDirection,
                         unsigned short tiltSpeed,
                         const char* sessionId );


        ///////////////////////////////////////////////////////////////////////
        //
        // Pan Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * panLeft
          *
          * Pans the Camera left at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will pan at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed    
          *             The speed to pan the Camera left at.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * @param      bool isContinuous
          *             If true =>  Starts the camera panning left and continues
          *                         panning until panHalt() is called.
          *             If false => Pans the camera left one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void panLeft( unsigned short speed, const char* sessionId, bool isContinuous );

        /**
          * panRight
          *
          * Pans the Camera right at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will pan at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed
          *             The speed to pan the Camera right at.
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          * @param      bool isContinuous
          *             If true =>  Starts the camera panning right and continues
          *                         panning until panHalt() is called.
          *             If false => Pans the camera left one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void panRight( unsigned short speed, const char* sessionId, bool isContinuous );

        /**
          * panHalt
          *
          * Stops panning the Camera.
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

        virtual void panHalt( const char* sessionId );

        ///////////////////////////////////////////////////////////////////////
        //
        // Tilt Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * tiltUp
          *
          * Tilts the Camera up at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will tilt at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed    
          *             The speed to tilt the Camera up at.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * @param      bool isContinuous
          *             If true =>  Starts the camera tilting up and continues
          *                         tilting until tiltHalt() is called.
          *             If false => Tilts the camera up one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void tiltUp( unsigned short speed, const char* sessionId, bool isContinuous );

        /**
          * tiltDown
          *
          * Tilts the Camera down at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will tilt at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed    
          *             The speed to tilt the Camera down at.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * @param      bool isContinuous
          *             If true =>  Starts the camera tilting down and continues
          *                         tilting until tiltHalt() is called.
          *             If false => Tilts the camera down one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void tiltDown( unsigned short speed, const char* sessionId, bool isContinuous );

        /**
          * tiltHalt
          *
          * Stops tilting the Camera.
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

        virtual void tiltHalt( const char* sessionId );

        ///////////////////////////////////////////////////////////////////////
        //
        // Zoom Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * zoomIn
          *
          * Zooms the Camera in at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will zoom at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed    
          *             The speed to zoom the Camera in at.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void zoomIn( unsigned short speed, const char* sessionId );

        /**
          * zoomOut
          *
          * Zooms the Camera out at the given speed.  If proportional gain
          * is not supported, the speed value will be ignored and the Camera
          * will zoom at either its default speed or its slowest speed 
          * depending on the Camera type.
          *
          * @param      unsigned short speed    
          *             The speed to zoom the Camera out at.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void zoomOut( unsigned short speed, const char* sessionId );

        /**
          * zoomHalt
          *
          * Stops zooming the Camera.
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

        virtual void zoomHalt( const char* sessionId );

        ///////////////////////////////////////////////////////////////////////
        //
        // Preset Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /** 
          * activatePreset
          *
          * Activates the given preset => moves the Camera to the given preset.
          *
          * @param      unsigned short index  
          *             The index of the preset to activate.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The preset index is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          */
        
        virtual void activatePreset( unsigned short index, const char* sessionId );

        /** 
          * getActivePreset
          *
          * Returns the currently activated preset.
          *
          * @return     MovementControlCorbaDef::CameraPreset*
          *             The active preset.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
		  *				1. The Camera or the protocol does not support presets.
		  *				2. The agent is in Monitor mode.
		  *				3. There is a communications failure with the hardware.
          */

        virtual TA_Base_Bus::MovementControlCorbaDef::CameraPreset* getActivePreset();

        /** 
          * getAllPresets
          *
          * Returns all the stored presets.
          *
          * @return     MovementControlCorbaDef::CameraPresetSequence    
          *             A vector of all stored presets or 
          *             an empty vector if there are none.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
		  *				1. The Camera or the protocol does not support presets.
		  *				2. The agent is in Monitor mode.
		  *				3. There is a communications failure with the hardware.
          */

        virtual TA_Base_Bus::MovementControlCorbaDef::CameraPresetSequence* getAllPresets();

        /** 
          * storePreset
          *
          * Stores the current position (pan/tilt/zoom/focus/iris... 
          * depending on hardware) of the Camera along with the given name and index.
          *
          * @param      const TA_Base_Bus::MovementControlCorbaDef::CameraPreset& preset
          *             The preset details.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The preset index is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          */
        
        virtual void storePreset( const TA_Base_Bus::MovementControlCorbaDef::CameraPreset& preset, const char* sessionId );

        /** 
          * getFullPresetRange
          *
          * Returns the range of presets that can be activated.
          *
          * @param		unsigned short& minimumIndex
		  *				Will contain the minimum preset index that can be activated.
		  *	@param		unsigned short& maximumIndex
          *             Will contain the maximum preset index that can be activated.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
		  *				1. The Camera or the protocol does not support presets.
		  *				2. The agent is in Monitor mode.
		  *				3. There is a communications failure with the hardware.
          */

        virtual void getFullPresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex );

        /** 
          * getModifiablePresetRange
          *
          * Returns the range of presets that can be modified.
          *
          * @param		unsigned short& minimumIndex
		  *				Will contain the minimum preset index that can be modified.
		  *	@param		unsigned short& maximumIndex
          *             Will contain the maximum preset index that can be modified.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
		  *				1. The Camera or the protocol does not support presets.
		  *				2. The agent is in Monitor mode.
		  *				3. There is a communications failure with the hardware.
          */

        virtual void getModifiablePresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex );

        /** 
          * setPresetName
          *
          * Sets the name of the preset with the given index.
          * The name can be cleared by passing in "" as the name.
          *
          * @param      unsigned short index
          *             The index of the preset to be altered.
          * @param      const char* name   
          *             The new name for the preset.
          * @param      const char* sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The preset index is invalid.
		  *				5. The agent is in Monitor mode.
		  *				6. The user does not have sufficient privileges for this operation.
		  *				7. There is a communications failure with the hardware.
          */

        virtual void setPresetName( unsigned short index, const char* name, const char* sessionId );

    private:

        //
        // Disable copy constructor and assignment operator
        //

        MovementControl( const MovementControl& theMovementControl );
        MovementControl& operator=( const MovementControl& theMovementControl );
        
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
          * @param      TA_Visual::EMovementFeature feature
          *             The feature to verify.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the Camera or the protocol does not support this feature.
          */
 
        void verifyFeatureIsSupported( TA_Visual::EMovementFeature feature );

        //
        // The Camera object that this object supplies the movement controls for.
        //

        Camera& m_camera;
        
        //
        // Object that handles the protocol dependent (PD/pd) parts of the functions
        // managing Camera movement.  
        // This is a smart pointer so it will automatically be cleaned up.
        //

        std::auto_ptr< IPDMovementControl > m_pdMovementControl;

        //
        // Determines if users have the rights to perform certain actions.
        //

        UserPrivilegeVerifier m_userPrivilegeVerifier;

        //
        // static constants
        //

        static const std::string GET_ACTIVE_PRESET;
        static const std::string GET_ALL_PRESETS;
        static const std::string GET_FULL_PRESET_RANGE;
        static const std::string GET_MODIFIABLE_PRESET_RANGE;


    }; // MovementControl

}    // TA_IRS_App

#endif // !defined(MovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_)
