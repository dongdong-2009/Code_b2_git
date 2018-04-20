/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsPlatform.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigAtsPlatform is a concrete implementation of IConfigAtsPlatform, which is in turn an implementation
  * of IAtsPlatform. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsPlatforms.
  */


#if !defined(ConfigAtsPlatform_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigAtsPlatform_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/ats/src/IConfigAtsPlatform.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class AtsPlatformHelper;

    class ConfigAtsPlatform : public IConfigAtsPlatform
    {

    public:
        /**
         * ConfigAtsPlatform (constructor)
         *
         * Constructs a new instance of the ConfigAtsPlatform with no id. This is used when
         * creating a *new* AtsPlatform - that is, one that does not yet exist in the database.
         */
        ConfigAtsPlatform();


        /**
         * ConfigAtsPlatform (constructor)
         *
         * Constructs a new instance of ConfigAtsPlatform with the specified id. This is used 
         * when creating a ConfigAtsPlatform based around an existing AtsPlatform in the database.
         *
         * @param key The key to this AtsPlatform in the database.
         */
        ConfigAtsPlatform(const unsigned long idKey);


        /**
         * ConfigAtsPlatform (copy constructor)
         *
         * @param ConfigAtsPlatform& - The AtsPlatform to make a copy of.
         */
        ConfigAtsPlatform( const ConfigAtsPlatform& theConfigAtsPlatform);

        /**
         * ConfigAtsPlatform (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigAtsPlatformHelper::ConfigAtsPlatformHelper(IData*)
         *
         */
        ConfigAtsPlatform(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigAtsPlatform();


    public:

        /**
         * deleteThisObject
         *
         * Removes this AtsPlatform from the database. 
         * The calling application MUST then delete this AtsPlatform object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this AtsPlatform was initially loaded from the database
         *      OR     - writeAtsPlatformData() has already been called
         *      This AtsPlatform has not been deleted
         */
        void deleteThisObject(bool cascade=false);

		
        virtual unsigned long getPlatformId();
		virtual unsigned long getStationId();
        virtual unsigned char getTisPlatformId();
        
		virtual void setPlatformId(unsigned long id);
        virtual void setStationId(unsigned long id);
        virtual void setTisPlatformId(unsigned char id);
       
        virtual unsigned long getKey();

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
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the AtsPlatform object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This AtsPlatform has not been deleted
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
            return m_atsPlatformChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the AtsPlatform has been changed by the user.
         *
         * @return bool - This will return true if the AtsPlatform does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the AtsPlatform changes is empty then nothing has changed
            return !m_atsPlatformChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this AtsPlatform is new. A new AtsPlatform is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the AtsPlatform has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for AtsPlatform - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * invalidate
         *
         * Make the data contained by this AtsPlatform as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateAtsPlatformChanges
         *
         * This updates the map recording the changes to the AtsPlatform. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateAtsPlatformChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

		
        // Assignment AtsPlatform not used so it is made private
		ConfigAtsPlatform& operator=(const ConfigAtsPlatform&);


        AtsPlatformHelper* m_atsPlatformHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        TA_Base_Core::ItemChanges m_atsPlatformChanges; // This is a map of all changes made to the AtsPlatform so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Core

#endif // !defined(ConfigAtsPlatform_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
