#if !defined SWITCH_MANAGER_H
#define SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Manages access to the entities (Cameras, VideoMonitors,
  * RecordingUnits, Sequences, Quads, ...) handled by the VideoSwitchAgent.
  * Implements the SwitchManager interface.
  *
  * This class is a singleton.
  *
  * Design Note: This used to have access to entities by name.  However, names
  * can be changed, so we use entity keys to identify entities instead.
  */

#include <string>
#include <map>

#include "bus/cctv/video_switch_agent/IDL/src/SwitchManagerCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include <boost/noncopyable.hpp>
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>

// Forward declarations.

namespace TA_IRS_App
{
    class VideoOutput;
    class VideoInput;
    class VideoMonitor;
    class Camera;
    class Sequence;
}

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
	class DefaultEntity;
}

namespace TA_IRS_App
{
    class SwitchManager : public virtual POA_TA_Base_Bus::SwitchManagerCorbaDef,
        public virtual TA_Base_Core::ServantBase,
        boost::noncopyable
    {
            friend class ACE_Singleton<SwitchManager, ACE_Recursive_Thread_Mutex>;
        public:
            /**
              * getInstance
              *
              * Gets the one and only AgentModeMonitor.
              *
              * @return     SwitchManager&
              *             The only SwitchManager.
              */

            static SwitchManager& getInstance();

            /**
              * ~SwitchManager
              *
              * Standard destructor.
              */

            virtual ~SwitchManager();

            /**
              * activate
              *
              * Activates the SwitchManager to handle the Corba requests for the agent.
              * It is registered under the agent's Corba name.
              *
              * Precondition:   AgentModeMonitor::getInstance().isInControlMode()
              */

            void activate();

            ///////////////////////////////////////////////////////////////////
            //
            // SwitchManagerCorbaDef Methods
            //
            ///////////////////////////////////////////////////////////////////

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoObjectsInfo* getVideoObjectsInfo();

            /**
              * getVideoMonitors
              *
              * Retrieves the list of VideoMonitorsCorbaDef.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence*
              *             The list of VideoMonitors or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* getVideoMonitors();

            /**
              * getVideoMonitor
              *
              * Retrieves an VideoMonitorCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::VideoMonitorCorbaDef_ptr
              *             The requested VideoMonitorCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the VideoMonitorCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual TA_Base_Bus::VideoMonitorCorbaDef_ptr getVideoMonitor(unsigned long entityKey);

            /**
              * getAllConsoleMonitors
              *
              * Retrieves the list of VideoMonitors that are console monitors.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence*
              *             The list of workstation VideoMonitors or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The workstation name for a VideoMonitor cannot be determined.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* getAllConsoleMonitors();

            /**
              * getConsoleMonitors
              *
              * Retrieves the list of VideoMonitors that are connected to the specified console.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence*
              *             The list of workstation VideoMonitors or
              *             0 if the list could not be created.
              *
              * @param      unsigned long consoleKey
              *             The key of the console. This must match the VideoMonitor entity
              *             parameter value for the Console parameter.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The console key for a VideoMonitor cannot be determined.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* getConsoleMonitors(
                unsigned long consoleKey);


            /**
              * getCameras
              *
              * Retrieves the list of Cameras.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence*
              *             The list of Cameras or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence* getCameras();

            /**
              * getCamera
              *
              * Retrieves an CameraCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::CameraCorbaDef_ptr
              *             The requested CameraCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the CameraCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual TA_Base_Bus::CameraCorbaDef_ptr getCamera(unsigned long entityKey);

            /**
              * getQuads
              *
              * Retrieves the list of Quads.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::QuadSequence*
              *             The list of Quads or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            // virtual TA_Base_Bus::SwitchManagerCorbaDef::QuadSequence* getQuads();

            /**
              * getQuad
              *
              * Retrieves an QuadCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::QuadCorbaDef_ptr
              *             The requested QuadCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the QuadCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            // virtual TA_Base_Bus::QuadCorbaDef_ptr getQuad( unsigned long entityKey );

            /**
              * getBVSStages
              *
              * Retrieves the list of BVSStages.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::BVSStageSequence*
              *             The list of BVSStages or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            // virtual TA_Base_Bus::SwitchManagerCorbaDef::BVSStageSequence* getBVSStages();

            /**
              * getBVSStage
              *
              * Retrieves an BVSStageCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::BVSStageCorbaDef_ptr
              *             The requested BVSStageCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the BVSStageCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            // virtual TA_Base_Bus::BVSStageCorbaDef_ptr getBVSStage( unsigned long entityKey );

            /**
              * getSequences
              *
              * Retrieves the list of Sequences.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence*
              *             The list of Sequences or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence* getSequences();

            /**
              * getSequence
              *
              * Retrieves an SequenceCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::SequenceCorbaDef_ptr
              *             The requested SequenceCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the SequenceCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual TA_Base_Bus::SequenceCorbaDef_ptr getSequence(unsigned long entityKey);

            /**
              * getVideoOutputs
              *
              * Retrieves the list of all VideoOutputs.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence*
              *             The list of VideoOutputs or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence* getVideoOutputs();

            /**
              * getVideoOutput
              *
              * Retrieves an VideoOutputCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::VideoOutputCorbaDef_ptr
              *             The requested VideoOutputCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the VideoOutputCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual TA_Base_Bus::VideoOutputCorbaDef_ptr getVideoOutput(unsigned long entityKey);

            /**
              * getVideoInputs
              *
              * Retrieves the list of all VideoInputs.
              *
              * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence*
              *             The list of VideoInputs or
              *             0 if the list could not be created.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence* getVideoInputs();

            /**
              * getVideoInput
              *
              * Retrieves an VideoInputCorbaDef by entity key.
              *
              * @return     TA_Base_Bus::VideoInputCorbaDef_ptr
              *             The requested VideoInputCorbaDef or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the VideoInputCorbaDef to retrieve.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual TA_Base_Bus::VideoInputCorbaDef_ptr getVideoInput(unsigned long entityKey);

            /**
              * clearAllMonitorsWithInput
              *
              * clears all monitors that have the specified input as an assignment
              *
              * @param      unsigned long inputEntityKey
              *             the entity of the input entity to be removed from all monitors
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The alarm with the given iD is not found.
              *             3. The alarm not configured to be manually closed
              */
            virtual void clearAllMonitorsWithInput(unsigned long inputEntityKey, const char* sessionId);


            ///////////////////////////////////////////////////////////////////
            //
            // Add/Remove Entity Methods
            //
            ///////////////////////////////////////////////////////////////////

            /**
              * addVideoMonitor
              *
              * Adds the given VideoMonitor.
              *
              * @param      VideoMonitor& videoMonitor
              *             The new VideoMonitor.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if an VideoMonitor with the given entity key already exists.
              * @exception  VideoSwitchAgentException (Corba exception)
              *             Thrown if the key of the VideoOutputGroup could not be determined.
              */

            void addVideoMonitor(VideoMonitor& videoMonitor);

            /**
              * removeVideoMonitor
              *
              * Removes the given VideoMonitor. This method does nothing if it
              * can't find a VideoMonitor with the given entity key.
              *
              * @param      unsigned long entityKey
              *             The VideoMonitor to remove.
              */

            void removeVideoMonitor(unsigned long entityKey);


            /**
              * addCamera
              *
              * Adds the given Camera.
              *
              * @param      Camera& camera
              *             The new Camera.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if a Camera with the given entity key already exists.
              * @exception  VideoSwitchAgentException (Corba exception)
              *             Thrown if the key of the VideoOutputGroup could not be determined.
              */

            void addCamera(Camera& camera);

            /**
              * removeCamera
              *
              * Removes the given Camera. This method does nothing if it
              * can't find a Camera with the given entity key.
              *
              * @param      unsigned long entityKey
              *             The Camera to remove.
              */

            void removeCamera(unsigned long entityKey);

            /**
              * addQuad
              *
              * Adds the given Quad.
              *
              * @param      Quad& quad
              *             The new Quad.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if an Quad with the given entity key already exists.
              * @exception  VideoSwitchAgentException (Corba exception)
              *             Thrown if the key of the VideoOutputGroup could not be determined.
              */

            //void addQuad( Quad& quad );

            /**
              * removeQuad
              *
              * Removes the given Quad. This method does nothing if it
              * can't find a Quad with the given entity key.
              *
              * @param      unsigned long entityKey
              *             The Quad to remove.
              */

            // void removeQuad( unsigned long entityKey );

            /**
              * addBVSStage
              *
              * Adds the given BVSStage.
              *
              * @param      BVSStage& bvsStage
              *             The new BVSStage.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if an BVSStage with the given entity key already exists.
              * @exception  VideoSwitchAgentException (Corba exception)
              *             Thrown if the key of the VideoOutputGroup could not be determined.
              */

            //void addBVSStage( BVSStage& bvsStage );

            /**
              * removeBVSStage
              *
              * Removes the given BVSStage. This method does nothing if it
              * can't find a BVSStage with the given entity key.
              *
              * @param      unsigned long entityKey
              *             The BVSStage to remove.
              */

            //void removeBVSStage( unsigned long entityKey );

            /**
              * addSequence
              *
              * Adds the given Sequence.
              *
              * @param      Sequence& sequence
              *             The new Sequence.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if an Sequence with the given entity key already exists.
              * @exception  VideoSwitchAgentException (Corba exception)
              *             Thrown if the key of the VideoOutputGroup could not be determined.
              */

            void addSequence(Sequence& sequence);

            /**
             * removeSequence
             *
             * Removes the given Sequence. This method does nothing if it
             * can't find a Sequence with the given entity key.
             *
             * @param      unsigned long entityKey
             *             The Sequence to remove.
             */

            void removeSequence(unsigned long entityKey);

            ///////////////////////////////////////////////////////////////////
            //
            // Helper Methods
            //
            ///////////////////////////////////////////////////////////////////

            /**
              * getVideoOutputInternal
              *
              * Retrieves an VideoOutput by entity key.
              *
              * @return     VideoOutput&
              *             The requested VideoOutput or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the VideoOutput to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual VideoOutput& getVideoOutputInternal(unsigned long entityKey);

            /**
              * getVideoInputInternal
              *
              * Retrieves an VideoInput by entity key.
              *
              * @return     VideoInput&
              *             The requested VideoInput or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the VideoInput to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual VideoInput& getVideoInputInternal(unsigned long entityKey);

            /**
              * getCameraInternal
              *
              * Retrieves an Camera by entity key.
              *
              * @return     Camera&
              *             The requested Camera or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the Camera to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual Camera& getCameraInternal(unsigned long entityKey);


            /**
              * getQuadInternal
              *
              * Retrieves a Quad by entity key.
              *
              * @return     Quad&
              *             The requested Quad or
              *             0 if the entity key is not known.
              *
              * @param      unsigned long entityKey
              *             The entity key of the Quad to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            //virtual Quad& getQuadInternal( unsigned long entityKey );


            /**
              * getSequenceInternal
              *
              * Retrieves an Sequence by entity key.
              *
              * @return     Sequence&
              *             The requested Sequence
              *
              * @param      unsigned long entityKey
              *             The entity key of the Sequence to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            virtual Sequence& getSequenceInternal(unsigned long entityKey);

            /**
              * getBVSStageInternal
              *
              * Retrieves an BVSStage by entity key.
              *
              * @return     BVSStage&
              *             The requested BVSStage
              *
              * @param      unsigned long entityKey
              *             The entity key of the BVSStage to retrieve.
              *
              * @exception  VideoSwitchAgentInternalException
              *             Thrown if:
              *             1. The agent is in Monitor mode.
              *             2. The object with the given key is not found.
              */

            //virtual BVSStage& getBVSStageInternal( unsigned long entityKey );

			std::vector<Camera*> getInternalCameras();

			std::vector<Sequence*> getInternalSequences();

			std::vector<VideoMonitor*> getInternalMonitors();


            void processAlarmStack(const ::TA_Base_Bus::SwitchManagerCorbaDef::TriggerAlarmSequence& alarms, ::CORBA::Boolean bOpen)
            {
            }

            void acknowledgeAlarm(const ::TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail, ::CORBA::ULong consoleId);

            void acknowledgeTrainCameraAlarm(::CORBA::ULong locationId, ::CORBA::ULong trainId, const ::TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence& cameraIds, ::CORBA::ULong consoleId)
            {
            }
            void setStageSwitchMode(::CORBA::ULong originatingStage, ::CORBA::ULong destinationStage, ::CORBA::Boolean allowSwitch)
            {
            }
            void closeAlarm(const ::TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail)
            {
            }

			void addCctvAlarmEntity(TA_Base_Core::DefaultEntity* cctvAlarmEntity);
        private:
            typedef std::map< unsigned long, VideoOutput* >         VideoOutputMap;
            typedef std::map< unsigned long, VideoInput* >          VideoInputMap;
            typedef std::map< unsigned long, VideoMonitor* >        VideoMonitorMap;
            typedef std::map< unsigned long, Camera* >              CameraMap;
            typedef std::map< unsigned long, Sequence* >            SequenceMap;
			typedef std::map< unsigned long, TA_Base_Core::DefaultEntity*> CctvAlarmMap;

            /**
              * SwitchManager
              *
              * Standard constructor. This is private because this class is a singleton.
              * Use getInstance() to get the one and only instance of this class.
              */
            SwitchManager();
            //
            // The GenericAgent object within the SwitchManagerAgent class maintains
            // the objects contained in these vectors as Entity objects.
            // It will clean them up on shutdown.
            // All objects are referenced by entity key.
            //

            VideoOutputMap          m_videoOutputs;
            VideoInputMap           m_videoInputs;
            VideoMonitorMap         m_videoMonitors;
            CameraMap               m_cameras;
            SequenceMap             m_sequences;
			CctvAlarmMap            m_cctvAlarms;
    };

} // TA_IRS_App

#endif // SWITCH_MANAGER_H
