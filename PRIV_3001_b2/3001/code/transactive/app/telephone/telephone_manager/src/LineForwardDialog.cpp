/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/LineForwardDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// LineForwardDialog.cpp : implementation file
//

#include "stdafx.h"
#include "telephone_manager.h"
#include "LineForwardDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LineForwardDialog dialog


LineForwardDialog::LineForwardDialog(CWnd* pParent /*=NULL*/)
    : CDialog(LineForwardDialog::IDD, pParent)
{
    FUNCTION_ENTRY( "LineForwardDialog" );

    // {{AFX_DATA_INIT(LineForwardDialog)

    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void LineForwardDialog::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(LineForwardDialog)
    DDX_Control(pDX, IDCB_NUMBER, m_numberCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(LineForwardDialog, CDialog)
    // {{AFX_MSG_MAP(LineForwardDialog)
    ON_BN_CLICKED(IDOK, onOK)
    ON_BN_CLICKED(IDC_IMM_FORWARD, onForwardImmediate)
    ON_BN_CLICKED(IDC_DEL_FORWARD, onForwardDelayed)
    ON_BN_CLICKED(IDC_BUS_FORWARD, onForwardBusy)
    ON_BN_CLICKED(IDC_NEV_FORWARD, onForwardNever)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioForwardDialog message handlers

BOOL LineForwardDialog::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // Set the defaul radio button
    this->CheckRadioButton(IDC_IMM_FORWARD,IDC_NEV_FORWARD,IDC_NEV_FORWARD);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void LineForwardDialog::onOK()
{
    FUNCTION_ENTRY( "onOK" );

    // Perform the forward setting

    m_statusStr = buildStatusString();
    m_forwardEnabled = (!this->IsDlgButtonChecked(IDC_NEV_FORWARD));

    EndDialog(IDOK);

    FUNCTION_EXIT;
}


void LineForwardDialog::onForwardImmediate()
{
    FUNCTION_ENTRY( "onForwardImmediate" );

    this->CheckRadioButton(IDC_IMM_FORWARD,IDC_NEV_FORWARD,IDC_IMM_FORWARD);

    FUNCTION_EXIT;
}


void LineForwardDialog::onForwardDelayed()
{
    FUNCTION_ENTRY( "onForwardDelayed" );

    this->CheckRadioButton(IDC_IMM_FORWARD,IDC_NEV_FORWARD,IDC_DEL_FORWARD);

    FUNCTION_EXIT;
}


void LineForwardDialog::onForwardBusy()
{
    FUNCTION_ENTRY( "onForwardBusy" );

    this->CheckRadioButton(IDC_IMM_FORWARD,IDC_NEV_FORWARD,IDC_BUS_FORWARD);

    FUNCTION_EXIT;
}


void LineForwardDialog::onForwardNever()
{
    FUNCTION_ENTRY( "onForwardNever" );

    this->CheckRadioButton(IDC_IMM_FORWARD,IDC_NEV_FORWARD,IDC_NEV_FORWARD);

    FUNCTION_EXIT;
}


CString LineForwardDialog::getStatusString()
{
    FUNCTION_ENTRY( "getStatusString" );
    FUNCTION_EXIT;
    return m_statusStr;
}


CString LineForwardDialog::buildStatusString()
{
    FUNCTION_ENTRY( "buildStatusString" );

    CString number;
    m_numberCombo.GetWindowText(number);

    if(IsDlgButtonChecked(IDC_NEV_FORWARD))
    {
        FUNCTION_EXIT;
        return "";
    }

    if(IsDlgButtonChecked(IDC_IMM_FORWARD))
    {
        FUNCTION_EXIT;
        return "IMMEDIATE FORWARD TO " + number;
    }
    if(IsDlgButtonChecked(IDC_DEL_FORWARD))
    {
        FUNCTION_EXIT;
        return "FORWARD ON NO REPLY TO " + number;
    }
    if(IsDlgButtonChecked(IDC_BUS_FORWARD))
    {
        FUNCTION_EXIT;
        return "FORWARD ON BUSY TO " + number;
    }


}


