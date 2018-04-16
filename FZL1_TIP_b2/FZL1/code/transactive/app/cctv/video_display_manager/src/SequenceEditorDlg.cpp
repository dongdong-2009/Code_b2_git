/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/SequenceEditorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/VisualAudit_MessageTypes.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/ErrorMessage.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/SequenceEditorSaveAsDlg.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "ace/OS.h"
#include <boost/foreach.hpp>


using TA_IRS_App::SDKCommunicator;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
    static const int MAX_SEQ_NAME_CHARACTERS = 90;
    static const int MAX_VIDEO_SOURCES = 20;

    // 2 sequence updates received in this many seconds will be treated as the same
    static const time_t UPDATE_THRESHOLD_TIME = 2;
}

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(SequenceEditorDlg, CDialog)

    SequenceEditorDlg::SequenceEditorDlg(CWnd* pParent/*=NULL*/, VideoSourcesTreeCtrl* pSequenceTreeCtrl/*=NULL*/)
        : CDialog(SequenceEditorDlg::IDD, pParent),
          m_sequence(NULL),
          m_existingDwellTime(),
          m_sequenceVideoSources(),
          m_changed(FALSE),
          m_lastUpdatedTime(0),
          m_stationID(0),
          m_sequencesListBox(true),
          m_auditSender(std::auto_ptr< TA_Base_Core::AuditMessageSender >(
                            TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
                                TA_Base_Core::VisualAudit::Context)))
    {
        FUNCTION_ENTRY("SequenceEditorDlg");
        m_hIcon = AfxGetApp()->LoadIcon(IDI_SEQUENCE);
        m_sequenceTreeCtrl = pSequenceTreeCtrl;
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY("DoDataExchange");
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_DWELLTIME_SPIN, m_spinControl);
        DDX_Control(pDX, IDC_SE_SEQUENCES_LIST, m_sequencesListBox);
        DDX_Control(pDX, IDC_SE_CAMERATREE, m_cameraTreeCtrl);
        DDX_Control(pDX, IDCANCEL, m_cancelButton);
        DDX_Control(pDX, IDC_SE_DOWN_BUTTON, m_downButton);
        DDX_Control(pDX, IDC_SE_UP_BUTTON, m_upButton);
        DDX_Control(pDX, IDC_SE_TOP_BUTTON, m_topButton);
        DDX_Control(pDX, IDC_SE_SAVE_BUTTON, m_saveButton);
        DDX_Control(pDX, IDC_SE_REMOVE_BUTTON, m_removeButton);
        DDX_Control(pDX, IDC_SE_BOTTOM_BUTTON, m_bottomButton);
        DDX_Control(pDX, IDC_SE_ADD_BUTTON, m_addButton);
        DDX_Control(pDX, IDC_SE_DESCRIPTION_EDIT_BOX, m_sequenceDescription);
        DDX_Control(pDX, IDC_SE_DWELLTIME_EDIT_BOX, m_sequenceDwellTime);
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(SequenceEditorDlg, CDialog)
        // {{AFX_MSG_MAP(SequenceEditorDlg)
        ON_EN_UPDATE(IDC_SE_DESCRIPTION_EDIT_BOX, OnUpdateSeDescriptionEditBox)
        ON_EN_UPDATE(IDC_SE_DWELLTIME_EDIT_BOX, OnUpdateSeDwelltimeEditBox)
        ON_BN_CLICKED(IDC_SE_ADD_BUTTON, OnSeAddButton)
        ON_BN_CLICKED(IDC_SE_REMOVE_BUTTON, OnSeRemoveButton)
        ON_BN_CLICKED(IDC_SE_SAVE_BUTTON, OnSeSaveButton)
        ON_BN_CLICKED(IDC_SE_TOP_BUTTON, OnSeTopButton)
        ON_BN_CLICKED(IDC_SE_UP_BUTTON, OnSeUpButton)
        ON_BN_CLICKED(IDC_SE_DOWN_BUTTON, OnSeDownButton)
        ON_BN_CLICKED(IDC_SE_BOTTOM_BUTTON, OnSeBottomButton)
        ON_WM_DESTROY()
        ON_MESSAGE(WM_CAMERA_UPDATED, onCameraUpdated)
        ON_MESSAGE(WM_SEQUENCE_UPDATED, onSequenceUpdated)
        ON_NOTIFY(NM_DBLCLK, IDC_SE_CAMERATREE, onDblclkCameratree)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // SequenceEditorDlg message handlers

    int SequenceEditorDlg::DoModal(Sequence* sequence, unsigned long stationID)
    {
        FUNCTION_ENTRY("DoModal");
        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
        TA_ASSERT(application != NULL, "Application is NULL");
        TA_ASSERT(sequence != NULL, "Sequence is NULL");
        //
        // Populate sequence data
        //
        m_sequence = sequence;
        m_stationID = stationID;
        m_sequenceVideoSources.empty();
        std::list<Camera*> sequenceVideoSources = m_sequence->getCameras();
        BOOST_FOREACH(Camera * i, sequenceVideoSources)
        {
            if(i->getLocationId() == stationID || stationID == 0 || stationID == 1)
            {
                m_sequenceVideoSources.push_back(i);
            }
        }
        m_existingDwellTime.Format("%d", m_sequence->getDwellTime());
        if(m_existingDwellTime == "0")
        {
            m_existingDwellTime.Format("%d", application->getDefaultDwellTime());
        }
        // pop up the dialog
        int result = CDialog::DoModal();
        m_sequence = NULL;
        FUNCTION_EXIT;
        return result;
    }


    void SequenceEditorDlg::enableAddButton(BOOL enable)
    {
        FUNCTION_ENTRY("enableAddButton");
        if(GetFocus()->GetSafeHwnd() != m_addButton.GetSafeHwnd())
        {
            m_addButton.EnableWindow(enable && (m_sequenceVideoSources.size() < MAX_VIDEO_SOURCES));
        }
        else
        {
            m_addButton.EnableWindow(TRUE);
        }
        m_addButton.BringWindowToTop();
        FUNCTION_EXIT;
    }


    BOOL SequenceEditorDlg::enableRemoveButton(BOOL enable)
    {
        FUNCTION_ENTRY("enableRemoveButton");
        CWnd* focusWnd = GetFocus();
        HWND focusHwnd = focusWnd->GetSafeHwnd();
        m_removeButton.BringWindowToTop();
        if((focusHwnd != m_removeButton.GetSafeHwnd()) &&
                (focusHwnd != m_topButton.GetSafeHwnd()) &&
                (focusHwnd != m_upButton.GetSafeHwnd()) &&
                (focusHwnd != m_downButton.GetSafeHwnd()) &&
                (focusHwnd != m_bottomButton.GetSafeHwnd()))
        {
            m_removeButton.EnableWindow(enable);
            FUNCTION_EXIT;
            return TRUE;
        }
        else
        {
            m_removeButton.EnableWindow(TRUE);
            FUNCTION_EXIT;
            return FALSE;
        }
    }


    void SequenceEditorDlg::enableSaveButton()
    {
        FUNCTION_ENTRY("enableSaveButton");
        TA_ASSERT(m_sequence != NULL, "Sequence is NULL");
        CString newDescription;
        m_sequenceDescription.GetWindowText(newDescription);
        CString newDwellTime;
        m_sequenceDwellTime.GetWindowText(newDwellTime);
        // enable the button if the description or the dwell time has been changed
        // or if the list of video sources has been changed
        m_saveButton.EnableWindow(
            (newDescription != m_sequence->getSequenceDescription().c_str()) || // TD16691
            (m_existingDwellTime != newDwellTime) ||
            m_changed);
        FUNCTION_EXIT;
    }


    bool SequenceEditorDlg::isSourcesModified()
    {
        FUNCTION_ENTRY("isSourcesModified");
        TA_ASSERT(m_sequence != NULL, "Sequence is NULL");
        std::list<Camera*> existingCameras = m_sequence->getCameras();
        // if the list size is different - they arent the same
        if(existingCameras.size() != m_sequenceVideoSources.size())
        {
            FUNCTION_EXIT;
            return true;
        }
        else
        {
            std::list<Camera*>::iterator existingIter = existingCameras.begin();
            std::list<Camera*>::iterator currentIter = m_sequenceVideoSources.begin();
            // if the list sizes are equal - test each element individually
            for(; (existingIter != existingCameras.end()) && (currentIter != m_sequenceVideoSources.end());
                    existingIter++, currentIter++)
            {
                if(*existingIter != *currentIter)
                {
                    FUNCTION_EXIT;
                    return true;
                }
            }
        }
        FUNCTION_EXIT;
        return false;
    }


    void SequenceEditorDlg::enableOrderButtons(BOOL top, BOOL up, BOOL down,
                                               BOOL bottom)
    {
        FUNCTION_ENTRY("enableOrderButtons");
        m_topButton.EnableWindow(top);
        m_topButton.BringWindowToTop();
        m_upButton.EnableWindow(up);
        m_upButton.BringWindowToTop();
        m_downButton.EnableWindow(down);
        m_downButton.BringWindowToTop();
        m_bottomButton.EnableWindow(bottom);
        m_bottomButton.BringWindowToTop();
        FUNCTION_EXIT;
    }


    BOOL SequenceEditorDlg::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");
        //
        // Set up dialog
        //
        CDialog::OnInitDialog();
        // Set the icon for this dialog.
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon
        LoadButtonIcons();
        setToolTips();
        m_saveButton.EnableWindow(FALSE);
        m_addButton.EnableWindow(FALSE);
        m_removeButton.EnableWindow(FALSE);
        m_upButton.EnableWindow(FALSE);
        m_downButton.EnableWindow(FALSE);
        m_topButton.EnableWindow(FALSE);
        m_bottomButton.EnableWindow(FALSE);
        //
        // populate the dialog with sequence data
        //
        TA_ASSERT(m_sequence != NULL, "Sequence is NULL");
        CString title("视频序列编辑器 - ");
        title += m_sequence->getName().c_str();
        if(m_sequence->getReadOnly())
        {
            title += " (只读)";
        }
        SetWindowText(title);
        //m_sequenceLabel.SetText(m_sequence->getName().c_str());
        // TES 588
        // limit to 90 characters
        m_sequenceDescription.SetLimitText(MAX_SEQ_NAME_CHARACTERS);
        // TD16691
        // m_sequenceDescription.SetWindowText(m_sequence->getDescription().c_str());
        m_sequenceDescription.SetWindowText(m_sequence->getSequenceDescription().c_str());
        // TD16691
        m_sequenceDwellTime.SetLimitText(2);
        m_spinControl.SetBuddy(GetDlgItem(IDC_SE_DWELLTIME_EDIT_BOX));
        m_spinControl.SetRange(1, 99);
        m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
        unsigned short dwellTime = static_cast< unsigned short >(strtoul(m_existingDwellTime, NULL, 10));
        m_spinControl.SetPos(dwellTime);
        loadTree();
        loadExistingVideoSources();
        // reset list scrolling to the top
        // deselct any items
        m_sequencesListBox.SetCurSel(0);
        m_sequencesListBox.SetCurSel(-1);
        // if this is a read only sequence - change the save button to say save as
        if(m_sequence->getReadOnly())
        {
            m_saveButton.SetWindowText("序列另存为..");
        }
        else
        {
            m_saveButton.SetWindowText("应用");
        }
        DisplayItemManager::getInstance().registerForCameraChanges(this);
        DisplayItemManager::getInstance().registerForSequenceChanges(this);
        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SequenceEditorDlg::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");
        DisplayItemManager::getInstance().deregisterForSequenceChanges(this);
        DisplayItemManager::getInstance().deregisterForCameraChanges(this);
        CDialog::OnDestroy();
        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void SequenceEditorDlg::OnOK()
    {
        FUNCTION_ENTRY("OnOK");
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::LoadButtonIcons()
    {
        FUNCTION_ENTRY("LoadButtonIcons");
        // Load check box icons.
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_addIcon = AfxGetApp()->LoadIcon(IDI_ADD);
        m_removeIcon = AfxGetApp()->LoadIcon(IDI_REMOVE);
        m_topIcon = AfxGetApp()->LoadIcon(IDI_TOP);
        m_upIcon = AfxGetApp()->LoadIcon(IDI_UP);
        m_downIcon = AfxGetApp()->LoadIcon(IDI_DOWN);
        m_bottomIcon = AfxGetApp()->LoadIcon(IDI_BOTTOM);
        // Set the check box icons
        m_addButton.SetIcon(m_addIcon);
        m_removeButton.SetIcon(m_removeIcon);
        m_topButton.SetIcon(m_topIcon);
        m_upButton.SetIcon(m_upIcon);
        m_downButton.SetIcon(m_downIcon);
        m_bottomButton.SetIcon(m_bottomIcon);
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::loadExistingVideoSources()
    {
        FUNCTION_ENTRY("loadExistingVideoSources");
        std::ostringstream desc;
        m_sequencesListBox.ResetContent();
		BOOST_FOREACH(Camera* i, m_sequenceVideoSources)
        {
            desc.str("");
            desc << i->getName() << " - " << i->getDescription();
            m_sequencesListBox.addSourceString(desc.str().c_str());
        }
        FUNCTION_EXIT;
    }


    BOOL SequenceEditorDlg::PreTranslateMessage(MSG* pMsg)
    {
        switch(pMsg->message)
        {
            case WM_KEYDOWN:
                if(pMsg->wParam == VK_ESCAPE)
                {
					return TRUE;
				}
                break;
            case WM_MOUSEMOVE:
                handleTooltipsActivation(pMsg, m_addButton);
                handleTooltipsActivation(pMsg, m_removeButton);
                handleTooltipsActivation(pMsg, m_topButton);
                handleTooltipsActivation(pMsg, m_upButton);
                handleTooltipsActivation(pMsg, m_downButton);
                handleTooltipsActivation(pMsg, m_bottomButton);
                break;
            default:
				break;
        }
        return CDialog::PreTranslateMessage(pMsg);
    }


    void SequenceEditorDlg::OnUpdateSeDescriptionEditBox()
    {
        FUNCTION_ENTRY("OnUpdateSeDescriptionEditBox");
        UpdateData(TRUE);
        enableSaveButton();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnUpdateSeDwelltimeEditBox()
    {
        FUNCTION_ENTRY("OnUpdateSeDwelltimeEditBox");
        CString strDwellTime;
        m_sequenceDwellTime.GetWindowText(strDwellTime);
        for(int i = 0; i < strDwellTime.GetLength(); i++)
        {
            char c = strDwellTime.GetAt(i);
            if(!(c >= '0' && c <= '9'))
            {
                m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                break;
            }
        }
        if(strDwellTime == "0" || strDwellTime == "00")
        {
            m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
        }
        enableSaveButton();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeAddButton()
    {
        FUNCTION_ENTRY("OnSeAddButton");
        if(m_sequenceVideoSources.size() < MAX_VIDEO_SOURCES)
        {
            VideoInput* selectedInput = m_cameraTreeCtrl.getSelectedInput();
            Camera* selectedCamera = dynamic_cast<Camera*>(selectedInput);
            TA_ASSERT(selectedCamera != NULL, "The selected camera was NULL");
            // update the list and the list box
            m_sequenceVideoSources.push_back(selectedCamera);
            std::ostringstream desc;
            desc << selectedCamera->getName() << " - " << selectedCamera->getDescription();
            m_sequencesListBox.addSourceString(desc.str().c_str());
            m_cameraTreeCtrl.SetFocus();
            m_changed = isSourcesModified();
            enableSaveButton();
        }
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::onDblclkCameratree(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY("onDblclkCameratree");
        Camera* selectedCamera = dynamic_cast<Camera*>(m_cameraTreeCtrl.getSelectedInput());
        // if a camera is selected, then add it to the list
        if(selectedCamera != NULL)
        {
            OnSeAddButton();
            enableAddButton(true);
        }
        *pResult = 0;
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeRemoveButton()
    {
        FUNCTION_ENTRY("OnSeRemoveButton");
		std::list<Camera*>::iterator it = m_sequenceVideoSources.begin();
		int curSel = m_sequencesListBox.GetCurSel();
		for(int i = 0; i < curSel; ++i)
		{
			++it;
		}
        // erase the item in the list
        m_sequenceVideoSources.erase(it);
        // delete the item from the list box
        this->m_sequencesListBox.deleteSourceString();
        m_sequencesListBox.SetFocus();
        if(m_sequencesListBox.GetCount() == 0)
        {
            m_removeButton.EnableWindow(FALSE);
        }
        m_changed = isSourcesModified();
        enableSaveButton();
        FUNCTION_EXIT;
    }


	void SequenceEditorDlg::submitSequenceEditEvent(Sequence* editSequence)
	{
		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		try
		{
			std::auto_ptr<TA_Base_Core::ISession> session(TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId));
			std::string operatorName = session->getOperatorName();
	        TA_Base_Core::DescriptionParameters params;
	        TA_Base_Core::NameValuePair p1("Operator", operatorName);
	        TA_Base_Core::NameValuePair p2("Sequence", editSequence->getName());
			params.push_back(&p1);
			params.push_back(&p2);
	        m_auditSender->sendAuditMessage(TA_Base_Core::VisualAudit::VisualEditSequence,      // Message Type
				                            editSequence->getKey(),
	                                        params,    // Description Parameters
	                                        "",        // Details
	                                        sessionId, // Session ID
	                                        "",        // Alarm ID
	                                        "",        // Incident ID
	                                        "");        // Parent Event ID
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error occur when submit sequence eidt event");
		}

	}

    void SequenceEditorDlg::OnSeSaveButton()
    {
        FUNCTION_ENTRY("OnSeSaveButton");
        TA_ASSERT(m_sequence != NULL, "Sequence is null");
        //
        // if this is a read only sequence pop up a save as dialog
        //
        if(m_sequence->getReadOnly())
        {
            SequenceEditorSaveAsDlg saveAsDialog(this);
            if(saveAsDialog.DoModal() == IDOK)
            {
                // get the selected sequence
                Sequence* selectedSequence = saveAsDialog.getSelectedSequence();
                TA_ASSERT(selectedSequence != NULL, "Ok cannot be pressed if a sequence is not selected");
                // now save the details as the selected sequence
                try
                {
                    CString newDwellTime;
                    m_sequenceDwellTime.GetWindowText(newDwellTime);
                    unsigned short dwellTime = static_cast< unsigned short >(strtoul(newDwellTime, NULL, 10));
                    if((m_sequenceVideoSources.size() == 0) && (dwellTime > 0))
                    {
                        // set the dwell time to 0 for empty sequences
                        dwellTime = 0;
                        // set the dwell time box back to the default
                        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
                        TA_ASSERT(application != NULL, "Application is NULL");
                        newDwellTime.Format("%d", application->getDefaultDwellTime());
                        m_existingDwellTime = newDwellTime;
                        m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                        m_spinControl.SetPos(application->getDefaultDwellTime());
                    }
                    if((m_sequenceVideoSources.size() > 0) && (dwellTime == 0))
                    {
						throw TA_Base_Core::TransactiveException(SEQUENCE_DEWELLTIME_INVALID.c_str());
                    }
                    // Save the sequence
                    //
                    //     m_presenter->setSequenceConfig ( selectedSequence, dwellTime, m_sequenceVideoSources  );
                    m_presenter->saveSequenceConfig(selectedSequence, dwellTime, m_sequenceVideoSources);
                    m_existingDwellTime = newDwellTime;
                    m_changed = FALSE;
                    CString newDescription;
                    m_sequenceDescription.GetWindowText(newDescription);
                    m_presenter->setSequenceDescription(selectedSequence, newDescription.GetBuffer(0));
                    // now set the current sequence
                    m_sequence = selectedSequence;
                    // force the display to update
                    onSequenceUpdated(NULL, reinterpret_cast<LPARAM>(m_sequence));
                }
                catch(TA_Base_Core::TransactiveException& e)
                {
			        AfxMessageBox(e.what(), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                }
            }
            else
            {
                FUNCTION_EXIT;
                return;
            }
        }
        else
        {
            CString newDwellTime;
            m_sequenceDwellTime.GetWindowText(newDwellTime);
			bool needSubmitEvent = false;
            try
            {
                if(isSourcesModified() || (m_existingDwellTime != newDwellTime))
                {
                    unsigned short dwellTime = static_cast< unsigned short >(strtoul(newDwellTime, NULL, 10));
                    if((m_sequenceVideoSources.size() == 0) && (dwellTime > 0))
                    {
                        // set the dwell time to 0 for empty sequences
                        dwellTime = 0;
                        // set the dwell time box back to the default
                        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
                        TA_ASSERT(application != NULL, "Application is NULL");
                        newDwellTime.Format("%d", application->getDefaultDwellTime());
                        m_existingDwellTime = newDwellTime;
                        m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                        m_spinControl.SetPos(application->getDefaultDwellTime());
                    }
                    if((m_sequenceVideoSources.size() > 0) && (dwellTime == 0))
                    {
						throw TA_Base_Core::TransactiveException(SEQUENCE_DEWELLTIME_INVALID.c_str());
                    }
                    // Save the sequence
                    m_presenter->saveSequenceConfig(m_sequence, dwellTime, m_sequenceVideoSources);
					needSubmitEvent = true;
                    //  m_presenter->setSequenceConfig ( m_sequence, dwellTime, m_sequenceVideoSources  );
                    m_existingDwellTime = newDwellTime;
                    m_changed = FALSE;
                }
                CString newDescription;
                m_sequenceDescription.GetWindowText(newDescription);
                if(newDescription != m_sequence->getSequenceDescription().c_str())   // TD16691
                {
                    m_presenter->setSequenceDescription(m_sequence, newDescription.GetBuffer(0));
					needSubmitEvent = true;
                }
				if(needSubmitEvent)
				{
					//submitSequenceEditEvent(m_sequence);		//There is no event in FZL6 CCTV
				}
            }
            catch(TA_Base_Core::TransactiveException& e)
            {
		        AfxMessageBox(e.what(), MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
            }
        }
        enableSaveButton();
        EndDialog(IDOK);
        //m_cancelButton.SetFocus();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeTopButton()
    {
        FUNCTION_ENTRY("OnSeTopButton");
        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();
        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for(int i = 0; i < curSel; i++)
        {
            theIterator++;
        }
        // get the item
        Camera* selectedCamera = *theIterator;
        // erase the item in the list
        m_sequenceVideoSources.erase(theIterator);
        // insert it at the front
        m_sequenceVideoSources.push_front(selectedCamera);
        // move the item in the list box
        this->m_sequencesListBox.moveSourceTop();
        m_sequencesListBox.SetFocus();
        m_changed = isSourcesModified();
        enableSaveButton();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeUpButton()
    {
        FUNCTION_ENTRY("OnSeUpButton");
        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();
        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for(int i = 0; i < curSel; i++)
        {
            theIterator++;
        }
        // get the item
        Camera* selectedCamera = *theIterator;
        // erase the item
        m_sequenceVideoSources.erase(theIterator);
        // insert the item one position up
        theIterator = m_sequenceVideoSources.begin();
        // theIterator += (curSel - 1);
        // no random access iterator for lists
        for(int i = 0; i < (curSel - 1); i++)
        {
            theIterator++;
        }
        m_sequenceVideoSources.insert(theIterator, selectedCamera);
        // move the item up in the list box
        this->m_sequencesListBox.moveSourceUp();
        m_sequencesListBox.SetFocus();
        m_changed = isSourcesModified();
        enableSaveButton();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeDownButton()
    {
        FUNCTION_ENTRY("OnSeDownButton");
        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();
        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for(int i = 0; i < curSel; i++)
        {
            theIterator++;
        }
        // get the item
        Camera* selectedCamera = *theIterator;
        // erase the item
        m_sequenceVideoSources.erase(theIterator);
        // insert the item one position down
        theIterator = m_sequenceVideoSources.begin();
        // theIterator += (curSel + 1);
        // no random access iterator for lists
        for(int i = 0; i < (curSel + 1); i++)
        {
            theIterator++;
        }
        m_sequenceVideoSources.insert(theIterator, selectedCamera);
        // move the item down in the list box
        this->m_sequencesListBox.moveSourceDown();
        m_sequencesListBox.SetFocus();
        m_changed = isSourcesModified();
        enableSaveButton();
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeBottomButton()
    {
        FUNCTION_ENTRY("OnSeBottomButton");
        std::list<Camera*>::iterator it = m_sequenceVideoSources.begin();
        int curSel = m_sequencesListBox.GetCurSel();
		for(int i = 0; i < curSel; ++i)
		{
			++it;
		}
        Camera* selectedCamera = *it;
        // erase the item in the list
        m_sequenceVideoSources.erase(it);
        // insert it at the bottom
        m_sequenceVideoSources.push_back(selectedCamera);
        // move the item in the list box
        this->m_sequencesListBox.moveSourceBottom();
        m_sequencesListBox.SetFocus();
        m_changed = isSourcesModified();
        enableSaveButton();
        FUNCTION_EXIT;
    }


    /**
      * SetToolTips
      *
      * Create the tool tips for the buttons.
      */
    void SequenceEditorDlg::setToolTips()
    {
        FUNCTION_ENTRY("setToolTips");
        // Create the tool tips for the check boxes
        m_ctlToolTip.Create(this);
        m_ctlToolTip.SetFont(GetFont());
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_ADD_BUTTON), "Add Camera");
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_REMOVE_BUTTON), "Remove Camera");
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_TOP_BUTTON), "Top");
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_UP_BUTTON), "Up");
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_DOWN_BUTTON), "Down");
        m_ctlToolTip.AddWindowTool(GetDlgItem(IDC_SE_BOTTOM_BUTTON), "Bottom");
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::handleTooltipsActivation(MSG *pMsg, const CButton& button)//, CWnd *This, CWnd *disabledCtrls[], int numOfCtrls, CToolTipCtrl *pTooltip)
    {
        FUNCTION_ENTRY("handleTooltipsActivation");
        CRect  rect;
        POINT  pt;
        HWND   hWnd     = pMsg->hwnd;
        LPARAM lParam   = pMsg->lParam;
        // ---------------------------------------------------------------------------
        // Disabled control do not show tool tips, in modal dialog
        //
        //
        // The hwnd of the WM_MOUSEMOVE above a disabled control
        // is the hWnd of the Dialog itself, this confuse the tooltip
        //
        // To correct this, if we get WM_MOUSEMOVE and the hwnd is the dialog's hwnd
        //
        // We check on all the controls that are Visible, but disabled if the point is in their
        // rectangle.
        //
        // In this case we alter the msg to the controls hWnd and coordinates before
        // Relaying it to the toolTip control
        // ----------------------------------------
        // ---------------------------
        // The point is in the dialog
        // client coordinates
        // ---------------------------
        pt.x = LOWORD(pMsg->lParam);  // horizontal position of cursor
        pt.y = HIWORD(pMsg->lParam);  // vertical position of cursor
        if(FALSE == button.IsWindowEnabled())
        {
            // ---------------------------------
            // rect is the control rectangle in
            // Dialog client coordinates
            // ----------------------------------
            button.GetWindowRect(&rect);
            ScreenToClient(&rect);
            if(rect.PtInRect(pt))
            {
                // ----------------------------------------------------------------
                // The mouse is inside the control
                //
                // 1. We change the Msg hwnd    to the controls hWnd
                // 2. We change the Msg lParam  to the controls client coordinates
                //
                // ----------------------------------------------------------------
                pMsg->hwnd = button.m_hWnd;
                ClientToScreen(&pt);
                button.ScreenToClient(&pt);
                pMsg->lParam = MAKELPARAM(pt.x, pt.y);
                // ---------------------------------------
                // Relay the msg to the tool tip control
                // ---------------------------------------
                m_ctlToolTip.RelayEvent(pMsg);
                m_ctlToolTip.Activate(TRUE);
                // --------------------------------------
                // Restore the original msg
                // --------------------------------------
                pMsg->hwnd = hWnd;
                pMsg->lParam = lParam;
            }
        }
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::loadTree()
    {
        FUNCTION_ENTRY("loadTree");
        m_cameraTreeCtrl.setToSequenceEditorTree();
        // load the cameras
        std::map<std::string, Camera*> cameras =
            DisplayItemManager::getInstance().getAllCameras();
        // build a vector
        std::vector<VideoInput*> inputs;
        for(std::map<std::string, Camera*>::iterator it = cameras.begin(); it != cameras.end(); ++it)
        {
            // insert the item
            Camera* cam = it->second;
            if(m_stationID == 0 || m_stationID == 1 || cam->getLocationId() == m_stationID)
			{
                inputs.push_back(it->second);
			}
        }
        m_cameraTreeCtrl.insertVideoSources(inputs);
        FUNCTION_EXIT;
    }


    LRESULT SequenceEditorDlg::onSequenceUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onSequenceUpdated");
        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Sequence update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Sequence update received for NULL display item");
        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
        TA_ASSERT(application != NULL, "Application is NULL");
        if(input == m_sequence)
        {
            // the current sequence was updated
            CString title("视频序列编辑器 - ");
            title += m_sequence->getName().c_str();
            if(m_sequence->getReadOnly())
            {
                title += " (只读)";
            }
            SetWindowText(title);
            //m_sequenceLabel.SetText(m_sequence->getName().c_str());
            if(m_sequence->getReadOnly())
            {
                m_saveButton.SetWindowText("序列另存为..");
            }
            else
            {
                m_saveButton.SetWindowText("应用");
            }
            // it must be tested against the current state
            // to see if it was changed externally or not
            // (this could be the result of a save operation made here)
            bool same = true;
            // check the description
            CString newDescription;
            m_sequenceDescription.GetWindowText(newDescription);
            same = same && (newDescription == m_sequence->getSequenceDescription().c_str()); // TD16691
            // check the dwell time
            CString newDwellTime;
            m_sequenceDwellTime.GetWindowText(newDwellTime);
            unsigned short dwellTime = m_sequence->getDwellTime();
            if(dwellTime == 0)
            {
                dwellTime = application->getDefaultDwellTime();
            }
            m_existingDwellTime.Format("%d", dwellTime);
            bool dwellTimeNeedsUpdate = (m_existingDwellTime != newDwellTime);
            // test the local cameras against the saved cameras
            same = same && !isSourcesModified();
            if(!same || dwellTimeNeedsUpdate)
            {
                // set the dwell time
                m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                m_spinControl.SetPos(dwellTime);
                // set the description
                m_sequenceDescription.SetWindowText(m_sequence->getSequenceDescription().c_str()); // TD16691
                // set the cameras
                m_sequenceVideoSources = m_sequence->getCameras();
                loadExistingVideoSources();
                // get the current time
                time_t currentTime;
                ACE_OS::time(&currentTime);
                if((m_lastUpdatedTime == 0)
                   || ((currentTime - m_lastUpdatedTime) > UPDATE_THRESHOLD_TIME))
                {
                    // set the last updated time
                    m_lastUpdatedTime = currentTime;
					/*
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_090072);
					*/
                    // AfxMessageBox( "The dwell time and/or ordered list of cameras for the currently loaded Sequence have been modified!" );
                }
                else
                {
                    // set the last updated time
                    m_lastUpdatedTime = currentTime;
                }
                if(m_sequenceTreeCtrl != NULL && m_sequenceTreeCtrl->m_hWnd != NULL)
				{
                    m_sequenceTreeCtrl->videoInputUpdated(input);
				}
            }
        }
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT SequenceEditorDlg::onCameraUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onCameraUpdated");
        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Camera update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Camera update received for NULL display item");
        // update the list control
        m_cameraTreeCtrl.videoInputUpdated(input);
        FUNCTION_EXIT;
        return 0;
    }


    void SequenceEditorDlg::Editsequence(Sequence * sequence, unsigned long stationID)
    {
        DoModal(sequence, stationID);
    }
} // TA_IRS_App
