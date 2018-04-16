/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/TiRatisDestinationHelper.h $
  * @author:  chunzhong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * TiRatisDestinationHelper is an object that is held by TiRatisDestination and ConfigTiRatisDestination objects. 
  * It contains all data used by TiRatisDestinations, and manipulation
  * methods for the data. It helps avoid re-writing code for both TiRatisDestination and ConfigTiRatisDestination.
  */

#ifndef TI_RATIS_DESTIONATION_HELPER_H_INCLUDED
#define TI_RATIS_DESTIONATION_HELPER_H_INCLUDED

#include <string>
#include <vector>
#include <climits>
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "core/data_access_interface/stis_manager/src/DatabaseQueryHelper.h"
#include "core/data_access_interface/stis_manager/src/GenericDaiHelper.h"


namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
    class TiRatisDestinationHelper
        : public GenericDaiHelper<TiRatisDestinationHelper>
    {

    public:


        /** 
         * TiRatisDestinationHelper
         *
         * This constructor creates a new TiRatisDestinationHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        TiRatisDestinationHelper(const unsigned long key);

        
        /** 
         * TiRatisDestinationHelper
         *
         * Copy constructor.
         *
         * @param   theTiRatisDestinationHelper the original TiRatisDestinationHelper to copy.
         *       
         */
        TiRatisDestinationHelper( const TiRatisDestinationHelper& theTiRatisDestinationHelper);

        /** 
         * TiRatisDestinationHelper
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
        TiRatisDestinationHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * TiRatisDestinationHelper
         *
         * This constructor creates a new TiRatisDestinationHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writeTiRatisDestinationData() is called for the first time.
         *
         */
        TiRatisDestinationHelper();


        /** 
         * ~TiRatisDestinationHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TiRatisDestinationHelper();


        /**
         * isNew
         *
         * This returns whether this is a new TiRatisDestination.  A new TiRatisDestination is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new TiRatisDestination
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        /**
         * getKey
         *
         * Returns the primary key of this TiRatisDestination.
         *
         * pre:    the data exists in the database
         *
         * @return the primary key 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. the PA Location Key should be an unsigned long)
         *         - NO_VALUE if the PaDvaMessage record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         */
        virtual unsigned long getKey();

		virtual std::string	getDestination();

		virtual void setDestination(const std::string & destination);


        /**
         * setKey
         *
         * Sets the ID of this TiRatisDestination. The set values are not written to the database
         * until writeTiRatisDestinationData() is called.  This method should only be used by the 
         * ConfigTiRatisDestination class.
         *
         * @param  id the id for this TiRatisDestination
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the TiRatisDestination record cannot be found for the helper 
         *           constructed with a key.
         *         - NOT_UNIQUE if the key returns multiple records 
         *
         */
        virtual void setKey(const unsigned long id);


        /**
         * setKeyToUse
         *
         * Sets the key to be used when writing new objects to the database
         *
         * @param keyToUse the key to use when updating / inserting into database
         *
         */
        virtual void setKeyToUse(unsigned long keyToUse);


        /**
         * invalidate
         *
         * Make the data contained by this TiRatisDestination as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeTiRatisDestinationData() has been called prior to calling
         * this method on a new TiRatisDestination, as it does not make any sense to invalidate an
         * TiRatisDestination that has not yet been written to the database.
         *
         * pre:    This TiRatisDestination was initially loaded from the database
         *         or writeTiRatisDestinationData() has already been called
         */
        void invalidate();


        /**
         * writeTiRatisDestinationData
         * 
         * Write this TiRatisDestination to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the TiRatisDestination record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the TiRatisDestination returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the TiRatisDestination object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeTiRatisDestinationData(); //throw(TA_Core::TransactiveException);


        /**
         * deleteTiRatisDestination
         *
         * Remove this TiRatisDestination from the database. Once this method has been executed, the
         * TiRatisDestinationHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This TiRatisDestination was initially loaded from the database
         *         or writeTiRatisDestinationData() has already been called
         *
         * @param  cascade
         *          set to true to force the removal of this record by removing all 
         *          references to this record
         *
         * @throws DatabaseException 
         *         - if there is a problem writing the data to the database.
         * @throws DataException
         *         - CANNOT_DELETE if references to this record exists in linked tables
         *           table.  These can be removed by specifying the cascade option.
         *
         */
        void deleteTiRatisDestination(bool cascade = false);
	
        /**
         * getBasicQueryData
         *
         * Required static implementation for template base class
         *
         * @see GenericDaiHelper::getBasicQueryData
         *
         */
        static const DatabaseQueryHelper::QueryData getBasicQueryData();

        /**
         * getModifiedKey
         *
         * @return the modified (if applicable) primary key, but if the
         *          key hasn't been modified, returns the actual primary key
         *
         */
        unsigned long getModifiedKey();

        /** 
         * isKeyDuplicate
         * 
         * Checks the database to confirm the user defined key value
         *  doesn't already exist in the database
         *
         * @return true if this key already present in database, false otherwise
         *
         * @param keyToUse the key to perform the search through table with
         *
         */       
        static bool isKeyDuplicate(unsigned long keyToUse);

    protected:

        /**
         * reloadUsing (interface implementation)
         *
         * @see GenericDaiHelper::reloadUsing for more details
         *
         */
        virtual void reloadUsing(unsigned long row, TA_Base_Core::IData& data);

    private:

        // Made private as it is not used
		TiRatisDestinationHelper& operator=(const TiRatisDestinationHelper &);

  
          /**
         * modifyExistingTiRatisDestination
         *
         * This will update an existing TiRatisDestination in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the TiRatisDestination object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingTiRatisDestination(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewTiRatisDestination
         *
         * This will add a new TiRatisDestination in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the TiRatisDestination object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         *
         */
        void addNewTiRatisDestination(); //throw(TA_Core::TransactiveException);
        
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }

        void validateKeyExistence(); //throw(TA_Core::TransactiveException)

        void validateKeyAbsence(); //throw(TA_Core::TransactiveException)
        
        void validateDestinationAbsence();  //throw(TA_Core::Transactiveexception)

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;

        // The key we want to change to (if at all)
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_modifiedKey;

		std::string	m_destination;

        bool m_isValidData;

        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(TiRatisDestinationHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
