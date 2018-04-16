#ifndef IPDSEQUENCE_H
#define IPDSEQUENCE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/src/IPDSequence.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * Handles the protocol dependent sections of the operations for a Video Sequence.
 *
 */

#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"

namespace TA_IRS_App
{
	class IPDSequence
	{

	public:

		/** 
		 * Destructor
		 *
		 */
		virtual ~IPDSequence() {};


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

        virtual void updateState( const TA_Base_Bus::SequenceCorbaDef::SequenceState& state ) = 0;


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

        virtual void updateSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig ) = 0;


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
		virtual void getDwellTimeRange( unsigned short& minimumDwellTime, unsigned short& maximumDwellTime ) = 0;


        /**
          * getMaxNumberOfInputs
          *
          * Returns the maximum number of VideoInputs that can be configured.
          *
          * @return     unsigned short
          *             The maximum number of VideoInputs that can
          *             be configured for this sequence.
          */
        virtual unsigned short getMaxNumberOfInputs() = 0;


        /**
          * setSequenceConfig
          *
          * Sets the current configuration of this Sequence.
          *
          * @param      SequenceConfig newConfig
          *             The configuration to be set for this Sequence.
          */
        virtual void setSequenceConfig( const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& newConfig ) = 0;


        /** 
          * getSequenceConfig
          *
          * Gets the current configuration of this Sequence.
          *
          * @return     SequenceConfig
          *             The configuration set for this Sequence.
          */
        virtual TA_Base_Bus::SequenceCorbaDef::SequenceConfig* getSequenceConfig() = 0;


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

        virtual TA_Base_Bus::SequenceCorbaDef::SequenceState getSequenceState( unsigned long videoOutputEntityKey ) = 0;

		virtual void addCameraForSequence(TA_IRS_App::SwitchManager::CameraMap& cameras) = 0;

	};
}

#endif // IPDSEQUENCE_H
