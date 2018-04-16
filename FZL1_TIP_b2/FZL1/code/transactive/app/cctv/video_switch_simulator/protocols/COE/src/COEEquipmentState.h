#ifndef COEEQUIPMENTSTATE_H
#define COEEQUIPMENTSTATE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Singleton to store state of COE Hardware.
  *
  */

#include <map>
#include <string>
#include <vector>
#include <bitset>

namespace TA_Base_Core
{
    class ReEntrantThreadLockable;
}


namespace TA_IRS_App
{
    class COESimulator;
}


namespace TA_IRS_App
{
	typedef struct
	{
		unsigned char stationId;
		unsigned char eventId;
	} TriggeringEvent;

	typedef std::vector< TriggeringEvent > TriggeringEventList;

    
    class COEEquipmentState
    {
    public:

        typedef enum
        {
            NOTHING  = 0,
            CAMERA   = 1,
            SEQUENCE = 2,
            QUAD     = 3,
            TRAIN    = 4,
            ALARM    = 5,
			PARKED   = 6
        } EMonitorAssignmentType;

        typedef enum
        {
            SYS_SUM   = 0,
            ANC_SUM   = 1,
            CAM_SUM   = 2,
            TRANS_SUM = 3,
			DVR_SUM = 4
        } ESummaryAlarmType;
        
        typedef struct
        {
			unsigned char monitorId;
            EMonitorAssignmentType assignmentType;
            unsigned char assignedDeviceID; // Only relevant for CAMERA || SEQUENCE || QUAD || TRAIN_BVS_STAGE.
            unsigned char assignedDeviceLocationKey; // Only relevant for CAMERA.
			TriggeringEventList alarmStack;
        } MonitorState;

		typedef struct
        {
            unsigned short dwellTime;
            std::vector< unsigned char > orderedCameras;
        } SequenceState;

        typedef enum
        {
            NO_RECORDING         = 1,
            REAL_TIME_RECORDING  = 2,
            TIME_LAPSE_RECORDING = 3
        } ERecordingUnitState;

        typedef std::vector< unsigned char > QuadState;

		virtual ~COEEquipmentState() {};

   
        /** 
          * getInstance
          *
          * Return reference to this singleton.
          *
          * @return reference to this singleton.
          */
        static COEEquipmentState& getInstance();


       /**
         * removeInstance
         *
         * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         */
        static void removeInstance();


        /** 
          * registerSimulator
          *
          * description
          *
          * @param 
          */
        void registerSimulator( const std::string& location, COESimulator& sim );


        /** 
          * deRegisterSimulator
          *
          * description
          *
          * @param 
          */
        void deRegisterSimulator( const std::string& location, COESimulator& sim );


        /** 
          * getSummaryAlarm
          *
          * description
          *
          * @param 
          *
          * @return 
          */
        bool getSummaryAlarm( ESummaryAlarmType type );


        /** 
          * setSummaryAlarm
          *
          * description
          *
          * @param 
          */
        void setSummaryAlarm( ESummaryAlarmType type, bool alarmVal );


        /** 
          * getMonitorState
          *
          * description
          *
          * @param location
          * @param 
          *
          * @return 
          */
        MonitorState& getMonitorState( const std::string& location, unsigned char monitorID );


        /** 
          * setMonitorState
          *
          * description
          *
          * @param location
          * @param 
          */
        void setMonitorState( const std::string& location, unsigned char monitorID, const MonitorState& state, bool sendUpdate );


        /** 
          * getSequenceState
          *
          * description
          *
          * @param location
          * @param 
          *
          * @return 
          */
        SequenceState getSequenceState( const std::string& location, unsigned char sequenceID );


        /** 
          * setSequenceState
          *
          * description
          *
          * @param location
          * @param 
          */
        void setSequenceState( const std::string& location, unsigned char sequenceID, const SequenceState& state );


        /** 
          * getQuadState
          *
          * description
          *
          * @param location
          * @param 
          *
          * @return 
          */
        QuadState getQuadState( const std::string& location, unsigned char quadID );


        /** 
          * setQuadState
          *
          * description
          *
          * @param location
          * @param 
          */
        void setQuadState( const std::string& location, unsigned char quadID, const QuadState& state );


        /** 
          * getRecordingUnitState
          *
          * description
          *
          * @param location
          * @param 
          *
          * @return 
          */
        ERecordingUnitState getRecordingUnitState( const std::string& location, unsigned char recordingUnitID );


        /** 
          * setMonitorState
          *
          * description
          *
          * @param location
          * @param 
          */
        void setRecordingUnitState( const std::string& location, unsigned char recordingUnitID, ERecordingUnitState state );


        /** 
          * getTriggeringEvents
          *
          * description
          *
          * @param location
          * @param 
          */
        TriggeringEventList& getTriggeringEvents( const std::string& location);

		/** 
          * openTriggeringEvent
          *
          * description
          *
          * @param location
          * @param 
          */
        void openTriggeringEvent( const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId);

		/** 
          * closeTriggeringEvent
          *
          * description
          *
          * @param location
          * @param 
          */
        void closeTriggeringEvent( const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId = 0 );

		/** 
          * listTriggeringEvents
          *
          * description
          *
          * @param location
          * @param 
          */
        void listOpenTriggeringEvents( const std::string& location );


		/** 
          * ackTriggeringEvent
          *
          * description
          *
          * @param location
          * @param 
          */
        void ackTriggeringEvent(const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId);
		
		/** 
          * acquireCommandLineOutLock
          *
          * description
          *
          */
        void acquireCommandLineOutLock();


        /** 
          * releaseCommandLineOutLock
          *
          * description
          *
          */
        void releaseCommandLineOutLock();


        //
        // Helper struct for safe acquisition of Command Line Out Lock by client code.
        //
        struct CommandLineOutGuard
        {
            CommandLineOutGuard( COEEquipmentState& coeEquipState )
                : m_coeEquipState( coeEquipState )
            {
                m_coeEquipState.acquireCommandLineOutLock();
            }

            ~CommandLineOutGuard()
            {
                m_coeEquipState.releaseCommandLineOutLock();
            }

            COEEquipmentState& m_coeEquipState;
        };


    private:
        /** 
          * Default Constructor
          *
          * Constructor is private so can control number of instances
          */
        COEEquipmentState();


        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        COEEquipmentState( const COEEquipmentState& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        COEEquipmentState& operator=( const COEEquipmentState& );

    private:
        
        typedef std::multimap< std::string, COESimulator* > LocationToSimulator;
        typedef std::map< unsigned char, MonitorState > MonitorIDToState;
        typedef std::map< unsigned char, SequenceState > SequenceIDToState;
        typedef std::map< unsigned char, QuadState > QuadIDToState;
        typedef std::map< unsigned char, ERecordingUnitState > RecordingUnitIDToState;
        typedef std::map< std::string, MonitorIDToState > LocationAndMonitorIDToState;
        typedef std::map< std::string, SequenceIDToState > LocationAndSequenceIDToState;
        typedef std::map< std::string, QuadIDToState > LocationAndQuadIDToState;
        typedef std::map< std::string, RecordingUnitIDToState > LocationAndRecordingUnitIDToState;
		typedef std::map< std::string, TriggeringEventList > LocationToTriggeringEventList;
		typedef std::bitset<5> SummaryAlarms;
        
        LocationToSimulator m_locationToSimulator;
        LocationAndMonitorIDToState m_locationAndMonitorIDToState;
        LocationAndSequenceIDToState m_locationAndSequenceIDToState;
        LocationAndQuadIDToState m_locationAndQuadIDToState;
        LocationAndRecordingUnitIDToState m_locationAndRecordingUnitIDToState;
        LocationToTriggeringEventList m_locationToTriggeringEventList;
		SummaryAlarms m_summaryAlarms;

        static COEEquipmentState* m_theInstance; // pointer to the single instance of this class.
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock; // Semaphore to thread guard this singleton.
        static TA_Base_Core::ReEntrantThreadLockable m_commandLineOutLock; // Semaphore to guard against multiple processes writing to std::cout at same time.

    };
} // namespace TA_IRS_App

#endif // COEEQUIPMENTSTATE_H
