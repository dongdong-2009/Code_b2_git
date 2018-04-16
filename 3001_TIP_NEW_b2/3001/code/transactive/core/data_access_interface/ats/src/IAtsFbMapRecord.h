/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/IAtsFbMapRecord.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/06/01 17:56:14 $
  * Last modified by: $Author: peter.wong $
  * 
  * IAtsFbMapRecord is an interface to an AtsFbMapRecord object. It allows the AtsFbMapRecord object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IAtsFbMapRecord_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IAtsFbMapRecord_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IAtsFbMapRecord : public virtual TA_Base_Core::IItem
    {

    public:

		virtual unsigned long getBranchId() =0;
		virtual std::string getBranchIdAsString() =0;
        virtual std::string getTrack() =0;
		virtual unsigned long getMinAbscissa() =0;
		virtual unsigned long getMaxAbscissa() =0;
		virtual double getZoneId() =0;
		virtual std::string getZoneTypeName() =0;
		virtual unsigned long getArmId() = 0;
        
        virtual ~IAtsFbMapRecord() {};

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<IAtsFbMapRecord*> IAtsFbMapRecords;
    typedef IAtsFbMapRecords::iterator    IAtsFbMapRecordsIt;
} //close namespace TA_IRS_Core

#endif // !defined(IAtsFbMapRecord_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
