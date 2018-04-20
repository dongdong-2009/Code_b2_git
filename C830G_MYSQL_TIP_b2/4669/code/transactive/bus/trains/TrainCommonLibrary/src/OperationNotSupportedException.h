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
#if !defined(OperationNotSupportedException_H)
#define OperationNotSupportedException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when the requested operation is not supported by the agent.'
     * Eg attempt to download a TTIS library at a station, where the feature is not available.
     * @author Adam Radics
     * @version 1.0
     * @created 18-Mar-2008 3:19:14 PM
     */
    class OperationNotSupportedException : public TrainException
    {

    public:

        /**
         * Constructor
         */
        OperationNotSupportedException() throw ();


        /**
         * Constructor
         *
         * @param msg
         * @exception ()
         */
        OperationNotSupportedException(const std::string& msg) throw ();


        /**
         * Destructor
         */
        virtual ~OperationNotSupportedException() throw ();
    };

}
#endif // !defined(OperationNotSupportedException_H)
