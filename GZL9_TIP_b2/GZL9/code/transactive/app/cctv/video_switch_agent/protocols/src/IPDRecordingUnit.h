#ifndef IPDRECORDINGUNIT_H
#define IPDRECORDINGUNIT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDRecordingUnit.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the operations for a RecordingUnit.
  *
  */

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"

namespace TA_IRS_App
{
	class IPDRecordingUnit
	{

	public:

		virtual ~IPDRecordingUnit() {};

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
        virtual TA_Base_Bus_GZL9::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures getSupportedFeatures() = 0;

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
		virtual TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState getRecordingUnitState() = 0;


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
		virtual TA_Base_Bus_GZL9::RecordingUnitCorbaDef::recordingSpeedSequence* getRecordingSpeeds() = 0;


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
		virtual void play() = 0;


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
		virtual void fastForward() = 0;


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
		virtual void rewind() = 0;


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
		virtual void pause() = 0;


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
		virtual void stopPlayback() = 0;


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
		virtual void skipToNextChapter() = 0;


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
		virtual void skipToPreviousChapter() = 0;


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
		virtual void record() = 0;


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
		virtual void stopRecording() = 0;


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
		virtual void setRecordingSpeed( unsigned short recordingSpeedIndex ) = 0;


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
          * @param      const TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState& state
          *             The new state of the RecordingUnit.
          */
        virtual void updateState( const TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState& state ) = 0;

	};
}

#endif // IPDRECORDINGUNIT_H
