/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/RadioBtnCfgFactory.cpp $
  * @author:  Mark See
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ 
  * 
  * 1: modified the singleton instance implementation due to the thread-safe calling
  *    Implementation of the Class RadioBtnCfgFactory
  * 2: modified the order clause with the colomun name in function getAllRadioBtnCfg()
  */

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/data_access_interface/radio/src/RadioBtnCfgFactory.h"
#include "core/data_access_interface/radio/src/RadioBtnCfg.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/synchronisation/src/ReadWriteThreadLockable.h"

namespace
{
	//Haipeng changed those codes to classes attributes in following 13 lines.
    static const int INVALID_VALUE = -1;
    static const char INVALID_RESOURCE_TYPE = ' ';
    
    //Table Name
    static const std::string    BTNCFG_TBL = "ra_btncfg";        
    
    //Column Names in the table
    static const std::string    PKEY_COL = "PKEY";
    static const std::string    OPERATOR_PROFILE_COL = "OPERATOR_PROFILE";
    static const std::string    IS_PROFILE_COL = "IS_PROFILE";
    static const std::string    BUTTONREF_COL = "BUTTONREF";
    static const std::string    RESOURCE_TYPE_COL = "RESOURCE_TYPE";
    static const std::string    RESOURCE_ID_COL = "RESOURCE_ID";
}

namespace TA_IRS_Core
{
    RadioBtnCfgFactory* RadioBtnCfgFactory::m_instance = NULL;

    RadioBtnCfgFactory::RadioBtnCfgFactory()
    {
    }
    
    RadioBtnCfgFactory::~RadioBtnCfgFactory()
    {
    }

    RadioBtnCfgFactory& RadioBtnCfgFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

		//using double-check to ensure the thread-safe singleton:
		if (m_instance == NULL)
		{
			//using the readwrite thread object to ensure the thread-safe.
			TA_Base_Core::ReadWriteThreadLockable readlock;
			readlock.acquire();

			if(m_instance == NULL)
			{
				m_instance = new RadioBtnCfgFactory();
			}
			readlock.release();
		}        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void RadioBtnCfgFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    IRadioBtnCfg* RadioBtnCfgFactory::getRadioBtnCfg(const unsigned long key)
    {
        FUNCTION_ENTRY("getRadioBtnCfg");
    
        TA_ASSERT(key > 0, "Invalid Radio Button Profile Configuration Id index.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }

        // create the SQL string to retrieve the name of the Train
        // this is a check to ensure an Train with the specified key actually exists.        
//      	std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_SELECT_66001, key);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_SELECT_66001, key);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(PKEY_COL);	

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));
        
        if (data->getNumRows() == 0)
        {
            std::ostringstream reason;
		    reason << "No record found for RadioBtnCfg with key = " << key;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Invalid RadioBtnCfg primary key." ) );
        }

        // Don't need to do anything with the data, only needed to check if it exists
        TA_ASSERT(data->getNumRows() == 1, "Invalid number of records.");        

        // Create the Train object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IRadioBtnCfg* theRadioBtnCfg = NULL;

        theRadioBtnCfg = new RadioBtnCfg(key);

        FUNCTION_EXIT;
        return theRadioBtnCfg;
    }

    std::vector<IRadioBtnCfg*> RadioBtnCfgFactory::getAllRadioBtnCfg(const std::string& sessionId)
    {
        FUNCTION_ENTRY("getAllRadioBtnCfg");

        TA_Base_Core::ISession* session = NULL;
        
        unsigned long operatorId = 0;
        unsigned long profileId = 0;

        try
        {
            session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
        
            operatorId = session->getOperatorKey();
            profileId = session->getProfileKey();
        }
        catch (...)
        {
            delete session;
            throw;
        }

        delete session;
        session = NULL;


        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }


        // create the SQL string to retrieve all the records from the table 
        // ordered by isProfile and operator/profile id
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_SELECT_66002, operatorId, profileId);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_SELECT_66002, operatorId, profileId);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
        columnNames.push_back(OPERATOR_PROFILE_COL);
        columnNames.push_back(IS_PROFILE_COL);
		columnNames.push_back(BUTTONREF_COL);
        columnNames.push_back(RESOURCE_TYPE_COL);       
        columnNames.push_back(RESOURCE_ID_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
        
        // create the vector of pointers to IRadioBtnCfg
        std::vector<IRadioBtnCfg*> radioPtrVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);
                radioPtrVector.push_back(new RadioBtnCfg(i,*data));
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return radioPtrVector;
    }

	std::vector<IRadioBtnCfg*> RadioBtnCfgFactory::getAllRadioBtnCfgByProfile(unsigned long profileKey)
	{
		FUNCTION_ENTRY("getAllRadioBtnCfgByProfile");


        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Read);

        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }


        // create the SQL string to retrieve all the records from the table 
        // ordered by isProfile and operator/profile id
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;

        columnNames.push_back(PKEY_COL);
        columnNames.push_back(OPERATOR_PROFILE_COL);
        columnNames.push_back(IS_PROFILE_COL);
		columnNames.push_back(BUTTONREF_COL);
        columnNames.push_back(RESOURCE_TYPE_COL);       
        columnNames.push_back(RESOURCE_ID_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		//std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_SELECT_66008,profileKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_SELECT_66008,profileKey);
		
		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);
		        
        // create the vector of pointers to IRadioBtnCfg
        std::vector<IRadioBtnCfg*> radioPtrVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows(); ++i )
            {   
                unsigned long key = data->getUnsignedLongData(i, PKEY_COL);
                radioPtrVector.push_back(new RadioBtnCfg(i,*data));
            }

            delete data;
            data = NULL;
        }
        while(databaseConnection->moreData(data) == true);

        FUNCTION_EXIT;
        return radioPtrVector;
	}
    

    void RadioBtnCfgFactory::associateButtonWithResource(const std::string& sessionId, bool isProfile, unsigned long btnref,  const RadioResource&  resource)
    {
        TA_Base_Core::ISession* session = NULL;
        
        unsigned long profileId = 0;
        
        try
        {
            session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);

            if (isProfile)
            {
                profileId = session->getProfileKey();
            }
            else
            {
                profileId = session->getOperatorKey();
            }
        }
        catch (...)
        {
            delete session;
            throw;
        }
        
        delete session;
        session = NULL;

        try
        {
            RadioBtnCfgHelper helper(isProfile, profileId, btnref, resource);
            helper.writeButtonData();
        }
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to clear button with owner id: %d.", profileId);
            throw;  
        }
    }

    void RadioBtnCfgFactory::clearButtonRef(const std::string& sessionId, bool isProfile, unsigned long btnref)
    {
        TA_Base_Core::ISession* session = NULL;
        
        unsigned long profileId = 0;
        
        try
        {
            session  = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
            
            if (isProfile)
            {
                profileId = session->getProfileKey();
            }
            else
            {
                profileId = session->getOperatorKey();
            }
        }
        catch (...)
        {
            delete session;
            throw;
        }
        
        delete session;
        session = NULL;
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }

//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_DELETE_66004,getLocalDatabaseName(),profileId, isProfile, btnref);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_DELETE_66004,getLocalDatabaseName(),profileId, isProfile, btnref);
        databaseConnection->executeModification( strSql );

    }

	void RadioBtnCfgFactory::insertPredefinedTG(unsigned long profileKey, unsigned long btnRef, const RadioResource& resource)
	{		
		try 
		{
			RadioBtnCfgHelper helper(true, profileKey, btnRef, resource);
			helper.writeButtonData();
		}
        catch (...)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                "Error catched while trying to clear button with owner id: %d.", profileKey);
            throw;  
        }
	}

	void RadioBtnCfgFactory::updatePredefinedTG(unsigned long profileKey, unsigned long btnRef, const RadioResource& resource)
	{	
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }
       
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_UPDATE_66008,getLocalDatabaseName(), resource.id, profileKey,btnRef);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_UPDATE_66008,getLocalDatabaseName(), resource.id, profileKey,btnRef);
		databaseConnection->executeModification(strSql);

	}

	void RadioBtnCfgFactory::deletePredefinedTG(unsigned long profileKey)
	{		
        
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
        
        if (databaseConnection == NULL)
        {
            TA_THROW(TA_Base_Core::DatabaseException("no database"));
        }
       
//		std::string strSql = databaseConnection->prepareSQLStatement(RA_BTNCFG_DELETE_66006,getLocalDatabaseName(), profileKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, RA_BTNCFG_DELETE_66006,getLocalDatabaseName(), profileKey);
		databaseConnection->executeModification(strSql);

	}

	std::string RadioBtnCfgFactory::getLocalDatabaseName()
	{
		std::string localDatabaseName;    
		if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw TA_Base_Core::DatabaseException("db-connection-file not set");
		}
		try
		{
			TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Radio_Cd, TA_Base_Core::Write);
			localDatabaseName = connections[0].first;  
		}
		catch(...) // most likely a file not found error
		{
			throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

}
