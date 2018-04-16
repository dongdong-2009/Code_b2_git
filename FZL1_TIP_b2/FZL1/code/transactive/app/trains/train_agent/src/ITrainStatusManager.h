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
#if !defined(ITrainStatusManager_H)
#define ITrainStatusManager_H

#include "app/trains/train_agent/src/TrainTisState.h"
#include "app/trains/train_agent/src/TrainPecState.h"
#include "app/trains/train_agent/src/TrainCommsState.h"
#include "app/trains/train_agent/src/TrainCallState.h"
#include "app/trains/train_agent/src/TrainCctvState.h"
#include "app/trains/train_agent/src/TrainPaState.h"
#include "app/trains/train_agent/src/TrainAlarmState.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/PaTypes.h"
#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"

#include "bus/trains/TrainTransactionModel/src/TrainStatusManager.h"

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:53 PM
     */
    class ITrainStatusManager
    {

    public:

        /**
         * Register for updates to TrainState objects.
         *
         * @param resourceType
         * @param listener
         */
        virtual void registerForStateChanges( TA_IRS_Bus::CommonTypes::ETrainResource resourceType,
                                              TA_IRS_Bus::ITrainStateUpdateListener& listener ) = 0;


        /**
         * Cancel registration for state changes.
         *
         * @param resourceType
         * @param listener
         */
        virtual void deregisterForStateChanges( TA_IRS_Bus::CommonTypes::ETrainResource resourceType,
                                                TA_IRS_Bus::ITrainStateUpdateListener& listener ) = 0;


        /**
         * Gets all train states for a resource category - used to search all known states for something.
         *
         * @param resource
         */
        virtual TA_IRS_Bus::TrainIdToStateMap getAllTrainStates( TA_IRS_Bus::CommonTypes::ETrainResource resource ) const = 0;


        /**
         * Get the comms state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainCommsStatePtr getTrainCommsState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the call state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainCctvStatePtr getTrainCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the Alarm state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainAlarmStatePtr getTrainAlarmState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the call state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainCallStatePtr getTrainCallState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the pa state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainPaStatePtr getTrainPaState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the PEC state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainPecStatePtr getTrainPecState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Get the TTIS state for a train with the given train ID
         *
         * @exception InvalidTrainException if the train is not in the map (or the requested resource is not in the map)
         *
         * @param trainId    the train to get the state for
         */
        virtual TrainTisStatePtr getTrainTisState( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) const = 0;


        /**
         * Gets the list of all possible train selections.
         *
         * @return all the selection list names
         */
        virtual std::vector<std::string> getTrainSelectionNames() const = 0;


        /**
         * Gets the trains to select for a given train seletion list
         *
         * @exception InvalidSelectionListException if the train selection name is unknown.
         *
         * @param selectionName
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainSelection( const std::string& selectionName ) const = 0;


        /**
         * Saves a named train selection.
         *
         * @return true if the selection was not previously existing and was added, false if it was pre-existing and was updated.
         *
         * @param selectionName    The selection name (will overwrite existing selections of the same name)
         * @param trainList    The list of train id's
         */
        virtual void saveTrainSelection( const std::string& selectionName,
                                         const TA_IRS_Bus::CommonTypes::TrainIdList& trainList ) = 0;


        /**
         * Deletes a named train selection.
         *
         * @exception InvalidSelectionListException if the train selection name is unknown.
         *
         * @param selectionName    The selection name
         */
        virtual void deleteTrainSelection( const std::string& selectionName ) = 0;


        /**
         * Given a TSI, find the train ID it belongs to.
         *
         * @return the train id, or 0 if it doesnt match
         *
         * @param tsi
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdType getTrainIdFromTsi( const std::string& tsi ) const = 0;


        /**
         * A Radio call has just been made, add the details to the list of calls this agent is involved in.
         *
         * @param callDetails    The details to add to the internal list
         */
        virtual void addRadioCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails ) = 0;


        /**
         * A Radio call has ended, or is about to be ended.
         * Remove the details from the list of calls this agent is involved in.
         *
         * When a call has ended externally, this is called as a result, when a call is to be ended by this agent, this must be called just before.
         *
         * @param consoleId    The console ID the call is for
         * @param callId    The call ID
         */
        virtual void removeRadioCall( unsigned long consoleId,
                                      unsigned long callId ) = 0;


        /**
         * Find this call in the list of ongoing calls, if it exists.
         *
         * @return the details of this call
         *
         * @exception TrainRadioException if the call is invalid
         *
         * @param consoleId    The console ID the call is for
         * @param callId    The call ID
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails getDetailsForRadioCall( unsigned long consoleId,
                                                                                       unsigned long callId ) const = 0;


        /**
         * Given an ongoing call (and the type of call), find out which trains are involved.
         *
         * @return the list of trains involved in the call
         *
         * @param callDetails    The details of the call (previously obtained from getDetailsForRadioCall)
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainsInvolvedInRadioCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails ) const = 0;


        /**
         * Given an announcement ID, find out which trains are involved.
         *
         * @return the list of trains involved in the announcement
         *
         * @param announcementId    The announcement ID to match
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainsInvolvedInLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const = 0;


        /**
         * Given an announcement ID, find out which trains are involved and require a continue
         *
         * @return the list of trains involved in the announcement requriring a continue
         *
         * @param announcementId    The announcement ID to match
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainsInvolvedInLiveAnnouncementRequiringContinue( TA_IRS_Bus::PaTypes::AnnouncementId announcementId ) const = 0;


        /**
         * Find the call id for the given live announcement if it exists
         *
         * @return the details of this call
         *
         * @exception TrainRadioException if there is no call
         *
         * @param announcementID    The announcement ID to find the call for
         */
        virtual TA_IRS_Bus::StateUpdateTypes::RadioCallDetails getCallDetailsForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementID ) const = 0;


        /**
         * Find the static group used/to be used for the given live announcement
         *
         * @return the group TSI if set
         *
         * @exception TrainRadioException if there is no call
         *
         * @param announcementID    The announcement ID to find the call for
         */
        virtual std::string getStaticGroupForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementID ) const = 0;

    };

} // TA_IRS_App

#endif // !defined(ITrainStatusManager_H)
