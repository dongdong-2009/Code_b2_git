#ifndef IMOSRECORDINGUNIT_H
#define IMOSRECORDINGUNIT_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSRecordingUnit.h $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/21 08:52:08 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Quad
  * using the IMOS protocol.
  * Inherits from the IPDRecordingUnit class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Quad and
  * that the Quad and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/src/IPDRecordingUnit.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/IStateValidationItem.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"
#include "core/data_access_interface/entity_access/src/RecordingUnit.h"

#include <string>
#include <memory>

// Forward declarations.

namespace TA_IRS_App
{
    class IMOSRecordingUnit : public IPDRecordingUnit//, public virtual IStateValidationItem
    {
    public:

        /**
          * IMOSRecordingUnit
          *
          * Standard constructor.
          *
          * @param      Quad& QuadData
          *             The protocol independent Quad object.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        IMOSRecordingUnit( TA_Base_Core::RecordingUnitPtr recordingUnitData );

        /**
          * ~IMOSRecordingUnit
          *
          * Standard destructor.
          */

        virtual ~IMOSRecordingUnit() {};

        virtual unsigned long getKey();

        virtual std::string getAddress();

        /**
          * notifyRecordingSpeedSet
          *
          * This is called when data is received from the hardware saying that the recording units
          * speed has been set, so the agent should be in Control mode.
          * Sends a message to notify agent clients that this recording units recording speed has
          * been set.
          *
          * @param      unsigned short recordingSpeedIndex
          *             The index of the recordingSpeed (relative to the list returned by getRecordingSpeeds()
          *             this RecordingUnit has been set to.
          */
        void notifyRecordingSpeedSet( unsigned short recordingSpeedIndex );

        ///////////////////////////////////////////////////////////////////////
        //
        // IPDRecordingUnit Methods
        //
        ///////////////////////////////////////////////////////////////////////

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
        TA_Base_Bus_GZL9::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures getSupportedFeatures();


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
		virtual TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState getRecordingUnitState();


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
		virtual TA_Base_Bus_GZL9::RecordingUnitCorbaDef::recordingSpeedSequence* getRecordingSpeeds();


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
		virtual void setRecordingSpeed( unsigned short recordingSpeedIndex );


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
		virtual void play();


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
		virtual void fastForward();


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
		virtual void rewind();


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
		virtual void pause();


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
		virtual void stopPlayback();


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
		virtual void skipToNextChapter();


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
		virtual void skipToPreviousChapter();


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
		virtual void record();


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
		virtual void stopRecording();


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
        virtual void updateState( const TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState& state );


        ///////////////////////////////////////////////////////////////////////
        //
        // IStateValidationItem Methods
        //
        ///////////////////////////////////////////////////////////////////////


        /** 
          * validateState
          *
          * Callback for StateValidationCycle to let this item know its time
          * to validate its state.
          */
        virtual void validateState();


    private:
        //
        // Disable copy constructor and assignment operator.
        //

        IMOSRecordingUnit( const IMOSRecordingUnit& theIMOSRecordingUnit );
        IMOSRecordingUnit& operator=( const IMOSRecordingUnit& );
        
        //
        // The configuration data object for this Quad.
        //

        TA_Base_Core::RecordingUnitPtr m_recordingUnitData;

        TA_Base_Bus_GZL9::RecordingUnitCorbaDef::RecordingUnitState m_recordingUnitState;    // The current state of this Quad.

    };

} // namespace TA_IRS_App

#endif // IMOSRECORDINGUNIT_H
