/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ecs/src/ConfigECSMasterMode.h $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2013/09/18 15:18:25 $
  * Last modified by: $Author: huangjian $
  * 
  * ConfigECSMasterMode is a concrete implementation of IConfigECSMasterMode, which is in turn an implementation
  * of IECSMasterMode. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSMasterModes.
  */


#if !defined(ConfigECSMasterMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigECSMasterMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/ecs/src/IConfigECSMasterMode.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class ECSMasterModeHelper;

    class ConfigECSMasterMode : public IConfigECSMasterMode
    {

    public:
        /**
         * ConfigECSMasterMode (constructor)
         *
         * Constructs a new instance of the ConfigECSMasterMode with no id. This is used when
         * creating a *new* ECSMasterMode - that is, one that does not yet exist in the database.
         */
        ConfigECSMasterMode();


        /**
         * ConfigECSMasterMode (constructor)
         *
         * Constructs a new instance of ConfigECSMasterMode with the specified id. This is used 
         * when creating a ConfigECSMasterMode based around an existing ECSMasterMode in the database.
         *
         * @param key The key to this ECSMasterMode in the database.
         */
        ConfigECSMasterMode(const unsigned long idKey);


        /**
         * ConfigECSMasterMode (copy constructor)
         *
         * @param ConfigECSMasterMode& - The ECSMasterMode to make a copy of.
         */
        ConfigECSMasterMode( const ConfigECSMasterMode& theConfigECSMasterMode);


        /**
         * ConfigECSMasterMode (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigECSMasterModeHelper::ConfigECSMasterModeHelper(IData*)
         *
         */
        ConfigECSMasterMode(unsigned long row, TA_Base_Core::IData& data);
        

        virtual ~ConfigECSMasterMode();


    public:

        /**
         * deleteThisObject
         *
         * Removes this ECSMasterMode from the database. 
         * The calling application MUST then delete this ECSMasterMode object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ECSMasterMode was initially loaded from the database
         *      OR     - writeECSMasterModeData() has already been called
         *      This ECSMasterMode has not been deleted
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
         * @exception DataConfigurationException If the data contained in the ECSMasterMode object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This ECSMasterMode has not been deleted
         */
        void applyChanges();
        

        /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the ECS Master Mode this is 
         * just the key in a string format e.g. "ECS Master Mode 1".
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
         * getKey
         *
         * Returns the key for this ECSMasterMode.
         *
         * @return The key for this ECSMasterMode as an unsigned long.
         *
         * pre: Either - this ECSMasterMode was initially loaded from the database
         *      OR     - writeECSMasterModeData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        virtual unsigned long getKey();

        virtual unsigned long getMasterMode();
        virtual std::string getCondition();
        virtual int getECSZoneId();
        virtual std::string getEvacuationDirection();
        virtual std::string getAirflowDirection();
		virtual std::string getSituation();
        virtual std::string getDisplayDirection();
		virtual std::string getVerificationMessage();


        /**
         * setName
         *
         * Not applicable for ECS Master Modes.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);

        virtual void setMasterMode(const unsigned long mode);
        virtual void setCondition(const std::string& Condition);
        virtual void setECSZoneId(const int zoneId);
        virtual void setEvacuationDirection(const std::string& evacDir);
        virtual void setAirflowDirection(const std::string& airDir);
		virtual void setSituation(const std::string& situation);
        virtual void setDisplayDirection(const std::string& displayDir);
		virtual void setVerificationMessage(const std::string& msg);


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
            return m_ECSMasterModeChanges;
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
         * Returns the date created for this ECSMasterMode
         *
         * @return The date created for this ECSMasterMode as a time_t.
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
         * Returns the date modified for this ECSMasterMode.
         *
         * @return The date modified for this ECSMasterMode as a time_t.
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
         * This should be called to determine if any part of the ECSMasterMode has been changed by the user.
         *
         * @return bool - This will return true if the ECSMasterMode does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the ECSMasterMode changes is empty then nothing has changed
            return !m_ECSMasterModeChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this ECSMasterMode is new. A new ECSMasterMode is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the ECSMasterMode has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * getAssociatedStationModes
         *
         * Returns the StationModes that this MasterMode triggers.
         *
         * @return The StationModes triggered by this MasterMode, as a vector of IStationMode*'s
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<IECSStationMode*> getAssociatedStationModes();

        
        /**
         * setAssociatedStationMode
         *
         * Changes a StationMode association for this MasterMode
         *
         * @param unsigned long - This is the key of the StationMode to add or remove from the associations
         * @param bool - This indicates whether this StationMode is to be added or removed. True if the
         *               StationMode is to be added to the associations, false if the StationMode is to be
         *               removed.
         *
         * pre: This MasterMode has not been deleted
         */
        virtual void setAssociatedStationMode( unsigned long StationModeKey, bool isAssociated);


        /**
         * getAssociatedStationModeKeys
         *
         * Returns the StationMode keys that are triggered by this MasterMode
         *
         * @return The StationModes triggered by this MasterMode, as a vector of keys
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAssociatedStationModeKeys();
        
        
        /**
         * invalidate
         *
         * Make the data contained by this ECSMasterMode as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateECSMasterModeChanges
         *
         * This updates the map recording the changes to the ECSMasterMode. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateECSMasterModeChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);

        /**
         * haveStationModesChanged
         *
         * We have to have some way of maintaining whether changes have been made to the list of associated
         * StationModes or not. It is too difficult to try and manage StationMode changes in strings in 
         * m_ECSMasterModeChanges.  Instead we use the m_originalStationModes and m_newStationModees. This 
         * methods adds and removes new and old StationModes from m_newStationModes and compares the sets 
         * to determine if changes have been made.
         *
         * @param unsigned long - This is the key of the StationMode to add or remove from the associations
         * @param bool - This indicates whether this profile is to be added or removed. True if the
         *               StationMode is to be added to the associations, false if the StationMode is to be
         *               removed.
         */
        bool haveStationModesChanged(unsigned long stationModeKey, bool isAssociated);
 
		
        // Assignment ECSMasterMode not used so it is made private
		ConfigECSMasterMode& operator=(const ConfigECSMasterMode&);


        // We cannot use keys for identification of operators in the
        // Configuration Editor because new operators do not have keys.
        // Therefore we use this method for identification.
        unsigned long m_uniqueIdentifier; 

        // The next unique identifier available
        static long s_nextAvailableIdentifier; 

        ECSMasterModeHelper* m_ECSMasterModeHelper;

        // This is a map of all changes made to the ECSMasterMode so that they are
        // recorded and can be sent in audit or online update messages.
        TA_Base_Core::ItemChanges m_ECSMasterModeChanges; 

        // This is a set of all the original StationModes
        // associated with this MasterMode. This is used to determine
        // whether changes have been made and allows us to report
        // those changes.
        std::set<unsigned long> m_originalStationModes; 

        // This is a set of all the StationMode changes. This allows us
        // to compare the sets to see if any changes have been made.
        std::set<unsigned long> m_newStationModes;      

    };
} // closes TA_Core

#endif // !defined(ConfigECSMasterMode_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
