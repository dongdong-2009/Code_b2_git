/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/3001/transactive/app/configuration/config_plugins/pa_station/src/TA_PAStationConfigPlugin.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  * 
  * This is the 'mainline' of the DLL. This is the entry point and is run when the 
  * main Configuration Editor application loads the DLL.
  */

#include <afxext.h>
#include <afxdllx.h>

static AFX_EXTENSION_MODULE Mfc_extensionsDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("TA_PASTATIONCONFIGPLUGIN.DLL Initializing!\n");
		    
		// Extension DLL one-time initialization
        if ( !AfxInitExtensionModule( Mfc_extensionsDLL, hInstance ) )
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

        new CDynLinkLibrary( Mfc_extensionsDLL );
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("TA_PASTATIONCONFIGPLUGIN.DLL Terminating!\n");
		// Terminate the library before destructors are called
        AfxTermExtensionModule( Mfc_extensionsDLL );
	}

	return 1;   // ok
}

#include "MftExecuteScriptStepComponent.h"
#include "app/response_plans/plan_step_plugin/common/src/ExportDef.h"
extern "C" Plan_Export TA_Base_App::IEditorStepComponent* createEditorStepComponent()
{
    return TA_Base_App::MftExecuteScriptStepComponent::instance();
}