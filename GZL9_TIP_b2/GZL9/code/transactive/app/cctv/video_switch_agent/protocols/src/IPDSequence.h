#ifndef IPDSEQUENCE_H
#define IPDSEQUENCE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDSequence.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Handles the protocol dependent sections of the operations for a Video Sequence.
 *
 */

#include "bus/cctv_gzl9/video_switch_agent/IDL/src/SequenceCorbaDef.h"

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
          * @param      const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state
          *             The new state of the Sequence.
          */

        virtual void updateState( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state ) = 0;


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
          * @param      const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig
          *             The new Sequence Config of the Sequence.
          */

        virtual void updateSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig ) = 0;


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
        virtual void setSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& newConfig,bool con ) = 0;


        /** 
          * getSequenceConfig
          *
          * Gets the current configuration of this Sequence.
          *
          * @return     SequenceConfig
          *             The configuration set for this Sequence.
          */
        virtual TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* getSequenceConfig() = 0;


        /**
          * cycleToNextVideoInput
          *
          * Manually cycles sequence playback to next VideoInput.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          */
        virtual void cycleToNextVideoInput( unsigned long videoOutputEntityKey ) = 0;


        /**
          * cycleToPreviousVideoInput
          *
          * Manually cycles sequence playback to previous VideoInput.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          */
        virtual void cycleToPreviousVideoInput( unsigned long videoOutputEntityKey ) = 0;


        /**
          * pauses
          *
          * Pauses playback of sequence.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          */
        virtual void pause( unsigned long videoOutputEntityKey ) = 0;

		
        /**
          * play
          *
          * Begin/Resume Playback of sequence.
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          *
          */
        virtual void play( unsigned long videoOutputEntityKey ) = 0;


        /**
          * stop
          *
          * Stop playback of sequence (can be same behaviour as pause depending on hardware).
          *
          * @param      unsigned long videoOutputEntityKey
          *             EntityKey of VideoOutput the dynamic playback manipulation
          *             should be applied to (set to 0 to apply to all VideoOutputs currently
          *             assigned this Sequence).
          */
        virtual void stop( unsigned long videoOutputEntityKey ) = 0;


        /**
          * getSequenceState
          *
          * Returns the sequence's current state.
          *
          * @param      unsigned long videoOutputEntityKey
          *             The key of the video output that the sequence is running on.
          *
          * @return     TA_Base_Bus_GZL9::SequenceCorbaDef::ESequenceState
          *             The sequence's current state.
          */

        virtual TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState getSequenceState( unsigned long videoOutputEntityKey ) = 0;

	};
}

#endif // IPDSEQUENCE_H
