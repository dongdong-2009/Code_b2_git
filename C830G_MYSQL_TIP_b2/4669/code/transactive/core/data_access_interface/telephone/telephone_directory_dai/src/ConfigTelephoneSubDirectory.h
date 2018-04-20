/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneSubDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * ConfigTelephoneSubDirectory is a concrete implementation of IConfigTelephoneSubDirectory, 
  * which is in turn an implementation of ITelephoneSubDirectory. 
  * It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TE_SUBDIRECTORY.
  */


#if !defined(ConfigTelephoneSubDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigTelephoneSubDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>

#include "core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneSubDirectory.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class TelephoneSubDirectoryHelper;

    class ConfigTelephoneSubDirectory : public virtual IConfigTelephoneSubDirectory
    {

    public:
        /**
         * ConfigTelephoneSubDirectory (constructor)
         *
         * Constructs a new instance of the ConfigTelephoneSubDirectory with no id. This is used when
         * creating a *new* subdirectory - that is, one that does not yet exist in the database.
         */
        ConfigTelephoneSubDirectory();

        /**
         * ConfigTelephoneSubDirectory (constructor)
         *
         * Constructs a new instance of ConfigTelephoneSubDirectory with the specified id. This is used 
         * when creating a ConfigTelephoneSubDirectory based around an existing subdirectory in the database.
         *
         * @param key The key to this subdirectory in the database.
         */
        ConfigTelephoneSubDirectory(const unsigned long idKey);

        /**
         * ConfigTelephoneSubDirectory (copy constructor)
         *
         * @param ConfigTelephoneSubDirectory& - The subdirectory to make a copy of.
         */
        ConfigTelephoneSubDirectory( const ConfigTelephoneSubDirectory& theConfigTelephoneSubDirectory);

        /**
         * ConfigTelephoneSubDirectory (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigTelephoneSubDirectoryHelper::ConfigTelephoneSubDirectoryHelper(IData*)
         *
         */
        ConfigTelephoneSubDirectory(unsigned long row, TA_Base_Core::IData& data);
        

	    /**
	      * ~ConfigTelephoneSubDirectory
	      * 
	      * Destructor
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual ~ConfigTelephoneSubDirectory();

    public:
        /**
         * deleteThisObject
         *
         * Removes this subdirectory from the database. 
         * The calling application MUST then delete this subdirectory object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException 
         *            A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this subdirectory was initially loaded from the database
         *      OR     - writesubdirectoryData() has already been called
         *      This subdirectory has not been deleted
         */
        void deleteThisObject(bool cascade=false);

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

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.  
         * It will call writeTelephoneSubDirData to add a new record or modify an existing one
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. TEDENT_ID should be an unsigned long)
         *          - NO_VALUE if the TelephoneDirectory record cannot be found for the helper 
         *            constructed with an TelephoneDirectory.
         *          - NOT_UNIQUE if the TelephoneDirectory returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the TelephoneDirectory object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         * pre: This subdirectory has not been deleted
         */
        void applyChanges();
    
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }

        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the display. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_SubDirectoryChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the subdirectory has been changed by the user.
         *
         * @return bool - This will return true if the subdirectory does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the subdirectory changes is empty then nothing has changed
            return !m_SubDirectoryChanges.empty();
        };

        /**
         * isNew
         *
         * This should be called to determine if this subdirectory is new. A new subdirectory is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the subdirectory has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for subdirectory - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);

        /**
         * invalidate
         *
         * Make the data contained by this subdirectory as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();

	    /**
	      * getPrimaryKey
	      * 
	      * Returns the primary key
	      * 
	      * @return virtual long 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual long getPrimaryKey();

	    /**
	      * getSubDirName
	      * 
	      * Returns the sub directory name
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getSubDirName();

	    /**
	      * getStatus
	      * 
	      * Returns the status of the record
	      * 
	      * @return virtual int 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual int getStatus();

	    /**
	      * setSubDirName
	      * 
	      * Set the sub directory name
	      * 
	      * @param : const std::string& name
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSubDirName(const std::string& name);

	    /**
	      * setStatus
	      * 
	      * Set the status of the record
	      * 
	      * @param : const int status
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setStatus(const int status);

	    /**
	      * long getKey
	      * 
	      * Returns the primary key
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getKey()
        {
            return getPrimaryKey();
        }

    private:
	    /**
         * updateSubdirectoryChanges
         *
         * This updates the map recording the changes to the subdirectory. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateSubdirectoryChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
	     * operator=
	     * 
	     * Assignment subdirectory not used so it is made private
	     * 
	     * @return ConfigTelephoneSubDirectory& 
	     * @param : const ConfigTelephoneSubDirectory&
	     * 
	     * @exception <exceptions> Optional
	     */
		ConfigTelephoneSubDirectory& operator=(const ConfigTelephoneSubDirectory&);

    private:
        /**
	     * Pointer to TelephoneSubDirectoryHelper
         */
        TelephoneSubDirectoryHelper* m_telephoneSubDirectoryHelper;

        /**
	     * We cannot use keys for identification of guis in the
         * Configuration Editor because new guis do not have keys.
         * Therefore we use this method for identification.
         */
        unsigned long m_uniqueIdentifier; 

        /**
	     * The next unique identifier available
         */
        static long s_nextAvailableIdentifier;  

        /**
	     * This is a map of all changes made to the sub directory so that they are
         * recorded and can be sent in audit or online update messages
         */
        TA_Base_Core::ItemChanges m_SubDirectoryChanges;
    };
} // closes TA_Core

#endif // !defined(ConfigTelephoneSubDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
