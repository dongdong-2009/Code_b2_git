/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_direct_lines_dai/src/ITelephoneDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
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

        virtual long getPrimaryKey() = 0;
        virtual long getSubDirKey() = 0;
        virtual std::string getTelephoneNum() = 0;
        virtual std::string getFullName() = 0;
        virtual std::string getLocation() = 0;
        virtual int getStatus() = 0;

        virtual ~ITelephoneDirectory() {};

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
