#if !defined(VideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define VideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/VideoOutput.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles all the common functions of video outputs.
  * Implements the IVideoOutput interface. 
  * Uses the IPDVideoOutput interface to perform the protocol
  * dependent sections of the control functions.
  *
  * NOTE: When inheriting from this class, if you implement the TA_Base_Bus::IEntity
  * methods, you need to call the TA_Base_Bus::IEntity methods in this class as part
  * of the overriding methods.
  * 
  */

#include <string>
#include <memory>

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDVideoOutput.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "app/cctv/video_switch_agent/src/VideoSwitchResource.h"
#include "core/data_access_interface/entity_access/src/VideoOutput.h"

#include "bus/generic_agent/src/IEntity.h"

#include "core/corba/src/ServantBase.h"

// Forward declarations
#define PTZ_PAN_LEFT   0x1
#define PTZ_PAN_RIGHT  0x2
#define PTZ_PAN_STOP   0x0
#define PTZ_TILT_UP    0x1
#define PTZ_TILT_DOWN  0x2
#define PTZ_TILT_STOP  0x0
#define PTZ_ZOOM_IN    0x1
#define PTZ_ZOOM_OUT   0x2
#define PTZ_ZOOM_STOP  0x0
#define PTZ_SPEED_LOW  0x1
#define PTZ_SPEED_HIGH 0x2
#define PTZ_CMD_PAN    0x0
#define PTZ_CMD_TILT   0x4
#define PTZ_CMD_ZOOM   0x8
namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_IRS_App
{
    class VideoOutput : public VideoSwitchResource,
                        public virtual POA_TA_Base_Bus_GZL9::VideoOutputCorbaDef,
                        public virtual TA_Base_Core::ServantBase
    {
    public:

        /**
          * VideoOutput
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data for this VideoOutput.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if:
          *             1. The entityData cannot be converted to VideoInput data.
          *             2. The protocol dependent initialisation fails.
          */

        VideoOutput( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * ~VideoOutput
          *
          * Standard destructor.
          */

        virtual ~VideoOutput();
                    
        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the VideoOutput.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state
          *             The new state of the VideoOutput.
          */

        void updateState( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState& state );
      
        ///////////////////////////////////////////////////////////////////////
        //
        // VideoOutputCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////
              
        /**
          * getVideoOutputState
          *
          * Gets the current state of this VideoOutput.
          *
          * @return     VideoOutputState
          *             A struct detailing the current VideoOutput state.
          *             It is the caller's responsibility to clean up this struct.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The agent is in Monitor mode.
          *             2. There is a communications failure with the hardware.
          */

        virtual TA_Base_Bus_GZL9::VideoOutputCorbaDef::VideoOutputState* getVideoOutputState();

        /**
          * getVideoOutputGroupName
          *
          * @return     char*
          *             The unique name of the group that this VideoOutput belongs to.
          *             The group name is used for grouping the outputs when
          *             they are displayed.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */

        virtual char* getVideoOutputGroupName();

        /**
          * getCurrentVideoInputKey
          *
          * Get the database (entity) key of the VideoInput currently switched to this VideoOutput.  
          * 0 indicates no VideoInput is siwtched to this VideoOutput.
          *
          * @return     unsigned long
          *             The database (entity) key of the VideoInput for this VideoOutput device or
          *             0 if there is no associated input.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The agent is in Monitor mode.
          *             2. There is a communications failure with the hardware.
          */

        virtual unsigned long getCurrentVideoInputKey();

        /**
          * requestSwitchOfInput
          *
          * Requests that the hardware switches the named VideoInput to this VideoOutput.
          *
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      const char* sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
          * @param      bool isDemand = false
          *             If false => If the VideoInput is locked (hardware locking), the 
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
          *             2. A polite request was made and the VideoInput is switch (hardware) locked.
          *             3. The VideoInput key is not recognised.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          *             6. There is a communications failure with the hardware.
          */

        virtual void requestSwitchOfInput( unsigned long videoInputEntityKey, 
                                           const char* sessionId,
                                           bool isDemand );

	    virtual void requestSwitchOfInputs(unsigned long topRightVideoInputEntityKey,
			unsigned long topLeftVideoInputEntityKey,
			unsigned long botRightVideoInputEntityKey,
			unsigned long botLeftVideoInputEntityKey,
			const char* sessionId,
			bool isDemand);

		virtual void activeVideoInput( unsigned long videoInputEntityKey,bool active,  
			const char*  sessionId, bool isDemand );
		/**
		  * cycleToNextVideoInput
		  *
		  * Manually cycles sequence playback to next VideoInput.
		  *
		  * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
		  * @exception	VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
		  *				2. The Sequence is not currently assigned to VideoOutput specified.
		  *				2. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware. 
		  */
		virtual void connectSequence( const TA_Base_Bus_GZL9::VideoOutputCorbaDef::UserDefinedSequenceConfig& newConfig,
			const char* sessionId ) ;
		virtual void disconnectSequence( const char* sessionId ) ;
		
		void cycleToNextVideoInput( const char* sessionId );

		/**
		  * cycleToPreviousVideoInput
		  *
		  * Manually cycles sequence playback to previous VideoInput.
		  *
		  * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
		  * @exception	VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
		  *				2. The Sequence is not currently assigned to VideoOutput specified.
		  *				2. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware. 
		  */

		void cycleToPreviousVideoInput( const char* sessionId );

		/**
		  * pauses
		  *
		  * Pauses playback of sequence.
		  *
		  * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
		  * @exception	VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
		  *				2. The Sequence is not currently assigned to VideoOutput specified.
		  *				2. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware. 
		  */

		void pause( const char* sessionId );

		/**
		  * play
		  *
		  * Begin/Resume Playback of sequence.
		  *
		  * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
		  * @exception	VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
		  *				2. The Sequence is not currently assigned to VideoOutput specified.
		  *				2. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware. 
		  */

		void play( const char* sessionId );

		/**
		  * stop
		  *
		  * Stop playback of sequence (can be same behaviour as pause depending on hardware).
		  *
		  * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
		  * @exception	VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
		  *				2. The Sequence is not currently assigned to VideoOutput specified.
		  *				2. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware. 
		  */

		void stop( const char* sessionId );

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
	//	virtual ::CORBA::Boolean OutputCameraPan(::CORBA::ULong cameraKey, ::CORBA::Char movement, ::CORBA::Char speed);
	//	virtual ::CORBA::Boolean OutputCameraTilt(::CORBA::ULong cameraKey, ::CORBA::Char movement, ::CORBA::Char speed) ;
	//	virtual ::CORBA::Boolean OutputCameraZoom(::CORBA::ULong cameraKey, ::CORBA::Char movement) ;
	//	virtual ::CORBA::Boolean OutputCameraPTZ(::CORBA::ULong cameraKey, const ::TA_Base_Bus_GZL9::VideoOutputCorbaDef::CameraPTZ& cmd) ;

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        VideoOutput( const VideoOutput& theVideoOutput );
        VideoOutput& operator=( const VideoOutput& theVideoOutput );

        //
        // The configuration data object for this VideoOutput.
        // The object that this points to is cleaned up by GenericAgent.
        //

        TA_Base_Core::VideoOutputPtr m_videoOutputData;

        //
        // The object that handles the protocol dependent tasks for this object.
        //

        std::auto_ptr< IPDVideoOutput > m_pdVideoOutput;

        //
        // Determines if users have the rights to perform certain actions.
        //

        UserPrivilegeVerifier m_userPrivilegeVerifier;

    }; // VideoOutput

} // TA_IRS_App
    
#endif // !defined(VideoOutput_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
