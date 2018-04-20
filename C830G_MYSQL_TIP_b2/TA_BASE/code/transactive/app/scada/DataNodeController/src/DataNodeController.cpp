/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeController.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "DataNodeController.h"
#include "DataNodeControllerDlg.h"
#include "DataNodeControllerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
	BEGIN_MESSAGE_MAP(CDataNodeControllerApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CDataNodeControllerApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG		
	END_MESSAGE_MAP()
	
	CDataNodeControllerApp::CDataNodeControllerApp() : 
		TA_Base_Bus::TransActiveWinApp(new CDataNodeControllerGUI(), "Data Node Controller")
	{
	}
	
	CDataNodeControllerApp theApp;
}
