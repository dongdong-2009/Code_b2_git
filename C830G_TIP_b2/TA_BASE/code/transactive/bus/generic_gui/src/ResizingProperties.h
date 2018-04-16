/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/ResizingProperties.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This structure is used by GenericGUI to determine how an application can
  * be resized.
  */

#ifndef TABUS_TAGENERICGUI_RESIZINGPROPERTIES
#define TABUS_TAGENERICGUI_RESIZINGPROPERTIES

namespace TA_Base_Bus
{
		struct ResizingProperties
        {
            bool canMaximise;
            long maxWidth;
            long maxHeight;
            long minWidth;
            long minHeight;
        };
}


#endif //TABUS_TAGENERICGUI_RESIZINGPROPERTIES
