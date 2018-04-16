/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/core/data_access_interface/telephone/telephone_directory_dai/src/ConfigTelephoneDirectory.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  *
  * ConfigTelephoneDirectory is a concrete implementation of IConfigTelephoneDirectory, 
  * which is in turn an implementation of ITelephoneDirectory. 
  * It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TelephoneDirectorys.
  */

#if !defined(ConfigTelephoneDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigTelephoneDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include "core/data_access_interface/telephone/telephone_directory_dai/src/IConfigTelephoneDirectory.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class TelephoneDirectoryHelper;

    class ConfigTelephoneDirectory : public IConfigTelephoneDirectory
    {

    public:
        /**
         * ConfigTelephoneDirectory (constructor)
         *
         * Constructs a new instance of the ConfigTelephoneDirectory with no id. This is used when
         * creating a *new* object - that is, one that does not yet exist in the database.
         */
        ConfigTelephoneDirectory();

        /**
         * ConfigTelephoneDirectory (constructor)
         *
         * Constructs a new instance of ConfigTelephoneDirectory with the specified id. This is used 
         * when creating a ConfigTelephoneDirectory based around an existing TelephoneDirectory in the 
         * database.
         *
         * @param key The key to this TelephoneDirectory in the database.
         */
        ConfigTelephoneDirectory(const unsigned long idKey);

        /**
         * ConfigTelephoneDirectory (copy constructor)
         *
         * @param ConfigTelephoneDirectory& - The TelephoneDirectory to make a copy of.
         */
        ConfigTelephoneDirectory( const ConfigTelephoneDirectory& theConfigTelephoneDirectory);

        /**
         * ConfigTelephoneDirectory (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigTelephoneDirectoryHelper::ConfigTelephoneDirectoryHelper(IData*)
         *
         */
        ConfigTelephoneDirectory(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigTelephoneDirectory();

    public:
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
        void deleteThisObject(bool cascade=false);

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
         * It will call writeTelephoneDirectoryData to add a new record or modify an existing one
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
         * pre: This TelephoneDirectory has not been deleted
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
            return m_telephoneDirectoryChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the TelephoneDirectory has been changed by the user.
         *
         * @return bool - This will return true if the TelephoneDirectory does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the TelephoneDirectory changes is empty then nothing has changed
            return !m_telephoneDirectoryChanges.empty();
        };

        /**
         * isNew
         *
         * This should be called to determine if this TelephoneDirectory is new. A new TelephoneDirectory is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the TelephoneDirectory has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for TelephoneDirectory - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);

        /**
         * invalidate
         *
         * Make the data contained by this TelephoneDirectory as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();

        /**
	      * long getPrimaryKey
	      * 
	      * Returns the primary key of the record
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
            virtual unsigned long getPrimaryKey();

	    /**
	      * long getSubDirKey
	      * 
	      * Returns the sub directory key of the record
          * If data was not set, reload() will be called
	      * 
	      * @return virtual unsigned 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual unsigned long getSubDirKey();

	    /**
	      * getTelephoneNum
	      * 
	      * Returns the telephone number
          * If data was not set, reload() will be called
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getTelephoneNum();

	    /**
	      * getFullName
	      * 
	      * Returns the fullname  
          * If data was not set, reload() will be called
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
          * If data was not set, reload() will be called
	      * 
	      * @return virtual std::string 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual std::string getLocation();

	    /**
	      * getStatus
	      * 
	      * Returns the Status  
          * If data was not set, reload() will be called
	      * 
	      * @return virtual int 
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual int getStatus();
    

	    /**
	      * setSubDirKey
	      * 
	      * Set the sub directory key value
	      * 
	      * @param : long entityKey
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setSubDirKey(long entityKey);

	    /**
	      * setTelephoneNum
	      * 
	      * Set the telephone number
	      * 
	      * @param : const std::string& number
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setTelephoneNum(const std::string& number);

	    /**
	      * setFullName
	      * 
	      * Set the full name
	      * 
	      * @param : const std::string& fullName
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setFullName(const std::string& fullName);

	    /**
	      * setLocation
	      * 
	      * Set the location
	      * 
	      * @param : const std::string& location
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setLocation(const std::string& location);

	    /**
	      * setStatus
	      * 
	      * Set the status
	      * 
	      * @param : int status
	      * 
	      * @exception <exceptions> Optional
	      */
        virtual void setStatus(int status);


	    /**
	      * long getKey
	      * 
	      * Retrieve the primary key
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
         * updateTelephoneDirectoryChanges
         *
         * This updates the map recording the changes to the TelephoneDirectory. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateTelephoneDirectoryChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
		
	    /**
	     * operator=
	     * 
         * Assignment TelephoneDirectory not used so it is made private
	     * 
	     * @return ConfigTelephoneDirectory& 
	     * @param : const ConfigTelephoneDirectory&
	     * 
	     */
		ConfigTelephoneDirectory& operator=(const ConfigTelephoneDirectory&);

    private:
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
         * This is a map of all changes made to the TelephoneDirectory so that they are
         * recorded and can be sent in audit or online update messages.
         */
        TA_Base_Core::ItemChanges m_telephoneDirectoryChanges; 

        /**
         * Holds a pointer to the the TelephoneDirectoryHelper Class
         */
        TelephoneDirectoryHelper* m_telephoneDirectoryHelper;
    };
} // closes TA_Core

#endif // !defined(ConfigTelephoneDirectory_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
