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
#if !defined(InvalidParameterException_H)
#define InvalidParameterException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Thrown when a given parameter for an operation is invalid
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:50 PM
     */
    class InvalidParameterException : public TrainException
    {
    public:

        /**
         * constructor
         */
        InvalidParameterException() throw ();

        /**
         * destructor
         */
        virtual ~InvalidParameterException() throw ();

        /**
         * 
         * @param msg
         * @param parameterName    The invalid parameter name
         * @param parameterValue    The invalid parameter value in string format
         * @exception ()
         */
        InvalidParameterException(const std::string& msg, 
                                  const std::string& parameterName, 
                                  const std::string& parameterValue) throw ();


        /**
         * Gets the invalid parameter name that caused this exception
         * 
         * @return the parameter name
         */
        std::string getParameterName() const throw ();


        /**
         * Gets the invalid parameter value that caused this exception
         * 
         * @return the parameter value
         */
        std::string getParameterValue() const throw ();

    private:

        /**
         * The invalid parameter name
         */
        std::string m_parameterName;

        /**
         * The invalid parameter value
         */
        std::string m_parameterValue;

    };

}
#endif // !defined(InvalidParameterException_H)
