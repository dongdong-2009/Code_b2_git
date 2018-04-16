 /**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsFbMapRecord.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * AtsFbMapRecord is an implementation of IAtsFbMapRecord. It holds the data specific to an AtsFbMapRecord entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(AtsFbMapRecord_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define AtsFbMapRecord_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/ats/src/AtsFbMapRecordHelper.h"
#include "core/data_access_interface/ats/src/IAtsFbMapRecord.h"

namespace TA_IRS_Core
{
    class AtsFbMapRecord : public IAtsFbMapRecord
    {

    public:

        /**
         * AtsFbMapRecord (constructor)
         *
         * Construct an AtsFbMapRecord class based around a key.
         *
         * @param key The key of this AtsFbMapRecord in the database
         */
        AtsFbMapRecord(const unsigned long idKey);

        /**
         * AtsFbMapRecord (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see AtsFbMapRecordHelper::AtsFbMapRecordHelper(IData*)
         *
         */
        AtsFbMapRecord(unsigned long row, TA_Base_Core::IData& data);

        virtual ~AtsFbMapRecord();

		virtual unsigned long getBranchId();
		virtual std::string getBranchIdAsString();
		virtual std::string getTrack();
		virtual unsigned long getMinAbscissa();
		virtual unsigned long getMaxAbscissa();
		virtual double getZoneId();
		virtual std::string getZoneTypeName();
		virtual unsigned long getArmId();
 
        virtual void invalidate();

		virtual unsigned long getKey();

        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
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

	public:
		static const std::string TRACK_INNERBOUND;
		static const std::string TRACK_CENTRALINNERBOUND;
		static const std::string TRACK_OUTERBOUND;
		static const std::string TRACK_CENTRALOUTERBOUND;

    private:

        // Assignment AtsFbMapRecord not used so it is made private
		AtsFbMapRecord& operator=(const AtsFbMapRecord &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the AtsFbMapRecordHelper
        // has only been written for ConfigLocation objects and will not copy the AtsFbMapRecord helper
        // for a read-only object (ie this one).
        AtsFbMapRecord( const AtsFbMapRecord& theAtsFbMapRecord);  


        TA_IRS_Core::AtsFbMapRecordHelper * m_atsFbMapRecordHelper;
    };
} // closes TA_Core

#endif // !defined(AtsFbMapRecord_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
