/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/TIRatisTagHelper.h $
  * @author:  chunzhong
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * TiRatisTagHelper is an object that is held by TiRatisTag and ConfigTiRatisTag objects. 
  * It contains all data used by TiRatisTags, and manipulation
  * methods for the data. It helps avoid re-writing code for both TiRatisTag and ConfigTiRatisTag.
  */

#ifndef TI_RATIS_TAG_HELPER_H_INCLUDED
#define TI_RATIS_TAG_HELPER_H_INCLUDED

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
    class TiRatisTagHelper
        : public GenericDaiHelper<TiRatisTagHelper>
    {

    public:


        /** 
         * TiRatisTagHelper
         *
         * This constructor creates a new TiRatisTagHelper for the specified primary key.
         *
         * @param  key the primary key
         *
         */
        TiRatisTagHelper(const unsigned long key);

        
        /** 
         * TiRatisTagHelper
         *
         * Copy constructor.
         *
         * @param   theTiRatisTagHelper the original TiRatisTagHelper to copy.
         *       
         */
        TiRatisTagHelper( const TiRatisTagHelper& theTiRatisTagHelper);

        /** 
         * TiRatisTagHelper
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
        TiRatisTagHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * TiRatisTagHelper
         *
         * This constructor creates a new TiRatisTagHelper for configuration, initiailising
         * the primary key to TA_Core::DatabaseKey::isInvalidKey.  This primary key will be automatically set to a valid value 
         * when writeTiRatisTagData() is called for the first time.
         *
         */
        TiRatisTagHelper();


        /** 
         * ~TiRatisTagHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~TiRatisTagHelper();


        /**
         * isNew
         *
         * This returns whether this is a new TiRatisTag.  A new TiRatisTag is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new TiRatisTag
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };

        /**
         * getKey
         *
         * Returns the primary key of this TiRatisTag.
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

		virtual std::string	getTagGroup();

		virtual std::string	getDetail();

		virtual std::string	getRatisTag();

		virtual std::string	getTagGroupString() const;

		virtual std::string	getDetailString() const;

		virtual void setTagGroup(const std::string & tagGroup);

		virtual void setDetail(const std::string& detail);



        /**
         * setKey
         *
         * Sets the ID of this TiRatisTag. The set values are not written to the database
         * until writeTiRatisTagData() is called.  This method should only be used by the 
         * ConfigTiRatisTag class.
         *
         * @param  id the id for this TiRatisTag
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. id should be an unsigned long)
         *         - NO_VALUE if the TiRatisTag record cannot be found for the helper 
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
         * Make the data contained by this TiRatisTag as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeTiRatisTagData() has been called prior to calling
         * this method on a new TiRatisTag, as it does not make any sense to invalidate an
         * TiRatisTag that has not yet been written to the database.
         *
         * pre:    This TiRatisTag was initially loaded from the database
         *         or writeTiRatisTagData() has already been called
         */
        void invalidate();


        /**
         * writeTiRatisTagData
         * 
         * Write this TiRatisTag to the database.
         *
         * @throws  DatabaseException 
         *          - if there is a problem establishing a connection with the database
         *          - if an error is encountered while retrieving data
         * @throws  DataException
         *          - if the data cannot be converted to the required format 
         *            (e.g. the PA Location Key should be an unsigned long)
         *          - NO_VALUE if the TiRatisTag record cannot be found for the helper 
         *            constructed with a key.
         *          - NOT_UNIQUE if the TiRatisTag returns multiple records
         *          - if the LOCATIONKEY references a non-existent LOCATION record.          
         * @throws  DataConfigurationException
         *          - if the data contained in the TiRatisTag object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeTiRatisTagData(); //throw(TA_Core::TransactiveException);


        /**
         * deleteTiRatisTag
         *
         * Remove this TiRatisTag from the database. Once this method has been executed, the
         * TiRatisTagHelper object should be destroyed.  Note that the track field can be NULL 
         * and doesn't require a value before writing to the database.
         *
         * pre:    This TiRatisTag was initially loaded from the database
         *         or writeTiRatisTagData() has already been called
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
        void deleteTiRatisTag(bool cascade = false);
	
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
		TiRatisTagHelper& operator=(const TiRatisTagHelper &);

  
  
        /**
         * modifyExistingTiRatisTag
         *
         * This will update an existing TiRatisTag in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the TiRatisTag object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the LOCATIONKEY references a non-existent LOCATION record. 
         * @throws  ValueNotSetException
         *          - If some of the required internal values for this zone have not been set
         */
        void modifyExistingTiRatisTag(); //throw(TA_Core::TransactiveException);

        
        /**
         * addNewTiRatisTag
         *
         * This will add a new TiRatisTag in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the TiRatisTag object is not sufficent to create an
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
        void addNewTiRatisTag(); //throw(TA_Core::TransactiveException);
        
        /**
         * getKeyDirect (base class pure virtual implementation)
         *
         * @see GenericDaiHelper::getKeyDirect
         *
         */
        virtual unsigned long getKeyDirect() const { return m_key.getValue(); }

        void validateKeyExistence(); //throw(TA_Core::TransactiveException)
        void validateKeyAbsence(); //throw(TA_Core::TransactiveException)
        void validateGroupDetailPairUniqueness(); //throw(TA_Core::TransactiveException)

        TA_Base_Core::PrimitiveWrapper<unsigned long> m_key;
        // The key we want to change to (if at all)
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_modifiedKey;

		std::string	m_tagGroup;
		std::string m_detail;
		std::string m_ratisTag;

        bool m_isValidData;
        bool m_isNew;

    };
} // closes TA_Core

#endif // !defined(TiRatisTagHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
