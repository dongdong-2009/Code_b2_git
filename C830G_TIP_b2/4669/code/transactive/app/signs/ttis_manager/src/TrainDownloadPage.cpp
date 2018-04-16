/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainDownloadPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the top level train download page.
  *
  */

#include "stdafx.h"
#include "TTISManager.h"
#include "TrainDownloadPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTrainDownloadPage dialog

namespace TA_IRS_App
{

CTrainDownloadPage::CTrainDownloadPage(CWnd* pParent /*=NULL*/)
    : CDialog(CTrainDownloadPage::IDD, pParent)
{
    FUNCTION_ENTRY( "CTrainDownloadPage" );

    // {{AFX_DATA_INIT(CTrainDownloadPage)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CTrainDownloadPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CTrainDownloadPage)
    DDX_Control(pDX, IDC_TRAIN_DOWNLOAD_TAB, m_trainDownloadTab);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CTrainDownloadPage, CDialog)
    // {{AFX_MSG_MAP(CTrainDownloadPage)
        // NOTE: the ClassWizard will add message map macros here
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTrainDownloadPage message handlers


void CTrainDownloadPage::Init(REBProgressManager & mgr)
{
    FUNCTION_ENTRY( "Init" );

    m_trainDownloadTab.Init(mgr);

    FUNCTION_EXIT;
}


void CTrainDownloadPage::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );
    FUNCTION_EXIT;
}


void CTrainDownloadPage::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );
    FUNCTION_EXIT;
}


BOOL CTrainDownloadPage::PreTranslateMessage(MSG* pMsg)
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
            case VK_RETURN:
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
    default:
        {
            // do nothing
        }
    }

    FUNCTION_EXIT;
    return CDialog::PreTranslateMessage(pMsg);
}


} // TA_IRS_App
