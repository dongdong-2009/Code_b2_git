#ifndef IMOSSEQUENCE_H
#define IMOSSEQUENCE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/IMOS/src/IMOSSequence.h $
  * @author:   Bruce Polmear
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/08/22 11:34:16 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the protocol dependent sections of the general functions for a Sequence
  * using the IMOS protocol.
  * Inherits from the IPDSequence class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Sequence and
  * that the Sequence and the protocol support the required features.
  */

#include <string>
#include <memory>
#include <map>

#include "app/cctv/video_switch_agent/protocols/src/IPDSequence.h"
//#include "app/cctv/video_switch_agent/protocols/IMOS/src/IStateValidationItem.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/SequenceCorbaDef.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"

#if defined(sparc)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_public.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_errcode.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/imos_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/sdk_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_sparc/include/sdk_struct.h"
#elif defined(WIN32)
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_public.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_errcode.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/imos_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/sdk_def.h"
#include "app/cctv/video_switch_agent/protocols/imos_sdk_win/include/sdk_struct.h"
#endif


namespace TA_IRS_App
{
    class IMOSSequence : public IPDSequence//, public virtual IStateValidationItem
    {

    public:

        /**
          * IMOSSequence
          *
          * Standard constructor.
          *
          * @param      Sequence& sequenceData
          *             The protocol independent Sequence object.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        IMOSSequence( TA_Base_Core::SequencePtr sequenceData );

        /**
          * ~IMOSSequence
          *
          * Standard destructor.
          */

        virtual ~IMOSSequence();


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
          * @param      const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state
          *             The new state of the Sequence.
          */
        virtual void updateState( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState& state );


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
        virtual void updateSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& sequenceConfig );


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
        virtual void setSequenceConfig( const TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig& newConfig ,bool con);


        /**
          * getSequenceConfig
          *
          * Gets the current configuration of this Sequence.
          *
          * @return     SequenceConfig
          *             The configuration currently set for this Sequence.
          */
        virtual TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceConfig* getSequenceConfig();


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
        virtual void cycleToNextVideoInput( unsigned long videoOutputEntityKey );


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
        virtual void cycleToPreviousVideoInput( unsigned long videoOutputEntityKey );


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
        virtual void pause( unsigned long videoOutputEntityKey );

        
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
        virtual void play( unsigned long videoOutputEntityKey );


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
        virtual void stop( unsigned long videoOutputEntityKey );


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
        virtual TA_Base_Bus_GZL9::SequenceCorbaDef::SequenceState getSequenceState( unsigned long videoOutputEntityKey );


        ///////////////////////////////////////////////////////////////////////
        //
        // IMOSSequence Methods
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

        IMOSSequence( const IMOSSequence& theIMOSSequence );
        IMOSSequence& operator=( const IMOSSequence& );
        
		SWITCH_BASE_S queryImosSeqBaseInfo();
        //
        // The configuration data object for this Sequence.
        //

        TA_Base_Core::SequencePtr m_sequenceData;

        unsigned short m_dwellTime; // Dwell time of sequence config.
        
        std::vector< unsigned long > m_videoInputs; // Keys of the ordered video inputs of the sequence config.
    };

} // namespace TA_IRS_App

#endif // IMOSSEQUENCE_H