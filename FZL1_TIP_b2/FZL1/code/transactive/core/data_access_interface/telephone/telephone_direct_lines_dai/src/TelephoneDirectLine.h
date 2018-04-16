 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLine.h $
  * @author Raymond Pau
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by: $Author: CM $
  * 
  * TelephoneDirectLine is an implementation of ITelephoneDirectLine. It holds the data specific to an TelephoneDirectLine entry
  * in the database, and allows read-only access to that data.
  */

#if !defined(TelephoneDirectLine_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define TelephoneDirectLine_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/TelephoneDirectLineHelper.h"
#include "core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectLine.h"

namespace TA_IRS_Core
{

    class TelephoneDirectLine : public ITelephoneDirectLine
    {
    public:
        /**
         * TelephoneDirectLine (constructor)
         *
         * Construct an TelephoneDirectLine class based around a key.
         *
         * @param key The key of this TelephoneDirectLine in the database
         */
        TelephoneDirectLine(const unsigned long key);

        /**
         * TelephoneDirectLine (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TelephoneDirectLineHelper::TelephoneDirectLineHelper(IData*)
         */
        TelephoneDirectLine(unsigned long row, TA_Base_Core::IData& data);
    
        /**
         * TelephoneDirectLine (constructor)
         *
         * Constructs a new instance using the TE_SPEED_DIAL_SET information
         */
        TelephoneDirectLine(unsigned long setPrimaryKey, long operaterKey, long profId, const std::string& setName);


	    /**
	     * TelephoneDirectLine
	     * 
	     * Default Constructor
	     * 
	     * @return 
	     * 
	     * @exception <exceptions> Optional
	     */
        TelephoneDirectLine();

        virtual ~TelephoneDirectLine();

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        virtual unsigned long getBtnSetKey();
        virtual long getOperatorKey();
        virtual long getSeProfId();
        virtual std::string getSetName();
        virtual int getButtonPosition();
        virtual std::string getTelephoneNumber();
        virtual std::string getLabel();

    private:
        /**
         * Assignment TelephoneDirectLine not used so it is made private
         */
	    TelephoneDirectLine& operator=(const TelephoneDirectLine &);
	    
        /**
         * Copy constructor should not be used and has therefore been made private. If a copy
         * constructor is required care should be taken. The copy constructor fo the TelephoneDirectLineHelper
         * has only been written for ConfigLocation objects and will not copy the TelephoneDirectLine helper
         * for a read-only object (ie this one).
         */
        TelephoneDirectLine(const TelephoneDirectLine& theTelephoneDirectLine);  

        /**
         * pointer to TelephoneDirectLineHelper
         */
        TelephoneDirectLineHelper * m_teSpeedDialSetHelper;
    };
} // close TA_IRS_Core

#endif // !defined(TelephoneDirectLine_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
