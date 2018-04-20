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

#if !defined(TransactionCancelledException_H)
#define TransactionCancelledException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * This is thrown from a transaction when it was externally cancelled.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:14 PM
     */
    class TransactionCancelledException : public TrainException
    {
    public:
  
        /**
         * destructor
         * @exception ()
         */
        virtual ~TransactionCancelledException() throw ();

        /**
         * constructor
         * @exception ()
         */
        TransactionCancelledException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        TransactionCancelledException(const std::string& msg) throw ();

    };

}
#endif // !defined(TransactionCancelledException_H)
