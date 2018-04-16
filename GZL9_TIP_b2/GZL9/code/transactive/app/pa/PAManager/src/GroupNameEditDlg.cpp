// ./src/GroupNameEditDlg.cpp : implementation file
//

#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"
#include "app/pa/PAManager/src/GroupNameEditDlg.h"


// GroupNameEditDlg dialog

IMPLEMENT_DYNAMIC(GroupNameEditDlg, CDialog)

GroupNameEditDlg::GroupNameEditDlg( PaRuntimeModel& refBroadcastModel, const std::string& strGroupName, CWnd* pParent ) : 
CDialog(GroupNameEditDlg::IDD, pParent),
m_strGroupName( strGroupName ),
m_ulNewGroupKey( 0 ),
m_refBroadcastModel( refBroadcastModel )
{
    if ( 0 == m_strGroupName.compare( GROUP_NAME_CUR_SEL ))
    {
        m_strGroupName = "";
    }
}

GroupNameEditDlg::~GroupNameEditDlg()
{
}

void GroupNameEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GroupNameEditDlg, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL GroupNameEditDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetDlgItemText( IDC_EDIT_GROUP_NAME, m_strGroupName.c_str() );
    
    return TRUE;
}

// GroupNameEditDlg message handlers

void GroupNameEditDlg::OnBnClickedOk()
{
    if ( UpdateGroupName() )
    {
        OnOK();
    }
}

bool GroupNameEditDlg::UpdateGroupName()
{
    char strGroupName[GROUP_NAME_BUFFER_SIZE] = { 0 };
    GetDlgItemText( IDC_EDIT_GROUP_NAME, strGroupName, GROUP_NAME_BUFFER_SIZE );
    m_strGroupName = strGroupName;

    if ( m_strGroupName.empty() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_GROUP_NAME_EMPTY );
        return false;
    }
    else if ( 0 == m_strGroupName.compare( GROUP_NAME_CUR_SEL ))
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_GROUP_NAME_SAME_AS_CUR_SEL );
        return false;
    }
    else if (  GROUP_NAME_MAX_CHAR_COUNT < m_strGroupName.length() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_GROUP_NAME_MAX_CHARS );
        return false;
    }
    else if ( PaDataManager::getInstance().isGroupNameExist( m_strGroupName ))
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_ZONE_GROUP_NAME_ALREADY_EXIST );
        return false;
    }

    std::vector<unsigned long> lstZoneKeys = m_refBroadcastModel.getActiveSelZoneKeysList();
    int nRet = PaDataManager::getInstance().createZoneGroupByZoneKeys( m_ulNewGroupKey, m_strGroupName, lstZoneKeys );
    if ( PaErrorHelper::checkError( nRet ))
    {
        return false;
    }

    return true;
}

std::string& GroupNameEditDlg::getGroupName()
{
    return m_strGroupName;
}

unsigned long GroupNameEditDlg::getNewGroupKey()
{
    return m_ulNewGroupKey;
}