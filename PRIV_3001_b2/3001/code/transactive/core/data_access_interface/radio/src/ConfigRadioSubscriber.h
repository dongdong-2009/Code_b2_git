/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioSubscriber.h $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#if !defined(AFX_CONFIGRADIOSUBSCRIBER_H__B60909A8_9B7C_477D_A6E6_A22DE9D0EB8E__INCLUDED_)
#define AFX_CONFIGRADIOSUBSCRIBER_H__B60909A8_9B7C_477D_A6E6_A22DE9D0EB8E__INCLUDED_

#include <string>
#include "core/data_access_interface/radio/src/IConfigRadioSubscriber.h"
#include "core/data_access_interface/radio/src/RadioSubscriberHelper.h"

namespace TA_IRS_Core
{
    class ConfigRadioSubscriber : public IConfigRadioSubscriber
    {
    public:
		ConfigRadioSubscriber();
	    ConfigRadioSubscriber(const unsigned long key);
        ConfigRadioSubscriber(const unsigned long row, TA_Base_Core::IData& data);
        ConfigRadioSubscriber(const RadioResource&  resource);
		ConfigRadioSubscriber(const ConfigRadioSubscriber& theConfigRadioSubscriber);
	    virtual ~ConfigRadioSubscriber();


        /*** IRadioSubscriber methods ***/
	    /**
	      * getKey
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getKey();

	    /**
	      * getResourceType
	      * 
	      * @return char
	      */
	    virtual char getResourceType();

	    /**
	      * getResourceId
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getResourceId();

	    /**
	      * getSSI
	      * 
	      * @return unsigned long
	      */
	    virtual unsigned long getSSI();

	    /**
	      * getAlias
	      * 
	      * @return std::string
	      */
	    virtual std::string getAlias();


        virtual void invalidate();
        virtual void save();

		virtual void setResourceType(RadioResourceType type);

		virtual void setResourceId(unsigned long rid);

		virtual void setSSI(unsigned long ssi);

		virtual void setAlias(const std::string& alias);

		/**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception TA_Base_Core::DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the VideoTriggeringEvent object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This RadioSubscriber has not been deleted
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
            return m_RadioSubscriberChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the VideoTriggeringEvent has been changed by the user.
         *
         * @return bool - This will return true if the VideoTriggeringEvent does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the RadioSubscriber changes is empty then nothing has changed
            return !m_RadioSubscriberChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this VideoTriggeringEvent is new. A new VideoTriggeringEvent is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the VideoTriggeringEvent has not been applied to the database.
         */
        virtual bool isNew();

		virtual time_t getDateCreated();

		virtual time_t getDateModified();

		virtual void deleteThisObject(bool cascade=false);

		virtual void setName(const std::string& name);
		virtual std::string getName();

    private:
        // disabled methods
		ConfigRadioSubscriber& operator=(const ConfigRadioSubscriber&);

	    /**
         * updateRadioSubscriberChanges
         *
         * This updates the map recording the changes to the RadioSubscriber. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateRadioSubscriberChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
	    /**
         * updateRadioSubscriberChanges
         *
         * This updates the map recording the changes to the RadioSubscriber. Whenever a call to set is made in this
         * class then this method must be called to store the changes.  The input values are converted to
         * string representations before being added to the map
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const unsigned long - The old value that was stored for this attribute
         * @param const unsigned long - The new value to be stored for this attribute
         */
        virtual void updateRadioSubscriberChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue);

	private:

        RadioSubscriberHelper *m_radioSubscriberHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of guis in the
                                          // Configuration Editor because new guis do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

		TA_Base_Core::ItemChanges m_RadioSubscriberChanges; // This is a map of all changes made to the VideoTriggeringEvent so that they are
                                       // recorded and can be sent in audit or online update messages.

    };
}

#endif // !defined(AFX_CONFIGRADIOSUBSCRIBER_H__B60909A8_9B7C_477D_A6E6_A22DE9D0EB8E__INCLUDED_)

