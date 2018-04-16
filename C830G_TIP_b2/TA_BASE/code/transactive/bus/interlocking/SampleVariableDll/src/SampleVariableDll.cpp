/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/SampleVariableDll.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the 'mainline' of the DLL. This is the entry point and is run when the 
  * Interlocking Rule Trigger Editor loads the DLL.
  *
  */
// SampleVariableDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include "bus/interlocking/InterlockingConfig/src/IVariable.h"
#include "Variable.h"
#include "StringContext.h"
#include "BooleanContext.h"
#include "RealNumberContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_Bus
{
    static AFX_EXTENSION_MODULE SampleVariableDllDLL = { NULL, NULL };

    extern "C" int APIENTRY
    DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
    {
	    // Remove this if you use lpReserved
	    UNREFERENCED_PARAMETER(lpReserved);

	    if (dwReason == DLL_PROCESS_ATTACH)
	    {
		    TRACE0("SAMPLEVARIABLELL.DLL Initializing!\n");
		    
		    // Extension DLL one-time initialization
		    if (!AfxInitExtensionModule(SampleVariableDllDLL, hInstance))
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

		    new CDynLinkLibrary(SampleVariableDllDLL);
	    }
	    else if (dwReason == DLL_PROCESS_DETACH)
	    {
		    TRACE0("SAMPLEVARIABLEDLL.DLL Terminating!\n");
		    // Terminate the library before destructors are called
		    AfxTermExtensionModule(SampleVariableDllDLL);
	    }
	    return 1;   // ok
    }

    /**
      * createVariableSelector
      *
      * Creates an IVariable object and returns it to the caller. All further
      * communication between the DLL and main application will happen through this interface.
      *
      *
      * @return IVariableAction*	A new object which implements
      *                             the IVariable interface.
      */
    extern "C" __declspec(dllexport) IVariable* createVariableSelector(IAddVariable* adder)
    {
        static Variable* v = NULL;
        
        if ( v == NULL )
        {
            v = new Variable(adder);
        }
        return v;
    }

    /**
      * creates IBooleanContext
      *
      * Creates an IBooleanContext object and returns it to the caller. 
      * This context will be able to turn a variable of the type supported by this dll
      * into a bool value for the math library
      *
      *
      * @return IBooleanContext*	A new object which implements
      *                             the IBooleanContext interface.
      */
    extern "C" __declspec(dllexport) IBooleanContext* createBooleanContext()
    {
        static BooleanContext* bc = NULL;
        
        if ( bc == NULL )
        {
            bc = new BooleanContext();
        }
        return bc;
    }

    /**
      * creates IStringContext
      *
      * Creates an IStringContext object and returns it to the caller. 
      * This context will be able to turn a variable of the type supported by this dll
      * into a String value for the math library
      *
      *
      * @return IStringContext*	A new object which implements
      *                         the IStringContext interface.
      */
    extern "C" __declspec(dllexport) IStringContext* createStringContext()
    {
        static StringContext* sc = NULL;
        
        if ( sc == NULL )
        {
            sc = new StringContext();
        }
        return sc;
    }

    /**
      * creates IRealNumberContext
      *
      * Creates an IRealNumberContext object and returns it to the caller. 
      * This context will be able to turn a variable of the type supported by this dll
      * into a RealNumber value for the math library
      *
      *
      * @return IRealNumberContext*	A new object which implements
      *                             the IRealNumberContext interface.
      */
    extern "C" __declspec(dllexport) IRealNumberContext* createRealNumberContext()
    {
        static RealNumberContext* rc = NULL;
        
        if ( rc == NULL )
        {
            rc = new RealNumberContext();
        }
        return rc;
    }

} // namespace