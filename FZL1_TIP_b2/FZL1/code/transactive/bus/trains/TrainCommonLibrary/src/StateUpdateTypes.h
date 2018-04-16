/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(StateUpdateTypes_H)
#define StateUpdateTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/CallTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PecTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

// better for using and returning fixed size arrays
#include <boost/array.hpp>

namespace TA_IRS_Bus
{

    namespace StateUpdateTypes
    {

        /**
         * This notification is used when an agent receives a protocol event for a train
         * id that it believes it does not/should not have duty for
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:28 PM
         */
        struct NonDutyTrainTSI
        {
            CommonTypes::TrainIdType trainId;

            std::string trainTsi;

            unsigned long trainLocation;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Used for state updates.
         *
         * This is a global state and is synchronised across all train agents
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:28 PM
         */
        struct TrainCctvSyncState
        {

            CommonTypes::TrainIdType trainId;

            /**
             * The current state of the cctv
             */
            CctvTypes::ECctvSwitchMode cctvState;

            /**
             * This is the first item, can be a sequence, quad etc.
             */
            boost::array<unsigned char, 4> items;

            /**
             * The train ID this train is in pre conflict with
             */
            CommonTypes::TrainIdType preConflictTrain;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector< TrainCctvSyncState > CctvStateList;


        /**
         * structure used to synchronise the train comms state
         *
         * This is a global state and is synchronised across all train agents
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainCommsSyncState
        {
            CommonTypes::TrainIdType trainId;

            /**
             * The TSI of the first MPU
             */
            std::string primaryMpuTsi;

            /**
             * The TSI of the second MPU
             */
            std::string secondaryMpuTsi;

            /**
             * Whether the primary or secondary MPU is the active one
             */
            ProtocolCommonTypes::ETimsOrigin activeMpu;

            /**
             * The PA library version on the train.
             */
            unsigned short paLibraryVersion;

            /**
             * The version of the predefined message library on the train
             */
            unsigned short ttisLibraryVersion;

            /**
             * The version of the time schedule library on the train
             */
            unsigned short ttisScheduleVersion;

            /**
             * true if the train is sending OA1 tables.
             */
            bool oa1Valid;

            /**
             * true if the train is sending ATC tables.
             */
            bool atcValid;

            /**
             * This is where the train is currently sending OA1/ATC tables. If the
             * communication is not valid, this is the last place they were sent, or 0 if they
             * were never sent.
             */
            unsigned long communicatingLocation;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector< TrainCommsSyncState > CommsStateList;


        /**
         * This is the global state synchronised across all locations
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct FullGlobalState
        {
            CommsStateList commsStates;

            CctvStateList cctvStates;

            RadioGroupTypes::TrainRadioGroupMap radioGroupMap;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * This is the train alarm state used for state updates. Decoupled from corba.
         *
         * This is a local state and is only synchronised between peer agents.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainAlarmSyncState
        {

            CommonTypes::TrainIdType trainId;

            /**
             * This is the alarm ID for the ATC timer alarm if it is raised
             */
            std::string atcAlarmId;

            /**
             * This is the alarm ID for the OA1 timer alarm if it is raised
             */
            std::string oa1AlarmId;

            /**
             * This is the alarm ID for the radio if it is raised
             */
            std::string radioModeAlarmId;

            /**
             * This is the alarm ID for the cctv pre conflict alarm if it is raised
             */
            std::string cctvPreConflictAlarmId;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector<TrainAlarmSyncState> AlarmStateList;


        /**
         * structure used to synchronise the train call state
         *
         * This is a local state and is only synchronised between peer agents.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainCallSyncState
        {

            CommonTypes::TrainIdType trainId;

            /**
             * If there is an incoming call
             */
            bool incomingCall;

            /**
             * If there is an incoming call, the time it came in.
             */
            time_t incomingCallTime;

            /**
             * the session id of the operator calling this train, "" if none
             */
            std::string sessionId;

            /**
             * The console ID so the call ID can be uniquely identified in the system
             */
            unsigned long consoleId;

            /**
             * the call in progress (0 if none)
             */
            unsigned long callId;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * If a call is in progress (established) and its type
             */
            CallTypes::ETrainCallType callType;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector<TrainCallSyncState> CallStateList;


        /**
         * structure used to synchronise the train pa state
         *
         * This is a local state and is only synchronised between peer agents.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainPaSyncState
        {

            CommonTypes::TrainIdType trainId;

            /**
             * The console ID so the call ID can be uniquely identified in the system
             */
            unsigned long consoleId;

            /**
             * The session ID of the operator performing a live broadcast.
             */
            std::string sessionId;

            /**
             * The Radio call ID of the live announcement
             */
            unsigned long liveAnnouncementCallId;

            /**
             * The ID of the ongoing live announcement on the train.
             */
            PaTypes::AnnouncementId liveAnnouncementId;

            /**
             * The static group to be used for the audio call if any
             */
            std::string liveAnnouncementStaticGroup;

            /**
             * This is the number of times a PA live announcement has been auto acknowledged.
             */
            unsigned char autoContinueAckCount;

            /**
             * This is true if auto continues have all been sent, and the operator's continue
             * is required.
             */
            bool continueRequired;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector<TrainPaSyncState> PaStateList;


        /**
         * structure used to synchronise the train pec state
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainPecSyncState
        {

            CommonTypes::TrainIdType trainId;

            PecTypes::ActivePecPointList activePecPoints;

            /**
             * The console ID so the call ID can be uniquely identified in the system
             */
            unsigned long consoleId;

            /**
             * The session ID of the operator who answered the PEC
             */
            std::string sessionId;

            /**
             * The Radio call ID of the pec call
             */
            unsigned long callId;

            /**
             * This is the number of times a pec has been auto acknowledged.
             */
            unsigned char autoContinueCounter;

            /**
             * This is true if auto continues have all been sent, and the operator's continue
             * is required.
             */
            bool continueRequired;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        typedef std::vector<TrainPecSyncState> PecStateList;


        /**
         * structure used to synchronise the train tis state
         *
         * This is a local state and is only synchronised between peer agents.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:29 PM
         */
        struct TrainTisSyncState
        {

            CommonTypes::TrainIdType trainId;

            /**
             * This is the type of library being downloaded to the train currently
             */
            TtisTypes::ETtisDownloadType downloadType;

            /**
             * This is the version currently being downloaded to the train.
             */
            unsigned short currentDownloadVersion;

            /**
             * This is the list of frames being downloaded to the train currently. Used for
             * repetition requests.
             * The frames are stored in order, so frame 1 at position 0 and so on.
             */
            TtisTypes::TtisFrameList currentDownload;

            /**
             * The session ID of the operator who initiated the download
             */
            std::string sessionId;

            /**
             * If there is a circuit mode transfer ongoing, the TSI it is to
             */
            std::string transferTsi;

            /**
             * The last update time of the state
             */
            time_t timestamp;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:30 PM
         */
        typedef std::vector<TrainTisSyncState> TisStateList;


        /**
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:30 PM
         */
        struct RadioCallDetails
        {

            /**
             * The console Id
             */
            unsigned long consoleId;

            /**
             * The call Id
             */
            unsigned long callId;

            /**
             * The session Id
             */
            std::string sessionId;

            /**
             * The called Tsi
             */
            std::string calledTsi;

            /**
             * The resource type
             */
            CommonTypes::ETrainResource resourceType;


            /**
             * Logs the call details
             *
             * @return a loggable string
             */
            std::string getLogString() const;


            /**
             * comparison functions to allow the use in sorted containers
             */
            bool operator<( const RadioCallDetails& rhs ) const;
            bool operator==( const RadioCallDetails& rhs ) const;

        };


        /**
          * The list of radio call details
          */
        typedef std::set<RadioCallDetails> RadioCallDetailsSet;


        /**
         * Train Selection name => trains within the selection
         */
        typedef std::map<std::string, CommonTypes::TrainIdList> TrainSelectionMap;


        /**
         * This is the local state synchronised between peer agents only
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:30 PM
         */
        struct FullLocalState
        {
            /**
             * The agents operation mode
             */
            TrainInformationTypes::AgentOperationMode operationMode;

            /**
             * The ongoing radio calls within this agent
             */
            RadioCallDetailsSet ongoingCalls;

            /**
             * A list of saved selection lists
             */
            TrainSelectionMap selectionLists;

            AlarmStateList alarmStates;

            CallStateList callStates;

            PaStateList paStates;

            PecStateList pecStates;

            TisStateList tisStates;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string cctvStateListToString( const CctvStateList& cctvStateList );
        std::string commsStateListToString( const CommsStateList& commsStateList );
        std::string alarmStateListToString( const AlarmStateList& alarmStateList );
        std::string callStateListToString( const CallStateList& callStateList );
        std::string paStateListToString( const PaStateList& paStateList );
        std::string pecStateListToString( const PecStateList& pecStateList );
        std::string tisStateListToString( const TisStateList& tisStateList );
        std::string radioCallDetailsSetToString( const RadioCallDetailsSet& radioCallDetailsSet );
        std::string trainSelectionMapToString( const TrainSelectionMap& trainSelectionMap );

    }

}

#endif // !defined(StateUpdateTypes_H)
