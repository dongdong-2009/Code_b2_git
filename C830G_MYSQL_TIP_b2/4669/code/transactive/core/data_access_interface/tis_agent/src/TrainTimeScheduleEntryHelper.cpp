/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryHelper.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TrainTimeScheduleEntryHelper is an object that is held by TrainTimeScheduleEntryData and TrainTimeScheduleEntryData objects. 
  * It does all the database access for a train time schedule entry.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include "ace/OS.h"

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryHelper.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include <time.h>

namespace TA_IRS_Core
{

    //
    // TrainTimeScheduleEntryHelper
    //
    TrainTimeScheduleEntryHelper::TrainTimeScheduleEntryHelper()
        : m_isValidData( false ),
          m_isNew( true ),
          m_hasChanged( true ),
          m_key( 0 ),
          m_parentScheduleKey( 0 ),
          m_startTime( 0 ),
          m_endTime( 0 ),
          m_repeatInterval( 0 ),
          m_messageTable( ITrainTimeScheduleEntry::FreeText ),
          m_messageId( 0 ),
          m_priority( 0 )
    {
		
    }

    //
    // TrainTimeScheduleEntryHelper
    //
    TrainTimeScheduleEntryHelper::TrainTimeScheduleEntryHelper( unsigned long trainTimeScheduleEntryPKey )
        : m_isValidData( false ),
          m_isNew( false ),
          m_hasChanged( false ),
          m_key( trainTimeScheduleEntryPKey )
    {
		
    }


    TrainTimeScheduleEntryHelper::TrainTimeScheduleEntryHelper(const TrainTimeScheduleEntryHelper& theTrainTimeScheduleEntryHelper)
        : m_isValidData( false ),
          m_isNew( true ),
          m_hasChanged( true ),
          m_key( 0 ),
          m_parentScheduleKey( 0 ),
          m_startTime( theTrainTimeScheduleEntryHelper.m_startTime ),
          m_endTime( theTrainTimeScheduleEntryHelper.m_endTime ),
          m_repeatInterval( theTrainTimeScheduleEntryHelper.m_repeatInterval ),
          m_messageTable( theTrainTimeScheduleEntryHelper.m_messageTable ),
          m_messageId( theTrainTimeScheduleEntryHelper.m_messageId ),
          m_priority( theTrainTimeScheduleEntryHelper.m_priority )
    {}

    
	//
    // getKey
    //
    unsigned long TrainTimeScheduleEntryHelper::getKey()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getKey" );
        
        // This method cannot be called until the key has been set.
        TA_ASSERT( 0 != m_key, "The TrainTimeScheduleEntry key has not been initialised." );

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getKey" );
        return m_key;
    }

    unsigned long TrainTimeScheduleEntryHelper::getParentKey()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getParentKey" );

        // This method cannot be called until the key has been set.
        TA_ASSERT( 0 != m_parentScheduleKey, "The TrainTimeScheduleEntry parent key has not been initialised." );

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getParentKey" );
        return m_parentScheduleKey;
    }

    
    time_t TrainTimeScheduleEntryHelper::getStartTime()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getStartTime" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getStartTime" );
        return m_startTime;
    }


	time_t TrainTimeScheduleEntryHelper::getEndTime()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getEndTime" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getEndTime" );
        return m_endTime;
    }


	unsigned short TrainTimeScheduleEntryHelper::getRepeatInterval()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getRepeatInterval" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getRepeatInterval" );
        return m_repeatInterval;
    }


    ITrainTimeScheduleEntry::MessageTableType TrainTimeScheduleEntryHelper::getMessageTable()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getMessageTable" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getMessageTable" );
        return m_messageTable;
    }


    unsigned short TrainTimeScheduleEntryHelper::getMessageId()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getMessageId" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getMessageId" );
        return m_messageId;
    }


    unsigned short TrainTimeScheduleEntryHelper::getPriority()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::getPriority" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::getPriority" );
        return m_priority;
    }


    void TrainTimeScheduleEntryHelper::setParentKey(unsigned long parentKey)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setParentKey" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_parentScheduleKey != parentKey)
        {
            m_parentScheduleKey = parentKey;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setParentKey" );
    }


	void TrainTimeScheduleEntryHelper::setStartTime(time_t startTime)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setStartTime" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_startTime != startTime)
        {
            m_startTime = startTime;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setStartTime" );
    }


	void TrainTimeScheduleEntryHelper::setEndTime(time_t endTime)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setEndTime" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_endTime != endTime)
        {
            m_endTime = endTime;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setEndTime" );
    }


	void TrainTimeScheduleEntryHelper::setRepeatInterval(unsigned short repeatInterval)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setRepeatInterval" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_repeatInterval != repeatInterval)
        {
            m_repeatInterval = repeatInterval;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setRepeatInterval" );
    }


    void TrainTimeScheduleEntryHelper::setMessageTable(ITrainTimeScheduleEntry::MessageTableType messageTable)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setMessageTable" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_messageTable != messageTable)
        {
            m_messageTable = messageTable;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setMessageTable" );
    }


    void TrainTimeScheduleEntryHelper::setMessageId(unsigned short messageId)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setMessageId" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_messageId != messageId)
        {
            m_messageId = messageId;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setMessageId" );
    }


    void TrainTimeScheduleEntryHelper::setPriority(unsigned short priority)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::setPriority" );
        
        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        if (m_priority != priority)
        {
            m_priority = priority;
            m_hasChanged = true;
        }
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::setPriority" );
    }


    //
    // invalidate
    //
    void TrainTimeScheduleEntryHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::invalidate" );

        // This method cannot be called until the TrainTimeScheduleEntryKey has been set.

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

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::invalidate" );
    }


    //
    // reload
    //
    void TrainTimeScheduleEntryHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::reload" );
        
        // Ensure the TrainTimeScheduleEntry entry has already been written to the database. 
        // It doesn't make any sense to reload a TrainTimeScheduleEntry entry that doesn't exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This TrainTimeScheduleEntry does not yet exist in the database. Therefore it cannot be reloaded.");
 
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd/*TD17787*/, TA_Base_Core::Read);

 
        // Create the SQL string to retrieve the data for the TrainTimeScheduleEntry
        // based upon the TrainTimeScheduleEntry key.

        std::stringstream entrySql;
        entrySql << "select " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN << ", ";
        entrySql << "TO_CHAR(" << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_START_TIME_COLUMN << ", 'YYYYMMDDHH24MISS'), ";
        entrySql << "TO_CHAR(" << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_END_TIME_COLUMN << ", 'YYYYMMDDHH24MISS'), ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN;
        entrySql << " from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME;
        entrySql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN;
        entrySql << "=" << m_key;

        std::vector< std::string > entryColumnNames;
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_START_TIME_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_END_TIME_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN );
        entryColumnNames.push_back( TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN );


        // Execute the query. 
        //
        // The executeQuery() method can throw a TA_Base_Core::DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.


        std::auto_ptr< TA_Base_Core::IData > entryData = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( entrySql.str(), entryColumnNames ) );


        // Check that one entry was found for this key.
        
        if ( 0 == entryData->getNumRows() ) // No entry found with the specified key.
        {
            std::stringstream error;
            error << "The TrainTimeScheduleEntry with the key " << m_key;
            error << " was not found in the database table ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }
        else if (1 < entryData->getNumRows()) // More than one entry found for the pkey
        {
            std::stringstream error;
            error << "More than one TrainTimeScheduleEntry with the key " << m_key;
            error << " was found in the database table ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.

      
        m_key = entryData->getUnsignedLongData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN );
            
        // try catch around the fields that can be null
        try
        {
            m_startTime = entryData->getDateData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_START_TIME_COLUMN );
        }
        catch( TA_Base_Core::DataException&)
        {
            m_startTime = 0;
        }
        try
        {
            m_endTime = entryData->getDateData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_END_TIME_COLUMN );
        }
        catch( TA_Base_Core::DataException&)
        {
            m_endTime = 0;
        }
        try
        {
            // cannot read or write unsigned short using the existing database code
            // but this field is only ever populated by unsigned short - so a cast is ok.
            m_repeatInterval = static_cast<unsigned short>( entryData->getUnsignedLongData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN ) );

            if ( (m_repeatInterval < 1) || (m_repeatInterval > 255) )
            {
                LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                    "Schedule entry with pkey %d has a repeat interval %d. Defaulting to 1",
                    m_key, m_repeatInterval);

                m_repeatInterval = 1;
            }
        }
        catch( TA_Base_Core::DataException&)
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                  "Error reading schedule entry with pkey %d. Defaulting to repeat interval 1",
                  m_key);
            m_repeatInterval = 1;
        }
        try
        {
            int table = entryData->getUnsignedLongData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN );
            switch (table)
            {
            case 0:
                m_messageTable = ITrainTimeScheduleEntry::EmergencyPreDefined;
                break;
            case 1:
                m_messageTable = ITrainTimeScheduleEntry::NormalPreDefined;
                break;
            case 9:
                m_messageTable = ITrainTimeScheduleEntry::FreeText;
                break;
            default:
                LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                    "Message table type for schedule entry with pkey %d was %d. Defaulting to normal pre-defined",
                    m_key, table);
                m_messageTable = ITrainTimeScheduleEntry::NormalPreDefined;
                break;
            }
        }
        catch( TA_Base_Core::DataException&)
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                  "Error reading schedule entry with pkey %d. Defaulting to normal pre-defined",
                  m_key);
            m_messageTable = ITrainTimeScheduleEntry::NormalPreDefined;
        }
        try
        {
            // cannot read or write unsigned short using the existing database code
            // but this field is only ever populated by unsigned short - so a cast is ok.
            m_messageId = static_cast<unsigned short>( entryData->getUnsignedLongData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN ) );

            if ( (m_messageId < 1) || (m_messageId > 255) )
            {
                LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                    "Schedule entry with pkey %d has a message id %d. Defaulting to 1",
                    m_key, m_messageId);

                m_messageId = 1;
            }
        }
        catch( TA_Base_Core::DataException&)
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                  "Error reading schedule entry with pkey %d. Defaulting to message id 1",
                  m_key);
            m_messageId = 1;
        }
        try
        {
            // cannot read or write unsigned short using the existing database code
            // but this field is only ever populated by unsigned short - so a cast is ok.
            m_priority = static_cast<unsigned short>( entryData->getUnsignedLongData( 0, TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN ) );
            if ( (m_priority < 1) || (m_priority > 8) )
            {
                LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                    "Schedule entry with pkey %d has a priority %d. Defaulting to priority 1",
                    m_key, m_priority);
                m_priority = 8;
            }
        }
        catch( TA_Base_Core::DataException&)
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                  "Error reading schedule entry with pkey %d. Defaulting to priority 8",
                  m_key);
            m_priority = 8;
        }


        // The data is now valid.
        m_isValidData = true;
        m_hasChanged = false;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::reload" );
    }


    void TrainTimeScheduleEntryHelper::writeTrainTimeSheduleEntryData()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::writeTrainTimeSheduleEntryData" );

        // Check all values exist. If not add them to the dataConfigurationException
        std::vector<std::string> fieldNames;
        if (m_parentScheduleKey == 0)
        {
            fieldNames.push_back("Parent Schedule");
        }
        if ( !fieldNames.empty() ) // Need to throw an exception
        {
            TA_THROW(TA_Base_Core::DataConfigurationException("Operator data not fully specified. Operator cannot be written to database",
                                                         fieldNames));
        }

        // Need to check if this is a new or an existing entry
        // as a new entry will be inserted, while an existing entry will be updated
        if (m_isNew) // This is new
        {
            addNewTrainTimeScheduleEntry();
        }
        else // This is an existing Operator
        {
            updateExistingTrainTimeScheduleEntry();
        }
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_hasChanged = false;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::writeTrainTimeSheduleEntryData" );
    }


    void TrainTimeScheduleEntryHelper::addNewTrainTimeScheduleEntry()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::addNewTrainTimeScheduleEntry" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Saving a new train time schedule entry. Getting pkey first." );

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd/*TD17787*/, TA_Base_Core::Write);

        // run a query to update m_pkey
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_SEQUENCE;
        selectSql << " from DUAL";

        std::vector< std::string > columnNames;
        columnNames.push_back( "NEXTVAL" );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // Check that one entry was found
        if ( 1 != data->getNumRows() )
        {
            std::stringstream error;
            error << "Cannot insert time schedule entry into ";
            error << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new entry. Inserting with key %d", m_key);

        // convert time_t to a string to insert
        struct tm startTimeStruct;
        ACE_OS::localtime_r(&m_startTime, &startTimeStruct);
        //struct tm t;
        //ACE_OS::gmtime_r(&currentTime, &t);

        char startTimeString[22] = {0};
        sprintf(startTimeString, "%02d/%02d/%04d %02d:%02d:%02d", startTimeStruct.tm_mday, startTimeStruct.tm_mon + 1,
		        startTimeStruct.tm_year + 1900, startTimeStruct.tm_hour, startTimeStruct.tm_min, startTimeStruct.tm_sec);

        struct tm endTimeStruct;
        ACE_OS::localtime_r(&m_endTime, &endTimeStruct);
        char endTimeString[22] = {0};
        sprintf(endTimeString, "%02d/%02d/%04d %02d:%02d:%02d", endTimeStruct.tm_mday, endTimeStruct.tm_mon + 1,
		        endTimeStruct.tm_year + 1900, endTimeStruct.tm_hour, endTimeStruct.tm_min, endTimeStruct.tm_sec);


        // create insert sql
        std::stringstream entrySql;
        entrySql << "insert into " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME;
        entrySql << "(";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PARENT_SCHEDULE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_START_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_END_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN;
        entrySql << ")";
        entrySql << " values (";
        entrySql << m_key << ", ";
        entrySql << m_parentScheduleKey << ", ";
        entrySql << "TO_DATE('" << startTimeString << "','dd/mm/yyyy hh24:mi:ss'), ";
        entrySql << "TO_DATE('" << endTimeString << "','dd/mm/yyyy hh24:mi:ss'), ";
        entrySql << m_repeatInterval << ", ";
        entrySql << m_messageTable << ", ";
        entrySql << m_messageId << ", ";
        entrySql << m_priority;
        entrySql << ")";
        
        // execute insert
        databaseConnection->executeModification(entrySql.str());
        
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::addNewTrainTimeScheduleEntry" );
    }


    void TrainTimeScheduleEntryHelper::updateExistingTrainTimeScheduleEntry()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::updateExistingTrainTimeScheduleEntry" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Time schedule entry with key %d already exists. It's data will be updated.", m_key);

        // If the entry hasnt changed, we won't bother writing it
        if ( !hasChanged() )
        {
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Time schedule entry with key %d has not changed. It's data won't be updated.", m_key);

            // dont write
            return;
        }

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

        // convert time_t to a string to insert
        struct tm startTimeStruct;
        ACE_OS::localtime_r(&m_startTime, &startTimeStruct);
        //struct tm t;
        //ACE_OS::gmtime_r(&currentTime, &t);

        char startTimeString[22] = {0};
        sprintf(startTimeString, "%02d/%02d/%04d %02d:%02d:%02d", startTimeStruct.tm_mday, startTimeStruct.tm_mon + 1,
		        startTimeStruct.tm_year + 1900, startTimeStruct.tm_hour, startTimeStruct.tm_min, startTimeStruct.tm_sec);

        struct tm endTimeStruct;
        ACE_OS::localtime_r(&m_endTime, &endTimeStruct);
        char endTimeString[22] = {0};
        sprintf(endTimeString, "%02d/%02d/%04d %02d:%02d:%02d", endTimeStruct.tm_mday, endTimeStruct.tm_mon + 1,
		        endTimeStruct.tm_year + 1900, endTimeStruct.tm_hour, endTimeStruct.tm_min, endTimeStruct.tm_sec);

        // build the update statement
        std::ostringstream sql;
        sql << "update " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME;
        sql << " set " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PARENT_SCHEDULE_COLUMN;
        sql << " = " << m_parentScheduleKey << ", ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_START_TIME_COLUMN;
        sql << " = TO_DATE('" << startTimeString << "','dd/mm/yyyy hh24:mi:ss'), ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_END_TIME_COLUMN;
        sql << " = TO_DATE('" << endTimeString << "','dd/mm/yyyy hh24:mi:ss'), ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN;
        sql << " = " << m_repeatInterval << ", ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_TABLE_COLUMN;
        sql << " = " << m_messageTable << ", ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_MESSAGE_ID_COLUMN;
        sql << " = " << m_messageId << ", ";
        sql << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_PRIORITY_COLUMN;
        sql << " = " << m_priority;
        sql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN << " = " << m_key;

        // execute the sql
        databaseConnection->executeModification(sql.str().c_str());

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::updateExistingTrainTimeScheduleEntry" );
    }

    void TrainTimeScheduleEntryHelper::deleteTrainTimeSheduleEntryData()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleEntryHelper::deleteTrainTimeSheduleEntryData" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Time schedule entry with key %d will be deleted.", m_key);

        // dont allow a new entry to be deleted
        // also assert if the primary key is not set
        TA_ASSERT( m_isNew == false, "Trying to delete a new TrainTimeScheduleEntry" );
        TA_ASSERT( 0 != m_key, "The TrainTimeScheduleEntry key has not been initialised, and it is not a new entry" );

        std::ostringstream sql;
        sql << "delete from " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_TABLE_NAME;
        sql << " where " << TA_TISAgentDAI::TRAIN_TIME_SCHEDULE_ENTRY_PKEY_COLUMN << " = " << m_key;

        // get a connection to the database
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

        databaseConnection->executeModification(sql.str().c_str());

        // Now that the data has been deleted, it is new.
        m_isNew = true;
        m_hasChanged = true;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleEntryHelper::deleteTrainTimeSheduleEntryData" );
    }


} // closes TA_Base_Core
