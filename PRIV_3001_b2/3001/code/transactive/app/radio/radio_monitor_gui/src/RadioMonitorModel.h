/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorModel.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Contains business side of RadioMonitorDlg class
  *
  */

#pragma once

#include <set>
#include <map>

namespace TA_IRS_App
{
    class RadioMonitorModel
    {
    public:
        typedef std::vector<std::string> TSISequence;

        RadioMonitorModel();

        /**
         * requestOneShotMonitoring
         *
         * Forwards a 'one-shot' monitoring request to the radio agent
         *
         * @param tsiList a set of subscriber UIDs
         *
         * @return set of strings defining the strings whose requests succeeded when
         *      forwarded to the agent
         *
         */
        std::set<std::string> requestOneShotMonitoring(const TSISequence& tsiList);

        /**
         * requestStartMonitoring
         *
         * Forwards a start monitoring request to the radio agent
         *
         * @param tsiList a set of subscriber UIDs
         *
         * @param enterMonitoringState flag that will define subsequent
         *          return values for isCurrentlyMonitoring
         *
         */
        void requestStartMonitoring(const TSISequence& tsiList, bool enterMonitoringState = false);

        /**
         * requestStopMonitoring
         *
         * Forwards a stop monitoring request to the radio agent
         *
         * @param tsiList a set of subscriber UIDs
         *
         * @param leaveMonitoringState flag that will define subsequent
         *          return values for isCurrentlyMonitoring
         *
         */
        void requestStopMonitoring(const TSISequence& tsiList, bool leaveMonitoringState = false);

        /**
         * requestStartListening
         *
         * Forwards a start listening request to the radio agent
         *
         * @param tsi the identifier of the group/user/train
         *
         */
        void requestStartListening(long callId);

        /**
         * requestStopListening
         *
         * Forwards a stop listening request to the radio agent
         *
         * @param tsi the identifier of the group/user/train
         *
         */
        void requestStopListening(long callId);

        /**
         * requestEndCall
         *
         * Forwards an end call request to the agent
         *
         * @param tsi the identifier of the group/user/train
         *
         */
        void requestEndCall(long callId);

        /**
         * requestJoinCall
         *
         * Forwards a join request to the radio agent
         *      On successful completion of this, launches Radio Manager Application
         *      in order to complete the join call request
         *
         * @param tsi the identifier of the group/user/train
         *
         */
        void requestJoinCall(long callId);

        /**
         * isCurrentlyMonitoring
         *
         * @return true if the agent is currently (actively) monitoring the state
         *          of any calls
         *
         */
        bool isCurrentlyMonitoring() { return m_isCurrentlyMonitoring; }

        /**
         * getMonitoredSet
         *
         * @return set of items we're currently monitoring, as defined by 
         *          successful calls to startMonitoring / stopMonitoring
         */
        std::set<std::string> getMonitoredSet() const { return m_itemsBeingMonitored; }

        /**
         * forceMonitorSetRemoval
         *
         * Forces removal from the current monitored set (as returned by getMonitoredSet)
         *      for the specified items
         *
         * @param tsiList specifies the items to remove (may not be present, handled OK)
         *
         */
        void forceMonitorSetRemoval(const TSISequence& tsiList);

    private:
        
        enum EErrorCode
        {
            START_MONITOR_FAIL,     // start monitoring 
            ONE_SHOT_FAIL,
            STOP_MONITOR_FAIL       // stop monitoring failure
        };

        /**
         * warnUser
         *
         * Warns the user of an error, as defined by the error code
         * @param errorCode defines the error code
         *
         */
        void warnUser(EErrorCode errorCode);

        /**
         * promptForContinue
         *
         * Warns the user of an error, prompting them if they want to continue
         *  trying the current operation
         * @param errorCode defines the error code
         * @return true if the user wants to continue trying
         *
         */
        bool promptForContinue(EErrorCode errorCode);

        /**
         * launchRadioManagerForCallPickup
         *
         * Launches the radio manager with special parameters instructing
         *  it to automatically pick up incoming calls matching input parameter
         *
         * @param callId defines the call the radio manager should listen out for
         *
         */   
        void launchRadioManagerForCallPickup(long callId);
            
        /**
         * getRadioSession
         *
         * @return current radio session object for our application instance
         *
         */
        RadioMonitor::IRadioSession& getRadioSession();

        bool m_isCurrentlyMonitoring;

        // Maintains internal list of items that have successfully been monitored
        std::set<std::string> m_itemsBeingMonitored;
    };
} // end namespace TA_IRS_App
