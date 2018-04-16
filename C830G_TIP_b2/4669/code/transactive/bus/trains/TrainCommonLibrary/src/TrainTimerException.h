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

#if !defined(TrainTimerException_H)
#define TrainTimerException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Used by the Transaction Management classes. This indicates a timer started for
     * a train expired before being stopped.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:12 PM
     */
    class TrainTimerException : public TrainException
    {
    public:
        /**
         * 
         * constructor
         */
        TrainTimerException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainTimerException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        TrainTimerException(const std::string& msg) throw ();

    };

}
#endif // !defined(TrainTimerException_H)
