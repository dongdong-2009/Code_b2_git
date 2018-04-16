/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneSubDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * ITelephoneSubDirectory is an interface to an subdirectory object. 
  * It allows the subdirectory object implementation to be chagned 
  * (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(ITelephoneSubDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ITelephoneSubDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class ITelephoneSubDirectory : public virtual TA_Base_Core::IItem
    {

    public:
        virtual ~ITelephoneSubDirectory(){};

	    /**
	      * getPrimaryKey
	      * 
	      * Gets the primary key
	      * 
	      * @return virtual long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual long getPrimaryKey() = 0;

	    /**
	      * getSubDirName
	      * 
	      * Gets the sub directory name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getSubDirName() = 0;

	    /**
	      * getStatus
	      * 
	      * Gets the status 
	      * 
	      * @return virtual int 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual int getStatus() = 0;

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };


    typedef std::vector<ITelephoneSubDirectory*>  ITelephoneSubDirectories;
    typedef ITelephoneSubDirectories::iterator    ITelephoneSubDirectoriesIt;
} //close namespace TA_IRS_Core

#endif // !defined(ITelephoneSubDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
