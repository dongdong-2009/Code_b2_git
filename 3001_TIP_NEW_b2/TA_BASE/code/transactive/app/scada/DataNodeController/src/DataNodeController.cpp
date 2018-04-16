/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeController.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
