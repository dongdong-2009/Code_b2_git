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
#if !defined(CctvException_H)
#define CctvException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is the result of a bad CCTV operation.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:35 PM
     */
    class CctvException : public TrainException
    {
    public:

        /**
         * Constructor
         */
        CctvException() throw ();

        /**
         * Destructor
         */
        virtual ~CctvException() throw ();

        /**
         * 
         * @param msg
         * @param errorType
         * @exception ()
         */
        CctvException( const std::string& msg,
                       ProtocolCctvTypes::ECctvCommandStatus errorType ) throw ();


        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        ProtocolCctvTypes::ECctvCommandStatus getErrorType() const throw ();

    private:

        /**
         * The error type given by the train
         */
        ProtocolCctvTypes::ECctvCommandStatus m_errorType;

    };

}
#endif // !defined(CctvException_H)
