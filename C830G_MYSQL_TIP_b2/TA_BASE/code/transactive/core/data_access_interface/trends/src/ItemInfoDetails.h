/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/trends/src/ItemInfoDetails.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
 
#ifndef ItemInfoDetails
#define ItemInfoDetails

      
namespace TA_Base_Core
{
	typedef struct
    {
        int index;				    // Index of the item
        //COLORREF colour;		    // Colour of the item
        std::string colour;         // Hex string representing colour
        std::string displayName;    // Display name of the item
    } ItemInfo;
}


#endif //ItemInfoDetails
