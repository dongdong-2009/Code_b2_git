/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsPlatformHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsPlatformHelper is an object that is held by AtsPlatform and ConfigAtsPlatform objects. 
  * It contains all data used by AtsPlatforms, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsPlatform and ConfigAtsPlatform.
  */

#if !defined(AtsPlatformHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsPlatformHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsPlatformHelper
    {

    public:


        /** 
         * AtsPlatformHelper
         *
         * This constructor creates a new AtsPlatformHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsPlatformHelper(const unsigned long idKey);
         
        
        /** 
         * AtsPlatformHelper
         *
         * Copy constructor.
         *
         * @param   theAtsPlatformHelper the original AtsPlatformHelper to copy.
         *
         */
        AtsPlatformHelper( const AtsPlatformHelper& theAtsPlatformHelper);

        /** 
         * AtsPlatformHelper
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
        AtsPlatformHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsPlatformHelper
         *
         * This constructor creates a new AtsPlatformHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsPlatformId() before writing 
         * to the database for the first time.
         *
         */
        AtsPlatformHelper();

        
        /** 
         * ~AtsPlatformHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsPlatformHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsPlatform.  A new AtsPlatform is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsPlatform
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getStationId
         *
         * Returns the Branch Id of this AtsPlatform record.  If this AtsPlatform record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPlatform has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getStationId();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsPlatform record as an unsigned long.  
		 * If this AtsPlatform record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPlatform has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();


        /**
         * getPlatformId
         *
         * Returns the Platform ID of this Platform.  If this AtsPlatform record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPlatform has been requested, all the data is loaded from the database.
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
         * getName
         *
         * Returns the name of this Platform.  If this AtsPlatform record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsPlatform has been requested, all the data is loaded from the database.
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
         * setStationId
         *
         * Sets the Ats Station Id of this AtsPlatform. The set values are not written to the database
         * until writeAtsPlatformData() is called.  This method should only be used by the 
         * ConfigAtsPlatform class.
         *
         * pre:    This is a new AtsPlatform constructed by the default constructor.
         *
         * @param  idKey the Branch Id (primary key) for this AtsPlatform
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPlatformId should be an unsigned long)
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setStationId(unsigned long id);


        /**
         * setPlatformId
         *
         * Sets the Platform Id of this AtsPlatform. The set values are not written to the database
         * until writeAtsPlatformData() is called.  This method should only be used by the 
         * ConfigAtsPlatform class.
         *
         * @param  track the Platform description for this AtsPlatform
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPlatformId should be an unsigned long)
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setPlatformId(unsigned long id);


        /**
         * setTisPlatformId
         *
         * Sets the TIS Platform Id of this AtsPlatform. The set values are not written to the database
         * until writeAtsPlatformData() is called.  This method should only be used by the 
         * ConfigAtsPlatform class.
         *
         * @param  TIS id for this AtsPlatform
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPlatformId should be an unsigned long)
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setTisPlatformId(unsigned char id);

		/**
         * setName
         *
         * Sets the name of this AtsPlatform. The set values are not written to the database
         * until writeAtsPlatformData() is called.  This method should only be used by the 
         * ConfigAtsPlatform class.
         *
         * @param  track the Platform description for this AtsPlatform
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPlatformId should be an unsigned long)
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setName(const std::string& name);

    
        /**
         * invalidate
         *
         * Make the data contained by this AtsPlatform as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsPlatformData() has been called prior to calling
         * this method on a new AtsPlatform, as it does not make any sense to invalidate an
         * AtsPlatform that has not yet been written to the database.
         *
         * pre:    This AtsPlatform was initially loaded from the database
         *         or writeAtsPlatformData() has already been called
         */
        void invalidate();


        /**
         * writeAtsPlatformData
         * 
         * Write this AtsPlatform to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsPlatformId should be an unsigned long)
         *          - NO_VALUE if the AtsPlatform record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsPlatform object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsPlatformData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsPlatform
         *
         * Remove this AtsPlatform from the database. Once this method has been executed, the
         * AtsPlatformHelper object should be destroyed.
         *
         * pre:    This AtsPlatform was initially loaded from the database
         *         or writeAtsPlatformData() has already been called
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
        void deleteAtsPlatform(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsPlatformHelper& operator=(const AtsPlatformHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsPlatform was initially loaded from the database
         *         or writeAtsPlatformData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsPlatformId should be an unsigned long)
         *         - NO_VALUE if the AtsPlatform record cannot be found for the helper 
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
         * modifyExistingAtsPlatform
         *
         * This will update an existing AtsPlatform in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPlatform object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsPlatform() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsPlatform
         *
         * This will add a new AtsPlatform in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsPlatform object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsPlatform() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        unsigned long m_idKey;
        unsigned long m_modifiedIdKey;
		unsigned long m_platformId;
        unsigned char m_tisPlatformId;
		unsigned long m_stationId;
		std::string m_name;
        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AtsPlatformHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
