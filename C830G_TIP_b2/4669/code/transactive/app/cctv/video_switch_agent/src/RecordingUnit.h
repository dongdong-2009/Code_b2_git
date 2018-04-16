#if !defined(RecordingUnit_B6AB8CD2_CAEC_4df4_A4C3_218806677402__INCLUDED_)
#define RecordingUnit_B6AB8CD2_CAEC_4df4_A4C3_218806677402__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/RecordingUnit.h $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/25 17:58:20 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "bus/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"
#include "app/cctv/video_switch_agent/src/VideoSwitchResource.h"

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/RecordingUnit.h"

namespace TA_IRS_App
{
    class IPDRecordingUnit;
}

namespace TA_IRS_App
{
    class RecordingUnit : public VideoSwitchResource,
                          public virtual POA_TA_Base_Bus::RecordingUnitCorbaDef,
                          public virtual TA_Base_Core::ServantBase
    {

    public:

        /**
          * RecordingUnit
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this RecordingUnit.
          *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
          *             1. The entityData cannot be converted to RecordingUnit data.
          *             2. The protocol dependent initialisation fails.
          */
        RecordingUnit( TA_Base_Core::IEntityDataPtr entityData );


	    virtual ~RecordingUnit();


        /**
          * updateState
          *
          * Updates the state of the RecordingUnit.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state
          *             The new state of the RecordingUnit.
          */
        void updateState( const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state );


		/**
		  * getSupportedFeatures
		  *
		  * Gets the features that this RecordingUnit supports.
		  *
		  * @return     SupportedRecordingUnitFeatures
		  *				The supported features struct for this recording unit.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
        TA_Base_Bus::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures getSupportedFeatures();

		/**
		  * getRecordingUnitState
		  *
		  * Gets the current state of this RecordingUnit.
		  *
		  * @return     RecordingUnitState
		  *				The current state of this recording unit.
		  * 
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
		  */
		TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState getRecordingUnitState();


		/**
		  * getRecordingSpeeds
		  *
		  * Get the recording speeds available for this device.
		  *
		  * @return     recordingSpeedSequence
		  *				Sequence of Strings corresponding to the available
		  *				recording speeds for this recording unit.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
		  */
		TA_Base_Bus::RecordingUnitCorbaDef::recordingSpeedSequence* getRecordingSpeeds();


		/**
		  * play 
		  *
		  * Send play command to recordingUnit.
		  *
		  * Precondition: isPlaybackControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void play( const char* sessionId );


		/**
		  * fastForward
		  *
		  * Send fast-forward command to recordingUnit.
		  *
		  * Precondition: isPlaybackControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void fastForward( const char* sessionId );


		/**
		  * rewind
		  *
		  * Send rewind command to recordingUnit.
		  *
		  * Precondition: isPlaybackControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void rewind( const char* sessionId );


		/**
		  * pause
		  *
		  * Send pause command to recordingUnit.
		  *
		  * Precondition: isPlaybackControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void pause( const char* sessionId );


		/**
		  * stopPlayback
		  *
		  * Send stop command to recordingUnit during playback.
		  *
		  * Precondition: isPlaybackControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void stopPlayback( const char* sessionId );


		/**
		  * skipToNextChapter
		  *
		  * Send skipToNextChapter command to recordingUnit.
		  *
		  * Precondition: ( isPlaybackControllable() && (ERecordingUnitType == DVD) )
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void skipToNextChapter( const char* sessionId );


		/**
		  * skipToPreviousChapter
		  *
		  * Send skipToNextChapter command to recordingUnit.
		  *
		  * Precondition: ( isPlaybackControllable() && (ERecordingUnitType == DVD) )
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void skipToPreviousChapter( const char* sessionId );


		/**
		  * record
		  *
		  * Send record command to recordingUnit.
		  *
		  * Precondition: isRecordingControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void record( const char* sessionId );


		/**
		  * stopRecording
		  *
		  * Send stop command to recordingUnit during recording.
		  *
		  * Precondition: isRecordingControllable()
		  *
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void stopRecording( const char* sessionId );


		/**
		  * setRecordingSpeed
		  *
		  * Set new recording speed from those available for this RecordingUnit.
		  *
		  * precondition: ( isRecordingSpeedControllable() && (recordingSpeedIndex < recordingSpeeds.size() )
		  *
		  * @param		unsigned short recordingSpeedIndex
		  *				Index within recordingSpeedSequence to set speed to.
          * @param      string sessionId   
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this RecordingUnit.
          *             2. The RecordingUnit or the protocol does not support this operation.
          *             3. The RecordingUnit is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
		  *				6. There is a communications failure with the hardware.
          *             7. The RecordingUnit address cannot be retrieved from the database.
		  */
		void setRecordingSpeed( unsigned short recordingSpeedIndex, const char* sessionId );

        /**
          * getConsoleKey
          *
          * Gets the key of the Console to which this VideoMonitor is attached.
          * (0 if this is not a console monitor)
          * 
          * @return     unsigned long
          *             The unique key of the console to which this VideoMonitor
          *             is attached or 0 if it is not a Console VideoMonitor.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */
        unsigned long getConsoleKey();


		//
		// IEntity functions
		//
		virtual void start();

		virtual void stop();

		virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

		virtual void remove();

		virtual void activateServantAtInit();

		virtual void deactivateServantAtClose();

		virtual void setToControlMode(){};

		virtual void setToMonitorMode(){};

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        RecordingUnit( const RecordingUnit& theRecordingUnit );
        RecordingUnit& operator=( const RecordingUnit& );

		TA_Base_Core::RecordingUnitPtr			m_recordingUnitEntityData;
		std::auto_ptr< IPDRecordingUnit > m_pdRecordingUnit;
//         UserPrivilegeVerifier           m_userPrivilegeVerifier;
        UserPrivilegeVerifier&           m_userPrivilegeVerifier; // HuangQi++
		unsigned long					m_entityKey;
    }; // RecordingUnit

} // TA_IRS_App

#endif // !defined(RecordingUnit_B6AB8CD2_CAEC_4df4_A4C3_218806677402__INCLUDED_)
