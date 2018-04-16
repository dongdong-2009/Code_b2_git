/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/banner/src/Component.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This holds the Component structure that is used throughout the Banner
  * and its pages.
  */

#if !defined(COMPONENT_H)
#define COMPONENT_H

#include <string>
#include <afxwin.h>

namespace TA_Base_Bus
{
    class IBannerPage; // Forward declaration
}

namespace TA_Base_App
{
    struct Component
    {
        // The entity name
        std::string entityName;

        // The DLL path
        std::string path;

        // This is the component interface that will provide access to the component
        // implementation. Populated by the ComponentLibraryAccessor
        TA_Base_Bus::IBannerPage* bannerPage;

        // This is the DLL instance. We need to store this so we can close and free the library
        // when we're done.
	    HINSTANCE instance;
    };
}

#endif // !defined(COMPONENT_H)
