/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsTWPDataHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  *
  * AtsTWPDataHelper is an object that is held by AtsTWPData and ConfigAtsTWPData objects. 
  * It contains all data used by AtsTWPDatas, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsTWPData and ConfigAtsTWPData.
  */

#if !defined(AtsTWPDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsTWPDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsTWPDataHelper
    {

    public:


        /** 
         * AtsTWPDataHelper
         *
         * This constructor creates a new AtsTWPDataHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsTWPDataHelper(const unsigned long idKey);
         
        
        /** 
         * AtsTWPDataHelper
         *
         * Copy constructor.
         *
         * @param   theAtsTWPDataHelper the original AtsTWPDataHelper to copy.
         *
         */
        AtsTWPDataHelper( const AtsTWPDataHelper& theAtsTWPDataHelper);

        /** 
         * AtsTWPDataHelper
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
        AtsTWPDataHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsTWPDataHelper
         *
         * This constructor creates a new AtsTWPDataHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsTWPDataId() before writing 
         * to the database for the first time.
         *
         */
        AtsTWPDataHelper();

        
        /** 
         * ~AtsTWPDataHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        ~AtsTWPDataHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsTWPData.  A new AtsTWPData is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsTWPData
         */
        bool isNew() const
        {
            return m_isNew;
        };

		/**
		 *	getTWPDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given ATS TWP
		 *
		 *  @return		entity key of the ISCS data point
		 *
		 */
		unsigned long getTWPDataPointEntityKey();

		/**
		 *	getTWPDataPointType()
		 *
		 *  Returns the type of the TWP datapoint. Either "Input" or "Output"
		 *
		 *  @return		data point type as string
		 *
		 */
		std::string getTWPDataPointType();

		/**
		 *	getTWPCommand()
		 *
		 *  Returns the TWP command corresponding to the data point entity
		 *
		 *  @return		TWP command as string
		 *
		 */
		std::string getTWPCommand();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsTWPData record as an unsigned long.  
		 * If this AtsTWPData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsTWPData has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsTWPData record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();

		/**
         * getName
         *
         * Returns the name of this Platform.  If this AtsTWPData record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsTWPData has been requested, all the data is loaded from the database.
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
         * Make the data contained by this AtsTWPData as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsTWPDataData() has been called prior to calling
         * this method on a new AtsTWPData, as it does not make any sense to invalidate an
         * AtsTWPData that has not yet been written to the database.
         *
         * pre:    This AtsTWPData was initially loaded from the database
         *         or writeAtsTWPDataData() has already been called
         */
        void invalidate();
        
		/**
		 *	setTWPDataPointEntityKey()
		 *
		 *  Returns the ISCS data point entity key which corresponds to given ATS TWP
		 *
		 *  @param		entity key of the ISCS data point
		 *
		 */
		void setTWPDataPointEntityKey(unsigned long key);

		/**
		 *	setTWPDataPointType()
		 *
		 *  Returns the type of the TWP datapoint. Either "Input" or "Output"
		 *
		 *  @param		data point type as string
		 *
		 */
		void setTWPDataPointType(std::string type);

		/**
		 *	setTWPCommand()
		 *
		 *  Returns the TWP command corresponding to the data point entity
		 *
		 *  @param		TWP command as string
		 *
		 */
		void setTWPCommand(std::string command);

		/**
         * setName
         *
         * Sets the name of this AtsTWPData. The set values are not written to the database
         * until writeAtsTWPDataData() is called.  This method should only be used by the 
         * ConfigAtsTWPData class.
         *
         * @param  track the Platform description for this AtsTWPData
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsTWPDataId should be an unsigned long)
         *         - NO_VALUE if the AtsTWPData record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setName(const std::string& name);


        /**
         * writeAtsTWPDataData
         * 
         * Write this AtsTWPData to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsTWPDataId should be an unsigned long)
         *          - NO_VALUE if the AtsTWPData record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsTWPData object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsTWPDataData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsTWPData
         *
         * Remove this AtsTWPData from the database. Once this method has been executed, the
         * AtsTWPDataHelper object should be destroyed.
         *
         * pre:    This AtsTWPData was initially loaded from the database
         *         or writeAtsTWPDataData() has already been called
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
        void deleteAtsTWPData(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsTWPDataHelper& operator=(const AtsTWPDataHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsTWPData was initially loaded from the database
         *         or writeAtsTWPDataData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsTWPDataId should be an unsigned long)
         *         - NO_VALUE if the AtsTWPData record cannot be found for the helper 
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
        void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        
        /**
         * modifyExistingAtsTWPData
         *
         * This will update an existing AtsTWPData in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsTWPData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsTWPData() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsTWPData
         *
         * This will add a new AtsTWPData in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsTWPData object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsTWPData() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        int m_idKey;
		int m_twpDataPointEntityKey;
		std::string	m_twpDataPointType;
		std::string	m_twpCommand;
		bool m_isValidData;
        bool m_isNew;
		
    };
} // closes TA_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // defined (_MSC_VER)

#endif // !defined(AtsTWPDataHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
