/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/pa_4669/src/ConfigPaDvaMessagePublicVersion.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigPaDvaMessagePublicVersion is a concrete implementation of IConfigPaDvaMessagePublicVersion, 
  * which is in turn an implementation
  * of IPaDvaMessagePublicVersion. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for PaDvaMessagePublicVersions.
  *
  */


#if !defined ConfigPaDvaMessagePublicVersion_H
#define ConfigPaDvaMessagePublicVersion_H

#include <string>
#include <set>

#include "core/data_access_interface/pa_4669/src/IConfigPaDvaMessagePublicVersion.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessagePublicVersionHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_Base_Core
{
    class PaDvaMessagePublicVersioncHelper;

    class ConfigPaDvaMessagePublicVersion : public IConfigPaDvaMessagePublicVersion
    {

    public:
        /**
         * ConfigPaDvaMessagePublicVersion (constructor)
         *
         * Constructs a new instance of the ConfigPaDvaMessagePublicVersion with no id. This is used when
         * creating a *new* PaDvaMessagePublicVersion - that is, one that does not yet exist in the database.
         */
        ConfigPaDvaMessagePublicVersion();

        /**
         * ConfigPaDvaMessagePublicVersion (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see PaDvaMessagePublicVersionHelper::PaDvaMessagePublicVersionHelper(IData*)
         *
         */
        ConfigPaDvaMessagePublicVersion(unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigPaDvaMessagePublicVersion (constructor)
         *
         * Constructs a new instance of ConfigPaDvaMessagePublicVersion with the specified id. This is used 
         * when creating a ConfigPaDvaMessagePublicVersion based around an existing PaDvaMessagePublicVersion in the database.
         *
         * @param key The key to this PaDvaMessagePublicVersion in the database.
         */
        ConfigPaDvaMessagePublicVersion(const unsigned long idKey);


        /**
         * ConfigPaDvaMessagePublicVersion (copy constructor)
         *
         * @param ConfigPaDvaMessagePublicVersion& - The PaDvaMessagePublicVersion to make a copy of.
         */
        ConfigPaDvaMessagePublicVersion( const ConfigPaDvaMessagePublicVersion& theConfigPaDvaMessagePublicVersion);

        
        virtual ~ConfigPaDvaMessagePublicVersion();
        
    public:

        /**
         * deleteThisObject
         *
         * Removes this PaDvaMessagePublicVersion from the database. 
         * The calling application MUST then delete this PaDvaMessagePublicVersion object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this PaDvaMessagePublicVersion was initially loaded from the database
         *      OR     - writePaDvaMessagePublicVersionData() has already been called
         *      This PaDvaMessagePublicVersion has not been deleted
         */
        void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the PaDvaMessagePublicVersion object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This PaDvaMessagePublicVersion has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this PaDvaMessagePublicVersion.
         *
         * @return The key for this PaDvaMessagePublicVersion as an unsigned long.
         *
         * pre: Either - this PaDvaMessagePublicVersion was initially loaded from the database
         *      OR     - writePaDvaMessagePublicVersionData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getLocationKey();
        virtual std::string getMessageVersion(IPaDvaMessagePublicVersion::ELocation location, unsigned long stationDvaMessageId);
        virtual std::string getVersionData(EVersionColumn column);

        virtual void setLocationKey(unsigned long key);
        virtual void setVersionData(const std::string& newVersion,
                                        IPaDvaMessagePublicVersion::EVersionColumn column);


         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA DVA Message Version this is 
         * just the key in a string format e.g. "PA DVA Message Version 1".
         *
         * @return The key description for this item as a std::string
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
         * setName
         *
         * Not applicable for PA DVA Message Versions.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the columns. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_paDvaMessagePublicVersionChanges;
        }

       /**
         * getUniqueIdentifier
         *
         * This retrieves the unique identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this operator
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        }

        /**
         * getDateCreated
         *
         * Returns the date created for this PaDvaMessagePublicVersion
         *
         * @return The date created for this PaDvaMessagePublicVersion as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong amount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PaDvaMessagePublicVersion.
         *
         * @return The date modified for this PaDvaMessagePublicVersion as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the PaDvaMessagePublicVersion has been changed by the user.
         *
         * @return bool - This will return true if the PaDvaMessagePublicVersion does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the PaDvaMessagePublicVersion changes is empty then nothing has changed
            return !m_paDvaMessagePublicVersionChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this PaDvaMessagePublicVersion is new. A new PaDvaMessagePublicVersion is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the PaDvaMessagePublicVersion has not been applied to the database.
         */
        virtual bool isNew();



        /**
         * invalidate
         *
         * Make the data contained by this PaDvaMessagePublicVersion as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


    private:
            
	    /**
         * updatePaDvaMessagePublicVersionChanges
         *
         * This updates the map recording the changes to the PaDvaMessagePublicVersion. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updatePaDvaMessagePublicVersionChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updatePaDvaMessagePublicVersionChanges
         *
         * This updates the map recording the changes to the PaDvaMessagePublicVersion. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updatePaDvaMessagePublicVersionChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment PaDvaMessagePublicVersion not used so it is made private
		ConfigPaDvaMessagePublicVersion& operator=(const ConfigPaDvaMessagePublicVersion&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of PaDvaMessagePublicVersions in the
                                          // Configuration Editor because new PaDvaMessagePublicVersions do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        PaDvaMessagePublicVersionHelper* m_paDvaMessagePublicVersionHelper;


        TA_Base_Core::ItemChanges m_paDvaMessagePublicVersionChanges; // This is a map of all changes made to the PaDvaMessagePublicVersion so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Base_Core

#endif // !defined ConfigPaDvaMessagePublicVersion_H
