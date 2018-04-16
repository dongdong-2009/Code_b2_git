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

#if !defined(TrainAccessDeniedException_H)
#define TrainAccessDeniedException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when the operator has no permissions
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:23 PM
     */
    class TrainAccessDeniedException : public TrainException
    {

    public:

        /**
         * Constructor
         */
        TrainAccessDeniedException() throw ();

        /**
         * Destructor
         */
        virtual ~TrainAccessDeniedException() throw ();

        /**
         * Access Denied Exception
         * @param msg
         * @exception ()
         */
        TrainAccessDeniedException(const std::string& msg) throw ();

    };

}

#endif // !defined(TrainAccessDeniedException_H)
