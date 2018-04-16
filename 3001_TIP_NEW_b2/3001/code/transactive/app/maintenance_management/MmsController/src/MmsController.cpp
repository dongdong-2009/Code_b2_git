/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/MmsController/src/MmsController.cpp $
  * @author Darren Sampson
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Main application entry point for the MMS Controller application
  *
  */

#pragma warning ( disable : 4250 4786 4284)

#include "app/maintenance_management/MmsController/src/stdafx.h"
#include "app/maintenance_management/MmsController/src/MmsController.h"
#include "app/maintenance_management/MmsController/src/MmsControllerDlg.h"
#include "app/maintenance_management/MmsController/src/MmsControllerGUI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveWinApp;

namespace TA_IRS_App
{
	BEGIN_MESSAGE_MAP(CMmsControllerApp, TransActiveWinApp)
		//{{AFX_MSG_MAP(CMmsControllerApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG		
	END_MESSAGE_MAP()
	
	CMmsControllerApp::CMmsControllerApp() : 
		TA_Base_Bus::TransActiveWinApp(new CMmsControllerGUI(), "MMS Controller")
	{
	}
	
	CMmsControllerApp theApp;
}
