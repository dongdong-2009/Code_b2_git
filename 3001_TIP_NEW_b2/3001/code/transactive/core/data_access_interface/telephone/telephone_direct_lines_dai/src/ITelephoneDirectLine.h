/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectLine.h $
  * @author Raymond Pau
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * ITelephoneDirectLine is an interface to a TelephoneDirectLine object.  It allows the TelephoneDirectLine object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(ITelephoneDirectLine_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ITelephoneDirectLine_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class ITelephoneDirectLine : public virtual TA_Base_Core::IItem
    {
    public:
        virtual ~ITelephoneDirectLine() {};

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey() =0;

        virtual std::string getSetName() = 0;
        virtual int getButtonPosition() = 0;
        virtual std::string getTelephoneNumber() = 0;
        virtual std::string getLabel() = 0;
        virtual unsigned long getBtnSetKey() =0;
        virtual long getOperatorKey() =0;
        virtual long getSeProfId() =0;
    };

    typedef std::vector<ITelephoneDirectLine*> ITelephoneDirectLines;
    typedef ITelephoneDirectLines::iterator    ITelephoneDirectLinesIt;
} // close namespace TA_IRS_Core

#endif // !defined(ITelephoneDirectLine_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
