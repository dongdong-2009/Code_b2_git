/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/Sequence.h $
 * @author:  Jade Welton
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2015/08/28 13:33:23 $
 * Last modified by:  $Author: hongzhi.zhang $
 * 
 * <description>
 */

#if !defined(Sequence_3D23C5DE_427D_4688_800E_ABCF8F975CA1__INCLUDED_)
#define Sequence_3D23C5DE_427D_4688_800E_ABCF8F975CA1__INCLUDED_

#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
//#include "core/data_access_interface/video_switch_agent/src/SequenceHelper.h"
#include "core/corba/src/ServantBase.h"

namespace TA_IRS_App
{
    class IPDSequence;

    class Sequence : public VideoInput,
                     public virtual POA_TA_Base_Bus::SequenceCorbaDef,
                     public virtual TA_Base_Core::ServantBase
    {

    public:
        /**
          * Sequence
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this Sequence.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if:
          *             1. The entityData cannot be converted to Sequence data.
          *             2. The protocol dependent initialisation fails.
          */

        Sequence( TA_Base_Core::IEntityDataPtr entityData );


        /**
          * ~Sequence
          *
          * Destructor.
          *
          */
        virtual ~Sequence();


        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the Sequence.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::SequenceCorbaDef::SequenceState& state
          *             The new state of the Sequence.
          */

        void updateState( const TA_Base_Bus::SequenceCorbaDef::SequenceState& state );


        /**
          * updateSequenceConfig
          *
          * Updates the Sequence Config of the Sequence.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig
          *             The new Sequence Config of the Sequence.
          */

        void updateSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig );


        ///////////////////////////////////////////////////////////////////////
        //
        // SequenceCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////


        /**
          * isReadOnly
          *
          * Indicates whether this Sequence's configuration can be changed.
          *
          * @return     boolean
          *             True if the Sequence configuration can be changed,
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual bool isReadOnly();

		// TD16691
        /**
          * getSequenceDescription
          *
          * Get the sequence description
          *
          * @return     string sequenceDescription
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual char* getSequenceDescription();
		// TD16691

        /**
          * getDwellTimeRange
          *
          * Provides the range of seconds that can be set as the Sequence's dwell time.
          *
          * @param      unsigned short minimumDwellTime
          *             will contain minimum dwell time (in seconds).
          * @param      unsigned short maximumDwellTime
          *             will contain maximum dwell time (in seconds).
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual void getDwellTimeRange( unsigned short& minimumDwellTime, unsigned short& maximumDwellTime );


        /**
          * getMaxNumberOfInputs
          *
          * Returns the maximum number of VideoInputs that can be configured.
          *
          * @return     unsigned short
          *             The maximum number of VideoInputs that can
          *             be configured for this sequence.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual unsigned short getMaxNumberOfInputs();


        /**
          * getSequenceConfig
          *
          * Gets the current configution of this Sequence.
          *
          * @return     SequenceConfig
          *             The current configuration of this Sequence.
          *
          * @exception  VideoSwitchAgentException
          *             1. The data cannot be retrieved.
          *             2. The retrieved data is invalid.
          *             3. The agent is in Monitor mode.
          */
        virtual TA_Base_Bus::SequenceCorbaDef::SequenceConfig* getSequenceConfig();


        /**
          * setSequenceConfig
          *
          * Sets the current configuration of this Sequence.
          *
          * Precondition: !isReadOnly()
          *
          * @param      SequenceConfig newConfig
          *             The configuration to be set for this Sequence.
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. A VideoInput key is not recognised.
          *             3. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig, const char* sessionId );


		/**
		  * setSequenceDescription
		  *
		  * Sets the current description of this Sequence.
		  *
		  * Precondition: !isReadOnly()
		  *
		  * @param		string newDescription
		  *				The description to be set for this Sequence.
          * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. A VideoInput key is not recognised.
		  *				3. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware.
          */
		
		void setSequenceDescription( const char* newDescription, const char* sessionId );


        /**
          * cycleToNextVideoInput
          *
          * Manually cycles sequence playback to next VideoInput.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The Sequence is not currently assigned to VideoOutput specified.
          *             2. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void cycleToNextVideoInput( unsigned long videoOutputEntityKey, const char* sessionId );


        /**
          * cycleToPreviousVideoInput
          *
          * Manually cycles sequence playback to previous VideoInput.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The Sequence is not currently assigned to VideoOutput specified.
          *             2. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void cycleToPreviousVideoInput( unsigned long videoOutputEntityKey, const char* sessionId );


        /**
          * pauses
          *
          * Pauses playback of sequence.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The Sequence is not currently assigned to VideoOutput specified.
          *             2. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void pause( unsigned long videoOutputEntityKey, const char* sessionId );


        /**
          * play
          *
          * Begin/Resume Playback of sequence.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The Sequence is not currently assigned to VideoOutput specified.
          *             2. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void play( unsigned long videoOutputEntityKey, const char* sessionId );


        /**
          * stop
          *
          * Stop playback of sequence (can be same behaviour as pause depending on hardware).
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The Sequence is not currently assigned to VideoOutput specified.
          *             2. The agent is in Monitor mode.
          *             4. The user does not have sufficient privileges for this operation.
          *             5. There is a communications failure with the hardware.
          */
        virtual void stop( unsigned long videoOutputEntityKey, const char* sessionId );


        /**
          * getSequenceState
          *
          * Returns the sequence's current state.
          *
          * @param      unsigned long videoOutputEntityKey
          *             The key of the video output that the sequence is running on.
          *
          * @return     TA_Base_Bus::SequenceCorbaDef::ESequenceState
          *             The sequence's current state.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. The agent is in Monitor mode.
          *             3. The sequence is not running on the specified video output.
          */

        virtual TA_Base_Bus::SequenceCorbaDef::SequenceState getSequenceState( unsigned long videoOutputEntityKey );


        ///////////////////////////////////////////////////////////////////////
        //
        // IEntity Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * start
          *
          * Creates the controls if required and
          * activates the controls and the Sequence Corba servants.
          *
          * IMPORTANT: Also calls VideoInput::start().
          */
        virtual void start();


        /**
          * stop
          *
          * Deregisters and deactivates the VideoMonitor Corba servant.
          *
          * IMPORTANT: Also calls VideoInput::stop().
          */
        virtual void stop();


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


		virtual void setToControlMode();

		virtual void setToMonitorMode();

		void addCameraForSequence(TA_IRS_App::SwitchManager::CameraMap& cameras);

    private:

		void activateTheServant();
        void deactivateTheServant();
        //
        // Disable copy constructor and assignment operator.
        //

        Sequence( const Sequence& theSequence );
        Sequence& operator=( const Sequence& );

        TA_Base_Core::SequencePtr              m_sequenceEntityData;
        std::auto_ptr< IPDSequence >    m_pdSequence;
        UserPrivilegeVerifier           m_userPrivilegeVerifier;

    }; // Sequence

} // TA_IRS_App

#endif // !defined(Sequence_3D23C5DE_427D_4688_800E_ABCF8F975CA1__INCLUDED_)
