/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/AtsDssPlanMapHelper.h $
  * @author:  Nick Jardine
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * AtsDssPlanMapHelper is an object that is held by AtsDssPlanMap and ConfigAtsDssPlanMap objects. 
  * It contains all data used by AtsDssPlanMaps, and manipulation
  * methods for the data. It helps avoid re-writing code for both AtsDssPlanMap and ConfigAtsDssPlanMap.
  */

#if !defined(AtsDssPlanMapHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define AtsDssPlanMapHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

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
    class AtsDssPlanMapHelper
    {

    public:


        /** 
         * AtsDssPlanMapHelper
         *
         * This constructor creates a new AtsDssPlanMapHelper for the specified Branch Id (primary key).
         *
         * @param  idKey the Branch Id (primary key)
         *
         */
        AtsDssPlanMapHelper(const unsigned long idKey);
         
        
        /** 
         * AtsDssPlanMapHelper
         *
         * Copy constructor.
         *
         * @param   theAtsDssPlanMapHelper the original AtsDssPlanMapHelper to copy.
         *
         */
        AtsDssPlanMapHelper( const AtsDssPlanMapHelper& theAtsDssPlanMapHelper);

        /** 
         * AtsDssPlanMapHelper
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
        AtsDssPlanMapHelper(unsigned long row, TA_Base_Core::IData& data);

        /** 
         * AtsDssPlanMapHelper
         *
         * This constructor creates a new AtsDssPlanMapHelper for configuration, initiailising
         * the Branch Id to 0.  This needs to be specified by setAtsDssPlanMapId() before writing 
         * to the database for the first time.
         *
         */
        AtsDssPlanMapHelper();

        
        /** 
         * ~AtsDssPlanMapHelper
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~AtsDssPlanMapHelper();

        /**
         * isNew
         *
         * This returns whether this is a new AtsDssPlanMap.  A new AtsDssPlanMap is created by the default
         * constructor.  Once written to the database, it is no longer considered new.
         *
         * @return true if this is a new AtsDssPlanMap
         */
        virtual bool isNew() const
        {
            return m_isNew;
        };


		/**
         * getDssEvent
         *
         * Returns the Branch Id of this AtsDssPlanMap record.  If this AtsDssPlanMap record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsDssPlanMap has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setDssEvent()
         *
         * @return the Branch Id as a string
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. DssEvent should be an unsigned long)
         *         - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getDssEvent();

		/**
         * getKey
         *
         * Returns the Branch Id of this AtsDssPlanMap record as an unsigned long.  
		 * If this AtsDssPlanMap record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsDssPlanMap has been requested, all the data is loaded from the database.
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
         *           (e.g. DssEvent should be an unsigned long)
         *         - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
         *           constructed with a Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         */
		unsigned long getKey();


		/**
         * getPlanPath
         *
         * Returns the Plan Path of this DSS Plan Map.  If this AtsDssPlanMap record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsDssPlanMap has been requested, all the data is loaded from the database.
         *
         * pre: The data exists in the database or has been set by a call to setPlanPath()
         *
         * @return the Plan Path 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. DSS Input should be an unsigned long)
         *         - NO_VALUE if the DSS Plan Map record cannot be found for the helper 
         *           constructed with a DSS Input.
         *         - NOT_UNIQUE if the DSS Input returns multiple records 
         */
		std::string getPlanPath();

		/**
         * getName
         *
         * Returns the name of this DSS Plan Map.  If this AtsDssPlanMap record does not 
         * exist in the database (constructed without an Id), then data will be retrieved
         * from within the class.  Otherwise, if this is the first time data for this 
         * existing AtsDssPlanMap has been requested, all the data is loaded from the database.
         *
         * @return the Plan Map Name 
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. DSS Input should be an unsigned long)
         *         - NO_VALUE if the DSS Plan Map record cannot be found for the helper 
         *           constructed with a DSS Input.
         *         - NOT_UNIQUE if the DSS Input returns multiple records 
         */
		std::string getName();
        

        /**
         * setDssEvent
         *
         * Sets the Ats Station Id of this AtsDssPlanMap. The set values are not written to the database
         * until writeAtsDssPlanMapData() is called.  This method should only be used by the 
         * ConfigAtsDssPlanMap class.
         *
         * @param  dssEvent the DSS Event for this AtsDssPlanMap
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsDssPlanMapId should be an unsigned long)
         *         - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setDssEvent(unsigned long dssEvent);

		/**
         * setPlanPath
         *
         * Sets the Plan Path of this AtsDssPlanMap. The set values are not written to the database
         * until writeAtsDssPlanMapData() is called.  This method should only be used by the 
         * ConfigAtsDssPlanMap class.
         *
         * @param  planPath the Plan Path for this AtsDssPlanMap
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the specified plan path does not denote an existing plan
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsDssPlanMapId should be an unsigned long)
         *         - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
         *           constructed with an Branch Id.
         *         - NOT_UNIQUE if the Branch Id returns multiple records 
         *
         */
        void setPlanPath(const std::string& planPath);
    
        /**
         * invalidate
         *
         * Make the data contained by this AtsDssPlanMap as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.  A possible scenario for this method to be used is when it is known
         * that the database has been modified outside the process.
         *
         * It is a precondition that writeAtsDssPlanMapData() has been called prior to calling
         * this method on a new AtsDssPlanMap, as it does not make any sense to invalidate an
         * AtsDssPlanMap that has not yet been written to the database.
         *
         * pre:    This AtsDssPlanMap was initially loaded from the database
         *         or writeAtsDssPlanMapData() has already been called
         */
        void invalidate();


        /**
         * writeAtsDssPlanMapData
         * 
         * Write this AtsDssPlanMap to the database.  Note that record uniqueness is enforced
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
         *            (e.g. AtsDssPlanMapId should be an unsigned long)
         *          - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
         *            constructed with an Branch Id.
         *          - NOT_UNIQUE if the Branch Id returns multiple records
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         * @throws  DataConfigurationException
         *          - if the data contained in the AtsDssPlanMap object is not sufficent to create 
         *            an entry in the database. This is thrown as an exception and not tested 
         *            as an assert as it can be directly linked to GUI components and may be 
         *            the result of a user action that occurs prior to filling in all sections.
         *
         */
        void writeAtsDssPlanMapData() throw(TA_Base_Core::TransactiveException);


        /**
         * deleteAtsDssPlanMap
         *
         * Remove this AtsDssPlanMap from the database. Once this method has been executed, the
         * AtsDssPlanMapHelper object should be destroyed.
         *
         * pre:    This AtsDssPlanMap was initially loaded from the database
         *         or writeAtsDssPlanMapData() has already been called
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
        void deleteAtsDssPlanMap(bool cascade = false);
		

    private:
        // Made private as it is not used
		AtsDssPlanMapHelper& operator=(const AtsDssPlanMapHelper &);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre:    This AtsDssPlanMap was initially loaded from the database
         *         or writeAtsDssPlanMapData() has already been called
         *
         * @throws DatabaseException 
         *         - if there is a problem establishing a connection with the database.
         *         - if an error is encountered while retrieving data.
         * @throws DataException
         *         - if the data cannot be converted to the required format 
         *           (e.g. AtsDssPlanMapId should be an unsigned long)
         *         - NO_VALUE if the AtsDssPlanMap record cannot be found for the helper 
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
         * modifyExistingAtsDssPlanMap
         *
         * This will update an existing AtsDssPlanMap in the database with the internals currently 
         * set in this instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsDssPlanMap object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void modifyExistingAtsDssPlanMap() throw(TA_Base_Core::TransactiveException);

        
        /**
         * addNewAtsDssPlanMap
         *
         * This will add a new AtsDssPlanMap in the database with the internals currently set in this 
         * instance.
         *
         * @throws  DatabaseException 
         *          - if there is a problem writing the data to the database.  
         * @throws  DataConfigurationException 
         *          - if the data contained in the AtsDssPlanMap object is not sufficent to create an
         *            entry in the database.  This is thrown as an exception and not tested as 
         *            an assert as it can be directly linked to GUI components and may be the 
         *            result of a user action that occurs prior to filling in all sections.
         * @throws  DataException 
         *          - if there is a problem writing the data to the database.
         *          - if the ENTITYKEY references a non-existent ENTITY record. 
         *
         */
        void addNewAtsDssPlanMap() throw(TA_Base_Core::TransactiveException);

		unsigned long getNextKey();

	private:

        int m_idKey;
        int m_modifiedIdKey;
		int m_dssEvent;
		std::string m_planPath;
        bool m_isValidData;
        bool m_isNew; 
        

    };
} // closes TA_Core

#endif // !defined(AtsDssPlanMapHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
