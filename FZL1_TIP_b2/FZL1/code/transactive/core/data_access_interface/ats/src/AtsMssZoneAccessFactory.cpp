/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ats/src/AtsMssZoneAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsMssZone.h"
#include "core/data_access_interface/ats/src/ConfigAtsMssZone.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string KEY_COL		= "ATMSSZ_ID";
    static const std::string STATIONID_COL	= "STATION_ID";
	static const std::string MSSZONE_COL	= "MSS_ZONE";

    AtsMssZoneAccessFactory* AtsMssZoneAccessFactory::m_instance = 0;

    AtsMssZoneAccessFactory& AtsMssZoneAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AtsMssZoneAccessFactory();
        }
        return *m_instance;
    }


    void AtsMssZoneAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IAtsMssZone* AtsMssZoneAccessFactory::getAtsMssZone(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the AtsMssZone
        // this is a check to ensure an AtsMssZone with the specified
        // PKEY actually exists.
        /*char sql[256] = {0};
		sprintf(sql,"select %s from AT_MSSZONE where %s = %lu",KEY_COL.c_str(), KEY_COL.c_str(), idKey);	*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_SELECT_48002, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48002, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48002, idKey);

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

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No data found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            // clean up the pointer
            delete data;
            data = NULL;

            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one entry found for id = %lu",idKey);			
                TA_THROW(DataException(reasonMessage,DataException::NOT_UNIQUE,""));
        }

        // Don't need to do anything with the data - the check was only that a single
        // item was returned.
        // Now that we're finished with the IData object, we need to delete it.
        delete data;
        data = NULL;

        // Create the AtsMssZone object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IAtsMssZone* theAtsMssZone;
        
        // decide which AtsMssZone object to return
        if (readWrite) // need a config AtsMssZone
        {
            theAtsMssZone = new ConfigAtsMssZone(idKey);
        }
        else // need a standard AtsMssZone
        {
            theAtsMssZone = new AtsMssZone(idKey);
        }

        // Return the AtsMssZone pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theAtsMssZone;
    }
    
    IAtsMssZones AtsMssZoneAccessFactory::getAllAtsMssZones( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the AtsMssZone
        // based upon the key
        /*std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << MSSZONE_COL << " from AT_MSSZONE";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_SELECT_48001);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48001);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48001);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(STATIONID_COL);
		columnNames.push_back(MSSZONE_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsMssZones AtsMssZonePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsMssZonePointerVector.push_back( new ConfigAtsMssZone(i, *data));
                    }
                    else
                    {
                        AtsMssZonePointerVector.push_back( new AtsMssZone(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsMssZonePointerVector;
    }
    
  
    IAtsMssZones AtsMssZoneAccessFactory::getAtsMssZonesWhere( const bool readWrite,
															   long stationId, 
															   const std::string& mssZone )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << ", " << STATIONID_COL << ", " << MSSZONE_COL << " from AT_MSSZONE where 1=1 ";
            
		if (stationId != -1)
        {
            sql << "and " << STATIONID_COL << " = " 
                << stationId
                << " ";
        }

        if (!mssZone.empty())
        {
            sql << "and " << MSSZONE_COL << " = '" 
                << (databaseConnection->escapeInsertString(mssZone)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_SELECT_48003, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48003, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48003, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(STATIONID_COL);
		columnNames.push_back(MSSZONE_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsMssZones AtsMssZonePointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsMssZonePointerVector.push_back( new ConfigAtsMssZone(i, *data));
                    }
                    else
                    {
                        AtsMssZonePointerVector.push_back( new AtsMssZone(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsMssZonePointerVector;
    }


    std::vector<int> AtsMssZoneAccessFactory::getAtsMssZoneIdsWhere
        ( long stationId, const std::string& mssZone  )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << " from AT_MSSZONE where 1=1 ";

        if (stationId != -1)
        {
            sql << "and " << STATIONID_COL << " = " 
                << stationId
                << " ";
        }

        if (!mssZone.empty())
        {
            sql << "and " << MSSZONE_COL << " = '" 
                << (databaseConnection->escapeInsertString(mssZone)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_MSSZONE_STD_SELECT_48004, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_MSSZONE_SELECT_48004, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_MSSZONE_SELECT_48004, sql.str());

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



    IConfigAtsMssZone* AtsMssZoneAccessFactory::createAtsMssZone()
    {            
        return new ConfigAtsMssZone();    
    }


    IConfigAtsMssZone* AtsMssZoneAccessFactory::copyAtsMssZone(const IConfigAtsMssZone* AtsMssZoneToCopy)
    {
        FUNCTION_ENTRY("copyAtsMssZone");

        TA_ASSERT(AtsMssZoneToCopy !=  NULL, "The AtsMssZone to copy was NULL");
        
        const ConfigAtsMssZone* cast = dynamic_cast<const ConfigAtsMssZone*>(AtsMssZoneToCopy);

        TA_ASSERT(cast != NULL, "AtsMssZone passed could not be converted into a ConfigAtsMssZone");

        FUNCTION_EXIT;
        return new ConfigAtsMssZone(*cast);

    }


} // closes TA_Core


