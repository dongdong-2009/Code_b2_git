/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/ConfigTIRatisTag.h $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigTiRatisTag is a concrete implementation of IConfigTiRatisTag, which is in turn an implementation
  * of ITiRatisTag. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TiRatisTags.
  */


#ifndef TI_RATIS_TAG_CONFIG_H_INCLUDED
#define TI_RATIS_TAG_CONFIG_H_INCLUDED


#include <string>
#include <set>

#include "core/data_access_interface/stis_manager/src/IConfigTiRatisTag.h"


namespace TA_Base_Core
{
    class IData;
}



namespace TA_IRS_Core
{
    class TiRatisTagHelper;

    class ConfigTiRatisTag : public IConfigTiRatisTag
    {

    public:
        /**
         * ConfigTiRatisTag (constructor)
         *
         * Constructs a new instance of the ConfigTiRatisTag with no id. This is used when
         * creating a *new* TiRatisTag - that is, one that does not yet exist in the database.
         */
        ConfigTiRatisTag();

        /**
         * ConfigTiRatisTag (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         * @see TiRatisTagHelper::TiRatisTagHelper(IData*)
         *
         */
        ConfigTiRatisTag(unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigTiRatisTag (constructor)
         *
         * Constructs a new instance of ConfigTiRatisTag with the specified id. This is used 
         * when creating a ConfigTiRatisTag based around an existing TiRatisTag in the database.
         *
         * @param key The key to this TiRatisTag in the database.
         */
        ConfigTiRatisTag(const unsigned long idKey);


        /**
         * ConfigTiRatisTag (copy constructor)
         *
         * @param ConfigTiRatisTag& - The TiRatisTag to make a copy of.
         */
        ConfigTiRatisTag( const ConfigTiRatisTag& theConfigTiRatisTag);

        
        virtual ~ConfigTiRatisTag();


    public:

        /**
         * deleteThisObject
         *
         * Removes this TiRatisTag from the database. 
         * The calling application MUST then delete this TiRatisTag object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this TiRatisTag was initially loaded from the database
         *      OR     - writeTiRatisTagData() has already been called
         *      This TiRatisTag has not been deleted
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
         * @exception DataConfigurationException If the data contained in the TiRatisTag object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This TiRatisTag has not been deleted
         */
        void applyChanges();
        

        /**
         * getKey
         *
         * Returns the key for this TiRatisTag.
         *
         * @return The key for this TiRatisTag as an unsigned long.
         *
         * pre: Either - this TiRatisTag was initially loaded from the database
         *      OR     - writeTiRatisTagData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        
        virtual unsigned long getKey();

		virtual std::string	getTagGroup();

		virtual std::string	getDetail();

		virtual std::string	getRatisTag();

		virtual void setTagGroup(const std::string & tagGroup);

		virtual void setDetail (const std::string& detail);


         /**
         * getName
         *
         * Returns the name of this item for the IItem interface.  For the PA Station this is 
         * just the key in a string format e.g. "PA Station 1".
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
         * Not applicable for PA DVA Messages.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to this zone. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all display changes
         */
        virtual TA_Base_Core::ItemChanges getAllItemChanges()
        {
            return m_tiRatisTagChanges;
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
        };

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the TiRatisTag has been changed by the user.
         *
         * @return bool - This will return true if the TiRatisTag does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the TiRatisTag changes is empty then nothing has changed
            return !m_tiRatisTagChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this TiRatisTag is new. A new TiRatisTag is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the TiRatisTag has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * getModifiedKey (interface implementation)
         *
         * @see IConfigPaTrainDvaMessage::getModifiedKey for more details
         */
        virtual unsigned long getModifiedKey();

        /**
         * invalidate
         *
         * Make the data contained by this TiRatisTag as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        void invalidate();


        private:
            
	    /**
         * updateTiRatisTagChanges
         *
         * This updates the map recording the changes to the TiRatisTag. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateTiRatisTagChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updateTiRatisTagChanges
         *
         * This updates the map recording the changes to the TiRatisTag. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updateTiRatisTagChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);
		
        // Assignment TiRatisTag not used so it is made private
		ConfigTiRatisTag& operator=(const ConfigTiRatisTag&);


        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of TiRatisTags in the
                                          // Configuration Editor because new TiRatisTags do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available


        TiRatisTagHelper* m_tiRatisTagHelper;


        TA_Base_Core::ItemChanges m_tiRatisTagChanges; // This is a map of all changes made to the TiRatisTag so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_IRS_Core

#endif // !defined(ConfigTiRatisTag_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
