/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * ITelephoneDirectory is an interface to an TelephoneDirectory object. 
  * It allows the TelephoneDirectory object implementation to be chagned 
  * (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#if !defined(ITelephoneDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define ITelephoneDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <vector>
#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class ITelephoneSubDirectory;
    class ITelephoneDirectory : public virtual TA_Base_Core::IItem
    {

    public:
        virtual ~ITelephoneDirectory() {};

	    /**
	      * getPrimaryKey
	      * 
	      * Get the primary key
	      * 
	      * @return virtual unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getPrimaryKey() = 0;

	    /**
	      * getSubDirKey
	      * 
	      * Get the sub directory key
	      * 
	      * @return virtual unsigned long
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getSubDirKey() = 0;

	    /**
	      * getTelephoneNum
	      * 
	      * Get the telephone number
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getTelephoneNum() = 0;

	    /**
	      * getFullName
	      * 
	      * Get the fullname
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getFullName() = 0;

	    /**
	      * getLocation
	      * 
	      * Get the location
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getLocation() = 0;

	    /**
	      * getStatus
	      * 
	      * Get the status
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


        /**
         * getAssociatedSubdirectory
         *
         * Call TelephoneSubDirectoryAccessFactory to retrieve specified subdirectory using the m_subDirKey 
         * in this object.  
         *
         * pre:    This Telephone Directory was initially loaded from the database
         *         or writeTelephoneDirectoryData() has already been called
         *
         * @return the ITelephoneSubDirectory
         *         if the object cannot be found, NULL will be return instead.
         *         otherwise TelephoneSubDirectory will be return
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         */
        virtual ITelephoneSubDirectory* getAssociatedSubdirectory() = 0; 
    };


    typedef std::vector<ITelephoneDirectory*> ITelephoneDirectories;
    typedef ITelephoneDirectories::iterator   ITelephoneDirectoriesIt;
} //close namespace TA_IRS_Core

#endif // !defined(ITelephoneDirectory_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
