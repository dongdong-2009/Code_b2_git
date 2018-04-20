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

#if !defined(TtisException_H)
#define TtisException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad TTIS operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:15 PM
     */
    class TtisException : public TrainException
    {
    public:
        /**
         * 
         * constructor
         */
        TtisException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TtisException() throw ();

       /**
         * 
         * @param msg
         * @param errorType
         * @exception ()
         */
        TtisException(const std::string& msg, 
                      ProtocolTtisTypes::ETtisResult errorType) throw ();


        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        ProtocolTtisTypes::ETtisResult getErrorType() const throw ();

    private:

        /**
         * The error type given by the train
         */
        ProtocolTtisTypes::ETtisResult m_errorType;

    };

}
#endif // !defined(TtisException_H)
