/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(GroupInUseException_H)
#define GroupInUseException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * This exception is thrown when the requested train radio group is already in use
     * by another operator (or its use blocked by a conflicting group)
     * @author Robert van Hugten
     * @version 1.0
     * @created 13-Nov-2007 3:50:48 PM
     */
    class GroupInUseException : public TrainException
    {
    public:

        /**
         * constructor
         */
        GroupInUseException() throw ();

        /**
         * destructor
         */
        virtual ~GroupInUseException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        GroupInUseException(const std::string& msg) throw ();

    };

}

#endif // !defined(GroupInUseException_H)
