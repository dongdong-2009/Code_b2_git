/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/DataNodeController/src/SummaryDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "stdafx.h"
#include "DataNodeController.h"
#include "SummaryDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




CSummaryDialog::CSummaryDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSummaryDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSummaryDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSummaryDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSummaryDialog)
	DDX_Control(pDX, IDC_LIST1, m_SummaryListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSummaryDialog, CDialog)
	//{{AFX_MSG_MAP(CSummaryDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



BOOL CSummaryDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_SummaryListCtrl.InitialiseList();

	return TRUE;  
}
