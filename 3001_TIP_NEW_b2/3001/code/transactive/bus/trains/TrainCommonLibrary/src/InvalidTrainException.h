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

#if !defined(InvalidTrainException_H)
#define InvalidTrainException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when the train Id is invalid
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:53 PM
     */
    class InvalidTrainException : public TrainException
    {
    public:

        /**
         * constructor
         */
        InvalidTrainException() throw ();

        
        /**
         * destructor
         */
        virtual ~InvalidTrainException() throw ();

        
        /**
         * 
         * @param msg
         * @param trainId
         * @exception ()
         */
        InvalidTrainException(const std::string& msg, CommonTypes::TrainIdType trainId) throw ();


        /**
         * Gets the invalid train ID
         * 
         * @return the invalid train ID
         */
        CommonTypes::TrainIdType getTrainId() const throw ();

    private:

        CommonTypes::TrainIdType m_trainId;

    };

}

#endif // !defined(InvalidTrainException_H)
