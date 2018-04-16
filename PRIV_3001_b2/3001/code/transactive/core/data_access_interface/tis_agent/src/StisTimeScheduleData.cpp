/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/tis_agent/src/StisTimeScheduleData.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by: $Author: grace.koh $
  * 
  * VideoOutputGroupHelper is an object that is held by VideoOutputGroup and ConfigVideoOutputGroup objects. 
  * It contains all data used by TypePluginMatrices, and manipulation
  * methods for the data. It helps avoid re-writing code for both VideoOutputGroup and ConfigVideoOutputGroup.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent/src/StisTimeScheduleData.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#define UPDATE_VALUE_IF(v1, v2) if(v1 != v2){v1=v2; m_hasChanged = true;}

namespace TA_IRS_Core
{
    StisTimeScheduleData::StisTimeScheduleData( unsigned long scheduleId)
        : m_isValidData( false ),
          m_key( scheduleId ),
		  m_startTime(""),
		  m_endTime(""),
		  m_textContent("")
    {	
    }



    //
    // getKey
    //
    unsigned long StisTimeScheduleData::getKey()
    {
        FUNCTION_ENTRY("getKey");
        
        // This method cannot be called until the key has been set.
        TA_ASSERT( 0 < m_key && m_key <= 20, "The StisTimeSchedule key is not invalid." );

        FUNCTION_EXIT;
        return m_key;
    }

	bool StisTimeScheduleData::isRecordEmtpy()
	{
		FUNCTION_ENTRY("isRecordEmpty");
		reload();
		FUNCTION_EXIT;
		return m_textContent.empty();
	}


    std::string StisTimeScheduleData::getTextContent()
	{
        FUNCTION_ENTRY("getTextContent");
        reload();
        FUNCTION_EXIT;
        return m_textContent;
	}

	void StisTimeScheduleData::setTextContent(const std::string& textContent)
	{
		FUNCTION_ENTRY("setTextContent");
		reload();
		UPDATE_VALUE_IF(m_textContent, textContent)
		FUNCTION_EXIT;
	}

	std::string StisTimeScheduleData::getStartTime()
	{
		FUNCTION_ENTRY("getStartTime");
		reload();
		FUNCTION_EXIT;
		return m_startTime;
	}

	void StisTimeScheduleData::setStartTime(const std::string& startTime)
	{
		FUNCTION_ENTRY("setStartTime");
		reload();
		UPDATE_VALUE_IF(m_startTime, startTime)
		FUNCTION_EXIT;
	}
    
	std::string StisTimeScheduleData::getEndTime()
	{
		FUNCTION_ENTRY("getEndTime");
		reload();
		FUNCTION_EXIT;
		return m_endTime;
	}

	void StisTimeScheduleData::setEndTime(const std::string& endTime)
	{
		FUNCTION_ENTRY("setEndTime");
		reload();
		UPDATE_VALUE_IF(m_endTime, endTime)
		FUNCTION_EXIT;
	}

	unsigned long StisTimeScheduleData::getPriority()
	{
		FUNCTION_ENTRY("getPriority");
		reload();
		FUNCTION_EXIT
		return m_priority;
	}

	void StisTimeScheduleData::setPriority(unsigned long priority)
	{
		FUNCTION_ENTRY("setPriority");
		reload();
		UPDATE_VALUE_IF(m_priority, priority)
		FUNCTION_EXIT
	}

	TA_Base_Core::EDisplayMode StisTimeScheduleData::getDisplayMode()
	{
		FUNCTION_ENTRY("getDisplayMode");
		reload();
		FUNCTION_EXIT
		return m_displayMode;
	}

	void StisTimeScheduleData::setDisplayMode(TA_Base_Core::EDisplayMode displayMode)
	{
		FUNCTION_ENTRY("setDisplayMode");
		reload();
		UPDATE_VALUE_IF(m_displayMode, displayMode)
		FUNCTION_EXIT
	}

	TA_Base_Core::EScrollSpeed StisTimeScheduleData::getScrollSpeed()
	{
		FUNCTION_ENTRY("getScrollSpeed");
		reload();
		FUNCTION_EXIT
		return m_scrollSpeed;
	}

	
	void StisTimeScheduleData::setScrollSpeed(TA_Base_Core::EScrollSpeed scrollSpeed)
	{
		FUNCTION_ENTRY("setScrollSpeed");
		reload();
		UPDATE_VALUE_IF(m_scrollSpeed, scrollSpeed)
		FUNCTION_EXIT
	}

	unsigned long StisTimeScheduleData::getRepeatInterval()
	{
		FUNCTION_ENTRY("getRepeatInterval");
		reload();
		FUNCTION_EXIT
		return m_repeatInterval;
	}

	void StisTimeScheduleData::setRepeatInterval(unsigned long repeatInterval)
	{
		FUNCTION_ENTRY("setRepeatInterval");
		reload();
		UPDATE_VALUE_IF(m_repeatInterval, repeatInterval)
		FUNCTION_EXIT
	}

	TA_Base_Core::ELcdFontSize StisTimeScheduleData::getLcdFontSize()
	{
		FUNCTION_ENTRY("getLcdFontSize");
		reload();
		FUNCTION_EXIT
		return m_lcdFontSize;
	}

	void StisTimeScheduleData::setLcdFontSize(TA_Base_Core::ELcdFontSize lcdFontSize)
	{
		FUNCTION_ENTRY("setLcdFontSize");
		reload();
		UPDATE_VALUE_IF(m_lcdFontSize, lcdFontSize)
		FUNCTION_EXIT
	}
	
	TA_Base_Core::ELcdFontColour StisTimeScheduleData::getLcdFontColour()
	{
		FUNCTION_ENTRY("getLcdFontColour");
		reload();
		FUNCTION_EXIT
		return m_lcdFontColour;
	}

	void StisTimeScheduleData::setLcdFontColour(TA_Base_Core::ELcdFontColour lcdFontColour)
	{
		FUNCTION_ENTRY("setLcdFontColour");
		reload();
		UPDATE_VALUE_IF(m_lcdFontColour, lcdFontColour)
		FUNCTION_EXIT
	}

	TA_Base_Core::ELEDFontSize StisTimeScheduleData::getLedFontSize()
	{
		FUNCTION_ENTRY("getLedFontSize");
		reload();
		FUNCTION_EXIT
		return m_ledFontSize;
	}

	void StisTimeScheduleData::setLedFontSize(TA_Base_Core::ELEDFontSize ledFontSize)
	{
		FUNCTION_ENTRY("setLedFontSize");
		reload();
		UPDATE_VALUE_IF(m_ledFontSize, ledFontSize)
		FUNCTION_EXIT
	}

	TA_Base_Core::ELEDColour StisTimeScheduleData::getLedFontColour()
	{
		FUNCTION_ENTRY("getLedFontColour");
		reload();
		FUNCTION_EXIT
		return m_ledFontColour;
	}

	void StisTimeScheduleData::setLedFontColour(TA_Base_Core::ELEDColour ledFontColour)
	{
		FUNCTION_ENTRY("setLedFontColour");
		reload();
		UPDATE_VALUE_IF(m_ledFontColour, ledFontColour)
		FUNCTION_EXIT
	}

	unsigned long StisTimeScheduleData::getDisplayTime()
	{
		FUNCTION_ENTRY("getDisplayTime");
		reload();
		FUNCTION_EXIT
		return m_displayTime;
	}

	void StisTimeScheduleData::setDisplayTime(unsigned long displayTime)
	{
		FUNCTION_ENTRY("setDisplayTime");
		reload();
		UPDATE_VALUE_IF(m_displayTime, displayTime)
		FUNCTION_EXIT
	}

	TA_Base_Core::EJustification StisTimeScheduleData::getJustification()
	{
		FUNCTION_ENTRY("getJunstification");
		reload();
		FUNCTION_EXIT
		return m_justification;
	}

	void StisTimeScheduleData::setJustification(TA_Base_Core::EJustification justification)
	{
		FUNCTION_ENTRY("setJustification");
		reload();
		UPDATE_VALUE_IF(m_justification, justification)
		FUNCTION_EXIT
	}
    //
    // invalidate
    //
    void StisTimeScheduleData::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "StisTimeScheduleData::invalidate" );

        m_isValidData = false;

        // reset the modified flag here
        // the next get/set operation will cause a reload
        // but if invalidate is called just before hasChanged
        // (without a reload in between) this object will show dirty.
        // theres no point reloading until needed, so just set it to clean.
        m_hasChanged = false;
		reload();

        LOG_FUNCTION_EXIT( SourceInfo, "StisTimeScheduleData::invalidate" );
    }




    //
    // reload
    //
    void StisTimeScheduleData::reload()
    {
        FUNCTION_ENTRY( "reload" );
		//if already load,return
		if(m_isValidData)
		{
			FUNCTION_EXIT;
		    return;
		}
        
        TA_ASSERT( m_key >= 1 && m_key <= 20, 
            "This StisTimeSchedule does not yet exist in the database. Therefore it cannot be reloaded.");
 
        // Get a connection to the database.
         TA_Base_Core::IDatabase* databaseConnection = 
             TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd/*TD17787*/, TA_Base_Core::Read);

 
        // Create the SQL string to retrieve the data for the StisTimeSchedule
        // based upon the StisTimeSchedule key.
        //delete by lin
        //std::stringstream selectSql;
        //selectSql << "select ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_START_TIME_COLUMN  << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_END_TIME_COLUMN  << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_PRIORITY_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_MODE_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_SCROLL_SPEED_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_SIZE_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_COLOUR_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_SIZE_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_COLOUR_COLUMN << ", ";
        //selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_TIME_COLUMN << ", ";
		//selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_JUSTIFICATION_COLUMN << ", ";
		//selectSql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TEXT_CONTENT_COLUMN;
        //selectSql << " from " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME;
        //selectSql << " where " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN;
        //selectSql << "=" << m_key;

        // Execute the query. 
        //
        // The executeQuery() method can throw a TA_Base_Core::DatabaseException, as documented 
        // in the comment of this method.
        //
        // We are responsible for deleting the returned IData object when we're done with it,
        // so make it an std::auto_ptr, then it will automatically be cleaned up.

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_START_TIME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_END_TIME_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_MODE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_SCROLL_SPEED_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_SIZE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_COLOUR_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_SIZE_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_COLOUR_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_JUSTIFICATION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::STIS_TIME_SCHEDULE_TEXT_CONTENT_COLUMN );

        std::auto_ptr< TA_Base_Core::IData > data;
		try
		{
			//data = std::auto_ptr< TA_Base_Core::IData >( databaseConnection->executeQuery( selectSql.str(), columnNames ) );

			//waitforcheck delete by lin
			TA_Base_Core::SQLStatement strSql;
			
			databaseConnection->prepareSQLStatement(strSql, TI_STIS_TIME_SCHEDULE_SELECT_80002,	m_key);			
				
	        data = std::auto_ptr< TA_Base_Core::IData >( databaseConnection->executeQuery(strSql, columnNames));
	        
	        
			if( NULL == data.get() )
			{
				throw TA_Base_Core::DatabaseException("Get null data");
			}
		}
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
			throw;
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unkown exception", "");
			return;
		}

        // Check that one entry was found for this key.
        
		try
		{
	        if ( 0 == data->getNumRows() ) // No entry found with the specified key.
	        {
	            std::stringstream error;
	            error << "The StisTimeSchedule with the key " << m_key;
	            error << " was not found in the database table ";
	            error << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME << ".";
	            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
	        }
	        else if (1 < data->getNumRows()) // More than one entry found for the pkey
	        {
	            std::stringstream error;
	            error << "More than one StisTimeSchedule with the key " << m_key;
	            error << " was found in the database table ";
	            error << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME << ".";
	            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
	        }

	        // Assign the data as appropriate to the member variables.
	        // These calls can throw DataExceptions, as documented in the comment of this method.
			m_startTime = data->getStringData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_START_TIME_COLUMN );
			m_endTime = data->getStringData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_END_TIME_COLUMN );
			m_priority = data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_PRIORITY_COLUMN );
			m_displayMode = static_cast<TA_Base_Core::EDisplayMode>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_MODE_COLUMN ));
			m_scrollSpeed = static_cast<TA_Base_Core::EScrollSpeed>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_SCROLL_SPEED_COLUMN ));
			m_repeatInterval = data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN );
			m_lcdFontSize = static_cast<TA_Base_Core::ELcdFontSize>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_SIZE_COLUMN ));
			m_lcdFontColour = static_cast<TA_Base_Core::ELcdFontColour>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_COLOUR_COLUMN ));
			m_ledFontSize = static_cast<TA_Base_Core::ELEDFontSize>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_SIZE_COLUMN ));
			m_ledFontColour = static_cast<TA_Base_Core::ELEDColour>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_COLOUR_COLUMN ));
			m_displayTime = data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_TIME_COLUMN );
			m_justification = static_cast<TA_Base_Core::EJustification>(data->getUnsignedLongData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_JUSTIFICATION_COLUMN ));
			m_textContent = data->getStringData( 0, TA_TISAgentDAI::STIS_TIME_SCHEDULE_TEXT_CONTENT_COLUMN );
		}
		catch( const TA_Base_Core::DataException& ex )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException",  ex.what());
			throw;
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unkown exception", "");
		}
        // The data is now valid.
        m_isValidData = true;
        m_hasChanged = false;

        FUNCTION_EXIT;
    }

    void StisTimeScheduleData::updateExistingStisTimeSchedule()
    {
        FUNCTION_ENTRY( "updateExistingStisTimeSchedule" );
		TA_ASSERT(0 < m_key && m_key <= 20, "Invalid time schedule id");

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
		try
		{
	        TA_Base_Core::IDatabase* databaseConnection = 
	            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Tis_Cd, TA_Base_Core::Write);

	        // build the update statement
	        //delete by lin
	        //std::ostringstream sql;
	        //sql << "update " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TABLE_NAME;
	        //sql << " set " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_START_TIME_COLUMN << " = '";
			//sql << m_startTime << "', ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_END_TIME_COLUMN << " = '";
			//sql << m_endTime << "', ";
	        //sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_PRIORITY_COLUMN << " = " << m_priority << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_MODE_COLUMN << " = " << m_displayMode << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_SCROLL_SPEED_COLUMN << " = " << m_scrollSpeed << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_REPEAT_INTERVAL_COLUMN << " = " << m_repeatInterval << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_SIZE_COLUMN << " = " << m_lcdFontSize << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LCD_FONT_COLOUR_COLUMN << " = " << m_lcdFontColour << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_SIZE_COLUMN << " = " << m_ledFontSize << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_LED_FONT_COLOUR_COLUMN << " =	" << m_ledFontColour << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_DISPLAY_TIME_COLUMN << " = " << m_displayTime << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_JUSTIFICATION_COLUMN << " = " << m_justification << ", ";
			//sql << TA_TISAgentDAI::STIS_TIME_SCHEDULE_TEXT_CONTENT_COLUMN << " = '" << m_textContent << "' ";
	        //sql << " where " << TA_TISAgentDAI::STIS_TIME_SCHEDULE_ID_COLUMN << " = " << m_key;

	        // execute the sql
	       // databaseConnection->executeModification(sql.str().c_str());
	        
	        
			//waitforcheck delete by lin
			TA_Base_Core::SQLStatement strSql;
			
			databaseConnection->prepareSQLStatement(strSql, TI_STIS_TIME_SCHEDULE_SELECT_80003,
				m_startTime, m_endTime, m_priority, m_displayMode, m_scrollSpeed, m_repeatInterval,
				m_lcdFontSize, m_lcdFontColour, m_ledFontSize, m_ledFontColour, m_displayTime,   m_justification,
				m_textContent, m_key);
				
			databaseConnection->executeModification(strSql);

		}
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataBaseException",  ex.what());
			throw;
		}
        FUNCTION_EXIT;
    }
} // closes TA_Base_Core
