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
#if !defined(PaException_H)
#define PaException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPaTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad PA operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:55 PM
     */
    class PaException : public TrainException
    {
    public:

        /**
         * constructor
         */
        PaException() throw ();

        /**
         * destructor
         */
        virtual ~PaException() throw ();

        /**
         * 
         * @param msg
         * @param errorType
         * @exception ()
         */
        PaException( const std::string& msg,
                     ProtocolPaTypes::EPaCommandResult errorType ) throw ();


        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        ProtocolPaTypes::EPaCommandResult getErrorType() const throw ();

    private:

        /**
         * The error type given by the train
         */
        ProtocolPaTypes::EPaCommandResult m_errorType;

    };

}
#endif // !defined(PaException_H)
