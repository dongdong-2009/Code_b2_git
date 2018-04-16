/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * in any form.
  *
  * Source:   $Source$
  * @author:
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2008/11/28 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(TrainSelector_H)
#define TrainSelector_H

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainRadioGroupUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainRadioGroupCorbaProxy.h"
#include "bus/trains/TrainSelectionComponent/src/ITrainSelectorGUI.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 19-Jan-2004 09:20:38 AM
     * @version 1.0
     */
    class TrainSelector : public ITrainRadioGroupUpdateObserver,
                          public ITrainInformationUpdateObserver
    {

    public:

        /**
         * Constructor
         *
         * @param callback    the GUI that implements the ITrainSelectorGUI callback interface.
         * @param locationKey    the location at which the GUI is being run.
         * @param stationMode    the mode in which the GUI starts. This can change at run time using setStationMode().
         */
        TrainSelector( ITrainSelectorGUI& callback,
                       unsigned long locationKey,
                       LocationCache::EStationType stationMode);


        /**
         * Destructor
         */
        ~TrainSelector();


        /**
         * Determine which trains should be shown (station, main line or depot)
         *
         * @param stationMode    the mode the GUI is in. For example Depot view.
         * @param locationKey    the key of the station the GUI is displaying, if stationMode is Station.
         * (Otherwise this parameter is ignored)
         */
        void setStationMode( LocationCache::EStationType stationMode, unsigned long locationKey );


        /**
         * Gets the trains involved in a single named train selection list.
         *
         * @return a list of train IDs that comprise the selection list
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName    the name of the train selection list.
         */
        CommonTypes::TrainIdList getTrainSelection( const std::string& selectionName );


        /**
         * Deletes an existing selection list. If none existed this has no effect.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    the name of the train selection list.
         * @param sessionId    the session ID of the operator
         */
        void deleteTrainSelection( const std::string& selectionName, const std::string& sessionId );


        /**
         * Save a selection of trains into a named selection list.
         * Creates a new list if there isnt already one by this name, otherwise overwrites any previously saved list with this name.
         *
         * @exception AgentCommunicationException if there was a problem contacting the agent
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    the name of a new or existing selection list.
         * @param trainIDs    list of train ids to be saved as the selection.
         * @param sessionId    the session ID of the operator
         */
        void saveTrainSelection( const std::string& selectionName,
                                 const CommonTypes::TrainIdList& trainIDs,
                                 const std::string& sessionId );


        /**
         * Get details about all known valid trains.
         *
         * This will get the current details about all known trains, that are valid - meaning they are communicating either by radio, or they are present in ATS.
         *
         * @return a list of all valid trains
         */
        TrainInformationTypes::TrainDetailsMap getTrainList();


        /**
         * Given a train ID, get the current details for it.
         * The train might not be valid or at a valid location, but the current known details will be given.
         *
         * @return the details of the given train
         *
         * @exception InvalidTrainException if the train ID given is not known
         *
         * @param trainId    The train to get the details for
         */
        TrainInformationTypes::TrainDetails getTrainDetails( const CommonTypes::TrainIdType& trainId );


        /**
         * Given a train TSI (can be primary or secondary), get the current details for it.
         * The train might not be valid or at a valid location, but the current known details will be given.
         *
         * @return the details of the given train
         *
         * @exception InvalidTrainException if the TSI does not belong to a known train
         *
         * @param tsi    Either the primary or secondary TSI of the train
         */
        TrainInformationTypes::TrainDetails getTrainDetails( const std::string& tsi );


        /**
         * Checks if the given TSI is from a train. This includes primary and non primary TSI.
         *
         * @return true if this TSI belongs to a train
         *
         * @param tsi    The TSI to check
         */
        bool isTrain( const std::string& tsi );


        /**
         * Given a group name, get the radio group details
         *
         * @return the group details
         *
         * @exception InvalidParameterException if the given group name is not valid
         */
        RadioGroupTypes::TrainRadioGroup getRadioGroupForName( const std::string& name );


        /**
         * Given a group tsi, get the radio group details
         *
         * @return the group details
         *
         * @exception InvalidParameterException if the given group tsi is not valid
         */
        RadioGroupTypes::TrainRadioGroup getRadioGroupForTsi( const std::string& tsi );


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TrainInformationTypes::AgentOperationMode& event );//limin, add reference


        /**
         * This is called by the GUI whenever train selection changes.
         * It causes the radio group availability to be updated.
         *
         * @param trainsAreSelected    Whether any trains are selected
         */
        void trainSelectionChanged( bool trainsAreSelected );


        /**
         * This will receive a TrainDetails message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainDetails( const TrainInformationTypes::TrainDetails& event );//limin, add reference


        /**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event );//limin, add reference


        /**
         * This will receive a TrainRadioGroup message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainRadioGroup( const RadioGroupTypes::TrainRadioGroup& event );

    private:

        TrainSelector();


        /**
         *
         * @param theTrainSelector
         */
        TrainSelector( const TrainSelector& theTrainSelector );


        /**
         *
         * @param theTrainSelector
         */
        TrainSelector operator=( const TrainSelector& theTrainSelector );


        /**
         * Gets the given StationType as a string.
         *
         * @param stationMode
         */
        std::string getStationTypeAsString( LocationCache::EStationType stationMode );


        /**
         * Initializes the train agent proxies the train selection component uses to communicate with the train agent.
         */
        void initialiseTrainAgentProxies();


        /**
         * Check the control of the respective train agents and  cache the information into LocationCache
         */
        void updateControlOfLocation();


        /**
         * Retrieves the train details list from the train agent and populates the internal cache.
         */
        void populateTrainDetailsList();


        /**
         * Retrieves the train selection lists from the train agent and populates the cache this object maintains.
         */
        void populateTrainSelectionLists();


        /**
         * Retrieves the train radio groups list from the train agent and populates the internal cache.
         */
        void populateTrainRadioGroups();


        /**
         * Tells the GUI to populate its train selection lists and add all visible trains.
         */
        void initialiseGUI();


        /**
         * Tells the GUI to add all trains to its inbound and outbound train lists.
         */
        void initialiseGUITrainLists();


        /**
         * Inform the GUI of the action to take, given the previous and new visibility of the train.
         * The action will be to add/delete/update the train to/from/in its inbound or outbound list.
         *
         * @param wasVisible    Whether the train was previously visible
         * @param isNowVisible    Whether the train is now visible
         * @param train    The train details
         */
        void updateGUITrainVisibility( bool wasVisible, bool isNowVisible, const TrainInformationTypes::TrainDetails& train );


        /**
         * Tells the GUI to update its radio group names with the cached copy this object maintains.
         */
        void updateGUITrainRadioGroups();


        /**
         * Tells the GUI to update its train selection lists with the cached copy this object maintains.
         */
        void updateGUITrainSelectionLists();


        /**
         * Indicates if a train is visible in the GUI.
         * Based on its ATS validity, its location and the mode and location the GUI is viewing.
         *
         * @param trainDetails    The details of the train to test visibility
         */
        bool isTrainVisible( const TrainInformationTypes::TrainDetails& trainDetails );


        /**
         * Used to get train information and train information updates from the local train agent
         */
        TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;


        /**
         * Used to get train radio groups and train radio group updates from the local train agent
         */
        TrainRadioGroupCorbaProxyPtr m_trainRadioGroupCorbaProxy;


        /**
         * The GUI that implements the ITrainSelectorGUI callback interface.
         */
        ITrainSelectorGUI& m_guiCallback;


        /**
         * Contains station if mode is Station
         */
        unsigned long m_locationKey;


        /**
         * The location key that refers to the depot (this is retrieved from the database)
         */
        unsigned long m_depotLocationKey;


        /**
         * Indicates which mode the GUI is in: InService, Depot or Station mode. See the definition of the enum for more information.
         */
        LocationCache::EStationType m_stationMode;


        /**
         * The list of train selection group names
         */
        TrainInformationTypes::TrainSelectionList m_trainSelectionList;


        /**
         * The map of train ID to train details
         */
        TrainInformationTypes::TrainDetailsMap m_trainsMap;


        /**
         * Keeps the TrainRadioGroups. Gets updated through processTrainRadioGroups. The unlocked RadioGroups from the map populate the ComboBox
         */
        RadioGroupTypes::TrainRadioGroupMap m_trainRadioGroups;

    };

}

#endif // !defined(TrainSelector_H)
