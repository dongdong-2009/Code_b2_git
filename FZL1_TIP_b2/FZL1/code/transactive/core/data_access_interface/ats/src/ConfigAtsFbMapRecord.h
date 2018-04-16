/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/ConfigAtsFbMapRecord.h $
  * @author Nick Jardine
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2013/04/01 16:13:40 $
  * Last modified by: $Author: raghu.babu $
  * 
  * ConfigAtsFbMapRecord is a concrete implementation of IConfigAtsFbMapRecord, which is in turn an implementation
  * of IAtsFbMapRecord. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsFbMapRecords.
  */


#if !defined(ConfigAtsFbMapRecord_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigAtsFbMapRecord_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/ats/src/IConfigAtsFbMapRecord.h"

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class AtsFbMapRecordHelper;

    class ConfigAtsFbMapRecord : public IConfigAtsFbMapRecord
    {

    public:
        /**
         * ConfigAtsFbMapRecord (constructor)
         *
         * Constructs a new instance of the ConfigAtsFbMapRecord with no id. This is used when
         * creating a *new* AtsFbMapRecord - that is, one that does not yet exist in the database.
         */
        ConfigAtsFbMapRecord();


        /**
         * ConfigAtsFbMapRecord (constructor)
         *
         * Constructs a new instance of ConfigAtsFbMapRecord with the specified id. This is used 
         * when creating a ConfigAtsFbMapRecord based around an existing AtsFbMapRecord in the database.
         *
         * @param key The key to this AtsFbMapRecord in the database.
         */
        ConfigAtsFbMapRecord(const unsigned long idKey);


        /**
         * ConfigAtsFbMapRecord (copy constructor)
         *
         * @param ConfigAtsFbMapRecord& - The AtsFbMapRecord to make a copy of.
         */
        ConfigAtsFbMapRecord( const ConfigAtsFbMapRecord& theConfigAtsFbMapRecord);

        /**
         * ConfigAtsFbMapRecord (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see ConfigAtsFbMapRecordHelper::ConfigAtsFbMapRecordHelper(IData*)
         *
         */
        ConfigAtsFbMapRecord(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigAtsFbMapRecord();


    public:

        /**
         * deleteThisObject
         *
         * Removes this AtsFbMapRecord from the database. 
         * The calling application MUST then delete this AtsFbMapRecord object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this AtsFbMapRecord was initially loaded from the database
         *      OR     - writeAtsFbMapRecordData() has already been called
         *      This AtsFbMapRecord has not been deleted
         */
        void deleteThisObject(bool cascade=false);
		virtual std::string getBranchName ();
		virtual unsigned long getBranchId();
		virtual std::string getBranchIdAsString();
        virtual std::string getTrack();
		virtual unsigned long getMinAbscissa();
		virtual unsigned long getMaxAbscissa();
		virtual double getZoneId();
		virtual std::string getZoneTypeName();
        
        virtual void setBranchIdAsString(const std::string& id);
        virtual void setMinAbscissa(unsigned long minAbscissa);
		virtual void setMaxAbscissa(unsigned long maxAbscissa);
		virtual void setZoneId(double zoneId);
		virtual void setZoneTypeName(std::string zoneTypeName);

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
         * @exception DataConfigurationException If the data contained in the AtsFbMapRecord object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This AtsFbMapRecord has not been deleted
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
            return m_AtsFbMapRecordChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the AtsFbMapRecord has been changed by the user.
         *
         * @return bool - This will return true if the AtsFbMapRecord does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the AtsFbMapRecord changes is empty then nothing has changed
            return !m_AtsFbMapRecordChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this AtsFbMapRecord is new. A new AtsFbMapRecord is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the AtsFbMapRecord has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * setName
         *
         * No applicable for AtsFbMapRecord - will just assert if called.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * invalidate
         *
         * Make the data contained by this AtsFbMapRecord as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateAtsFbMapRecordChanges
         *
         * This updates the map recording the changes to the AtsFbMapRecord. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateAtsFbMapRecordChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

		
        // Assignment AtsFbMapRecord not used so it is made private
		ConfigAtsFbMapRecord& operator=(const ConfigAtsFbMapRecord&);


        AtsFbMapRecordHelper* m_atsFbMapRecordHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        TA_Base_Core::ItemChanges m_AtsFbMapRecordChanges; // This is a map of all changes made to the AtsFbMapRecord so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_Core

#endif // !defined(ConfigAtsFbMapRecord_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
