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

#if !defined(PecException_H)
#define PecException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolPecTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad PEC operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:57 PM
     */
    class PecException : public TrainException
    {
    public:

        /**
         * constructor
         */
        PecException() throw ();

        /**
         * destructor
         */
        virtual ~PecException() throw ();

        /**
         * 
         * @param msg    A description of the error for logging purposes
         * @param errorType    The error reason
         * @exception ()
         */
        PecException(const std::string& msg,
                     ProtocolPecTypes::EPecCommandResult errorType) throw ();

        /**
         * Gets the reason the PEC operation failed
         */
        ProtocolPecTypes::EPecCommandResult getErrorType() const throw ();

     private:

        /**
         * The reason for the exception
         */
        ProtocolPecTypes::EPecCommandResult m_errorType;

    };

}
#endif // !defined(PecException_H)
