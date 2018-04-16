/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/src/ResizingProperties.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
