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
#if !defined(ProtocolException_H)
#define ProtocolException_H

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"

namespace TA_IRS_Bus
{
    /**
     * Used when a protocol failure occurs.
     * @author Adam Radics
     * @version 1.0
     * @created 13-Nov-2007 3:50:59 PM
     */
    class ProtocolException : public TrainException
    {
    public:

        /**
         * constructor
         */
        ProtocolException() throw ();

        /**
         * destructor
         */
        virtual ~ProtocolException() throw ();

        /**
         * 
         * @param msg
         * @param fieldName    The name of the field
         * @param fieldValue    The field value
         * @exception ()
         */
        ProtocolException(const std::string& msg, 
                          const std::string& fieldName, 
                          const std::string& fieldValue) throw ();

        /**
         * Gets the name of the field that caused this exception
         * 
         * @return the field name
         */
        std::string getFieldName() const throw ();

        /**
         * Gets the invalid value that caused this exception
         * 
         * @return the field value
         */
        std::string getFieldValue() const throw ();

    private:

        /**
         * The invalid field name
         */
        std::string m_fieldName;

        /**
         * The invalid field value
         */
        std::string m_fieldValue;

    };

}
#endif // !defined(ProtocolException_H)
