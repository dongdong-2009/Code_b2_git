/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  */
#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsDssPlanMapAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/ConfigAtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/IndexedAtsDssPlanMaps.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string KEY_COL			= "ATDPMA_ID";
	static const std::string DSSEVENT_COL		= "DSS_EVENT";
	static const std::string PLANPATH_COL		= "PLAN_PATH";

    AtsDssPlanMapAccessFactory* AtsDssPlanMapAccessFactory::m_instance = 0;

    AtsDssPlanMapAccessFactory& AtsDssPlanMapAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AtsDssPlanMapAccessFactory();
        }
        return *m_instance;
    }


    void AtsDssPlanMapAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IAtsDssPlanMap* AtsDssPlanMapAccessFactory::getAtsDssPlanMap(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the AtsDssPlanMap
        // this is a check to ensure an AtsDssPlanMap with the specified
        // PKEY actually exists.
       /* char sql[256];
		sprintf(sql,"select %s from AT_DSS_PLAN_MAP where %s = %lu",KEY_COL.c_str(), KEY_COL.c_str(), idKey);*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_SELECT_46501, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46501, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46501, idKey);

        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "No data found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256];
			sprintf(reasonMessage, "More than one entry found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the AtsDssPlanMap object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IAtsDssPlanMap* theAtsDssPlanMap;
        
        // decide which AtsDssPlanMap object to return
        if (readWrite) // need a config AtsDssPlanMap
        {
            theAtsDssPlanMap = new ConfigAtsDssPlanMap(idKey);
        }
        else // need a standard AtsDssPlanMap
        {
            theAtsDssPlanMap = new AtsDssPlanMap(idKey);
        }

        // Return the AtsDssPlanMap pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theAtsDssPlanMap;
    }
    
    IAtsDssPlanMaps AtsDssPlanMapAccessFactory::getAllAtsDssPlanMaps( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the AtsDssPlanMap
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << DSSEVENT_COL << ", " 
			<< PLANPATH_COL << " from AT_DSS_PLAN_MAP";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_SELECT_46502);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46502);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46502);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(DSSEVENT_COL);
		columnNames.push_back(PLANPATH_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsDssPlanMaps AtsDssPlanMapPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsDssPlanMapPointerVector.push_back( new ConfigAtsDssPlanMap(i, *data));
                    }
                    else
                    {
                        AtsDssPlanMapPointerVector.push_back( new AtsDssPlanMap(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsDssPlanMapPointerVector;
    }
    
  
    IAtsDssPlanMaps AtsDssPlanMapAccessFactory::getAtsDssPlanMapsWhere( const bool readWrite,
																  long dssEvent,
																  const char* const planPath)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		/*sql << "select " << KEY_COL << ", " << DSSEVENT_COL << ", " 
			<< PLANPATH_COL << " from AT_DSS_PLAN_MAP where 1=1 ";*/
            
		if (dssEvent != -1)
        {
            sql << "and " << DSSEVENT_COL << " = " 
                << dssEvent
                << " ";
        }		
        if (planPath != NULL)
        {
            sql << "and " << PLANPATH_COL << " = '" 
                << databaseConnection->escapeQueryString(planPath)
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_SELECT_46503, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46503, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46503, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(DSSEVENT_COL);
		columnNames.push_back(PLANPATH_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsDssPlanMaps AtsDssPlanMapPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsDssPlanMapPointerVector.push_back( new ConfigAtsDssPlanMap(i, *data));
                    }
                    else
                    {
                        AtsDssPlanMapPointerVector.push_back( new AtsDssPlanMap(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsDssPlanMapPointerVector;
    }


    std::vector<int> AtsDssPlanMapAccessFactory::getAtsDssPlanMapIdsWhere
		( long dssEvent /* = -1 */, const char* const planPath /* = NULL */)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << " from AT_DSS_PLAN_MAP where 1=1 ";

        if (dssEvent != -1)
        {
            sql << "and " << DSSEVENT_COL << " = " 
                << dssEvent
                << " ";
        }
        if (planPath != NULL)
        {
            sql << "and " << PLANPATH_COL << " = '" 
                << databaseConnection->escapeQueryString(planPath)
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_DSS_PLAN_MAP_STD_SELECT_46504, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_DSS_PLAN_MAP_SELECT_46504, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_DSS_PLAN_MAP_SELECT_46504, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);


        // create the vector of keys
        std::vector<int> zoneIds;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                zoneIds.push_back(data->getUnsignedLongData(i, KEY_COL));
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return zoneIds;
    }

	IndexedAtsDssPlanMaps* AtsDssPlanMapAccessFactory::getIndexedAtsDssPlanMaps()
	{
		IAtsDssPlanMaps dssPlanMaps = getAllAtsDssPlanMaps(false);

		return new IndexedAtsDssPlanMaps(dssPlanMaps);
	}
    

    IConfigAtsDssPlanMap* AtsDssPlanMapAccessFactory::createAtsDssPlanMap()
    {            
        return new ConfigAtsDssPlanMap();    
    }


    IConfigAtsDssPlanMap* AtsDssPlanMapAccessFactory::copyAtsDssPlanMap(const IConfigAtsDssPlanMap* AtsDssPlanMapToCopy)
    {
        FUNCTION_ENTRY("copyAtsDssPlanMap");

        TA_ASSERT(AtsDssPlanMapToCopy !=  NULL, "The AtsDssPlanMap to copy was NULL");
        
        const ConfigAtsDssPlanMap* cast = dynamic_cast<const ConfigAtsDssPlanMap*>(AtsDssPlanMapToCopy);

        TA_ASSERT(cast != NULL, "AtsDssPlanMap passed could not be converted into a ConfigAtsDssPlanMap");

        FUNCTION_EXIT;
        return new ConfigAtsDssPlanMap(*cast);

    }


} // closes TA_Core


