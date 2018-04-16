/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scheduling/scheduling_manager/src/Component.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This holds the Component structure that is used to maintain job-specific plugin information.
  *
  * Adapted from Configuration Editor.
  */

#if !defined(Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_)
#define Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_

#include <string>

namespace TA_Base_App
{
    class IJobDetailsPlugin;

    struct Component
    {
        // This holds the help file to use for this component. 
        std::string help;

        // The DLL path
        std::string path;

        // This is the component interface that will provide access to the component implementation
        // Populated by the ComponentLibraryAccessor
        IJobDetailsPlugin* plugin;

        // This is the DLL instance. We need to store this so we can close and free the library when we're done.
	    HINSTANCE instance;
    };
};
#endif // !defined(Component_2271D6DD_BC63_4274_9F58_EEBB6EC28DCD__INCLUDED_)
