/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainException_H)
#define TrainException_H

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Bus
{
    /**
     * The generic train agent exception.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:07 PM
     */
    class TrainException : public TA_Base_Core::TransactiveException
    {
    public:
        /**
         * 
         * constructor
         */
        TrainException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        TrainException(const std::string& msg) throw ();

    };

}
#endif // !defined(TrainException_H)
