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

#if !defined(CctvConflictException_H)
#define CctvConflictException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_Bus
{
    /**
     * This is raised when a CCTV operation is denied due to a conflicting train.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:34 PM
     */
    class CctvConflictException : public TrainException
    {
    public:

        /**
         * @constructor
         */
        CctvConflictException() throw ();

        /**
         * destructor
         */
        virtual ~CctvConflictException() throw ();

        /**
         * 
         * @param msg
         * @param otherTrain    The other train that is broadcasting
         * @exception ()
         */
        CctvConflictException( const std::string& msg, 
                               CommonTypes::TrainIdType otherTrain ) throw ();

        /**
         * Gets the error given by the train.
         * 
         * @return the error reason
         */
        CommonTypes::TrainIdType getConflictingTrain() const throw ();

    private:

        /**
         * The conflicting train
         */
        CommonTypes::TrainIdType m_trainId;

    };

}

#endif // !defined(CctvConflictException_H)
