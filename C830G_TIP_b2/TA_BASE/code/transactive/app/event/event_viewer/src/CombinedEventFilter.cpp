/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/event/event_viewer/src/CombinedEventFilter.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/06/25 12:06:02 $
  * Last modified by:  $Author: builder $
  *
  * CombinedEventFilter is an implementation of the abstract class Filter, from the
  * EventListComponent. It specifies the details of events from both the EVENT table
  * and the LO_DATAPT_STATE_UPDATE table, and includes the column names, and
  * filterable data items, in constant static variables.
  *
  */

// Disable "identifier was trucated to '255' characters" warnings.
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "stdafx.h"

#include <algorithm>
#include <functional>
#include <map>
#include <utility>

#include "app/event/event_viewer/src/ActionCreateIncidentReport.h"
#include "app/event/event_viewer/src/ActionShowComment.h"
#include "app/event/event_viewer/src/AvalancheNamedFilter.h"
#include "app/event/event_viewer/src/CombinedEventFilter.h"
#include "app/event/event_viewer/src/CombinedEventFilterDlg.h"
#include "app/event/event_viewer/src/CombinedEventItem.h"
#include "app/event/event_viewer/src/DatabaseAccess.h"
#include "app/event/event_viewer/src/CreateDefaultFilterThread.h" //zhiqiang++

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/CombinedEventAccessFactory.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::CombinedEventAccessFactory;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ICombinedEventData;
using TA_Base_Core::IDatabaseCallbackUser;

// Assign the required values to the column constants.
const std::string CombinedEventFilter::COLUMN_DATETIME("Date/Time");
const std::string CombinedEventFilter::COLUMN_SEVERITY("Sev");
const std::string CombinedEventFilter::COLUMN_ASSET("Asset");
const std::string CombinedEventFilter::COLUMN_DESCRIPTION("Description");
const std::string CombinedEventFilter::COLUMN_VALUE("Value");
const std::string CombinedEventFilter::COLUMN_MMS("M");
const std::string CombinedEventFilter::COLUMN_DSS("D");
const std::string CombinedEventFilter::COLUMN_AVL("A");
const std::string CombinedEventFilter::COLUMN_OPERATOR("Operator");
const std::string CombinedEventFilter::COLUMN_COMMENT("C");

// Assign the required values to the hidden column constants.
const std::string CombinedEventFilter::HIDDEN_COLUMN_ALARMID("Alarm ID");
const std::string CombinedEventFilter::HIDDEN_COLUMN_ACKED("Acked");
const std::string CombinedEventFilter::HIDDEN_COLUMN_STATUS("Status");

// Assign the required values to the filter constants.
const std::string CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM("Application Subsystem");
const std::string CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM("Physical Subsystem");
const std::string CombinedEventFilter::FILTER_LOCATION("Location");
const std::string CombinedEventFilter::FILTER_OPERATOR("Operator");
const std::string CombinedEventFilter::FILTER_EVENTTYPE("Event Type");
const std::string CombinedEventFilter::FILTER_ASSET("Asset");
const std::string CombinedEventFilter::FILTER_DESCRIPTION("Description");
const std::string CombinedEventFilter::FILTER_SEVERITY("Severity");
const std::string CombinedEventFilter::FILTER_ALARMTYPE("Alarm Type");
const std::string CombinedEventFilter::FILTER_ALARMID("Alarm ID");

// The name of the default filter.
const std::string CombinedEventFilter::NAMEDFILTER_DEFAULT( "Default Filter" );

const unsigned long CombinedEventFilter::MIN_DISPLAY_ITEM_NUM = 50;


namespace
{
    const unsigned int TWENTY_FOUR_HOURS = 24*60*60;
    const std::string EMPTY_STRING("");

	const COLORREF COLOUR_BLACK( RGB(0, 0, 0) );
    const COLORREF COLOUR_BEIGE( RGB( 230, 227, 194 ) );
    const COLORREF COLOUR_DARKBEIGE( RGB( 219, 186, 119 ) );

    const unsigned long DEFAULT_EVENT_LEVEL( 5 );

    const std::string STATUS_QUERYING( "Executing database query..." );
    const std::string STATUS_DUTY( "Retrieving duty..." );
}

CombinedEventFilter::CombinedEventFilter() : m_latestPkey(0), m_authenticationLib(NULL), m_pageStatus(0),
                                             m_pCreateDefaultFilterThread(NULL) //zhiqiang++
{
    // Initialise the default background colour to beige and foreground to black.
	m_foregroundColour = COLOUR_BLACK;
    m_backgroundColour = COLOUR_BEIGE;

    // Set up the actions.
    m_actions.push_back( new ActionCreateIncidentReport() );
    m_actions.push_back( new ActionShowComment() );

    // Add all the required columns to the vector.
	// TD 13144 increase width of date/time and asset name to ensure they are fully visible
    m_columns.push_back( Filter::Column( COLUMN_DATETIME, 135, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_SEVERITY, 40, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_ASSET, 210, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_DESCRIPTION, 450, ACTION_NO_ACTION ) ); //TD15005
    m_columns.push_back( Filter::Column( COLUMN_VALUE, 167, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_MMS, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_DSS, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_AVL, 24, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_OPERATOR, 150, ACTION_NO_ACTION ) );
    m_columns.push_back( Filter::Column( COLUMN_COMMENT, 24, ACTION_SHOW_COMMENT ) );

	//*zhiqiang++begin
	//m_pCreateDefaultFilterThread = new CCreateDefaultFilterThread(this);
	//m_pCreateDefaultFilterThread->start();
	CreateDefaultFilter();
	//*/ //zhiqiang++end

	// Setup loading thread.
	//m_loadingThread = new LoadingThread(this);
 
	// Set default sorting.
    sort( getDefaultSortColumn(), false );

}

CombinedEventFilter::~CombinedEventFilter()
{
    FUNCTION_ENTRY("destructor");

    unsigned int i;
    TA_Base_Core::ThreadGuard guard(m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
    // Remove all columns.
    m_columns.clear();

    // Delete all the displayItems - this is important, as the filter can be desrtucted while the applicaiton is running 
// 	if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
// 	{
// 		// If no visibility rules, only display items for current page are stored in client side.
// 		for ( i=0; i<m_displayItems.size(); i++ )
// 		{
// 			delete m_displayItems[i];
// 		}
// 	}
// 	else
	{
		// All event items not excluded by current filter are stored in client side.
		for( i=0; i<m_eventStoreListItems.size(); i++)
		{
			delete m_eventStoreListItems[i];
		}
		m_eventStoreListItems.clear();
	}

	m_displayItems.clear();


    // The filter owns any actions so we need to delete them.
    for ( i=0; i<m_actions.size(); i++ )
    {
        delete m_actions[i];
    }
    m_actions.clear();
	if( m_authenticationLib != NULL )
	{
		delete m_authenticationLib;
		m_authenticationLib = NULL;
	}
	//zhiqiang++begin
	if (m_pCreateDefaultFilterThread != NULL)
	{
		delete m_pCreateDefaultFilterThread;
		m_pCreateDefaultFilterThread = NULL;
	}
    //zhiqiang++end
    FUNCTION_EXIT;
}

CombinedEventFilter::LoadingThread::LoadingThread(CombinedEventFilter* filter) : m_isStopped (true) , m_filter(filter){}
CombinedEventFilter::LoadingThread::~LoadingThread() {}

bool CombinedEventFilter::LoadingThread::getStopFlag()
{
	return m_isStopped;
}

void CombinedEventFilter::LoadingThread::setColumnRange(std::string startTime, unsigned long totalRows)
{
	//m_startTimeStr = startTime;
	m_totalRows = totalRows;
}

void CombinedEventFilter::LoadingThread::terminate() {}

void CombinedEventFilter::LoadingThread::run()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: Entering Loading Thread. ");
	m_isStopped = false;
	m_filter->setPauseRefreshFlag(true);
    CombinedEventAccessFactory::EventFilter filter;
    m_filter->populatePublicFilter(filter);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "populating filter finished.");
	try
	{
		std::vector<ICombinedEventData*> events;
		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Begin calling loadDesignatedEvents() function.");
		
		events = CombinedEventAccessFactory::getInstance().loadDesignatedEvents(true, filter, m_startTimeStr, m_totalRows);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Stop calling loadDesignatedEvents() function.");
		
		if(!events.empty())
		{
			std::vector<ICombinedEventData*>::iterator eventIt;
			for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
			{
				std::auto_ptr<CombinedEventItem> newItem;
				
				try
				{
					// Construct the event item and make sure the event data is cleaned up.
					std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
					newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
				}
				catch ( ... )
				{
					// If there was any problem constructing the item don't add it to the display list.
					continue;
				}
				
				m_filter->pushIntoStoredListItems( newItem.release());			
			}
			
			m_filter->assignDisplayListItems();
			
			// Get the next set of events.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:calling getNextEvents() function.");
			events = CombinedEventAccessFactory::getInstance().getNextEvents();
			
			// Pause if required.
			while ( m_filter->getPauseLoadingFlag() && !m_filter->getStopLoadingFlag() )
			{
				Sleep( 250 );
			}
			m_filter->preloadListView();
		}

		while ( !events.empty() && !m_filter->getStopLoadingFlag() )
		{
			// Allocate required space in the display list.
			//m_filter->reserveStoredListItemsSize( m_filter->getStoredListItemsSize() + events.size() );
			
			// Add the events to the store list.
			std::vector<ICombinedEventData*>::iterator eventIt;
			for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
			{
				std::auto_ptr<CombinedEventItem> newItem;
				
				try
				{
					// Construct the event item and make sure the event data is cleaned up.
					std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
					newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
				}
				catch ( ... )
				{
					// If there was any problem constructing the item don't add it to the display list.
					continue;
				}
				
				m_filter->pushIntoStoredListItems( newItem.release());			
			}
			
			m_filter->assignDisplayListItems();

			// Get the next set of events.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:calling getNextEvents() function.");
			events = CombinedEventAccessFactory::getInstance().getNextEvents();
			
			// Pause if required.
			while ( m_filter->getPauseLoadingFlag() && !m_filter->getStopLoadingFlag() )
			{
				Sleep( 250 );
			}

		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Loading finished.");
	}
	catch(...)
	{
		// If there's a problem (any problem), notify the user, then return 0.
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
		LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
	}

	m_filter->refreshListView();

	m_filter->setPauseRefreshFlag(false);
	m_isStopped = true;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log: Loading Thread stops.");
	
}

void CombinedEventFilter::internalGetListData(std::string startTime, unsigned long totalRows)
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: Entering Loading Thread. ");
	
	setPauseRefreshFlag(true);
	CombinedEventAccessFactory::EventFilter filter;
	populatePublicFilter(filter);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "populating filter finished.");
	try
	{
		CWnd* pWnd = AfxGetMainWnd();
		std::vector<ICombinedEventData*> events;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Begin calling loadDesignatedEvents() function.");

		bool enableVisibilityRule=true;

		if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
		{
			enableVisibilityRule=false;
		}

		events = CombinedEventAccessFactory::getInstance().loadDesignatedEvents(enableVisibilityRule, filter, startTime, totalRows);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Stop calling loadDesignatedEvents() function.");

		if(!events.empty())
		{
			std::vector<ICombinedEventData*>::iterator eventIt;
			for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
			{
				std::auto_ptr<CombinedEventItem> newItem;

				try
				{
					// Construct the event item and make sure the event data is cleaned up.
					std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
					newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
				}
				catch ( ... )
				{
					// If there was any problem constructing the item don't add it to the display list.
					continue;
				}

				pushIntoStoredListItems( newItem.release());			
			}

			assignDisplayListItems();

			// Get the next set of events.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:calling getNextEvents() function.");
			events = CombinedEventAccessFactory::getInstance().getNextEvents();

			// Pause if required.
			while (getPauseLoadingFlag() && !getStopLoadingFlag() )
			{
				Sleep( 250 );
			}
			preloadListView();
		}

        TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
		m_eventStoreListItems.reserve(m_maxEvents);
		unsigned long count=0;
		while ( !events.empty() && !getStopLoadingFlag() )
		{
			// Allocate required space in the display list.
			// Add the events to the store list.
			std::vector<ICombinedEventData*>::iterator eventIt;
			for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
			{
				std::auto_ptr<CombinedEventItem> newItem;
                count++;
				try
				{
					// Construct the event item and make sure the event data is cleaned up.
					std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
					newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
				}
				catch ( ... )
				{
					// If there was any problem constructing the item don't add it to the display list.
					continue;
				}
				if ((count%800)==0  && !getStopLoadingFlag() )
				{
					Sleep( 250 );
				}
                m_eventStoreListItems.push_back(newItem.release());				 		
			}		

			// Get the next set of events.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:calling getNextEvents() function.");
			events = CombinedEventAccessFactory::getInstance().getNextEvents();

			// Pause if required.
			while (getPauseLoadingFlag() && !getStopLoadingFlag() )
			{
				Sleep( 250 );
			}
			// show what we already have
			//preloadListView();
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Loading finished.");
	}
	catch(...)
	{
		// If there's a problem (any problem), notify the user, then return 0.
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
		LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
	}
     //assignDisplayListItems();
     refreshListView();
     sendStatusBarUpdate( 0, NULL );
	 setPauseRefreshFlag(false);
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log: Loading Thread stops.");
}
void CombinedEventFilter::getListData(bool clearPage /*=true*/)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:Entering getListData() function. ");

	//Wait the loading thread to stop.
	m_stopEventLoading = true;
// 	while(!m_loadingThread->getStopFlag())
// 	{
// 		TA_Base_Core::Thread::sleep(200);
// 	}
	//m_loadingThread->terminateAndWait();
	unsigned long eventCount;

    // Clear all selections when loading from scratch.
    m_selectedItems.clear();

    // Querying the duty agent.
    //sendStatusBarUpdate( 0, STATUS_DUTY.c_str() );  //we don't need to get duty for event filtering.
    sendStatusBarUpdate( 0, STATUS_QUERYING.c_str() );
	

	// Determine the page status
	if (clearPage)
	{
		m_currentPage = 0;
		m_pageTimeMap.clear();
		m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage, EMPTY_STRING));
		    LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
	}
	bool hasNextPage = false;
	//populateFilter cannt be called until createDefaultFilter thread finish all work
	//m_pCreateDefaultFilterThread->terminateAndWait(); //zhiqiang++

    CombinedEventAccessFactory::EventFilter filter;

    // Notify that the list control contains no elements.
    sendListUpdate( 0 );

	m_stopEventLoading = false;
// 	if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
// 	{
// 		{	 
// 			TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
// 			// Clean up all of the old items.
// 			std::vector<DisplayItem*>::iterator itemIt;
// 			{ 
// 				TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
// 				for ( itemIt=m_displayItems.begin(); itemIt!=m_displayItems.end(); itemIt++ )
// 				{
// 					delete *itemIt;
// 				}
// 				m_displayItems.clear();
// 				m_displayItems.reserve(m_maxEvents);
// 			}
// 		}
//         populateFilter(filter);
// 
// 		try
// 		{
// 			// Executing the query.
// 			sendStatusBarUpdate( 0, STATUS_QUERYING.c_str() );
// 
// 			// Load the initial set of events.
// 			std::vector<ICombinedEventData*> events;
// 			events = CombinedEventAccessFactory::getInstance().getFirstEventsOnPage( filter, m_currentPage, m_maxEvents );
// 
// 			// Prepare the status bar.
// 			sendStatusBarUpdate( m_maxEvents, NULL );
//             TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
// 			while ( !events.empty() && !m_stopEventLoading )
// 			{
// 				// Allocate required space in the display list.
// 				m_displayItems.reserve( m_displayItems.size() + events.size() );
// 
// 				// Add the events to the display list.
// 				std::vector<ICombinedEventData*>::iterator eventIt;
// 				for ( eventIt=events.begin(); eventIt!=events.end(); eventIt++ )
// 				{
// 					std::auto_ptr<CombinedEventItem> newItem;
// 
// 					try
// 					{
// 						// Construct the event item and make sure the event data is cleaned up.
// 						std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
// 						newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
// 					}
// 					catch ( ... )
// 					{
// 						// If there was any problem constructing the item don't add it to the display list.
// 						continue;
// 					}
// 
// 					m_displayItems.push_back( newItem.release() );
// 				}
// 
// 				// Notify that list control has more elements every 1000.
// 				if ( 0 == m_displayItems.size()%1000 )
// 				{
// 					sendListUpdate( m_displayItems.size() );
// 				}
// 
// 				// Get the next set of events.
// 				events = CombinedEventAccessFactory::getInstance().getNextEvents();
// 
// 				// Pause if required.
// 				while ( m_pauseEventLoading && !m_stopEventLoading )
// 				{
// 					Sleep( 250 );
// 				}
// 			}
// 		}
// 		catch(...)
// 		{
// 			// If there's a problem (any problem), notify the user, then return 0.
// 			TA_Base_Bus::TransActiveMessage userMsg;
// 			UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
// 			LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
// 		}
// 	}
// 	else
	{		 
		sendListViewUpdate(0,0);
		std::vector<DisplayItem*>::iterator itemIt;
		{		 
			TA_Base_Core::ThreadGuard guard(m_storedItemsLock);		 	 
			
			for ( itemIt=m_eventStoreListItems.begin(); itemIt!=m_eventStoreListItems.end(); itemIt++ )
			{
				delete *itemIt;
			}
			m_eventStoreListItems.clear();
			m_eventStoreListItems.reserve(m_maxEvents);
		}
	 
		{ 
			// Clean up all of the old items.
			// wenguang++ TD14188
			TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);				 
			m_displayItems.clear();
			m_displayItems.reserve(m_maxEvents);
		} 

		//Starting the loading thread

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Start the loading thread.");
		m_stopEventLoading = false;
		PageTimeMap::iterator pageIt = m_pageTimeMap.find(m_currentPage);
		if (pageIt != m_pageTimeMap.end())
		{
			//m_loadingThread->setColumnRange(pageIt->second, m_maxEvents+1);
			internalGetListData(pageIt->second, m_maxEvents+1);
		}else{
			//m_loadingThread->setColumnRange("", m_maxEvents+1);
			internalGetListData("", m_maxEvents+1);
		}
		//m_loadingThread->start();

		// ++wenguang TD14188

	 
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log:Leaving getListData() function come out");
}

bool CombinedEventFilter::refreshListData()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering refreshListData() function. ");
	if ( m_pauseRefresh ) return false;//changxing
	if (m_currentPage>0) return false;//chenlei
//	if (!m_loadingThread->getStopFlag()) return false; //chenlei
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Refreshing event data...");
	
    bool changeMade = false;

// 	if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
// 	{
// 		try
// 		{
// 			// Load new events since the last load.
// 			std::vector<ICombinedEventData*> events;
// 			events = CombinedEventAccessFactory::getInstance().getEventsSinceLastLoad();
//           
// 			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "REFRESH: Found %d new elements.", events.size() );
//             TA_Base_Core::ThreadGuard guard(m_storedItemsLock);
// 	        TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
// 			if ( 0 < events.size() )
// 			{
// 				// Event size is non-zero so a change will be made.
// 				changeMade = true;
// 
// 				// Allocate required space in the display list.
// 				m_displayItems.reserve( m_displayItems.size() + events.size() );
// 
// 				// Add the events to the display list in the correct position.
// 				std::vector<ICombinedEventData*>::iterator eventIt = events.begin();
// 				for ( ; eventIt!=events.end(); eventIt++ )
// 				{
// 					std::auto_ptr<CombinedEventItem> newItem;
// 
// 					try
// 					{
// 						// Construct the event item and make sure the event data is cleaned up.
// 						std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
// 						newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
// 					}
// 					catch ( ... )
// 					{
// 						// If there was any problem constructing the item don't add it to the display list.
// 						continue;
// 					}
// 
// 					// Find where to insert this event - using the std::find_if method and compare_events (defined in the Filter base 
// 					// class) with the newItem bound to the first variable
// 					std::vector<DisplayItem*>::iterator insertPoint = 
// 						std::find_if( m_displayItems.begin(), m_displayItems.end(), std::bind1st( compare_events(),newItem.get() ) );
// 
// 					// The conditions for insertion depend on the current page.
// 					// 1. Can insert at the start of the list when on the first page.
// 					// 2. Can insert at the end of the list when on the last page.
// 					// 3. Can always insert into the middle of the list.
// 					if ( ( ( insertPoint == m_displayItems.begin() ) && ( 0 == m_currentPage ) ) ||
// 						 ( ( insertPoint == m_displayItems.end() ) && ( m_maxEvents > m_displayItems.size() ) ) ||
// 						 ( ( insertPoint != m_displayItems.begin() ) && ( insertPoint != m_displayItems.end() ) ) )
// 					{
// 						// Insert the item at the point found.
// 						m_displayItems.insert(insertPoint, newItem.release() );
// 
// 						// Convert the iterator to an index. This can be done using the distance method as a vector uses
// 						// random access iterators.
// 						std::vector<unsigned long>::difference_type newIndex = std::distance( m_displayItems.begin(), insertPoint );
// 
// 						// Now adjust selections as required.
// 						std::vector<unsigned long>::iterator selectedIt = m_selectedItems.begin();
// 						for ( ; selectedIt!=m_selectedItems.end(); selectedIt++ )
// 						{
// 							if ( *selectedIt >= newIndex )
// 							{
// 								(*selectedIt)++;
// 							}
// 						}
// 					}
// 				}
// 
// 				// TD11341: New events are in, now prune off any events that should now be on the next page.
// 				if ( m_displayItems.size() > m_maxEvents )
// 				{
// 					// m_displayItems.begin() is a random access iterator so we can convert from an index
// 					// to an iterator using addition.
// 					std::vector<DisplayItem*>::iterator toDelete;
// 					for ( toDelete=m_displayItems.begin()+m_maxEvents; toDelete!=m_displayItems.end(); toDelete++ )
// 					{
// 						delete *toDelete;
// 					}
// 					m_displayItems.erase( m_displayItems.begin()+m_maxEvents, m_displayItems.end() );
// 				}
// 			}
// 		}
// 		catch(...)
// 		{
// 			// If there's a problem (any problem), notify the user, then return 0.
// 			TA_Base_Bus::TransActiveMessage userMsg;
// 			UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
// 			LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
// 		}
// 	}
// 	else
	{
		try
		{
			// Load new events since the last load.
			std::vector<ICombinedEventData*> events;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering getDesignatedEventsSinceLastLoad.");

            bool enableVisibilityRule=true;
			if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
			{
				enableVisibilityRule=false;
			}
			events = CombinedEventAccessFactory::getInstance().getDesignatedEventsSinceLastLoad(enableVisibilityRule);

			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "REFRESH: Found %d new elements.", events.size() );
			TA_Base_Core::ThreadGuard guard(m_storedItemsLock);
	        TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
			if ( 0 < events.size() )
			{
				// Event size is non-zero so a change will be made.
				changeMade = true;

				// Allocate required space in the display list.
				m_eventStoreListItems.reserve( m_eventStoreListItems.size() + events.size() );

				// Add the events to the display list in the correct position.
				std::vector<ICombinedEventData*>::iterator eventIt = events.begin();
				for ( ; eventIt!=events.end(); eventIt++ )
				{
					std::auto_ptr<CombinedEventItem> newItem;

					try
					{
						// Construct the event item and make sure the event data is cleaned up.
						std::auto_ptr<ICombinedEventData> newEvent( *eventIt );
						newItem = std::auto_ptr<CombinedEventItem>( new CombinedEventItem( newEvent.get() ) );
					}
					catch ( ... )
					{
						// If there was any problem constructing the item don't add it to the display list.
						continue;
					}
					m_eventStoreListItems.push_back(newItem.release());
/*
					if( m_eventStoreListItems.size() == 0 )
					{
						m_eventStoreListItems.push_back( newItem.release());
					}
					else
					{
						std::vector<DisplayItem*>::iterator insertPoint = 
							std::lower_bound(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), newItem.get(), compare_events());
						m_eventStoreListItems.insert(insertPoint, newItem.release() );
					}
*/				
				}

				std::stable_sort(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), compare_events());

				LPARAM pageStatus = 0;
				if(m_eventStoreListItems.size() > m_maxEvents)
				{
					pageStatus |= MASK_PAGEDOWN;
					std::vector<DisplayItem*>::iterator toDelete;
					for ( toDelete=m_eventStoreListItems.begin()+m_maxEvents; toDelete!=m_eventStoreListItems.end(); toDelete++ )
					{
						delete *toDelete;
					}
					m_eventStoreListItems.erase(m_eventStoreListItems.begin()+m_maxEvents, m_eventStoreListItems.end());
					if (m_eventStoreListItems.size() > m_maxEvents)
					{
						std::string  str= static_cast<CombinedEventItem*>(m_eventStoreListItems[m_maxEvents])->getCreateTimeStr();
						int pagenum = m_currentPage+1;
						m_pageTimeMap.insert( PageTimeMap::value_type(pagenum, str) );				
					}
						//m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage+1, static_cast<CombinedEventItem*>(m_eventStoreListItems[m_maxEvents])->getCreateTimeStr()));
				}
		        LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
				m_pageStatus = pageStatus;
				// Copy the list items to display list.
				m_displayItems.clear();
				m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

				sendListViewUpdate(m_displayItems.size(), pageStatus);
				/*
				// TD11341: New events are in, now prune off any events that should now be on the next page.
				if ( m_displayItems.size() > m_maxEvents )
				{
					// m_displayItems.begin() is a random access iterator so we can convert from an index
					// to an iterator using addition.
					std::vector<DisplayItem*>::iterator toDelete;
					for ( toDelete=m_displayItems.begin()+m_maxEvents; toDelete!=m_displayItems.end(); toDelete++ )
					{
						delete *toDelete;
					}
					m_displayItems.erase( m_displayItems.begin()+m_maxEvents, m_displayItems.end() );
				}
				*/
			}
		}
		catch(...)
		{
			// If there's a problem (any problem), notify the user, then return 0.
			TA_Base_Bus::TransActiveMessage userMsg;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_040030);
			LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Database load was not successful");
		}
	}
	sendStatusBarUpdate( 0, NULL );
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving refreshListData() function. ");
    return changeMade;
}

const std::vector<Filter::Column>& CombinedEventFilter::getColumns() const
{
    return m_columns;
}

const std::vector<std::string> CombinedEventFilter::getFilters() const
{
    // Filters will be returned 
    std::vector<std::string> filters;

    // Add all the filters to the list.
    filters.push_back(FILTER_APPLICATION_SUBSYSTEM);
    filters.push_back(FILTER_PHYSICAL_SUBSYSTEM);
    filters.push_back(FILTER_LOCATION);
    filters.push_back(FILTER_OPERATOR);
    filters.push_back(FILTER_EVENTTYPE);
    filters.push_back(FILTER_ASSET);
    filters.push_back(FILTER_DESCRIPTION);
    filters.push_back(FILTER_SEVERITY);
    filters.push_back(FILTER_ALARMTYPE);
    filters.push_back(FILTER_ALARMID);

    return filters;
}

const std::string& CombinedEventFilter::getItemData(const unsigned long index,const std::string& columnName,const bool summarise/*=true*/)
{
    // This array index operation has a small window where it can fail - which is the time between the data vector
    // being updated, but before the "SetItemCount" method can be called on the list. For this time (possilby only two-to
    // three calls of this method, and only when the vector is being shrunk, while the end of the list is still visible)
    // there needs to be a backup solution - so instead of crashing the application, we just return an empty string. It 
    // does nothing, and will leave an empty cell (if an update ever has time to complete its paint - which is unlikely),
    // but it is the best solution.

	int i=0;
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: Getting Item No. %d.", index);
	TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
	if (index >= m_displayItems.size())
	{
// 		if (i>10)
// 		{
			return EMPTY_STRING;
// 		}
// 		TA_Base_Core::Thread::sleep(200);
// 		i++;
	}

    try
    {
        if ( summarise )
        {
            return m_displayItems[index]->getColumnData(columnName);
        }
        else
        {
            return m_displayItems[index]->getCompleteColumnData(columnName);
        }
    }
    catch(...)
    {
        return EMPTY_STRING;
    }
}

COLORREF CombinedEventFilter::getRowColour( const unsigned long row )
{
    // Make sure the row isn't out of range.
	COLORREF foreground = m_foregroundColour;
	COLORREF background = m_backgroundColour;
	TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
    if ( row < m_displayItems.size() )
    {

        // Colour decision should be based upon the existance of an alarm id.
        if ( 0 < m_displayItems[row]->getColumnData(CombinedEventFilter::HIDDEN_COLUMN_ALARMID).size() )
        {
			determineRowColours(row, foreground, background);
        }

    }
	return foreground;
}

COLORREF CombinedEventFilter::getRowColourDefault( TA_Base_Core::IEventColourData::EEventColourType colourType  )
{
	COLORREF groundColor = m_foregroundColour;
    
	switch (colourType)
	{
		//foreground
		case TA_Base_Core::IEventColourData::OPEN_UNACKED_FG1:
		case TA_Base_Core::IEventColourData::CLOSED_UNACKED_FG1:
		case TA_Base_Core::IEventColourData::OPEN_ACKED_FG1:
		case TA_Base_Core::IEventColourData::CLOSED_ACKED_FG1:
		{
			groundColor = m_foregroundColour;
			break;
		}
		//background
		case TA_Base_Core::IEventColourData::OPEN_UNACKED_BG1:
		case TA_Base_Core::IEventColourData::CLOSED_UNACKED_BG1:
		case TA_Base_Core::IEventColourData::OPEN_ACKED_BG1:
		case TA_Base_Core::IEventColourData::CLOSED_ACKED_BG1:
		{
			groundColor = m_backgroundColour;
			break;
		}
		default:
		{
			groundColor = m_foregroundColour;
			break;
		}
	}
	
	return groundColor;
}

void CombinedEventFilter::determineRowColours( const unsigned long row, unsigned long& foregroundColour, unsigned long& backgroundColour )
{
	FUNCTION_ENTRY("determineRowColours");
    TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
    // Make sure the row isn't out of range.
    if ( row < m_displayItems.size() )
    {
		// Only proceed if the it is an alarm related event
        if ( 0 < m_displayItems[row]->getColumnData(CombinedEventFilter::HIDDEN_COLUMN_ALARMID).size() )
		{
			// EventColourType variables
			TA_Base_Core::IEventColourData::EEventColourType foregroundColourType;
			TA_Base_Core::IEventColourData::EEventColourType backgroundColourType;

			// Determine the event's is acked and closed status
			bool isAcked = false;
			bool isClosed = false;

			if ( 0 < m_displayItems[row]->getColumnData(CombinedEventFilter::HIDDEN_COLUMN_ACKED).size() ) isAcked = true;
			if ( 0 ==  atoi( m_displayItems[row]->getColumnData(CombinedEventFilter::HIDDEN_COLUMN_STATUS).c_str() ) ) isClosed = true;

			// Now determine the colour types based on the 3 parameter-combinations
			if ( !isAcked && !isClosed )
			{
				foregroundColourType = TA_Base_Core::IEventColourData::OPEN_UNACKED_FG1;
				backgroundColourType = TA_Base_Core::IEventColourData::OPEN_UNACKED_BG1;
			}
			else if ( !isAcked && isClosed )	// CL17270++
			{
				foregroundColourType = TA_Base_Core::IEventColourData::CLOSED_UNACKED_FG1;
				backgroundColourType = TA_Base_Core::IEventColourData::CLOSED_UNACKED_BG1;
			}
			else if ( isAcked && !isClosed )
			{
				foregroundColourType = TA_Base_Core::IEventColourData::OPEN_ACKED_FG1;
				backgroundColourType = TA_Base_Core::IEventColourData::OPEN_ACKED_BG1;
			}
			else if ( isAcked && isClosed )
			{
				foregroundColourType = TA_Base_Core::IEventColourData::CLOSED_ACKED_FG1;
				backgroundColourType = TA_Base_Core::IEventColourData::CLOSED_ACKED_BG1;
			}
			else // set to default and return
			{
				foregroundColour = m_foregroundColour;
				backgroundColour = m_backgroundColour;

				FUNCTION_EXIT;
				return;
			}

			// Now determine the colour reference				
			foregroundColour = getColourReference(row, foregroundColourType);
			backgroundColour = getColourReference(row, backgroundColourType);
		}
		// return the default colours
		else
		{
			foregroundColour = m_foregroundColour;
			backgroundColour = m_backgroundColour;
		}
	}

	FUNCTION_EXIT;
}

const COLORREF CombinedEventFilter::getColourReference(const unsigned long row, TA_Base_Core::IEventColourData::EEventColourType colourType)
{
	FUNCTION_ENTRY("getColourReference");
    TA_Base_Core::ThreadGuard guard(m_displayItemsLock);
	std::string severityName = m_displayItems[row]->getColumnData(CombinedEventFilter::COLUMN_SEVERITY);
	std::map<std::string, TA_Base_Core::IEventColourData*>::iterator colourItr = m_eventColoursMap.find(severityName);
	if (colourItr != m_eventColoursMap.end() )
	{
		TA_Base_Core::IEventColourData* eventColData = colourItr->second;
		
		FUNCTION_EXIT;
		return eventColData->getEventColourKey(colourType);
	}
	
    LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Can't determine row: %li colours. Reason: Can't find severity name in map", row);

	LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Getting the default colours");
	
	FUNCTION_EXIT;
	//return getRowColour(row); //This causes infinite loop, which stops the event viewer
	return getRowColourDefault(colourType); //Use other getRowColour function to avoid infinite loop
}

const COLORREF CombinedEventFilter::getItemColour(const unsigned long index, const std::string& columnName)
{
	COLORREF foreground = getRowColour(index);
	COLORREF background = getBackgroundColour();

	determineRowColours(index, foreground, background);

	return background; // For print the foreground is always black while the background varies in colour
}

// yanrong++ CL-15055
std::string CombinedEventFilter::privGetCurrentFilterString()
{
    FUNCTION_ENTRY("CombinedEventFilter::privGetCurrentFilterString");

    // Filter data is contained in two maps - a string-string and a string-long map, so create iterators for both.
    TA_Base_Core::NamedFilter::StringList stringFilterList = m_namedFilter->getStringFilterList();
    TA_Base_Core::NamedFilter::StringList numericFilterList = m_namedFilter->getNumericFilterList();

    TA_Base_Core::NamedFilter::StringList::iterator stringIter;

    // Iterate through each map, adding any filters to the string..
    std::string filterString("");
    for ( stringIter=stringFilterList.begin(); stringIter!=stringFilterList.end(); stringIter++ )
    {
        filterString += " [" + *stringIter + "]";
    }

    for ( stringIter=numericFilterList.begin(); stringIter!=numericFilterList.end(); stringIter++ )
    {
        filterString += " [" + *stringIter + "]";
    }

    FUNCTION_EXIT;
    return filterString;
}


void CombinedEventFilter::saveAsDefaultFilter(const TA_Base_Core::NamedFilter& filter)
{
	m_defaultFilterString = privGetCurrentFilterString();
}
// ++yanrong CL-15055



std::string CombinedEventFilter::getCurrentFilterString()
{
    FUNCTION_ENTRY("CombinedEventFilter::getCurrentFilterString");

    std::string filterString = privGetCurrentFilterString();
	if(filterString.compare(m_defaultFilterString) == 0)
		filterString = "";

    FUNCTION_EXIT;
    return filterString;
}

const std::string& CombinedEventFilter::getDefaultSortColumn()
{
    // Default sort will always apply to time column.
    return COLUMN_DATETIME;
}

void CombinedEventFilter::resetListLoadFlag()
{
    // Reset load flag on the DAI.
    CombinedEventAccessFactory::getInstance().resetStopFlag();
}

void CombinedEventFilter::populateFilter( TA_Base_Core::CombinedEventAccessFactory::EventFilter& filter, bool refresh /*=false*/ )
{
    FUNCTION_ENTRY("CombinedEventFilter::populateFilter");

    // Define two vectors for retrieveing filter data
    std::vector<std::string> stringVector;
    std::vector<unsigned long> ulVector;

	filter.dutyEnabled = true;  
    if ( m_dutyEnabled )
    {
		DutyAdapter::DutyMap duty;
		filter.duty = duty;
		filter.profiles.clear();
		{
			TA_Base_Core::ThreadGuard storeGuard(m_sessionMapLock);
			std::string str_sessionId( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );
			SESSION_MAP::iterator iter=m_SessionMap.find(str_sessionId) ;
			if (iter!=m_SessionMap.end())
			{
				filter.sessionId = str_sessionId;
				TA_Base_Bus::SessionInfo sessionInfo = iter->second;
				filter.profiles = sessionInfo.ProfileId;
			}
			else
			{
				if( m_authenticationLib != NULL )  
				{				
					filter.sessionId = str_sessionId;
					//filter.profiles.push_back(0);  //testing only
					TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(str_sessionId, str_sessionId);
					filter.profiles = sessionInfo.ProfileId;					
					m_SessionMap.insert(SESSION_MAP::value_type(str_sessionId.c_str(), sessionInfo));
				}
			}
		}
        //system only get events based on right, so delete the following codes.

//         try
//         {
// 			if( TA_Base_Core::RunParams::getInstance().isSet(EventConstants::RPARAM_NO_VISIBILITY_RULE.c_str()))
// 			{
// 				// Get the locations and subsystems the user can and wants to view.
// 				DutyAdapter::DutyMap duty = m_dutyAdapter.getDutyMap( m_namedFilter->getNumericFilter(FILTER_LOCATION),
// 																	  m_namedFilter->getNumericFilter(FILTER_PHYSICAL_SUBSYSTEM) );
// 
// 				// Convert the duty to a format understood by the filter.
// 				// Currently both the duty adapter and the filter store the data as the same map so
// 				// we can just directly copy.
// 				filter.duty = duty;
// 			}
// 			else
// 			{
// 				// Get the locations and subsystems the user can and wants to view.
// 				TA_Base_Core::CombinedEventAccessFactory::FullDutyMap duty = m_dutyAdapter.getFullDutyMap( m_namedFilter->getNumericFilter(FILTER_LOCATION),
// 																	  m_namedFilter->getNumericFilter(FILTER_PHYSICAL_SUBSYSTEM),
// 																	  m_namedFilter->getNumericFilter(FILTER_APPLICATION_SUBSYSTEM));
// 
// 				// Convert the duty to a format understood by the filter.
// 				// Currently both the duty adapter and the filter store the data as the same map so
// 				// we can just directly copy.
// 				filter.fullDuty = duty;
// 
// 				std::string str_sessionId( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) );
// 				filter.sessionId = str_sessionId;
// 
// 				// Duty filter is enabled.
// 				filter.dutyEnabled = true;
// 
// 				if( m_authenticationLib != NULL )
// 				{
// 					TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLib->getSessionInfo(str_sessionId, str_sessionId);
// 					filter.profiles = sessionInfo.ProfileId;
// 				}
// 			}
//             // Duty filter is enabled.
//             filter.dutyEnabled = true;
//         }
//         catch ( ... )
//         {
//             // Couldn't obtain duty information, so revert to a no duty state.
//         }
    }
    
    // Application subsystem Ids
    ulVector = m_namedFilter->getNumericFilter(FILTER_APPLICATION_SUBSYSTEM);
    if ( !ulVector.empty() )
    {
        filter.appSubsystemIds = ulVector;
    }

    // Physical subsystem Ids.
    ulVector = m_namedFilter->getNumericFilter(FILTER_PHYSICAL_SUBSYSTEM);
    if ( !ulVector.empty() )
    {
        filter.physSubsystemIds = ulVector;
    }

    // Location Ids
    ulVector = m_namedFilter->getNumericFilter(FILTER_LOCATION);
    if ( !ulVector.empty() )
    {
        filter.locationIds = ulVector;
    }

    // Operator Ids
    ulVector = m_namedFilter->getNumericFilter(FILTER_OPERATOR);
    if ( !ulVector.empty() )
    {
        filter.operatorIds = ulVector;
    }
    
    // Event Types
    ulVector = m_namedFilter->getNumericFilter(FILTER_EVENTTYPE);
    if ( !ulVector.empty() )
    {
        filter.eventTypes = ulVector;
    }
    
    // Asset Name
    stringVector = m_namedFilter->getStringFilter(FILTER_ASSET);
    if ( !stringVector.empty() )
    {
        filter.assetName = "%" + stringVector[0] + "%";
    }
    
    // Start with description
    stringVector = m_namedFilter->getStringFilter(FILTER_DESCRIPTION);
    if ( !stringVector.empty() )
    {
        filter.description = "%" + stringVector[0] + "%";
    }

    // Set the time range.
    filter.fromTime = getFromTime();
    filter.toTime = getToTime();

    // Don't show non-alarm events by default.
    filter.showNonAlarmEvents = false;

    // Severity Ids
    ulVector = m_namedFilter->getNumericFilter(FILTER_SEVERITY);
    if ( !ulVector.empty() )
    {

        // Search through and see if non-alarm events are to be displayed.
        std::vector<unsigned long>::iterator it = ulVector.begin();
        for ( ; it!=ulVector.end(); it++ )
        {
            if ( *it == static_cast<unsigned long>(CombinedEventFilterDlg::EVENT_ONLY_KEY) )
            {
                // Remove it from the list of severities and continue.
                ulVector.erase( it );
                filter.showNonAlarmEvents = true;
                break;
            }
        }

        // Set severity filters.
        filter.severityIds = ulVector;
    }

    // Alarm Types
    ulVector = m_namedFilter->getNumericFilter(FILTER_ALARMTYPE);
    if ( !ulVector.empty() )
    {
        filter.alarmTypes = ulVector;
    }

	// Alarm ID
    stringVector = m_namedFilter->getStringFilter(FILTER_ALARMID);
    if ( !stringVector.empty() )
    {
		// clear filter.alarmId vector
		filter.alarmId.clear();
		// push_back the vector of alarm id string
		for (std::vector<std::string>::iterator itr=stringVector.begin(); itr!=stringVector.end(); ++itr)
		{
			filter.alarmId.push_back(*itr);
		}
    }


    // Event level.
    filter.eventLevel = m_eventLevel;

    // Column to sort by.
    if ( COLUMN_DATETIME == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
    }
    else if ( COLUMN_SEVERITY == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_SEVERITY;
    }
    else if ( COLUMN_ASSET == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_ASSET;
    }
    else if ( COLUMN_DESCRIPTION == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_DESCRIPTION;
    }
    else if ( COLUMN_VALUE == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_VALUE;
    }
    else if ( COLUMN_MMS == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_MMS;
    }
    else if ( COLUMN_DSS == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_DSS;
    }
    else if ( COLUMN_AVL == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_AVL;
    }
    else if ( COLUMN_OPERATOR == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_OPERATOR;
    }
    else if ( COLUMN_COMMENT == DisplayItem::s_sortColumn )
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_COMMENT;
    }
    else
    {
        filter.sortColumn = CombinedEventAccessFactory::SORT_CREATETIME;
    }

    // Order in which the sort should be performed.
    if ( DisplayItem::s_sortAscending )
    {
        filter.sortOrder = CombinedEventAccessFactory::SORT_ASCENDING;
    }
    else
    {
        filter.sortOrder = CombinedEventAccessFactory::SORT_DESCENDING;
    }

    FUNCTION_EXIT;
}



void CombinedEventFilter::nextPage()
{
	FUNCTION_ENTRY("nextPage");
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering nextPage() function. ");
	
	m_currentPage++;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
	getListData(false);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving nextPage() function. ");
	FUNCTION_EXIT;
}

void CombinedEventFilter::previousPage()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering previousPage() function. ");
	FUNCTION_ENTRY("previousPage");
	if( m_currentPage > 0 )
	{
		m_currentPage--;
	}
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:current page %d. ", m_currentPage);
	getListData(false);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving previousPage() function. ");
	FUNCTION_EXIT;
}


void CombinedEventFilter::sortListItems()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering sortListItems() function. ");
	FUNCTION_ENTRY("sortListItems");

	pauseRefresh();

// 	while (!m_loadingThread->getStopFlag())
// 	{
// 		TA_Base_Core::Thread::sleep(200);
// 	}

    sendListUpdate( 0 );
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	std::stable_sort(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), compare_events());
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
	m_displayItems.clear();
	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());


	sendListViewUpdate(m_displayItems.size(), m_pageStatus);
	/*
	if( m_eventStoreListItems.size() > m_maxEvents )
	{

		std::partial_sort( m_eventStoreListItems.begin(), m_eventStoreListItems.begin()+m_maxEvents, m_eventStoreListItems.end(), compare_events());
		m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.begin()+m_maxEvents);
		
		m_currentPage = 0;
		LPARAM pageStatus = 0;
		pageStatus |= MASK_PAGEDOWN;
		sendListViewUpdate(m_maxEvents, pageStatus);

		std::stable_sort(m_eventStoreListItems.begin()+m_maxEvents, m_eventStoreListItems.end(), compare_events());
	}
	else
	{
		std::stable_sort(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), compare_events());

		m_displayItems.clear();
		m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

	}
	*/
	resumeRefresh();
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving sortListItems() function. ");
	FUNCTION_EXIT;
}


void CombinedEventFilter::reverseListItems()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Entering reverseListItems() function. ");
	FUNCTION_ENTRY("reverseListItems");

// 	while (!m_loadingThread->getStopFlag())
// 	{
// 		TA_Base_Core::Thread::sleep(200);
// 	}
    sendListUpdate( 0 );
    TA_Base_Core::ThreadGuard guard (m_storedItemsLock);

	std::reverse(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
    TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
	m_displayItems.clear();

	/*
	// Copy the list item to display list.
	unsigned long pageStart = m_maxEvents*m_currentPage;
	TA_ASSERT( pageStart>=0, "Invalid event page start point(<0)");
	unsigned long pageEnd = (pageStart+m_maxEvents)< m_eventStoreListItems.size() ? (pageStart+m_maxEvents): m_eventStoreListItems.size();
	*/
	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());

	sendListViewUpdate(m_displayItems.size(), m_pageStatus);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "New EV Log:Leaving reverseListItems() function. ");
	FUNCTION_EXIT;
}

void CombinedEventFilter::sessionChanged()
{
	TA_Base_Core::ThreadGuard storeGuard(m_sessionMapLock);
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "sessionChanged");
	m_SessionMap.clear();
}

void CombinedEventFilter::preloadListView()
{
	TA_Base_Core::ThreadGuard storeGuard(m_storedItemsLock);
    unsigned long nCnt = m_eventStoreListItems.size();
	sendListViewUpdate( nCnt, 0 );
	LOG_GENERIC( SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", nCnt );	
	//sendListViewUpdate(m_maxEvents, 0);
	//LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", m_maxEvents);	
}

void CombinedEventFilter::refreshListView()
{
	//std::stable_sort(m_eventStoreListItems.begin(), m_eventStoreListItems.end(), compare_events());
	// std::copy( m_eventStoreListItems.begin()+oneHourEventsCount, m_eventStoreListItems.end(), m_displayItems.begin()+oneHourEventsCount);
	//sendListViewUpdate(0,0);
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);
	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
	LPARAM pageStatus = 0;
	if ( 0 < m_currentPage )
	{
		// If we aren't on the first page, we can move up.
		pageStatus |= MASK_PAGEUP;
	}
	if ( m_displayItems.size() > m_maxEvents )
	{
		// If we are now displaying more than a full page assume
		// we can move down..
		pageStatus |= MASK_PAGEDOWN;
		m_pageTimeMap.insert(PageTimeMap::value_type(m_currentPage+1, static_cast<CombinedEventItem*>(m_displayItems[m_maxEvents])->getCreateTimeStr()));
		    LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log:current Page %d. ", m_currentPage);
	}
	m_pageStatus = pageStatus;
	sendListViewUpdate( (m_displayItems.size() > m_maxEvents)?m_maxEvents:m_displayItems.size(), m_pageStatus);
	LOG_GENERIC(SourceInfo, DebugUtil::DebugTrace, "New EV Log: updating list view with total number %d", (m_displayItems.size() > m_maxEvents)?m_maxEvents:m_displayItems.size());
}

void CombinedEventFilter::reserveStoredListItemsSize(unsigned long newSize)
{
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	m_eventStoreListItems.reserve(newSize);
}

void CombinedEventFilter::pushIntoStoredListItems(CombinedEventItem* eventItem)
{
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	m_eventStoreListItems.push_back(eventItem);
}

unsigned long CombinedEventFilter::getStoredListItemsSize()
{
	return m_eventStoreListItems.size();
}

void CombinedEventFilter::populatePublicFilter(TA_Base_Core::CombinedEventAccessFactory::EventFilter &filter, bool refresh /* = false */)
{
	populateFilter(filter, refresh);
}

void CombinedEventFilter::assignDisplayListItems()
{
	TA_Base_Core::ThreadGuard guard (m_storedItemsLock);
	TA_Base_Core::ThreadGuard displayGuard(m_displayItemsLock);	
	m_displayItems.assign(m_eventStoreListItems.begin(), m_eventStoreListItems.end());
}

//*zhiqiang++begin
void CombinedEventFilter::CreateDefaultFilter()
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "void CombinedEventFilter::InitialFilterSetupInfo [enter initial filter info] ");
	try
    {
        m_earliestTime = 0; //too expensive to get the earliest time, so remove it.
		// Retrieve the earliest event time.
        //m_earliestTime = CombinedEventAccessFactory::getInstance().getEarliestEventTime();
    }
    catch(...)
    {
        // Earliest time will be set to 0, so "Previous" button will never be disabled.
        m_earliestTime = 0;
    }
    // Create the default filter.
    std::auto_ptr<TA_Base_Core::NamedFilter> defaultFilter( new TA_Base_Core::NamedFilter() );
    defaultFilter->addNumericFilter( Filter::FILTER_TOTIME, Filter::CURRENT_TIME );
    defaultFilter->addNumericFilter( Filter::FILTER_FROMTIME, Filter::PREVIOUS_DAY );
	
    // Create the avalanche filter.
    std::auto_ptr<TA_Base_Core::NamedFilter> avalancheFilter( new AvalancheNamedFilter( this ) );
    avalancheFilter->addNumericFilter( Filter::FILTER_TOTIME, Filter::CURRENT_TIME );
    avalancheFilter->addNumericFilter( Filter::FILTER_FROMTIME, static_cast<unsigned long>(Filter::CURRENT_TIME) );
	
    // Set the severities for both filters.
    DatabaseAccess& data=DatabaseAccess::getInstance();
    std::vector<std::pair<unsigned long,std::string> > severities = data.getSeverities();
    std::vector<std::pair<unsigned long,std::string> >::iterator it = severities.begin();
    for( it=severities.begin(); it!=severities.end(); it++ )
    {
        defaultFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, it->first );
        avalancheFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, it->first );
    }
    defaultFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, CombinedEventFilterDlg::EVENT_ONLY_KEY );
    avalancheFilter->addNumericFilter( CombinedEventFilter::FILTER_SEVERITY, CombinedEventFilterDlg::EVENT_ONLY_KEY );
	
    // Give the filters appropriate names.
    defaultFilter->setName( NAMEDFILTER_DEFAULT );
    avalancheFilter->setName( AvalancheNamedFilter::NAMEDFILTER_AVALANCHE );
	
    // Use the default filter and event level.
    m_namedFilter = std::auto_ptr<TA_Base_Core::NamedFilter>( defaultFilter->clone() );
    m_eventLevel = DEFAULT_EVENT_LEVEL;

	// yanrong++ CL-15055
	saveAsDefaultFilter(*defaultFilter.get());
	// ++yanrong CL-15055
	
	// Initialise and obtain the event colour maps
	m_eventColoursMap = data.getEventColours();
	
	// obtain the default foreground and background colours from the map
	std::map<std::string, TA_Base_Core::IEventColourData*>::iterator colourItr = m_eventColoursMap.find("DEFAULT");
	if (colourItr != m_eventColoursMap.end() )
	{
		TA_Base_Core::IEventColourData* eventColData = colourItr->second;
		m_foregroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_FG1);
		m_backgroundColour = eventColData->getEventColourKey(TA_Base_Core::IEventColourData::OPEN_ACKED_BG1);
	}
    // Register the static filters with the access factory.
    TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( defaultFilter );
    TA_Base_Core::NamedFilterAccessFactory::getInstance().registerStaticNamedFilter( avalancheFilter );
	
	// Load the authentication agent library
    try
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: CombinedEventFilter::CreateDefaultFilter new AuthenticationLibrary");
		m_authenticationLib = new TA_Base_Bus::AuthenticationLibrary(true);
    }
    catch(const TA_Base_Core::DataException& )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
    catch (const TA_Base_Core::DatabaseException&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException","Cannot create the authentication library. This is because the authentication agent configuration has not yet been set up so we go straight to the database login instead");
    }
	
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New EV Log: CombinedEventFilter::CreateDefaultFilter come out");
}
//*/ //zhiqiang++end

void CombinedEventFilter::terminateThread()
{
//	m_loadingThread->terminateAndWait();
}
