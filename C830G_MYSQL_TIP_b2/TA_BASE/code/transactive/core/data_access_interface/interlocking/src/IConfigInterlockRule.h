/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking/src/IConfigInterlockRule.h $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  * 
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(IConfigInterlockRule_H)
#define IConfigInterlockRule_H

#include <string>

#include "core/data_access_interface/interlocking/src/IInterlockRule.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    /**
    * IConfigInterlockRule - This is the interface supported by objects that allow configuration of
    * interlocking rules. It provides methods for retrieving, settings, and writing data for a rule object.
    * The set methods in this interface do not set the values in the database
    * straight away. They only set the values in the object after validating that
    * they are correct.
    */
    class IConfigInterlockRule : public IInterlockRule, public virtual IConfigItem
    {

    public:

        /**
         * deleteThisObject
         *
         * Removes this rule from the database. 
         * The calling application MUST then delete this rule object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this rule was initially loaded from the database
         *      OR     - writeInterlockRuleData() has already been called
         *      This rule has not been deleted
         */
        virtual void deleteThisObject() = 0;


        virtual ~IConfigInterlockRule() {};
    };
} //close namespace TA_Base_Core
#endif // !defined(IConfigInterlockRule_H)
