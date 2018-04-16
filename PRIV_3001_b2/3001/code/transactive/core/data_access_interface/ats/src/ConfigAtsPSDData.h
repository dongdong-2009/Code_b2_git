/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/ConfigAtsPSDData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * ConfigAtsPSDData is an implementation of IConfigAtsPSDData. 
  */

#if !defined(AFX_CONFIGATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_)
#define AFX_CONFIGATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_

#include "core/data_access_interface/ats/src/AtsPSDDataHelper.h"
#include "core/data_access_interface/ats/src/IConfigAtsPSDData.h"

namespace TA_IRS_Core
{
	class ConfigAtsPSDData : public IConfigAtsPSDData
	{

	public:

		/**
         * ConfigAtsPSDData (constructor)
         *
         * Constructs a new instance of the ConfigAtsPSDData with no id. This is used when
         * creating a *new* ConfigAtsPSDData - that is, one that does not yet exist in the database.
         */
        ConfigAtsPSDData();


        /**
         * ConfigAtsPSDData (constructor)
         *
         * Constructs a new instance of ConfigAtsPSDData with the specified id. This is used 
         * when creating a ConfigAtsPSDData based around an existing AtsPSDData in the database.
         *
         * @param key The key to this AtsPSDData in the database.
         */
        ConfigAtsPSDData(const unsigned long idKey);


        /**
         * ConfigAtsPSDData (copy constructor)
         *
         * @param ConfigAtsPSDData& - The ConfigAtsPSDData to make a copy of.
         */
        ConfigAtsPSDData( const ConfigAtsPSDData& theConfigAtsPSDData);

        /**
         * ConfigAtsPSDData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        ConfigAtsPSDData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~ConfigAtsPSDData();

		/**
		 *	getPSDDataPointEntityKey()
		 *
		 *	Returns the data point entity key as unsigned long
		 *
		 */
		unsigned long getPSDDataPointEntityKey();
		
		/**
		 *	getPSDDataPointType()
		 *
		 *	Returns the data point type as string
		 *
		 */
		std::string getPSDDataPointType();
		
		/**
		 *	getPSDDataPointNameOfType()
		 *
		 *	Returns the name of the data point type as string
		 *
		 */
		std::string getPSDDataPointNameOfType();
		
		/**
		 *	getStationId()
		 *
		 *	Returns the id of the station as unsigned long
		 *
		 */
		unsigned long getStationId();
		
		/**
		 *	getPlatformId()
		 *
		 *	Returns the id of the platform as unsigned long
		 *
		 */
		unsigned long getPlatformId();
		
		/**
		 *	getPlatformName()
		 *
		 *	Returns the name of the platform as string
		 *
		 */
		std::string getPlatformName();

        /** 
          * getTisPlatformId
          *
          * gets the platform ID of this platform for sending
          * platform information to the STIS server.
          *
          * @return a single digit number (1-9)
          */
        virtual unsigned char getTisPlatformId();
		
		/**
		 *	getPSDNumber()
		 *
		 *	Returns the PSD door number as integer
		 *
		 */
		int getPSDNumber();

		/**
		 *	getMSSZone()
		 *
		 *	Returns the name of the MSS Zone as string
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
		 *	setPSDDataPointEntityKey()
		 *
		 *  Sets the ISCS data point entity key which corresponds to given Ats PSD door number
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		virtual void setPSDDataPointEntityKey(unsigned long key);

		/**
		 *	setPSDDataPointType()
		 *
		 *  Sets the ISCS data point's type.
		 *
		 *  @param		Type as string. One of "Input" / "Output"
		 *
		 */
		virtual void setPSDDataPointType(const std::string& type);

		/**
		 *	getPSDDataPointNameOfType()
		 *
		 *  Sets the ISCS data point type's name.
		 *
		 *  @param		Name as string. One of "Door Status" / "Door Inhibit" / "Pre-Close Warning"
		 *
		 */
		virtual void setPSDDataPointNameOfType(const std::string& nameOfType);
		
		/**
		 *	setPlatformKey()
		 *
		 *  Sets the Id of the platform where this PSD is found
		 *
		 *  @param		platform Id
		 *
		 */
		virtual void setPlatformKey(unsigned long key);
		
		/**
		 *	setPSDNumber()
		 *
		 *  Sets the number of the PSD door
		 *
		 *  @param		PSD Number
		 *
		 */
		virtual void setPSDNumber(int psdNumber);

        /**
         * deleteThisObject
         *
         * Remove this AtsPSDData from the database. Once this method has been executed, the
         * AtsPSDDataHelper object should be destroyed.
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
         * pre: - this AtsPSDData was initially loaded from the database
         *      - writeAtsPSDDataData() has already been called
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
            return m_atsPSDDataChanges;
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
            return !m_atsPSDDataChanges.empty();
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
         * updateAtsPSDDataChanges
         *
         * This updates the map recording the changes to the AtsPSDData. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
		void updateAtsPSDDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
    

	public:
		static const std::string OUTPUT_DOOR_INHIBIT_STATE;
		static const std::string OUTPUT_PRE_CLOSE_WARNING;
		static const std::string INPUT_DOOR_STATUS;
		static const std::string INPUT_TYPE;
		static const std::string OUTPUT_TYPE;

	protected:

		unsigned long	m_uniqueIdentifier;			// We cannot use keys for identification of guis in the
													// Configuration Editor because new guis do not have keys.
													// Therefore we use this method for identification.

        static long		s_nextAvailableIdentifier;	// The next unique identifier available

		TA_Base_Core::ItemChanges		m_atsPSDDataChanges;		// This is a map of all changes made to the PSD Data so that they are
													// recorded and can be sent in audit or online update messages.


        TA_IRS_Core::AtsPSDDataHelper * m_atsPSDDataHelper;

    private:

        // Assignment ConfigAtsPSDData not used so it is made private
		ConfigAtsPSDData& operator=(const ConfigAtsPSDData &);

	};

} // TA_Core

#endif // !defined(AFX_CONFIGATSPSDDATA_H__84DFFFA7_4BE2_4095_925E_55EA7DD97517__INCLUDED_)
