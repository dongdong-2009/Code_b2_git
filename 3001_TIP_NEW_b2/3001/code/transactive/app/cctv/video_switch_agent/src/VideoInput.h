#if !defined(VideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)
#define VideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/VideoInput.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Handles all the common functions of video inputs.
  * Implements the IVideoInput interface. 
  * Uses the IPDVideoInput interface to perform the protocol
  * dependent sections of the control functions.
  *
  * NOTE: When inheriting from this class, if you implement the TA_Base_Bus::IEntity
  * methods, you need to call the TA_Base_Bus::IEntity methods in this class as part
  * of the overriding methods.
  * 
  */


// FUTURE TODO SWITCH_LOCK EXCEPTION FOR ALL CONTROL FUNCTIONS OF INPUTS - re KL
  

#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"
#include "app/cctv/video_switch_agent/src/SuperimposedTextControl.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDVideoInput.h"
#include "app/cctv/video_switch_agent/src/VideoSwitchResource.h"

#include "bus/generic_agent/src/IEntity.h"

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"

// Forward declarations.

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_IRS_App
{
	class VideoInput : public VideoSwitchResource,
                       public virtual POA_TA_Base_Bus::VideoInputCorbaDef,
                       public virtual TA_Base_Core::ServantBase
	{
	public:
		/**
		  * VideoInput
		  *
		  * Standard constructor.
		  *
		  * @param		TA_Base_Core::IEntityData& entityData
		  *				The configuration data for this VideoInput.
          *             This object is owned by GenericAgent, so it should not be deleted.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
          *             1. The entityData cannot be converted to VideoInput data.
          *             2. The protocol dependent initialisation fails.
		  */

        VideoInput( TA_Base_Core::IEntityDataPtr entityData );

		/**
		  * ~VideoInput
		  *
		  * Standard destructor.
		  */

		virtual ~VideoInput();
        
        /**
          * getVideoInputData
          *
          * Gets the database entity for this VideoInput.
          *
          * @return     TA_Base_Core::VideoInput&
          *             The VideoInput data.
          */

        TA_Base_Core::VideoInputPtr& getVideoInputData();
       

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the VideoInput.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state
          *             The new state of the VideoInput.
          */

        void updateState( const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state );

		///////////////////////////////////////////////////////////////////////
		//
		// VideoInputCorbaDef Methods
		//
		///////////////////////////////////////////////////////////////////////
                
        /**
          * getSupportedVideoInputFeatures
          *
          * Gets the features that this VideoInput supports.
          *
          * @return     TA_Base_Bus::VideoInputCorbaDef::SupportedVideoInputFeatures
          *             A struct detailing which VideoInput features are supported.
		  *				If an error occurs while retrieving the information from
		  *				the database, it will be assumed that the feature is 
		  *				not supported.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
          */
 
        virtual TA_Base_Bus::VideoInputCorbaDef::SupportedVideoInputFeatures getSupportedVideoInputFeatures();

        /**
          * getVideoInputState
          *
          * Gets the current state of this VideoInput.
          *
          * @return     VideoInputState
          *             A struct detailing the current VideoInput state.
		  *				It is the caller's responsibility to clean up this struct.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
		  *				5. There is a communications failure with the hardware.
          */

		virtual TA_Base_Bus::VideoInputCorbaDef::VideoInputState* getVideoInputState();
		        

		/**
		  * getVideoInputGroupName
		  *
          * Gets the name of the group that this VideoInput belongs to.
          *
		  * @return		char* 
		  *				The unique name of the group that this VideoInput belongs to.
          *             The group name is used for grouping the inputs when
          *             they are displayed.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

		virtual char*  getVideoInputGroupName();
		
		/** 
		  * isSuperimposedTextControllable
		  *
		  * Identifies if superimposing text on the VideoInput is supported.
		  * Note: This does not identify if all the superimpose text functions
		  *	(superimpose date, name or free text) are suppported. It should 
		  * only be used to determine if the interface to the SuperimposedTextControl 
		  * object exists.
		  *
		  * @return		bool
		  *				True if the interface to the SuperimposedTextControl object exists.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
		
		virtual bool isSuperimposedTextControllable();

		/**
		  * getSuperimposedTextControl
		  *
		  * Gets the interface to the SuperimposedTextControl.
		  *
		  * Precondition: isSuperimposedTextControllable()
		  *
		  * @return		TA_Base_Bus::SuperimposedTextControlCorbaDef_ptr
		  *				The interface to the control for displaying superimposed text.		
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */

        virtual TA_Base_Bus::SuperimposedTextControlCorbaDef_ptr getSuperimposedTextControl();

        //*********************************************************************
        //
        // Note regarding lock ownership.
        //
        // Lock ownership is not persistant.  It will be maintained as part of
        // the state updates from Control to Monitor mode agents, but it will
        // not be maintained if all agents are shutdown.  We don't want to have
        // to figure out who last had the lock on an input if the system has
        // been down for maintenance for a while and it is easy enough for the
        // necessary clients to reobtain the lock if required.
        //
        //*********************************************************************

		/**
		  * lock
		  *
		  * Locks the VideoInput so that the given user session has exclusive
		  *	control of the VideoInput.
		  * 
		  * @param		const char* sessionId
		  *				Security id used to identify if this user has permission
		  *				to lock this object and to identify the holder of the lock.
		  * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. Another user owns the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
  		  *				3. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware.
		  */
		
		virtual void lock( const char* sessionId );

		/**
		  * unlock
		  *
		  * Unlocks the VideoInput so that the given user session no longer has 
		  *	exclusive control of the VideoInput.  This method does nothing if the
          * lock is not already held by this user.
		  * 
		  * @param		const char* sessionId
		  *				Security id used to identify if this user currently holds
		  *				the lock.
		  * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
  		  *				2. The agent is in Monitor mode.
		  *				3. The user does not have sufficient privileges for this operation.
		  *				4. There is a communications failure with the hardware.
		  */	
		
		virtual void unlock( const char* sessionId );

		/**
		  * overrideLock
		  *
		  * Overrides any current ownership of the lock for this VideoInput giving
          * the given user the exclusive camera lock.
		  * 
		  * @param		const char* sessionId
		  *				Security id used to identify if this user currently holds
		  *				the lock or if this user has the privileges to override 
		  *				the lock.
		  * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
  		  *				2. The agent is in Monitor mode.
		  *				3. The user does not have sufficient privileges for this operation.
		  *				4. There is a communications failure with the hardware.
		  */	
		
		virtual void overrideLock( const char* sessionId );

		/**
		  * isPublicViewingAllowed
		  *
		  * Identifies if this VideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @return		bool
		  *				True if the VideoInput is allowed to be viewed publicly.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
		  *				5. There is a communications failure with the hardware.
		  */

		virtual bool isPublicViewingAllowed();

		/**
		  * setPublicViewingAllowed
		  *
		  * Sets if this VideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @param		bool isPublicViewingAllowed
		  *				True if the VideoInput is allowed to be viewed publicly.
		  *				False otherwise.
		  * @param		const char* sessionId
		  *				Security id used to identify if this user has permission
		  *				to set the viewing status of this VideoInput.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
          *             1. The user does not own the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
		  *				3. The database cannot be accessed.
		  *				4. The data cannot be retrieved.
		  *				5. The retrieved data is invalid.
		  *				6. The agent is in Monitor mode.
		  *				7. The user does not have sufficient privileges for this operation.
		  *				8. There is a communications failure with the hardware.
		  */

		virtual void setPublicViewingAllowed( bool isPublicViewingAllowed, const char* sessionId );
		
		/**
		  * isInService
		  *
		  * Identifies if this VideoInput is in or out of service.
		  *
		  * @return		bool
		  *				True if the VideoInput is in service.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
		  *				5. There is a communications failure with the hardware.
		  */

		virtual bool isInService();
		
		/**
		  * setInService
		  *
		  * Sets this VideoInput in or out of service.
		  *
		  * @param		bool isInService
		  *				True to set the VideoInput in service.
		  *				False to set the VideoInput out of service.
		  * @param		const char* sessionId
		  *				Security id used to identify if this user has permission
		  *				to set the service status of this VideoInput.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
          *             1. The user does not own the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
		  *				3. The database cannot be accessed.
		  *				4. The data cannot be retrieved.
		  *				5. The retrieved data is invalid.
		  *				6. The agent is in Monitor mode.
		  *				7. The user does not have sufficient privileges for this operation.
		  *				8. There is a communications failure with the hardware.
 		  */
		
		virtual void setInService( bool isInService, const char* sessionId );


		///////////////////////////////////////////////////////////////////////
		//
		// Supporting Methods
		//
		///////////////////////////////////////////////////////////////////////

        /** 
          * getLockOwner
          *
          * Gets the sessionId of the current owner of the exclusive lock
          * for this VideoInput.
          *
		  * @param		std::string
		  *				The sessionId of the lock owner or
          *             "" if the lock is not held by anyone or if locking
          *             is not being used by this VideoInput.
          */

        std::string getLockOwner();

		/**
		  * verifyUserHoldsLock
		  *
		  * Verifies that the user holds the exclusive lock for this VideoInput.
          * If not, an exception is thrown.
		  *
		  * @param		const std::string& sessionId
		  *				Used to identify the user.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the user does not hold the lock for this
		  *				VideoInput.  The exception will contain the details of the
		  *				user that holds the lock for this VideoInput.
		  */

		void verifyUserHoldsLock( const std::string& sessionId );

        /**
		  * verifyInService
		  *
		  * Verifies that this VideoInput is in service.
          * If not, an exception is thrown.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the VideoInput is out of service.
		  */

		void verifyInService();

        ///////////////////////////////////////////////////////////////////////
        //
        // IEntity Methods
        //
        ///////////////////////////////////////////////////////////////////////
  
        /**
          * isValid
          *
          * This is a pure vitual method that must be implemented by 
          * all entity classes. 
          *
          * NOTE: This always returns true!
          *
          * @return     bool
          *             True if the entity is valid.
          *             False otherwise.
          */

        virtual bool isValid();

        /**
          * start
          *
          * Creates and activates the SuperimposedTextControl if required.
          *
          * IMPORTANT: When inheriting from this class, if this method
          * is overridden, the overriding method must call this 
          * method explicitly.
          */

        virtual void start();

        /**
          * stop
          *
          * Deactivates and deletes the SuperimposedTextControl.
          *
          * IMPORTANT: When inheriting from this class, if this method
          * is overridden, the overriding method must call this 
          * method explicitly.
          */

        virtual void stop();

        /**
          * update
          *
          * Called by the agent controlling the entity to ask 
          * the entity to update its configuration. Called after 
          * an update configuration event is received.
          *
          * IMPORTANT: When inheriting from this class, if this method
          * is overridden, the overriding method must call this 
          * method explicitly.
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
          * IMPORTANT: When inheriting from this class, if this method
          * is overridden, the overriding method must call this 
          * method explicitly.
          */

        virtual void remove();

	private:
        //
        // Disable copy constructor and assignment operator.
        //

        VideoInput( const VideoInput& theVideoInput);
		VideoInput& operator=( const VideoInput& );

        /**
          * createControls
          *
          * Creates the SuperimposedText control if the relevant features 
          * are supported by the protocol and this VideoInput. If there are any
          * problems with the database or with the data from the database,
          * then that control will not be supported (created).
          */    

        void createControls();

        /**
          * activateCorbaServants
          *
          * Activates the Camera object and the control objects, so they 
          * can accept Corba requests.
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
		// The configuration data object for this VideoInput.
		//

        TA_Base_Core::VideoInputPtr m_videoInputData;

		//
		// The object that handles the protocol dependent tasks for this object.
		//

		std::auto_ptr< IPDVideoInput > m_pdVideoInput;
	
        //
		// The interfaces to the controls for this object.
		//

        std::auto_ptr< SuperimposedTextControl > m_superimposedTextControl;

        //
        // Determines if users have the rights to perform certain actions.
        //

        UserPrivilegeVerifier m_userPrivilegeVerifier;

        //
        // Thread lock used for guarding the lock status of the VideoInput.
        //

        TA_Base_Core::ReEntrantThreadLockable m_lockOwnerLock;

        //
        // static constants
        //

        static const std::string GET_ADDRESS;
        static const std::string GET_SUBSYSTE_KEY;
        static const std::string GET_LOCATION_KEY;
        static const std::string GET_SUPPORTED_FEATURES;
        static const std::string GET_STATE;
        static const std::string GET_KEY;
        static const std::string GET_NAME;
        static const std::string GET_TYPE;
        static const std::string GET_DESCRIPTION;
        static const std::string GET_VIDEO_INPUT_GROUP_NAME;
        static const std::string IS_SUPERIMPOSED_TEXT_CONTROLLABLE;
        static const std::string GET_SUPERIMPOSED_TEXT_CONTROL;
        static const std::string IS_PUBLIC_VIEWING_ALLOWED;
        static const std::string IS_IN_SERVICE;
	};

} // namespace TA_IRS_App

#endif // !defined(VideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)