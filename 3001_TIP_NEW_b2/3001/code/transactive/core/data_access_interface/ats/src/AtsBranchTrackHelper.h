/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsBranchTrackHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * AtsBranchTrackHelper is an object that is held by AtsBranchTrack and ConfigAtsBranchTrack objects. 
  * It contains all data used by AtsBranchTracks, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsBranchTrack and ConfigAtsBranchTrack.
  */

#if !defined(AtsBranchTrackHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsBranchTrackHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsBranchTrackHelper
    {

    public:


        /** 
         * AtsBranchTrackHelper
         *
         * This constructor creates a new AtsBranchTrackHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsBranchTrackHelper(const unsigned long idKey);
         
        
        /** 
         * AtsBranchTrackHelper
         *
         * Copy constructor.
         *
         * @param   theAtsBranchTrackHelper the original AtsBranchTrackHelper to copy.
         *
         */
        AtsBranchTrackHelper( const AtsBranchTrackHelper& theAtsBranchTrackHelper);

        /** 
         * AtsBranchTrackHelper
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
        AtsBranchTrackHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsBranchTrackHelper
         *
         * This constructor creates a new AtsBranchTrackHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsBranchTrackId() before writing 
         * to the database for the first time.
         *
         */
        AtsBranchTrackHelper();

        
        /** 
         * ~AtsBranchTrackHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsBranchTrackHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsBranchTrack.  A new AtsBranchTrack is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsBranchTrack
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getAtsBranchId
         *
         * Returns the Branch Id of this AtsBranchTrack record.  If this AtsBranchTrack record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsBranchTrack has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		std::string getAtsBranchId();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsBranchTrack record as an unsigned long.  
		 * If this AtsBranchTrack record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsBranchTrack has been requested, all the data is loaded from the database.
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
         *         - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();


        /**
         * getTrack
         *
         * Returns the Track of this Branch.  If this AtsBranchTrack record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsBranchTrack has been requested, all the data is loaded from the database.
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
         * setAtsBranchTrackId
         *
         * Sets the AtsBranchTrackId of this AtsBranchTrack. The set values are not written to the database
         * until writeAtsBranchTrackData() is called.  This method should only be used by the 
         * ConfigAtsBranchTrack class.
         *
         * pre:    This is a new AtsBranchTrack constructed by the default constructor.
         *
         * @param  idKey the Branch Id (primary key) for this AtsBranchTrack
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchTrackId should be an unsigned long)
         *         - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setAtsBranchId(const std::string& id);


        /**
         * setTrack
         *
         * Sets the Track of this AtsBranchTrack. The set values are not written to the database
         * until writeAtsBranchTrackData() is called.  This method should only be used by the 
         * ConfigAtsBranchTrack class.
         *
         * @param  track the Track description for this AtsBranchTrack
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchTrackId should be an unsigned long)
         *         - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setTrack(const std::string& track);

    
        /**
         * invalidate
         *
         * Make the data contained by this AtsBranchTrack as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsBranchTrackData() has been called prior to calling
         * this method on a new AtsBranchTrack, as it does not make any sense to invalidate an
         * AtsBranchTrack that has not yet been written to the database.
         *
         * pre:    This AtsBranchTrack was initially loaded from the database
         *         or writeAtsBranchTrackData() has already been called
         */
        void invalidate();


        /**
         * writeAtsBranchTrackData
         * 
         * Write this AtsBranchTrack to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsBranchTrackId should be an unsigned long)
         *          - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsBranchTrack object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsBranchTrackData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsBranchTrack
         *
         * Remove this AtsBranchTrack from the database. Once this method has been executed, the
         * AtsBranchTrackHelper object should be destroyed.
         *
         * pre:    This AtsBranchTrack was initially loaded from the database
         *         or writeAtsBranchTrackData() has already been called
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
        void deleteAtsBranchTrack(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsBranchTrackHelper& operator=(const AtsBranchTrackHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsBranchTrack was initially loaded from the database
         *         or writeAtsBranchTrackData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsBranchTrackId should be an unsigned long)
         *         - NO_VALUE if the AtsBranchTrack record cannot be found for the helper 
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
         * modifyExistingAtsBranchTrack
         *
         * This will update an existing AtsBranchTrack in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsBranchTrack object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsBranchTrack() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsBranchTrack
         *
         * This will add a new AtsBranchTrack in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsBranchTrack object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsBranchTrack() throw(TA_Base_Core::TransactiveException);

        void validateBranchIdUniqueness() throw(TA_Base_Core::TransactiveException);

        int m_idKey;
        int m_modifiedIdKey;
        std::string m_track;
        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_IRS_Core

#if defined(_MSC_VER)
	#pragma warning(pop)
#endif // _MSC_VER

#endif // !defined(AtsBranchTrackHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
