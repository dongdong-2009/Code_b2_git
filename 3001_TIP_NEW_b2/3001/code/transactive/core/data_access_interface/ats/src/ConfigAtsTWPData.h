/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/ConfigAtsTWPData.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * ConfigAtsTWPData is an implementation of IConfigAtsTWPData. 
  */

#if !defined(AFX_CONFIGATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_)
#define AFX_CONFIGATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_

#include <string>
#include "core/data_access_interface/ats/src/IConfigAtsTWPData.h"
#include "core/data_access_interface/ats/src/AtsTWPDataHelper.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
	class ConfigAtsTWPData : public IConfigAtsTWPData
	{
	
	public:		

		/**
         * ConfigAtsTWPData (constructor)
         *
         * Constructs a new instance of the ConfigAtsTWPData with no id. This is used when
         * creating a *new* ConfigAtsTWPData - that is, one that does not yet exist in the database.
         */
        ConfigAtsTWPData();


        /**
         * ConfigAtsTWPData (constructor)
         *
         * Constructs a new instance of ConfigAtsTWPData with the specified id. This is used 
         * when creating a ConfigAtsTWPData based around an existing AtsTWPData in the database.
         *
         * @param key The key to this AtsTWPData in the database.
         */
        ConfigAtsTWPData(const unsigned long idKey);


        /**
         * ConfigAtsTWPData (copy constructor)
         *
         * @param ConfigAtsTWPData& - The ConfigAtsTWPData to make a copy of.
         */
        ConfigAtsTWPData( const ConfigAtsTWPData& theConfigAtsTWPData);

        /**
         * ConfigAtsTWPData (constructor)
         *
         * Constructs a new instance using the information provided in IData interface
         *
         */
        ConfigAtsTWPData(unsigned long row, TA_Base_Core::IData& data);

		virtual ~ConfigAtsTWPData();

		unsigned long getTWPDataPointEntityKey();
		std::string   getTWPDataPointType();
		std::string   getTWPCommand();

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
		 *	setTWPDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given ATS TWP
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		virtual void setTWPDataPointEntityKey(unsigned long key);

		/**
		 *	setTWPDataPointType()
		 *
		 *  Returns the type of the TWP datapoint. Either "Input" or "Output"
		 *
		 *  @param		data point type as string
		 *
		 */
		virtual void setTWPDataPointType(std::string type);

		/**
		 *	setTWPCommand()
		 *
		 *  Returns the TWP command corresponding to the data point entity
		 *
		 *  @param		TWP command as string
		 *
		 */
		virtual void setTWPCommand(std::string command);

		/**
         * deleteThisObject
         *
         * Remove this AtsTWPData from the database. Once this method has been executed, the
         * AtsTWPDataHelper object should be destroyed.
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
         * pre: - this AtsTWPData was initially loaded from the database
         *      - writeAtsTWPDataData() has already been called
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
            return m_atsTWPDataChanges;
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
            return !m_atsTWPDataChanges.empty();
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
         * updateAtsTWPDataChanges
         *
         * This updates the map recording the changes to the AtsTWPData. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
		void updateAtsTWPDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
	
	private:

        // Assignment ConfigAtsTWPData not used so it is made private
		ConfigAtsTWPData& operator=(const ConfigAtsTWPData &);

	protected:

		unsigned long	m_uniqueIdentifier;			// We cannot use keys for identification of guis in the
													// Configuration Editor because new guis do not have keys.
													// Therefore we use this method for identification.

        static long		s_nextAvailableIdentifier;	// The next unique identifier available

		TA_Base_Core::ItemChanges		m_atsTWPDataChanges;		// This is a map of all changes made to the PSD Data so that they are
													// recorded and can be sent in audit or online update messages.


        TA_IRS_Core::AtsTWPDataHelper * m_atsTWPDataHelper;

	};

} // TA_Core

#endif // !defined(AFX_ATSTWPDATA_H__7001673F_2D0B_4055_8049_44131E355A69__INCLUDED_)
