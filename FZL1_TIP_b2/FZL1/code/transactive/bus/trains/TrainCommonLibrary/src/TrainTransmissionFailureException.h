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
#if !defined(TrainTransmissionFailureException_H)
#define TrainTransmissionFailureException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when a command cannot be sent to the train via the radio interface.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:13 PM
     */
    class TrainTransmissionFailureException : public TrainException
    {
    public:
        /**
         * 
         * constructor
         */
        TrainTransmissionFailureException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainTransmissionFailureException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        TrainTransmissionFailureException(const std::string& msg) throw ();

    };

}
#endif // !defined(TrainTransmissionFailureException_H)
