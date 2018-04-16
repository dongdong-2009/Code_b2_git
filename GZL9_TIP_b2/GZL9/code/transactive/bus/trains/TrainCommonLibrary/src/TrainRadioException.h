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

#if !defined(TrainRadioException_H)
#define TrainRadioException_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad Radio operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:51:09 PM
     */
    class TrainRadioException : public TrainException
    {
    public:
        /**
         * 
         * constructor
         */
        TrainRadioException() throw ();
 
        /**
         * 
         * destructor
         */
        virtual ~TrainRadioException() throw ();

        /**
         * 
         * @param msg    A description of the error for logging purposes
         * @param errorType    The error reason
         * @exception ()
         */
        TrainRadioException(const std::string& msg, CommonTypes::ERadioErrorType errorType) throw ();

        /**
         * Gets the reason the PEC operation failed
         */
        CommonTypes::ERadioErrorType getErrorType() const throw ();

    private:

        /**
         * The reason for the exception
         */
        CommonTypes::ERadioErrorType m_errorType;

    };

}
#endif // !defined(TrainRadioException_H)
