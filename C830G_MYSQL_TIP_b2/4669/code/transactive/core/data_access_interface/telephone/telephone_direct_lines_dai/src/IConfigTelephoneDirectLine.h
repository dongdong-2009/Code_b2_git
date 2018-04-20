/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/IConfigTelephoneDirectLine.h $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ITelephoneDirectLine - This is the interface supported by objects that allow configuration of
  * TelephoneDirectLines. It provides methods for retrieving, settings, and writing data for an TelephoneDirectLine object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigTelephoneDirectLine_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigTelephoneDirectLine_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectLine.h"

namespace TA_IRS_Core
{

    class IConfigTelephoneDirectLine : public virtual TA_Base_Core::IConfigItem, public virtual ITelephoneDirectLine
    {

    public:

        virtual ~IConfigTelephoneDirectLine() {};

        virtual void setSetName         (const std::string& name) = 0;
        virtual void setButtonPosition  (const int btnPos) = 0;
        virtual void setTelephoneNumber (const std::string& number) = 0;
        virtual void setLabel           (const std::string& label) = 0;
        virtual void setBtnSetKey       (const unsigned long key) = 0;
        virtual void setOperatorKey     (const long key) = 0;
        virtual void setSeProfId        (const long profId) = 0;

        virtual void invalidate() = 0;

    
    };
} // close TA_IRS_Core

#endif // !defined(IConfigTelephoneDirectLine_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
