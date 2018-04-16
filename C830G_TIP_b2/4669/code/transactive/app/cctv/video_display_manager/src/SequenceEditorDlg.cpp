/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/SequenceEditorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 11:31:40 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "videodisplaymanager.h"
#include "VideoDisplayManagerDlg.h"
#include "SequenceEditorDlg.h"
#include "SwitchAgentCommunicator.h"
#include "DisplayItemManager.h"
#include "Sequence.h"
#include "Camera.h"
#include "SequenceEditorSaveAsDlg.h"
//#include "core/exceptions/src/MessageResourceException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "ace/OS.h"

using TA_IRS_App::SwitchAgentCommunicator;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
    static const int MAX_SEQ_NAME_CHARACTERS = 90;
    static const int MAX_VIDEO_SOURCES = 64;

    // 2 sequence updates received in this many seconds will be treated as the same
    static const time_t UPDATE_THRESHOLD_TIME = 2;
}

namespace TA_IRS_App
{

    SequenceEditorDlg::SequenceEditorDlg(CWnd* pParent/*=NULL*/, VideoSourcesTreeCtrl* pSequenceTreeCtrl/*=NULL*/)
      : CDialog(SequenceEditorDlg::IDD, pParent),
        m_sequence(NULL),
        m_existingDwellTime(),
        m_sequenceVideoSources(),
        m_changed(FALSE),
        m_lastUpdatedTime(0)
    {
        FUNCTION_ENTRY( "SequenceEditorDlg" );

        // {{AFX_DATA_INIT(SequenceEditorDlg)
        // }}AFX_DATA_INIT
        m_hIcon = AfxGetApp()->LoadIcon(IDI_SEQUENCE);
        m_sequenceTreeCtrl = pSequenceTreeCtrl;

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(SequenceEditorDlg)
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
        DDX_Control(pDX, IDC_SE_SEQUENCE_LABEL, m_sequenceLabel);
        DDX_Control(pDX, IDC_SE_DESCRIPTION_EDIT_BOX, m_sequenceDescription);
        DDX_Control(pDX, IDC_SE_DWELLTIME_EDIT_BOX, m_sequenceDwellTime);
        // }}AFX_DATA_MAP

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

    int SequenceEditorDlg::DoModal(Sequence* sequence)
    {
        FUNCTION_ENTRY( "DoModal" );

        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
        TA_ASSERT(application != NULL, "Application is NULL");

        TA_ASSERT(sequence != NULL, "Sequence is NULL");

        // 
        // Populate sequence data
        // 

        m_sequence = sequence;
        m_sequenceVideoSources = m_sequence->getCameras();
        m_existingDwellTime.Format("%d", m_sequence->getDwellTime());

        // TES 725
        // if the seqeunce is empty - the dwell time is 0.
        // set it to the default dwell time
        if (m_existingDwellTime == "0")
        {
            m_existingDwellTime.Format("%d", application->getDefaultDwellTime());
        }

        // pop up the dialog

        int result = CDialog::DoModal();

        // after the dialog is closed - set the sequence back to null
        m_sequence = NULL;
        // HANDLE prevParent = SetParent(application);
        FUNCTION_EXIT;
        return result;
    }


    void SequenceEditorDlg::enableAddButton(BOOL enable)
    {
        FUNCTION_ENTRY( "enableAddButton" );

        if ( GetFocus()->GetSafeHwnd() != m_addButton.GetSafeHwnd() )
        {
            m_addButton.EnableWindow(enable && (m_sequenceVideoSources.size()<MAX_VIDEO_SOURCES));
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
        FUNCTION_ENTRY( "enableRemoveButton" );

        CWnd* focusWnd = GetFocus();
        HWND focusHwnd = focusWnd->GetSafeHwnd();
        m_removeButton.BringWindowToTop();
        if ( (focusHwnd != m_removeButton.GetSafeHwnd()) &&
            (focusHwnd != m_topButton.GetSafeHwnd()) &&
            (focusHwnd != m_upButton.GetSafeHwnd()) &&
            (focusHwnd != m_downButton.GetSafeHwnd()) &&
            (focusHwnd != m_bottomButton.GetSafeHwnd()) )
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
        FUNCTION_ENTRY( "enableSaveButton" );

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
            m_changed );

        FUNCTION_EXIT;
    }


    bool SequenceEditorDlg::isSourcesModified()
    {
        FUNCTION_ENTRY( "isSourcesModified" );

        TA_ASSERT(m_sequence != NULL, "Sequence is NULL");

        std::list<Camera*> existingCameras = m_sequence->getCameras();

        // if the list size is different - they arent the same
        if ( existingCameras.size() != m_sequenceVideoSources.size() )
        {
            FUNCTION_EXIT;
            return true;
        }
        else
        {
            std::list<Camera*>::iterator existingIter = existingCameras.begin();
            std::list<Camera*>::iterator currentIter = m_sequenceVideoSources.begin();

            // if the list sizes are equal - test each element individually
            for (; (existingIter != existingCameras.end()) && (currentIter != m_sequenceVideoSources.end());
            existingIter++, currentIter++)
            {
                if ( *existingIter != *currentIter )
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
        FUNCTION_ENTRY( "enableOrderButtons" );

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
        FUNCTION_ENTRY( "OnInitDialog" );

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

        CString title("Sequence Editor - ");
        title += m_sequence->getName().c_str();
        if (m_sequence->getReadOnly())
        {
            title += " (Read Only)";
        }
        SetWindowText(title);

        m_sequenceLabel.SetText(m_sequence->getName().c_str());

        // TES 588
        // limit to 90 characters
        m_sequenceDescription.SetLimitText(MAX_SEQ_NAME_CHARACTERS);
        // TD16691
        // m_sequenceDescription.SetWindowText(m_sequence->getDescription().c_str());
        m_sequenceDescription.SetWindowText(m_sequence->getSequenceDescription().c_str());
        // TD16691

        m_sequenceDwellTime.SetLimitText(2);
        m_spinControl.SetBuddy( GetDlgItem(IDC_SE_DWELLTIME_EDIT_BOX) );
        m_spinControl.SetRange(1,99);

        m_sequenceDwellTime.SetWindowText( m_existingDwellTime );
        unsigned short dwellTime = static_cast< unsigned short >( strtoul( m_existingDwellTime, NULL, 10 ) );
        m_spinControl.SetPos( dwellTime );


        loadTree();

        loadExistingVideoSources();

        // reset list scrolling to the top
        // deselct any items
        m_sequencesListBox.SetCurSel(0);
        m_sequencesListBox.SetCurSel(-1);


        // if this is a read only sequence - change the save button to say save as
        if (m_sequence->getReadOnly())
        {
            m_saveButton.SetWindowText("&Save Sequence As..");
        }
        else
        {
            m_saveButton.SetWindowText("&Save Sequence");
        }

        DisplayItemManager::getInstance().registerForCameraChanges(this);
        DisplayItemManager::getInstance().registerForSequenceChanges(this);

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SequenceEditorDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        DisplayItemManager::getInstance().deregisterForSequenceChanges(this);
        DisplayItemManager::getInstance().deregisterForCameraChanges(this);
        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Prevents application closing if user presses enter.
    void SequenceEditorDlg::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );
        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::LoadButtonIcons()
    {
        FUNCTION_ENTRY( "LoadButtonIcons" );

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
        FUNCTION_ENTRY( "loadExistingVideoSources" );

        m_sequencesListBox.ResetContent();
        std::ostringstream desc;
        for ( std::list<Camera*>::iterator iter = m_sequenceVideoSources.begin();
        iter != m_sequenceVideoSources.end(); iter++ )
        {
            desc.str("");
            desc << (*iter)->getName() << " - " << (*iter)->getDescription();
            m_sequencesListBox.addSourceString(desc.str().c_str());
        }

        FUNCTION_EXIT;
    }


    BOOL SequenceEditorDlg::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                default:
                    {
                        // do nothing
                    }
                    break;
                }
            }
            break;
        case WM_MOUSEMOVE:
            {
                handleTooltipsActivation(pMsg, m_addButton);
                handleTooltipsActivation(pMsg, m_removeButton);
                handleTooltipsActivation(pMsg, m_topButton);
                handleTooltipsActivation(pMsg, m_upButton);
                handleTooltipsActivation(pMsg, m_downButton);
                handleTooltipsActivation(pMsg, m_bottomButton);
            }
            break;
        default:
            {
                // do nothing
            }
        }

        FUNCTION_EXIT;
        return CDialog::PreTranslateMessage(pMsg);
    }


    void SequenceEditorDlg::OnUpdateSeDescriptionEditBox()
    {
        FUNCTION_ENTRY( "OnUpdateSeDescriptionEditBox" );

        UpdateData(TRUE);

        enableSaveButton();

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnUpdateSeDwelltimeEditBox()
    {
        FUNCTION_ENTRY( "OnUpdateSeDwelltimeEditBox" );

        enableSaveButton();

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeAddButton()
    {
        FUNCTION_ENTRY( "OnSeAddButton" );

        if ( m_sequenceVideoSources.size() < MAX_VIDEO_SOURCES )
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
        FUNCTION_ENTRY( "onDblclkCameratree" );

        Camera* selectedCamera = dynamic_cast<Camera*>(m_cameraTreeCtrl.getSelectedInput());

        // if a camera is selected, then add it to the list
        if (selectedCamera != NULL)
        {
            OnSeAddButton();
        }

        enableAddButton(true);

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeRemoveButton()
    {
        FUNCTION_ENTRY( "OnSeRemoveButton" );

        // get the index of the selected item in the list box
        int curSel = m_sequencesListBox.GetCurSel();

        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for (int i = 0; i < curSel; i++) {theIterator++;}

        // erase the item in the list
        m_sequenceVideoSources.erase(theIterator);

        // delete the item from the list box
        this->m_sequencesListBox.deleteSourceString();

        m_sequencesListBox.SetFocus();
        if (m_sequencesListBox.GetCount() == 0)
        {
            m_removeButton.EnableWindow(FALSE);
        }

        m_changed = isSourcesModified();
        enableSaveButton();

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeSaveButton()
    {
        FUNCTION_ENTRY( "OnSeSaveButton" );

        TA_ASSERT(m_sequence != NULL, "Sequence is null");

        // 
        // if this is a read only sequence pop up a save as dialog
        // 
        if (m_sequence->getReadOnly())
        {
            SequenceEditorSaveAsDlg saveAsDialog(this);
            int ret = saveAsDialog.DoModal();

            if (ret == IDOK)
            {
                // get the selected sequence
                Sequence* selectedSequence = saveAsDialog.getSelectedSequence();
                TA_ASSERT(selectedSequence != NULL, "Ok cannot be pressed if a sequence is not selected");

                // now save the details as the selected sequence
                try
                {
                    CString newDwellTime;
                    m_sequenceDwellTime.GetWindowText(newDwellTime);
                    unsigned short dwellTime = static_cast< unsigned short >( strtoul( newDwellTime, NULL, 10 ) );

                    if ( ( m_sequenceVideoSources.size() == 0 ) && ( dwellTime > 0 ) )
                    {
                        // set the dwell time to 0 for empty sequences
                        dwellTime = 0;

                        // set the dwell time box back to the default
                        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
                        TA_ASSERT(application != NULL, "Application is NULL");
                        newDwellTime.Format("%d", application->getDefaultDwellTime());
                        m_existingDwellTime = newDwellTime;
                        m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                        m_spinControl.SetPos( application->getDefaultDwellTime() );
                    }
                    if ( ( m_sequenceVideoSources.size() > 0 ) && ( dwellTime == 0 ) )
                    {
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << 0;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090078);

                        FUNCTION_EXIT;
                        return;
                    }

                    // Save the sequence
                    SwitchAgentCommunicator::getInstance().setSequenceConfig( selectedSequence, dwellTime, m_sequenceVideoSources );

                    m_existingDwellTime = newDwellTime;
                    m_changed = FALSE;

                    CString newDescription;
                    m_sequenceDescription.GetWindowText(newDescription);

                    SwitchAgentCommunicator::getInstance().setSequenceDescription( selectedSequence, newDescription.GetBuffer(0) );

                    // now set the current sequence
                    m_sequence = selectedSequence;
                    // force the display to update
                    onSequenceUpdated(NULL, reinterpret_cast<LPARAM>(m_sequence));
                }
                catch(TA_Base_Core::TransactiveException& te)
                {
                    CString errorNumber1 = "UE-090047";
                    CString errorNumber2 = "UE-090048";
                    CString errorNumber3 = "UE-020056";
                    CString errorNumber4 = "UE-010021";
                    CString message = "Saving sequence configuration";
                    TA_Base_Bus::TransActiveMessage userMsg;
                    // TD16879
                    if(te.what() == errorNumber1)
                    {
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                    }
                    else if(te.what() == errorNumber2)
                    {
                        userMsg << message;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                    }
                    else if(te.what() == errorNumber3)
                    {
                        userMsg << message;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                    }
                    else if(te.what() == errorNumber4)
                    {
						std::string userName;
						std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
						TA_Base_Bus::AuthenticationLibrary authLibrary;
						TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
						std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
						userName = iOperator->getName();
						userMsg << userName;
						userMsg << message;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                    }
                    else
                    {
                        userMsg << message;
                        userMsg<< te.what();
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
                    }
                    // TD16879
                    // AfxMessageBox( te.what() );
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

            try
            {
                if ( isSourcesModified() || ( m_existingDwellTime != newDwellTime ) )
                {
                    unsigned short dwellTime = static_cast< unsigned short >( strtoul( newDwellTime, NULL, 10 ) );

                    if ( ( m_sequenceVideoSources.size() == 0 ) && ( dwellTime > 0 ) )
                    {
                        // set the dwell time to 0 for empty sequences
                        dwellTime = 0;

                        // set the dwell time box back to the default
                        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
                        TA_ASSERT(application != NULL, "Application is NULL");
                        newDwellTime.Format("%d", application->getDefaultDwellTime());
                        m_existingDwellTime = newDwellTime;
                        m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                        m_spinControl.SetPos( application->getDefaultDwellTime() );
                    }
                    if ( ( m_sequenceVideoSources.size() > 0 ) && ( dwellTime == 0 ) )
                    {

                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << 0;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_090078);

                        FUNCTION_EXIT;
                        return;
                    }

                    // Save the sequence
                    SwitchAgentCommunicator::getInstance().setSequenceConfig( m_sequence, dwellTime, m_sequenceVideoSources );

                    m_existingDwellTime = newDwellTime;
                    m_changed = FALSE;
                }

                CString newDescription;
                m_sequenceDescription.GetWindowText(newDescription);

                if ( newDescription != m_sequence->getSequenceDescription().c_str() )// TD16691
                {
                    SwitchAgentCommunicator::getInstance().setSequenceDescription( m_sequence, newDescription.GetBuffer(0) );
                }

            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                CString errorNumber1 = "UE-090047";
                CString errorNumber2 = "UE-090048";
                CString errorNumber3 = "UE-020056";
                CString errorNumber4 = "UE-010021";
                CString message = "Saving sequence configuration";
                TA_Base_Bus::TransActiveMessage userMsg;
                // TD16879
                if(te.what() == errorNumber1)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                }
                else if(te.what() == errorNumber2)
                {
                    userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                }
                else if(te.what() == errorNumber3)
                {
                    userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                }
                else if(te.what() == errorNumber4)
                {
					std::string userName;
					std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					TA_Base_Bus::AuthenticationLibrary authLibrary;
					TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
					std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
					userName = iOperator->getName();
					userMsg << userName;
					userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                }
                else
                {
                    userMsg << message;
                    userMsg<< te.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
                }
                // TD16879
                // AfxMessageBox( te.what() );
            }
        }

        enableSaveButton();
        m_cancelButton.SetFocus();

        FUNCTION_EXIT;
    }


    void SequenceEditorDlg::OnSeTopButton()
    {
        FUNCTION_ENTRY( "OnSeTopButton" );

        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();

        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for (int i = 0; i < curSel; i++) {theIterator++;}

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
        FUNCTION_ENTRY( "OnSeUpButton" );

        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();

        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
		int i = 0;
        for (; i < curSel; i++) {theIterator++;}

        // get the item
        Camera* selectedCamera = *theIterator;

        // erase the item
        m_sequenceVideoSources.erase(theIterator);

        // insert the item one position up
        theIterator = m_sequenceVideoSources.begin();
        // theIterator += (curSel - 1);
        // no random access iterator for lists
        for (i = 0; i < (curSel-1); i++) {theIterator++;}
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
        FUNCTION_ENTRY( "OnSeDownButton" );

        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();

        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
		int i = 0;
        for (; i < curSel; i++) {theIterator++;}

        // get the item
        Camera* selectedCamera = *theIterator;

        // erase the item
        m_sequenceVideoSources.erase(theIterator);

        // insert the item one position down
        theIterator = m_sequenceVideoSources.begin();
        // theIterator += (curSel + 1);
        // no random access iterator for lists
        for (i = 0; i < (curSel+1); i++) {theIterator++;}
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
        FUNCTION_ENTRY( "OnSeBottomButton" );

        // get the selected item
        int curSel = m_sequencesListBox.GetCurSel();

        // get an iterator to the beginning of the camera list
        std::list<Camera*>::iterator theIterator;
        theIterator = m_sequenceVideoSources.begin();
        // get an iterator to the selected item in the list
        // theIterator+= curSel;
        // no random access iterator for lists
        for (int i = 0; i < curSel; i++) {theIterator++;}

        // get the item
        Camera* selectedCamera = *theIterator;

        // erase the item in the list
        m_sequenceVideoSources.erase(theIterator);

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
        FUNCTION_ENTRY( "setToolTips" );

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
        FUNCTION_ENTRY( "handleTooltipsActivation" );

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

        if (FALSE == button.IsWindowEnabled())
        {
            // ---------------------------------
            // rect is the control rectangle in
            // Dialog client coordinates
            // ----------------------------------
            button.GetWindowRect(&rect);
            ScreenToClient(&rect);

            if(rect.PtInRect(pt) )
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
        FUNCTION_ENTRY( "loadTree" );

        m_cameraTreeCtrl.setToSequenceEditorTree();

        // load the cameras
        std::map<std::string, Camera*> cameras =
            DisplayItemManager::getInstance().getAllCameras();

        // build a vector
        std::vector<VideoInput*> inputs;
        for ( std::map<std::string, Camera*>::iterator it = cameras.begin(); it != cameras.end(); it++ )
        {
            // insert the item
            inputs.push_back( it->second );
        }

        m_cameraTreeCtrl.insertVideoSources(inputs);

        FUNCTION_EXIT;
    }


    LRESULT SequenceEditorDlg::onSequenceUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onSequenceUpdated" );

        // lParam holds the DisplayItem* that was updated
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Sequence update received for NULL display item");
        VideoInput* input = dynamic_cast<VideoInput*>(item);
        TA_ASSERT(input != NULL, "Sequence update received for NULL display item");

        VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
        TA_ASSERT(application != NULL, "Application is NULL");

        if (input == m_sequence)
        {
            // the current sequence was updated

            CString title("Sequence Editor - ");
            title += m_sequence->getName().c_str();
            if (m_sequence->getReadOnly())
            {
                title += " (Read Only)";
            }
            SetWindowText(title);

            m_sequenceLabel.SetText(m_sequence->getName().c_str());

            if (m_sequence->getReadOnly())
            {
                m_saveButton.SetWindowText("&Save Sequence As..");
            }
            else
            {
                m_saveButton.SetWindowText("&Save Sequence");
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
            // TES 725
            // if the seqeunce is empty - the dwell time is 0.
            // set it to the default dwell time
            if (dwellTime == 0)
            {
                dwellTime = application->getDefaultDwellTime();
            }
            m_existingDwellTime.Format("%d", dwellTime);
            bool dwellTimeNeedsUpdate = (m_existingDwellTime != newDwellTime);

            // test the local cameras against the saved cameras
            same = same && !isSourcesModified();

            if ( !same || dwellTimeNeedsUpdate )
            {
                // set the dwell time
                m_sequenceDwellTime.SetWindowText(m_existingDwellTime);
                m_spinControl.SetPos( dwellTime );

                // set the description
                m_sequenceDescription.SetWindowText(m_sequence->getSequenceDescription().c_str()); // TD16691

                // set the cameras
                m_sequenceVideoSources = m_sequence->getCameras();
                loadExistingVideoSources();

                // get the current time
                time_t currentTime;
                ACE_OS::time( &currentTime );

                if ( (m_lastUpdatedTime == 0) ||
                     ((currentTime - m_lastUpdatedTime) > UPDATE_THRESHOLD_TIME) )
                {
                    // set the last updated time
                    m_lastUpdatedTime = currentTime;
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_090072);
                    // AfxMessageBox( "The dwell time and/or ordered list of cameras for the currently loaded Sequence have been modified!" );
                }
                else
                {
                    // set the last updated time
                    m_lastUpdatedTime = currentTime;
                }
                // TD16691
                m_sequenceTreeCtrl->videoInputUpdated(input);
                // TD16691
            }
        }

        FUNCTION_EXIT;
        return 0;
    }


    LRESULT SequenceEditorDlg::onCameraUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "onCameraUpdated" );

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


} // TA_IRS_App
