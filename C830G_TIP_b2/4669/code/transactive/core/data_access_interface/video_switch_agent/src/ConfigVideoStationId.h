/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/ConfigVideoStationId.h $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigVideoStationId is a concrete implementation of IConfigVideoStationId, which is in turn an implementation
  * of IVideoStationId. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for VideoStationIds.
  */


#if !defined(ConfigVideoStationId_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigVideoStationId_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/video_switch_agent/src/IConfigVideoStationId.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_IRS_Core
{
    class VideoStationIdHelper;

    class ConfigVideoStationId : public IConfigVideoStationId
    {

    public:
        /**
         * ConfigVideoStationId (constructor)
         *
         * Constructs a new instance of the ConfigVideoStationId with no id. This is used when
         * creating a *new* VideoStationId - that is, one that does not yet exist in the database.
         */
        ConfigVideoStationId();


        /**
         * ConfigVideoStationId (constructor)
         *
         * Constructs a new instance of ConfigVideoStationId with the specified pkey. This is used 
         * when creating a ConfigVideoStationId based around an existing VideoStationId in the database.
         *
         * @param key The key to this VideoStationId in the database.
         */
        ConfigVideoStationId(const unsigned long pKey);


        /**
         * ConfigVideoStationId (copy constructor)
         *
         * @param ConfigVideoStationId& - The VideoStationId to make a copy of.
         */
        ConfigVideoStationId( const ConfigVideoStationId& theConfigVideoStationId);

        /**
         * ConfigVideoStationId (constructor)
         *
         * Constructs a new instance using the information provided in TA_Base_Core::IData interface
         *
         * @see ConfigVideoStationIdHelper::ConfigVideoStationIdHelper(TA_Base_Core::IData*)
         *
         */
        ConfigVideoStationId(unsigned long row, TA_Base_Core::IData& data);
        
        virtual ~ConfigVideoStationId();


    public:

        /**
         * deleteThisObject
         *
         * Removes this VideoStationId from the database. 
         * The calling application MUST then delete this VideoStationId object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this VideoStationId was initially loaded from the database
         *      OR     - writeVideoStationIdData() has already been called
         *      This VideoStationId has not been deleted
         */
        void deleteThisObject(bool cascade=false);

        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


		virtual unsigned long getStationId();
        virtual unsigned long getTaLocation();
        
        virtual void setStationId(unsigned long stationId);
        virtual void setTaLocation(unsigned long taLocation);

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception TA_Base_Core::DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the VideoStationId object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This VideoStationId has not been deleted
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
            return m_VideoStationIdChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the VideoStationId has been changed by the user.
         *
         * @return bool - This will return true if the VideoStationId does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the VideoStationId changes is empty then nothing has changed
            return !m_VideoStationIdChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this VideoStationId is new. A new VideoStationId is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the VideoStationId has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * invalidate
         *
         * Make the data contained by this VideoStationId as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        virtual void invalidate();


        /**
         * getDateCreated
         *
         * Returns the date created for this VideoStationId
         *
         * @return The date created for this VideoStationId as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this VideoStationId.
         *
         * @return The date modified for this VideoStationId as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified();

        // Need to satisfy IItem for the config editor.

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception TA_Base_Core::DataException A TA_Base_Core::DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();


        /**
         * setName
         *
         * Not applicable for Video Station Id.  Will just assert.
         *
         * @param name The name to give this item.
         *
         */
        virtual void setName(const std::string& name);


        private:
            
	    /**
         * updateVideoStationIdChanges
         *
         * This updates the map recording the changes to the VideoStationId. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateVideoStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 

		
        // Assignment VideoStationId not used so it is made private
		ConfigVideoStationId& operator=(const ConfigVideoStationId&);


        VideoStationIdHelper* m_VideoStationIdHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

		TA_Base_Core::ItemChanges m_VideoStationIdChanges; // This is a map of all changes made to the VideoStationId so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
} // closes TA_IRS_Core

#endif // !defined(ConfigVideoStationId_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
