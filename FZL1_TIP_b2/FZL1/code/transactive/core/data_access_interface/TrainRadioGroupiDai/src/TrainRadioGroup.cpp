/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $14:14 2007-11-5$
  * Last modified by:  $Lan Yuan$
  *
  */

#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroup.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DataConversion.h"
#include "core/utilities/src/DebugUtil.h"

#include <algorithm>

namespace TA_IRS_Core
{

    const std::string TrainRadioGroup::TR_RADIO_GROUP_TABLE = "TR_RADIO_GROUP";
    const std::string TrainRadioGroup::TR_GROUP_COVERAGE_TABLE = "TR_GROUP_COVERAGE";
    const std::string TrainRadioGroup::GROUP_PKEY_COLUMN = "TRRADG_ID";
    const std::string TrainRadioGroup::GROUP_TSI_COLUMN = "GROUP_TSI";
    const std::string TrainRadioGroup::LOCATION_COLUMN = "LOCATION";
    const std::string TrainRadioGroup::RA_SUBSCRIBERS_TABLE = "RA_SUBSCRIBERS";
    const std::string TrainRadioGroup::SUBSCRIBER_ID_COLUMN = "RASUBS_ID";
    const std::string TrainRadioGroup::SUBSCRIBER_NAME_COLUMN = "SUBSCRIBER_NAME";
    const std::string TrainRadioGroup::GROUP_PKEY_SEQUENCE = "TRRADG_SEQ";


    TA_Base_Core::NonReEntrantThreadLockable TrainRadioGroup::s_columnNamesLock;
    std::vector<std::string> TrainRadioGroup::s_columnNames;
    std::vector<std::string> TrainRadioGroup::s_radioSubscriberColumnNames;
    unsigned long TrainRadioGroup::s_uniqueIdentifier = 0;



    TrainRadioGroup::TrainRadioGroup()
        : m_uniqueIdentifier( ++s_uniqueIdentifier ),
          m_isNew( true ),
          m_isValidData(false),
          m_pkey( 0 ),
          m_groupTsi( "" ),
          m_name( "" )
    {
        FUNCTION_ENTRY( "TrainRadioGroup" );
        FUNCTION_EXIT;
    }


    TrainRadioGroup::TrainRadioGroup( const TrainRadioGroup& rhs )
        : m_uniqueIdentifier( ++s_uniqueIdentifier ),
          m_isNew( false ),
          m_isValidData( true ),
          m_pkey( rhs.m_pkey ),
          m_groupTsi( rhs.m_groupTsi ),
          m_locationCoverage( rhs.m_locationCoverage ),
          m_name( rhs.m_name )
    {
        FUNCTION_ENTRY( "TrainRadioGroup( TrainRadioGroup )" );
        FUNCTION_EXIT;
    }


    TrainRadioGroup::TrainRadioGroup( TA_Base_Core::IData& data, unsigned long& row )
        : m_uniqueIdentifier( ++s_uniqueIdentifier ),
          m_isNew( false ),
          m_isValidData( false ), // this will be set to true on a successful load
          m_pkey( 0 ),
          m_groupTsi( "" ),
          m_name( "" )
    {
        FUNCTION_ENTRY( "TrainRadioGroup( data, row )" );
        
        loadData( data, row );

        FUNCTION_EXIT;
    }

 
    std::vector< ITrainRadioGroupPtr > TrainRadioGroup::loadAllTrainRadioGroups()
    {
        FUNCTION_ENTRY( "loadAllTrainRadioGroups" );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Read );

        // create a query to load all rows from the 
        // TR_RADIO_GROUP_TABLE, TR_GROUP_COVERAGE_TABLE
        // the join criteria is the GROUP_PKEY_COLUMN of both tables order the query by 
        // the GROUP_PKEY_COLUMN (so all results for one group are grouped together)
      /*  std::ostringstream sql;
        sql << " SELECT A." << GROUP_PKEY_COLUMN 
            << ", A." << GROUP_TSI_COLUMN 
            << ", B." << LOCATION_COLUMN
            << " FROM " << TR_RADIO_GROUP_TABLE << " A, " << TR_GROUP_COVERAGE_TABLE << " B "
            << " WHERE A."<< GROUP_PKEY_COLUMN << " = B." << GROUP_PKEY_COLUMN 
            << " ORDER BY A." << GROUP_PKEY_COLUMN;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TR_RADIO_GROUP_STD_SELECT_60001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_SELECT_60001);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_SELECT_60001);

        // use the static getColumnNames to get the columns to select
        // This will result in a number of rows, each row will have the same
        // GROUP_PKEY_COLUMN, and GROUP_TSI_COLUMN, but different LOCATION_COLUMN
        // be sure to keep calling moreData until all data is returned, 
        // each time appending the new data to the existing IData object.
        
        std::vector< ITrainRadioGroupPtr > iTRGPtrList;
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, getColumnNames() );
         
        do
        {
            // for each row
            for ( unsigned long i = 0; i < data->getNumRows() ; /* incremented in TrainRadioGroup */ )
            {
                // extract the row into an object catch each one individually,
                // because one failure should not stop the rest loading
                try
                {
                    // create a new TrainRadioGroup object,giving it the IData reference,
                    // and the row index reference add the new object to the return vector
                    ITrainRadioGroupPtr iTRGPtr( new TrainRadioGroup( *data, i ) );
                    iTRGPtrList.push_back( iTRGPtr );
                }
                catch ( TA_Base_Core::DataException& de )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
                }
                catch ( TA_Base_Core::DatabaseException& dbe )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
                }
            }
            
            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData( data ) );

        // return the result vector
        FUNCTION_EXIT;
        return iTRGPtrList;
    }


    ITrainRadioGroupPtr TrainRadioGroup::loadTrainRadioGroup( unsigned long pkey ) 
    {
        FUNCTION_ENTRY( "loadTrainRadioGroup(pkey)" );

        // call loadDataByPkey
        TA_Base_Core::IData* data = loadDataByPkey( pkey );

        // create a new TrainRadioGroup object, giving it the
        // IData reference, and the row index reference
        unsigned long rowIndex = 0;

        // Initilize a shared pointer of ITrainRadioGroup
        ITrainRadioGroupPtr iTrainRadioGroupPtr( new TrainRadioGroup( *data, rowIndex ) );

        // delete the IData object
        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return iTrainRadioGroupPtr;
    }


    ITrainRadioGroupPtr TrainRadioGroup::loadTrainRadioGroup( const std::string& groupTsi ) 
    {
        FUNCTION_ENTRY( "loadTrainRadioGroup(groupTsi)" );

        // call loadDataByGroupTsi
        TA_Base_Core::IData* data = loadDataByGroupTsi( groupTsi );
        unsigned long rowIndex = 0;

        // create a new TrainRadioGroup object, giving it the 
        // IData reference, and the row index reference

        // Initilize a shared pointer of ITrainRadioGroup
        ITrainRadioGroupPtr iTrainRadioGroupPtr( new TrainRadioGroup( *data, rowIndex ) );

        // delete the IData object
        delete data;
        data = NULL;

        FUNCTION_EXIT;
        return iTrainRadioGroupPtr;
    }

 
    TA_Base_Core::IData* TrainRadioGroup::loadDataByPkey( unsigned long pkey ) 
    {
        FUNCTION_ENTRY( "loadDataByPkey" );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Read );

        // create a query to load all rows from the TR_RADIO_GROUP_TABLE,
        // TR_GROUP_COVERAGE_TABLE(join),where the GROUP_PKEY_COLUMN matches the 
        // given pkey the join criteria is the GROUP_PKEY_COLUMN of both tables

       /* std::ostringstream sql;
        sql << " SELECT A." <<GROUP_PKEY_COLUMN
            << ", A." << GROUP_TSI_COLUMN
            << ", B." << LOCATION_COLUMN
            << " FROM " << TR_RADIO_GROUP_TABLE << " A, " << TR_GROUP_COVERAGE_TABLE << " B "
            << " WHERE A."<< GROUP_PKEY_COLUMN  << " = B." << GROUP_PKEY_COLUMN   
            << " AND B." << GROUP_PKEY_COLUMN << " = " <<  pkey ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TR_RADIO_GROUP_STD_SELECT_60002, pkey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_SELECT_60002, pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_SELECT_60002, pkey);
        
        // use the static getColumnNames for executing the query
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, getColumnNames() );

        // This will result in a number of rows, each row will have the same GROUP_PKEY_COLUMN,
        // and GROUP_TSI_COLUMN, but different LOCATION_COLUMN ,be sure to keep calling moreData
        // until all data is returned,each time appending the new data to the existing IData object.
        
        // 11-9 note there is no more than 500 row data according to Adam, If there is any change, check the code later;

        FUNCTION_EXIT;
        return data;
    }


    TA_Base_Core::IData* TrainRadioGroup::loadDataByGroupTsi( const std::string& groupTsi ) 
    {
        FUNCTION_ENTRY( "loadDataByGroupTsi" );

        // create a query to load all rows from the TR_RADIO_GROUP_TABLE,
        // TR_GROUP_COVERAGE_TABLE(join),where the GROUP_TSI_COLUMN matches the 
        // given tsi, the join criteria is the GROUP_PKEY_COLUMN of both tables
        
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Read );

      /*  std::ostringstream sql;
        sql << " SELECT A." << GROUP_PKEY_COLUMN
            << ", A." << GROUP_TSI_COLUMN
            << ", B." << LOCATION_COLUMN
            << " FROM " << TR_RADIO_GROUP_TABLE << " A, " << TR_GROUP_COVERAGE_TABLE << " B "
            << " WHERE A."<< GROUP_PKEY_COLUMN  << " = B." << GROUP_PKEY_COLUMN   
            << " AND A." << GROUP_TSI_COLUMN << " = " <<  groupTsi ;*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), TR_RADIO_GROUP_STD_SELECT_60003, groupTsi);
//		std::string strSql  = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_SELECT_60003, groupTsi);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_SELECT_60003, groupTsi);
        
        // use the static getColumnNames for executing the query
        TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, getColumnNames() );

        // This will result in a number of rows, each row will have the same GROUP_PKEY_COLUMN,
        // and GROUP_TSI_COLUMN, but different LOCATION_COLUMN
        
        FUNCTION_EXIT;
        return data;
    }

    
    const std::vector<std::string>& TrainRadioGroup::getColumnNames()
    {
        FUNCTION_ENTRY( "getColumnNames" );

        TA_THREADGUARD(s_columnNamesLock);

        // if the s_columnNames vector is empty,populate it with all column names
        // GROUP_PKEY_COLUMN GROUP_TSI_COLUMN LOCATION_COLUMN
        if ( true == s_columnNames.empty() )
        {
            s_columnNames.push_back( GROUP_PKEY_COLUMN );
            s_columnNames.push_back( GROUP_TSI_COLUMN );
            s_columnNames.push_back( LOCATION_COLUMN );
        }

        FUNCTION_EXIT;        
        return s_columnNames;
    }


    const std::vector<std::string>& TrainRadioGroup::getRadioSubscriberColumnNames()
    {
        FUNCTION_ENTRY( "getRadioSubscriberColumnNames" );

        TA_THREADGUARD(s_columnNamesLock);

        // if the s_columnNames vector is empty,populate it with all column names
        // GROUP_PKEY_COLUMN GROUP_TSI_COLUMN LOCATION_COLUMN
        if ( true == s_radioSubscriberColumnNames.empty() )
        {
            s_radioSubscriberColumnNames.push_back( SUBSCRIBER_NAME_COLUMN );
        }

        FUNCTION_EXIT;        
        return s_radioSubscriberColumnNames;
    }


    unsigned long TrainRadioGroup::getKey()
    {
        FUNCTION_ENTRY( "getKey" );

		//This method cannot be called until the key has been set
		if ( !m_isValidData && !m_isNew )
		{
			reload();
		}
        
        
        if ( true == m_isNew )
        {
            // if this item is new, throw a DataException
            TA_THROW( TA_Base_Core::DataException( "Cannot get the key of a new item",
                                                   TA_Base_Core::DataException::NO_VALUE, "" ) );
        }

        // otherwise, return m_pkey
        FUNCTION_EXIT;
        return m_pkey;
    }


    std::string TrainRadioGroup::getName()
    {
        FUNCTION_ENTRY( "getName" );

		//Should not call reload if m_pkey is equal 0
		if ( !m_isValidData && !m_isNew )
		{
			reload();
		}
        
        FUNCTION_EXIT;
        return m_name;
    }

 
    std::string TrainRadioGroup::getTsi()
    {
        FUNCTION_ENTRY( "getTsi" );

        if ( !m_isValidData && !m_isNew  )
		{
			reload();
		}

        FUNCTION_EXIT;
        return m_groupTsi;
    }


    ITrainRadioGroup::LocationSet TrainRadioGroup::getLocationCoverage()
    {
        FUNCTION_ENTRY( "getLocationCoverage" );

        if ( !m_isValidData && !m_isNew )
		{
			reload();
		}

        FUNCTION_EXIT;  
        return m_locationCoverage;
    }


    void TrainRadioGroup::setName( const std::string& name )
    {
        FUNCTION_ENTRY( "setName" );

		//BUG : Should not call ta_throw
        //Throws a DataException - the name is not under the control of this object
//         TA_THROW( TA_Base_Core::DataException( "Cannot set the name, it is derived from the group TSI",
//                                                TA_Base_Core::DataException::INVALID_VALUE, name ) );

		if ( !m_isValidData && !m_isNew )
		{
			reload();
		}

		m_name = name;

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::setTsi( const std::string& tsi )
    {
        FUNCTION_ENTRY( "setTsi" );

        if ( !m_isValidData && !m_isNew )
		{
			reload();
		}
        
        if ( tsi == m_groupTsi )
        {
            FUNCTION_EXIT;
            return;
        }
        
        std::string oldValue( m_groupTsi );
        
        // sets m_groupTsi
        m_groupTsi = tsi;

        // clears m_name
        m_name = "";

		//Double check if the current tsi is not the same as the prevous one
		if ( oldValue.compare(tsi) != 0 )
		{
			// then set to true
			m_isNew = true;
		}

        // Using the utility method in ConfigChangesTypes, add an item to m_itemChanges
        // with the name GROUP_TSI_COLUMN, the data will be m_groupTsi
        TA_Base_Core::ConfigChangesTypes::updateChanges(m_itemChanges, GROUP_TSI_COLUMN, oldValue, m_groupTsi );

        // refresh the subscriber name field
        loadSubscriberName();

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::setLocationCoverage( const LocationSet& locations )
    {
        FUNCTION_ENTRY( "setLocationCoverage" );

        if ( !m_isValidData && !m_isNew  )
		{
			reload();
		}

        // set m_locationCoverage to locations
        m_locationCoverage = locations;

        // Using the utility method in ConfigChangesTypes, add an item to
        // m_itemChanges with the name LOCATION_COLUMN, the data doesn't matter,
        // it can contain a string such as "modified"
        TA_Base_Core::ConfigChangesTypes::updateChanges(m_itemChanges, LOCATION_COLUMN, "", "modified" );

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::addLocation( unsigned long location )
    {
        FUNCTION_ENTRY( "addLocation" );

        LocationSet::iterator ite = m_locationCoverage.find( location );
        
        if ( ite == m_locationCoverage.end() )
        {
            // add the location to m_locationCoverage
            m_locationCoverage.insert( location );

			m_isValidData = true;

            // Using the utility method in ConfigChangesTypes, add an item to
            // m_itemChanges with the name LOCATION_COLUMN, the data doesnt matter,
            // it can contain a string such as "modified"
            TA_Base_Core::ConfigChangesTypes::updateChanges(m_itemChanges, LOCATION_COLUMN, "", "modified" );
        }
        
        FUNCTION_EXIT;
    }


    void TrainRadioGroup::removeLocation( unsigned long location )
    {
        FUNCTION_ENTRY( "removeLocation" );

        // remove the location from m_locationCoverage
        LocationSet::iterator ite = m_locationCoverage.find( location );
        
        if ( ite != m_locationCoverage.end() )
        {
            m_locationCoverage.erase( ite );

			m_isValidData = true;
            //Using the utility method in ConfigChangesTypes, add an item to
            //m_itemChanges with the name LOCATION_COLUMN, the data doesnt matter,
            //it can contain a string such as "modified" 
            TA_Base_Core::ConfigChangesTypes::updateChanges( m_itemChanges,
                                                             LOCATION_COLUMN, 
                                                             "", 
                                                             "modified" );
        }
        
        FUNCTION_EXIT;
    }

 
    void TrainRadioGroup::invalidate()
    {
        FUNCTION_ENTRY( "invalidate" );


		// sets m_validData to false
		m_isValidData = false;

    
		// clears m_name
		m_name = "";

		// clears m_itemChanges
		m_itemChanges.clear();  
			

        FUNCTION_EXIT;
    }


    unsigned long TrainRadioGroup::getUniqueIdentifier()
    {
        FUNCTION_ENTRY( "getUniqueIdentifier" );

        FUNCTION_EXIT;
        return m_uniqueIdentifier;
    }


    TA_Base_Core::ItemChanges TrainRadioGroup::getAllItemChanges()
    {
        FUNCTION_ENTRY( "getAllItemChanges" );

        FUNCTION_EXIT;
        return m_itemChanges;
    }


    bool TrainRadioGroup::hasChanged()
    {
        FUNCTION_ENTRY( "hasChanged" );

        FUNCTION_EXIT;
        return !m_itemChanges.empty();
    }


    bool TrainRadioGroup::isNew()
    {
        FUNCTION_ENTRY( "isNew" );

        FUNCTION_EXIT;
        return m_isNew;
    }


    void TrainRadioGroup::applyChanges()
    {
        FUNCTION_ENTRY( "applyChanges" );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Write );
		//int nDbServerType = databaseConnection->GetDbServerType();

        if ( true == m_isNew )
        {
            // if this item is new generate a new pkey using the
            // GROUP_PKEY_SEQUENCE, store it in m_pkey
            // insert a new record into TR_RADIO_GROUP_TABLE

           /* std::string fieldName = GROUP_PKEY_SEQUENCE + ".nextval";

            std::ostringstream sql;
            sql << "select "<< fieldName << " from dual";*/
			//std::string strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_Oracle_SELECT_60051);
//			std::string strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_SELECT_60051);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_SELECT_60051);

            std::vector<std::string> pkeyColumn;
            pkeyColumn.push_back( GROUP_PKEY_SEQUENCE );
            
            TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, pkeyColumn );
            
            m_pkey = data->getUnsignedLongData( 0, GROUP_PKEY_SEQUENCE );

           /* sql.clear();
            sql << "INSERT INTO "
                << TR_RADIO_GROUP_TABLE
                << " ( " << GROUP_PKEY_COLUMN << ", " << GROUP_TSI_COLUMN << " ) "
                << " VALUES (" << m_pkey << ", '" << m_groupTsi << "' )";*/
			//strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_INSERT_60301, m_pkey, m_groupTsi);
//			strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_INSERT_60301, m_pkey, m_groupTsi);

		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_INSERT_60301, m_pkey, m_groupTsi);
            
            // execute insertion
            databaseConnection->executeModification( strSql );

            // sets m_isNew to false
            m_isNew = false;
        }
        else
        {
            // update the m_groupTsi in the TR_RADIO_GROUP_TABLE 
            // where the GROUP_PKEY_COLUMN matches m_pkey

          /*  std::ostringstream sql;
            sql << "UPDATE " << TR_RADIO_GROUP_TABLE
                << " SET " << GROUP_TSI_COLUMN << " = '" << m_groupTsi
                << "' WHERE " << GROUP_PKEY_COLUMN << " = " << m_pkey;*/
			//std::string strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_UPDATE_60151, m_groupTsi, m_pkey);
//			std::string strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_UPDATE_60151, m_groupTsi, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_UPDATE_60151, m_groupTsi, m_pkey);

            // execute update
            databaseConnection->executeModification( strSql );
        }

        // now set the locations
        // delete all entries from the TR_GROUP_COVERAGE_TABLE 
        // where the GROUP_PKEY_COLUMN matches m_pkey 

      /*  std::ostringstream sql;
        sql << "DELETE FROM " << TR_GROUP_COVERAGE_TABLE 
            << " WHERE " << GROUP_PKEY_COLUMN << " = " << m_pkey;*/
		//std::string strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_DELETE_60401, m_pkey);
//		std::string strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_DELETE_60401, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_DELETE_60402, m_pkey);
        
        // execute deletion
        databaseConnection->executeModification( strSql );

        // for each item in m_locationCoverage, insert a new entry in the
        // TR_GROUP_COVERAGE_TABLE, with m_pkey and the location
        for ( LocationSet::iterator ite = m_locationCoverage.begin();
              ite != m_locationCoverage.end();
              ite++ )
        {
            /*sql.clear();
            sql << "INSERT INTO " << TR_GROUP_COVERAGE_TABLE
                << " ( " << GROUP_PKEY_COLUMN << ", " << LOCATION_COLUMN << " ) "
                << " values ( " << m_pkey << ", " << *ite << ")";*/
			//strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_INSERT_60302, m_pkey, *ite);
//			strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_INSERT_60302, m_pkey, *ite);

		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_INSERT_60302, m_pkey, *ite);
            
            databaseConnection->executeModification( strSql );
        }
        
        // sets m_isValidData to true
        m_isValidData = true;

        // clears m_itemChanges
        m_itemChanges.clear();

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::deleteThisItem()
    {
        FUNCTION_ENTRY( "deleteThisItem" );

        // if this item is new, return
        if ( m_isNew == true )
        {
            FUNCTION_EXIT;
            return;
        }

        // delete all entries from the TR_GROUP_COVERAGE table 
        // where the GROUP_PKEY_COLUMN is the same as m_pkey

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection =
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Write );
		//int nDbServerType = databaseConnection->GetDbServerType();

       /* std::ostringstream sql;
        sql << "DELETE FROM " << TR_GROUP_COVERAGE_TABLE
            << " WHERE " << GROUP_PKEY_COLUMN << " = " << m_pkey;*/
		//std::string strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_DELETE_60402, m_pkey);
//		std::string strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_DELETE_60402, m_pkey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_DELETE_60402, m_pkey);
        
        databaseConnection->executeModification( strSql );

        // then delete from the TR_RADIO_GROUP_TABLE where the
        // GROUP_PKEY_COLUMN is the same as m_pkey
      /*  sql.clear();
        sql << "DELETE FROM " << TR_RADIO_GROUP_TABLE
            << " WHERE " << GROUP_PKEY_COLUMN << " = " << m_pkey;*/
		//strSql = defPrepareSQLStatement(nDbServerType, TR_RADIO_GROUP_STD_DELETE_60401, m_pkey);
//		strSql = databaseConnection->prepareSQLStatement(TR_RADIO_GROUP_DELETE_60401, m_pkey);

		databaseConnection->prepareSQLStatement(strSql, TR_RADIO_GROUP_DELETE_60401, m_pkey);

        databaseConnection->executeModification( strSql );

        // set m_isNew to true 
        m_isNew = true;

        // set m_isValidData to true
        //m_isValidData = true;

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::reload()
    {
        FUNCTION_ENTRY( "reload" );

        // if the item is not new, and the item is not valid data
        // clear m_groupTsi, m_name, and m_locationCoverage
        if ( ( false == m_isNew ) &&
             ( false == m_isValidData ) )
        {
            m_groupTsi = "";
            m_name = "";
            m_locationCoverage.clear();            
        }

        // use the static loadDataByPkey pass the data 
        // onto loadData delete the returned data
        //( also be sure to delete it in the case of any exceptions)
        TA_Base_Core::IData* data = loadDataByPkey( m_pkey );
        
        try
        {
            unsigned long rowIndex = 0;
            loadData( *data, rowIndex );
        }
        catch( ... )
        {
            delete data;
            data = NULL;
            
            throw;
        }

        delete data;
        data = NULL;

        // regardless of whether this is new or not, 
        // the subscriber name should always be loaded
        loadSubscriberName();

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::loadData( TA_Base_Core::IData& data,
                                    unsigned long& row )
    {
        FUNCTION_ENTRY( "loadData" );

        // This will load each row of the data, the first row will set m_groupTsi,
        // each row will add a location key to m_locationCoverage.

        // Get PKEY
        m_pkey = data.getUnsignedLongData( row, GROUP_PKEY_COLUMN );

        // Get Group_tsi
        m_groupTsi = data.getStringData( row, GROUP_TSI_COLUMN );

        // clear m_locationCoverage
        m_locationCoverage.clear();
        
        while ( row < data.getNumRows() )
        {
            if ( m_pkey != data.getUnsignedLongData( row, GROUP_PKEY_COLUMN ) )
            {
                break; //exit the loop, this is a different group record
            }

            m_locationCoverage.insert( data.getUnsignedLongData( row, LOCATION_COLUMN ) );
            
            ++row;
        }

        FUNCTION_EXIT;
    }


    void TrainRadioGroup::loadSubscriberName()
    {
		FUNCTION_ENTRY( "loadSubscriberName" );

		////if m_name is not empty, return
		//if ( false == m_name.empty() )
		//{
		//    FUNCTION_EXIT;
		//    return;
		//}

		//// Executes a query to load SUBSCRIBER_NAME_COLUMN from
		//// RA_SUBSCRIBERS_TABLE where SUBSCRIBER_ID_COLUMN is m_groupTsi
		//
		//// get a connection to the database
		//TA_Base_Core::IDatabase* databaseConnection =
		//    TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Radio_Cd, TA_Base_Core::Read );

		//std::ostringstream sql;
		//sql << "SELECT " << SUBSCRIBER_NAME_COLUMN
		//    << " FROM " << RA_SUBSCRIBERS_TABLE 
		//    << " WHERE " << SUBSCRIBER_ID_COLUMN << " = '" << m_groupTsi << "'";

		//TA_Base_Core::IData* data = NULL;

		//// if there is a result, populate m_name with the
		//// subscriber name, otherwise, populate it with m_groupTsi
		//
		//// extract the row into an object catch each one individually,
		////  because one failure should not stop the rest loading
		//try
		//{
		//    data = databaseConnection->executeQuery( sql.str(), getRadioSubscriberColumnNames() );
		//    
		//    if ( data->getNumRows() > 0 )
		//    {
		//        m_name = data->getStringData( 0, SUBSCRIBER_NAME_COLUMN );
		//    }
		//    else
		//    {
		//        m_name = m_groupTsi;
		//    }
		//}
		////catch all exceptions, and if there are errors, use m_groupTsi as m_name
		//catch ( TA_Base_Core::DataException& de )
		//{
		//    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
		//    m_name = m_groupTsi;
		//}
		//catch ( TA_Base_Core::DatabaseException& dbe )
		//{
		//    LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
		//    m_name = m_groupTsi;
		//}
		//
		//delete data;
		//data = NULL;        

		// DTL, GROUP_TSI_COLUMN itself contain the group name
		m_name = m_groupTsi;

		FUNCTION_EXIT;
    }

}
