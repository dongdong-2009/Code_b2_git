#ifndef Public_Type_Defs_01_03_2005
#define Public_Type_Defs_01_03_2005

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/types/public_types/TypeDefs.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * Contains public type definitions.
  */

#include <string>
#include <map>


namespace TA_Base_Core
{
    /**
     * To be used when specifying alarm descriptions through the ItaAlarm interface.
     * The map is keyed on the parameter name and stores the value for the parameter.
     */
    typedef std::map<std::string, std::string> NameValuePairs;

}

#endif //Public_Type_Defs_01_03_2005
