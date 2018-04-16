/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleHelper.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * VideoOutputGroupHelper is an object that is held by VideoOutputGroup and ConfigVideoOutputGroup objects. 
  * It contains all data used by TypePluginMatrices, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoOutputGroup and ConfigVideoOutputGroup.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleHelper.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_Core
{

    TrainTimeScheduleHelper::TrainTimeScheduleHelper(  )
        : m_isValidData( false ),
          m_isNew( true ),
          m_hasChanged( false ),
          m_key( 0 ),
          m_name( "" )
    {
        m_adhocMessages[0] = "";
        m_adhocMessages[1] = "";
        m_adhocMessages[2] = "";
        m_adhocMessages[3] = "";
        m_adhocMessages[4] = "";
        m_adhocMessages[5] = "";
        m_adhocMessageTitles[0] = "Free Text 1";
        m_adhocMessageTitles[1] = "Free Text 2";
        m_adhocMessageTitles[2] = "Free Text 3";
        m_adhocMessageTitles[3] = "Free Text 4";
        m_adhocMessageTitles[4] = "Free Text 5";
        m_adhocMessageTitles[5] = "Free Text 6";
    }

    
    TrainTimeScheduleHelper::TrainTimeScheduleHelper( unsigned long trainTimeScheduleKey )
        : m_isValidData( false ),
          m_isNew( false ),
          m_key( trainTimeScheduleKey )
    {	
    }


    TrainTimeScheduleHelper::TrainTimeScheduleHelper( const TrainTimeScheduleHelper& timeScheduleHelper)
        : m_isValidData( false ),
          m_isNew( true ),
          m_hasChanged( false ),
          m_key( 0 ),
          m_name( timeScheduleHelper.m_name )
    {
        m_name.append(" (New)");

        m_adhocMessages[0] = timeScheduleHelper.m_adhocMessages[0];
        m_adhocMessages[1] = timeScheduleHelper.m_adhocMessages[1];
        m_adhocMessages[2] = timeScheduleHelper.m_adhocMessages[2];
        m_adhocMessages[3] = timeScheduleHelper.m_adhocMessages[3];
        m_adhocMessages[4] = timeScheduleHelper.m_adhocMessages[4];
        m_adhocMessages[5] = timeScheduleHelper.m_adhocMessages[5];
        m_adhocMessageTitles[0] = timeScheduleHelper.m_adhocMessageTitles[0];
        m_adhocMessageTitles[1] = timeScheduleHelper.m_adhocMessageTitles[1];
        m_adhocMessageTitles[2] = timeScheduleHelper.m_adhocMessageTitles[2];
        m_adhocMessageTitles[3] = timeScheduleHelper.m_adhocMessageTitles[3];
        m_adhocMessageTitles[4] = timeScheduleHelper.m_adhocMessageTitles[4];
        m_adhocMessageTitles[5] = timeScheduleHelper.m_adhocMessageTitles[5];
    }

    //
    // getKey
    //
    unsigned long TrainTimeScheduleHelper::getKey()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::getKey" );
        
        // This method cannot be called until the key has been set.
        TA_ASSERT( 0 != m_key, "The TrainTimeSchedule key has not been initialised." );

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::getKey" );
        return m_key;
    }


    std::string TrainTimeScheduleHelper::getName()
	{
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::getName" );
        
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::getName" );
        return m_name;
	}


    std::string TrainTimeScheduleHelper::getAdHocMessageTitle(unsigned short messageNumber)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::getAdHocMessageTitle" );

        TA_ASSERT(messageNumber <= NUM_ADHOC_MSGS, "Invalid message number given" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::getAdHocMessageTitle" );
        return m_adhocMessageTitles[messageNumber-1];
    }

    
    std::string TrainTimeScheduleHelper::getAdHocMessage(unsigned short messageNumber)
	{
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::getAdHocMessage" );

        TA_ASSERT(messageNumber <= NUM_ADHOC_MSGS, "Invalid message number given" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::getAdHocMessage" );
        return m_adhocMessages[messageNumber-1];
	}


    //
    // invalidate
    //
    void TrainTimeScheduleHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::invalidate" );

        m_isValidData = false;

        // reset the modified flag here
        // the next get/set operation will cause a reload
        // but if invalidate is called just before hasChanged
        // (without a reload in between) this object will show dirty.
        // theres no point reloading until needed, so just set it to clean.
        if (!m_isNew)
        {
            m_hasChanged = false;
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::invalidate" );
    }


    void TrainTimeScheduleHelper::setName(std::string name)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::setName" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (name != m_name)
        {
            m_name = name;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::setName" );
    }


    void TrainTimeScheduleHelper::setAdHocMessage(unsigned short messageNumber,
                                                  std::string message)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::setAdHocMessage" );

        TA_ASSERT(messageNumber <= NUM_ADHOC_MSGS, "Invalid message number given" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_adhocMessages[messageNumber-1] != message)
        {
            m_adhocMessages[messageNumber-1] = message;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::setAdHocMessage" );
    }


    void TrainTimeScheduleHelper::setAdHocMessageTitle(unsigned short messageNumber,
                                                       std::string messageTitle)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::setAdHocMessageTitle" );

        TA_ASSERT(messageNumber <= NUM_ADHOC_MSGS, "Invalid message number given" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_adhocMessageTitles[messageNumber-1] != messageTitle)
        {
            m_adhocMessageTitles[messageNumber-1] = messageTitle;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::setAdHocMessageTitle" );
    }


    //
    // reload
    //
    void TrainTimeScheduleHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::reload" );
        
        // Ensure the TrainTimeSchedule entry has already been written to the database. 
        // It doesn't make any sense to reload a TrainTimeSchedule entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This TrainTimeSchedule does not yet exist in the database. Therefore it cannot be reloaded.");
 
        // Get a connection to the database.
         TA_Base_Core::IDatabase* databaseConnection = 
             TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd/*TD17787*/, TA_Base_Core::Read);

 
        // Create the SQL string to retrieve the data for the TrainTimeSchedule
        // based upon the TrainTimeSchedule key.

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::SCHEDULE_NAME_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE1_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE1_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE2_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE2_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE3_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE3_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE4_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE4_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE5_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE5_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE6_TITLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::ADHOC_MESSAGE6_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
        selectSql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN;
        selectSql << "=" << m_key;

        // Execute the query. 
        //
        // The executeQuery() method can throw a TA_Base_Core::DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::SCHEDULE_NAME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE1_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE1_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE2_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE2_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE3_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE3_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE4_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE4_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE5_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE5_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE6_TITLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::ADHOC_MESSAGE6_COLUMN );

        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // Check that one entry was found for this key.
        
        if ( 0 == data->getNumRows() ) // No entry found with the specified key.
        {
            std::stringstream error;
            error << "The TrainTimeSchedule with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::stringstream error;
            error << "More than one TrainTimeSchedule with the key " << m_key;
            error << " was found in the database table ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.

        m_name = data->getStringData( 0, TA_TISAgentDAI::SCHEDULE_NAME_COLUMN );
        m_adhocMessageTitles[0] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE1_TITLE_COLUMN );
        m_adhocMessages[0] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE1_COLUMN );
        m_adhocMessageTitles[1] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE2_TITLE_COLUMN );
        m_adhocMessages[1] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE2_COLUMN );
        m_adhocMessageTitles[2] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE3_TITLE_COLUMN );
        m_adhocMessages[2] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE3_COLUMN );
        m_adhocMessageTitles[3] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE4_TITLE_COLUMN );
        m_adhocMessages[3] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE4_COLUMN );
        m_adhocMessageTitles[4] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE5_TITLE_COLUMN );
        m_adhocMessages[4] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE5_COLUMN );
        m_adhocMessageTitles[5] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE6_TITLE_COLUMN );
        m_adhocMessages[5] = data->getStringData( 0, TA_TISAgentDAI::ADHOC_MESSAGE6_COLUMN );

        // The data is now valid.
        m_isValidData = true;
        m_hasChanged = false;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::reload" );
    }


    void TrainTimeScheduleHelper::writeTrainTimeSchedule()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::writeTrainTimeSchedule" );

        // Check all values exist. If not add them to the dataConfigurationException
        std::vector<std::string> fieldNames;
        if (m_name.empty())
        {
            fieldNames.push_back("Name");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Time Schedule data not fully specified. Time Schedule cannot be written to database",
                                                         fieldNames));
        }

        // Need to check if this is a new or an existing entry
        // as a new entry will be inserted, while an existing entry will be updated
        if (m_isNew) // This is new
        {
            addNewTrainTimeSchedule();
        }
        else // This is an existing Operator
        {
            updateExistingTrainTimeSchedule();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_hasChanged = false;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::writeTrainTimeSchedule" );
    }


    void TrainTimeScheduleHelper::addNewTrainTimeSchedule()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::addNewTrainTimeSchedule" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Saving a new train time schedule. Getting pkey first." );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd/*TD17787*/, TA_Base_Core::Write);

        // run a query to update m_key
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_SEQUENCE;
        selectSql << " from DUAL";

        std::vector< std::string > columnNames;
        columnNames.push_back( "NEXTVAL" );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // Check that one entry was found
        if ( 1 != data->getNumRows() )
        {
            // unable to get a new key, throw an exception
            std::stringstream error;
            error << "Cannot insert time schedule into ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new schedule. Inserting with key %d", m_key);

        // create insert sql
        std::stringstream entrySql;
        entrySql << "insert into " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
        entrySql << "(";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::SCHEDULE_NAME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE1_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE1_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE2_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE2_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE3_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE3_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE4_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE4_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE5_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE5_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE6_TITLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::ADHOC_MESSAGE6_COLUMN;
        entrySql << ")";
        entrySql << " values (";
        entrySql << m_key << ", ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_name) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[0]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[0]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[1]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[1]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[2]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[2]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[3]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[3]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[4]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[4]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessageTitles[5]) << "', ";
        entrySql << "'" << databaseConnection->escapeInsertString(m_adhocMessages[5]) << "')";
        
        // execute insert
        databaseConnection->executeModification(entrySql.str());
        
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::addNewTrainTimeSchedule" );
    }


    void TrainTimeScheduleHelper::updateExistingTrainTimeSchedule()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::updateExistingTrainTimeSchedule" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Time schedule with key %d already exists. It's data will be updated.", m_key);

        // If the entry hasnt changed, we won't bother writing it
        if ( !hasChanged() )
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Time schedule with key %d has not changed. It's data won't be updated.", m_key);

            // dont write
            return;
        }

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

        // build the update statement
        std::ostringstream sql;
        sql << "update " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
        sql << " set " << TA_TISAgentDAI::SCHEDULE_NAME_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_name) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE1_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[0]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE1_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[0]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE2_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[1]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE2_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[1]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE3_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[2]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE3_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[2]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE4_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[3]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE4_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[3]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE5_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[4]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE5_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[4]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE6_TITLE_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessageTitles[5]) << "', ";
        sql << TA_TISAgentDAI::ADHOC_MESSAGE6_COLUMN << " = '";
        sql << databaseConnection->escapeInsertString(m_adhocMessages[5]);
        sql << "' where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN << " = " << m_key;

        // execute the sql
        databaseConnection->executeModification(sql.str().c_str());

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::updateExistingTrainTimeSchedule" );
    }

    
    void TrainTimeScheduleHelper::deleteTrainTimeSchedule()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::deleteTrainTimeSchedule" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Time schedule with key %d will be deleted.", m_key);

        // dont allow a new entry to be deleted
        // also assert if the primary key is not set
        TA_ASSERT( m_isNew == false, "Trying to delete a new TrainTimeSchedule" );
        TA_ASSERT( 0 != m_key, "The TrainTimeSchedule key has not been initialised, and it is not a new schedule" );

        std::ostringstream sql;
        sql << "delete from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_TABLE_NAME;
        sql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PKEY_COLUMN << " = " << m_key;

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

        databaseConnection->executeModification(sql.str().c_str());

        // Now that the data has been deleted, it is new.
        m_isNew = true;
        m_hasChanged = true;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::deleteTrainTimeSchedule" );
    }


    std::vector<unsigned long> TrainTimeScheduleHelper::getEntryPkeys()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleHelper::getEntryPkeys" );
        
        std::vector<unsigned long> keys;

        // only load entries if this schedule is saved in the database
        if ( isNew() )
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Time schedule is new, no entries will be loaded");

            // if its new
            // return an empty vector
            return keys;
        }

        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Read);

        // otherwise create the sql
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME;
        selectSql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PARENT_SCHEDULE_COLUMN;
        selectSql << " = " << m_key;

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it,

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN );

        TA_Base_Core::IData* data = databaseConnection->executeQuery( selectSql.str(), columnNames );
        
        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < data->getNumRows(); ++i )
		        {                
			        keys.push_back(
                        data->getUnsignedLongData( i, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN ) );
		        }
            
                // clean up used data object
                delete data;
                data = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(data);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (data != NULL)
            {
                delete data;
                data = NULL;
            }

            // throw on
            throw;
        }
        
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleHelper::getEntryPkeys" );
        return keys;
    }

} // closes TA_Base_Core
