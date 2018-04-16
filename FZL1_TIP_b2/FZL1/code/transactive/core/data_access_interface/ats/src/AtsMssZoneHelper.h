/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsMssZoneHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * AtsMssZoneHelper is an object that is held by AtsMssZone and ConfigAtsMssZone objects. 
  * It contains all data used by AtsMssZones, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsMssZone and ConfigAtsMssZone.
  */

#if !defined(AtsMssZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsMssZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsMssZoneHelper
    {

    public:


        /** 
         * AtsMssZoneHelper
         *
         * This constructor creates a new AtsMssZoneHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsMssZoneHelper(const unsigned long idKey);
         
        
        /** 
         * AtsMssZoneHelper
         *
         * Copy constructor.
         *
         * @param   theAtsMssZoneHelper the original AtsMssZoneHelper to copy.
         *
         */
        AtsMssZoneHelper( const AtsMssZoneHelper& theAtsMssZoneHelper);

        /** 
         * AtsMssZoneHelper
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
        AtsMssZoneHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsMssZoneHelper
         *
         * This constructor creates a new AtsMssZoneHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsMssZoneId() before writing 
         * to the database for the first time.
         *
         */
        AtsMssZoneHelper();

        
        /** 
         * ~AtsMssZoneHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsMssZoneHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsMssZone.  A new AtsMssZone is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsMssZone
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getStationId
         *
         * Returns the Branch Id of this AtsMssZone record.  If this AtsMssZone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsMssZone has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsMssZone record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getStationId();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsMssZone record as an unsigned long.  
		 * If this AtsMssZone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsMssZone has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsMssZone record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();


        /**
         * getMssZone
         *
         * Returns the MssZone of this Branch.  If this AtsMssZone record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsMssZone has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setMssZone()
         *
         * @return the MssZone 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Branch Id should be an unsigned long)
         *         - NO_VALUE if the MssZone record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
        std::string getMssZone();
        

        /**
         * setAtsMssZoneId
         *
         * Sets the AtsMssZoneId of this AtsMssZone. The set values are not written to the database
         * until writeAtsMssZoneData() is called.  This method should only be used by the 
         * ConfigAtsMssZone class.
         *
         * pre:    This is a new AtsMssZone constructed by the default constructor.
         *
         * @param  idKey the Branch Id (primary key) for this AtsMssZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsMssZoneId should be an unsigned long)
         *         - NO_VALUE if the AtsMssZone record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setStationId(unsigned long id);


        /**
         * setMssZone
         *
         * Sets the MssZone of this AtsMssZone. The set values are not written to the database
         * until writeAtsMssZoneData() is called.  This method should only be used by the 
         * ConfigAtsMssZone class.
         *
         * @param  track the MssZone description for this AtsMssZone
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsMssZoneId should be an unsigned long)
         *         - NO_VALUE if the AtsMssZone record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setMssZone(const std::string& track);

    
        /**
         * invalidate
         *
         * Make the data contained by this AtsMssZone as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsMssZoneData() has been called prior to calling
         * this method on a new AtsMssZone, as it does not make any sense to invalidate an
         * AtsMssZone that has not yet been written to the database.
         *
         * pre:    This AtsMssZone was initially loaded from the database
         *         or writeAtsMssZoneData() has already been called
         */
        void invalidate();


        /**
         * writeAtsMssZoneData
         * 
         * Write this AtsMssZone to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsMssZoneId should be an unsigned long)
         *          - NO_VALUE if the AtsMssZone record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsMssZone object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsMssZoneData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsMssZone
         *
         * Remove this AtsMssZone from the database. Once this method has been executed, the
         * AtsMssZoneHelper object should be destroyed.
         *
         * pre:    This AtsMssZone was initially loaded from the database
         *         or writeAtsMssZoneData() has already been called
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
        void deleteAtsMssZone(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsMssZoneHelper& operator=(const AtsMssZoneHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsMssZone was initially loaded from the database
         *         or writeAtsMssZoneData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsMssZoneId should be an unsigned long)
         *         - NO_VALUE if the AtsMssZone record cannot be found for the helper 
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
         * modifyExistingAtsMssZone
         *
         * This will update an existing AtsMssZone in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsMssZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsMssZone() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsMssZone
         *
         * This will add a new AtsMssZone in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsMssZone object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsMssZone() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        int m_idKey;
        int m_modifiedIdKey;
		int m_stationId;
        std::string m_mssZone;
        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_IRS_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AtsMssZoneHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
