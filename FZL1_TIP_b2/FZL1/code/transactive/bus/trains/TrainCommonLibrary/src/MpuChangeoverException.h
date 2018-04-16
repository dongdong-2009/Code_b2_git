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

#if !defined(MpuChangeoverException_H)
#define MpuChangeoverException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * This is used when an MPU changeover interrupts a train operation
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:54 PM
     */
    class MpuChangeoverException : public TrainException
    {
    public:

        /**
         * constructor
         */
        MpuChangeoverException() throw ();

        /**
         * destructor
         */
        virtual ~MpuChangeoverException() throw ();

        /**
         * 
         * @param msg
         * @exception ()
         */
        MpuChangeoverException(const std::string& msg) throw ();

    };

}

#endif // !defined(MpuChangeoverException_H)
