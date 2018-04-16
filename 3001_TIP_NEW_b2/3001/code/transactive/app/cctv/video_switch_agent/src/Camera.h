#if !defined(Camera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
#define Camera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/Camera.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Provides access to the Camera state, supported features and Camera control.
  * Inherits from the VideoInput class and implements the ICameraCorbaDef interface.
  */

#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "app/cctv/video_switch_agent/src/MovementControl.h"
#include "app/cctv/video_switch_agent/src/FocusIrisControl.h"
#include "app/cctv/video_switch_agent/src/WashWipeControl.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDCamera.h"

#include "core/corba/src/ServantBase.h"
#include "core/naming/src/INamedObject.h"
#include "core/data_access_interface/entity_access/src/Camera.h"

namespace TA_IRS_App
{
    class Camera : public VideoInput,
                   public virtual POA_TA_Base_Bus::CameraCorbaDef,
                   public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * Camera
          *
          * Constructor that creates the control interfaces.  If any of the
          * control interfaces cannot be created, the constructor will succeed,
          * but the Camera will not support control of the interface that failed.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this Camera.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
          *             1. The entityData cannot be converted to Camera data.
          *             2. The protocol dependent initialisation fails.
          */

        Camera( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * ~Camera
          *
          * Standard destructor.
          */

        virtual ~Camera();

        /**
          * getCameraData
          *
          * Gets the database entity for this camera.
          *
          * @return     TA_Base_Core::Camera&
          *             The VideoInput data.
          */

        TA_Base_Core::CameraPtr& getCameraData();
        
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the Camera.  
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
        // CameraCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////
        
        /**
          * getSupportedFeatures
          *
          * Gets the features that this Camera supports.
          *
          * @return     TA_Base_Bus::CameraCorbaDef::SupportedCameraFeatures
          *             A struct detailing which camera features are supported.
		  *				If an error occurs while retrieving the information from
		  *				the database, it will be assumed that the feature is 
		  *				not supported.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
          */
 
        virtual TA_Base_Bus::CameraCorbaDef::SupportedCameraFeatures getSupportedFeatures();

        /**
          * getState
          *
          * Gets the current state of this Camera.
          *
          * @return     TA_Base_Bus::CameraCorbaDef::CameraState
          *             A struct detailing the current Camera state.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

		virtual TA_Base_Bus::CameraCorbaDef::CameraState getState();

		/** 
		  * isMovementControllable
		  *
		  * Identifies if this Camera supports at least one of the movement controls. 
		  * It should only be used to determine if the MovementControl interface exists.
		  *
		  * @return		bool
		  *				True if the MovementControl interface exists.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
		
		virtual bool isMovementControllable();

		/**
		  * getMovementControl
		  *
		  * Gets the MovementControl interface.
		  *
		  * @return		TA_Base_Bus::MovementControlCorbaDef_ptr
		  *				The interface to the control for movement of this Camera.		
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if 
          *             1. The agent is in Monitor mode.
          *             2. !isMovementControllable()
		  */

		virtual TA_Base_Bus::MovementControlCorbaDef_ptr getMovementControl();
        
		/** 
		  * isFocusIrisControllable
		  *
		  * Identifies if this Camera supports at least one of the focus or iris controls. 
		  * It should only be used to determine if the FocusIrisControl interface exists.
		  *
		  * @return		bool
		  *				True if the FocusIrisControl interface exists.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
		
		virtual bool isFocusIrisControllable();

		/**
		  * getFocusIrisControl
		  *
		  * Gets the FocusIrisControl interface.
		  *
		  * @return		TA_Base_Bus::FocusIrisControlCorbaDef_ptr
		  *				The interface to the focus and iris control for this Camera.		
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if 
          *             1. The agent is in Monitor mode.
          *             2. !isFocusIrisControllable()
		  */

		virtual TA_Base_Bus::FocusIrisControlCorbaDef_ptr getFocusIrisControl();

		/** 
		  * isWashWipeControllable
		  *
		  * Identifies if this Camera supports at least one of the wash or wipe controls. 
		  * It should only be used to determine if the WashWipeControl interface exists.
		  *
		  * @return		bool
		  *				True if the WashWipeControl interface exists.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
		
		virtual bool isWashWipeControllable();

		/**
		  * getWashWipeControl
		  *
		  * Gets the WashWipeControl interface.
		  *
		  * @return		TA_Base_Bus::WashWipeControlCorbaDef_ptr
		  *				The interface to the wash and wipe control for this Camera.		
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if 
          *             1. The agent is in Monitor mode.
          *             2. !isWashWipeControllable()
		  */

        virtual TA_Base_Bus::WashWipeControlCorbaDef_ptr getWashWipeControl();


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
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
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
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          */

        virtual bool isWasherOn( const char* sessionId );

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntity Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * start
          *
          * Creates the controls if required and
          * activates the controls and the Camera Corba servants.
          *
          * IMPORTANT: Also calls VideoInput::start().
          */

        virtual void start();

        /**
          * update
          *
          * Called by the agent controlling the entity to ask 
          * the entity to update its configuration. Called after 
          * an update configuration event is received.
          *
          * IMPORTANT: Also calls VideoInput::update().
          *
          * @param      const TA_Base_Core::ConfigUpdateDetails& updateEvent
          *             The received event containing the configuration information.
          */

        virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

        /**
          * remove
          *
          * When a configuration update event is received to delete an entity 
          * from an agent, the agent calls this method to tell the entity 
          * to commence any clean up operations. The agent then removes 
          * the object from its vector of entities.
          *
          * This method calls stop().
          *
          * IMPORTANT: Also calls VideoInput::remove().
          */

        virtual void remove();

        /**
          * setToControlMode
          *
          * Called when the agent's mode of operation changes to Control. 
          * Registers the Camera so it can be resolved by the agent's clients.
          */

        virtual void activateServantAtInit();

        /**
          * setToMonitorMode
          *
          * Called when the agent's mode of operation changes to Monitor. 
          * Deregisters the Camera so it cannot be resolved by the agent's clients.
          */

        virtual void deactivateServantAtClose();

		virtual void setToControlMode(){};

		virtual void setToMonitorMode(){};

        ///////////////////////////////////////////////////////////////////////
        //
        // Internal Helper Methods
        //
        ///////////////////////////////////////////////////////////////////////

		/**
		  * getMovementControlInternal
		  *
		  * Gets the MovementControl interface.
		  *
		  * Precondition:	isMovementControllable()
		  *
		  * @return		MovementControl&
		  *				The interface to the control for movement of this Camera.		
		  *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if the agent is in Monitor mode.
		  */

		MovementControl& getMovementControlInternal();
        
		/**
		  * getFocusIrisControlInternal
		  *
		  * Gets the FocusIrisControl interface.
		  *
		  * Precondition:	isFocusIrisControllable()
		  *
		  * @return		FocusIrisControl&
		  *				The interface to the focus and iris control for this Camera.		
		  *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if the agent is in Monitor mode.
		  */

		FocusIrisControl& getFocusIrisControlInternal();

		/**
		  * getWashWipeControlInternal
		  *
		  * Gets the WashWipeControl interface.
		  *
		  * Precondition:	isWashWipeControllable()
		  *
		  * @return		WashWipeControl&
		  *				The interface to the wash and wipe control for this Camera.		
		  *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if the agent is in Monitor mode.
		  */

        WashWipeControl& getWashWipeControlInternal();

    private:
        //
        // Disable copy constructor and assignment operator.
        //

        Camera( const Camera& theCamera );
        Camera& operator=( const Camera& );

        /**
          * createControls
          *
          * Creates the Movement, FocusIris and WashWipe controls
          * if the relevant features are supported by the protocol and
          * by this Camera.  If there are any problems with the 
          * database or with the data from the database,
          * then that control will not be supported (created).
          *
          * Precondition:   0 != m_cameraData
          */    

        void createControls();

        /**
          * activateCorbaServants
          *
          * Activates the Camera object and the control objects, so they 
          * can accept Corba requests.
          *
          * Precondition:   AgentModeMonitor::getInstance().isInControlMode()
          */

        void activateCorbaServants();

        /**
          * deactivateCorbaServants
          *
          * Deactivates the Camera object and the control objects, so they 
          * no longer accept Corba requests.
          */

        void deactivateCorbaServants();

        //
		// The configuration data object for this Camera.
		//

        TA_Base_Core::CameraPtr m_cameraData;

		//
		// The object that handles the protocol dependent tasks for this object.
		//

		std::auto_ptr< IPDCamera > m_pdCamera;

        //
        // The interfaces to the controls for this camera. 
        // These are auto_ptrs so they will automatically be cleaned up.
        //

        std::auto_ptr< MovementControl >  m_movementControl;
        std::auto_ptr< FocusIrisControl > m_focusIrisControl;
        std::auto_ptr< WashWipeControl >  m_washWipeControl;
        unsigned long                     m_entityKey;

        //
        // static constants
        //
        static const std::string GET_SUPPORTED_FEATURES;
        static const std::string GET_STATE;
        static const std::string IS_MOVEMENT_CONTROLLABLE;
        static const std::string GET_MOVEMENT_CONTROL;
        static const std::string IS_FOCUS_IRIS_CONTROLLABLE;
        static const std::string GET_FOCUS_IRIS_CONTROL;
        static const std::string IS_WASH_WIPE_CONTROLLABLE;
        static const std::string GET_WASH_WIPE_CONTROL;
        static const std::string GET_MOVEMENT_CONTROL_INTERNAL;
        static const std::string GET_FOCUS_IRIS_CONTROL_INTERNAL;
        static const std::string GET_WASH_WIPE_CONTROL_INTERNAL;
    };

} // namespace TA_IRS_App

#endif // !defined(Camera_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
