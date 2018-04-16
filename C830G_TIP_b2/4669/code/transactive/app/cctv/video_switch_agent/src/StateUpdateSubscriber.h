#if !defined(StateUpdateSubscriber_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_)
#define StateUpdateSubscriber_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/StateUpdateSubscriber.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Subscribes to and processes VisualStateUpdate messages from the 
  * Video Switch Agent that is in Control mode.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "bus/cctv/video_switch_agent/IDL/src/MovementControlCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"

// FUTURE TODO Add Rec Unit, Seq, Quad updates.

namespace TA_IRS_App
{
    class StateUpdateSubscriber : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::StateUpdateMessageCorbaDef >
    {
    public:
        /**
          * StateUpdateSubscriber
          *
          * Standard constructor.
          */

        StateUpdateSubscriber() {};
    
        /**
          * StateUpdateSubscriber
          *
          * Standard destructor.
          */

        virtual ~StateUpdateSubscriber() {};
        
        ///////////////////////////////////////////////////////////////////////
        //
        // SpecialisedMessageSubscriber Method
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * receiveMessage
          *
          * Processes any received StateUpdate messages.
          */

        virtual void receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message );
    
    private:

        //
        // Disable copy constructor and assignment operator.
        //

        StateUpdateSubscriber( const StateUpdateSubscriber& theStateUpdateSubscriber );
        StateUpdateSubscriber& operator=( const StateUpdateSubscriber& theStateUpdateSubscriber );

        ///////////////////////////////////////////////////////////////////////
        //
        // Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updatePresetName
          *
          * Updates the preset name for the given Camera and preset index.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long cameraKey
          *             The database (entity) key of the Camera.
          * @param      const TA_Base_Bus::MovementControlCorbaDef::CameraPreset& preset
          *             A struct containing the preset index and new name.
          */

        void updatePresetName( unsigned long cameraKey, 
            const TA_Base_Bus::MovementControlCorbaDef::CameraPreset& preset );
    
        /**
          * updateCameraState
          *
          * Updates the state for the given Camera.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long cameraKey
          *             The database (entity) key of the Camera.
          * @param      const TA_Base_Bus::CameraCorbaDef::CameraState& state
          *             A struct containing the state of the Camera.
          */

        void updateCameraState( unsigned long cameraKey, 
            const TA_Base_Bus::CameraCorbaDef::CameraState& state );

        /**
          * updateVideoInputState
          *
          * Updates the state for the given VideoInput.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long videoInputKey
          *             The database (entity) key of the VideoInput.
          * @param      const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state
          *             A struct containing the state of the VideoInput.
          */

        void updateVideoInputState( unsigned long videoInputKey, 
            const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state );

        /**
          * updateVideoOutputState
          *
          * Updates the state for the given VideoOutput.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long videoOutputKey
          *             The database (entity) key of the VideoOutput.
          * @param      const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state
          *             A struct containing the state of the VideoOutput.
          */

        void updateVideoOutputState( unsigned long videoOutputKey, 
            const TA_Base_Bus::VideoOutputCorbaDef::VideoOutputState& state );


        /**
          * updateQuadState
          *
          * Updates the state for the given Quad.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long quadKey
          *             The database (entity) key of the Quad.
          * @param      const TA_Base_Bus::VideoOutputCorbaDef::QuadState& state
          *             A struct containing the state of the Quad.
          */

        void updateQuadState( unsigned long quadKey, 
            const TA_Base_Bus::QuadCorbaDef::QuadState& state );


        /**
          * updateSequenceState
          *
          * Updates the state for the given Sequence.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long sequenceKey
          *             The database (entity) key of the Sequence.
          * @param      const TA_Base_Bus::SequenceCorbaDef::SequenceState& state
          *             A struct containing the state of the Sequence.
          */

        void updateSequenceState( unsigned long sequenceKey, 
                                  const TA_Base_Bus::SequenceCorbaDef::SequenceState& state );


        /**
          * updateSequenceConfig
          *
          * Updates the Sequence Config for the given Sequence.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long sequenceKey
          *             The database (entity) key of the Sequence.
          * @param      const TA_Base_Bus::SequenceCorbaDef::SequenceState& state
          *             A struct containing the sequence config of the Sequence.
          */

        void updateSequenceConfig( unsigned long sequenceKey, 
                                  const TA_Base_Bus::SequenceCorbaDef::SequenceConfig& sequenceConfig );


        /**
          * updateBVSStageState
          *
          * Updates the BVSStage state for the BVS Stage
          * Any failures in this method are just logged.
          *
          * @param      unsigned long bvsKey
          *             The database (entity) key of the BVSStage.
          * @param      const TA_Base_Bus::BVSStageCorbaDef::BVSStageState& state 
          *             A struct containing the BVS Stage state
          */

        void updateBVSStageState( unsigned long bvsKey,  
									const TA_Base_Bus::BVSStageCorbaDef::BVSStageState& state );
    
		/**
          * updateRecordingUnitState
          *
          * Updates the state for the given RecordingUnit.
          * Any failures in this method are just logged.
          *
          * @param      unsigned long recordingUnitKey
          *             The database (entity) key of the RecordingUnit.
          * @param      const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state
          *             A struct containing the state of the RecordingUnit.
          */

        void updateRecordingUnitState( unsigned long recordingUnitKey, 
            const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state );

		void updateAlarmStack( TA_Base_Bus::SwitchManagerCorbaDef::AlarmStack& stack);


    }; // StateUpdateSubscriber

} // TA_IRS_App

#endif // !defined(StateUpdateSubscriber_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_)