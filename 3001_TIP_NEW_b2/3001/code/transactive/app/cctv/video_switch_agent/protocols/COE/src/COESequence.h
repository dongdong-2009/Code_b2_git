#ifndef COESEQUENCE_H
#define COESEQUENCE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COESequence.h $
  * @author:   Bruce Polmear
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2015/08/28 13:33:23 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * Handles the protocol dependent sections of the general functions for a Sequence
  * using the COE protocol.
  * Inherits from the IPDSequence class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Sequence and
  * that the Sequence and the protocol support the required features.
  */

#include <string>
#include <memory>
#include <map>

#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDSequence.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"


namespace TA_IRS_App
{
    class COESequence : public IPDSequence
    {

    public:

        /**
          * COESequence
          *
          * Standard constructor.
          *
          * @param      Sequence& sequenceData
          *             The protocol independent Sequence object.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        COESequence( TA_Base_Core::SequencePtr sequenceData );

        /**
          * ~COESequence
          *
          * Standard destructor.
          */

        virtual ~COESequence();


        /** 
          * notifySequenceConfigUpdated
          *
          * This is called when data is received from the hardware saying that a
          * modification to sequence config has occured, so the agent should be in Control mode.
          * Sends a message to notify agent clients that the sequence config has
          * been modified
          *
          * @param unsigned short dwellTime
          *        The dwell time of the sequence config
          * @param std::vector< unsigned long >& videoInputs
          *        Keys of the ordered video inputs of the sequence config.
          */
        void notifySequenceConfigUpdated( unsigned short dwellTime, const std::vector< unsigned long >& videoInputs );


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
        virtual void updateState( const TA_Base_Bus::SequenceCorbaDef::SequenceState& state );


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
        virtual void updateSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig );


        std::string getAddress();


        ///////////////////////////////////////////////////////////////////////
        //
        // IPDSequence Methods
        //
        ///////////////////////////////////////////////////////////////////////

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
         */
        virtual void getDwellTimeRange( unsigned short& minimumDwellTime,
                                        unsigned short& maximumDwellTime );


        /**
          * getMaxNumberOfInputs
          *
          * Returns the maximum number of VideoInputs that can be configured.
          *
          * @return     unsigned short
          *             The maximum number of VideoInputs that can
          *             be configured for this sequence.
          */
        virtual unsigned short getMaxNumberOfInputs();


        /**
          * setSequenceConfig
          *
          * Sets the current configuration of this Sequence.
          *
          * @param      SequenceConfig newConfig
          *             The configuration to be set for this Sequence.
          */
        virtual void setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig );


        /**
          * getSequenceConfig
          *
          * Gets the current configuration of this Sequence.
          *
          * @return     SequenceConfig
          *             The configuration currently set for this Sequence.
          */
        virtual TA_Base_Bus::SequenceCorbaDef::SequenceConfig* getSequenceConfig();


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
          */
        virtual TA_Base_Bus::SequenceCorbaDef::SequenceState getSequenceState( unsigned long videoOutputEntityKey );


        ///////////////////////////////////////////////////////////////////////
        //
        // COESequence Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getKey
          *
          * Gets the database entity key of the Camera.
          *
          * @return        unsigned long
          *                The database entity key of the Camera.
          *
          */

        unsigned long getKey() const;


        ///////////////////////////////////////////////////////////////////////
        //
        // IStateValidationItem Methods
        //
        ///////////////////////////////////////////////////////////////////////

		void addCameraForSequence(TA_IRS_App::SwitchManager::CameraMap& cameras);

    private:


        //
        // Disable copy constructor and assignment operator.
        //

        COESequence( const COESequence& theCOESequence );
        COESequence& operator=( const COESequence& );
        
        //
        // The configuration data object for this Sequence.
        //

        TA_Base_Core::SequencePtr m_sequenceData;

        unsigned short m_dwellTime; // Dwell time of sequence config.
        
        std::vector< unsigned long > m_videoInputs; // Keys of the ordered video inputs of the sequence config.

		std::string m_cameraList;
    };

} // namespace TA_IRS_App

#endif // COESEQUENCE_H