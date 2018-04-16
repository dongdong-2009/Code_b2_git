/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationProgressDlg.cpp $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class implements a dialog that is used to represent the percentage of
  * configuration plugins that have been loaded.
  *
  */

#include "stdafx.h"
#include "configurationeditor.h"
#include "ConfigurationProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConfigurationProgressDlg dialog


CConfigurationProgressDlg::CConfigurationProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigurationProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CConfigurationProgressDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CConfigurationProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConfigurationProgressDlg)
	DDX_Control(pDX, IDC_LOADPROGRESS, m_progress);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CConfigurationProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CConfigurationProgressDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigurationProgressDlg message handlers

BOOL CConfigurationProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Initialise the progress bar.
    m_progress.SetRange( 0, 100 );
    m_progress.SetPos( 0 );

    // Should the progress be always on top?
    //SetWindowPos( &this->wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigurationProgressDlg::updateLoadProgress( unsigned char percentage )
{
	CWnd * pWnd = GetDlgItem(IDC_PROGRESS_STATIC);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
		CString strMsg;
		strMsg.Format("Loading Configuration Plugin Libraries: %d", percentage);
		strMsg += "%";
		pWnd->SetWindowText(strMsg);
	}
    // Clamp the upper limit.
    if ( 100 < percentage )
    {
        percentage = 100;
    }

    // Update progress.
    m_progress.SetPos( percentage );
}
