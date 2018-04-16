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

#if !defined(InvalidSelectionListException_H)
#define InvalidSelectionListException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when the given selection list does not exist
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:52 PM
     */
    class InvalidSelectionListException : public TrainException
    {
    public:

        /**
         * constructor
         */
        InvalidSelectionListException() throw ();

        /**
         * destructor
         */
        virtual ~InvalidSelectionListException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        InvalidSelectionListException(const std::string& msg) throw ();

    };

}
#endif // !defined(InvalidSelectionListException_H)
