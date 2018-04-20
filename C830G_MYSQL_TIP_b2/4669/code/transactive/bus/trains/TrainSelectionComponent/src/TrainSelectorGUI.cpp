/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/11/20 15:34:51 $
  * Last modified by:  $Author: builder $
  *
  * This handles the enabling/disabling of GUI items for train selection.
  * It also handles interaction between the components - so each gui can have
  * a common feel/operation, but the layout can be different.
  *
  */

#include "bus/trains/TrainSelectionComponent/src/StdAfx.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidSelectionListException.h"
#include "bus/generic_gui/src/TransactiveMessage.h" 
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/FunctionEntry.h"


namespace TA_IRS_Bus
{
    using TA_Base_Core::RunParams;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::FunctionEntry;


    TrainSelectorGUI::TrainSelectorGUI(TrainListCtrl& inboundList,
                                       TrainListCtrl* outboundList,
                                       CButton* allInbound,
                                       CButton* allOutbound,
                                       CButton* allTrains,
                                       CButton* clearAllTrains,
                                       CStatic* inboundLabel,
                                       CStatic* outboundLabel,
                                       TrainGroupComboBox* trainGroupCombo,
                                       CButton* saveGroup,
                                       CButton* deleteGroup,
									   TrainStaticGroupComboBox* trainStaticGroupCombo,
                                       ITrainSelectionListener* selectionListener /* = NULL */)
      : m_currentMode(LocationCache::InService),
        m_trainSelector(NULL),
        m_inboundList(&inboundList),
        m_outboundList(outboundList),
        m_allInbound(allInbound),
        m_allOutbound(allOutbound),
        m_allTrains(allTrains),
        m_clearAllTrains(clearAllTrains),
        m_inboundLabel(inboundLabel),
        m_outboundLabel(outboundLabel),
        m_trainGroupCombo(trainGroupCombo),
        m_saveGroup(saveGroup),
        m_deleteGroup(deleteGroup),
        m_trainListsEnabled(true),
        m_trainGroupsEnabled(true),
        m_trainGroupSaveEnabled(false),
        m_trainGroupDeleteEnabled(false),
        m_selectionListener(selectionListener),
        m_trainsAreSelected(false),
		m_trainStaticGroupCombo(trainStaticGroupCombo),
        m_enableStaticGroupCombo(true),
		m_isInRadioFallback(false)
    {
		FUNCTION_ENTRY( "TrainSelectorGUI" );

        TA_ASSERT(m_inboundList != m_outboundList, "Dont give two pointers to the same list. Just pass in one, and NULL for the second.");

        // reset train lists and selection combo
        clearData();

        // tell the lists to pass events on
        m_inboundList->setEventListener(this);
        if (m_outboundList != NULL)
        {
            m_outboundList->setEventListener(this);
        }

        // tell the combo box to pass events on
        if (m_trainGroupCombo != NULL)
        {
            m_trainGroupCombo->setEventListener(this);
        }

		FUNCTION_EXIT;
    }
        

    TrainSelectorGUI::~TrainSelectorGUI()
    {
		FUNCTION_ENTRY( "~TrainSelectorGUI" );

        // clear the listener because we dont want any callbacks
        m_selectionListener = NULL;

        // reset train lists and selection combo
        setMainlineMode();

        // tell the lists not to pass events on
		if (m_inboundList != NULL)
		{
			m_inboundList->setEventListener(NULL);
		}
        
        if (m_outboundList != NULL)
        {
            m_outboundList->setEventListener(NULL);
        }

        // tell the combo box not to pass events on
        if (m_trainGroupCombo != NULL)
        {
            m_trainGroupCombo->setEventListener(NULL);
        }

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::setTrainSelector(TrainSelector* theTrainSelector)
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::setDepotMode" );

        m_trainSelector = theTrainSelector;

        // now based on the new settings, enable/disable train groups
        enableGroupControls();
        
        FUNCTION_EXIT( "TrainSelectorGUI::setDepotMode" );
    }


    void TrainSelectorGUI::setDepotMode()
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::setDepotMode" );

        // momentarily hide the lists - because they flicker
        // this also looks cool - because it looks like a tab is actually being changed
        m_inboundList->ShowWindow( FALSE );
        if (m_outboundList != NULL)
        {
            m_outboundList->ShowWindow( FALSE );
        }

        // reset train lists and selection combo
        clearData();

        m_currentMode = LocationCache::Depot;

        // hide the GUI elements that should not be displayed for depot
        showInboundOutboundControls( false );

        // add padding to the list
        // only if there are two lists to work as one
        if ( m_outboundList != NULL )
        {
            m_outboundList->padList();
            
            // also hide the vertical scroll on the second list
            m_outboundList->setScrollBarVisibility( false );
            // temporarily hide on the first (until both lists are full)
            m_inboundList->setScrollBarVisibility( false );
        }

        m_inboundList->ShowWindow( TRUE );
        if ( m_outboundList != NULL )
        {
            m_outboundList->ShowWindow( TRUE );
        }

        FUNCTION_EXIT;
    }


    void TrainSelectorGUI::setMainlineMode()
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::setMainlineMode" );

        // momentarily hide the lists - because they flicker
        // this also looks cool - because it looks like a tab is actually being changed
        m_inboundList->ShowWindow( FALSE );
        if ( m_outboundList != NULL )
        {
            m_outboundList->ShowWindow( FALSE );
        }

        // reset train lists and selection combo
        clearData();

        // remove padding from the list
        // only if there are two lists to work as one
        if ( m_outboundList != NULL )
        {
            m_outboundList->removePadding();

            // also show the vertical scroll on the second list
            m_outboundList->setScrollBarVisibility( true );
            m_inboundList->setScrollBarVisibility( true );
        }

        m_inboundList->ShowWindow( TRUE );
        if ( m_outboundList != NULL )
        {
            m_outboundList->ShowWindow( TRUE );
        }


        m_currentMode = LocationCache::InService;

        // show the GUI elements that should be displayed for mainline
        showInboundOutboundControls( true );

        FUNCTION_EXIT;
    }


    void TrainSelectorGUI::setRights( bool canSelectTrains,
                                      bool canUseTrainGroups,
                                      bool canSaveTrainGroups,
                                      bool canDeleteTrainGroups )
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::setRights" );

        m_trainListsEnabled = canSelectTrains;
        m_trainGroupsEnabled = canUseTrainGroups;
        m_trainGroupSaveEnabled = canSaveTrainGroups;
        m_trainGroupDeleteEnabled = canDeleteTrainGroups;

        // enable/disable train lists
        m_inboundList->EnableWindow( m_trainListsEnabled );
        if ( m_outboundList != NULL )
        {
            m_outboundList->EnableWindow( m_trainListsEnabled );
        }

        // now based on the new settings, enable/disable train groups
        enableGroupControls();

        // notify the observer
        notifySelectionChanged();

        FUNCTION_EXIT;
    }


    CommonTypes::TrainIdList TrainSelectorGUI::getSelectedTrains() const
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::getSelectedTrains" );
        
        CommonTypes::TrainIdList inBoundTrains = m_inboundList->getSelectedTrainIds();

        // if in mainline mode, and the second list exists
        // then merge the selections from the second list into those from the first

        // ignore the second list in depot mode (selections will be mirrored)
        // and skip this if the second list doesnt exist
        if ( ( m_currentMode != LocationCache::Depot ) &&
             ( m_outboundList != NULL ) )
        {
            CommonTypes::TrainIdList outBoundTrains = m_outboundList->getSelectedTrainIds();

            // Merge the two vectors - into inBoundTrains
			for ( CommonTypes::TrainIdList::iterator iter = outBoundTrains.begin();
				  iter != outBoundTrains.end(); ++iter)
			{
					  inBoundTrains.insert( *iter );
			}
        }

        FUNCTION_EXIT;
        return inBoundTrains;
    }


    TrainInformationTypes::TrainDetailsList TrainSelectorGUI::getSelectedTrainData() const
    {
        FUNCTION_ENTRY( "TrainSelectorGUI::getSelectedTrainData" );
        
        TrainInformationTypes::TrainDetailsList inBoundTrains
            = m_inboundList->getSelectedTrainData();

        // if in mainline mode, and the second list exists
        // then merge the selections from the second list into those from the first

        // ignore the second list in depot mode (selections will be mirrored)
        // and skip this if the second list doesnt exist

        if ( (m_currentMode != LocationCache::Depot) &&
             (m_outboundList != NULL) )
        {
            TrainInformationTypes::TrainDetailsList outBoundTrains
                = m_outboundList->getSelectedTrainData();

            // Merge the two vectors - into inBoundTrains
            inBoundTrains.insert( inBoundTrains.end(), 
                                  outBoundTrains.begin(),
                                  outBoundTrains.end() );
        }

        FUNCTION_EXIT;
        return inBoundTrains;
    }


    void TrainSelectorGUI::selectAllInbound()
    {
		FUNCTION_ENTRY( "selectAllInbound" );

		if ( m_inboundList != NULL )
		{
			m_inboundList->selectAll();
			// selection change event will be received, no need to enable/disable buttons
		}

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::selectAllOutbound()
    {
		FUNCTION_ENTRY( "selectAllOutbound" );

        if (m_outboundList != NULL)
        {
            m_outboundList->selectAll();
            // selection change event will be received, no need to enable/disable buttons
        }

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::selectAllTrains()
    {
		FUNCTION_ENTRY( "selectAllTrains" );

        selectAllInbound();
        selectAllOutbound();

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::clearSelection()
    {
		FUNCTION_ENTRY( "clearSelection" );
		
        m_inboundList->deselectAll();
        if (m_outboundList != NULL)
        {
            m_outboundList->deselectAll();
        }
        // selection change event will be received, no need to enable/disable buttons

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::saveTrainSelection()
    {
		FUNCTION_ENTRY( "saveTrainSelection" );

        TA_ASSERT( m_trainGroupCombo != NULL,
                   "Received a save train selection for a NULL combo box...");
        TA_ASSERT( m_trainSelector != NULL,
                   "If the train selector is not set, this item should be disabled.");

        std::string dynamicGroupName = m_trainGroupCombo->getCurrentGroupName();

        // if a valid group name is in the box
        if ( dynamicGroupName != "" )
        {
            // prompt first if the group exists
            if ( m_trainGroupCombo->currentGroupNameIsAnExistingGroup() )
            {
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << dynamicGroupName;
				UINT ret = userMsg.showMsgBox(IDS_UW_650003);
                if ( ret != IDYES)
                {
                    FUNCTION_EXIT;
                    return;
                }
             }

            // get the selected trains
			CommonTypes::TrainIdList trains = getSelectedTrains();

            // get the session Id
            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

            try
            {
                // attempt to save the group
                m_trainSelector->saveTrainSelection( dynamicGroupName, trains, sessionId );
            }
			catch( const AgentCommunicationException& e )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AgentcommunicationException", e.what() );

				m_trainGroupCombo->SetCurSel( -1 );

				// enable/disable train group controls
				enableGroupControls();
				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << std::string( "save" );
				userMsg.showMsgBox(IDS_UE_070090);
			}
			catch( const TrainAccessDeniedException& e )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TrainAccessDeniedException", e.what() );
				
				m_trainGroupCombo->SetCurSel( -1 );
				
				// enable/disable train group controls
				enableGroupControls();
				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << std::string( "Access Denied" );
				userMsg.showMsgBox(IDS_UE_650011);
			}
        }

    }


    void TrainSelectorGUI::deleteTrainSelection()
    {
        FUNCTION_ENTRY( "deleteTrainSelection" );

        TA_ASSERT( m_trainGroupCombo != NULL,
                   "Received a delete train selection for a NULL combo box...");
        TA_ASSERT( m_trainSelector != NULL,
                   "If the train selector is not set, this item should be disabled.");

        // if a valid group is selected
        if ( m_trainGroupCombo->currentGroupNameIsAnExistingGroup() )
        {

            // get the selected train group
            std::string dynamicGroupName = m_trainGroupCombo->getCurrentGroupName();
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << dynamicGroupName;
			UINT ret = userMsg.showMsgBox(IDS_UW_650004);
            if( ret != IDYES)
            {
                FUNCTION_EXIT;
                return;
            }

            // get the session Id
            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

            try
            {
                // attempt to delete the group
                m_trainSelector->deleteTrainSelection(dynamicGroupName, sessionId);

                // successfully deleted, just remove the text in the box
                m_trainGroupCombo->SetCurSel(-1);
                m_trainGroupCombo->SetWindowText("");
            }
			catch( const AgentCommunicationException& e )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AgentcommunicationException", e.what() );
				
				m_trainGroupCombo->SetCurSel( -1 );
				
				// enable/disable train group controls
				enableGroupControls();

				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << std::string( "delete" );
				userMsg.showMsgBox(IDS_UE_070090);
			}
			catch( const TrainAccessDeniedException& e )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TrainAccessDeniedException", e.what() );
				
				m_trainGroupCombo->SetCurSel( -1 );
				
				// enable/disable train group controls
				enableGroupControls();

				userMsg << std::string( "Access Denied" );
				userMsg.showMsgBox(IDS_UE_650012);
			}
        }

        FUNCTION_EXIT;
    }


    void TrainSelectorGUI::selectionChangedGroupCombo()
    {
        FUNCTION_ENTRY( "selectionChangedGroupCombo" );

        TA_ASSERT( m_trainGroupCombo != NULL,
                   "Received a selection changed for a NULL combo box...");
        TA_ASSERT( m_trainSelector != NULL,
                   "If the train selector is not set, this item should be disabled.");

        // if a valid group was selected (as opposed to the user typing a new name)
        // update the selection with that group
        if ( m_trainGroupCombo->currentGroupNameIsAnExistingGroup() )
        {
            // get the selected group
            std::string dynamicGroupName = m_trainGroupCombo->getCurrentGroupName();

            // update the train selection using the selected group
            try
            {
				CommonTypes::TrainIdList trainSelData = m_trainSelector->getTrainSelection(dynamicGroupName);
                
                // Update the selection     
                m_inboundList->setSelectedTrains(trainSelData);

                if (m_outboundList != NULL)
                {
                    m_outboundList->setSelectedTrains(trainSelData);
                }

            }
            catch ( const InvalidSelectionListException& e )
            {
                // a train selected is invalid
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "InvalidSelectionList", e.what() );

                // just deselect the item and throw the exception on.
                m_trainGroupCombo->SetCurSel(-1);

				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << std::string( "load" );
				userMsg.showMsgBox(IDS_UE_070092);
            }
			catch( const AgentCommunicationException& e )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "AgentcommunicationException", e.what() );
				
				m_trainGroupCombo->SetCurSel( -1 );
				
				// enable/disable train group controls
				enableGroupControls();
				TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << std::string( "load" );
				userMsg.showMsgBox(IDS_UE_070090);
			}
            catch ( const TA_Base_Core::TransactiveException& te )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TransactiveException", te.what() );

                // Probably failed to resolve CORBA object

                // just deselect the item and throw the exception on.
                m_trainGroupCombo->SetCurSel(-1);

				/*AfxMessageBox("Error connecting to Train Agent. Unable to retrieve train selection.", MB_OK | MB_ICONSTOP);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << te.what();
				userMsg.showMsgBox(IDS_UE_650014);
            }

        }

        // finally, update the group controls
        enableGroupControls();

        FUNCTION_EXIT;
    }


    void TrainSelectorGUI::editChangedGroupCombo()
    {
        FUNCTION_ENTRY( "editChangedGroupSelection" );

        TA_ASSERT( m_trainGroupCombo != NULL,
                   "Received an edit changed for a NULL combo box...");

        TA_ASSERT( m_trainSelector != NULL,
                   "If the train selector is not set, this item should be disabled.");

        // update the group controls
        enableGroupControls();

        FUNCTION_EXIT;
    }





    void TrainSelectorGUI::updateInboundList( ITrainSelectorGUI::EAction action,
											  const TrainInformationTypes::TrainDetails& trainDetails )
    {
		FUNCTION_ENTRY( "updateInboundList" );

        // update the inbound list
        m_inboundList->updateTrainList( action, trainDetails );

        // if in depot mode, both lists will be the same
        if ( ( m_outboundList != NULL) &&
             ( m_currentMode == LocationCache::Depot ) )
        {
            m_outboundList->updateTrainList( action, trainDetails );

            // check list sizes and enable scroll bar only if there are enough items
            // also only pad the list if it is needed.
            bool enable = m_inboundList->GetItemCount() >
                          ( m_inboundList->GetCountPerPage() + m_outboundList->GetCountPerPage() );
            if (enable)
            {
                m_outboundList->removePadding();
            }
            else
            {
                m_outboundList->padList();
            }

            m_inboundList->setScrollBarVisibility( enable );
            m_outboundList->setScrollBarVisibility( enable );
        }

        // scroll positions
        positionListItems( m_inboundList );

        // enable/disable buttons
        enableButtons();

        // enable/disable train group controls
        enableGroupControls();

        // notify the observer
        notifySelectionChanged();

		FUNCTION_EXIT;
    }

        
    void TrainSelectorGUI::updateOutboundList( ITrainSelectorGUI::EAction action,
											   const TrainInformationTypes::TrainDetails& trainDetails )
    {
		FUNCTION_ENTRY( "updateOutboundList" );

        if ( m_currentMode != LocationCache::Depot )
        {
            // not in depot mode
            if ( m_outboundList != NULL )
            {
                // update the outbound list if there is one
                m_outboundList->updateTrainList( action, trainDetails );
            }
            else
            {
                // otherwise - there is no outbound list
                // update the inbound list with the outbound trains
                m_inboundList->updateTrainList( action, trainDetails );
            }
        }
        // depot mode is active
        // update both the inbound and outbound lists
        else
        {
            m_inboundList->updateTrainList( action, trainDetails );
            if (m_outboundList != NULL)
            {
                m_outboundList->updateTrainList( action, trainDetails );

                // check list sizes and enable scroll bar only if there are enough items
                // also only pad the list if it is needed.
                bool enable = m_inboundList->GetItemCount() >
                              ( m_inboundList->GetCountPerPage() + m_outboundList->GetCountPerPage() );
                if (enable)
                {
                    m_outboundList->removePadding();
                }
                else
                {
                    m_outboundList->padList();
                }

                m_inboundList->setScrollBarVisibility( enable );
                m_outboundList->setScrollBarVisibility( enable );
            }
        }

        // scroll positions
        positionListItems( m_inboundList );

        // enable/disable buttons
        enableButtons();

        // enable/disable train group controls
        enableGroupControls();

        // notify the observer
        notifySelectionChanged();

		FUNCTION_EXIT;
    }


	void TrainSelectorGUI::updateFallbackStatus( bool inFallback )
    {
		FUNCTION_ENTRY( "updateFallbackStatus" );
		if ( m_isInRadioFallback != inFallback )
		{
			//m_isInRadioFallback = !inFallback is right ???
			m_isInRadioFallback = !inFallback;
			enableGroupControls();

            if ( ( true == inFallback ) &&
                 ( NULL != m_trainStaticGroupCombo ) )
            {
                // select the blank item
                m_trainStaticGroupCombo->SetCurSel( 0 );
            }
		}
		
		FUNCTION_EXIT;
    }

        
    void TrainSelectorGUI::updateSelectionCombo( const TrainInformationTypes::TrainSelectionList& trainListNames )
    {
		FUNCTION_ENTRY( "updateSelectionCombo" );

        if ( m_trainGroupCombo != NULL )
        {
            m_trainGroupCombo->updateGroupNames( trainListNames );

            // enable/disable train group controls
            enableGroupControls();
        }

		FUNCTION_EXIT;
    }


	bool TrainSelectorGUI::hasRadioGroupSupport()
    {
		FUNCTION_ENTRY( "hasRadioGroupSupport");
		FUNCTION_EXIT;
        return ( m_trainStaticGroupCombo != NULL );
	
    }


	void TrainSelectorGUI::updateRadioGroupCombo(const std::vector<std::string>& trainListNames)
    {
		FUNCTION_ENTRY( "updateRadioGroupCombo" );

		if( NULL != m_trainStaticGroupCombo )
		{
			m_trainStaticGroupCombo->updateGroupNames( trainListNames );
		}
		
		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::enableRadioGroupCombo( bool enable )
    {
		FUNCTION_ENTRY( "enableRadioGroupCombo" );

		if( enable != m_enableStaticGroupCombo )
		{
			m_enableStaticGroupCombo = enable;

            enableGroupControls();
		}
		
		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::listScrolledV( TrainListCtrl* list,
                                          UINT nSBCode,
                                          UINT nPos,
                                          CScrollBar *pScrollBar )
    {
		FUNCTION_ENTRY( "listScrolledV" );

        // if this is not in depot mode - just allow the list to scroll
        if ( ( m_currentMode != LocationCache::Depot ) ||
             ( m_outboundList == NULL ) )
        {
            list->redirectedVScroll( nSBCode, nPos, pScrollBar );
        }
        else
        {
            // if the scroll direction is up, scroll the first list
            // and match the second
            
            // if the scroll direction is down, scroll the second list
            // and match the first

            switch (nSBCode)
            {
            case SB_BOTTOM:
            case SB_LINEDOWN:
            case SB_PAGEDOWN:
                m_outboundList->redirectedVScroll( nSBCode, nPos, pScrollBar );
                positionListItems( m_outboundList );
                break;
            case SB_TOP:
            case SB_LINEUP:
            case SB_PAGEUP:
                m_inboundList->redirectedVScroll( nSBCode, nPos, pScrollBar );
                positionListItems( m_inboundList );
                break;
            case SB_THUMBPOSITION:
            case SB_THUMBTRACK:
                // nPos is simply the 'proposed' first item
                if (list == m_inboundList)
                {
                    // make sure it doesnt get scrolled down too far
                    int maxPos = m_inboundList->GetItemCount() - 
                                 m_inboundList->GetCountPerPage() -
                                 m_outboundList->GetCountPerPage();
                    if ( maxPos >= 0 && nPos <= static_cast< unsigned int >( maxPos ) )
                    {
                        list->redirectedVScroll(nSBCode, nPos, pScrollBar);
                        positionListItems(list);
                    }
                }
                else if (list == m_outboundList)
                {
                    // make sure it doesnt get scrolled up too far
                    unsigned int minPos = m_inboundList->GetCountPerPage();
                    if (nPos >= minPos)
                    {
                        list->redirectedVScroll( nSBCode, nPos, pScrollBar );
                        positionListItems( list );
                    }
                }
                break;
            case SB_ENDSCROLL:
                list->redirectedVScroll( nSBCode, nPos, pScrollBar );
                positionListItems( list );
                break;
            }
        }

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::onKeyDown( TrainListCtrl* list,
                                      UINT nChar,
                                      UINT nRepCnt,
                                      UINT nFlags )
    {
		FUNCTION_ENTRY( "onKeyDown" );

        // if this is not in depot mode - just allow the list to scroll
        if ( ( m_currentMode != LocationCache::Depot) ||
             ( m_outboundList == NULL) )
        {
            list->redirectedKeyDown( nChar, nRepCnt, nFlags );
        }
        else
        {
            // dont pass on arrow keys
            // but let everything else through
            switch (nChar)
	        {
		    case VK_UP:
            case VK_DOWN:
            case VK_LEFT:
            case VK_RIGHT:
            case VK_HOME:
            case VK_END:
            case VK_PRIOR:
            case VK_NEXT:
                break;
            default:
                list->redirectedKeyDown( nChar, nRepCnt, nFlags );
                break;
	        }
        }

		FUNCTION_EXIT;
    }



    BOOL TrainSelectorGUI::onMouseWheel( TrainListCtrl* list,
                                         UINT nFlags,
                                         short zDelta,
                                         CPoint pt )
    {
		FUNCTION_ENTRY( "onMouseWheel" );

        // if this is not in depot mode - just allow the list to scroll
        if ( ( m_currentMode != LocationCache::Depot ) ||
             ( m_outboundList == NULL ) )
        {
			FUNCTION_EXIT;
            return list->redirectedOnMouseWheel( nFlags, zDelta, pt );
        }
        else
        {
            BOOL retval = TRUE;

            // only allow mouse wheel if there are enough items
            if ( m_inboundList->GetItemCount() >
                 ( m_inboundList->GetCountPerPage() + m_outboundList->GetCountPerPage() ) )
            {
                // if scrolling down - scroll the second list
                if ( zDelta < 0 )
                {
                    retval = m_outboundList->redirectedOnMouseWheel( nFlags, zDelta, pt );
                    // then make the first list match
                    positionListItems(m_outboundList);
                }
                // if scrolling up scroll the first list
                else if ( zDelta > 0 )
                {
                    retval = m_inboundList->redirectedOnMouseWheel( nFlags, zDelta, pt );
                    // then make the second list match
                    positionListItems( m_inboundList );
                }
            }

			FUNCTION_EXIT;
            return retval;
        }
		
		FUNCTION_EXIT;
        return TRUE;
    }


    void TrainSelectorGUI::onColumnclick( TrainListCtrl* list,
                                          NMHDR* pNMHDR,
                                          LRESULT* pResult )
    {
		FUNCTION_ENTRY( "onColumnclick" );

        // makes sure both lists sort the same way
        // pass the click on
        m_inboundList->redirectedColumnclick( pNMHDR, pResult );

        // propagate the sort on
        if (m_outboundList != NULL)
        {
            m_outboundList->redirectedColumnclick( pNMHDR, pResult );
            
            // re-sync list positions
            positionListItems( m_inboundList );
        }

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::onClick( TrainListCtrl* list,
                                    NMHDR* pNMHDR,
                                    LRESULT* pResult )
    {
		FUNCTION_ENTRY( "onClick" );

        LPNMITEMACTIVATE lpnmitem = reinterpret_cast<LPNMITEMACTIVATE>( pNMHDR );
        TrainListCtrl* otherList = NULL;
        if ( list == m_inboundList )
        {
            otherList = m_outboundList;
        }
        else if ( list == m_outboundList )
        {
            otherList = m_inboundList;
        }

        if ( m_currentMode != LocationCache::Depot )
        {
            // If user clicks on inbound list without holding ctrl down, clear outbound list selection
            if ( !( lpnmitem->uKeyFlags & LVKF_CONTROL) &&
                 ( otherList != NULL ) )
            {
                otherList->deselectAll();
            }
        }
        else
        {
            // this is so you can shift+select across lists
            otherList->SetSelectionMark( list->GetSelectionMark() );

            // get the selected items from this list,
            // and set them in the other list
            otherList->setSelectedTrains( list->getSelectedTrainIds() );
        }
        // selection change event will be received, no need to enable/disable buttons

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::onItemchanged( TrainListCtrl* list,
                                          NMHDR* pNMHDR,
                                          LRESULT* pResult )
    {
		FUNCTION_ENTRY( "onIsSelectable" );
		
        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
        // Only interested in state changes (ie selection)
	    if (pNMListView->uChanged != LVIF_STATE) 
	    {   
			FUNCTION_EXIT;
		    return;
	    }

        // enable/disable train group controls
        enableGroupControls();

        // notify the observer
        notifySelectionChanged();

		FUNCTION_EXIT;
    }

   
    bool TrainSelectorGUI::onIsSelectable( TrainListCtrl* list,
                                           CommonTypes::TrainIdType trainId,
                                           unsigned long trainLocation, 
                                           int rowNumber,
                                           unsigned long trainCommunicationLocation ) //limin++, ECP-338 CL-20150
    {
		FUNCTION_ENTRY( "onIsSelectable" );
		FUNCTION_EXIT;
        return LocationCache::getInstance().isInControlOfLocation( trainLocation, trainCommunicationLocation );
    }


	void TrainSelectorGUI::editChangedStaticGroupCombo()
    {
		FUNCTION_ENTRY( "selectionChangedStaticGroupCombo" );
        //The selection of static groups has changed, do nothing for now.
		FUNCTION_EXIT;
		return;
		
    }
	
	
    void TrainSelectorGUI::selectionChangedStaticGroupCombo()
    {
		FUNCTION_ENTRY( "selectionChangedStaticGroupCombo" );
        //The selection of static groups has changed, do nothing for now.
		FUNCTION_EXIT;
		return;
		
    }
    

    void TrainSelectorGUI::showInboundOutboundControls( bool show )
    {
		FUNCTION_ENTRY( "showInboundOutboundControls" );

        // show or hide depending on the parameter
        int command = show ? SW_SHOW : SW_HIDE;

        // buttons dealing with in and outbound
        if ( m_allInbound != NULL )
        {
            m_allInbound->ShowWindow( command );
        }
        if ( m_allOutbound != NULL )
        {
            m_allOutbound->ShowWindow( command );
        }

        // labels dealing with in and outbound
        if ( m_inboundLabel != NULL )
        {
            m_inboundLabel->ShowWindow( command );
        }
        if ( m_outboundLabel != NULL )
        {
            m_outboundLabel->ShowWindow( command );
        }

        // if the buttons are being shown, enable/disable them accordingly
        if ( show )
        {
            enableButtons();
        }

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::enableButtons()
    {
		FUNCTION_ENTRY( "enableButtons" );

        // The all inbound button should be enabled if there are items
        // in the inbound list
        if ( m_allInbound != NULL )
        {
            m_allInbound->EnableWindow( m_trainListsEnabled &&
                                        m_inboundList->GetItemCount() > 0);
        }

        // The all outbound button should be enabled if there are items
        // in the outbound list
        if (m_allOutbound != NULL)
        {
            bool enable = m_trainListsEnabled &&
                          ( m_outboundList != NULL ) &&
                          ( m_outboundList->GetItemCount() > 0 );

            m_allOutbound->EnableWindow(enable);
        }

        // The all trains button should be enabled if there are items
        // in the inbound or outbound list
        // the clear all button should be enabled if the select all button is

        bool enable = m_trainListsEnabled &&
                      ( m_inboundList->GetItemCount() > 0 );
        
        // if not in depot mode count the second list too
        if ( m_currentMode != LocationCache::Depot )
        {
            enable = enable || 
                    ( ( m_outboundList != NULL ) && ( m_outboundList->GetItemCount() > 0 ) );
        }


        if (m_allTrains != NULL)
        {
            m_allTrains->EnableWindow( enable );
        }

        if (m_clearAllTrains != NULL)
        {
            m_clearAllTrains->EnableWindow( enable );
        }
		
		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::enableGroupControls()
    {
		FUNCTION_ENTRY( "enableGroupControls" );

        bool m_trainSelectorConfigured = ( m_trainSelector != NULL );

        // enable/disable the combo box
        if ( m_trainGroupCombo != NULL )
        {
            m_trainGroupCombo->EnableWindow( m_trainSelectorConfigured &&
                                             m_trainListsEnabled &&
                                             m_trainGroupsEnabled );
        }
		
        // the group can be saved if
        bool saveEnabled = m_trainSelectorConfigured && // there is access to a train selector
                           m_trainListsEnabled &&
                           m_trainGroupsEnabled &&  // groups are enabled
                           m_trainGroupSaveEnabled &&   // there is permission to save
                           ( m_trainGroupCombo->getCurrentGroupName() != "" ) &&  // there is a name in the combo box
                           ( getSelectedTrains().size() > 0 );    // there are trains selected

        if ( m_saveGroup != NULL )
        {
            m_saveGroup->EnableWindow( saveEnabled );
        }

        // the group can be deleted if
        bool deleteEnabled = m_trainSelectorConfigured && // there is access to a train selector
                             m_trainListsEnabled &&
                             m_trainGroupsEnabled &&  // groups are enabled
                             m_trainGroupDeleteEnabled &&   // there is permission to delete
                             m_trainGroupCombo->currentGroupNameIsAnExistingGroup();  // there is a valid name in the combo box

        if (m_deleteGroup != NULL)
        {
            m_deleteGroup->EnableWindow( deleteEnabled );
        }

		// the static group box can be enabled if
        bool staticGroupEnabled = m_trainSelectorConfigured && // there is access to a train selector
                                  m_enableStaticGroupCombo && // application permission
								  m_trainListsEnabled &&    // train lists are enabled
								  ( false == m_isInRadioFallback ); // there is no radio fallback
		
        if ( m_trainStaticGroupCombo != NULL )
        {
            m_trainStaticGroupCombo->EnableWindow( staticGroupEnabled );
        }
		
		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::notifySelectionChanged()
    {
		FUNCTION_ENTRY( "notifySelectionChanged" );

		CommonTypes::TrainIdList selectedTrains = getSelectedTrains();
        bool selectionExists = selectedTrains.size() > 0;
		
        if ( m_trainsAreSelected != selectionExists )
        {
            m_trainsAreSelected = selectionExists;
            if ( m_selectionListener != NULL )
            {
                // tell it whether trains are selected
                m_selectionListener->trainSelectionChanged( m_trainsAreSelected );
            }
        }
		
		if ( m_trainSelector != NULL )
		{
			// notify on selection change
			m_trainSelector->trainSelectionChanged( m_trainsAreSelected );
		}

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::clearData()
    {
		FUNCTION_ENTRY( "clearData" );

        // reset train lists and selection combo
		TrainInformationTypes::TrainDetails dummy;
        updateInboundList( ITrainSelectorGUI::ClearAll, dummy );
        updateOutboundList( ITrainSelectorGUI::ClearAll, dummy );

        if (m_trainGroupCombo != NULL)
        {
            m_trainGroupCombo->SetCurSel(-1);
            m_trainGroupCombo->SetWindowText( "" );
        }

        // enable/disable buttons
        enableButtons();

        // enable/disable train group controls
        enableGroupControls();

		FUNCTION_EXIT;
    }


    void TrainSelectorGUI::positionListItems( TrainListCtrl* referenceList )
    {
		FUNCTION_ENTRY( "positionListItems" );

        if ( ( m_outboundList != NULL ) &&
             ( m_currentMode == LocationCache::Depot ) )
        {
            if ( referenceList == m_inboundList )
            {
                // the outbound list moves

                // get the top most item position for the second list
                int firstItem = m_inboundList->GetTopIndex() + m_inboundList->GetCountPerPage();

                // get the maximum first item the second list can scroll to
                int maximumFirstItem = m_inboundList->GetItemCount() - m_outboundList->GetCountPerPage();

                // get the scroll amount for the second list
                int itemsToScroll = firstItem - m_outboundList->GetTopIndex();

                // the amount to scroll
                RECT windowSize;
                m_outboundList->GetClientRect( &windowSize );

                CSize size;
                size.cx = 0;
                size.cy = ( windowSize.bottom / m_outboundList->GetCountPerPage() ) * itemsToScroll;
            
                m_outboundList->Scroll( size );
            }
            else
            {
                // the inbound list moves

                // get the top most item position for the first list
                int firstItem = m_outboundList->GetTopIndex() - m_inboundList->GetCountPerPage();

                // get the scroll amount for the second list
                int itemsToScroll = firstItem - m_inboundList->GetTopIndex();

                // the amount to scroll
                RECT windowSize;
                m_inboundList->GetClientRect( &windowSize );

                CSize size;
                size.cx = 0;
                size.cy = ( windowSize.bottom / m_inboundList->GetCountPerPage() ) * itemsToScroll;
                m_inboundList->Scroll( size );
            }
        }

		FUNCTION_EXIT;
    }


} // TA_IRS_Bus

