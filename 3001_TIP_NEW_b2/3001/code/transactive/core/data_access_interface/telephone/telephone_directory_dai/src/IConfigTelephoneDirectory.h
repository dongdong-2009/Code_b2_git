/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * IConfigTelephoneDirectory - This is the interface supported by objects that allow configuration of
  * TelephoneDirectorys. It provides methods for retrieving, settings, and writing data for an 
  * TelephoneDirectory object.  The set methods in this interface do not set the values in the database
  * straight away. They only set the values in the object after validating that
  * they are correct.
  */

#if !defined(IConfigTelephoneDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
#define IConfigTelephoneDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_

#include <string>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/ITelephoneDirectory.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{

    class IConfigTelephoneDirectory : public virtual TA_Base_Core::IConfigItem, public virtual ITelephoneDirectory
    {

    public:
        virtual ~IConfigTelephoneDirectory() {};


	    /**
	      * setSubDirKey
	      * 
	      * Sets the sub directory key
	      * 
	      * @param : long entityKey
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSubDirKey(long entityKey) = 0;

	    /**
	      * setTelephoneNum
	      * 
	      * Sets the telephone number
	      * 
	      * @param : const std::string& number
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setTelephoneNum(const std::string& number) = 0;

	    /**
	      * setFullName
	      * 
	      * Sets the full name
	      * 
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setFullName(const std::string& name) = 0;

	    /**
	      * setLocation
	      * 
	      * Sets the location
	      * 
	      * @param : const std::string& location
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setLocation(const std::string& location) = 0;

	    /**
	      * setStatus
	      * 
	      * Sets the status
	      * 
	      * @return virtual void 
	      * @param : int status
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setStatus(int status) = 0;

        /**
         * deleteThisObject
         *
         * Removes this TelephoneDirectory from the database. 
         * The calling application MUST then delete this TelephoneDirectory object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this TelephoneDirectory was initially loaded from the database
         *      OR     - writeTelephoneDirectoryData() has already been called
         *      This TelephoneDirectory has not been deleted
         */
        virtual void deleteThisObject(bool cascade=false) = 0;

    };
    
    typedef std::vector<IConfigTelephoneDirectory*> IConfigTelephoneDirectories;
    typedef IConfigTelephoneDirectories::iterator   IConfigTelephoneDirectoriesIt;

} //close namespace TA_IRS_Core
#endif // !defined(IConfigTelephoneDirectory_9E7AE6B0_D600_4de9_9290_A8480311C649__INCLUDED_)
