/**
  * 
  *
  * Implementation of the Class TrainAccessFactory
  */

#pragma warning(disable:4786 4290)

#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/TrainStateDai/src/TrainAccessFactory.h"
#include "core/data_access_interface/TrainStateDai/src/Train.h"



namespace TA_IRS_Core
{
    //
    // Constants
    //
     
    static const std::string TRTRAIN_TBL("TR_TRAIN");
    static const std::string TRAINDID_COL("TRAIN_ID");
    static const std::string PRIMARY_TSI_COL("PRIMARY_TSI");
    static const std::string SECONDARY_TSI_COL("SECONDARY_TSI");
	static const std::string DATEMODIFIED_COL("DATE_MODIFIED");
	static const std::string DATECREATED_COL("DATE_CREATED");

	//only for testing getAllLines, the code should be in base/dai/locationacccessfactory.
	const std::string TrainAccessFactory::CO_LINE_TBL = "co_line";
	const std::string TrainAccessFactory::COLINE_ID = "coline_id";
	const std::string TrainAccessFactory::NAME = "name";

	TA_Base_Core::NonReEntrantThreadLockable TrainAccessFactory::s_columnNamesLock;
	std::vector<std::string> TrainAccessFactory::s_columnNames;

    TrainAccessFactory* TrainAccessFactory::m_instance = NULL;


    TrainAccessFactory::TrainAccessFactory()
    {
    }

    TrainAccessFactory::~TrainAccessFactory()
    {
    }

    TrainAccessFactory& TrainAccessFactory::getInstance()
    {
        FUNCTION_ENTRY("getInstance");

        if(NULL == m_instance)
        {
            m_instance = new TrainAccessFactory();
        }
        
        FUNCTION_EXIT;
        return *m_instance;
    }

    void TrainAccessFactory::removeInstance()
    {
        FUNCTION_ENTRY("removeInstance");

		delete m_instance;
		m_instance = NULL;

        FUNCTION_EXIT;
    }

    ITrain* TrainAccessFactory::createTrain()
    {
        FUNCTION_ENTRY("createTrain");
            
        FUNCTION_EXIT;
        return new Train();
    }

	ITrain* TrainAccessFactory::copyTrain(const ITrain* trainToCopy)
	{
		 FUNCTION_ENTRY("copyTrain");

        TA_ASSERT(trainToCopy !=  NULL, "The train to copy was NULL");
        
        const Train* cast = dynamic_cast<const Train*>(trainToCopy);

        TA_ASSERT(cast != NULL, "trainToCopy passed could not be converted into a train");

        FUNCTION_EXIT;
        return new Train(*cast);
	}
//    ITrain* TrainAccessFactory::getTrain(const unsigned long key, const bool readWrite /* = false */)
//    {
//        FUNCTION_ENTRY("getTrain");
//    
//        TA_ASSERT(key > 0, "Invalid Train Id index.");
//
//        // get a connection to the database
//        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);
//
//        // create the SQL string to retrieve the name of the Train
//        // this is a check to ensure an Train with the specified key actually exists.        
//        std::ostringstream sql;
//        sql << "select " << PKEY_COL 
//            << " from "  << SITRAIN_TBL
//            << " where " << PKEY_COL << " = " << key;
//
//        // Set up the columnNames vector to be passed to executeQuery()
//        std::vector<std::string> columnNames;
//        columnNames.push_back(PKEY_COL);	
//
//        // Execute the query. The method can throw a DatabaseException.
//        // This is documented in the comment of this method.
//        // We are responsible for deleting the returned IData object when we're done with it
//        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(sql.str(), columnNames));
//        
//        if (0 == data->getNumRows())
//        {
//            // Train record with the specified primary key not found
//            std::ostringstream reason;
//		    reason << "No record found for Train with key = " << key;
//
//            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
//                     TA_Base_Core::DataException::NO_VALUE, "Invalid Train primary key." ) );
//        }
//
//        // Don't need to do anything with the dataa, only needed to check if it exists
//        TA_ASSERT(1 == data->getNumRows(), "Invalid number of records.");        
//
//        // Create the Train object to represent this object. Initially only populate it
//        // with id. We will populate the rest of the data on demand.
//        ITrain* theTrain = NULL;
//        
//        // decide the type of Train object to return based on the readWrite param
//        if (readWrite == true)
//        {
//            theTrain = new ConfigTrain(key);
//        }
//        else
//        {
//            theTrain = new Train(key);
//        }
//
//        // Return the Train pointer. The caller is responsible for deleting it.
//        FUNCTION_EXIT;
//        return theTrain;
//    }

    std::vector<ITrain*> TrainAccessFactory::getAllTrains(const bool readWrite /* = false */)
    {
		FUNCTION_ENTRY("getAllTrains");

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60707);

		TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, getColumnNames());

		// create the vector of pointers to ITrain
		std::vector<ITrain*> trainPtrVector;

		do
		{
			for (unsigned long i = 0; i < data->getNumRows(); ++i )
			{   
				trainPtrVector.push_back(new Train(i, *data));
			}

			delete data;
			data = NULL;
		}
		while(databaseConnection->moreData(data) == true);

		FUNCTION_EXIT;
		return trainPtrVector;
    }

// 	std::map<unsigned long, std::string> TrainAccessFactory::getAllLines ()
// 	{
// 		//return a map of line keys to line names from the co_line table
// 		FUNCTION_ENTRY("getAllSignallingBlocks");
// 		// get a connection to the database
// 		TA_Base_Core::IDatabase* databaseConnection = NULL;
// 		databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::OnlineUpdatable_Cd, TA_Base_Core::Write);
// 		
// 		if (databaseConnection == NULL)
// 		{
// //			TA_THROW( DataException("Can not connect to the database!", DataException::NO_VALUE, ""));
// 		}
// 		
//         // create the SQL string to retrieve the data of the radio subscriber based upon the key
//         std::ostringstream sql;
// 		sql << "select " 
// 			<< COLINE_ID << ","
// 			<< NAME
//             << " from " << CO_LINE_TBL
// 			<< std::ends;
// 
// 		
// 		
//         // Set up the column Names vector to be passed to executeQuery()
//         std::vector<std::string> columnNames;
//         columnNames.push_back(COLINE_ID);
// 		columnNames.push_back(NAME);
// 		
//         // Execute the query. The method can throw a DatabaseException.
//         // This is documented in the comment of this method.
//         // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
//         TA_Base_Core::IData *data = databaseConnection->executeQuery(sql.str(), columnNames);
// 		
// 		std::map<unsigned long, std::string> linesMap;
// 		do
// 		{
// 			for (unsigned long i = 0; i < data->getNumRows(); ++i )
// 			{   
// 				std::string lineName = data->getStringData(i, NAME);
// 				if (lineName.size() < 1)
// 				{
// //					TA_THROW(TA_Base_Core::DataException("The length of the Array is zero",
// //						TA_Base_Core::DataException::NO_VALUE, "Can not get the subscriber information from database" ) );
// 				}
// 				
// 				unsigned long lineID = data->getUnsignedLongData(i, COLINE_ID);
// 				linesMap[lineID] = lineName;
// 			}
// 			
// 			delete data;
// 			data = NULL;
// 		}
// 		while(databaseConnection->moreData(data) == true);
// 		
// 		FUNCTION_EXIT;
// 		return linesMap;	
// 	}

	const std::vector<std::string>& TrainAccessFactory::getColumnNames()
	{
		FUNCTION_ENTRY("getColumnNames");

		TA_THREADGUARD(s_columnNamesLock);

		// if the s_columnNames vector is empty,populate it with all column names
		// GROUP_PKEY_COLUMN GROUP_TSI_COLUMN LOCATION_COLUMN
		if ( true == s_columnNames.empty() )
		{
			s_columnNames.push_back( TRAINDID_COL );
			s_columnNames.push_back( PRIMARY_TSI_COL );
			s_columnNames.push_back( SECONDARY_TSI_COL );
			s_columnNames.push_back( DATECREATED_COL);
			s_columnNames.push_back( DATEMODIFIED_COL);
		}

		FUNCTION_EXIT;        
		return s_columnNames;
	}

	bool TrainAccessFactory::isTrainIDExist(unsigned long trainID)
	{
		FUNCTION_ENTRY("isTrainIDExist");

		bool retIsTrainExist = false;

		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60704, trainID);

		// Set up the vector to pass to executeQuery
		std::vector<std::string> columnNames;
		columnNames.push_back(TRAINDID_COL);

		// execute the query
		const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

		if (0 != data->getNumRows())
		{
			// train id is already in the db
			std::ostringstream reason;
			reason  << "A record is already using the train id " << trainID;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"%s", reason.str().c_str() );

			retIsTrainExist = true;
		}

		FUNCTION_EXIT;
		return retIsTrainExist;
	}
}