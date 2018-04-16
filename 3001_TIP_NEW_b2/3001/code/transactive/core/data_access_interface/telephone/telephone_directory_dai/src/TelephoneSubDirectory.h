/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * TelephoneSubDirectory is an implementation of ITelephoneSubDirectory. It holds the data specific to an TelephoneSubDirectory entry
  * in the database, and allows read-only access to that data.
  */


#if !defined(TelephoneSubDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define TelephoneSubDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)


#include <string>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/TelephoneSubDirectoryHelper.h"
#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneSubDirectory.h"

namespace TA_IRS_Core
{
    class TelephoneSubDirectory : public virtual ITelephoneSubDirectory
    {

    public:

        /**
         * TelephoneSubDirectory (constructor)
         *
         * Construct an TelephoneSubDirectory class based around a key.
         *
         * @param key The key of this TelephoneSubDirectory in the database
         */
        TelephoneSubDirectory(const unsigned long idKey);

        /**
         * TelephoneSubDirectory (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TelephoneSubDirectoryHelper::TelephoneSubDirectoryHelper(IData*)
         *
         */
        TelephoneSubDirectory(unsigned long row, TA_Base_Core::IData& data);

        virtual ~TelephoneSubDirectory();

        /**
         * getName
         *
         * Returns the name of this item. 
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

        virtual long getPrimaryKey();
        virtual std::string getSubDirName();
        virtual int getStatus();

        virtual void invalidate();

		virtual unsigned long getKey()
        {
            return getPrimaryKey();
        }

    private:

	    /**
	      * operator=
	      * 
	      * Assignment TelephoneSubDirectory not used so it is made private
	      */
		TelephoneSubDirectory& operator=(const TelephoneSubDirectory &);
		
	    /**
	     * TelephoneSubDirectory
	     * 
         * Copy constructor should not be used and has therefore been made private. If a copy
         * constructor is required care should be taken. The copy constructor fo the TelephoneSubDirectoryHelper
         * has only been written for ConfigLocation objects and will not copy the TelephoneSubDirectory helper
         * for a read-only object (ie this one).
	     * 
	     */
        TelephoneSubDirectory( const TelephoneSubDirectory& theTelephoneSubDirectory);  

    private:
        /**
	      * Pointer to the TelephoneSubDirectoryHelper object
          */
        TA_IRS_Core::TelephoneSubDirectoryHelper * m_TelephoneSubDirectoryHelper;
    };
} // closes TA_Core

#endif // !defined(TelephoneSubDirectory_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
