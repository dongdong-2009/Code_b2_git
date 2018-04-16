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
// CallStackOtherPage.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"

#include "CallStackOtherPage.h"
#include "CurrentCall.h"
#include "RadioManagerDlg.h"
#include "RmLayoutHelper.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/utilities/src/runparams.h"

#include <time.h>


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCallStackOtherPage, CDialog)

/////////////////////////////////////////////////////////////////////////////
// CCallStackOtherPage property page

CCallStackOtherPage::CCallStackOtherPage()
                    : CDialog(CCallStackOtherPage::IDD),
                      m_OtherCallStack(TA_IRS_App::CRadioCallStackListCtrl::EStackType::OthersStack)
{
    FUNCTION_ENTRY( "CCallStackOtherPage" );

    //{{AFX_DATA_INIT(CCallStackOtherPage)
    //}}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CCallStackOtherPage::~CCallStackOtherPage()
{
    FUNCTION_ENTRY( "~CCallStackOtherPage" );
    FUNCTION_EXIT;
}


void CCallStackOtherPage::setRadioSessionReference(long radioSessionRef)
{
    FUNCTION_ENTRY( "setRadioSessionReference" );

    //m_OtherCallStack.setSessionRef(radioSessionRef);

    FUNCTION_EXIT;
}


void CCallStackOtherPage::setRadioEntityName(std::string radioEntityName)
{
    FUNCTION_ENTRY( "setRadioEntityName" );

    m_OtherCallStack.setRadioName(radioEntityName);

    FUNCTION_EXIT;
}


void CCallStackOtherPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCallStackOtherPage)
    DDX_Control(pDX, IDC_OTHERCALLLIST, m_OtherCallStack);
    //}}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CCallStackOtherPage, CDialog)
    //{{AFX_MSG_MAP(CCallStackOtherPage)
    ON_BN_CLICKED(IDC_KILL, onKillCallButton)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_OTHERCALLLIST, onItemChangedOtherCalllist)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CCallStackOtherPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    const DWORD TIME_COL_WIDTH = 125;
    const DWORD NAME_COL_WIDTH = 100;
    const DWORD LOC_COL_WIDTH = 85;
    const DWORD STATUS_COL_WIDTH = 60;

    m_OtherCallStack.setColumnName(0,"Date/Time");
    m_OtherCallStack.setColumnName(1,"Caller ID");
    m_OtherCallStack.setColumnName(2,"Source");
    m_OtherCallStack.setColumnName(3,"Called ID");
    m_OtherCallStack.setColumnName(4,"Destination");
    m_OtherCallStack.setColumnName(5,"Status");
    m_OtherCallStack.setColumnName(6,"Reference");
    m_OtherCallStack.setColumnName(7,"Type");
    m_OtherCallStack.SetColumnWidth(0,  TIME_COL_WIDTH);
    m_OtherCallStack.SetColumnWidth(1,  NAME_COL_WIDTH);
    m_OtherCallStack.SetColumnWidth(2,  LOC_COL_WIDTH);

    m_OtherCallStack.SetColumnWidth(3,  NAME_COL_WIDTH);
    m_OtherCallStack.SetColumnWidth(4,  LOC_COL_WIDTH);

    m_OtherCallStack.SetColumnWidth(5,  STATUS_COL_WIDTH);
    m_OtherCallStack.SetColumnWidth(6,0);
    m_OtherCallStack.SetColumnWidth(7,0);

    m_OtherCallStack.SetExtendedStyle(LVS_EX_FULLROWSELECT);

    // TES #422 - Provide default formatting
    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(m_OtherCallStack, 5);

    FUNCTION_EXIT;
    return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CCallStackOtherPage::Init()
{
    FUNCTION_ENTRY( "Init" );

    onUpdateRights();

    FUNCTION_EXIT;
}


void CCallStackOtherPage::onUpdateRights()
{
    FUNCTION_ENTRY( "onUpdateRights" );

    try
    {
        // Check if Kill button should be enabled.
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        bool enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_TERMINATE_CALL);
        GetDlgItem(IDC_KILL)->EnableWindow(enable&&(m_OtherCallStack.GetSelectedCount()>0));

        enable = manager->isFunctionPermitted(TA_Base_Bus::aca_RADIO_MAKE_TRAIN_CALL);
        m_OtherCallStack.setCallTrainPermitted(enable);
    }
    SILENT_RM_CATCH_HANDLER("CCallStackOtherPage::onUpdateRights");

    FUNCTION_EXIT;
}


void CCallStackOtherPage::onKillCallButton()
{
    FUNCTION_ENTRY( "onKillCallButton" );

    long  callID;
    std::vector<long> callList;

    try
    {
        CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(AfxGetApp()->GetMainWnd());
        TA_ASSERT(manager!=NULL,"Radio manager dialog not defined.");
        m_OtherCallStack.getSelectedCallList(callList);
        while (!callList.empty())
        {
            callID = callList.back();
            callList.pop_back();

            manager->getRadioSessionCorbaDef()->terminateCall(callID,TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str(),false); // TD14247
        }
    }
    SILENT_RM_CATCH_HANDLER("onKillCallButton");

    FUNCTION_EXIT;
}


afx_msg void CCallStackOtherPage::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Do nothing

    FUNCTION_EXIT;
}


afx_msg void CCallStackOtherPage::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    // Do nothing

    FUNCTION_EXIT;
}


void CCallStackOtherPage::onItemChangedOtherCalllist(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onItemChangedOtherCalllist" );

    onUpdateRights();
    *pResult = 0;

    FUNCTION_EXIT;
}


