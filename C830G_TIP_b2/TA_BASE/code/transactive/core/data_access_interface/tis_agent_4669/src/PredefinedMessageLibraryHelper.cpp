/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryHelper.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PredefinedMessageLibraryHelper is an object that is held by PredefinedMessageLibraryData and ConfigPredefinedMessageLibraryData objects. 
  * It allows reading and writing a pre-defined message library to and from the database.
  */

#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: decorated type length exceeded, type was truncated

#include <vector>
#include <memory>

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryHelper.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{

	PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper( unsigned short version,
                                                                    const std::string& libraryType,
                                                                    std::vector<unsigned char>& libraryFileContent )
		: m_version( version ),
          m_libraryType( libraryType ),
          m_key(0),
		  m_isValidData(true),
          m_isNew(true),
          m_libraryFileContentPopulated(false)
	{
		m_libraryFileContent = libraryFileContent;
        m_libraryFileContentPopulated = true;

		// write this to the database straight away!
		addNewMessageLibraryData();
	}


	PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper()
		: m_isValidData( false ),
          m_key( 0 ),
          m_isNew( true ),
          m_libraryFileContentPopulated(false)
	{
	}


    PredefinedMessageLibraryHelper::PredefinedMessageLibraryHelper( unsigned long predefinedMessageLibraryKey )
        : m_isValidData( false ),
          m_key( predefinedMessageLibraryKey ),
          m_isNew( false ),
          m_libraryFileContentPopulated(false)
    {
	}


    unsigned long PredefinedMessageLibraryHelper::getKey()
    {
        // This method cannot be called until the key has been set.

        TA_ASSERT( 0 != m_key, "The message library key has not been initialised." );
        return m_key;
    }


	unsigned short PredefinedMessageLibraryHelper::getVersion()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the version can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_version;
	}


	std::string PredefinedMessageLibraryHelper::getLibraryType()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the library type can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_libraryType;
	}


	void PredefinedMessageLibraryHelper::getLibraryFileContent( std::vector<unsigned char>& fileContent )
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the library file content can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // the library file content is not used much and it is slow
        // so only load it when necessary
        if (!m_libraryFileContentPopulated && !m_isNew)
        {
            loadLibraryFileContent();
        }

		fileContent = m_libraryFileContent;
	}


	IPredefinedMessageLibrary::PredefinedMessageList PredefinedMessageLibraryHelper::getMessages()
	{
       TA_ASSERT(!m_isNew,"The data must be written to the database before the messages can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

		return m_messageList;
	}


    PredefinedMessage* PredefinedMessageLibraryHelper::getMessage(int librarySection, unsigned short messageTag)
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the messages can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // now find the matching message
        PredefinedMessage* matchingMessage = NULL;


        for (IPredefinedMessageLibrary::PredefinedMessageList::iterator iter = m_messageList.begin();
        iter != m_messageList.end(); iter++)
        {
            if ( ((*iter)->messageTag == messageTag) &&
                 ((*iter)->librarySection == librarySection) )
            {
                // found!
                matchingMessage = (*iter);
                break;
            }
        }


        return matchingMessage;
    }


    STISLedAttributes PredefinedMessageLibraryHelper::getDefaultSTISLedAttributes()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the default stis led attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_stisLedAttributes;
    }


    STISPlasmaAttributes PredefinedMessageLibraryHelper::getDefaultSTISPlasmaAttributes()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the stis plasma attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_stisPlasmaAttributes;
    }


    TTISLedAttributes PredefinedMessageLibraryHelper::getDefaultTTISLedAttributes()
    {
        TA_ASSERT(!m_isNew,"The data must be written to the database before the ttis led attributes can be retrieved");

        if (!m_isValidData && !m_isNew)
        {
            reload();
        }

        // if there are none, then this will return defaults
		return m_ttisLedAttributes;
    }


    void PredefinedMessageLibraryHelper::addMessage( const PredefinedMessage& theMessage )
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Write);
        
		// run a query to get a new primary key for this message
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_SEQUENCE;
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
            error << "Cannot insert predefined message into ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        unsigned long messageKey = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new predefined message. Inserting with key %d", messageKey);

        // Now we can formulate our SQL string
		std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "insert into ";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
		formatSQL << " (";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::DESCRIPTION_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::PRIORITY_COLUMN << ",";
        formatSQL << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::ENGLISH_ONLY_COLUMN << ") ";
		formatSQL << "values (";
		formatSQL << messageKey << ",";
		formatSQL << m_key << ",";
		formatSQL << theMessage.messageTag << ",";
		formatSQL << theMessage.librarySection << ",";
		formatSQL << "'" << databaseConnection->escapeInsertString(theMessage.description) << "',";
		formatSQL << theMessage.priority << ",";
        formatSQL << theMessage.repeatInterval << ",";
		formatSQL << "'" << databaseConnection->escapeInsertString(theMessage.message) << "',";
		formatSQL << theMessage.englishOnly << ")";

        databaseConnection->executeModification( formatSQL.str() );

		m_isNew = false;
	}

    void PredefinedMessageLibraryHelper::setDefaultDisplayAttributes( 
                                            const STISLedAttributes& stisLedAttributes,
                                            const STISPlasmaAttributes& stisPlasmaAttributes,
                                            const TTISLedAttributes& ttisLedAttributes )
    {
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Write);

        // check if the default attributes exist in the database
        // for this message library
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN;
        selectSql << " from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
        selectSql << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << " = " << m_key;

        std::vector< std::string > columnNames;
        columnNames.push_back( TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN );
        
        std::auto_ptr< TA_Base_Core::IData > data = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( selectSql.str(), columnNames ) );

        // if no entries were found, add one
        if ( 0 == data->getNumRows() )
        {
            // west side massiv insert statement
            std::ostringstream formatSQL;
		    formatSQL.str("");
            formatSQL << "insert into ";
		    formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
		    formatSQL << " (";
		    formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_COLUMN << ",";
            formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_JUSTIFICATION_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_INTENSITY_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_BACKGROUND_COLOUR_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_LATIN_FONT_SIZE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_JUSTIFICATION_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_INTENSITY_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_JUSTIFICATION_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_SIZE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_TYPE_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FOREGROUND_COLOUR_COLUMN << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_BACKGROUND_COLOUR_COLUMN;
            formatSQL << ") ";
		    formatSQL << "values (";
		    formatSQL << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PKEY_SEQUENCE << ",";
		    formatSQL << m_key << ",";
            formatSQL << stisLedAttributes.displayMode << ",";
		    formatSQL << stisLedAttributes.scrollSpeed << ",";
		    formatSQL << stisLedAttributes.validityPeriod << ",";
		    formatSQL << stisLedAttributes.repeatInterval << ",";
		    formatSQL << stisLedAttributes.displayTime << ",";
		    formatSQL << stisLedAttributes.justification << ",";
		    formatSQL << stisLedAttributes.fontSize << ",";
		    formatSQL << stisLedAttributes.intensity << ",";
		    formatSQL << stisLedAttributes.fontColour << ",";
		    formatSQL << stisLedAttributes.backgroundColour << ",";
		    formatSQL << ttisLedAttributes.displayMode << ",";
		    formatSQL << ttisLedAttributes.scrollSpeed << ",";
		    formatSQL << ttisLedAttributes.validityPeriod << ",";
		    formatSQL << ttisLedAttributes.repeatInterval << ",";
		    formatSQL << ttisLedAttributes.displayTime << ",";
		    formatSQL << ttisLedAttributes.latinFontSize << ",";
		    formatSQL << ttisLedAttributes.justification << ",";
		    formatSQL << ttisLedAttributes.intensity << ",";
		    formatSQL << stisPlasmaAttributes.displayMode << ",";
		    formatSQL << stisPlasmaAttributes.scrollSpeed << ",";
		    formatSQL << stisPlasmaAttributes.validityPeriod << ",";
		    formatSQL << stisPlasmaAttributes.repeatInterval << ",";
		    formatSQL << stisPlasmaAttributes.displayTime << ",";
		    formatSQL << stisPlasmaAttributes.justification << ",";
		    formatSQL << stisPlasmaAttributes.fontSize << ",";
		    formatSQL << stisPlasmaAttributes.fontType << ",";
		    formatSQL << stisPlasmaAttributes.fontColour << ",";
		    formatSQL << stisPlasmaAttributes.backgroundColour;
            formatSQL << ")";

            databaseConnection->executeModification( formatSQL.str() );
        }
        // if one entry was found, update it
        else if ( 1 == data->getNumRows() )
        {
            // update statement
            std::ostringstream formatSQL;
		    formatSQL.str("");
            formatSQL << "update " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE << " set ";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << " = " << stisLedAttributes.displayMode << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << " = " << stisLedAttributes.scrollSpeed << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << " = " << stisLedAttributes.validityPeriod << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << " = " << stisLedAttributes.repeatInterval << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << " = " << stisLedAttributes.displayTime << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_JUSTIFICATION_COLUMN << " = " << stisLedAttributes.justification << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN << " = " << stisLedAttributes.fontSize << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_INTENSITY_COLUMN << " = " << stisLedAttributes.intensity << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN << " = " << stisLedAttributes.fontColour << ",";
		    formatSQL << TA_TISAgentDAI::STIS_LED_DEFAULT_BACKGROUND_COLOUR_COLUMN << " = " << stisLedAttributes.backgroundColour << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << " = " << ttisLedAttributes.displayMode << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << " = " << ttisLedAttributes.scrollSpeed << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << " = " << ttisLedAttributes.validityPeriod << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << " = " << ttisLedAttributes.repeatInterval << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << " = " << ttisLedAttributes.displayTime << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_LATIN_FONT_SIZE_COLUMN << " = " << ttisLedAttributes.latinFontSize << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_JUSTIFICATION_COLUMN << " = " << ttisLedAttributes.justification << ",";
		    formatSQL << TA_TISAgentDAI::TTIS_LED_DEFAULT_INTENSITY_COLUMN << " = " << ttisLedAttributes.intensity << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_MODE_COLUMN << " = " << stisPlasmaAttributes.displayMode << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_SCROLL_SPEED_COLUMN << " = " << stisPlasmaAttributes.scrollSpeed << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_VALIDITY_PERIOD_COLUMN << " = " << stisPlasmaAttributes.validityPeriod << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_REPEAT_INTERVAL_COLUMN << " = " << stisPlasmaAttributes.repeatInterval << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_TIME_COLUMN << " = " << stisPlasmaAttributes.displayTime << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_JUSTIFICATION_COLUMN << " = " << stisPlasmaAttributes.justification << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_SIZE_COLUMN << " = " << stisPlasmaAttributes.fontSize << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_TYPE_COLUMN << " = " << stisPlasmaAttributes.fontType << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FOREGROUND_COLOUR_COLUMN << " = " << stisPlasmaAttributes.fontColour << ",";
		    formatSQL << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_BACKGROUND_COLOUR_COLUMN << " = " << stisPlasmaAttributes.backgroundColour;
            formatSQL << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << " = " << m_key;

            databaseConnection->executeModification( formatSQL.str() );
        }
        // multiple entries were found throw an error
        else
        {
            // unable to get a new key, throw an exception
            std::stringstream error;
            error << "Multiple default attributes rows for the message library with key ";
            error << m_key << " in ";
            error << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE, "" ));
        }
        
		m_isNew = false;
    }


    void PredefinedMessageLibraryHelper::invalidate()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "PredefinedMessageLibraryHelper::invalidate" );

        // This method cannot be called until the pkey has been set

        TA_ASSERT( !m_isNew, "Attempted to call invalidate() on a new PredefinedMessageLibrary entry.");
        
        m_isValidData = false;

        LOG_FUNCTION_EXIT( SourceInfo, "PredefinedMessageLibraryHelper::invalidate" );
    }


    void PredefinedMessageLibraryHelper::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "PredefinedMessageLibraryHelper::reload" );
        
        // Ensure the Predefined message library entry has already been written to the database. 
        // It doesn't make any sense to reload a predefined message library entry that doesn't
        // exist in the database.
        
        TA_ASSERT( 0 != m_key, 
            "This MessageLibrary does not yet exist in the database. Therefore it cannot be reloaded.");
  
		m_messageList.clear();
        
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Read);

		// check for existing libraries that match version and type
        std::ostringstream sql;
        sql << "select ";
        sql << TA_TISAgentDAI::VERSION_COLUMN << ", ";
        sql << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN;
        sql << " FROM " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        sql << " WHERE " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << " = " << m_key;
 
        // Set up the columnNames vector to be passed to executeQuery()
        std::vector<std::string> columnNames;
        columnNames.push_back( TA_TISAgentDAI::VERSION_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::LIBRARY_TYPE_COLUMN );

        // Execute the query. The method can throw a TA_Base_Core::DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned IData object when we're done with it
		std::auto_ptr< TA_Base_Core::IData > data;
        data = std::auto_ptr< TA_Base_Core::IData >(databaseConnection->executeQuery(sql.str(),columnNames) );

		if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            std::stringstream error;
			error << "No message library found for pkey = " << m_key;
            
            TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE,""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            std::stringstream error;
			error << "More than one Predefined Message Library found with pkey = " << m_key;

			TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

        // Assign the data as appropriate to the member variables.
        // These calls can throw DataExceptions, as documented in the comment of this method.
        m_version = (unsigned short)data->getIntegerData(0,TA_TISAgentDAI::VERSION_COLUMN);
        m_libraryType = data->getStringData(0,TA_TISAgentDAI::LIBRARY_TYPE_COLUMN);

		// get the messages that form this library
		std::stringstream messageSql;
		columnNames.clear();

		messageSql << "SELECT ";
        messageSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_TAG_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::DESCRIPTION_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::PRIORITY_COLUMN << ", ";
        messageSql << TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN << ", ";
		messageSql << TA_TISAgentDAI::ENGLISH_ONLY_COLUMN;
		messageSql << " FROM " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
		messageSql << " WHERE " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << " = " << m_key;
        messageSql << " ORDER BY " << TA_TISAgentDAI::LIBRARY_SECTION_COLUMN << ", " << TA_TISAgentDAI::MESSAGE_TAG_COLUMN;

		// Set up the column names for data we are expecting to receive
		columnNames.push_back( TA_TISAgentDAI::PREDEFINED_MESSAGE_PKEY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::PARENT_LIBRARY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::DESCRIPTION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::PRIORITY_COLUMN );
        columnNames.push_back( TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::ENGLISH_ONLY_COLUMN );

		int size = columnNames.size();

		TA_Base_Core::IData* messagedata = databaseConnection->executeQuery(messageSql.str(), columnNames);

        try
        {
            bool moreData = true;
            while (moreData)
            {
		        for ( unsigned long i = 0; i < messagedata->getNumRows(); ++i )
		        {   
			        PredefinedMessage* messageData = new PredefinedMessage();
		        
			        messageData->messageTag = messagedata->getIntegerData( i, TA_TISAgentDAI::MESSAGE_TAG_COLUMN );
			        messageData->librarySection = messagedata->getIntegerData( i, TA_TISAgentDAI::LIBRARY_SECTION_COLUMN );
			        messageData->description = messagedata->getStringData( i, TA_TISAgentDAI::DESCRIPTION_COLUMN );
			        messageData->priority = messagedata->getIntegerData( i, TA_TISAgentDAI::PRIORITY_COLUMN );
                    messageData->repeatInterval = messagedata->getIntegerData( i, TA_TISAgentDAI::REPEAT_INTERVAL_COLUMN );
			        messageData->message = messagedata->getStringData( i, TA_TISAgentDAI::MESSAGE_CONTENT_COLUMN ); 
			        messageData->englishOnly = (0 != messagedata->getIntegerData( i, TA_TISAgentDAI::ENGLISH_ONLY_COLUMN ) ); 

			        m_messageList.push_back( messageData );
		        }
            
                // clean up used data object
                delete messagedata;
                messagedata = NULL;

                // try to read more data
                moreData = databaseConnection->moreData(messagedata);
            }
        }
        catch(...)
        {
            // clean up in case an exception was thrown midway
            if (messagedata != NULL)
            {
                delete messagedata;
                messagedata = NULL;
            }

            // throw on
            throw;
        }


        // read default attributes
        std::stringstream attributeSql;
        attributeSql << "select ";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_JUSTIFICATION_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_INTENSITY_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_LED_DEFAULT_BACKGROUND_COLOUR_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_LATIN_FONT_SIZE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_JUSTIFICATION_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::TTIS_LED_DEFAULT_INTENSITY_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_MODE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_SCROLL_SPEED_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_VALIDITY_PERIOD_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_REPEAT_INTERVAL_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_TIME_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_JUSTIFICATION_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_SIZE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_TYPE_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FOREGROUND_COLOUR_COLUMN << ",";
		attributeSql << TA_TISAgentDAI::STIS_PLASMA_DEFAULT_BACKGROUND_COLOUR_COLUMN;
        attributeSql << " from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
        attributeSql << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN << " = " << m_key;

        columnNames.clear();
        columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_MODE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_SCROLL_SPEED_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_JUSTIFICATION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_INTENSITY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_LED_DEFAULT_BACKGROUND_COLOUR_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_MODE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_SCROLL_SPEED_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_LATIN_FONT_SIZE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_JUSTIFICATION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::TTIS_LED_DEFAULT_INTENSITY_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_MODE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_SCROLL_SPEED_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_VALIDITY_PERIOD_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_REPEAT_INTERVAL_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_TIME_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_JUSTIFICATION_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_SIZE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_TYPE_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FOREGROUND_COLOUR_COLUMN );
		columnNames.push_back( TA_TISAgentDAI::STIS_PLASMA_DEFAULT_BACKGROUND_COLOUR_COLUMN );
        
        std::auto_ptr< TA_Base_Core::IData > attributeData = std::auto_ptr< TA_Base_Core::IData >( 
            databaseConnection->executeQuery( attributeSql.str(), columnNames ) );

        // if there are default attributes set
        if ( 1 == attributeData->getNumRows() )
        {
            // read the values into the structures        
            m_stisLedAttributes.displayMode = static_cast<EDisplayMode>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_MODE_COLUMN ) );
		    m_stisLedAttributes.scrollSpeed = static_cast<EScrollSpeed>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_SCROLL_SPEED_COLUMN ) );
		    m_stisLedAttributes.validityPeriod = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN );
		    m_stisLedAttributes.repeatInterval = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN );
		    m_stisLedAttributes.displayTime = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_DISPLAY_TIME_COLUMN );
		    m_stisLedAttributes.justification = static_cast<EJustification>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_JUSTIFICATION_COLUMN ) );
		    m_stisLedAttributes.fontSize = static_cast<ELEDFontSize>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_SIZE_COLUMN ) );
		    m_stisLedAttributes.intensity = static_cast<ELEDIntensity>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_INTENSITY_COLUMN ) );
		    m_stisLedAttributes.fontColour = static_cast<ELEDColour>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_FONT_COLOUR_COLUMN ) );
		    m_stisLedAttributes.backgroundColour = static_cast<ELEDColour>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_LED_DEFAULT_BACKGROUND_COLOUR_COLUMN ) );

		    m_ttisLedAttributes.displayMode = static_cast<EDisplayMode>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_MODE_COLUMN ) );
		    m_ttisLedAttributes.scrollSpeed = static_cast<EScrollSpeed>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_SCROLL_SPEED_COLUMN ) );
		    m_ttisLedAttributes.validityPeriod = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_VALIDITY_PERIOD_COLUMN );
		    m_ttisLedAttributes.repeatInterval = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_REPEAT_INTERVAL_COLUMN );
		    m_ttisLedAttributes.displayTime = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_DISPLAY_TIME_COLUMN );
		    m_ttisLedAttributes.latinFontSize = static_cast<ELEDFontSize>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_LATIN_FONT_SIZE_COLUMN ) );
		    m_ttisLedAttributes.justification = static_cast<EJustification>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_JUSTIFICATION_COLUMN ) );
		    m_ttisLedAttributes.intensity = static_cast<ETTISLEDIntensity>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::TTIS_LED_DEFAULT_INTENSITY_COLUMN ) );

		    m_stisPlasmaAttributes.displayMode = static_cast<EDisplayMode>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_MODE_COLUMN ) );
		    m_stisPlasmaAttributes.scrollSpeed = static_cast<EScrollSpeed>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_SCROLL_SPEED_COLUMN ) );
		    m_stisPlasmaAttributes.validityPeriod = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_VALIDITY_PERIOD_COLUMN );
		    m_stisPlasmaAttributes.repeatInterval = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_REPEAT_INTERVAL_COLUMN );
		    m_stisPlasmaAttributes.displayTime = 
                attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_DISPLAY_TIME_COLUMN );
		    m_stisPlasmaAttributes.justification = static_cast<EJustification>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_JUSTIFICATION_COLUMN ) );
		    m_stisPlasmaAttributes.fontSize = static_cast<EPlasmaFontSize>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_SIZE_COLUMN ) );
		    m_stisPlasmaAttributes.fontType = static_cast<EFontType>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FONT_TYPE_COLUMN ) );
		    m_stisPlasmaAttributes.fontColour = static_cast<EPlasmaColour>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_FOREGROUND_COLOUR_COLUMN ) );
		    m_stisPlasmaAttributes.backgroundColour = static_cast<EPlasmaColour>
                ( attributeData->getIntegerData( 0, TA_TISAgentDAI::STIS_PLASMA_DEFAULT_BACKGROUND_COLOUR_COLUMN ) );
        }
        // otherwise there are none
        else if ( 0 == attributeData->getNumRows() )
        {
            std::stringstream error;
			error << "No default adhoc attributes found in "
                  << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
                  << " for pre-defined message library key = " << m_key;
            
            TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NO_VALUE,""));
        }
        // or multiple
        else
        {
            std::stringstream error;
            error << "More than one default adhoc attributes entry found in "
                  << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE
                  << " for pre-defined message library key = " << m_key;

			TA_THROW(TA_Base_Core::DataException(error.str().c_str(), TA_Base_Core::DataException::NOT_UNIQUE,""));
        }

        // The data is now valid.
        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "PredefinedMessageLibraryHelper::reload" );
    }


    void PredefinedMessageLibraryHelper::loadLibraryFileContent()
    {
        // Get a connection to the database.
        TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Read);

        std::stringstream whereSql;
		whereSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
		whereSql << " = " << m_key;

		// Get the predefined message library raw file content
		databaseConnection->readBLOB( TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN, 
			                          TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME,
			                          whereSql.str(),
                                      m_libraryFileContent );

        m_libraryFileContentPopulated = true;
    }


	void PredefinedMessageLibraryHelper::addNewMessageLibraryData()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Write);
   
		// run a query to get a new primary key for this message
        std::stringstream selectSql;
        selectSql << "select " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_SEQUENCE;
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
            error << "Cannot insert predefined message into ";
            error << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME << ".";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), TA_Base_Core::DataException::NO_VALUE, "" ));
        }

        // get the pkey to use
        m_key = data->getUnsignedLongData( 0, "NEXTVAL" );

        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Got a unique pkey for new predefined message library. Inserting with key %d", m_key);

        // Now we can formulate our SQL string
		std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "insert into ";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
		formatSQL << "(";
		formatSQL << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::VERSION_COLUMN << ",";
        formatSQL << TA_TISAgentDAI::LIBRARY_TYPE_COLUMN << ",";
		formatSQL << TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN << ") ";
		formatSQL << "values (";
		formatSQL << m_key << ",";
		formatSQL << m_version << ",";
		formatSQL << "'" << databaseConnection->escapeQueryString(m_libraryType) << "',";
        formatSQL <<  "EMPTY_BLOB())";      // must set the blob to empty (not null) or oci cant open it.

        databaseConnection->executeModification( formatSQL.str() );

		std::stringstream whereSql;
		whereSql << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN;
		whereSql << " = " << m_key;

		// Write the predefined message library raw file content
		databaseConnection->writeBLOB( TA_TISAgentDAI::LIBRARY_FILE_CONTENT_COLUMN, 
			TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME,
			whereSql.str(), m_libraryFileContent );

		m_isNew = false;
	}


	void PredefinedMessageLibraryHelper::deleteThisLibrary()
	{
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Tis_Cd, Write);
        
        // delete the default attributes
        std::ostringstream formatSQL;
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_TABLE;
        formatSQL << " where " << TA_TISAgentDAI::STIS_DEFAULT_ADHOC_ATTRIBUTES_PARENT_LIBRARY_COLUMN;
        formatSQL << " = " << m_key;

        databaseConnection->executeModification( formatSQL.str() );

        // Then delete all the messages that 'belong' to this library
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_TABLE_NAME;
        formatSQL << " where " << TA_TISAgentDAI::PARENT_LIBRARY_COLUMN << " = " << m_key;
 
        databaseConnection->executeModification( formatSQL.str() );

		// Then delete the message library itself
		formatSQL.str("");
        formatSQL << "delete from " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_TABLE_NAME;
        formatSQL << " where " << TA_TISAgentDAI::PREDEFINED_MESSAGE_LIBRARY_PKEY_COLUMN << " = " << m_key;

		databaseConnection->executeModification( formatSQL.str() );
	}



  
} // closes TA_Base_Core

