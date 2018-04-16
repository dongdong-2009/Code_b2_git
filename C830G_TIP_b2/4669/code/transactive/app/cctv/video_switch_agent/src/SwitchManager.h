#if !defined SWITCH_MANAGER_H
#define SWITCH_MANAGER_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/SwitchManager.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
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
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/RecordingUnitCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDAlarmInterface.h"


#include "core/corba/src/ServantBase.h"

// Forward declarations.

namespace TA_IRS_App
{
    class VideoOutputGroup;
    class VideoOutput;
    class VideoInput;
    class VideoMonitor;
    class RecordingUnit;
    class Camera;
    class Quad;
    class Sequence;
    class BVSStage;
    class IPDStageModeInterface; //++tianxiang Merge ECP++
}

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}

namespace TA_IRS_App
{
    class SwitchManager : public virtual POA_TA_Base_Bus::SwitchManagerCorbaDef,
                          public virtual TA_Base_Core::ServantBase
    {
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

        /**
          * getVideoOutputGroups
          *
	      * Retrieves the list of VideoOutputGroups. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputGroupSequence* 
          *             The list of VideoOutputGroups or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputGroupSequence* getVideoOutputGroups();

	    /**
          * getVideoOutputGroup
          *
	      * Retrieves an VideoOutputGroupCorbaDef by entity key.
	      * VideoOutputGroup is a video output group, 
          * such as a video wall monitor group, or a wallboard.
          *
          * @return     TA_Base_Bus::VideoOutputGroupCorbaDef_ptr
          *             The requested VideoOutputGroupCorbaDef or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the VideoOutputGroupCorbaDef to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::VideoOutputGroupCorbaDef_ptr getVideoOutputGroup( unsigned long entityKey );

        /**
          * getVideoMonitors
          *
	      * Retrieves the list of VideoMonitorsCorbaDef. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* 
          *             The list of VideoMonitors or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::VideoMonitorCorbaDef_ptr getVideoMonitor( unsigned long entityKey );

        /**
          * getAllConsoleMonitors
          *
	      * Retrieves the list of VideoMonitors that are console monitors. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence*
          *             The list of workstation VideoMonitors or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
          *             1. The agent is in Monitor mode.
          *             2. The console key for a VideoMonitor cannot be determined.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::VideoMonitorSequence* getConsoleMonitors( 
            unsigned long consoleKey );

        /**
          * getRecordingUnits
          *
	      * Retrieves the list of RecordingUnits. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::RecordingUnitSequence*
          *             The list of RecordingUnits or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::RecordingUnitSequence* getRecordingUnits();

        /**
          * getConsoleRecordingUnits
          *
	      * Retrieves the list of RecordingUnits that are connected to the specified console. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::RecordingUnitSequence*
          *             The list of workstation RecordingUnits or
          *             0 if the list could not be created.
		  *
          * @param      unsigned long consoleKey
          *             The key of the console. This must match the RecordingUnit entity
          *             parameter value for the Console parameter.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
          *             1. The agent is in Monitor mode.
          *             2. The console key for a RecordingUnit cannot be determined.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::RecordingUnitSequence* getConsoleRecordingUnits( unsigned long consoleKey );

	    /**
          * getRecordingUnit
          *
	      * Retrieves an RecordingUnitCorbaDef by entity key.
          *
          * @return     TA_Base_Bus::RecordingUnitCorbaDef_ptr
          *             The requested RecordingUnitCorbaDef or
          *             0 if the entity key is not known.
          *
          * @param      unsigned long entityKey
          *             The entity key of the RecordingUnitCorbaDef to retrieve.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::RecordingUnitCorbaDef_ptr getRecordingUnit( unsigned long entityKey );

        /**
          * getCameras
          *
	      * Retrieves the list of Cameras. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::CameraSequence* 
          *             The list of Cameras or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::CameraCorbaDef_ptr getCamera( unsigned long entityKey );

        /**
          * getQuads
          *
	      * Retrieves the list of Quads. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::QuadSequence* 
          *             The list of Quads or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::QuadSequence* getQuads();

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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::QuadCorbaDef_ptr getQuad( unsigned long entityKey );

        /**
          * getBVSStages
          *
	      * Retrieves the list of BVSStages. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::BVSStageSequence* 
          *             The list of BVSStages or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
	      */

        virtual TA_Base_Bus::SwitchManagerCorbaDef::BVSStageSequence* getBVSStages();

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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::BVSStageCorbaDef_ptr getBVSStage( unsigned long entityKey );

        /**
          * getSequences
          *
	      * Retrieves the list of Sequences. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::SequenceSequence* 
          *             The list of Sequences or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::SequenceCorbaDef_ptr getSequence( unsigned long entityKey );

        /**
          * getVideoOutputs
          *
	      * Retrieves the list of all VideoOutputs. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoOutputSequence* 
          *             The list of VideoOutputs or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::VideoOutputCorbaDef_ptr getVideoOutput( unsigned long entityKey );

        /**
          * getVideoInputs
          *
	      * Retrieves the list of all VideoInputs. 
          *
          * @return     TA_Base_Bus::SwitchManagerCorbaDef::VideoInputSequence*
          *             The list of VideoInputs or
          *             0 if the list could not be created.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the agent is in Monitor mode.
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
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual TA_Base_Bus::VideoInputCorbaDef_ptr getVideoInput( unsigned long entityKey );

		/**
          * acknowledgeAlarm
          *
	      * acknowledges an alarm on the videoswitch alarm stack
          *
          * @param      AlarmDetailCorbaDef pAlarmDetail
          *             the transactive alarm Id associated with the alarm to be Acked
		  *				TD15853 - changed param to AlarmDetailCorbaDef
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */

        virtual void acknowledgeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail, CORBA::ULong consoleId);
        
		/**
          * acknowledgeTrainCameraAlarm (station to OCC version)
          *
	      * acknowledges an alarm on the videoswitch alarm stack
          *
          * @param      unsigned long stationId, unsigned long eventId
          *					the triggeringEvent to close
		  *				unsigned long monitorId the workstationId for the console that acked the alarm
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
	      */

        virtual void acknowledgeTrainCameraAlarm( unsigned long locationId, unsigned long trainId, const TA_Base_Bus::SwitchManagerCorbaDef::TrainCameraSequence& cameraIds, unsigned long consoleId );
	    //virtual void acknowledgeTrainCameraAlarm( unsigned long locationId, unsigned long trainId, unsigned long cameraId, unsigned long consoleId );

		
		void acknowledgeAlarmWithEvent( unsigned long stationId, unsigned long eventId, unsigned long consoleId );

		/**
          * closeAlarm
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      AlarmDetailCorbaDef pAlarmDetail
          *             the transactive alarm Id associated with the alarm to be closed
		  *				TD15853 - changed param to AlarmDetailCorbaDef
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */

        virtual void closeAlarm(const TA_Base_Core::AlarmDetailCorbaDef& pAlarmDetail);

		/**
          * closeAlarm (station to OCC version)
          *
	      * closed an alarm on the videoswitch alarm stack that the switch cant close itself
          *
          * @param      in unsigned long stationId, in unsigned long eventId
          *             the triggeringEvent to close
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */

        virtual void closeAlarmWithEvent( unsigned long stationId, unsigned long eventId );

		/**
          * clearAllMonitorsWithInput
          *
	      * clears all monitors that have the specified input as an assignment
          *
          * @param      unsigned long inputEntityKey
          *             the entity of the input entity to be removed from all monitors
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The alarm with the given iD is not found.
		  *				3. The alarm not configured to be manually closed
	      */
		virtual void clearAllMonitorsWithInput( unsigned long inputEntityKey, const char* sessionId );

		virtual void ping(){};

		
		///////////////////////////////////////////////////////////////////
        //
        // Add/Remove Entity Methods
        //
        ///////////////////////////////////////////////////////////////////
  
        /** 
          * addVideoOutputGroup
          *
          * Adds the given VideoOutputGroup.
          *
          * @param      VideoOutputGroup& videoOutputGroup
          *             The new VideoOutputGroup.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if an VideoOutputGroup with the given entity key already exists.
          * @exception  VideoSwitchAgentException (Corba exception)
          *             Thrown if the key of the VideoOutputGroup could not be determined.
          */
        
        void addVideoOutputGroup( VideoOutputGroup& videoOutputGroup );

        /** 
          * removeVideoOutputGroup
          *
          * Removes the given VideoOutputGroup. This method does nothing if it
          * can't find a VideoOutputGroup with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The VideoOutputGroup to remove.
          */
        
        void removeVideoOutputGroup( unsigned long entityKey );

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
        
        void addVideoMonitor( VideoMonitor& videoMonitor );

        /** 
          * removeVideoMonitor
          *
          * Removes the given VideoMonitor. This method does nothing if it
          * can't find a VideoMonitor with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The VideoMonitor to remove.
          */
        
        void removeVideoMonitor( unsigned long entityKey );

        /** 
          * addRecordingUnit
          *
          * Adds the given RecordingUnit.
          *
          * @param      RecordingUnit& recordingUnit
          *             The new RecordingUnit.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if an RecordingUnit with the given entity key already exists.
          * @exception  VideoSwitchAgentException (Corba exception)
          *             Thrown if the key of the VideoOutputGroup could not be determined.
          */
        
        void addRecordingUnit( RecordingUnit& recordingUnit );

        /** 
          * removeRecordingUnit
          *
          * Removes the given RecordingUnit. This method does nothing if it
          * can't find a RecordingUnit with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The RecordingUnit to remove.
          */
        
        void removeRecordingUnit( unsigned long entityKey );
    
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
        
        void addCamera( Camera& camera );

        /** 
          * removeCamera
          *
          * Removes the given Camera. This method does nothing if it
          * can't find a Camera with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The Camera to remove.
          */
        
        void removeCamera( unsigned long entityKey );
 
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
        
        void addQuad( Quad& quad );

        /** 
          * removeQuad
          *
          * Removes the given Quad. This method does nothing if it
          * can't find a Quad with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The Quad to remove.
          */
        
        void removeQuad( unsigned long entityKey );

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
        
        void addBVSStage( BVSStage& bvsStage );

        /** 
          * removeBVSStage
          *
          * Removes the given BVSStage. This method does nothing if it
          * can't find a BVSStage with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The BVSStage to remove.
          */
        
        void removeBVSStage( unsigned long entityKey );

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
        
        void addSequence( Sequence& sequence );

         /** 
          * removeSequence
          *
          * Removes the given Sequence. This method does nothing if it
          * can't find a Sequence with the given entity key.
          *
          * @param      unsigned long entityKey
          *             The Sequence to remove.
          */
        
        void removeSequence( unsigned long entityKey ); 

        /**
          * setAlarmInterface
          *
	      * sets The AlarmInterface in whick to ack/close alarms on
          *
          * @param      IPDAlarmInterface alarmInterface
          *             The AlarmInterface in whick to ack/close alarms on
		  *
		  */
        void setAlarmInterface(IPDAlarmInterface* alarmInterface);

		IPDAlarmInterface* getAlarmInterface();
//++tianxiang Merge ECP++
        void setStageModeInterface(IPDStageModeInterface* stageModeInterface);
        
        IPDStageModeInterface* getStageModeInterface();
//++tianxiang Merge ECP++
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
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual VideoOutput& getVideoOutputInternal( unsigned long entityKey );

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
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual VideoInput& getVideoInputInternal( unsigned long entityKey );

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
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual Camera& getCameraInternal( unsigned long entityKey );


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
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. The object with the given key is not found.
	      */

        virtual Quad& getQuadInternal( unsigned long entityKey );


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

        virtual Sequence& getSequenceInternal( unsigned long entityKey );

        /**
          * getRecordingUnitInternal
          *
          * Retrieves an RecordingUnit by entity key.
          *
          * @return     RecordingUnit&
          *             The requested RecordingUnit
          *
          * @param      unsigned long entityKey
          *             The entity key of the RecordingUnit to retrieve.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if:
          *             1. The agent is in Monitor mode.
          *             2. The object with the given key is not found.
          */

        virtual RecordingUnit& getRecordingUnitInternal( unsigned long entityKey );

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

        virtual BVSStage& getBVSStageInternal( unsigned long entityKey );


        void setStageSwitchMode(unsigned long originatingStage, unsigned long destinationStage, bool allowSwitch); //++tianxiang Merge ECP++
    private:
        typedef std::map< unsigned long, VideoOutputGroup* >    VideoOutputGroupMap;
        typedef std::map< unsigned long, VideoOutput* >         VideoOutputMap;
        typedef std::map< unsigned long, VideoInput* >          VideoInputMap;
        typedef std::map< unsigned long, VideoMonitor* >        VideoMonitorMap;
        typedef std::map< unsigned long, RecordingUnit* >       RecordingUnitMap;
        typedef std::map< unsigned long, Camera* >              CameraMap;
        typedef std::map< unsigned long, Quad* >                QuadMap;
        typedef std::map< unsigned long, BVSStage* >            BVSStageMap;
        typedef std::map< unsigned long, Sequence* >            SequenceMap;

        /**
          * SwitchManager
          *
          * Standard constructor. This is private because this class is a singleton.
          * Use getInstance() to get the one and only instance of this class.
          */
        SwitchManager();

        //
        // Disable copy constructor and assignment operator
        //

        SwitchManager( const SwitchManager& theSwitchManager );
        SwitchManager& operator=( const SwitchManager& theSwitchManager );

		//
		// The one and only instance of this class.
		//

		static SwitchManager* m_theClassInstance;
		
		//
        // Protect singleton creation
        //

		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        //
        // The GenericAgent object within the SwitchManagerAgent class maintains
        // the objects contained in these vectors as Entity objects.  
        // It will clean them up on shutdown. 
        // All objects are referenced by entity key.
        //

	    VideoOutputGroupMap		m_videoOutputGroups;
	    VideoOutputMap			m_videoOutputs;
	    VideoInputMap			m_videoInputs;
        VideoMonitorMap			m_videoMonitors;
        RecordingUnitMap		m_recordingUnits;
        CameraMap				m_cameras;
        QuadMap					m_quads;
        BVSStageMap				m_bvsStages;
        SequenceMap				m_sequences;
		IPDAlarmInterface*	m_switchAlarmInterface;
        IPDStageModeInterface*      m_stageModeInterface;  //++tianxiang Merge ECP++
    };

} // TA_IRS_App

#endif // SWITCH_MANAGER_H
