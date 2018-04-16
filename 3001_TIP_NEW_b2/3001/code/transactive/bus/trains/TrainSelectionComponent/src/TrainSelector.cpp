/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:
  * @version: $Revision: #10 $
  *
  * Last modification: $Date: 2018/01/12 $
  * Last modified by:  $Author: huirong.luo $
  *
  */

#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupUtils.h"
#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroupAccessFactory.h"
#include "core/threads/src/Thread.h"

#include <vector>
#include <algorithm>
#include <functional>

#include "core/data_access_interface/src/LocationAccessFactory.h"

namespace TA_IRS_Bus
{
    TrainSelector::TrainSelector( ITrainSelectorGUI& callback,
                                  unsigned long locationKey,
                                  LocationCache::EStationType stationMode )
        : m_guiCallback( callback ),
          m_locationKey( locationKey ),
          m_stationMode( stationMode )
    {
        FUNCTION_ENTRY( "TrainSelector" );

        //Initializes the train agent proxies
        initialiseTrainAgentProxies();

		//Get all Depot locations from the Database
		typedef std::vector<TA_Base_Core::ILocation*> LocationList;
		typedef std::map<unsigned long, std::string> DepotLocationMap;
		DepotLocationMap depotLocations;
        try
        {
            // Load any required information from the database
            // Get the location key of All Depot locations
			LocationList allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

			for ( LocationList::iterator locationIter = allLocations.begin();
				allLocations.end() != locationIter; ++locationIter )
			{
				//For all Depot locations
				if (TA_Base_Core::ILocation::DPT == (*locationIter)->getLocationType() )
				{
					depotLocations.insert(DepotLocationMap::value_type((*locationIter)->getKey(),
						(*locationIter)->getName()));
				}

				//Make sure to clean Location object to avoid memory leak
				delete (*locationIter);
				(*locationIter) = NULL;
			}
			
        }
        catch ( const TA_Base_Core::DatabaseException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", te.what());

            throw;
        }
        catch( const TA_Base_Core::DataException& te)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", te.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                   " in TrainSelector::TrainSelector, cleaning up and throwing on.");

            throw TA_Base_Core::TransactiveException("Unknown error initialising Train Selection Component");
        }


        try
        {
            // Get all initial information from the Train Agent and populate the internal caches.
            updateControlOfLocation();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"populateTrainSelectionLists" );
            populateTrainSelectionLists();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"populateTrainDetailsList" );
            populateTrainDetailsList();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"addObserverTrainInfo" );
			addObserverTrainInfo(); // Subscriber should be called after the TrainLists had been populated already.
            populateTrainRadioGroups();
        }
        catch ( TA_Base_Core::TransactiveException& te )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...",
                   " in TrainSelector::TrainSelector, cleaning up and throwing on.");
        }

        // Tell the GUI to update its train details and train selection list.
        initialiseGUI();

        FUNCTION_EXIT;
    }


    TrainSelector::~TrainSelector()
    {
        FUNCTION_ENTRY( "~TrainSelector" );

        //Clear train lists
        TrainInformationTypes::TrainDetails dummy;
        m_guiCallback.updateInboundList( ITrainSelectorGUI::ClearAll, dummy );
        m_guiCallback.updateOutboundList( ITrainSelectorGUI::ClearAll, dummy );

        //Unsubscribe from updates to the corba proxies
        
		if ( m_trainInformationCorbaProxy )
        {
            m_trainInformationCorbaProxy->removeObserver( this );

			//Just put a little bit delay for graceful exit, suggested by Bihui.
			TA_Base_Core::Thread::sleep(3000);
        }

        if ( m_trainRadioGroupCorbaProxy )
        {
            m_trainRadioGroupCorbaProxy->removeObserver( this );
        }

        FUNCTION_EXIT;
    }


    void TrainSelector::setStationMode(LocationCache::EStationType stationMode, unsigned long locationKey)
    {
        FUNCTION_ENTRY( "setStationMode" );

        m_stationMode = stationMode;
        m_locationKey = locationKey;

        // Clear all trains from the train lists and then re-initialise the GUI.
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Clearing and re-initialising train list in GUI" );

        TrainInformationTypes::TrainDetails dummy;
        m_guiCallback.updateInboundList( ITrainSelectorGUI::ClearAll, dummy );
        m_guiCallback.updateOutboundList( ITrainSelectorGUI::ClearAll, dummy );

        initialiseGUITrainLists();

        FUNCTION_EXIT;
    }


    CommonTypes::TrainIdList TrainSelector::getTrainSelection(const std::string& selectionName)
    {
        FUNCTION_ENTRY( "getTrainSelection" );
        FUNCTION_EXIT;
        return m_trainInformationCorbaProxy->getTrainSelection( selectionName );
    }


    void TrainSelector::deleteTrainSelection( const std::string& selectionName, const std::string& sessionId )
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        m_trainInformationCorbaProxy->deleteTrainSelection( selectionName, sessionId );

        FUNCTION_EXIT;
    }


    void TrainSelector::saveTrainSelection( const std::string& selectionName,
                                            const CommonTypes::TrainIdList& trainIDs,
                                            const std::string& sessionId )
    {
        FUNCTION_ENTRY( "saveTrainSelection" );

        m_trainInformationCorbaProxy->saveTrainSelection( selectionName, trainIDs, sessionId );

        FUNCTION_EXIT;
    }


    TrainInformationTypes::TrainDetailsMap TrainSelector::getTrainList()
    {
        FUNCTION_ENTRY( "getTrainList" );
       
		TA_THREADGUARD( m_trainMapLock);

        return m_trainsMap;

		FUNCTION_EXIT;
    }


    TrainInformationTypes::TrainDetails TrainSelector::getTrainDetails( const CommonTypes::TrainIdType& trainId )
    {
        FUNCTION_ENTRY( "getTrainDetails" );

		TA_THREADGUARD( m_trainMapLock);

        //find the train in the cached map
        TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainsMap.find( trainId );

        if ( findIter == m_trainsMap.end() )
        {
            std::ostringstream msg;
            msg << "Train " << static_cast<unsigned int>( trainId ) << " not found";
            TA_THROW( InvalidTrainException( msg.str().c_str(), trainId ) );
        }

        FUNCTION_EXIT;
        return findIter->second;
    }


    TrainInformationTypes::TrainDetails TrainSelector::getTrainDetails( const std::string& tsi )
    {
        FUNCTION_ENTRY( "getTrainDetails" );

		TA_THREADGUARD( m_trainMapLock);

        //if one matches primary or secondary TSI, return its details
        
		for ( TrainInformationTypes::TrainDetailsMap::iterator iter = m_trainsMap.begin();
              iter != m_trainsMap.end(); ++iter )
        {
            
			if ( ( tsi == iter->second.primaryTsi ) ||
                 ( tsi == iter->second.secondaryTsi ) )
            {
                FUNCTION_EXIT;
                return iter->second;
            }

        }

        //if none match, throw an InvalidTrainException
        TA_THROW (TrainException( "Given TSI is does not belong to a known train" ) );
    }


    bool TrainSelector::isTrain( const std::string& tsi )
    {
        FUNCTION_ENTRY( "isTrain" );

		TA_THREADGUARD( m_trainMapLock);

        //check each item in m_trainsMap
        //if one matches primary or secondary TSI, return true
        
		for ( TrainInformationTypes::TrainDetailsMap::iterator iter = m_trainsMap.begin();
              iter != m_trainsMap.end(); ++iter )
        {
            
			if ( ( tsi == iter->second.primaryTsi ) ||
                 ( tsi == iter->second.secondaryTsi ) )
            {
                FUNCTION_EXIT;
                return true;
            }

        }

        FUNCTION_EXIT;
        return false;
    }


    RadioGroupTypes::TrainRadioGroup TrainSelector::getRadioGroupForName( const std::string& name )
    {
        FUNCTION_ENTRY( "getRadioGroupForName" );
		
		// the map is mapped on tsi

        for ( RadioGroupTypes::TrainRadioGroupMap::iterator iter = m_trainRadioGroups.begin();
              m_trainRadioGroups.end() != iter; ++iter )
        {
            
			if ( name == iter->second.groupName )
            {
                FUNCTION_EXIT;
                return iter->second;
            }

        }

        TA_THROW( InvalidParameterException( "Radio group not found", "Name", name ) );

        FUNCTION_EXIT;
        return RadioGroupTypes::TrainRadioGroup();
    }


    RadioGroupTypes::TrainRadioGroup TrainSelector::getRadioGroupForTsi( const std::string& tsi )
    {
        FUNCTION_ENTRY( "getRadioGroupForTsi" );

        // the map is mapped on tsi
        RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_trainRadioGroups.find( tsi );

        if ( m_trainRadioGroups.end() == findIter )
        {
            TA_THROW( InvalidParameterException( "Radio group not found", "TSI", tsi ) );
        }

        FUNCTION_EXIT;
        return findIter->second;
    }


    void TrainSelector::processAgentOperationMode(const TrainInformationTypes::AgentOperationMode& event )//limin, add reference
    {
        FUNCTION_ENTRY( "processAgentOperationMode" );

        //update the status of radio fallback
        m_guiCallback.updateFallbackStatus( event.fallbackMode );

        //update the location control map
        LocationCache::getInstance().cacheLocationControl( event );

		TA_THREADGUARD( m_trainMapLock);

		//the train list must refresh which items are enabled/disabled
        
		for ( TrainInformationTypes::TrainDetailsMap::iterator iter = m_trainsMap.begin();
              iter != m_trainsMap.end(); ++iter )
        {
            processTrainDetails( iter->second );
        }

        FUNCTION_EXIT;
    }


    void TrainSelector::trainSelectionChanged( bool trainsAreSelected )
    {
        FUNCTION_ENTRY( "trainSelectionChanged" );

        updateGUITrainRadioGroups();

        FUNCTION_EXIT;
    }


    void TrainSelector::processTrainDetails( const TrainInformationTypes::TrainDetails& event )//limin, add reference
    {
        FUNCTION_ENTRY( "processTrainDetails" );

        bool wasVisible = false;
        bool isNowVisible = false;

		TA_THREADGUARD( m_trainMapLock);

        // find the old entry if it exists
        TrainInformationTypes::TrainDetailsMap::iterator findIter = m_trainsMap.find( event.trainId );

        if ( findIter != m_trainsMap.end() )
        {
            
			// check if there are any changes at all
			if ( findIter->second == event )
            {
                FUNCTION_EXIT;
                return;
            }

            // check if it was visible
            wasVisible = isTrainVisible( findIter->second );

            // check if it changed track
            bool changedTrack = findIter->second.track != event.track;

            // if it changed track, remove it from the old track
            
			if ( true == changedTrack )
            {
                updateGUITrainVisibility( wasVisible, false, findIter->second );

                // also if it changed track we must treat it as was not visible
                // because it has now been removed from all lists
                wasVisible = false;
            }

        }

        // update the cache
        m_trainsMap[ event.trainId ] = event;

        // check the new visibility
        isNowVisible = isTrainVisible( event );

        // update the GUI
        updateGUITrainVisibility( wasVisible, isNowVisible, event );

        FUNCTION_EXIT;
    }


    void TrainSelector::processTrainSelectionUpdate( const TrainInformationTypes::TrainSelectionUpdate& event )//limin, add reference
    {
        FUNCTION_ENTRY( "processTrainSelectionUpdate" );

        TrainInformationTypes::TrainSelectionList::iterator findIter = std::find( m_trainSelectionList.begin(),
                                                           m_trainSelectionList.end(),
                                                           event.selectionName );

        switch ( event.change )
        {
			case TrainInformationTypes::SelectionListUpdate:
					{
	                    
						if ( findIter == m_trainSelectionList.end() )
						{
							m_trainSelectionList.push_back( event.selectionName );
						}

					}
					break;
			case TrainInformationTypes::SelectionListDeletion:
					{
	                    
						if ( findIter != m_trainSelectionList.end() )
						{
							m_trainSelectionList.erase( findIter );
						}

					}
					break;
			default:;
			// fall through
        }

        //notify the GUI of the new list
        updateGUITrainSelectionLists();

        FUNCTION_EXIT;
    }


    void TrainSelector::processTrainRadioGroup( const RadioGroupTypes::TrainRadioGroup& event )
    {
        FUNCTION_ENTRY( "processTrainRadioGroup" );

        //update the radio groups
        m_trainRadioGroups[ event.groupTsi ] = event;
        //notify the GUI to update
        updateGUITrainRadioGroups();

        FUNCTION_EXIT;
    }


    std::string TrainSelector::getStationTypeAsString( LocationCache::EStationType stationMode )
    {
        FUNCTION_ENTRY( "getStationTypeAsString" );

        std::string stationModeStr;

        switch(stationMode)
        {
        case LocationCache::InService: stationModeStr = "In Service"; break;
        case LocationCache::Depot:     stationModeStr = "Depot";      break;
        case LocationCache::Station:   stationModeStr = "Station";    break;
        case LocationCache::All:       stationModeStr = "All";        break;
        default:        stationModeStr = "Undefined";  break;
        }

        FUNCTION_EXIT;
        return stationModeStr;
    }


    void TrainSelector::initialiseTrainAgentProxies()
    {
        FUNCTION_ENTRY( "initialiseTrainAgentProxies" );

        //Get the session ID from the run params
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

        //Initialize the proxy factory
        TrainAgentProxyFactory::getInstance().guiInitialise( sessionId );

        //get the train information corba proxy
        m_trainInformationCorbaProxy = TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();
       // m_trainInformationCorbaProxy->addObserver( this );

        if ( true == m_guiCallback.hasRadioGroupSupport() )
        {
            m_trainRadioGroupCorbaProxy = TrainAgentProxyFactory::getInstance().createTrainRadioGroupCorbaProxy();
            //m_trainRadioGroupCorbaProxy->addObserver( this );
        }

        FUNCTION_EXIT;
    }


    void TrainSelector::updateControlOfLocation()
    {
        FUNCTION_ENTRY( "updateControlOfLocation" );
		try
		{
			TrainInformationTypes::AgentOperationMode operationMode = m_trainInformationCorbaProxy->getAgentOperationMode();
			LocationCache::getInstance().cacheLocationControl( operationMode );
			//update the status of fallback in the GUI
			m_guiCallback.updateFallbackStatus( operationMode.fallbackMode );
		}
		catch ( TA_Base_Core::TransactiveException& te )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what());
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "...",
				" in TrainSelector::TrainSelector, cleaning up and throwing on.");
		}

        

        FUNCTION_EXIT;
    }


    void TrainSelector::populateTrainDetailsList()
    {
        FUNCTION_ENTRY( "populateTrainDetailsList" );
		try
		{
			TrainInformationTypes::TrainDetailsList trains = m_trainInformationCorbaProxy->getTrainList();

			TA_THREADGUARD( m_trainMapLock);

			for ( TrainInformationTypes::TrainDetailsList::iterator iter = trains.begin(); iter != trains.end(); ++iter )
			{
				m_trainsMap[ iter->trainId ] = *iter;
			}
		}
		catch ( TA_Base_Core::TransactiveException& te )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what());
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "...",
				" in TrainSelector::TrainSelector, cleaning up and throwing on.");
		}

        FUNCTION_EXIT;
    }


    void TrainSelector::populateTrainSelectionLists()
    {
        FUNCTION_ENTRY( "populateTrainSelectionLists" );     
		try
		{
			m_trainSelectionList = m_trainInformationCorbaProxy->getTrainSelectionList();
		}
		catch ( TA_Base_Core::TransactiveException& te )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what());
		}
		catch ( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "...",
				" in TrainSelector::TrainSelector, cleaning up and throwing on.");
		}

        FUNCTION_EXIT;
    }


    void TrainSelector::populateTrainRadioGroups()
    {
        FUNCTION_ENTRY( "populateTrainRadioGroups" );

     //   if ( m_trainRadioGroupCorbaProxy )
    //    {
        //    m_trainRadioGroups = m_trainRadioGroupCorbaProxy->getTrainRadioGroups( m_locationKey );
      //  }

		this->getRadioGroups();

        FUNCTION_EXIT;
    }


    void TrainSelector::initialiseGUI()
    {
        FUNCTION_ENTRY( "initialiseGUI" );

        // Tell the GUI to add all trains that are visible to its train lists
        initialiseGUITrainLists();

        // Tell the GUI to populate its train selection lists
        updateGUITrainSelectionLists();

        // Tell the GUI to populate its train radio group combo
        updateGUITrainRadioGroups();

        FUNCTION_EXIT;
    }


    void TrainSelector::initialiseGUITrainLists()
    {
        FUNCTION_ENTRY( "initialiseGUITrainLists" );

		TA_THREADGUARD( m_trainMapLock);

        for( TrainInformationTypes::TrainDetailsMap::iterator it = m_trainsMap.begin();
            it != m_trainsMap.end(); ++it )
        {
                TrainInformationTypes::TrainDetails& train = (*it).second;

            bool isNowVisible = isTrainVisible( train );

            updateGUITrainVisibility( false, isNowVisible, train );
        }

        FUNCTION_EXIT;
    }

	void TrainSelector::getRadioGroups()
	{
		FUNCTION_ENTRY( "getRadioGroups" );

	//	TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap radioGroups;
	//	if ( !m_trainRadioGroups.empty() ) return ;
        m_trainRadioGroups.clear() ;  
		try
		{
			// Using TrainRadioGroupAccessFactory, load all TrainRadioGroups
			typedef std::vector< TA_IRS_Core::ITrainRadioGroupPtr > ITrainRadioGroupPtrList;
			ITrainRadioGroupPtrList radioGroupConfiguration = TA_IRS_Core::TrainRadioGroupAccessFactory::loadAllTrainRadioGroups();

			// package them up to be returned

			for ( ITrainRadioGroupPtrList::iterator radioGroupIter = radioGroupConfiguration.begin();
				radioGroupConfiguration.end() != radioGroupIter; ++radioGroupIter )
			{
				TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup newGroup;
				newGroup.groupName = ( *radioGroupIter )->getName();
				newGroup.groupTsi = ( *radioGroupIter )->getTsi();
				newGroup.locationCoverage = ( *radioGroupIter )->getLocationCoverage();
				newGroup.locked = false;
				newGroup.lastUpdateTime = 0;
				newGroup.groupId = ( *radioGroupIter )->getKey () ;

				m_trainRadioGroups.insert( TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::value_type( newGroup.groupTsi, newGroup ) );
			}
		}
		catch ( const TA_Base_Core::TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

		//	TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
		}

		FUNCTION_EXIT;
		
	}


    void TrainSelector::updateGUITrainVisibility( bool wasVisible,
                                                  bool isNowVisible,
                                                  const TrainInformationTypes::TrainDetails& train )
    {
        FUNCTION_ENTRY( "updateGUITrainVisibility" );

        // Update
        if( ( wasVisible ) &&
            ( isNowVisible ) )
        {
            
			if( train.track == TrainInformationTypes::AtsTrackInner )
            {
                m_guiCallback.updateInboundList( ITrainSelectorGUI::Update, train );
            }

            else
            {
                m_guiCallback.updateOutboundList( ITrainSelectorGUI::Update, train );
            }
        }
        // Delete
        
		else if( ( wasVisible ) &&
                 ( !isNowVisible ) )
        {
            
			if( train.track == TrainInformationTypes::AtsTrackInner )
            {
                m_guiCallback.updateInboundList( ITrainSelectorGUI::Delete, train );
            }

            else
            {
                m_guiCallback.updateOutboundList( ITrainSelectorGUI::Delete, train );
            }

        }
        
		// Add
		else if( ( !wasVisible ) &&
                 (isNowVisible ) )
        {
            
			if( train.track == TrainInformationTypes::AtsTrackInner )
            {
                m_guiCallback.updateInboundList( ITrainSelectorGUI::Add, train );
            }
            
			else
            {
                m_guiCallback.updateOutboundList( ITrainSelectorGUI::Add, train );
            }
        }

        FUNCTION_EXIT;
    }


    void TrainSelector::updateGUITrainRadioGroups()
    {
        FUNCTION_ENTRY( "updateGUITrainRadioGroups" );

        if ( m_trainRadioGroupCorbaProxy )
        {
            //if radio groups are supported
            //Takes the list of train radio groups cached by this object
            TrainInformationTypes::TrainDetailsList selectedTrains = m_guiCallback.getSelectedTrainData();

            //filters it based on current train selection and group availability
            RadioGroupTypes::GroupLocationList selectedLocations;
            
			for ( TrainInformationTypes::TrainDetailsList::iterator iterDetails = selectedTrains.begin();
                  iterDetails != selectedTrains.end(); ++iterDetails )
            {
                
				if ( true == iterDetails->atsValid )
                {
                    selectedLocations.insert( iterDetails->currentLocation );
                }

            }

            RadioGroupTypes::TrainRadioGroupMap filteredList = RadioGroupTypes::RadioGroupUtils::getAvaliableGroups( m_trainRadioGroups );
            filteredList = RadioGroupTypes::RadioGroupUtils::filterRadioGroupsByLocation( filteredList, selectedLocations );

            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Updating GUI train radio groups. New list length: %d",
                filteredList.size() );
            //the method updateRadioGroupCombo need vector<string> but hte filteredList is a map ???
            std::vector<std::string> groupNamesList;
			typedef TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup TypeTRG;
			std::vector<TypeTRG> trgVec;
            
			for ( RadioGroupTypes::TrainRadioGroupMap::iterator iterGroup = filteredList.begin();
                  iterGroup != filteredList.end(); ++iterGroup)
            {

                //groupNamesList.push_back( iterGroup->second.groupName );			
				trgVec.push_back(iterGroup->second);
            }
			// disable sorting of the list, show the list as obtained from the DB
			//std::sort(trgVec.begin(), trgVec.end(), std::less<TypeTRG>());
			std::vector<TypeTRG>::const_iterator iter = trgVec.begin();
			
			while (iter != trgVec.end() )
			{
				groupNamesList.push_back( iter->groupName );
				iter++;
			}
			
            m_guiCallback.updateRadioGroupCombo( groupNamesList );
        }

        FUNCTION_EXIT;
    }


    void TrainSelector::updateGUITrainSelectionLists()
    {
        FUNCTION_ENTRY( "updateGUITrainSelectionLists" );

        //Simply passes the list of train selection list names to the GUI
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
            "Updating GUI train selection lists. New list length: %d",
            m_trainSelectionList.size() );

        m_guiCallback.updateSelectionCombo( m_trainSelectionList );

        FUNCTION_EXIT;
    }


    bool TrainSelector::isTrainVisible(const TrainInformationTypes::TrainDetails& trainDetails)
    {
        FUNCTION_ENTRY( "isTrainVisible" );

        bool visibility = false;

        //First, visibility is based on ATS Validity
        //Check if ATS is valid, if it is check the train location
        //the train is visible when:
        //- the GUI is in InService mode and the train is not at the depot
        //- the GUI is in Depot mode and the train is at the Depot
        //- the GUI is in PSC (station) mode and the train is at the station being displayed.
        //- the GUI is in all mode
        
		if ( true == trainDetails.atsValid )
        {
        
			switch ( m_stationMode )
            {
                // GUI is showing all trains in service
            case LocationCache::InService:
					//Check if current location is not a Depot location
					visibility = !isDepotLocation(trainDetails.currentLocation);
                    break;

                // GUI is showing all trains at the depot
            case LocationCache::Depot:
					//Check if current location is a depot location
                    visibility = ( isDepotLocation(trainDetails.currentLocation) ||
                                   (trainDetails.currentLocation == 0) );
                    break;

                // GUI is showing a particular station
            case LocationCache::Station:
                    visibility = ( trainDetails.currentLocation == m_locationKey );
                    break;

                // GUI is showing all trains
            case LocationCache::All:
                    visibility = true;
                    break;
				// add as the parasoft reports that having a switch case that doesnt have a
				// default is a violation for coding standards..
			default:
				break;

            }
        }

        //Second, if visibility is still false
        //check if communication is valid, and get the communicating location
        //if the communicating location is the current location key, visibility is true

        if ( false == visibility )
		{
            
			if ( ( true == trainDetails.communicationValid ) &&
                 ( trainDetails.communicatingLocation == m_locationKey ) )
			{
                visibility = true;
            }

        }

        FUNCTION_EXIT;
        return visibility;
    }

	void TrainSelector::addObserverTrainInfo()
	{
		FUNCTION_ENTRY("registerSubsriber");

		m_trainInformationCorbaProxy->addObserver( this );

		FUNCTION_EXIT;
	}

	bool TrainSelector::isDepotLocation(unsigned long locationKey)
	{
		FUNCTION_ENTRY("isDepotLocationExist");

		bool isLocationExist = false;

		//Check if locationKey exist in the depot location cache
		std::map<unsigned long, std::string>::iterator iterDepot = m_depotLocationKey.find(locationKey);

		if ( iterDepot != m_depotLocationKey.end() )
		{
			//LocationKey exist in the Depot location cache
			isLocationExist = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Location key : %d, Depot Location Name : %s is exist in Depot Cache.",
				iterDepot->first, iterDepot->second.c_str());

		}

		FUNCTION_EXIT;
		return isLocationExist;

	}


	void TrainSelector::recoveryTrainList()
	{
		FUNCTION_ENTRY( "recoveryTrainList" );

		TrainInformationTypes::TrainDetailsList trains = m_trainInformationCorbaProxy->getTrainList();

		updateTrainList(trains);
		deleteTrainList(trains);

		FUNCTION_EXIT;
	}

	void TrainSelector:: updateTrainList(TrainInformationTypes::TrainDetailsList& trains)	
	{
		FUNCTION_ENTRY( "updateTrainList" );

        TrainInformationTypes::TrainDetailsList::iterator iter = trains.begin();
		for ( ; iter != trains.end(); ++iter )
		{
			processTrainDetails(*iter);	
		}
		FUNCTION_EXIT;
	}

	void TrainSelector:: deleteTrainList(TrainInformationTypes::TrainDetailsList& trains)	
	{
		FUNCTION_ENTRY( "deleteTrainList" );

		bool wasVisible;

		TA_THREADGUARD( m_trainMapLock);

        TrainInformationTypes::TrainDetailsMap::iterator iterTrainsMap = m_trainsMap.begin();
		for(; iterTrainsMap != m_trainsMap.end(); )
		{
			bool bIsToDelete = true;

			TrainInformationTypes::TrainDetailsList::iterator iter = trains.begin();

			for ( ; iter != trains.end(); ++iter )
			{
				//Check if this train ID can be found from agent side train list
				if(iterTrainsMap->first == iter->trainId)
				{
					bIsToDelete = false;
					break;
				}
			}
			//if this train id can not be found at agent side train list, then remove from the cache and update UI
			if(bIsToDelete)
			{
				wasVisible = isTrainVisible( iterTrainsMap->second );

				//update UI
				updateGUITrainVisibility( wasVisible, false, iterTrainsMap->second );

				TrainInformationTypes::TrainDetailsMap::iterator iterTemp = iterTrainsMap;

				++iterTrainsMap;

				// remove from cache
				m_trainsMap.erase(iterTemp);	
			}
			else
			{
				++iterTrainsMap;
			}
		}

		FUNCTION_EXIT;
	}

}
