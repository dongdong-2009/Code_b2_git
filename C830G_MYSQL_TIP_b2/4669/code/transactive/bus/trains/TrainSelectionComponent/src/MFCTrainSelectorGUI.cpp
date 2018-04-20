/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Adam Radics
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

#include "stdafx.h"
#include <algorithm>

#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainCommonLibrary//src/LocationCache.h"

#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

namespace TA_IRS_Bus
{


MFCTrainSelectorGUI::MFCTrainSelectorGUI(CWnd* callbackWindow,
                                         int updateInboundMessage,
                                         int updateOutboundMessage,
                                         int updateSelectionMessage)
: m_updateInboundMessage(updateInboundMessage),
  m_updateOutboundMessage( updateOutboundMessage ),
  m_updateSelectionMessage( updateSelectionMessage ),
  m_inboundList(),
  m_outboundList(),
  m_currentSelectionList()
{
	FUNCTION_ENTRY( "MFCTrainSelectorGUI" );

    m_callbackWindows.push_back(callbackWindow);

	FUNCTION_EXIT;
}


MFCTrainSelectorGUI::MFCTrainSelectorGUI( std::vector<CWnd*> callbackWindows,
                                          int updateInboundMessage,
                                          int updateOutboundMessage,
                                          int updateSelectionMessage )
: m_callbackWindows( callbackWindows ),
  m_updateInboundMessage( updateInboundMessage ),
  m_updateOutboundMessage( updateOutboundMessage ),
  m_updateSelectionMessage( updateSelectionMessage ),
  m_inboundList(),
  m_outboundList(),
  m_currentSelectionList()
{
	FUNCTION_ENTRY( "MFCTrainSelectorGUI" );
	FUNCTION_EXIT;
}


MFCTrainSelectorGUI::MFCTrainSelectorGUI( int updateInboundMessage,
                                          int updateOutboundMessage,
                                          int updateSelectionMessage )
: m_callbackWindows(),
  m_updateInboundMessage( updateInboundMessage ),
  m_updateOutboundMessage( updateOutboundMessage ),
  m_updateSelectionMessage( updateSelectionMessage ),
  m_inboundList(),
  m_outboundList(),
  m_currentSelectionList()
{
	FUNCTION_ENTRY( "MFCTrainSelectorGUI" );
	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::addCallbackWindow( CWnd* callbackWindow )
{
	FUNCTION_ENTRY( "addCallbackWindow" );

    if ( callbackWindow != NULL )
    {
        // lock the list
        TA_Base_Core::ThreadGuard guard( m_callbackListLock );

        // add the window to the list of callbacks
        m_callbackWindows.push_back( callbackWindow );
        
        // the newly added window missed all the previous updates
        // it must be synchronised with the current train lists
        // and the current list of group selections
        synchroniseWindow( callbackWindow );
    }

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::removeCallbackWindow( CWnd* callbackWindow )
{
	FUNCTION_ENTRY( "removeCallbackWindow" );

    // lock the list
	TA_THREADGUARD( m_callbackListLock );

    std::vector<CWnd*>::iterator iter; // = std::find(m_callbackWindows.begin(),m_callbackWindows.end(),callbackWindow);
	
	bool bFindWindow = false;
	
	for (iter = m_callbackWindows.begin(); iter != m_callbackWindows.end(); iter++)
	{
		if ( (*iter)->GetSafeHwnd() == callbackWindow->GetSafeHwnd())
		{
			m_callbackWindows.erase(iter);
			bFindWindow = true;
			break;
		}
	}

	if (!bFindWindow)
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Callback window not found for deregistration");
	}
	
  //  if (iter != m_callbackWindows.end())
  // {
        // Remove the entry from the vector
  //      m_callbackWindows.erase(iter);
  //  }
  //  else
  //  {
  //      // The window wasn't found
  //      LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Callback window not found for deregistration");
  //  }

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::updateInboundList( EAction action,
											 const TrainInformationTypes::TrainDetails& trainDetails)
{
	FUNCTION_ENTRY( "updateInboundList" );

    // update the internal list
    // it threadlocks itself so dont need to do it here
    m_inboundList.updateList( action, trainDetails );

    postUpdates( action, trainDetails, m_updateInboundMessage );

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::updateOutboundList( EAction action,
											  const TrainInformationTypes::TrainDetails& trainDetails )
{
	FUNCTION_ENTRY( "updateOutboundList" );

    // update the internal list
    // it threadlocks itself so dont need to do it here
    m_outboundList.updateList( action, trainDetails );
	
    postUpdates( action, trainDetails, m_updateOutboundMessage );

	FUNCTION_EXIT;
}


TA_IRS_Bus::TrainInformationTypes::TrainDetailsList MFCTrainSelectorGUI::getSelectedTrainData() const 
{
	FUNCTION_ENTRY( "getSelectedTrainData" );

	TrainInformationTypes::TrainDetailsList trainDetailsList;

	FUNCTION_EXIT;
	return trainDetailsList;
}


void MFCTrainSelectorGUI::updateSelectionCombo( const std::vector<std::string>& trainListNames )
{
	FUNCTION_ENTRY( "updateSelectionCombo" );

    {
        // update the internal list
        TA_Base_Core::ThreadGuard guard( m_selectionListLock );
        m_currentSelectionList = trainListNames;
    }

    postUpdates( trainListNames, m_updateSelectionMessage );

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::synchroniseWindow( CWnd* callbackWindow )
{
	FUNCTION_ENTRY( "synchroniseWindow" );

    // no need to lock, already aquired the lock above
    // give it the latest selection list
    {
        TA_Base_Core::ThreadGuard guard( m_selectionListLock );
        postUpdate( m_currentSelectionList, m_updateSelectionMessage, callbackWindow );
    }

    // for each train in the inbound list send an add command
	TrainInformationTypes::TrainDetailsMap inboundTrains = m_inboundList.getCurrentTrains();
    for ( TrainInformationTypes::TrainDetailsMap::iterator inboundIter = inboundTrains.begin();
														  inboundIter != inboundTrains.end(); 
														  inboundIter++ )
    {
        postUpdate( ITrainSelectorGUI::Add, 
					inboundIter->second, 
					m_updateInboundMessage, 
					callbackWindow );
    }

    // for each train in the outbound list send an add command
    TrainInformationTypes::TrainDetailsMap outboundTrains = m_outboundList.getCurrentTrains();
    for ( TrainInformationTypes::TrainDetailsMap::iterator outboundIter = outboundTrains.begin();
														  outboundIter != outboundTrains.end(); 
														  outboundIter++ )
    {
        postUpdate( ITrainSelectorGUI::Add, 
					outboundIter->second, 
					m_updateOutboundMessage, 
					callbackWindow);
    }

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::postUpdates( EAction action,
                                       const TrainInformationTypes::TrainDetails& trainDetails,
                                       int messageNumber )
{
	FUNCTION_ENTRY( "postUpdates" );

    // lock the list
    TA_Base_Core::ThreadGuard guard( m_callbackListLock );

    for ( std::vector<CWnd*>::const_iterator iter = m_callbackWindows.begin();
											iter != m_callbackWindows.end(); 
											iter++ )
    {
        if ( ( *iter ) != NULL)
        {
            postUpdate( action, trainDetails, messageNumber, *iter );
        }
    }
	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::postUpdate( EAction action,
                                      const TrainInformationTypes::TrainDetails& trainDetails,
                                      int messageNumber,
                                      CWnd* window ) const
{
	FUNCTION_ENTRY( "postUpdate" );

    // create a structure on the heap. The receiver must delete it.
    // each receiver needs a new structure, because one handles and deletes
    // it before the other
    TrainListUpdate* update = new TrainListUpdate();
    update->action = action;
    update->trainDetails = trainDetails;

    // post it. A reinterpret cast or c-style cast is needed to convert the pointer to an unsigned int.
    int ret = window->PostMessage( messageNumber,
                                   reinterpret_cast<unsigned int>( update ) );
    if ( ret == 0 )
    {
        LOG_GENERIC(
        SourceInfo, DebugUtil::DebugError,
        "Error posting train update message. %d", messageNumber );

        // clean up - it wasnt posted so never will be deleted
        delete update;
        update = NULL;
    }
}


void MFCTrainSelectorGUI::postUpdates( const std::vector<std::string>& trainListNames,
                                       int messageNumber )
{
	FUNCTION_ENTRY( "postUpdates" );
    // lock the list
    TA_Base_Core::ThreadGuard guard( m_callbackListLock );

    for ( std::vector<CWnd*>::const_iterator iter = m_callbackWindows.begin();
											 iter != m_callbackWindows.end(); 
											 iter++ )
    {
        if ( (*iter) != NULL)
        {
            postUpdate(trainListNames, messageNumber, *iter);
        }
    }

	FUNCTION_EXIT;
}


void MFCTrainSelectorGUI::postUpdate(const std::vector<std::string>& trainListNames,
                                     int messageNumber,
                                     CWnd* window) const
{
	FUNCTION_ENTRY( "postUpdate" );

    // create a structure on the heap. The receiver must delete it.
    // each receiver needs a new structure, because one handles and deletes
    // it before the other
    SelectionListUpdate* update = new SelectionListUpdate();
    update->trainListNames = trainListNames;

    // post it. A reinterpret cast or c-style cast is needed to convert the pointer
    // to an unsigned int.
    int ret = window->PostMessage( messageNumber,
                                   reinterpret_cast<unsigned int>( update ) );
    if (ret == 0)
    {
        LOG_GENERIC(
        SourceInfo, DebugUtil::DebugError,
        "Error posting train updateSelectionCombo message.");

        // clean up
        delete update;
        update = NULL;
    }

	FUNCTION_EXIT;
}

bool MFCTrainSelectorGUI::isLocationInControl( unsigned long trainLocation, unsigned long trainCommunicationLocation)//limin++, ECP-338 CL-21050
{
	FUNCTION_ENTRY( "isLocationInControl" );
	FUNCTION_EXIT;
	return LocationCache::getInstance().isInControlOfLocation( trainLocation, trainCommunicationLocation );
}

} // TA_IRS_Bus

