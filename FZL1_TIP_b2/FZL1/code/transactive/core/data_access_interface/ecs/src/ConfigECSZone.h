/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ConfigECSZone.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigECSZone is a concrete implementation of IConfigECSZone, which is in turn an implementation
  * of IECSZone. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSZones.
  */


#if !defined(ConfigECSZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigECSZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/ecs/src/IConfigECSZone.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class ECSZoneHelper;

    class ConfigECSZone : public IConfigECSZone
    {

    public:
        /**
         * ConfigECSZone (constructor)
         *
         * Constructs a new instance of the ConfigECSZone with no id. This is used when
         * creating a *new* ECSZone - that is, one that does not yet exist in the database.
         */
        ConfigECSZone();


        /**
         * ConfigECSZone (constructor)
         *
         * Constructs a new instance of ConfigECSZone with the specified id. This is used 
         * when creating a ConfigECSZone based around an existing ECSZone in the database.
         *
         * @param key The key to this ECSZone in the database.
         */
        ConfigECSZone(const unsigned long idKey);


        /**
         * ConfigECSZone (copy constructor)
         *
         * @param ConfigECSZone& - The ECSZone to make a copy of.
         */
        ConfigECSZone( const ConfigECSZone& theConfigECSZone);

        /**
         * ConfigECSZone (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigECSZoneHelper::ConfigECSZoneHelper(IData*)
         *
         */
        ConfigECSZone(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigECSZone();


    public:

        /**
         * deleteThisObject
         *
         * Removes this ECSZone from the database. 
         * The calling application MUST then delete this ECSZone object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ECSZone was initially loaded from the database
         *      OR     - writeECSZoneData() has already been called
         *      This ECSZone has not been deleted
         */
        void deleteThisObject(bool cascade=false);


    

        virtual int getECSZoneId();
        virtual std::string getECSLocation();
        virtual std::string getTrack();
        virtual unsigned long getOccEquipmentEntityKey();
        virtual unsigned long getStnEquipmentEntityKey();
		virtual bool getIsIgnoringCongestion();

        virtual void setECSZoneId(int id);
        virtual void setECSLocation(const std::string& ecsLocation);
        virtual void setTrack(const std::string& track);
        virtual void setOccEquipmentEntityKey(unsigned long key);
        virtual void setStnEquipmentEntityKey(unsigned long key);
		virtual void setIsIgnoringCongestion(bool isIgnoringCongestion);

        virtual unsigned long getKey()
        {
            return getECSZoneId();
        }

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
         * getDateCreated
         *
         * Returns the date created for this ECSZone
         *
         * @return The date created for this ECSZone as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this ECSZone.
         *
         * @return The date modified for this ECSZone as a time_t.
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
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the ECSZone object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This ECSZone has not been deleted
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
            return m_ECSZoneChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the ECSZone has been changed by the user.
         *
         * @return bool - This will return true if the ECSZone does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the ECSZone changes is empty then nothing has changed
            return !m_ECSZoneChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this ECSZone is new. A new ECSZone is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the ECSZone has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for ECS Zones - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * invalidate
         *
         * Make the data contained by this ECSZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateECSZoneChanges
         *
         * This updates the map recording the changes to the ECSZone. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateECSZoneChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

		
        // Assignment ECSZone not used so it is made private
		ConfigECSZone& operator=(const ConfigECSZone&);


        ECSZoneHelper* m_ECSZoneHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        TA_Base_Core::ItemChanges m_ECSZoneChanges; // This is a map of all changes made to the ECSZone so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Core

#endif // !defined(ConfigECSZone_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
