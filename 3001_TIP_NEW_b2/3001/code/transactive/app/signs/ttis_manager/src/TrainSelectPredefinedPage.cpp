/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainSelectPredefinedPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the page that has the normal and emergency
  * pre-defined messages on it.
  *
  */


#include "stdafx.h"
#include "TTISManager.h"
#include "TrainSelectPredefinedPage.h"
#include "TTISPredefinedMessages.h"
#include "TTISMessageManager.h"
#include "DisplayAttributeManager.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

// owning page
#include "TrainDisplayPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::DataConfigurationException;
using TA_Base_Core::PredefinedMessage;


namespace TA_IRS_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CTrainSelectPredefinedPage dialog


	CTrainSelectPredefinedPage::CTrainSelectPredefinedPage(CWnd* pParent /*=NULL*/)
	: CDialog(CTrainSelectPredefinedPage::IDD, pParent),
	  m_messageSelectionListener(NULL),
      m_ttisPredefinedMessages( NULL )
	{
		//{{AFX_DATA_INIT(CTrainSelectPredefinedPage)
		//}}AFX_DATA_INIT
	}


	void CTrainSelectPredefinedPage::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CTrainSelectPredefinedPage)
		DDX_Control(pDX, IDC_EMERGENCY_PREDEFINED, m_emergencyList);
		DDX_Control(pDX, IDC_LANGUAGES, m_languages);
		DDX_Control(pDX, IDC_SEARCH_EMERGENCY, m_searchEmergency);
		DDX_Control(pDX, IDC_SEARCH_NORMAL, m_searchNormal);
		DDX_Control(pDX, IDC_TRAIN_PREDEFINED_CONTENT, m_predefinedContent);
		DDX_Control(pDX, IDC_TRAIN_NORMAL_PREDEFINED, m_normalList);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CTrainSelectPredefinedPage, CDialog)
		//{{AFX_MSG_MAP(CTrainSelectPredefinedPage)
		ON_EN_UPDATE(IDC_SEARCH_EMERGENCY, OnUpdateSearchEmergency)
		ON_EN_UPDATE(IDC_SEARCH_NORMAL, OnUpdateSearchNormal)
		ON_NOTIFY(NM_CLICK, IDC_EMERGENCY_PREDEFINED, OnClickEmergencyPredefined)
		ON_NOTIFY(NM_CLICK, IDC_TRAIN_NORMAL_PREDEFINED, OnClickNormalPredefined)
		ON_WM_SHOWWINDOW()
		ON_MESSAGE(WM_UPDATE_CURRENT_TTIS_VERSION, OnUpdateCurrentTTISVersion)
		ON_WM_DESTROY()
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRAIN_NORMAL_PREDEFINED, onItemchangedTrainNormalPredefined)
		ON_NOTIFY(LVN_ITEMCHANGED, IDC_EMERGENCY_PREDEFINED, onItemchangedEmergencyPredefined)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CTrainSelectPredefinedPage message handlers


	BOOL CTrainSelectPredefinedPage::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void CTrainSelectPredefinedPage::Init()
	{
		// extended styles (including infotips)
		// column widths for message titles
		m_emergencyList.SetExtendedStyle(m_emergencyList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
		m_normalList.SetExtendedStyle(m_normalList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

		RECT rect;
		m_emergencyList.GetClientRect(&rect);
		m_emergencyList.InsertColumn(0,"Message Name");
		m_emergencyList.SetColumnWidth(0, rect.right);
		m_normalList.GetClientRect(&rect);
		m_normalList.InsertColumn(0,"Message Name");
		m_normalList.SetColumnWidth(0, rect.right);

        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }

		// load the pre-defined messages into the lists
		populateLists();

		// register with the pre-defined message singleton to be updated of a change
		m_ttisPredefinedMessages->registerCurrentVersionUser(this);

		// set the available languages box to empty
		m_languages.SetWindowText("");

		m_emergencyList.setScrollBarVisibility(true);
		m_normalList.setScrollBarVisibility(true);
	}

	void CTrainSelectPredefinedPage::OnDestroy() 
	{
		CDialog::OnDestroy();
		
        if ( NULL != m_ttisPredefinedMessages )
        {
            // deregister for pre-defined message library version update
            m_ttisPredefinedMessages->deregisterCurrentVersionUser(this);

            m_ttisPredefinedMessages = NULL;
            
            TTISPredefinedMessages::removeInstance();
        }
	}


	/** 
	  * OnUpdateCurrentTTISVersion
	  *
	  * When it is discovered that the pre-defined message library has been upgraded,
	  * we get this message. This rebuilds the list of pre-defined messages from the
	  * list held in the singleton.
	  *
	  * @param wParam
	  * @param lParam
	  *
	  * @return 
	  *
	  */
	LRESULT CTrainSelectPredefinedPage::OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam)
	{
		// rebuild the lists
		populateLists();

		// update all the stuff to do with selected items in lists
		updatePreviewArea();

		return 0;
	}


	/** 
	  * hasValidSelection
	  *
	  * test if a message is selected.
	  *
	  * @return true if a valid pre-defined message is selected
	  */
	bool CTrainSelectPredefinedPage::hasValidSelection() const
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
		// get the selected message
		// NULL means no selection
		const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();

		if (messagePointer != NULL)
		{
			LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
			return true;
		}

		LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
		return false;
	}


	/** 
	  * getSelectedMessage
	  *
	  * Retrieves the selected pre-defined message.
	  *
	  * @see hasValidSelection
	  *
	  * @return The selected pre-defined message
	  */
	TA_Base_Core::PredefinedMessage CTrainSelectPredefinedPage::getSelectedMessage() const
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectPredefinedPage::getSelectedMessage" );
		const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();

		TA_ASSERT (messagePointer != NULL,
				   "CTrainSelectPredefinedPage::getSelectedMessage() called when there is no valid message selected. Call hasValidSelection() first." );

		LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::getSelectedMessage" );
		return *messagePointer;
	}


	/** 
	  * populateLists
	  *
	  * Populates the pre-defined message lists.
	  *
	  */
	void CTrainSelectPredefinedPage::populateLists()
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectPredefinedPage::populateLists" );

		// clear the list of pre-defined messages
		m_normalList.DeleteAllItems();
		m_emergencyList.DeleteAllItems();
    
		// Load the predefined message library

		// get the TTIS message libraries
		std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
				m_ttisPredefinedMessages->getNormalMessages();
			std::vector<TA_Base_Core::PredefinedMessage> emergencyMessages =
				m_ttisPredefinedMessages->getEmergencyMessages();
   
    
		std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter;

		int count = 0;
		//Bohong++ for CL-20682 
		if (0 == normalMessages.size())
		{
			m_normalList.InsertItem(count, "No Predefined Message available");
			m_normalList.EnableWindow(false);
		}
		//++Bohong
		for(messageIter = normalMessages.begin();
		messageIter != normalMessages.end(); messageIter++)
		{
			m_normalList.EnableWindow(true);
			// add the string to the list
			int index = m_normalList.InsertItem(count, messageIter->description.c_str());
			m_normalList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag) );
			count++;
		}

		count = 0;
		//Bohong++ for CL-20682
		if (0 == emergencyMessages.size())
		{
			m_emergencyList.InsertItem(count, "No Predefined Message available");
			m_emergencyList.EnableWindow(false);
		}
		//++Bohong
		for(messageIter = emergencyMessages.begin();
		messageIter != emergencyMessages.end(); messageIter++)
		{
			m_emergencyList.EnableWindow(true);
			// add the string to the list
			int index = m_emergencyList.InsertItem(count, messageIter->description.c_str());
			m_emergencyList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag) );
			// set the emergency colour to red
			m_emergencyList.setItemColour( count,
										   COLORREF( RGB( 255, 0, 0 ) ),
										   ListCtrlSelNoFocus::I_INDEX );
			count++;
		}

		LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::populateLists" );
	}


	/** 
	  * getSelectedMessageData
	  *
	  * Retrieves the selected pre-defined message.
	  *
	  *
	  * @return The selected pre-defined message (pointer)
	  *         Null if nothing valid is selected.
	  */
	const TA_Base_Core::PredefinedMessage* CTrainSelectPredefinedPage::getSelectedMessageData() const
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectPredefinedPage::getSelectedMessageData" );
		const PredefinedMessage* message = NULL;

		// get the selected index for the normal priority list
		int normalSelected = -1;
		POSITION currSel = m_normalList.GetFirstSelectedItemPosition();
    
		// if something is selected
		if(currSel != NULL)
		{
			// get the selected index
			normalSelected = m_normalList.GetNextSelectedItem(currSel);
		}

		// get the index for the emergency priority list
		currSel = NULL;
		int emergencySelected = -1;
		currSel = m_emergencyList.GetFirstSelectedItemPosition();

		// if something is selected
		if(currSel != NULL)
		{
			// get the selected index
			emergencySelected = m_emergencyList.GetNextSelectedItem(currSel);
		}

		// if a normal message is selected
		if( normalSelected > -1 )
		{
			// get the data for the selected item
			unsigned short itemData = static_cast<unsigned short> (m_normalList.GetItemData(normalSelected));
    
			// find the appropriate message
			message = m_ttisPredefinedMessages->getNormalMessageById( itemData );
        
			// anything selected should point to a valid message
			TA_ASSERT( message != NULL,
				"The selected message is not a valid message");
		}
		// otherwise if an emergency message is selected
		else if (emergencySelected > -1)
		{
			// get the text for the selected item
			unsigned short itemData = static_cast<unsigned short> (m_emergencyList.GetItemData(emergencySelected));

			// find the appropriate message
			message = m_ttisPredefinedMessages->getEmergencyMessageById( itemData );

			// anything selected should point to a valid message
			TA_ASSERT( message != NULL,
				"The selected message is not a valid message");
        
		}
		// otherwise, return NULL

		LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::getSelectedMessageData" );
		return message;
	}


	/** 
	  * updatePreviewArea
	  *
	  * updates the message preview area with the text from the selected message.
	  * also updates the languages box.
	  *
	  */
	void CTrainSelectPredefinedPage::updatePreviewArea()
	{
		// get the selected message
		const TA_Base_Core::PredefinedMessage* message = getSelectedMessageData();

    // notify the owning window
//    CWnd* parent = GetParent();
//    // down cast to the right window type
//    CTrainDisplayPage* trainDisplayPage = dynamic_cast<CTrainDisplayPage*>(parent);

    // NULL means nothing is selected
    if (message != NULL)
    {
        // set the message text
        m_predefinedContent.SetWindowText(message->message.c_str());
        
        // set the english only flag
        if (message->englishOnly)
        {
            m_languages.SetWindowText("English Only");
        }
        else
        {
            m_languages.SetWindowText("All Languages");
        }

        // notify the owning window
        if (/*trainDisplayPage*/ m_messageSelectionListener != NULL)
        {
            DisplayAttributeManager* displayAttributeManager = NULL;

            // get the object managing display attributes
            displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();

            // it can be null before initialised
            if (displayAttributeManager != NULL)
            {
                // disable the combo selection
                displayAttributeManager->enableTimeControls(false);

                // set the priority
                displayAttributeManager->setPriority( message->priority, false);

                // set the repeat interval
                displayAttributeManager->setRepeatInterval( message->repeatInterval, false);

                // a message is selected
                m_messageSelectionListener->messageSelectionChanged(true);
            }
        }
    }
    else
    {
        // clear the text in the preview window
        m_predefinedContent.SetWindowText("");
        // clear the language field
        m_languages.SetWindowText("");

        // notify the owning window
        if (m_messageSelectionListener  != NULL)
        {
            DisplayAttributeManager* displayAttributeManager = NULL;

            // get the object managing display attributes
            displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();

            // get the object managing display attributes
            displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();

            if (displayAttributeManager != NULL)
            {
                // clear and disable the time controls
                // and the priority combo
                displayAttributeManager->blankAndDisableTimeAndPriority();

                // a message is not selected
                m_messageSelectionListener->messageSelectionChanged(false);
            }
        }
    }
}


	void CTrainSelectPredefinedPage::windowShown()
	{
		// update preview area, and owning page, if the window is being shown
		updatePreviewArea();
	}


	/** 
	  * onItemchangedTrainNormalPredefined
	  *
	  * This is called when the selection on the normal pre-defined
	  * message list changes. It updates the preview area.
	  */
	void CTrainSelectPredefinedPage::onItemchangedTrainNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
		
		// Only interested in state changes
		if (pNMListView->uChanged == LVIF_STATE) 
		{        
			// if it is being selected
			if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
				 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
			{
				// deselect emergency messages
				m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);

				// update the display
				updatePreviewArea();
			}
		}

		*pResult = 0;
	}


	void CTrainSelectPredefinedPage::OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
	{
		// deselect emergency messages
		 m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);

		// if nothing is selected then this was effectively a deselection click
		// update the preview area
		POSITION currSel = m_normalList.GetFirstSelectedItemPosition();
    
		// if nothing is selected
		if(currSel == NULL)
		{
			// update the display
			updatePreviewArea();
		}

	if(pResult != NULL)
	{
		*pResult = 0;
	}
}


	/** 
	  * onItemchangedEmergencyPredefined
	  *
	  * This is called when the selection on the emergency pre-defined
	  * message list changes. It updates the preview area.
	  *
	  */
	void CTrainSelectPredefinedPage::onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

		// Only interested in state changes
		if (pNMListView->uChanged == LVIF_STATE) 
		{        
			// if it is being selected
			if ( ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
				 ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
			{
				// Deselect the normal priority listbox
				m_normalList.SetItemState(-1, 0, LVIS_SELECTED);

				// update the display
				updatePreviewArea();
			}
		}
	
	*pResult = 0;
}


	void CTrainSelectPredefinedPage::OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		// Deselect the normal priority listbox
		m_normalList.SetItemState(-1, 0, LVIS_SELECTED);

		// if nothing is selected then this was effectively a deselection click
		// update the preview area

		// get the index for the emergency priority list
		POSITION currSel = m_emergencyList.GetFirstSelectedItemPosition();

		// if something is selected
		if(currSel == NULL)
		{
			// update the display
			updatePreviewArea();
		}

		if(pResult != NULL)
		{
			*pResult = 0;
		}
	}


	/** 
	  * OnUpdateSearchNormal
	  *
	  * When the user types in the filter box under the 
	  * normal pre-defined messages, this applies the text in that
	  * box as a filter.
	  *
	  */
	void CTrainSelectPredefinedPage::OnUpdateSearchNormal() 
	{
		// get the text to search for from the text box.
		CString searchText;
		m_searchNormal.GetWindowText(searchText);
		searchText.MakeLower();

		// rebuild the list based on the search text
		m_normalList.DeleteAllItems();

		// get the TTIS message library
		// the singleton below that retrieves the pre-defined messages will
		// only throw exceptions the first time it is used.
		std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
				m_ttisPredefinedMessages->getNormalMessages();

		int count = 0;
		for(std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter = normalMessages.begin();
		messageIter != normalMessages.end(); messageIter++)
		{
			CString messageString(messageIter->description.c_str());
			messageString.MakeLower();

			// if the string matches
			if ( messageString.Find(searchText) != -1 )
			{
				// add the string to the list
				int index = m_normalList.InsertItem(count, messageIter->description.c_str());
				m_normalList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag) );
				count++;
			}
		}

		// deselect item and set window text
		m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
		m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
		updatePreviewArea();
	}


	/** 
	  * OnUpdateSearchEmergency
	  *
	  * When the user types in the filter box under the 
	  * emergency pre-defined messages, this applies the text in that
	  * box as a filter.
	  *
	  */
	void CTrainSelectPredefinedPage::OnUpdateSearchEmergency() 
	{
		CString searchText;
		m_searchEmergency.GetWindowText(searchText);
		searchText.MakeLower();

		// rebuild the list based on the string
		m_emergencyList.DeleteAllItems();
    
		// Load the predefined message library
		// the singleton below that retrieves the pre-defined messages will
		// only throw exceptions the first time it is used.
		std::vector<TA_Base_Core::PredefinedMessage> emergencyMessages =
			m_ttisPredefinedMessages->getEmergencyMessages();


		int count = 0;
		for(std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter = emergencyMessages.begin();
		messageIter != emergencyMessages.end(); messageIter++)
		{
			CString messageString(messageIter->description.c_str());
			messageString.MakeLower();

			// if the string matches
			if ( messageString.Find(searchText) != -1 )
			{
				// add the string to the list
				int index = m_emergencyList.InsertItem(count, messageIter->description.c_str());
				m_emergencyList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag) );
				// set the emergency colour to red
				m_emergencyList.setItemColour( count,
											   COLORREF( RGB( 255, 0, 0 ) ),
											   ListCtrlSelNoFocus::I_INDEX );
				count++;
			}
		}

		// deselect item and set window text
		m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
		m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
		updatePreviewArea();
	}


	void CTrainSelectPredefinedPage::OnOK()
	{
	}


	void CTrainSelectPredefinedPage::OnCancel()
	{
	}


	BOOL CTrainSelectPredefinedPage::PreTranslateMessage(MSG* pMsg) 
	{
		switch ( pMsg->message )
		{
		case WM_KEYDOWN:
			{
				switch ( pMsg->wParam )
				{
				case VK_ESCAPE:
					{
						return TRUE;
					}
					break;
				case VK_RETURN:
					{
						return TRUE;
					}
					break;
				default:
					{
						//do nothing
					}
					break;
				}
			}
			break;
		default:
			{
				//do nothing
			}
		}
		
		return CDialog::PreTranslateMessage(pMsg);
	}
	
	//TD 15349
	//zhou yuan++
	bool CTrainSelectPredefinedPage::findAndSelectMessageNameInList(CListCtrl& list, const std::string& messageName)
	{
		bool ret = false;
		
		int num = list.GetItemCount();
		for(int i =0 ; i < num ; ++i)
		{
			CString msgContent = list.GetItemText(i, 0);
			if(stricmp(msgContent, messageName.c_str()) == 0)
			{
				clearCurrentSelection(list);
				list.SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
				ret = true;
				break;
			}
		}
		return ret;
	}
	
	void CTrainSelectPredefinedPage::clearCurrentSelection(CListCtrl& list)
	{
		list.SetItemState(-1, 0, LVIS_SELECTED);
	}
	
	bool CTrainSelectPredefinedPage::findAndSelectStationMessage(const std::string& messageName)
	{
		bool ret = false;
		if(findAndSelectMessageNameInList(m_emergencyList, messageName))
		{
			OnClickEmergencyPredefined(0, 0);
			ret = true;
		}
		else if(findAndSelectMessageNameInList(m_normalList, messageName))
		{
			OnClickNormalPredefined(0, 0);
			ret = true;
		}
		return ret;
	}

	void CTrainSelectPredefinedPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
	{
		m_messageSelectionListener = messageSelectionListener;
	}
	//++zhou yuan

} // TA_IRS_App

