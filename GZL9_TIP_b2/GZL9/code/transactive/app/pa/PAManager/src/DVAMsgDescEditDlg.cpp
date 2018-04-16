/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"
#include "app/pa/PAManager/src/DVAMsgDescEditDlg.h"


IMPLEMENT_DYNAMIC(DVAMsgDescEditDlg, CDialog)

DVAMsgDescEditDlg::DVAMsgDescEditDlg( PaRuntimeModel& refBroadcastModel, int nMsgType, CWnd* pParent ) : 
CDialog(DVAMsgDescEditDlg::IDD, pParent),
m_nMsgType( nMsgType ),
m_strCurDescription( "" ),
m_refBroadcastModel( refBroadcastModel )
{
}

DVAMsgDescEditDlg::~DVAMsgDescEditDlg()
{
}

void DVAMsgDescEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DVAMsgDescEditDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL DVAMsgDescEditDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_strCurDescription = m_refBroadcastModel.getCurDVAMsgDesc( m_nMsgType );

    SetDlgItemText( IDC_EDIT_BROADCAST_DES, m_strCurDescription.c_str() );

    return TRUE;
}

// DVAMsgDescEditDlg message handlers
std::string& DVAMsgDescEditDlg::getBroadcastDes()
{
    return m_strCurDescription;
}

void DVAMsgDescEditDlg::OnBnClickedOk()
{
    char strBroadcastDes[DVA_MSG_DESC_BUFFER_SIZE] = { 0 };
    GetDlgItemText( IDC_EDIT_BROADCAST_DES, strBroadcastDes, DVA_MSG_DESC_BUFFER_SIZE );
    std::string strNewBroadcastDes = strBroadcastDes;

    // If no change, just act as cancel operation
    if ( 0 == m_strCurDescription.compare( strBroadcastDes ))
    {
        OnCancel();
    }
    else if ( DVA_DESC_MAX_CHAR_COUNT < strNewBroadcastDes.length() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_DVA_MSG_DESC_MAX_CHARS );
        return;
    }

    int nRet = m_refBroadcastModel.modifyCurDVAMsgDesc( m_nMsgType, strNewBroadcastDes );

    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    m_strCurDescription = strNewBroadcastDes;

    // Need to change, update the description
    OnOK();
}
