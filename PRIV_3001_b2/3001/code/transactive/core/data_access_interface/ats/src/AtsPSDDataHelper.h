/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsPSDDataHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * AtsPSDDataHelper is an object that is held by AtsPSDData and ConfigAtsPSDData objects. 
  * It contains all data used by AtsPSDDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPSDData and ConfigAtsPSDData.
  */

#if !defined(AtsPSDDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsPSDDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined (_MSC_VER)

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
    class AtsPSDDataHelper
    {

    public:


        /** 
         * AtsPSDDataHelper
         *
         * This constructor creates a new AtsPSDDataHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsPSDDataHelper(const unsigned long idKey);
         
        
        /** 
         * AtsPSDDataHelper
         *
         * Copy constructor.
         *
         * @param   theAtsPSDDataHelper the original AtsPSDDataHelper to copy.
         *
         */
        AtsPSDDataHelper( const AtsPSDDataHelper& theAtsPSDDataHelper);

        /** 
         * AtsPSDDataHelper
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
        AtsPSDDataHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsPSDDataHelper
         *
         * This constructor creates a new AtsPSDDataHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsPSDDataId() before writing 
         * to the database for the first time.
         *
         */
        AtsPSDDataHelper();

        
        /** 
         * ~AtsPSDDataHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsPSDDataHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsPSDData.  A new AtsPSDData is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsPSDData
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getPSDDataPointEntityKey
         *
         * Returns the PSD DataPoint Entity Key of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return PSD DataPoint Entity Key
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getPSDDataPointEntityKey();

		/**
         * getPSDDataPointType
         *
         * Returns the PSD DataPoint Type of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return PSD DataPoint Type
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getPSDDataPointType();

		/**
         * getPSDDataPointNameOfType
         *
         * Returns the PSD DataPoint Name of Type of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return PSD DataPoint Name of Type
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getPSDDataPointNameOfType();

		/**
         * getPSDNumber
         *
         * Returns the PSD Number of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return PSD Number
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getPSDNumber();

		/**
         * getPlatformId
         *
         * Returns the Platform ID of this Platform.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
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
        unsigned long getPlatformId();

		/**
         * getPlatformKey
         *
         * Returns the Platform Key of this Platform.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
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
        unsigned long getPlatformKey();

		/**
         * getPlatformName
         *
         * Returns the Platform Name of the Platform.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setPlatform()
         *
         * @return the Platform Name
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
		std::string getPlatformName();

        /**
         * getTisPlatformId
         *
         * Returns the TIS Platform ID of this Platform.  If this AtsPlatform record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPlatform has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTisPlatform()
         *
         * @return the Platform ID
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
        unsigned char getTisPlatformId();

		/**
         * getStationId
         *
         * Returns the Station Id of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return the Station Id
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getStationId();

		/**
         * getMSSZone
         *
         * Returns the MSS Zone of this AtsPSDData record.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return MSS Zone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getMSSZone();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsPSDData record as an unsigned long.  
		 * If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();

		/**
         * getName
         *
         * Returns the name of this Platform.  If this AtsPSDData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPSDData has been requested, all the data is loaded from the database.
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
         * Make the data contained by this AtsPSDData as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsPSDDataData() has been called prior to calling
         * this method on a new AtsPSDData, as it does not make any sense to invalidate an
         * AtsPSDData that has not yet been written to the database.
         *
         * pre:    This AtsPSDData was initially loaded from the database
         *         or writeAtsPSDDataData() has already been called
         */
        void invalidate();
        
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
		 *  Sets the Key of the platform where this PSD is found
		 *
		 *  @param		platform Key
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
         * setName
         *
         * Sets the name of this AtsPSDData. The set values are not written to the database
         * until writeAtsPSDDataData() is called.  This method should only be used by the 
         * ConfigAtsPSDData class.
         *
         * @param  track the Platform description for this AtsPSDData
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPSDDataId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setName(const std::string& name);


        /**
         * writeAtsPSDDataData
         * 
         * Write this AtsPSDData to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsPSDDataId should be an unsigned long)
         *          - NO_VALUE if the AtsPSDData record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsPSDData object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsPSDDataData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsPSDData
         *
         * Remove this AtsPSDData from the database. Once this method has been executed, the
         * AtsPSDDataHelper object should be destroyed.
         *
         * pre:    This AtsPSDData was initially loaded from the database
         *         or writeAtsPSDDataData() has already been called
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
        void deleteAtsPSDData(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsPSDDataHelper& operator=(const AtsPSDDataHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsPSDData was initially loaded from the database
         *         or writeAtsPSDDataData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPSDDataId should be an unsigned long)
         *         - NO_VALUE if the AtsPSDData record cannot be found for the helper 
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
         * modifyExistingAtsPSDData
         *
         * This will update an existing AtsPSDData in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPSDData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsPSDData() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsPSDData
         *
         * This will add a new AtsPSDData in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPSDData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsPSDData() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        int m_idKey;
        int m_modifiedIdKey;
		int m_psdDataPointEntityKey;
		std::string m_psdDataPointType;
		std::string m_psdDataPointNameOfType;
		int m_psdNumber;
		int m_platformId;
		int m_platformKey;
		std::string m_platformName;
        unsigned char m_tisPlatformId;
		int m_stationId;
		std::string m_mssZone;
		std::string m_name;
        bool m_isValidData;
        bool m_isNew; 
    };
} // closes TA_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // defined (_MSC_VER)

#endif // !defined(AtsPSDDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
