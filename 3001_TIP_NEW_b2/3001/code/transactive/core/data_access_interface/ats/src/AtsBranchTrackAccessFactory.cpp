/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER


#include "core/data_access_interface/ats/src/AtsBranchTrackAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsBranchTrack.h"
#include "core/data_access_interface/ats/src/ConfigAtsBranchTrack.h"

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SQLCode.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DataException;

namespace TA_IRS_Core
{
    static const std::string KEY_COL = "ATBTRA_ID";
    static const std::string TRACK_COL = "TRACK";

    AtsBranchTrackAccessFactory* AtsBranchTrackAccessFactory::m_instance = 0;

    AtsBranchTrackAccessFactory& AtsBranchTrackAccessFactory::getInstance()
    {
        if( m_instance == 0 )
        {
            m_instance = new AtsBranchTrackAccessFactory();
        }
        return *m_instance;
    }


    void AtsBranchTrackAccessFactory::removeInstance( )
    {
	    if ( m_instance != NULL )
	    {
		    delete m_instance;
		    m_instance = NULL;
	    }
    }
    
    
    IAtsBranchTrack* AtsBranchTrackAccessFactory::getAtsBranchTrack(const unsigned long idKey,const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the name of the AtsBranchTrack
        // this is a check to ensure an AtsBranchTrack with the specified
        // PKEY actually exists.
       /* char sql[256];
		sprintf(sql,"select %s from AT_BRANCH_TRACK where %s = %lu",KEY_COL.c_str(), KEY_COL.c_str(), idKey);	*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46001, idKey);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46001, idKey);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46001, idKey);

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

        // Create the AtsBranchTrack object to represent this object. Initially only populate it
        // with id. We will populate the rest of the data on demand.
        IAtsBranchTrack* theAtsBranchTrack;
        
        // decide which AtsBranchTrack object to return
        if (readWrite) // need a config AtsBranchTrack
        {
            theAtsBranchTrack = new ConfigAtsBranchTrack(idKey);
        }
        else // need a standard AtsBranchTrack
        {
            theAtsBranchTrack = new AtsBranchTrack(idKey);
        }

        // Return the AtsBranchTrack pointer. The class that recieves this pointer is responsible
        // for deleting it.
        return theAtsBranchTrack;
    }
    
    IAtsBranchTracks AtsBranchTrackAccessFactory::getAllAtsBranchTracks( const bool readWrite)
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the data of the AtsBranchTrack
        // based upon the key
       /* std::ostringstream sql;
		sql << "select " << KEY_COL << ", " << TRACK_COL << " from AT_BRANCH_TRACK";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46002);
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46002);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46002);
 
        // Set up the columnNames vector to be passed to executeQuery()

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(TRACK_COL);
        
        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsBranchTracks AtsBranchTrackPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsBranchTrackPointerVector.push_back( new ConfigAtsBranchTrack(i, *data));
                    }
                    else
                    {
                        AtsBranchTrackPointerVector.push_back( new AtsBranchTrack(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsBranchTrackPointerVector;
    }
    
  
    IAtsBranchTracks AtsBranchTrackAccessFactory::getAtsBranchTracksWhere( const bool readWrite,
                                                      const std::string& track )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << ", " << TRACK_COL << " from AT_BRANCH_TRACK where 1=1 ";
            
        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46003, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46003, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46003, sql.str());

        std::vector<std::string> columnNames;
        columnNames.push_back(KEY_COL);
		columnNames.push_back(TRACK_COL);

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
        TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);
        
        // create the vector of pointers to IEntityData
        IAtsBranchTracks AtsBranchTrackPointerVector;

        do
        {
            for (unsigned long i = 0; i < data->getNumRows() ; ++i )
            {   
                unsigned long id = data->getUnsignedLongData(i,KEY_COL);
                if (id != 0)
                {
                    if (readWrite)
                    {
                        AtsBranchTrackPointerVector.push_back( new ConfigAtsBranchTrack(i, *data));
                    }
                    else
                    {
                        AtsBranchTrackPointerVector.push_back( new AtsBranchTrack(i, *data));
                    }
                }
            }

            delete data;
            data = NULL;
        }
        while( databaseConnection->moreData(data) );

        return AtsBranchTrackPointerVector;
    }


    std::vector<int> AtsBranchTrackAccessFactory::getAtsBranchTrackIdsWhere
        ( const std::string& track )
    {
        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

        // create the SQL string to retrieve the guis
        std::ostringstream sql;
		//sql << "select " << KEY_COL << " from AT_BRANCH_TRACK where 1=1 ";

        if (!track.empty())
        {
            sql << "and TRACK = '" 
                << (databaseConnection->escapeInsertString(track)).c_str()
                << "' ";
        }
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), AT_BRANCH_TRACK_STD_SELECT_46004, sql.str());
//		std::string strSql  = databaseConnection->prepareSQLStatement(AT_BRANCH_TRACK_SELECT_46004, sql.str());
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, AT_BRANCH_TRACK_SELECT_46004, sql.str());

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



    IConfigAtsBranchTrack* AtsBranchTrackAccessFactory::createAtsBranchTrack()
    {            
        return new ConfigAtsBranchTrack();    
    }


    IConfigAtsBranchTrack* AtsBranchTrackAccessFactory::copyAtsBranchTrack(const IConfigAtsBranchTrack* AtsBranchTrackToCopy)
    {
        FUNCTION_ENTRY("copyAtsBranchTrack");

        TA_ASSERT(AtsBranchTrackToCopy !=  NULL, "The AtsBranchTrack to copy was NULL");
        
        const ConfigAtsBranchTrack* cast = dynamic_cast<const ConfigAtsBranchTrack*>(AtsBranchTrackToCopy);

        TA_ASSERT(cast != NULL, "AtsBranchTrack passed could not be converted into a ConfigAtsBranchTrack");

        FUNCTION_EXIT;
        return new ConfigAtsBranchTrack(*cast);

    }


} // closes TA_Core


