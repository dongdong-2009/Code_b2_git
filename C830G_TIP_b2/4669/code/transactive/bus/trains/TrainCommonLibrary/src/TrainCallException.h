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
#if !defined(TrainCallException_H)
#define TrainCallException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCallTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad Train Call operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:04 PM
     */
    class TrainCallException : public TrainException
    {
    public:

        /**
         * 
         * constructor
         */
        TrainCallException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainCallException() throw ();

        /**
         * 
         * @param msg
         * @param errorType
         * @exception ()
         */
        TrainCallException(const std::string& msg, 
                           ProtocolCallTypes::ECallTrainCommandStatus errorType) throw ();

        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        ProtocolCallTypes::ECallTrainCommandStatus getErrorType() const throw ();

    private:

        /**
         * The error type given by the train
         */
        ProtocolCallTypes::ECallTrainCommandStatus m_errorType;

    };

}

#endif // !defined(TrainCallException_H)
