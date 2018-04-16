/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsPowerData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * ConfigAtsPowerData is an implementation of IConfigAtsPowerData. 
  */

#if !defined(AFX_CONFIGATSPOWERDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_)
#define AFX_CONFIGATSPOWERDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_

#include "core/data_access_interface/ats/src/AtsPowerDataHelper.h"
#include "core/data_access_interface/ats/src/IConfigAtsPowerData.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
	class ConfigAtsPowerData : public IConfigAtsPowerData
	{

	public:	
	
		/**
         * ConfigAtsPowerData (constructor)
         *
         * Constructs a new instance of the ConfigAtsPowerData with no id. This is used when
         * creating a *new* ConfigAtsPowerData - that is, one that does not yet exist in the database.
         */
        ConfigAtsPowerData();


        /**
         * ConfigAtsPowerData (constructor)
         *
         * Constructs a new instance of ConfigAtsPowerData with the specified id. This is used 
         * when creating a ConfigAtsPowerData based around an existing AtsPowerData in the database.
         *
         * @param key The key to this AtsPowerData in the database.
         */
        ConfigAtsPowerData(const unsigned long idKey);


        /**
         * ConfigAtsPowerData (copy constructor)
         *
         * @param ConfigAtsPowerData& - The ConfigAtsPowerData to make a copy of.
         */
        ConfigAtsPowerData( const ConfigAtsPowerData& theConfigAtsPowerData);

        /**
         * ConfigAtsPowerData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        ConfigAtsPowerData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~ConfigAtsPowerData();

		/**
		 * Constructor
		 *
		 * Constructs a ConfigAtsPowerData object based on the information provided in the input 
		 * parameters as read from the database
		 *
		 * @input	aKey						Primary key of the database record to which this information 
												belongs
		 * @input	aElectricalSubsectionId		Id of the ATS Electrical subsection
		 * @input	aTractionPowerDataPointEntityKey	
												ISCS entity key which corresponds to the 
												ATS Electrical subsection
		 * @input	aLocationKey				Key of the location (station) of the ATS Electrical subsection
		 * @input	aMSSZone					Name of the MSS Zone in which this ATS Electrical
												subsection is found
		 * 
		 */
		ConfigAtsPowerData(unsigned long aKey, 
						unsigned long aElectricalSubsectionId, 
						unsigned long aTractionPowerDataPointEntityKey,
						unsigned long aLocationKey,
						const std::string &aMSSZone);


		/**
		 *	getTractionPowerDataPointEntityKey()
		 *
		 *  Returns the ISCS entity key of the data point which corresponds directly to
		 *  the ATS Electrical subsection. Based on this key, status of the Electrical
		 *  subsection can be retrieved
		 *
		 */
		unsigned long getTractionPowerDataPointEntityKey();

		/**
		 *	getTractionPowerBitPosition()
		 *
		 *  Returns the bit position of the data point which corresponds directly to
		 *  the ATS Electrical subsection. Based on this key, status of the Electrical
		 *  subsection can be retrieved
		 *
		 */
		unsigned long getTractionPowerBitPosition();
		
		/**
		 *	getElectricalSubSectionId()
		 *
		 *  Returns the Id of the ATS Electrical Sub Section
		 *
		 */
		unsigned long getElectricalSubSectionId();

		/**
		 *	getLocationId()
		 *
		 *  Returns the Id of the location at which this ATS Electrical Sub Section can be found
		 *
		 */
		unsigned long getLocationId();

		/**
		 *	getMSSZone()
		 *
		 *  Returns the name of the MSS Zone within which this ATS Electrical Sub Section is found
		 *
		 */
		std::string getMSSZone();

		/**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();


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
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

		/**
		 *	setElectricalSubSectionId()
		 *
		 *  Returns the id of the Electrical SubSection within the main line
		 *
		 *  @param		subsection id
		 *
		 */
		virtual void setElectricalSubSectionId(unsigned long id);		

		/**
		 *	setLocationId()
		 *
		 *  Returns the location key for this record
		 *
		 *  @param		location id
		 *
		 */
		virtual void setLocationId(unsigned long id);

		/**
		 *	setTractionPowerDataPointEntityKey()
		 *
		 *  Returns the entity key of the ISCS datapoint which corresponds to the electrical 
		 *  subsection within the main line
		 *
		 *  @param		entity key
		 *
		 */
		virtual void setTractionPowerDataPointEntityKey(unsigned long key);

		/**
         * deleteThisObject
         *
         * Remove this AtsPowerData from the database. Once this method has been executed, the
         * AtsPowerDataHelper object should be destroyed.
         *
         * @exception DatabaseException 
         *            - if there is a problem writing the data to the database.
         *            - if references to this record exists in AT_FIXED_BLOCK_MAP table.  
		 *				These can be removed by specifying the cascade option.
         *
         * @param cascade
         *        - force the removal of this record by removing all references to this record
         *          in other tables (AT_FIXED_BLOCK_MAP).  This should be set to true to prevent the 
         *          DatabaseException from being thrown.
         *
         * pre: - this AtsPowerData was initially loaded from the database
         *      - writeAtsPowerDataData() has already been called
         */
        virtual void deleteThisObject(bool cascade=false);

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
            return m_atsPowerDataChanges;
        }
       
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the AtsBranchTrack has been changed by the user.
         *
         * @return bool - This will return true if the AtsBranchTrack does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the AtsBranchTrack changes is empty then nothing has changed
            return !m_atsPowerDataChanges.empty();
        };

        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

		/**
         * updateAtsPowerDataChanges
         *
         * This updates the map recording the changes to the AtsPowerData. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
		void updateAtsPowerDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
    

	private:

        // Assignment ConfigAtsPowerData not used so it is made private
		ConfigAtsPowerData& operator=(const ConfigAtsPowerData &);

	protected:

		unsigned long	m_uniqueIdentifier;			// We cannot use keys for identification of guis in the
													// Configuration Editor because new guis do not have keys.
													// Therefore we use this method for identification.

        static long		s_nextAvailableIdentifier;	// The next unique identifier available

		TA_Base_Core::ItemChanges		m_atsPowerDataChanges;		// This is a map of all changes made to the PSD Data so that they are
													// recorded and can be sent in audit or online update messages.


        TA_IRS_Core::AtsPowerDataHelper * m_atsPowerDataHelper;


	};

} // TA_Core

#endif // !defined(AFX_ATSPOWERDATA_H__24959B4D_99B2_4C66_8349_6776BBBAF172__INCLUDED_)
