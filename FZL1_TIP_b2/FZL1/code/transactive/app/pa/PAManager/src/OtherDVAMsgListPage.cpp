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
#include "app/pa/PAManager/src/DVAMsgDescEditDlg.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"
#include "app/pa/PAManager/src/OtherDVAMsgListPage.h"


// OtherDVAMsgListPage dialog

IMPLEMENT_DYNAMIC(OtherDVAMsgListPage, CPropertyPage)

OtherDVAMsgListPage::OtherDVAMsgListPage( PaRuntimeModel& refBroadcastModel ) : 
CPropertyPage(OtherDVAMsgListPage::IDD), 
ICurSelDVAMsgObserver(),
m_btnDVADescEdit(),
m_edBroadcastDes(),
m_slcBroadcastList( refBroadcastModel, OTHER_DVA_MSG ),
m_bEnableDVAMsgDescEdit( true ),
m_refBroadcastModel( refBroadcastModel )
{
    m_refBroadcastModel.registerCurSelDVAMsgObserver( OTHER_DVA_MSG, this );
}

OtherDVAMsgListPage::~OtherDVAMsgListPage()
{
    m_refBroadcastModel.deregisterCurSelDVAMsgObserver( OTHER_DVA_MSG, this );
}

void OtherDVAMsgListPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BTN_OTHER_DES_EDIT, m_btnDVADescEdit);
    DDX_Control(pDX, IDC_EDIT_BROADCAST_DES_O, m_edBroadcastDes);
    DDX_Control(pDX, IDC_LIST_OTHER_BROADCAST, m_slcBroadcastList);
}


BEGIN_MESSAGE_MAP(OtherDVAMsgListPage, CPropertyPage)
    ON_BN_CLICKED(IDC_BTN_LIST_ALL_O, OnBnClickedBtnListAll)
    ON_BN_CLICKED(IDC_BTN_OTHER_DES_EDIT, OnBnClickedOtherDesEdit)
    ON_EN_CHANGE(IDC_EDIT_SEARCH_KEYWORD_O, OnEnChangeSearchKeyword)
    ON_MESSAGE(WM_SEL_QUICK_BROADCAST_DVA_MSG, OnSelQuickBroadcastDVAMsg)
    ON_MESSAGE(WM_UPDATE_DVA_MSG_LIST, OnUpdateDVAMsgList)
END_MESSAGE_MAP()


// OtherDVAMsgListPage message handlers

BOOL OtherDVAMsgListPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    m_btnDVADescEdit.EnableWindow( FALSE );

    return TRUE;
}

void OtherDVAMsgListPage::searchDVAMsg()
{
    char strKeyword[SEARCH_KEYWORD_MAX_CHARS] = { 0 };
    GetDlgItemText( IDC_EDIT_SEARCH_KEYWORD_O, strKeyword, SEARCH_KEYWORD_MAX_CHARS );
    m_slcBroadcastList.updateList( std::string(strKeyword) );
}

void OtherDVAMsgListPage::OnBnClickedBtnListAll()
{
    SetDlgItemText( IDC_EDIT_SEARCH_KEYWORD_O, EMPTY_STRING );
    //m_slcBroadcastList.updateList( std::string("") );
}

void OtherDVAMsgListPage::OnBnClickedOtherDesEdit()
{
    // Check rights first
    if ( !PaRightsManager::getInstance().hasRightToModifyDvaMsgDescription() )
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_NO_RIGHTS_TO_MODIFY_DVA_MSG_DESCRIPTION );
        return;
    }

    if ( NULL == m_refBroadcastModel.getCurSelDVAMsg( OTHER_DVA_MSG ))
    {
        // The Edit button is disable when no DVA message is selected
        // and this function should not be called
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
        return;
    }

    DVAMsgDescEditDlg dlgDVAMsgDescEdit( m_refBroadcastModel, OTHER_DVA_MSG, this );

    int nOperatorSel = dlgDVAMsgDescEdit.DoModal();
    if ( IDCANCEL == nOperatorSel )
    {
        return;
    }

    m_edBroadcastDes.SetWindowText( dlgDVAMsgDescEdit.getBroadcastDes().c_str() );
}

void OtherDVAMsgListPage::processDVAMsgSelChanged( int nType )
{
    BOOL bEnableEdit = (( NULL != m_refBroadcastModel.getCurSelDVAMsg( OTHER_DVA_MSG )) && m_bEnableDVAMsgDescEdit ) ? TRUE : FALSE;
    m_btnDVADescEdit.EnableWindow( bEnableEdit );
    m_edBroadcastDes.SetWindowText( m_refBroadcastModel.getCurDVAMsgDesc( nType ).c_str() );
}

LRESULT OtherDVAMsgListPage::OnSelQuickBroadcastDVAMsg( WPARAM wParam, LPARAM lParam )
{
    if ( !m_slcBroadcastList.selQuickBroadcastItem( wParam ) && ( 0 == lParam ))
    {
        SetDlgItemText( IDC_EDIT_SEARCH_KEYWORD_O, EMPTY_STRING );
        PostMessage( WM_SEL_QUICK_BROADCAST_DVA_MSG, wParam, 1 );
    }

    return 0;
}

LRESULT OtherDVAMsgListPage::OnUpdateDVAMsgList( WPARAM wParam, LPARAM lParam )
{
    m_slcBroadcastList.updateList( std::string("") );
    return 0;
}

void OtherDVAMsgListPage::enableEditDVAMsgDescription( bool bEnable )
{
    int nShowWnd = bEnable ? SW_SHOW : SW_HIDE;
    m_btnDVADescEdit.ShowWindow( nShowWnd );
    m_bEnableDVAMsgDescEdit = bEnable;
}

void OtherDVAMsgListPage::OnEnChangeSearchKeyword()
{
    searchDVAMsg();
}

unsigned int OtherDVAMsgListPage::getTemplateId()
{
    return IDD;
}

std::string OtherDVAMsgListPage::getCaption()
{
    return DVA_MSG_TYPE_OTHER_TEXT;
}