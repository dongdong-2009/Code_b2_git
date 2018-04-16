/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/VariableTypeFactory.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  VariableTypeFactory.cpp
//  Implementation of the Class VariableTypeFactory
//  Created on:      09-Feb-2005 12:51:51 PM
///////////////////////////////////////////////////////////
#include "stdafx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DllHandler.h"
#include "VariableTypeFactory.h"

#include "IVariable.h"
#include "IAddVariable.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    VariableTypeFactory::VariableTypeFactory()
    {

    }

    VariableTypeFactory::~VariableTypeFactory()
    {

    }

    /**
     * Creates a new IVariable object
     * @param dllName
     * @param added
     * 
     */
    IVariable* VariableTypeFactory::createVariableSelector(const std::string& dllName, const IAddVariable* adder )
    {
	    // Creates a new IVariable object
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Loading dynamic link library %s", dllName.c_str() );

        // Load the DLL
        HINSTANCE libraryInstance = AfxLoadLibrary( dllName.c_str() );
        if ( 0 == libraryInstance )
	    {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", dllName.c_str() );
            return NULL;
	    }

 
        LOG_GENERIC( SourceInfo, DebugUtil::DebugNorm, "Dynamic link library %s loaded successfully", dllName.c_str() );

	    // Load the DLL function.
		    
        typedef IVariable* (*pfnVariableSelector)(const IAddVariable*);

	    pfnVariableSelector DllVariableSelector = NULL;
	    DllVariableSelector = (pfnVariableSelector) ::GetProcAddress( libraryInstance, "createVariableSelector" );
	    if ( NULL == DllVariableSelector )
	    {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", dllName.c_str() );

            FreeLibrary( libraryInstance );
		    libraryInstance = 0;
            return NULL;
	    }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugNorm, "createVariableSelector() function loaded successfully for plugin %s", dllName.c_str() );

    
        // Now call the DLL function to get the IVariable interface object
        IVariable* componentInterface = DllVariableSelector( adder );
		componentInterface->libraryInstance = libraryInstance;
        TA_ASSERT(componentInterface != NULL, "The component created is NULL");

        // Ensure any Singltons in the dll are synced correctly
        // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
        TA_Base_Core::DllHandler dllHandler;

        // Now initialise all the singletons inside the DLLs
        componentInterface->setUpSingletons( dllHandler );

        return componentInterface;
    }


} // namespace