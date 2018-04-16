/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ConfigECSStationMode.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigECSStationMode is a concrete implementation of IConfigECSStationMode, which is in turn an implementation
  * of IECSStationMode. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSStationModes.
  */


#if !defined(ConfigECSStationMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigECSStationMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/ecs/src/IConfigECSStationMode.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class ECSStationModeHelper;

    class ConfigECSStationMode : public IConfigECSStationMode
    {

    public:
        /**
         * ConfigECSStationMode (constructor)
         *
         * Constructs a new instance of the ConfigECSStationMode with no id. This is used when
         * creating a *new* ECSStationMode - that is, one that does not yet exist in the database.
         */
        ConfigECSStationMode();


        /**
         * ConfigECSStationMode (constructor)
         *
         * Constructs a new instance of ConfigECSStationMode with the specified id. This is used 
         * when creating a ConfigECSStationMode based around an existing ECSStationMode in the database.
         *
         * @param key The key to this ECSStationMode in the database.
         */
        ConfigECSStationMode(const unsigned long idKey);


        /**
         * ConfigECSStationMode (copy constructor)
         *
         * @param ConfigECSStationMode& - The ECSStationMode to make a copy of.
         */
        ConfigECSStationMode( const ConfigECSStationMode& theConfigECSStationMode);


        /**
         * ConfigECSStationMode (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigECSStationModeHelper::ConfigECSStationModeHelper(IData*)
         *
         */
        ConfigECSStationMode(unsigned long row, TA_Base_Core::IData& data);

        
        virtual ~ConfigECSStationMode();


    public:

        /**
         * deleteThisObject
         *
         * Removes this ECSStationMode from the database. 
         * The calling application MUST then delete this ECSStationMode object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ECSStationMode was initially loaded from the database
         *      OR     - writeECSStationModeData() has already been called
         *      This ECSStationMode has not been deleted
         */
        void deleteThisObject(bool cascade=false);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the ECSStationMode object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This ECSStationMode has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this ECSStationMode.
         *
         * @return The key for this ECSStationMode as an unsigned long.
         *
         * pre: Either - this ECSStationMode was initially loaded from the database
         *      OR     - writeECSStationModeData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();
        virtual unsigned long getStationMode();
        virtual unsigned long getLocationKey();
        virtual std::string getOperation();
        virtual std::string getTrack();
        virtual std::string getCondition();
        virtual std::string getCongestionLogic();

        virtual void setStationMode(const unsigned long mode);
        virtual void setLocationKey(const unsigned long locationKey);
        virtual void setOperation(const std::string& operation);
        virtual void setTrack(const std::string& track);
        virtual void setCondition(const std::string& condition);
        virtual void setCongestionLogic(const std::string& congestionLogic);

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the ECS Station Mode this is 
         * just the key in a string format e.g. "ECS Station Mode 1".
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
         * Not applicable for ECS Station Modes.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);

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
            return m_ECSStationModeChanges;
        }

       /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this operator. We cannot use keys for identification
         * in the configuration editor because new operators do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this operator
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };

        /**
         * getDateCreated
         *
         * Returns the date created for this ECSStationMode
         *
         * @return The date created for this ECSStationMode as a time_t.
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
         * Returns the date modified for this ECSStationMode.
         *
         * @return The date modified for this ECSStationMode as a time_t.
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
         * This should be called to determine if any part of the ECSStationMode has been changed by the user.
         *
         * @return bool - This will return true if the ECSStationMode does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the ECSStationMode changes is empty then nothing has changed
            return !m_ECSStationModeChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this ECSStationMode is new. A new ECSStationMode is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the ECSStationMode has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this ECSStationMode as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateECSStationModeChanges
         *
         * This updates the map recording the changes to the ECSStationMode. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateECSStationModeChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
		
        // Assignment ECSStationMode not used so it is made private
		ConfigECSStationMode& operator=(const ConfigECSStationMode&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of operators in the
                                          // Configuration Editor because new operators do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        ECSStationModeHelper* m_ECSStationModeHelper;


        TA_Base_Core::ItemChanges m_ECSStationModeChanges; // This is a map of all changes made to the ECSStationMode so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Core

#endif // !defined(ConfigECSStationMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
