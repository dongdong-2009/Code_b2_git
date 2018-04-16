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

#if !defined(BadCommandException_H)
#define BadCommandException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCommonTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is raised when a command to the train results in a Bad Command event.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:32 PM
     */
    class BadCommandException : public TrainException
    {
    public:
        /**
         * constructor
         * 
         */
        BadCommandException() throw ();

        /**
         * desconstructor
        */
        virtual ~BadCommandException() throw ();

        /**
         * 
         * @param msg
         * @param errorType
         * @exception ()
         */
        BadCommandException( const std::string& msg,
                             ProtocolCommonTypes::EBadCommandError errorType ) throw ();


        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        ProtocolCommonTypes::EBadCommandError getErrorType() const throw ();

    private:

        /**
         * The error type given by the train
         */
        ProtocolCommonTypes::EBadCommandError m_errorType;

    };

}
#endif // !defined(BadCommandException_H)
