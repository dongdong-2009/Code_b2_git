/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
// CallStackCurrentPage.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"

#include "CallStackCurrentPage.h"
#include "CurrentCall.h"
#include "RadioManager.h"
#include "RadioManagerDlg.h"
#include "RmLayoutHelper.h"
#include "SelectedCall.h"

#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"

#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/utilities/src/runparams.h"

#include <time.h>


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CCallStackCurrentPage, CDialog)


/////////////////////////////////////////////////////////////////////////////
// CCallStackCurrentPage property page

CCallStackCurrentPage::CCallStackCurrentPage()
                      : CDialog(CCallStackCurrentPage::IDD),
                        m_incomingCallList(TA_IRS_App::CRadioCallStackListCtrl::EStackType::IncomingStack),
                        m_heldCallList(TA_IRS_App::CRadioCallStackListCtrl::EStackType::HeldStack)
{
    FUNCTION_ENTRY( "CCallStackCurrentPage" );

    //{{AFX_DATA_INIT(CCallStackCurrentPage)
    //}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CCallStackCurrentPage::~CCallStackCurrentPage()
{
    FUNCTION_ENTRY( "~CCallStackCurrentPage" );
    FUNCTION_EXIT;
}


void CCallStackCurrentPage::setRadioSessionReference(long radioSessionRef)
{
    FUNCTION_ENTRY( "setRadioSessionReference" );

    //    m_incomingCallList.setSessionRef(radioSessionRef);
//    m_heldCallList.setSessionRef(radioSessionRef);

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::setRadioEntityName(std::string radioEntityName)
{
    FUNCTION_ENTRY( "setRadioEntityName" );

    m_incomingCallList.setRadioName(radioEntityName);
    m_heldCallList.setRadioName(radioEntityName);

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCallStackCurrentPage)
    DDX_Control(pDX, IDC_RICHEDIT_TIP, m_richedtTIP);
    DDX_Control(pDX, IDC_ACCEPT, m_acceptButton);
    DDX_Control(pDX, IDC_RECALL, m_recallButton);
    DDX_Control(pDX, IDC_CALLLIST_IN, m_incomingCallList);
    DDX_Control(pDX, IDC_CALLLIST_HOLD, m_heldCallList);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CCallStackCurrentPage, CDialog)
    //{{AFX_MSG_MAP(CCallStackCurrentPage)
    ON_BN_CLICKED(IDC_ACCEPT, OnAccept)
    ON_BN_CLICKED(IDC_RECALL, onRecall)
    ON_NOTIFY(NM_DBLCLK, IDC_CALLLIST_IN, onDblclkCalllistIn)
    ON_NOTIFY(NM_DBLCLK, IDC_CALLLIST_HOLD, onDblclkCalllistHold)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_CALLLIST_HOLD, onItemchangedCalllistHold)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_CALLLIST_IN, onItemchangedCalllistIn)
    ON_MESSAGE(WM_CALLID_RPARAM_CHANGE, OnCallIdChange)
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CCallStackCurrentPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    // Adjust our font
    CDialog::OnInitDialog();

    const DWORD TIME_COL_WIDTH = 120;
    const DWORD CALLER_COL_WIDTH = 200;
    const DWORD TSI_COL_WIDTH = 100;
    const DWORD LOC_COL_WIDTH = 50;
    const DWORD STATUS_COL_WIDTH = 80;

    // Setup the incoming call list headings
    m_incomingCallList.setColumnName(0, "Date/Time");
    m_incomingCallList.setColumnName(1, "Caller");
    m_incomingCallList.setColumnName(2, "TSI");
    m_incomingCallList.setColumnName(3, "Loc");
    m_incomingCallList.setColumnName(4, "Status");
    m_incomingCallList.setColumnName(5, "Reference");
    m_incomingCallList.setColumnName(6, "Type");
    m_incomingCallList.SetColumnWidth(0, TIME_COL_WIDTH);
    m_incomingCallList.SetColumnWidth(1, CALLER_COL_WIDTH);
    m_incomingCallList.SetColumnWidth(2, TSI_COL_WIDTH);
    m_incomingCallList.SetColumnWidth(3, LOC_COL_WIDTH);
    m_incomingCallList.SetColumnWidth(4, STATUS_COL_WIDTH);
    m_incomingCallList.SetColumnWidth(5, 0);
    m_incomingCallList.SetColumnWidth(6, 0);
    m_incomingCallList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // TES #422 - Provide default formatting
    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_incomingCallList, 4);

    // Setup the held call list headings
    m_heldCallList.setColumnName(0,     "Date/Time");
    m_heldCallList.setColumnName(1,     "Called");
    m_heldCallList.setColumnName(2,     "TSI");
    m_heldCallList.setColumnName(3,     "Loc");
    m_heldCallList.setColumnName(4,     "Status");
    m_heldCallList.setColumnName(5,     "Reference");
    m_heldCallList.setColumnName(6,     "Type");
    m_heldCallList.SetColumnWidth(0, TIME_COL_WIDTH);
    m_heldCallList.SetColumnWidth(1, CALLER_COL_WIDTH);
    m_heldCallList.SetColumnWidth(2, TSI_COL_WIDTH);
    m_heldCallList.SetColumnWidth(3, LOC_COL_WIDTH);
    m_heldCallList.SetColumnWidth(4, STATUS_COL_WIDTH);
    m_heldCallList.SetColumnWidth(5, 0);
    m_heldCallList.SetColumnWidth(6, 0);
    m_heldCallList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // TES #422 - Provide default formatting
    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_heldCallList, 4);

    //TD9399, hongzhi
    CString strTemp;
    strTemp = "Multi-Select\r\nMax limit:\r\nOCC-5GP\r\nPSC-2GP";
    m_richedtTIP.SetWindowText(strTemp);
    m_richedtTIP.SetSel(0,24);
    CHARFORMAT cf;
    m_richedtTIP.GetSelectionCharFormat(cf);
    cf.dwEffects = CFE_UNDERLINE;
    m_richedtTIP.SetSelectionCharFormat(cf);
    //COLORREF cr(RGB(212,208,200));
    COLORREF cr(GetSysColor(COLOR_3DFACE));
    m_richedtTIP.SetBackgroundColor(false,cr);

    FUNCTION_EXIT;
    return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CCallStackCurrentPage::Init()
{
    FUNCTION_ENTRY( "Init" );

    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, CRadioManager::RPARAM_CALLID);
    answerRunParamCall();

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

    FUNCTION_EXIT;
}


// ExceptionChecked
void CCallStackCurrentPage::answerRunParamCall()
{
    FUNCTION_ENTRY( "answerRunParamCall" );

    try
    {
        // select the given call (if set)
        if ( TA_Base_Core::RunParams::getInstance().isSet(CRadioManager::RPARAM_CALLID) )
        {
            // get it from runparams
            std::stringstream callIdStream;
            callIdStream << TA_Base_Core::RunParams::getInstance().get(CRadioManager::RPARAM_CALLID);

            // convert to a number
            long callIdToSelect = -1;
            callIdStream >> callIdToSelect;

            // if valid, select it
            if ( callIdToSelect > -1 )
            {
                // if it was selected
                if ( m_incomingCallList.selectCall(callIdToSelect) )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                             "%s parameter set. Selecting call %d.",
                             CRadioManager::RPARAM_CALLID, callIdToSelect);

                    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

                    // Now answer the call, provided we're allowed to (TD #3081)
                    if (manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_ANSWER_CALL))
                    {
                        OnAccept();
                    }
                    else
                    {
                        // Important to give the user feedback, so they don't think the call banner
                        // simply doesn't work..
                        // TD14164 ++
                        TA_Base_Bus::TransActiveMessage userMsg;
                        CString errMsg = userMsg.constructMessage(IDS_UE_610010);
                        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Operator does not have the necessary rights to execute this request.");*/
                        RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
                        // TD14164 ++
                    }
                }
                else
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                             "%s parameter set. Selecting call %d failed.",
                             CRadioManager::RPARAM_CALLID, callIdToSelect);
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "%s parameter set incorrectly. Cannot select %s",
                         CRadioManager::RPARAM_CALLID,
                         TA_Base_Core::RunParams::getInstance().get(CRadioManager::RPARAM_CALLID).c_str());
            }

            onUpdateRights();
        }
    }
    SILENT_RM_CATCH_HANDLER("CallStackCurrentPage::answerRunParamCall");

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onUpdateRights()
{
    FUNCTION_ENTRY( "onUpdateRights" );

    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

        updateAcceptButtonState();

        bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL);
        m_incomingCallList.setCallTrainPermitted(enable);
        m_heldCallList.setCallTrainPermitted(enable);
    }
    SILENT_RM_CATCH_HANDLER("CallStackCurrentPage::onUpdateRights");

    FUNCTION_EXIT;
}


afx_msg void CCallStackCurrentPage::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Do nothing

    FUNCTION_EXIT;
}


afx_msg void CCallStackCurrentPage::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // Do nothing

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::OnAccept()
{
    FUNCTION_ENTRY( "OnAccept" );

    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

        if (!manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_ANSWER_CALL))
        {
            // Shouldn't rely on this check, GUI shouldn't permit entry into this function without this right..
            FUNCTION_EXIT;
            return;
        }

        // Get selected incoming call id
        if(m_incomingCallList.IsCallSelected() == false)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            CString errMsg = userMsg.constructMessage(IDS_UW_632001);
            RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
                errMsg,
                "Radio Manager",
                MB_OK | MB_ICONEXCLAMATION);
        }
        else
        {
            TA_IRS_App::CRadioCallStackListCtrl::ECallSource source;
            long itemID = m_incomingCallList.getSelectedItemWithType(source);
            if(source == TA_IRS_App::CRadioCallStackListCtrl::Train)
            {
                acceptTrainCall( itemID );
            }
            else
            {
                acceptRadioCall( itemID );
            }
        }
    }
    SILENT_RM_CATCH_HANDLER("CallStackCurrentPage::OnAccept");

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::acceptRadioCall(long callID)
{
    FUNCTION_ENTRY( "acceptRadioCall" );

    TA_Base_Bus::IRadioCorbaDef::ECallCategory    callType;

    try
    {
        CORBA_CALL_RETURN( callType, ( (CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd) )->getRadioCorbaDef(), getCallCategory,
                           ( callID, ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );
    }
    catch(TA_Base_Bus::IRadioCorbaDef::invalidCallIDException& ex)
    {
        std::string reason = "Call not found within system. Reason: "
            + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "IRadioCorbaDef::invalidCallIDException",reason.c_str());
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632001);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
        "Call not found in system. Answer not possible.",
        "Radio Manager",
        MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager",
            MB_OK | MB_ICONEXCLAMATION);
        // ++ TD14164
        FUNCTION_EXIT;
        return;
    }
    catch(...)
    {
        std::string reason = "Error answering call.";
        LOG_EXCEPTION_CATCH(SourceInfo, "unknown",reason.c_str());
        // TD14164 ++
        TA_Base_Bus::TransActiveMessage userMsg;
        CString actionName = "determine call category";
        userMsg << actionName;
        CString errMsg = userMsg.constructMessage(IDS_UW_632002);
        /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
        "Error while attempting to determine call category.",
        "Radio Manager",
        MB_OK | MB_ICONEXCLAMATION);*/
        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd,
            errMsg,
            "Radio Manager",
            MB_OK | MB_ICONEXCLAMATION);
        // ++ TD14164
        FUNCTION_EXIT;
        return;
    }

    CString message;

    switch (callType)
    {
    case TA_Base_Bus::IRadioCorbaDef::ECallCategory::FullDuplex:
    case TA_Base_Bus::IRadioCorbaDef::ECallCategory::HalfDuplex:
        // Answer the call
        {
            try
            {
                CORBA_CALL( ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getRadioSessionCorbaDef(), answerCall, (callID,TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str()) );//limin
            }
            catch(TA_Base_Bus::IRadioSessionCorbaDef::accessDeniedException& ex)
            {
                std::string reason = "Access denied when attempting to answer call. Reason: "
                    + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
                LOG_EXCEPTION_CATCH(SourceInfo, "IRadioSessionCorbaDef::accessDeniedException",reason.c_str());
                // TD14164 ++
                TA_Base_Bus::TransActiveMessage userMsg;
                CString errMsg = userMsg.constructMessage(IDS_UW_632003);
                /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, "Not permitted to answer this call","Radio Manager", MB_OK | MB_ICONEXCLAMATION);*/
                RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg, "Radio Manager", MB_OK | MB_ICONEXCLAMATION);
                // ++ TD14164
            }
            catch(TA_Base_Bus::IRadioSessionCorbaDef::radioAPIException& ex)
            {
                std::string reason = "Radio API error. Reason: "
                    + TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
                LOG_EXCEPTION_CATCH(SourceInfo, "IRadioSessionCorbaDef::radioAPIException",reason.c_str());
                // TD14164 ++
                TA_Base_Bus::TransActiveMessage userMsg;
                CString errMsg = userMsg.constructMessage(IDS_UW_632004);
                /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, "Radio system error while attempting to answer call","Radio Manager", MB_OK | MB_ICONEXCLAMATION);*/
                RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
                // ++ TD14164
            }
            catch(...)
            {
                std::string reason = "Error answering call.";
                LOG_EXCEPTION_CATCH(SourceInfo, "unknown",reason.c_str());
                // TD14164 ++
                TA_Base_Bus::TransActiveMessage userMsg;
                CString actionName = "answer call";
                userMsg << actionName;
                CString errMsg = userMsg.constructMessage(IDS_UW_632002);
                /*RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, "Error while attempting to answer call.","Radio Manager", MB_OK | MB_ICONEXCLAMATION);*/
                RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
                // ++ TD14164
            }
            break;
        }
    case TA_Base_Bus::IRadioCorbaDef::ECallCategory::TextMessage:
        {
            AfxGetApp()->GetMainWnd()->PostMessage(WM_DISPLAY_SDS_MESSAGE, callID, 0);
            //displaySDSMessage(callID);
            break;
        }

    default:
        TRACE0("Invalid call type in OnAcceptCall");
    }

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::acceptTrainCall(TA_IRS_Bus::CommonTypes::TrainIdType trainId)
{
    FUNCTION_ENTRY( "acceptTrainCall" );

    try
    {
        CSelectedCall::GetInstance()->answerDriverCall(trainId);
    }
    catch ( TA_IRS_Bus::AgentCommunicationException& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::AgentCommunicationException", ex.what() );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString reasonMsg = "- Error communicating with train agent";
        userMsg << reasonMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }
    catch( TA_IRS_Bus::TrainAccessDeniedException& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", ex.what() );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString reasonMsg = "- Access Denied";
        userMsg << reasonMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }
    catch( TA_IRS_Bus::InvalidTrainException& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::InvalidTrainException", ex.what() );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString reasonMsg = "- No call request from this train";
        userMsg << reasonMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }
    catch( TA_IRS_Bus::TrainTransmissionFailureException& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::TransmissionFailureException", ex.what() );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString reasonMsg = "- Error while sending acknowledgement to train";
        userMsg << reasonMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }
    catch( TA_IRS_Bus::TrainRadioException& ex )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Bus::TrainRadioException", ex.what() );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString reasonMsg = "- Error setting up audio to train";
        userMsg << reasonMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "...", "While answering OCC call" );

        TA_Base_Bus::TransActiveMessage userMsg;
        CString errMsg = userMsg.constructMessage(IDS_UW_632005);

        RadioManagerErrorHandler::getInstance().MessageBox(m_hWnd, errMsg,"Radio Manager", MB_OK | MB_ICONEXCLAMATION);
    }

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onRecall()
{
    FUNCTION_ENTRY( "onRecall" );

    // Resume the call
    try
    {
        // Get selected incoming call id
        long callID = m_heldCallList.getSelectedCallID();

        CORBA_CALL( ((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getRadioSessionCorbaDef(), resumeCall, ( callID,TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str() ) );//limin
    }
    SILENT_RM_CATCH_HANDLER("CCallStackCurrentPage::onRecall");

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onDblclkCalllistIn(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onDblclkCalllistIn" );

    if (m_acceptButton.IsWindowEnabled())
    {
        OnAccept();
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onDblclkCalllistHold(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onDblclkCalllistHold" );

    if (m_recallButton.IsWindowEnabled())
    {
        onRecall();
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onItemchangedCalllistHold(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onItemchangedCalllistHold" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    try
    {
        // Enable the recall button if there is a selected items
        m_recallButton.EnableWindow(m_heldCallList.IsCallSelected());
    }
    SILENT_RM_CATCH_HANDLER("CCallStackCurrentPage::onItemchangedCalllistHold");

    *pResult = 0;

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::onItemchangedCalllistIn(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onItemchangedCalllistIn" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    updateAcceptButtonState();

    *pResult = 0;

    FUNCTION_EXIT;
}


void CCallStackCurrentPage::updateAcceptButtonState()
{
    FUNCTION_ENTRY( "updateAcceptButtonState" );

    try
    {
        bool enable = getShouldEnableAnswerButton();
        m_acceptButton.EnableWindow(enable);

        // TES#555 - Update the related answer toolbar button (to keep buttons in sync)
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        manager->refreshAnswerToolbarButton();
    }
    SILENT_RM_CATCH_HANDLER("CCallStackCurrentPage::onItemchangedCalllistHold");

    FUNCTION_EXIT;
}


bool CCallStackCurrentPage::getShouldEnableAnswerButton()
{
    FUNCTION_ENTRY( "getShouldEnableAnswerButton" );

    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());

        // Enable the recall button if there is a selected items
        bool hasRight = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_ANSWER_CALL);

        FUNCTION_EXIT;
        return hasRight && m_incomingCallList.IsCallSelected();
    }
    SILENT_RM_CATCH_HANDLER("getShouldEnableAnswerButton");

    // Default to no right to accept call
    FUNCTION_EXIT;
    return false;
}


void CCallStackCurrentPage::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY( "onRunParamChange" );

    // TD #3081
    if (CRadioManager::RPARAM_CALLID == name)
    {
        // Expect will have de-registered before the window handle became invalid, check anyway
        if (0 != m_hWnd)
        {
            PostMessage(WM_CALLID_RPARAM_CHANGE, 0, 0);
        }
    }

    FUNCTION_EXIT;
}


afx_msg LRESULT CCallStackCurrentPage::OnCallIdChange(UINT wParam, LONG lParam)
{
    FUNCTION_ENTRY( "OnCallIdChange" );

    answerRunParamCall();

    FUNCTION_EXIT;
    return 0;
}


