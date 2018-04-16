/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/src/ResizingProperties.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
