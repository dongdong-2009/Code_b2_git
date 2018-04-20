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

#if !defined(TrainTimeoutException_H)
#define TrainTimeoutException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Used by the Transaction Management classes. This indicates a transaction has
     * timed out waiting for a response.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:11 PM
     */
    class TrainTimeoutException : public TrainException
    {
    public:
        /**
         * 
         * constructor
         */
        TrainTimeoutException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainTimeoutException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        TrainTimeoutException(const std::string& msg) throw ();

    };

}
#endif // !defined(TrainTimeoutException_H)
