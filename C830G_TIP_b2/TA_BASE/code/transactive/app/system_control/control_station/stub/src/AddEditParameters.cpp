/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/control_station/stub/src/AddEditParameters.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// AddEditParameters.cpp : implementation file
//

#include "stdafx.h"
#include "Dummy_Process_Monitor.h"
#include "AddEditParameters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AddEditParameters dialog


AddEditParameters::AddEditParameters(CWnd* pParent /*=NULL*/)
	: CDialog(AddEditParameters::IDD, pParent)
{
	//{{AFX_DATA_INIT(AddEditParameters)
	m_name = _T("");
	m_value = _T("");
	//}}AFX_DATA_INIT
}


void AddEditParameters::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AddEditParameters)
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AddEditParameters, CDialog)
	//{{AFX_MSG_MAP(AddEditParameters)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AddEditParameters message handlers
