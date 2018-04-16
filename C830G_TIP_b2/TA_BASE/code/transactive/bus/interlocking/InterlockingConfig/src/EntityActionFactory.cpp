/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/InterlockingConfig/src/EntityActionFactory.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  *
  */
///////////////////////////////////////////////////////////
//  EntityActionFactory.cpp
//  Implementation of the Class EntityActionFactory
//  Created on:      03-Feb-2005 12:34:13 PM
///////////////////////////////////////////////////////////
#include "stdafx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DllHandler.h"
#include "EntityActionFactory.h"

#include "IEntityAction.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
    EntityActionFactory::EntityActionFactory()
    {
    }



    EntityActionFactory::~EntityActionFactory()
    {
    }


    /**
     * Creates a new IEntityAction object
     * @param dllName
     * 
     */
    IEntityAction* EntityActionFactory::createEntityAction(const std::string& dllName)
    {

	    //Creates a new IEntityAction object from the requested dll
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
			
        typedef IEntityAction* (*pfnEntityAction)();

		pfnEntityAction DLLEntityAction = NULL;
		DLLEntityAction = (pfnEntityAction) ::GetProcAddress( libraryInstance, "createEntityAction" );
		if ( NULL == DLLEntityAction )
		{
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", dllName.c_str() );

            FreeLibrary( libraryInstance );
			libraryInstance = 0;
            return NULL;
		}

        LOG_GENERIC( SourceInfo, DebugUtil::DebugNorm, "createEntityAction() function loaded successfully for plugin %s", dllName.c_str() );

        
        // Now call the DLL function to get the IEntityAction interface object
        IEntityAction* componentInterface = DLLEntityAction();
        TA_ASSERT(componentInterface != NULL, "The component created is NULL");

        // Ensure any Singltons in the dll are synced correctly
        // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
        TA_Base_Core::DllHandler dllHandler;

        // Now initialise all the singletons inside the DLLs
        componentInterface->setUpSingletons( dllHandler );

        return componentInterface;
    }


} // namespace