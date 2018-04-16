/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPowerDataHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * AtsPowerDataHelper is an object that is held by AtsPowerData and ConfigAtsPowerData objects. 
  * It contains all data used by AtsPowerDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPowerData and ConfigAtsPowerData.
  */

#if !defined(AtsPowerDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsPowerDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // _MSC_VER

#include "core/exceptions/src/TransactiveException.h"

#include <string>
#include <vector>
#include <climits>

namespace TA_Base_Core
{
	class IData;
}

namespace TA_IRS_Core
{
    class AtsPowerDataHelper
    {

    public:


        /** 
         * AtsPowerDataHelper
         *
         * This constructor creates a new AtsPowerDataHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsPowerDataHelper(const unsigned long idKey);
         
        
        /** 
         * AtsPowerDataHelper
         *
         * Copy constructor.
         *
         * @param   theAtsPowerDataHelper the original AtsPowerDataHelper to copy.
         *
         */
        AtsPowerDataHelper( const AtsPowerDataHelper& theAtsPowerDataHelper);

        /** 
         * AtsPowerDataHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        AtsPowerDataHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsPowerDataHelper
         *
         * This constructor creates a new AtsPowerDataHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsPowerDataId() before writing 
         * to the database for the first time.
         *
         */
        AtsPowerDataHelper();

        
        /** 
         * ~AtsPowerDataHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsPowerDataHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsPowerData.  A new AtsPowerData is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsPowerData
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getTractionPowerDataPointEntityKey
         *
         * Returns the PSD DataPoint Entity Key of this AtsPowerData record.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocationId()
         *
         * @return PSD DataPoint Entity Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. LocationId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getTractionPowerDataPointEntityKey();

		/**
         * getElectricalSubSectionId
         *
         * Returns the Subsection Number of this AtsPowerData record.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocationId()
         *
         * @return Subsection Number
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. LocationId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getElectricalSubSectionId();

		/**
         * getLocationId
         *
         * Returns the Station Id of this AtsPowerData record.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocationId()
         *
         * @return the Station Id
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. LocationId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getLocationId();

		/**
         * getBitPosition
         *
         * Returns the bitPosition of this AtsPowerData record.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setbitPosition()
         *
         * @return the bit position
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. BitPosition should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getBitPosition();

		/**
         * getMSSZone
         *
         * Returns the MSS Zone of this AtsPowerData record.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocationId()
         *
         * @return MSS Zone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. LocationId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getMSSZone();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsPowerData record as an unsigned long.  
		 * If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setLocationId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. LocationId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();

		/**
         * getName
         *
         * Returns the name of this Platform.  If this AtsPowerData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPowerData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setPlatform()
         *
         * @return the Platform 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Platform Id should be an unsigned long)
         *         - NO_VALUE if the Platform record cannot be found for the helper 
         *           constructed with an Platform Id.
         *         - NOT_UNIQUE if the Platform Id returns multiple records 
         */
		std::string getName();
        
		/**
         * invalidate
         *
         * Make the data contained by this AtsPowerData as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsPowerDataData() has been called prior to calling
         * this method on a new AtsPowerData, as it does not make any sense to invalidate an
         * AtsPowerData that has not yet been written to the database.
         *
         * pre:    This AtsPowerData was initially loaded from the database
         *         or writeAtsPowerDataData() has already been called
         */
        void invalidate();
        
		/**
		 *	setPowerDataPointEntityKey()
		 *
		 *  Sets the ISCS data point entity key which corresponds to given Ats Electrical subsection
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		virtual void setTractionPowerDataPointEntityKey(unsigned long key);

		/**
		 *	setLocationId()
		 *
		 *  Returns the location id for this record
		 *
		 *  @param		location id
		 *
		 */
		virtual void setLocationId(unsigned long id);

		/**
		 *	setBitPosition()
		 *
		 *  Returns none
		 *
		 *  @param		bit Position
		 *
		 */
		virtual void setBitPosition(int bitPosition);

		/**
		 *	setElectricalSubSectionId()
		 *
		 *  Sets the number of the subsection
		 *
		 *  @param		PSD Number
		 *
		 */
		virtual void setElectricalSubSectionId(int subsectionNumber);

		/**
         * setName
         *
         * Sets the name of this AtsPowerData. The set values are not written to the database
         * until writeAtsPowerDataData() is called.  This method should only be used by the 
         * ConfigAtsPowerData class.
         *
         * @param  track the Platform description for this AtsPowerData
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPowerDataId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setName(const std::string& name);


        /**
         * writeAtsPowerDataData
         * 
         * Write this AtsPowerData to the database.  Note that record uniqueness is enforced
         * for the primary key ATMSSZ_ID 
         *
         * e.g. Records 2,3 & 4 will clash:
         *
         * 1. ATMSSZ_ID    = 1
         *    STATION_ID   = 3
		 *	  MSS_ZONE	   = DBG
         * 2. ATMSSZ_ID    = 1                  <= clash with record 1
         *    STATION_ID   = 4
		 *	  MSS_ZONE	   = DBG
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. AtsPowerDataId should be an unsigned long)
         *          - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsPowerData object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsPowerDataData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsPowerData
         *
         * Remove this AtsPowerData from the database. Once this method has been executed, the
         * AtsPowerDataHelper object should be destroyed.
         *
         * pre:    This AtsPowerData was initially loaded from the database
         *         or writeAtsPowerDataData() has already been called
         *
         * @param  cascade set to true to force the removal of this record by removing all 
         *         references to this record in other tables (AT_FIXED_BLOCK_MAP).  This should 
         *         be set to true to prevent the DatabaseException from being thrown.
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           AT_FIXED_BLOCK_MAP.  These can be removed by specifying the cascade option.
         *
         */
        void deleteAtsPowerData(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsPowerDataHelper& operator=(const AtsPowerDataHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsPowerData was initially loaded from the database
         *         or writeAtsPowerDataData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPowerDataId should be an unsigned long)
         *         - NO_VALUE if the AtsPowerData record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void reload();

        /**
         * reloadUsing
         *
         * Fills out the local members with data contained in the input data object
         *
         * @param row the row to query in the data object
         *
         * @param data the IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *         
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        
        /**
         * modifyExistingAtsPowerData
         *
         * This will update an existing AtsPowerData in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPowerData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsPowerData() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsPowerData
         *
         * This will add a new AtsPowerData in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPowerData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsPowerData() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        int m_idKey;
		int m_powerDataPointEntityKey;
		int m_subsectionNumber;
		int m_locationId;
		int m_bitPosition;						//CL20376, add bit position, hongzhi
		std::string m_mssZone;
		bool m_isValidData;
        bool m_isNew; 
    };
} // closes TA_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AtsPowerDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
