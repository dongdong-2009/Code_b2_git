/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * TelephoneDirectory is an implementation of ITelephoneDirectory. It holds the data specific to a
  * TelephoneDirectory entry in the database, and allows read-only access to that data.
  */


#if !defined(TelephoneDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define TelephoneDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneDirectoryHelper.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h"

namespace TA_IRS_Core
{
    class TelephoneDirectory : public ITelephoneDirectory
    {

    public:

        /**
         * TelephoneDirectory (constructor)
         *
         * Construct an TelephoneDirectory class based around a key.
         *
         * @param key The key of this TelephoneDirectory in the database
         */
        TelephoneDirectory(const unsigned long idKey);

        /**
         * TelephoneDirectory (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TelephoneDirectoryHelper::TelephoneDirectoryHelper(IData*)
         *
         */
        TelephoneDirectory(unsigned long row, TA_Base_Core::IData& data);

        virtual ~TelephoneDirectory();

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

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
        virtual ITelephoneSubDirectory* getAssociatedSubdirectory();

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

	    /**
	      * getPrimaryKey
	      * 
	      * Return the primary key value
	      * 
	      * @return virtual unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getPrimaryKey();

	    /**
	      * getSubDirKey
	      * 
	      * Return the sub directory key value
	      * 
	      * @return virtual unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getSubDirKey();

	    /**
	      * getTelephoneNum
	      * 
	      * Return the telephone number
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getTelephoneNum();

	    /**
	      * getFullName
	      * 
	      * Returns the full name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getFullName();

	    /**
	      * getLocation
	      * 
	      * Returns the location 
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getLocation();

	    /**
	      * getStatus
	      * 
	      * Returns the status
	      * 
	      * @return virtual int 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual int getStatus();


	    /**
	      * getKey
	      * 
	      * Returns the primary key
	      * 
	      * @return virtual unsigned long 
	      * 
	      * @exception <exceptions> Optional
	      */
		virtual unsigned long getKey()
        {
            return getPrimaryKey();
        }

    private:

        // Assignment TelephoneDirectory not used so it is made private
		TelephoneDirectory& operator=(const TelephoneDirectory &);
		
        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor fo the TelephoneDirectoryHelper
        // has only been written for ConfigLocation objects and will not copy the TelephoneDirectory helper
        // for a read-only object (ie this one).
        TelephoneDirectory( const TelephoneDirectory& theTelephoneDirectory);  

    private:

        /**
	      * Pointer to TelephoneDirectoryHelper object
          */
        TA_IRS_Core::TelephoneDirectoryHelper * m_telephoneDirectoryHelper;
    };
} // closes TA_Core

#endif // !defined(TelephoneDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
