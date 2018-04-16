/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/PidsManager/src/PidStationIdHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * PidStationIdHelper is an object that is held by PidStationId and ConfigPidStationId objects. 
  * It contains all data used by PidStationIds, and manipulation
  * methods for the data. It helps avoid re-writing code for both PidStationId and ConfigPidStationId.
  */

#if !defined(PidStationIdHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define PidStationIdHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

#ifdef _MSC_VER
	#pragma warning(push)
	#pragma warning(disable: 4290) // C++ Exception Specification ignored
#endif

#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/IData.h"

#include <string>
#include <vector>
#include <climits>

namespace TA_IRS_Core
{

    class PidStationIdHelper
    {

    public:


        /** 
         * PidStationIdHelper
         *
         * This constructor creates a new PidStationIdHelper for the specified primary key.
         *
         * @param  the primary key
         *
         */
        PidStationIdHelper(const unsigned long pKey);
         
        
        /** 
         * PidStationIdHelper
         *
         * Copy constructor.
         *
         * @param   thePidStationIdHelper the original PidStationIdHelper to copy.
         *
         */
        PidStationIdHelper( const PidStationIdHelper& thePidStationIdHelper);

        /** 
         * PidStationIdHelper
         *
         * This constructor creates a new object using the input data
         *      which is representative of a row returned from SQL statement
         *
         * @param row the row of data in the data object that we should collect data from
         *          in order to construct ourselves
         *
         * @param data the TA_Base_Core::IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *
         * @see getBasicQueryData         
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        PidStationIdHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * PidStationIdHelper
         *
         * This constructor creates a new PidStationIdHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setPidStationIdId() before writing 
         * to the database for the first time.
         *
         */
        PidStationIdHelper();

        
        /** 
         * ~PidStationIdHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PidStationIdHelper();

        /**
         * isNew
         *
         * This returns whether this is a new PidStationId.  A new PidStationId is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new PidStationId
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


        /** 
          * getKey
          *
          * Gets the primary key of this row.
          *
          * @return the primary key.
          */
        unsigned long getKey();


		/**
         * getStationId
         *
         * Returns the Branch Id of this PidStationId record.  If this PidStationId record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PidStationId has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setStationId()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. StationId should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getStationId();


		/**
         * getTaLocation
         *
         * Returns the TaLocation of this Branch.  If this PidStationId record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PidStationId has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setTaLocation()
         *
         * @return the TaLocation 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. Branch Id should be an unsigned long)
         *         - NO_VALUE if the BranchTaLocation record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
        unsigned long getTaLocation();
        

        /**
         * setPidStationIdId
         *
         * Sets the PidStationIdId of this PidStationId. The set values are not written to the database
         * until writePidStationIdData() is called.  This method should only be used by the 
         * ConfigPidStationId class.
         *
         * pre:    This is a new PidStationId constructed by the default constructor.
         *
         * @param  idKey the Branch Id (primary key) for this PidStationId
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PidStationIdId should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setStationId(unsigned long id);


        /**
         * setTaLocation
         *
         * Sets the TaLocation of this PidStationId. The set values are not written to the database
         * until writePidStationIdData() is called.  This method should only be used by the 
         * ConfigPidStationId class.
         *
         * @param  track the TaLocation description for this PidStationId
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PidStationIdId should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setTaLocation(unsigned long taLocation);


        /**
         * getDateCreated
         *
         * Returns the date created for this PidStationId.  If this PidStationId record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PidStationId has been requested, all the data is loaded from the database.
         *
         * @return the date created for this PidStationId as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this PidStationId.  If this PidStationId record does not 
         * exist in the database (constructed without a key), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing PidStationId has been requested, all the data is loaded from the database.
         *
         * @return the date modified for this PidStationId as a time_t.
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        time_t getDateModified();

    
        /**
         * invalidate
         *
         * Make the data contained by this PidStationId as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writePidStationIdData() has been called prior to calling
         * this method on a new PidStationId, as it does not make any sense to invalidate an
         * PidStationId that has not yet been written to the database.
         *
         * pre:    This PidStationId was initially loaded from the database
         *         or writePidStationIdData() has already been called
         */
        void invalidate();


        /**
         * writePidStationIdData
         * 
         * Write this PidStationId to the database.  Note that record uniqueness is enforced
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
         * @throws  TA_Base_Core::DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. PidStationIdId should be an unsigned long)
         *          - NO_VALUE if the PidStationId record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the PidStationId object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writePidStationIdData();


        /**
         * deletePidStationId
         *
         * Remove this PidStationId from the database. Once this method has been executed, the
         * PidStationIdHelper object should be destroyed.
         *
         * pre:    This PidStationId was initially loaded from the database
         *         or writePidStationIdData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws TA_Base_Core::DataException 
         *         - CANNOT_DELETE if references to this record exists in 
         *           AT_FIXED_BLOCK_MAP.  These can be removed by specifying the cascade option.
         *
         */
        void deletePidStationId();
		

    private:
        // Made private as it is not used
		PidStationIdHelper& operator=(const PidStationIdHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This PidStationId was initially loaded from the database
         *         or writePidStationIdData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. PidStationIdId should be an unsigned long)
         *         - NO_VALUE if the PidStationId record cannot be found for the helper 
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
         * @param data the TA_Base_Core::IData interface that should have been obtained using the 
         *              getBasicQueryData function
         *         
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws TA_Base_Core::DataException
         *         - if the data cannot be converted to the required format 
         *         - NO_VALUE if the record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the primary key returns multiple records 
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);
        
        /**
         * modifyExistingPidStationId
         *
         * This will update an existing PidStationId in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PidStationId object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  TA_Base_Core::DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingPidStationId();

        /**
         * addNewPidStationId
         *
         * This will add a new PidStationId in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the PidStationId object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  TA_Base_Core::DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewPidStationId();

        unsigned long m_pkey;
        unsigned long m_stationId;
        unsigned long m_taLocation;

        time_t m_dateCreated;
        time_t m_dateModified;
        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_IRS_Core

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif // !defined(PidStationIdHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
