#if !defined(IPDMovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_)
#define IPDMovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_


/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDMovementControl.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the pan, tilt, zoom and preset
  * operations for a Camera.  
  * This interface is used by the MovementControl class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include <utility>
#include <vector>

#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/MovementControlCorbaDef.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/CameraCorbaDef.h"

namespace TA_IRS_App
{
    class IPDMovementControl
    {
    public:
        /**
          * ~IPDMovementControl
          *
          * Standard destructor.
          */

        virtual ~IPDMovementControl() {};
        
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////


        /**
          * updatePreset
          *
          * Updates the given preset.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * The index needs to be checked to validate that it is within
          * the modifiable range.
          *
          * @param      unsigned short index
          *             The index of the preset to update.
          * @param      const std::string& name
          *             The new name for the preset.
          */

        virtual void updatePreset( unsigned short index, const std::string& name ) = 0;
        
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
          * @param      const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state
          *             The new state of the Camera.
          */

        virtual void updateState( const TA_Base_Bus_GZL9::CameraCorbaDef::CameraState& state ) = 0;

        ///////////////////////////////////////////////////////////////////
        //
        // Movement Controls
        //
        ///////////////////////////////////////////////////////////////////

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
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The speed value is invalid.
          *             5. The agent is in Monitor mode.
          *             6. The user does not have sufficient privileges for this operation.
          *             7. There is a communications failure with the hardware.
          *             8. The Camera address cannot be retrieved from the database.
          */

        virtual void panAndTilt( TA_Base_Bus_GZL9::MovementControlCorbaDef::EPanDirection panDirection,
                                 unsigned short panSpeed,
                                 TA_Base_Bus_GZL9::MovementControlCorbaDef::ETiltDirection tiltDirection,
                                 unsigned short tiltSpeed ) = 0;


        ///////////////////////////////////////////////////////////////////
        //
        // Pan Controls
        //
        ///////////////////////////////////////////////////////////////////

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
          * @param      bool isContinuous
          *             If true =>  Starts the camera panning left and continues
          *                         panning until panHalt() is called.
          *             If false => Pans the camera left one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void panLeft( unsigned short speed,
                              bool isContinuous ) = 0;

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
          * @param      bool isContinuous
          *             If true =>  Starts the camera panning right and continues
          *                         panning until panHalt() is called.
          *             If false => Pans the camera left one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void panRight( unsigned short speed,
                               bool isContinuous ) = 0;

        /**
          * panHalt
          *
          * Stops panning the Camera.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void panHalt() = 0;

        ///////////////////////////////////////////////////////////////////
        //
        // Tilt Controls
        //
        ///////////////////////////////////////////////////////////////////

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
          * @param      bool isContinuous
          *             If true =>  Starts the camera tilting up and continues
          *                         tilting until tiltHalt() is called.
          *             If false => Tilts the camera up one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void tiltUp( unsigned short speed,
                             bool isContinuous ) = 0;

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
          * @param      bool isContinuous
          *             If true =>  Starts the camera tilting down and continues
          *                         tilting until tiltHalt() is called.
          *             If false => Tilts the camera down one position then stops.
          *             NB: This parameter may be ignored if the protocol or 
          *             Camera does not support this functionality.
           * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void tiltDown( unsigned short speed,
                               bool isContinuous ) = 0;

        /**
          * tiltHalt
          *
          * Stops tilting the Camera.
          *
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void tiltHalt() = 0;

        ///////////////////////////////////////////////////////////////////
        //
        // Zoom Controls
        //
        ///////////////////////////////////////////////////////////////////

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
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void zoomIn( unsigned short speed ) = 0;

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
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The speed value is invalid.
          *             2. There is a communications failure with the hardware.
          *             3. The Camera address cannot be retrieved from the database.
          */

        virtual void zoomOut( unsigned short speed ) = 0;

        /**
          * zoomHalt
          *
          * Stops zooming the Camera.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void zoomHalt() = 0;

        ///////////////////////////////////////////////////////////////////
        //
        // Preset Controls
        //
        ///////////////////////////////////////////////////////////////////
                
        /** 
          * activatePreset
          *
          * Activates the given preset => moves the Camera to the given preset.
          *
          * The index has already been checked to validate that it is within
          * the allowable range.
          *
          * @param      unsigned short index  
          *             The index of the preset to activate.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */
        
        virtual void activatePreset( unsigned short index ) = 0;

        /** 
          * getActivePreset
          *
          * Returns the currently activated preset.
          *
          * @return     TA_IRS_App::MovementControlCorbaDef::CameraPreset*
          *             The active preset.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset* getActivePreset() = 0;

        /** 
          * getAllPresets
          *
          * Returns all the stored presets.
          *
          * @return     TA_IRS_App::MovementControlCorbaDef::CameraPresetSequence*   
          *             A vector of all stored presets or 
          *             an empty vector if there are none.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */
        
        virtual TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPresetSequence* getAllPresets() = 0;

        /** 
          * storePreset
          *
          * Stores the current position (pan/tilt/zoom/focus/iris... 
          * depending on hardware) of the Camera with the given name and index.
          *
          * The index has already been checked to validate that it is within
          * the modifiable range.
          *
          * @param      const TA_IRS_App::MovementControlCorbaDef::CameraPreset& preset
          *             The preset details.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */
        
        virtual void storePreset( const TA_Base_Bus_GZL9::MovementControlCorbaDef::CameraPreset& preset ) = 0;

        /** 
          * getFullPresetRange
          *
          * Returns the range of presets that can be activated.
          *
          * @param      unsigned short& minimumIndex
          *             Will contain the minimum preset index that can be activated.
          * @param      unsigned short& maximumIndex
          *             Will contain the maximum preset index that can be activated.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void getFullPresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex ) = 0;

        /** 
          * getModifiablePresetRange
          *
          * Returns the range of presets that can be modified.
          *
          * @param      unsigned short& minimumIndex
          *             Will contain the minimum preset index that can be modified.
          * @param      unsigned short& maximumIndex
          *             Will contain the maximum preset index that can be modified.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void getModifiablePresetRange( unsigned short& minimumIndex, unsigned short& maximumIndex ) = 0;

        /** 
          * setPresetName
          *
          * Sets the name of the preset with the given index.
          * The name can be cleared by passing in "" as the name.
          *
          * The index has already been checked to validate that it is within
          * the modifiable range.
          *
          * @param      unsigned short index
          *             The index of the preset to be altered.
          * @param      const std::string& name   
          *             The new name for the preset.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void setPresetName( unsigned short index, const std::string& name ) = 0;

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
          * @param      TA_Visual::EMovementFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::EMovementFeature feature ) = 0;
 
    }; // IPDMovementControl

}    // TA_IRS_App

#endif // !defined(IPDMovementControl_A5A991A4_216B_4ce0_B9CE_BF2D06785816__INCLUDED_)
