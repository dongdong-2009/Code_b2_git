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
#if !defined(TrainAlarmState_H)
#define TrainAlarmState_H

#include "bus/trains/TrainTransactionModel/src/TrainState.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

namespace TA_IRS_App
{

    /**
     * Holds the alarm IDs (and associated communication timers) for train alarms.
     *
     * This is a local state and is only synchronised between peer agents.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:23 PM
     */
    class TrainAlarmState : public TA_IRS_Bus::TrainState
    {

    public:

        /**
         * Creates the object with a train ID
         *
         * @param trainId
         */
        TrainAlarmState( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Virtual destructor
         */
        virtual ~TrainAlarmState();


        /**
         * Returns the train resource this state represents.
         *
         * @return the resource category this state represents.
         */
        virtual TA_IRS_Bus::CommonTypes::ETrainResource getResourceCategory() const;


        /**
         * Gets the Alarm ID of the OA1 alarm (if raised)
         *
         * @return the alarm ID, or "" if not raised.
         */
        const std::string& getOa1AlarmId() const;


        /**
         * Gets the Alarm ID of the ATC alarm (if raised)
         *
         * @return the alarm ID, or "" if not raised.
         */
        const std::string& getAtcAlarmId() const;


        /**
         * Gets the Alarm ID of the cctv pre conflict alarm (if raised)
         *
         * @return the alarm ID, or "" if not raised.
         */
        const std::string& getCctvPreConflictAlarmId() const;


        /**
         * Gets the Alarm ID of the radio mode alarm (if raised)
         *
         * @return the alarm ID, or "" if not raised.
         */
        const std::string& getRadioModeAlarmId() const;


        /**
         * Sets the Alarm ID of the OA1 alarm (if raised)
         *
         * @param alarmId    The alarm ID to set ("" to clear)
         */
        void setOa1AlarmId( const std::string& alarmId = "" );


        /**
         * Sets the Alarm ID of the ATC alarm (if raised)
         *
         * @param alarmId    The alarm ID to set ("" to clear)
         */
        void setAtcAlarmId( const std::string& alarmId = "" );


        /**
         * Sets the Alarm ID of the cctv alarm (if raised)
         *
         * @param alarmId    The alarm ID to set ("" to clear)
         */
        void setCctvPreConflictAlarmId( const std::string& alarmId = "" );


        /**
         * Sets the Alarm ID of the radio mode alarm (if raised)
         *
         * @param alarmId    The alarm ID to set ("" to clear)
         */
        void setRadioModeAlarmId( const std::string& alarmId = "" );


        /**
         * Gets the state for synchronisation purposes
         *
         * @return the state needing to be synchronised
         */
        TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState getSyncState() const;


        /**
         * Sets the sync state for synchronisation purposes
         *
         * @param state    The new internal state
         */
        void setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainAlarmSyncState& state );


    private:

        /**
         * Private default constructor
         */
        TrainAlarmState();


        /**
         * This is the alarm ID for the OA1 timer alarm if it is raised
         */
        std::string m_oa1AlarmId;


        /**
         * This is the alarm ID for the ATC timer alarm if it is raised
         */
        std::string m_atcAlarmId;


        /**
         * This is the alarm ID for the radio modealarm if it is raised
         */
        std::string m_radioModeAlarmId;


        /**
         * This is the alarm ID for the cctv pre conflict alarm if it is raised
         */
        std::string m_cctvPreConflictAlarmId;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:24 PM
     */
    typedef boost::shared_ptr< TrainAlarmState > TrainAlarmStatePtr;

} // TA_IRS_App

#endif // !defined(TrainAlarmState_H)
