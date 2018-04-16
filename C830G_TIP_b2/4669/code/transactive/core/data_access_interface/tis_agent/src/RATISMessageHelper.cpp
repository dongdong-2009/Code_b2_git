/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/RATISMessageHelper.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * RATISMessageHelper is an object that is held by RATISMessage and ConfigDisplayRequest objects. 
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/RATISMessageHelper.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"



namespace TA_IRS_Core
{

	RATISMessageHelper::RATISMessageHelper()
		: m_isValidData( false ), m_key( 0 ), m_isNew( true ), m_hasChanged( false )
	{
		m_sessionRef = 0;
		m_requiresVetting = true;
		m_overridable = false;
		m_type = TA_Base_Core::RATIS_OUT_NEW;
		m_status = TA_Base_Core::NOT_APPROVED;
		m_creationDate = 0;
		m_messageText = "";
		m_priority = 0;
		m_tag = "";
		m_destination = "";
	}

    //
    // RATISMessageHelper
    //
    RATISMessageHelper::RATISMessageHelper( unsigned long ratisMessageKey )
        : m_isValidData( false ),
          m_key( ratisMessageKey ),
          m_isNew( false )
    {
		
    }

    //
    // getKey
    //
    unsigned long RATISMessageHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The display request key has not been initialised." );

        return m_key;
    }

	long RATISMessageHelper::getSessionRef()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_sessionRef;
	}

	std::string RATISMessageHelper::getStartTime()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_startTime;
	}

	std::string RATISMessageHelper::getEndTime()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_endTime;
	}

	bool RATISMessageHelper::getRequiresVetting()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_requiresVetting;
	}

	bool RATISMessageHelper::getOverridable()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_overridable;

	}

	TA_Base_Core::ERATISMessageType RATISMessageHelper::getType()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_type;
	}

	TA_Base_Core::ERATISMessageStatus RATISMessageHelper::getStatus()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_status;
	}

	long RATISMessageHelper::getCreationDate()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_creationDate;
	}

	std::string RATISMessageHelper::getMessageText()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_messageText;
	}

	int RATISMessageHelper::getPriority()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_priority;
	}

	std::string RATISMessageHelper::getTag()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_tag;
	}


	std::string RATISMessageHelper::getDestination()
	{
       if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_destination;
	}


    //
    // invalidate
    //
    void RATISMessageHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "DisplayRequestHelper::invalidate" );

        // This method cannot be called until the VideoOutputGroupKey and 
        // the VideoOutput position have been set.

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new DisplayRequest entry.");
        
        m_isValidData = false;

        LOG_FUNCTION_EXIT( SourceInfo, "DisplayRequestHelper::invalidate" );
    }


    //
    // reload
    //
    void RATISMessageHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "RATISMessageHelper::reload" );
      
        TA_ASSERT( 0 != m_key, 
            "This RATIS Message does not yet exist in the database. Therefore it cannot be reloaded.");
  
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Read);

        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::SESSION_REF_COLUMN << ", ";
		selectSql << TA_TISAgentDAI::START_TIME_COLUMN << ", ";
		selectSql << TA_TISAgentDAI::END_TIME_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::REQUIRES_VETTING_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::OVERRIDABLE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::TYPE_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::STATUS_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::CREATION_DATE_COLUMN << ", ";
		selectSql << TA_TISAgentDAI::MESSAGE_TEXT_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        selectSql << TA_TISAgentDAI::TAG_COLUMN << ", ";
		selectSql << TA_TISAgentDAI::DESTINATION_COLUMN;

        selectSql << " from " << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;
        selectSql << " where " << TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN;
        selectSql << "=" << m_key;

         // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( TA_TISAgentDAI::SESSION_REF_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::START_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::END_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::REQUIRES_VETTING_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::OVERRIDABLE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::TYPE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STATUS_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::CREATION_DATE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_TEXT_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::TAG_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::DESTINATION_COLUMN );

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr< TA_Base_Core::IData > data;
		try
		{
			data = std::auto_ptr< TA_Base_Core::IData >( databaseConnection->executeQuery(selectSql.str(),columnNames) );
		}
		catch( const TA_Base_Core::DataException& )
		{

		}
		catch( const TA_Base_Core::DatabaseException& )
		{

		}

		if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "No message library found for pkey = %s", m_key );			
                TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            char reasonMessage[256] = {0};
			sprintf(reasonMessage, "More than one Display Request found with pkey = %s", m_key );
			TA_THROW(TA_Base_Core::DataException(reasonMessage,TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

		// TODO 
		// Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.

		try
		{
			m_sessionRef = (long)data->getIntegerData( 0,TA_TISAgentDAI::SESSION_REF_COLUMN );
		}
		catch ( ... )
		{
			m_sessionRef = 0;
		}

		m_startTime = data->getStringData( 0, TA_TISAgentDAI::START_TIME_COLUMN );

		m_endTime = data->getStringData( 0, TA_TISAgentDAI::END_TIME_COLUMN );

		try
		{
			m_requiresVetting = (0 != data->getIntegerData( 0,TA_TISAgentDAI::REQUIRES_VETTING_COLUMN ) );
		}
		catch( ... )
		{
			m_requiresVetting = true;
		}

		try
		{
			m_overridable = (0 != data->getIntegerData(0,TA_TISAgentDAI::OVERRIDABLE_COLUMN ) );
		}
		catch( ... )
		{
			m_overridable = false;
		}

		try
		{
			m_type = ( TA_Base_Core::ERATISMessageType ) data->getIntegerData(0,TA_TISAgentDAI::TYPE_COLUMN );
		}
		catch( ... )
		{
			m_type = TA_Base_Core::RATIS_OUT_NEW;
		}

		try
		{
			m_status = ( TA_Base_Core::ERATISMessageStatus ) data->getIntegerData(0,TA_TISAgentDAI::STATUS_COLUMN );
		}
		catch( ... )
		{
			m_status = TA_Base_Core::NOT_APPROVED;
		}

		try
		{
			m_creationDate = (long)data->getIntegerData(0,TA_TISAgentDAI::CREATION_DATE_COLUMN );
		}
		catch( ... )
		{
			m_creationDate = 0;
		}

		m_messageText = data->getStringData(0,TA_TISAgentDAI::MESSAGE_TEXT_COLUMN );

		try
		{
			m_priority = data->getIntegerData(0,TA_TISAgentDAI::PRIORITY_COLUMN );
		}
		catch( ... )
		{
			m_priority = 0;
		}

		m_tag = data->getStringData(0,TA_TISAgentDAI::TAG_COLUMN );

		m_destination = data->getStringData(0,TA_TISAgentDAI::DESTINATION_COLUMN );

        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "DisplayRequestHelper::reload" ); 
    }

	void RATISMessageHelper::addNewRATISMessageData()
	{
        FUNCTION_ENTRY("RATISMessageHelper::addNewRATISMessageData");

		TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
  
		// run a query to update m_key
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::RATIS_MESSAGE_PKEY_SEQUENCE;
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
            error << "Cannot insert Display Request into ";
            error << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new RATIS Message. Inserting with key %d", m_key);

        std::stringstream entrySql;
		entrySql.str("");
        entrySql << "insert into " << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;
        entrySql << " (";
        entrySql << TA_TISAgentDAI::RATIS_MESSAGE_PKEY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::SESSION_REF_COLUMN << ", ";
		entrySql << TA_TISAgentDAI::START_TIME_COLUMN << ", ";
		entrySql << TA_TISAgentDAI::END_TIME_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::REQUIRES_VETTING_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::OVERRIDABLE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TYPE_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::STATUS_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::CREATION_DATE_COLUMN << ", ";
		entrySql << TA_TISAgentDAI::MESSAGE_TEXT_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        entrySql << TA_TISAgentDAI::TAG_COLUMN << ", ";
		entrySql << TA_TISAgentDAI::DESTINATION_COLUMN;

        entrySql << ")";
        entrySql << " values (";
        entrySql << m_key << ", ";
        entrySql << m_sessionRef << ", ";
		entrySql << "'" << m_startTime << "', ";
		entrySql << "'" << m_endTime << "', ";
        entrySql << m_requiresVetting << ", ";
        entrySql << m_overridable << ", ";
        entrySql << m_type << ", ";
        entrySql << m_status << ", ";
        entrySql << m_creationDate << ", ";
		entrySql << "'" << m_messageText << "', ";
		entrySql << m_priority << ", ";
        entrySql << "'" << m_tag << "', ";
		entrySql << "'" << m_destination << "')";
        
        // execute insert
		try
		{
			databaseConnection->executeModification(entrySql.str());
		}
		catch( ... )
		{
	        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Could not insert new display request into database");
		}

		m_isNew = false;

		FUNCTION_EXIT;
	}

	void RATISMessageHelper::updateExistingRATISMessageData()
	{
        FUNCTION_ENTRY("RATISMessageHelper::updateExistingRATISMessageData");

		TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
  
		// TODO - complete UPDATE

        std::stringstream entrySql;
		entrySql.str("");
        entrySql << "update " << TA_TISAgentDAI::RATIS_MESSAGE_TABLE_NAME;
        entrySql << " set ";
        entrySql << TA_TISAgentDAI::SESSION_REF_COLUMN << " = " << m_sessionRef << ", ";
		entrySql << TA_TISAgentDAI::START_TIME_COLUMN << " = '" << m_startTime << "', ";
		entrySql << TA_TISAgentDAI::END_TIME_COLUMN << " = '" << m_endTime << "', ";
        entrySql << TA_TISAgentDAI::REQUIRES_VETTING_COLUMN << " = " << m_requiresVetting << ", ";
        entrySql << TA_TISAgentDAI::OVERRIDABLE_COLUMN << " = " << m_overridable << ", ";
        entrySql << TA_TISAgentDAI::TYPE_COLUMN << " = " << m_type << ", ";
        entrySql << TA_TISAgentDAI::STATUS_COLUMN << " = " << m_status << ", ";
        entrySql << TA_TISAgentDAI::CREATION_DATE_COLUMN << " = " << m_creationDate << ", ";
		entrySql << TA_TISAgentDAI::MESSAGE_TEXT_COLUMN << " = '" << m_messageText << "', ";
        entrySql << TA_TISAgentDAI::PRIORITY_COLUMN << " = " << m_priority << ", ";
		entrySql << TA_TISAgentDAI::DESTINATION_COLUMN << " = '" << m_destination << "'";
        entrySql << " where " << TA_TISAgentDAI::TAG_COLUMN << " = " << m_tag;
        
        // execute insert
		try 
		{
			databaseConnection->executeModification(entrySql.str());
		}
		catch( ... )
		{
	        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Could not update existing display request");
		}

		// This should already be false
		m_isNew = false;

		FUNCTION_EXIT;
	}

	void RATISMessageHelper::writeRATISMessageData()
	{
        FUNCTION_ENTRY("RATISMessageHelper::writeRATISMessageData");
        
		if( m_isNew ) 
		{
			addNewRATISMessageData();
		}
		else
		{
			updateExistingRATISMessageData();
		}
        
        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isNew = false;
        m_hasChanged = false;

        FUNCTION_EXIT;

	}

	void RATISMessageHelper::deleteThisRATISMessage()
	{
		TA_Base_Core::IDatabase* databaseConnection = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::TisRatis_Sd, TA_Base_Core::Write);
        
		std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "delete from TI_RATIS_MESSAGE ";
        formatSQL << "where TIRMES_ID = " << m_key;

		try
		{
			databaseConnection->executeModification( formatSQL.str() );
		}
		catch( ... )
		{

		}

		// Now that the data has been deleted, it is new.
        m_isNew = true;
        m_hasChanged = true;

	}

	void RATISMessageHelper::setSessionRef( long sessionRef )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_sessionRef = sessionRef;
	}

	void RATISMessageHelper::setStartTime( const std::string& startTime )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_startTime = startTime;
	}

	void RATISMessageHelper::setEndTime( const std::string& endTime )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_endTime = endTime;
	}
	
	void RATISMessageHelper::setRequiresVetting( bool requiresVetting )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_requiresVetting = requiresVetting;

	}

	void RATISMessageHelper::setOverridable( bool overridable )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_overridable = overridable;

	}

	void RATISMessageHelper::setType( TA_Base_Core::ERATISMessageType type )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_type = type;

	}

	void RATISMessageHelper::setStatus( TA_Base_Core::ERATISMessageStatus status )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_status = status;
	}

	void RATISMessageHelper::setCreationDate( long creationDate )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_creationDate = creationDate;
	}

	void RATISMessageHelper::setMessageText( const std::string& messageText )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_messageText = messageText;
	}

	void RATISMessageHelper::setPriority( int priority )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_priority = priority;
	}

	void RATISMessageHelper::setTag( const std::string& tag )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_tag = tag;
	}

	void RATISMessageHelper::setDestination( const std::string& destination )
	{
		if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		m_destination = destination;
	}
  
} // closes TA_Base_Core
