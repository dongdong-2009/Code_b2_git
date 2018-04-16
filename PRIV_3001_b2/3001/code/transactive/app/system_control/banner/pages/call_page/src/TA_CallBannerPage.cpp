/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/call_page/src/TA_CallBannerPage.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Defines the initialization routines for the DLL.
  * 
  */

#pragma warning(disable:4786)

#include "stdafx.h"
#include "bus/banner_framework/src/IBannerPage.h"
#include "app/system_control/banner/pages/call_page/src/CallPage.h"

#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static AFX_EXTENSION_MODULE TA_CallBannerPageDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("TA_CALLBANNERPAGE.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(TA_CallBannerPageDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(TA_CallBannerPageDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("TA_CALLBANNERPAGE.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(TA_CallBannerPageDLL);
	}
	return 1;   // ok
}


/**
  * createComponent
  *
  * Creates an IBannerPage object and returns it to the caller. All further
  * communication between the DLL and main application will happen through this interface.
  *
  * @return IBannerPage*  A new CAlarmsGuiDlg object which implements the IBannerPage interface.
  *                       NOTE: The caller is responsible for deleting the pointer.
  */
extern "C" __declspec(dllexport) TA_Base_Bus::IBannerPage* createComponent(const std::string& buttonName, int xPosition, int yPosition, 
                                                                    int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
{
    return new TA_IRS_App::CallPage(buttonName, xPosition, yPosition, width, height, parentDialog);
}
