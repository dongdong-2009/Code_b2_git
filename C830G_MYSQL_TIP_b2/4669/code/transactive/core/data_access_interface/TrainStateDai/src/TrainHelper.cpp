/**
  *
  *
  * Implementation of the TrainHelper class.
  */
#pragma warning(disable:4786 4290)

#include <algorithm>
#include <ctime>

#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/utilities/src/DataConversion.h"

#include "core/data_access_interface/TrainStateDai/src/TrainHelper.h"
#include "core/database/src/CommonType.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_Core
{
    //
    // Constants
    //
    
    // Table name    
	
	static const std::string TRTRAIN_TBL("TR_TRAIN");
    static const std::string TRAINDID_COL("TRAIN_ID");
    static const std::string PRIMARY_TSI_COL("PRIMARY_TSI");
    static const std::string SECONDARY_TSI_COL("SECONDARY_TSI");

	static const std::string DATEMODIFIED_COL("DATE_MODIFIED");
	static const std::string DATECREATED_COL("DATE_CREATED");
    
    // Used for data validation
    enum
    {
        INVALID_KEY     =  -1,
        TRAINID_DEFAULT =   0,
        TRAINID_MIN     =   1,
        TRAINID_MAX     = 150,
		INVALID_EMUID = -1
    };

    //
    // Static helper functions
    //

    /** 
	  * toUpper
	  * 
	  * Converts a string to uppercase and returns it back.
	  * 
	  * @return std::string 
	  * @param : const std::string& in
	  */
    static std::string toUpper(const std::string& in)
    {
        FUNCTION_ENTRY("toUpper");

        std::string out(in);
        std::transform(out.begin(), out.end(), out.begin(), toupper);

        FUNCTION_EXIT;
        return out;
    }

	TA_Base_Core::NonReEntrantThreadLockable TrainHelper::s_columnNamesLock;
	std::vector<std::string> TrainHelper::s_columnNames;

    //
    // Constructors and destructor
    //
    
    TrainHelper::TrainHelper()
    :   m_isNew(true),
        m_isValidData(false),	
        //m_key(INVALID_KEY),
        m_trainId(TRAINID_DEFAULT),
        m_primaryTsi(""),
        m_secondaryTsi(""),
		m_dateCreated(0),
		m_dateModified(0)
    {
        FUNCTION_ENTRY("TrainHelper ctor()");
        FUNCTION_EXIT;
    }

    TrainHelper::TrainHelper(const TrainHelper& theTrainHelper)
    :   m_isNew(true),
        m_isValidData(false),
        //m_key(INVALID_KEY),
        m_trainId(theTrainHelper.m_trainId),
        m_primaryTsi(theTrainHelper.m_primaryTsi),
        m_secondaryTsi(theTrainHelper.m_secondaryTsi),
		m_dateCreated(theTrainHelper.m_dateCreated),
		m_dateModified(theTrainHelper.m_dateModified)
    {
        FUNCTION_ENTRY("TrainHelper copy ctor");
        FUNCTION_EXIT;
    }

    TrainHelper::TrainHelper(const unsigned long key)
    :   m_isNew(false),
        m_isValidData(false),
        //m_key(key),
        m_trainId(TRAINID_DEFAULT),
        m_primaryTsi(""),
        m_secondaryTsi(""),
		m_dateCreated(0),
		m_dateModified(0)
    {
        FUNCTION_ENTRY("TrainHelper ctor(const unsigned long)");
        FUNCTION_EXIT;
    }

    TrainHelper::TrainHelper(const unsigned long row, TA_Base_Core::IData& data)
    :   m_isNew(false),
        m_isValidData(false),
        //m_key(INVALID_KEY),
        m_trainId(TRAINID_DEFAULT),
        m_primaryTsi(""),
        m_secondaryTsi(""),
		m_dateCreated(0),
		m_dateModified(0)
    {
        FUNCTION_ENTRY("TrainHelper ctor(const unsigned long, TA_Base_Core::IData&)");        
  
        reloadUsing(row, data);
        FUNCTION_EXIT;
    }

    TrainHelper::~TrainHelper()
    {
        FUNCTION_ENTRY("~TrainHelper");
        FUNCTION_EXIT;
    }

    //
    // TrainHelper methods
    //

    unsigned long TrainHelper::getTrainId()
    {
        FUNCTION_ENTRY("getTrainId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_trainId;
    }

   

	void TrainHelper::setTrainId(unsigned long trainId)
    {
        FUNCTION_ENTRY("setTrainId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_trainId = trainId;
        FUNCTION_EXIT;
    }

    
	std::string TrainHelper::getPrimaryTsi()
	{
		FUNCTION_ENTRY("getPrimaryEmuId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_primaryTsi;
	}
	
	
	std::string TrainHelper::getSecondaryTsi()
	{
		FUNCTION_ENTRY("getSecondaryEmuId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        FUNCTION_EXIT;
        return m_secondaryTsi;
	}
	
	
	void TrainHelper::setPrimaryTsi(const std::string& primaryTsi)
	{
		FUNCTION_ENTRY("setPrimaryEmuId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_primaryTsi = primaryTsi;
        FUNCTION_EXIT;
	}
	
	
	void TrainHelper::setSecondaryTsi(const std::string& secondaryTsi)
	{
		FUNCTION_ENTRY("setSecondaryEmuId");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
        m_secondaryTsi = secondaryTsi;
        FUNCTION_EXIT;
	}
	
	
	std::string TrainHelper::getName()
	{
		FUNCTION_ENTRY("getName");
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }
       
		std::ostringstream name;
		name << m_trainId;
		
        FUNCTION_EXIT;
        return name.str();
	}
	
	

    void TrainHelper::writeTrainData()
    {
        FUNCTION_ENTRY("writeTrainData");

        std::vector<std::string> fieldNames;

        // validate each field
        if (m_trainId < TRAINID_MIN || m_trainId > TRAINID_MAX)
        {
            fieldNames.push_back(TRAINDID_COL);
        }
        if (m_primaryTsi.empty() )
        {
            fieldNames.push_back(PRIMARY_TSI_COL);
        }
        if (m_secondaryTsi.empty() )
        {
            fieldNames.push_back(SECONDARY_TSI_COL);
        }
        

        // throw exception if any of the field is invalid
        if (! fieldNames.empty())
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Train data not fully specified. " \
                                                              "Train cannot be written to database.",
                                                              fieldNames));
        }

	
        // update the database...
        if (m_isNew)
        {
            addNewTrain();
        }
        else
        {
            modifyExistingTrain();
        }

        // after writing the data, what we hold is now valid
        m_isValidData = true;
        m_isNew = false;

        FUNCTION_EXIT;
    }

    void TrainHelper::deleteTrain()
    {
        FUNCTION_ENTRY("deleteTrain");

        TA_ASSERT(! m_isNew, "This Train does not yet exist in the database and therefore cannot be deleted."); 

		try
		{
			// get a connection to the database
			TA_Base_Core::IDatabase* databaseConnection =
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase( TA_Base_Core::Ats_Cd, TA_Base_Core::Write );
		
			TA_Base_Core::SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_DELETE_60702,
				       getLocalDatabaseName(), m_trainId );

			// stored procedure execution
			databaseConnection->executeProcedure( strSql );
		}
		catch ( TA_Base_Core::DataException& de )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
		}
		catch ( TA_Base_Core::DatabaseException& dbe )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", "UNKNOWN" );
		}

       FUNCTION_EXIT;
    }

    void TrainHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // This method cannot be called until the key has been set
        TA_ASSERT(! m_isNew, "Attempted to call invalidate() on a new Train.");

        m_isValidData = false;

        FUNCTION_EXIT;
    }

    //
    // Private methods
    //

    void TrainHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Ensure the Train has already been written to the database. It doesn't make
        // any sense to reload an Train that doesn't exist in the database.
        TA_ASSERT(!m_isNew, "This Train does not yet exist in the database. " 
                            "Therefore it cannot be reloaded.");

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60703, m_trainId);
        

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it (handled by auto pointer)
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, getColumnNames()));

		//TA_Base_Core::IData* data = databaseConnection->executeQuery( strSql, columnNames);

        if (0 == data->getNumRows()) 
        {
            // Train record not found
            std::ostringstream reason;
            reason << "No record found for TrainID  = " << m_trainId << std::endl;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NO_VALUE, "Train Id" ) );
        }

        TA_ASSERT(data->getNumRows() == 1, "Invalid number of Train records."); 
        
        reloadUsing(0, *data);

        FUNCTION_EXIT;  
    }

    void TrainHelper::reloadUsing(const unsigned long row, TA_Base_Core::IData& data)
    {
        FUNCTION_ENTRY("reloadUsing");

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_trainId       = data.getUnsignedLongData(row, TRAINDID_COL);
        m_primaryTsi    = data.getStringData(row, PRIMARY_TSI_COL);
        m_secondaryTsi  = data.getStringData(row, SECONDARY_TSI_COL);
		m_dateCreated   = data.getDateData(row, DATECREATED_COL);
		
		try
		{
			m_dateModified = data.getDateData( row, DATEMODIFIED_COL );
			struct tm newtm;

			// Set up the date created structure to equal the value we set it to when we get NULL
			// from the database
			newtm.tm_sec = 0;		//seconds
			newtm.tm_min = 0;		// minutes 
			newtm.tm_hour = 12;		// hours 
			newtm.tm_mday = 1;		// day of the month 
			newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
			newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
			// between 1901 and 2038
			newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

			time_t value;
			value = mktime(&newtm);

			if (value == m_dateModified) // The date was actually NULL
			{
				m_dateModified = 0;
			}
		}
		catch (TA_Base_Core::DataException& ex)
		{
			//If we catch this exception, need to check if its a "NO_VALUE" type
			if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
			{   // Need to on-throw this exception
				throw;
			}
			else // The date-modified column was null - this is not fatal
			{
				m_dateModified = 0;
			}
		}
       
        // Need to record that we now have valid data
        m_isValidData = true;
        
        FUNCTION_EXIT;
    }

    void TrainHelper::addNewTrain()
    {
        FUNCTION_ENTRY("addNewTrain");

        LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
            "Train %d is a new Train. It will be added to the database.", m_trainId);

		try
		{
			validateTrainIdUniqueness(true);

			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

			TA_Base_Core::SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_INSERT_60705, 
				  getLocalDatabaseName() ,m_trainId, m_primaryTsi, m_secondaryTsi);

			databaseConnection->executeModification( strSql );

			//For Date Created
			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60709, m_trainId);

			std::vector<std::string> columnNames;
			columnNames.push_back(TRAINDID_COL);
			columnNames.push_back(DATECREATED_COL);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql,columnNames);

			if (0 == data->getNumRows()) 
			{
				// PA Zone record not found            
				delete data;
				data = NULL;

				std::ostringstream reasonMessage;
				reasonMessage << "No data found for TrainID = " << m_trainId;			
				TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"PA Station key" ) );
			}

			// Retrieve the pkey as an unsigned long. There should only be a single returned item 
			// (as any other circumstance is specifically prevented by the above checks).
			// This method can throw a DataException
			try
			{
				m_dateCreated = data->getDateData(0,DATECREATED_COL);
			}
			catch ( ... )
			{
				delete data;
				data = NULL;
				throw;
			}

			// Now that we're finished with the IData object, we need to delete it.
			delete data;
			data = NULL;

		}
		catch ( TA_Base_Core::DataException& de )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
		}
		catch ( TA_Base_Core::DatabaseException& dbe )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", "UNKNOWN" );
		}
        
        FUNCTION_EXIT;    
    }

    void TrainHelper::modifyExistingTrain()
    {
        FUNCTION_ENTRY("modifyExistingTrain");

		try
		{
			validateTrainIdUniqueness(false);

			LOG (SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"TrainId = %ul already exists. Data will be updated.", m_trainId);

			TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Write);

			TA_Base_Core::SQLStatement strSql;
			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_UPDATE_60706, 
				  getLocalDatabaseName(),m_trainId, m_primaryTsi, m_secondaryTsi, m_trainId);

			databaseConnection->executeModification( strSql );

			//For Date Created and Date Modified

			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60710, m_trainId);

			std::vector<std::string> columnNames;
			columnNames.push_back(DATECREATED_COL);
			columnNames.push_back(DATEMODIFIED_COL);

			// Execute the query. The method can throw a DatabaseException.
			// This is documented in the comment of this method.
			// We are responsible for deleting the returned IData object when we're done with it
			TA_Base_Core::IData* data = databaseConnection->executeQuery(strSql, columnNames);

			if (0 == data->getNumRows()) 
			{
				// PA Zone record not found

				delete data;
				data = NULL;

				std::ostringstream reasonMessage;
				reasonMessage << "No data found for TrainID = " << m_trainId;			
				TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"TrainID" ) );
			}

			try
			{
				// Clear the modified key flag, as now current key will reflect modified key
				// (if in fact the modified key was set)
				//m_modifiedKey.resetValue();

				m_dateCreated = data->getDateData(0, DATECREATED_COL);
				m_dateModified = data->getDateData(0, DATEMODIFIED_COL);
			}
			catch ( ... )
			{
				delete data;
				data = NULL;
				throw;
			}

			// Now that we're finished with the IData object, we need to delete it.
			delete data;
			data = NULL;
		}
		catch ( TA_Base_Core::DataException& de )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
		}
		catch ( TA_Base_Core::DatabaseException& dbe )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", "UNKNOWN" );
		}
        
        FUNCTION_EXIT;
    }   

    void TrainHelper::validateTrainIdUniqueness(bool isNew)
    {        
        // check if train id is in use
        //std::ostringstream sql;  

		TA_Base_Core::IDatabase* databaseConnection = 
			TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Ats_Cd, TA_Base_Core::Read);

		TA_Base_Core::SQLStatement strSql;

		if ( isNew )
		{
			databaseConnection->prepareSQLStatement(strSql, TR_TRAIN_SELECT_60704, m_trainId);
		}
		else
		{
			databaseConnection->prepareSQLStatement(strSql,TR_TRAIN_SELECT_60708, m_trainId, m_primaryTsi, m_secondaryTsi);
		}
		
        // Set up the vector to pass to executeQuery
        std::vector<std::string> columnNames;
        columnNames.push_back(TRAINDID_COL);

        // execute the query
        const std::auto_ptr<TA_Base_Core::IData> data(databaseConnection->executeQuery(strSql, columnNames));

        if (0 != data->getNumRows())
        {
            // train id is already in the db
            std::ostringstream reason;
			reason  << "A record is already using the train id " << m_trainId;

            TA_THROW(TA_Base_Core::DataException(reason.str().c_str(),
                     TA_Base_Core::DataException::NOT_UNIQUE, "Train Id"));
        }        
    }

	std::string TrainHelper::getLocalDatabaseName()
	{
		std::string localDatabaseName;    
		if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
		{
			throw TA_Base_Core::DatabaseException("db-connection-file not set");
		}
		try
		{
			std::string strFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			localDatabaseName = TA_Base_Core::DatabaseFactory::getInstance().getDatabaseName(strFile, TA_Base_Core::Ats_Cd, TA_Base_Core::Write);
		}
		catch(...) // most likely a file not found error
		{
			throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
		}
		return localDatabaseName;
	}

	const std::vector<std::string>& TrainHelper::getColumnNames()
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

	time_t TrainHelper::getDateCreated()
	{
		FUNCTION_ENTRY("getDateCreated");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_dateCreated;
	}

	time_t TrainHelper::getDateModified()
	{
		FUNCTION_ENTRY("getDateCreated");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_dateModified;
	}

}