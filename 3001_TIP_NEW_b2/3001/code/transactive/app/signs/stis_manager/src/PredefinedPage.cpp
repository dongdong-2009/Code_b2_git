/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/PredefinedPage.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2014/04/02 18:37:03 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * The predefined message selection tab
  */

#include "stdafx.h"
#include "stismanager.h"
#include "PredefinedPage.h"
#include "STISPredefinedMessages.h"


#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"

#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core\data_access_interface\entity_access\src\TISAgentEntityData.h"
#include "core\data_access_interface\entity_access\src\STISEntityData.h"
//#include "bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h"
#include "TisDialogCtrl.h"
#include "helperfun.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::PredefinedMessage;

namespace TA_IRS_App
{

    CPredefinedPage::CPredefinedPage(CWnd* pParent /*=NULL*/)
        : HandledDlg(CPredefinedPage::IDD, pParent),
          m_messageSelectionListener(NULL),
          m_stisPredefinedMessages(NULL),
          m_predefinedContent(NULL)
    {
        //{{AFX_DATA_INIT(CPredefinedPage)
        //}}AFX_DATA_INIT
    }

    CPredefinedPage::~CPredefinedPage()
    {
    }


    void CPredefinedPage::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CPredefinedPage)
        DDX_Control(pDX, IDC_NORMAL_PREDEFINED, m_normalList);
        DDX_Control(pDX, IDC_EMERGENCY_PREDEFINED, m_emergencyList);
        DDX_Control(pDX, IDC_SEARCH_NORMAL, m_searchNormal);
        DDX_Control(pDX, IDC_SEARCH_EMERGENCY, m_searchEmergency);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CPredefinedPage, CDialog)
        //{{AFX_MSG_MAP(CPredefinedPage)
        ON_WM_SHOWWINDOW()
        ON_EN_UPDATE(IDC_SEARCH_EMERGENCY, OnUpdateSearchEmergency)
        ON_EN_UPDATE(IDC_SEARCH_NORMAL, OnUpdateSearchNormal)
        ON_NOTIFY(NM_CLICK, IDC_EMERGENCY_PREDEFINED, OnClickEmergencyPredefined)
        ON_NOTIFY(NM_CLICK, IDC_NORMAL_PREDEFINED, OnClickNormalPredefined)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_NORMAL_PREDEFINED, onItemchangedNormalPredefined)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_EMERGENCY_PREDEFINED, onItemchangedEmergencyPredefined)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnUpdateCurrentSTISVersion)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CPredefinedPage message handlers

    BOOL CPredefinedPage::OnInitDialog()
    {
        CDialog::OnInitDialog();
        // Create unicode text ctrl for show tamil language
        RECT r;
        CWnd* w = GetDlgItem(IDC_PREDEFINED_CONTENT);
		if(NULL != w)
		{
	        w->GetClientRect(&r);
	        m_predefinedContent = CreateWindowExW(0, L"edit", NULL,
	                                              WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_TABSTOP | WS_BORDER | ES_AUTOVSCROLL | ES_READONLY,
	                                              r.left, r.top, r.right - r.left, r.bottom - r.top, w->m_hWnd, NULL, NULL, NULL);
            CFont* font = GetFont();
            HFONT hFont = *font;
            font->Detach();
            ::PostMessage(m_predefinedContent, WM_SETFONT, (WPARAM)hFont, 0);
		}
        return TRUE;
    }

    void CPredefinedPage::Init()
    {
        m_stisPredefinedMessages = STISPredefinedMessages::getInstance();
        // extended styles (including infotips)
        // column widths for message titles
        m_emergencyList.SetExtendedStyle(m_emergencyList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_normalList.SetExtendedStyle(m_normalList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        RECT rect;
        m_emergencyList.GetClientRect(&rect);
        m_emergencyList.InsertColumn(0, "Message Name");
        m_emergencyList.SetColumnWidth(0, rect.right);
        m_normalList.GetClientRect(&rect);
        m_normalList.InsertColumn(0, "Message Name");
        m_normalList.SetColumnWidth(0, rect.right);
        // load the pre-defined messages into the lists
        // populateLists();
        // register with the pre-defined message singleton to be updated of a change
        m_stisPredefinedMessages->registerCurrentVersionUser(this);
        m_emergencyList.setScrollBarVisibility(true);
        m_normalList.setScrollBarVisibility(true);
    }

    void CPredefinedPage::OnDestroy()
    {
        // no longer accepting updates
        if(m_stisPredefinedMessages != NULL)
        {
            m_stisPredefinedMessages->deregisterCurrentVersionUser(this);
            m_stisPredefinedMessages->removeInstance();
            m_stisPredefinedMessages = NULL;
        }
        CDialog::OnDestroy();
    }


    void CPredefinedPage::windowShown()
    {
        if(m_messageSelectionListener != NULL)
        {
            m_validMessage = hasValidSelection();
            if(m_validMessage)
            {
                PredefinedMessage message = getMessage();
                m_messageSelectionListener->predefinedMessageSelected(true, m_validMessage, message.priority, message.repeatInterval);
            }
            else
            {
                m_messageSelectionListener->predefinedMessageSelected(true, m_validMessage);
            }
        }
    }


    void CPredefinedPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }


    LRESULT CPredefinedPage::OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam)
    {
        // rebuild the lists
        populateLists();
        // update all the stuff to do with selected items in lists, if dialog active
        CTisDialogCtrl* tab = getHandleTab();
        if((NULL != tab) && (tab->getActiveDialog() == this))
        {
            updatePreviewArea();
        }
        return 0;
    }


    bool CPredefinedPage::hasValidSelection() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CPredefinedPage::hasValidSelection");
        // get the selected message
        // NULL means no selection
        const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();
        if(messagePointer != NULL)
        {
            LOG_FUNCTION_EXIT(SourceInfo, "CPredefinedPage::hasValidSelection");
            return true;
        }
        LOG_FUNCTION_EXIT(SourceInfo, "CPredefinedPage::hasValidSelection");
        return false;
    }


    TA_Base_Core::PredefinedMessage CPredefinedPage::getMessage() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CPredefinedPage::getMessage");
        const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();
        TA_ASSERT(messagePointer != NULL,
                  "CPredefinedPage::getMessage() called when there is no valid message selected. Call hasValidSelection() first.");
        LOG_FUNCTION_EXIT(SourceInfo, "CPredefinedPage::getMessage");
        return *messagePointer;
    }


    void CPredefinedPage::populateLists()
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "PredefinedMessage::populateLists");
        // clear the list of pre-defined messages
        m_normalList.DeleteAllItems();
        m_emergencyList.DeleteAllItems();
        // Load the predefined message library
        // get the STIS message libraries
        std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
            m_stisPredefinedMessages->getNormalMessages();
        std::vector<TA_Base_Core::PredefinedMessage> emergencyMessages =
            m_stisPredefinedMessages->getEmergencyMessages();
        std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter;
        int count = 0;
        for(messageIter = normalMessages.begin();
                messageIter != normalMessages.end(); messageIter++)
        {
            // add the string to the list
            int index = m_normalList.InsertItem(count, messageIter->description.c_str());
            m_normalList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag));
            count++;
        }
        count = 0;
        for(messageIter = emergencyMessages.begin();
                messageIter != emergencyMessages.end(); messageIter++)
        {
            // add the string to the list
            int index = m_emergencyList.InsertItem(count, messageIter->description.c_str());
            m_emergencyList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag));
            // set the emergency colour to red
            m_emergencyList.setItemColour(count,
                                          COLORREF(RGB(255, 0, 0)),
                                          ListCtrlSelNoFocus::I_INDEX);
            count++;
        }
        LOG_FUNCTION_EXIT(SourceInfo, "PredefinedMessage::populateLists");
    }


    const TA_Base_Core::PredefinedMessage* CPredefinedPage::getSelectedMessageData() const
    {
        LOG_FUNCTION_ENTRY(SourceInfo, "CPredefinedPage::getSelectedMessageData");
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
        if(normalSelected > -1)
        {
            // get the data for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_normalList.GetItemData(normalSelected));
            // find the appropriate message
            message = m_stisPredefinedMessages->getNormalMessageById(itemData);
            // anything selected should point to a valid message
            TA_ASSERT(message != NULL,
                      "The selected message is not a valid message");
        }
        // otherwise if an emergency message is selected
        else if(emergencySelected > -1)
        {
            // get the text for the selected item
            unsigned short itemData = static_cast<unsigned short>(m_emergencyList.GetItemData(emergencySelected));
            // find the appropriate message
            message = m_stisPredefinedMessages->getEmergencyMessageById(itemData);
            // anything selected should point to a valid message
            TA_ASSERT(message != NULL,
                      "The selected message is not a valid message");
        }
        // otherwise, return NULL
        LOG_FUNCTION_EXIT(SourceInfo, "CPredefinedPage::getSelectedMessageData");
        return message;
    }


    /**
      * updatePreviewArea
      *
      * updates the message preview area with the text from the selected message.
      * also updates the languages box.
      *
      */
    void CPredefinedPage::updatePreviewArea()
    {
        // get the selected message
        const TA_Base_Core::PredefinedMessage* message = getSelectedMessageData();
        // NULL means nothing is selected
        if(message != NULL)
        {
            // set the message text
			setPredefinedContent(convertUtf8ToUtf16le(message->message.c_str()));
            // notify the owning window
            if(m_messageSelectionListener != NULL)
            {
                m_validMessage = true;
                m_messageSelectionListener->predefinedMessageSelected(false, m_validMessage,
                                                                      message->priority, message->repeatInterval);
            }
        }
        else
        {
            // clear the text in the preview window
			setPredefinedContent(L"");
            // notify the owning window
            if(m_messageSelectionListener != NULL)
            {
                m_validMessage = false;
                m_messageSelectionListener->predefinedMessageSelected(false, m_validMessage);
            }
        }
    }


    void CPredefinedPage::onItemchangedNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        // Only interested in state changes
        if(pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if(((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                    ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
                // deselect emergency messages
                m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
                // update the display
                updatePreviewArea();
            }
        }
        *pResult = 0;
    }


    void CPredefinedPage::OnClickNormalPredefined(NMHDR* pNMHDR, LRESULT* pResult)
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


    void CPredefinedPage::onItemchangedEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        // Only interested in state changes
        if(pNMListView->uChanged == LVIF_STATE)
        {
            // if it is being selected
            if(((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED) &&
                    ((pNMListView->uOldState & LVIS_SELECTED) == 0))
            {
                // Deselect the normal priority listbox
                m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
                // update the display
                updatePreviewArea();
            }
        }
        *pResult = 0;
    }


    void CPredefinedPage::OnClickEmergencyPredefined(NMHDR* pNMHDR, LRESULT* pResult)
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


    void CPredefinedPage::OnUpdateSearchNormal()
    {
        // get the text to search for from the text box.
        CString searchText;
        m_searchNormal.GetWindowText(searchText);
        searchText.MakeLower();
        // rebuild the list based on the search text
        m_normalList.DeleteAllItems();
        // get the STIS message library
        // the singleton below that retrieves the pre-defined messages will
        // only throw exceptions the first time it is used.
        std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
            m_stisPredefinedMessages->getNormalMessages();
        int count = 0;
        for(std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter = normalMessages.begin();
                messageIter != normalMessages.end(); messageIter++)
        {
            CString messageString(messageIter->description.c_str());
            messageString.MakeLower();
            // if the string matches
            if(messageString.Find(searchText) != -1)
            {
                // add the string to the list
                int index = m_normalList.InsertItem(count, messageIter->description.c_str());
                m_normalList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag));
                count++;
            }
        }
        // deselect item and set window text
        m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
        m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
        updatePreviewArea();
    }


    void CPredefinedPage::OnUpdateSearchEmergency()
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
            m_stisPredefinedMessages->getEmergencyMessages();
        int count = 0;
        for(std::vector<TA_Base_Core::PredefinedMessage>::iterator messageIter = emergencyMessages.begin();
                messageIter != emergencyMessages.end(); messageIter++)
        {
            CString messageString(messageIter->description.c_str());
            messageString.MakeLower();
            // if the string matches
            if(messageString.Find(searchText) != -1)
            {
                // add the string to the list
                int index = m_emergencyList.InsertItem(count, messageIter->description.c_str());
                m_emergencyList.SetItemData(index, static_cast<DWORD>(messageIter->messageTag));
                // set the emergency colour to red
                m_emergencyList.setItemColour(count,
                                              COLORREF(RGB(255, 0, 0)),
                                              ListCtrlSelNoFocus::I_INDEX);
                count++;
            }
        }
        // deselect item and set window text
        m_emergencyList.SetItemState(-1, 0, LVIS_SELECTED);
        m_normalList.SetItemState(-1, 0, LVIS_SELECTED);
        updatePreviewArea();
    }


    void CPredefinedPage::OnOK()
    {
    }


    void CPredefinedPage::OnCancel()
    {
    }

    bool CPredefinedPage::findAndSelectMessageNameInList(CListCtrl& list, const std::string messageName)
    {
        bool ret = false;
        int num = list.GetItemCount();
        for(int i = 0 ; i < num ; ++i)
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

    void CPredefinedPage::clearCurrentSelection(CListCtrl& list)
    {
        list.SetItemState(-1, 0, LVIS_SELECTED);
    }

    bool CPredefinedPage::findAndSelectStationMessage(const std::string& messageName)
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

	void CPredefinedPage::setPredefinedContent(const std::wstring& content)
	{
		::SetWindowTextW(m_predefinedContent, content.c_str());
	}
} // TA_IRS_App
