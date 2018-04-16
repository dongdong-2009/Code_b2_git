#ifndef SWITCHAGENTCOMMUNICATOR_H
#define SWITCHAGENTCOMMUNICATOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_display_manager/src/SwitchAgentCommunicator.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Singleton to provide access to Agent data and functions
  *
  */

#include <map>
#include <string>
#include <vector>

#include "stdafx.h"

#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"

#include "core/naming/src/NamedObject.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/types/VisualComms_MessageTypes.h"

#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "VideoOutput.h"

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

namespace TA_IRS_App
{
  //  class BvsStage;
    class Camera;
  //  class Quad;
    class Sequence;
    class Monitor;

    class DisplayItem;
    class VideoInput;

    class SwitchAgentCommunicator
    {
    public:


        enum ESequencePlaybackCommandType
        {
            SEQUENCE_HOLD,
            SEQUENCE_SKIP_FORWARD,
            SEQUENCE_SKIP_BACKWARD,
            SEQUENCE_PLAY
        };

        /**
          * getInstance
          *
          * Return reference to this singleton.
          *
          * @return reference to this singleton.
          */
        static SwitchAgentCommunicator& getInstance();


       /**
         * removeInstance
         *
         * Removes the instance of the class (if already created) and cleanup the members.  Primarily
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         */
        static void removeInstance();

        virtual ~SwitchAgentCommunicator();


        /**
          * assignMonitor
          *
          * Assigns an input to a monitor
          *
          * @param monitor  The monitor to assign to
          * @param source   The source
          *
          * @exception  TA_Core::TransactiveException
          *             If the assignment fails
          *             the reason will be given so it can be formatted into an error messsage
          */
        void assignMonitor( Monitor* monitor,
                            VideoInput* source );

        /**
          * assignQuad
          *
          * Assigns cameras to a quad.
          *
          * @param quad             a pointer to the quad (not NULL)
          * @param topLeftCam       the new top left camera (or NULL to clear)
          * @param topRightCam      the new top right camera (or NULL to clear)
          * @param bottomLeftCam    the new bottom left camera (or NULL to clear)
          * @param bottomRightCam   the new bottom right camera (or NULL to clear)
          *
          * @exception  TA_Core::TransactiveException
          *             If the assignment fails
          *             the reason will be given so it can be formatted into an error messsage
          */
        void assignQuad( Monitor* monior,
                         Camera* topLeftCam,
                         Camera* topRightCam,
                         Camera* bottomLeftCam,
                         Camera* bottomRightCam );


        /**
          * setSequenceConfig
          *
          * Sets the dwell time and cameras for a sequence
          *
          * @param sequence     The sequence (not NULL)
          * @param dwellTime    the new dwelltime
          * @param cameraInputs the list of cameras (none NULL)
          *
          * @exception  TA_Core::TransactiveException
          *             If the assignment fails
          *             the reason will be given so it can be formatted into an error messsage
          */
        void setSequenceConfig( Sequence* sequence,
                                unsigned short dwellTime,
                                const std::list< Camera* > cameraInputs );


        /**
          * setSequenceDescription
          *
          * Sets the description for a sequence
          *
          * @param sequence         The sequence (not NULL)
          * @param newDescription   The new description
          *
          * @exception  TA_Core::TransactiveException
          *             If the assignment fails
          *             the reason will be given so it can be formatted into an error messsage
          */
        void setSequenceDescription( Sequence* sequence,
                                     const std::string& newDescription );


        /**
          * clearMonitor
          *
          * clears the given monitor
          *
          * @param monitor  the monitor to clear (not NULL)
          *
          * @exception  TA_Core::TransactiveException
          *             If the assignment fails
          *             the reason will be given so it can be formatted into an error messsage
          */
        void clearMonitor( Monitor* monitor );

		// for test support
		void acknowledgeAlarm( unsigned long entityKey);
        /* These are used by the DisplayItemManager to build the initial states of everything */

        typedef std::map< unsigned long, TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject* >        CamKeyToNamedObject;
        typedef std::map< unsigned long, TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject* >      SequenceKeyToNamedObject;
        typedef std::map< unsigned long, TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject* >  MonitorKeyToNamedObject;
        typedef std::map< unsigned long, TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* >   OutputKeyToNamedObject;

        /**
          * updateSequence
          *
          * This is called by the DisplayItemManager to update a sequences
          * details when it receives a config update.
          *
          * @param sequence The sequence to update (not NULL)
          */
        void updateSequence(Sequence* sequence);


    private:
        /**
          * Default Constructor
          *
          * Constructor is private so can control number of instances
          */
        SwitchAgentCommunicator();


        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        SwitchAgentCommunicator( const SwitchAgentCommunicator& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        SwitchAgentCommunicator& operator=( const SwitchAgentCommunicator& );


        /* Look up names in the maps and return corresponding named objects
           or NULL if there is none matching */
        TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject*     getMonObjByKey(const unsigned long key);
        TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject*          getCamObjByKey(const unsigned long key);
        TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject*        getSeqObjByKey(const unsigned long key);
    private:

        CamKeyToNamedObject m_camKeyToNamedObject;
        SequenceKeyToNamedObject m_seqKeyToNamedObject;
        MonitorKeyToNamedObject m_monKeyToNamedObject;

        static SwitchAgentCommunicator* m_theInstance; // pointer to the single instance of this class.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock; // Semaphore to thread guard this singleton.
    };

} // namespace TA_IRS_App

#endif // SWITCHAGENTCOMMUNICATOR_H
