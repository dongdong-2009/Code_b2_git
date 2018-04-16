/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneSubDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * IConfigTelephoneSubDirectory - This is the interface supported by objects that allow configuration of
  * subdirectory. It provides methods for retrieving, settings, and writing data for an subdirectory object.
  * The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigTelephoneSubDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigTelephoneSubDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneSubDirectory.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigTelephoneSubDirectory : public virtual TA_Base_Core::IConfigItem, public virtual ITelephoneSubDirectory
    {

    public:
        virtual ~IConfigTelephoneSubDirectory() {};

	    /**
	      * setSubDirName
	      * 
	      * Sets the sub directory name
	      * 
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSubDirName(const std::string& name) = 0;

	    /**
	      * setStatus
	      * 
	      * Sets the status 
	      * 
	      * @param : const int status
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setStatus(const int status) = 0;
        
        /**
         * deleteThisObject
         *
         * Remove this subdirectory from the database. Once this method has been executed, the
         * subdirectory object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in TE_DIRECTORY tables.  
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (TE_DIRECTORY).  Currently cascade feature is not implemented
         *
         * pre: - this subdirectory was initially loaded from the database
         *      - writeTelephoneSubDirData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigTelephoneSubDirectory*> IConfigTelephoneSubDirectories;
    typedef IConfigTelephoneSubDirectories::iterator   IConfigTelephoneSubDirectoriesIt;
} //close namespace TA_IRS_Core
#endif // !defined(IConfigTelephoneSubDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
