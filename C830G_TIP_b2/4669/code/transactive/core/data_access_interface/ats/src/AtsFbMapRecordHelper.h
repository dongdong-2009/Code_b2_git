/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/AtsFbMapRecordHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * AtsFbMapRecordHelper is an object that is held by AtsFbMapRecord and ConfigAtsFbMapRecord objects. 
  * It contains all data used by AtsFbMapRecords, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsFbMapRecord and ConfigAtsFbMapRecord.
  */

#if !defined(AtsFbMapRecordHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsFbMapRecordHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsFbMapRecordHelper
    {

    public:


        /** 
         * AtsFbMapRecordHelper
         *
         * This constructor creates a new AtsFbMapRecordHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsFbMapRecordHelper(const unsigned long idKey);
         
        
        /** 
         * AtsFbMapRecordHelper
         *
         * Copy constructor.
         *
         * @param   theAtsFbMapRecordHelper the original AtsFbMapRecordHelper to copy.
         *
         */
        AtsFbMapRecordHelper( const AtsFbMapRecordHelper& theAtsFbMapRecordHelper);

        /** 
         * AtsFbMapRecordHelper
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
        AtsFbMapRecordHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsFbMapRecordHelper
         *
         * This constructor creates a new AtsFbMapRecordHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsFbMapRecordId() before writing 
         * to the database for the first time.
         *
         */
        AtsFbMapRecordHelper();

        
        /** 
         * ~AtsFbMapRecordHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsFbMapRecordHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsFbMapRecord.  A new AtsFbMapRecord is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsFbMapRecord
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getAtsBranchId
         *
         * Returns the Branch Id of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getBranchId();

		/**
         * getAtsBranchId
         *
         * Returns the Branch Id of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getBranchIdAsString();

		/**
         * getTrack
         *
         * Returns the Track of this Branch.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTrack()
         *
         * @return the Track 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Branch Id should be an unsigned long)
         *         - NO_VALUE if the BranchTrack record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
        std::string getTrack();

		/**
         * getMinAbscissa
         *
         * Returns the Min Abscissa of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getMinAbscissa();

		/**
         * getMaxAbscissa
         *
         * Returns the Max Abscissa of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getMaxAbscissa();

		/**
         * getZoneId
         *
         * Returns the Generic Zone Id of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		double getZoneId();

		/**
         * getZoneTypeName
         *
         * Returns the Zone Type Name of this AtsFbMapRecord record.  If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Zone Type Name as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getZoneTypeName();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsFbMapRecord record as an unsigned long.  
		 * If this AtsFbMapRecord record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsFbMapRecord has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setAtsBranchId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();
        

        /**
         * setBranchIdAsString
         *
         * Sets the Branch Id of this AtsFbMapRecord. The set values are not written to the database
         * until writeAtsFbMapRecordData() is called.  This method should only be used by the 
         * ConfigAtsFbMapRecord class.
         *
         * pre:    This is a new AtsFbMapRecord constructed by the default constructor.
         *
         * @param  idKey the Branch Id (primary key) for this AtsFbMapRecord
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setBranchIdAsString(const std::string& id);


        /**
         * setMinAbscissa
         *
         * Sets the Min Abscissa of this AtsFbMapRecord. The set values are not written to the database
         * until writeAtsFbMapRecordData() is called.  This method should only be used by the 
         * ConfigAtsFbMapRecord class.
         *
         * @param  minAbscissa The min Abscissa for this AtsFbMapRecord
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setMinAbscissa(unsigned long minAbscissa);

		/**
         * setMaxAbscissa
         *
         * Sets the Max Abscissa of this AtsFbMapRecord. The set values are not written to the database
         * until writeAtsFbMapRecordData() is called.  This method should only be used by the 
         * ConfigAtsFbMapRecord class.
         *
         * @param  maxAbscissa The min Abscissa for this AtsFbMapRecord
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setMaxAbscissa(unsigned long maxAbscissa);

		/**
         * setZoneId
         *
         * Sets the Zone Id of this AtsFbMapRecord. The set values are not written to the database
         * until writeAtsFbMapRecordData() is called.  This method should only be used by the 
         * ConfigAtsFbMapRecord class.
         *
         * @param  zoneId The zone id for this AtsFbMapRecord
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setZoneId(double zoneId);

		/**
         * setZoneTypeName
         *
         * Sets the Zone Type of this AtsFbMapRecord. The set values are not written to the database
         * until writeAtsFbMapRecordData() is called.  This method should only be used by the 
         * ConfigAtsFbMapRecord class.
         *
         * pre:    This is a new AtsFbMapRecord constructed by the default constructor.
         *
         * @param  zoneTypeName the Zone Type Name for this AtsFbMapRecord
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setZoneTypeName(const std::string& zoneTypeName);

        /**
         * invalidate
         *
         * Make the data contained by this AtsFbMapRecord as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsFbMapRecordData() has been called prior to calling
         * this method on a new AtsFbMapRecord, as it does not make any sense to invalidate an
         * AtsFbMapRecord that has not yet been written to the database.
         *
         * pre:    This AtsFbMapRecord was initially loaded from the database
         *         or writeAtsFbMapRecordData() has already been called
         */
        void invalidate();


        /**
         * writeAtsFbMapRecordData
         * 
         * Write this AtsFbMapRecord to the database.  Note that record uniqueness is enforced
         * for the primary key ATBTRA_ID 
         *
         * e.g. Records 2,3 & 4 will clash:
         *
         * 1. ATBTRA_ID    = 1
         *    TRACK        = IB
         * 2. ATBTRA_ID    = 1                  <= clash with record 1
         *    TRACK        = OB            
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. AtsFbMapRecordId should be an unsigned long)
         *          - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsFbMapRecord object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsFbMapRecordData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsFbMapRecord
         *
         * Remove this AtsFbMapRecord from the database. Once this method has been executed, the
         * AtsFbMapRecordHelper object should be destroyed.
         *
         * pre:    This AtsFbMapRecord was initially loaded from the database
         *         or writeAtsFbMapRecordData() has already been called
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
        void deleteAtsFbMapRecord(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsFbMapRecordHelper& operator=(const AtsFbMapRecordHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsFbMapRecord was initially loaded from the database
         *         or writeAtsFbMapRecordData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsFbMapRecordId should be an unsigned long)
         *         - NO_VALUE if the AtsFbMapRecord record cannot be found for the helper 
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
         * modifyExistingAtsFbMapRecord
         *
         * This will update an existing AtsFbMapRecord in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsFbMapRecord object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsFbMapRecord() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsFbMapRecord
         *
         * This will add a new AtsFbMapRecord in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsFbMapRecord object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsFbMapRecord() throw(TA_Base_Core::TransactiveException);

	protected:
		unsigned long getNextKey();

	private:
        int m_idKey;
        int m_modifiedIdKey;

		unsigned long m_branchId;
        std::string m_track;
		unsigned long m_minAbscissa;
		unsigned long m_maxAbscissa;
		double m_zoneId; 
		unsigned long m_zoneType;
		std::string m_zoneTypeName;

        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // defined (_MSC_VER)

#endif // !defined(AtsFbMapRecordHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
