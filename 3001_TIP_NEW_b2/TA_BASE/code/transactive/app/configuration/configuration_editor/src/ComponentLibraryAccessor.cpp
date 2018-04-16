/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/configuration/configuration_editor/src/ComponentLibraryAccessor.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/03 17:11:08 $
  * Last modified by:  $Author: limin.zhu $
  * 
  * This class accesses the component libraries to retrieve and set up the Component structure.
  * These can then be retrieved for processing.
  */

#pragma warning(disable:4786)

#include "app/configuration/configuration_editor/src/StdAfx.h"
#include "app/configuration/configuration_editor/src/ComponentLibraryAccessor.h"
#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/DllHandler.h"
#include "app/configuration/configuration_editor/src/RightsChecker.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/configuration/src/Component.h"
#include "core/data_access_interface/configuration/src/ComponentAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    ComponentLibraryAccessor::ComponentLibraryAccessor()
    {

    }


    ComponentLibraryAccessor::~ComponentLibraryAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            for (std::vector<Component>::iterator thisComponent = m_components.begin(); thisComponent != m_components.end(); ++thisComponent )
            {
                // Free'ing the library seems to hang the application so we will leave it commented out
                // for now
                //if (thisComponent->libraryInstance != NULL)
                //{
                    //FreeLibrary( thisComponent->libraryInstance );
                //}

                if (thisComponent->componentInterface != NULL)
                {
                    delete thisComponent->componentInterface;
                    thisComponent->componentInterface = NULL;
                }
            }            
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void ComponentLibraryAccessor::loadComponents( IComponentLibraryAccessorCallback* callback /*=NULL*/ )
    {
        FUNCTION_ENTRY("loadComponents");

        CString unavailableComponents = "";

        std::vector<TA_Base_Core::IComponent*> components;

        try
        {
             components = TA_Base_Core::ComponentAccessFactory::getInstance().getAllComponents();
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "No components will be shown in the Configuration Editor");
            return;
        }
        catch( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "No components will be shown in the Configuration Editor");
            return;
        }
      

        for (std::vector<TA_Base_Core::IComponent*>::iterator iter = components.begin(); iter != components.end(); ++iter)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Loading dynamic link library %s for plugin %s", (*iter)->getLibrary().c_str(), (*iter)->getComponent().c_str());

            // Start setting up the component structure
            Component currentComponent;
            currentComponent.category = (*iter)->getCategory().c_str();
            currentComponent.component = (*iter)->getComponent().c_str();
            currentComponent.isAccessible = false;
            currentComponent.helpFile = (*iter)->getHelpPath();

            // Load the DLL
            currentComponent.libraryInstance = AfxLoadLibrary( (*iter)->getLibrary().c_str() );
            if ( 0 == currentComponent.libraryInstance )
		    {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load the plugin %s", (*iter)->getLibrary().c_str() );
                unavailableComponents += currentComponent.component + " - " + (*iter)->getLibrary().c_str() + _T("\n");
                continue;
		    }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Dynamic link library %s loaded successfully for plugin %s", (*iter)->getLibrary().c_str(), currentComponent.component );

			// Load the DLL function.
			    
            typedef IConfigurableComponent* (*pfnCreateComponent)(const std::string&, const std::vector<std::string>&);

		    pfnCreateComponent DLLCreateComponent;
		    DLLCreateComponent = (pfnCreateComponent) ::GetProcAddress( currentComponent.libraryInstance, "createComponent" );
		    if ( NULL == DLLCreateComponent )
		    {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not load function in the plugin %s", (*iter)->getLibrary().c_str() );
                unavailableComponents += currentComponent.component + " - " + (*iter)->getLibrary().c_str() + _T("\n");

                FreeLibrary( currentComponent.libraryInstance );
				currentComponent.libraryInstance = 0;
                continue;
		    }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "createComponent() function loaded successfully for plugin %s", currentComponent.component );

            std::vector<std::string> types = findOtherTypes( iter, components );

            // Now call the DLL function to get the IConfigurableComponent interface object
            currentComponent.componentInterface = DLLCreateComponent( std::string(currentComponent.component), types );
            TA_ASSERT(currentComponent.componentInterface != NULL, "The component created is NULL");

            // Finally we can add the component to our list
            m_components.push_back(currentComponent);

            // Now update the callback if required.
            if ( NULL != callback )
            {
                // A vector iterator is a random access iterator so we can gauge progress by iterator differences.
                unsigned char progress = iter - components.begin();
                progress = ( progress * 100 ) / components.size();
                callback->updateLoadProgress( progress );
            }
        }
        
        if ( !unavailableComponents.IsEmpty() )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << unavailableComponents;
            userMsg.showMsgBox(IDS_UW_030016);

            if ( NULL == AfxGetMainWnd() || ( FALSE == ::IsWindow( AfxGetMainWnd()->GetSafeHwnd() ) ) )
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "Main windows was destroyed when waiting for user's action. must exit." );
                ::exit(0);
            }
        }

        FUNCTION_EXIT;
    }


    void ComponentLibraryAccessor::setUpSingletonsInComponents()
    {
        FUNCTION_ENTRY("setUpSingletonsInComponents");

        // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
        DllHandler dllHandler;

        // Now initialise all the singletons inside the DLLs
        for (std::vector<Component>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Stored component interface is NULL");
            iter->componentInterface->setUpSingletons(dllHandler);
        }

        FUNCTION_EXIT;
    }


    void ComponentLibraryAccessor::determineAvailableComponents(RightsChecker& checker)
    {
        FUNCTION_ENTRY("determineAvailableComponents");

        for(std::vector<Component>::iterator iter = m_components.begin(); iter != m_components.end(); ++iter)
        {
            TA_ASSERT(iter->componentInterface != NULL, "Stored component interface is NULL");

            unsigned long action = iter->componentInterface->getRightsActionForAddingThisComponent();

            iter->isAccessible = checker.canPerformAction(action);
            
            if (!iter->isAccessible)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "User will not be able to access component %s - %s",
                            std::string(iter->category).c_str(),
                            std::string(iter->component).c_str() );
            }
        }

        FUNCTION_EXIT;
    }


    std::vector<std::string> ComponentLibraryAccessor::findOtherTypes( const std::vector<TA_Base_Core::IComponent*>::iterator& currentComp,
                                                                       std::vector<TA_Base_Core::IComponent*>& components)
    {
        FUNCTION_ENTRY("findOtherTypes");

        TA_ASSERT( *currentComp != NULL, "Iterator passed in contains a NULL pointer");

        std::vector<std::string> types;
        // Add the current component entity type name to the list if it is set to something
        if ( !(*currentComp)->getEntityTypeName().empty() )
        {
            types.push_back( (*currentComp)->getEntityTypeName() );
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Finding other types matching: %s, %s, %s",(*currentComp)->getCategory().c_str(), (*currentComp)->getComponent().c_str(),(*currentComp)->getLibrary().c_str() );

        // Start our processing one item after the currentComp and add all other strings
        std::vector<TA_Base_Core::IComponent*>::iterator iter = currentComp + 1;
        while( iter != components.end() )
        {
            LOGMORE( SourceInfo, "%s %s %s",(*iter)->getCategory().c_str(), (*iter)->getComponent().c_str(),(*iter)->getLibrary().c_str() );
            if( (*iter != NULL) &&
                ((*currentComp)->getCategory() == (*iter)->getCategory()) &&
                ((*currentComp)->getComponent() == (*iter)->getComponent()) &&
                ((*currentComp)->getLibrary() == (*iter)->getLibrary()) &&
                (!(*iter)->getEntityTypeName().empty())  )
            {
                types.push_back( (*iter)->getEntityTypeName() );

                // Now remove this item from the vector so we don't process it again
                delete *iter;
                *iter = NULL;

                // Erase will increment our iterator for us
                iter = components.erase( iter );
                continue;

            }
            ++iter;
        }

        FUNCTION_EXIT;
        return types;
    }

}
