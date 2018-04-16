#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/exceptions/src/DataException.h"
#include "boost/shared_ptr.hpp"
#include "MessageLibraryParser.h"
#include "StringHelper.h"
#include "STISManager.h"
#include "StringConverter.h"
#include "TISMessageDataDefinitionsHelper.h"
#include <iomanip>
#include <boost/regex.hpp>



//two char make wchar_t, a is low byte, b is high byte
#define MAKEWCHAR(a, b) ((wchar_t)(((char)(a)) | ((wchar_t)((char)(b))) << 8 ))
#define GENSTRINGARGS(s) #s, L###s

namespace TA_IRS_App
{
    using namespace TA_Base_Core;
    ////////////////////////////////////////////////////
    // MessageLibraryParser
    ////////////////////////////////////////////////////
	const StringHolder MessageLibraryParser::DATABASE_LENGTH_TAG(GENSTRINGARGS([Database Length]));
	const StringHolder MessageLibraryParser::VERSION_TAG(GENSTRINGARGS([Version]));
	const StringHolder MessageLibraryParser::CREATED_BY_TAG(GENSTRINGARGS([Created By]));
	const StringHolder MessageLibraryParser::CREATE_DATE_TAG(GENSTRINGARGS([Create Date]));
	const StringHolder MessageLibraryParser::DESCRIPTION_TAG(GENSTRINGARGS([Description]));
	const StringHolder MessageLibraryParser::MESSAGE_TYPE_TAG(GENSTRINGARGS([Message Type]));
	const StringHolder MessageLibraryParser::NUMBER_MESSAGES_TAG(GENSTRINGARGS([Number of Predefined Message]));
	const StringHolder MessageLibraryParser::DEFAULT_ATTRIBUTE_TAG(GENSTRINGARGS([Default Attribute for Ad hoc Message]));
	const StringHolder MessageLibraryParser::CHECKSUM_TAG(GENSTRINGARGS([Database Checksum]));
	const StringHolder MessageLibraryParser::NEW_LINE(GENSTRINGARGS(\n));
	const StringHolder MessageLibraryParser::EQUALS(GENSTRINGARGS(=));
	const StringHolder MessageLibraryParser::DELIMITER(",", L","); // Can not pass comma to macro
	const StringHolder MessageLibraryParser::WINDOWS_NEW_LINE(GENSTRINGARGS(\r\n));
	const StringHolder MessageLibraryParser::EMPTY_STRING("", L"");
	const StringHolder MessageLibraryParser::TRIM_REGX("\\A\\s+|\\s+\\z", L"\\A\\s+|\\s+\\z");

    MessageLibraryParser::MessageLibraryParser( const std::vector<unsigned char>& fileContent,
                                                EMessageLibraryType libraryType,
                                                TA_Base_Core::EDataTypes conntype ) :
            m_fileContent( fileContent ),
            m_dbOperator( conntype ),
            m_libraryType( libraryType ),
            m_libraryVersion( 0 )
    {
    }

    MessageLibraryParser::DBOperator::DBOperator( TA_Base_Core::EDataTypes conntype ) : m_connType( conntype )
    {

    }

    void MessageLibraryParser::DBOperator::deleteLibrary( unsigned long version, const std::string& libraryType )
    {
		try
		{
	        std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary>
	        oldLibrary( TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary
	                    ( version,
	                      libraryType,
	                      m_connType ) );

	        if ( 0 != oldLibrary.get() )
	        {
	            oldLibrary->deleteThisLibrary();
	        }
		}
		catch(const TA_Base_Core::DataException&)
		{
		}
    }

    std::auto_ptr<TA_Base_Core::IConfigPredefinedMessageLibrary>
    MessageLibraryParser::DBOperator::createNewPredefinedMessageLibrary( unsigned long version,
                                                                         const std::string& libraryType,
                                                                         std::vector<unsigned char>& libraryFileContent )
    {

        return std::auto_ptr<TA_Base_Core::IConfigPredefinedMessageLibrary>
               ( TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().createNewPredefinedMessageLibrary
                 ( version, libraryType, libraryFileContent, m_connType ) );

    }

    ///////////////////////////////////////////////////////////
    // StisLibraryParser
    ///////////////////////////////////////////////////////////
    StisLibraryParser::StisLibraryParser(
        const std::vector<unsigned char>& fileContent,       
        TA_Base_Core::EDataTypes conntype
    ) : MessageLibraryParser( fileContent, STATION_LIBRARY, conntype )
    {
		m_stisFile = StringConverter::vectorCharToWstring(fileContent);
    }

    unsigned long StisLibraryParser::getLibraryVersion()
    {
        if ( 0 == m_libraryVersion )
        {
            int version = getNumber( getValue( m_stisFile, VERSION_TAG ) );
			if( version > 0 )
			{
				m_libraryVersion = (unsigned long)version;
			}
        }
        return m_libraryVersion;
    }

    void StisLibraryParser::parseLibrary()
    {
		FUNCTION_ENTRY("parseLibrary");	
        unsigned long nextVersion = getLibraryVersion();
        if ( nextVersion == 0 )
        {
            TA_THROW( TA_IRS_Core::TISAgentException( "The version number could not be extracted from the predefined message library file or value is zero" ) );
        }

		boost::wregex r1(L"\\[Message Type\\]="
			             L"([01])" // message type
						 L"(?:\\n|\\r\\n)\\[Number of Predefined Message\\]="
						 L"(?!000)(\\d{3})" // message number
						 L"(?:\\n|\\r\\n)"
						 L"(.*?)" // messages
						 L"((?=\\[Message Type\\])|(?=\\[Default Attribute for Ad hoc Message\\]))");

		boost::wregex r2(L"\\[Default Attribute for Ad hoc Message\\](?:\\n|\\r\\n)\\d{24}(?:\\n|\\r\\n)\\d{24}(?:\\n|\\r\\n)"
			             L"(\\d{24})" // attributes we use 
						 );

		boost::wsregex_iterator m1(m_stisFile.begin(), m_stisFile.end(), r1);
		boost::wsregex_iterator m2(m_stisFile.begin(), m_stisFile.end(), r2);
		boost::wsregex_iterator mEnd;
        std::vector< boost::shared_ptr<TA_Base_Core::PredefinedMessage> > messages;

		if(m1 == mEnd || m2 == mEnd)
		{
            TA_THROW( TA_IRS_Core::TISAgentException("The format of predefined message library is not correct"));
		}

		for(;m1 != mEnd; ++m1)
		{
			// message type
			std::wstring messageTypeString = (*m1)[1].str();
            int messageType = getNumber( messageTypeString );
			// message number
            std::wstring numberOfMessagesString = (*m1)[2].str();
            int numMessages = getNumber( numberOfMessagesString );
			// messages
			std::wstring messagesString = (*m1)[3].str();
			std::vector<std::wstring> messagesVec;
			boost::wregex r3(L"(.*?)" // per message
				             L"(?:\\n|\\r\\n)"
							 );
			boost::wsregex_iterator m3(messagesString.begin(), messagesString.end(), r3);
			for(;m3 != mEnd; ++m3)
			{
				messagesVec.push_back((*m3)[1].str());
			}
			if(messagesVec.size() != numMessages)
			{
                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - Message number not correct" ) );
			}
			for(std::vector<std::wstring>::iterator it = messagesVec.begin(); it != messagesVec.end(); ++it)
			{
                boost::shared_ptr<TA_Base_Core::PredefinedMessage>
					message( parseSTISMessage( *it, static_cast<TA_Base_Core::ELibrarySection>( messageType )));
                messages.push_back( message );
			}
		}

		// now attribute string
        std::wstring attributeString = (*m2)[1].str();
        TA_Base_Core::DisplayAttributes stisAttributes;
        TA_Base_Core::LCDAttributesSTIS stisLcdAttributes;
        TA_Base_Core::LEDAttributes stisLedAttributes;
		parseSTISAttributeString( StringConverter::wstringToString( attributeString), stisAttributes, stisLcdAttributes, stisLedAttributes );

		// save to database, delete if exist
        m_dbOperator.deleteLibrary( nextVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS );

        //  Write the library to the database
        std::auto_ptr<TA_Base_Core::IConfigPredefinedMessageLibrary> messageLibraryDB;
        try
        {
            // Create a new message library based on the type
            messageLibraryDB = m_dbOperator.createNewPredefinedMessageLibrary( nextVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS, m_fileContent );
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Created new predefined message library in database, attempting to add messages..." );
        }
        catch ( ... )
        {
            TA_THROW( TA_IRS_Core::TISAgentException( "Could not create a new predefined message library in the database" ) );
        }

        // Now add the messages to the database
        std::vector< boost::shared_ptr<TA_Base_Core::PredefinedMessage> >::iterator iter;
        try
        {
            for ( iter=messages.begin() ; iter != messages.end() ; iter++ )
            {
                // Add this predefined message to the database
                messageLibraryDB->addMessage( *( *iter ) );
            }
            messageLibraryDB->setDefaultSTISDisplayAttributes( stisAttributes, stisLcdAttributes, stisLedAttributes );
        }
        catch ( ... )
        {
            // Couldn't store the messages - delete what we have stored so far (the predefined message library table entry)
			try
			{
                messageLibraryDB->deleteThisLibrary();
			}
			catch( ... )
			{
			}
            TA_THROW( TA_IRS_Core::TISAgentException( "Could not add the predefined messages to the message table" ) );
        }
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Successfully added predefined messages to database" );
		FUNCTION_EXIT;
    }

    TA_Base_Core::PredefinedMessage* StisLibraryParser::parseSTISMessage( const std::wstring& stisMessageStr,
                                                                          TA_Base_Core::ELibrarySection librarySection )
    {
		FUNCTION_ENTRY("parseSTISMessage");

		// generate regular string
		//0[0-9A-Fa-f]{3},.{25},[1-3],(.{24},){2}.{6}\d{3}.{15}(,|).*
		// or 1[0-9A-Fa-f]{3},.{25},[4-6],(.{24},){2}.{6}\d{3}.{15}(,|).* 
		std::wstring regString;
		regString += (librarySection == TA_Base_Core::EMERGENCY_SECTION ? L"0":L"1");
		regString += L"[0-9A-Fa-f]{3},.{25},";
		regString += (librarySection == TA_Base_Core::EMERGENCY_SECTION ? L"[1-3]":L"[4-6]");
		regString += L",(.{24},){2}.{6}\\d{3}.{15}(,|).*";
		// check the format first
		boost::wsregex_iterator mEnd;
		boost::wregex r1(regString);
		if(false == boost::regex_search(stisMessageStr, r1))
		{
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,"The predefined message library format is not correct");
            TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library format is not correct"));
		}

		boost::wregex r2(
			L"(.{4})" // message tag
			L","
			L"(.{25})" // description
			L","
			L"(\\d)" // priority
			L",.{24},.{24},.{6}"
			L"(\\d{3})" //repeat interval
			L".{15}(?:,|)" // comment, this comma is not exist in icd, but current library file has it
			L"(.*)"  // message content
			);
		boost::wsregex_iterator m2(stisMessageStr.begin(), stisMessageStr.end(), r2);

        TA_Base_Core::PredefinedMessage* message = new TA_Base_Core::PredefinedMessage();
        message->librarySection = librarySection;
        message->englishOnly = false;
		// message id, 4 byte
        std::wstring messageTagString = (*m2)[1].str();
        message->messageTag = getHexNumber( messageTagString );
        // description - after the DELIMITER
        std::wstring description = (*m2)[2].str();
        // sometimes the names have leading whitespace within the library file - no point keeping it
        description = trim( description );
		message->description = StringConverter::wstringToString( description );

        // priority - after the description, and a DELIMITER
        std::wstring priorityString = (*m2)[3].str();
        message->priority = getNumber( priorityString );

        // display attributes - start after the DELIMITER
        // we only concern the 3rd attributes string
        std::wstring repeatIntervalString = (*m2)[4].str();
        message->repeatInterval = getNumber(repeatIntervalString);

        // process message content
        std::wstring messageString = (*m2)[5].str();
        //convert utf-16le string to utf-8 string, so it can save in std::string
#if !defined(WIN32)
		message->message = StringConverter::convertUtf32ToUtf8(messageString);
#else
		message->message = StringConverter::convertUtf16leToUtf8( messageString );
#endif
        // After a newline, this should be the start of the next message
        // the position of the next message
		FUNCTION_EXIT;
        return message;
    }

    void StisLibraryParser::parseSTISAttributeString( const std::string& attributeString,
                                                      TA_Base_Core::DisplayAttributes& stisAttributes,
                                                      TA_Base_Core::LCDAttributesSTIS& stisLcdAttributes,
                                                      TA_Base_Core::LEDAttributes& stisLedAttributes )
    {
		boost::regex r1(
			"^"
			"([0-9A-H])" //display mode
			"([08F])"    //scroll speed
			".{4}"
			"(\\d{3})"   //repeat interval
			"(\\d{3})"   //display time
			"([0-2])"    //justification
			"([1-3])"    //lcd font size
			"."
			"([0-8])"    //lcd font colour
			".{4}"
			"([1-3])"    //led font size
			"."
			"([2-4])"    //led font clour
			"."
			"$"
			);
		boost::sregex_iterator m(attributeString.begin(), attributeString.end(), r1);
		boost::sregex_iterator mEnd;
		if(m == mEnd)
		{
            TA_THROW( TA_IRS_Core::TISAgentException("The predefined message library has been incorrectly formatted - The default attributes format are not correct"));
		}
		//display mode
        char tmpCh = attributeString[0];
		stisAttributes.displayMode = TISMessageDataDefinitionsHelper::convertCharToDisplayMode(tmpCh);

        // scroll speed 0, 8 or F
		tmpCh = attributeString[1];
		stisAttributes.scrollSpeed = TISMessageDataDefinitionsHelper::convertCharToScrollSpeed(tmpCh);

        // repeat interval
        // 3 bytes 000 to 999
		std::stringstream tempStream;
		int temp = 0;
        tempStream << attributeString[6] << attributeString[7] << attributeString[8];
        tempStream >> std::dec >> temp;
        stisAttributes.repeatInterval = temp;

        // display time
        // 3 bytes 001 to 999
        tempStream.clear();
        tempStream.str( "" );
        tempStream << attributeString[9] << attributeString[10] << attributeString[11] << std::ends;
        tempStream >> std::dec >> temp;
        stisAttributes.displayTime = temp;

        // justification
        // 1 byte 0-2
        tempStream.clear();
        tempStream.str( "" );
        tempStream << attributeString[12] << std::ends;
        tempStream >> std::dec >> temp;
        stisAttributes.justification = static_cast<TA_Base_Core::EJustification>( temp );

        // lcd font size
        // 1 to 3
		tmpCh = attributeString[13];
		stisLcdAttributes.fontSize = TISMessageDataDefinitionsHelper::convertCharToLcdFontSize(tmpCh);
        // lcd font color
        // 0-8
        tempStream.clear();
        tempStream.str( "" );
        tempStream << attributeString[15];
        tempStream >> std::dec >> temp;
        stisLcdAttributes.fontColour = static_cast<TA_Base_Core::ELcdFontColour>( temp );

        // led font size
        // 1-3 (0 unused)
        tmpCh =  attributeString[20];
		stisLedAttributes.fontSize = TISMessageDataDefinitionsHelper::convertCharToLedFontSize(tmpCh);
        // led font colour
        // 2-4
        tmpCh = attributeString[22];
		stisLedAttributes.fontColour = TISMessageDataDefinitionsHelper::convertCharToLedFontColour(tmpCh);
    }
    ////////////////////////////////////////////////////////////
    // TtisLibraryParser
    ////////////////////////////////////////////////////////////
	const std::string TtisLibraryParser::TTIS_MESSAGE_MARK = "\"";
	const int TtisLibraryParser::MAX_MESSAGE_LENGTH = 256;
	const std::string TtisLibraryParser::m_comprisedFileSearchMark = "BGZFILESTART";
    TtisLibraryParser::TtisLibraryParser(
        const std::vector<unsigned char>& fileContent,        
        TA_Base_Core::EDataTypes conntype
    )
            :MessageLibraryParser( fileContent, TRAIN_LIBRARY, conntype ) 
    {
		m_ttisFile = StringConverter::vectorCharToString(fileContent);
		// Erase the comprised file content if exist
		std::string::size_type pos = m_ttisFile.find(m_comprisedFileSearchMark.c_str());
		if( pos != std::string::npos )
		{
			m_ttisFile.erase(pos);
		}
    }

	std::string TtisLibraryParser::getComprisedFileSearchMark()
	{
		return m_comprisedFileSearchMark;
	}

    void TtisLibraryParser::parseLibrary()
    {
		FUNCTION_ENTRY("parseLibrary");
        // get the version
        unsigned long version = getLibraryVersion();		
		if ( 0 == version )
		{
			TA_THROW( TA_IRS_Core::TISAgentException( "The version number could not be extracted from the predefined message library file or value is zero" ) );
		}
        // Read all the messages
        // ttis message only normal message, and not have priority, for database compatibility, all messages set priority to 4
        std::vector< boost::shared_ptr<TA_Base_Core::PredefinedMessage> > messages;
		boost::regex r1("\\[Number of Predefined Message\\]="
			            "(?!000)([0-9A-Fa-f]{3})" // message number
						"(?:\\n|\\r\\n)"
						"(.*)" // messages
						);
		boost::sregex_iterator m1(m_ttisFile.begin(), m_ttisFile.end(), r1);
		boost::sregex_iterator mEnd;
		if(m1 == mEnd)
		{
            TA_THROW(TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted"));
		}
		// message numbers
        std::string numberOfMessagesString = (*m1)[1].str();
		int numMessages = getHexNumber( numberOfMessagesString );
		// messages
		std::vector<std::string> messagesVec;
		std::string messagesString = (*m1)[2].str();
		boost::regex r2("([0-9A-Fa-f]{3}.*?)" // per message
			            "((\\n|\\r\\n)|$)"
						);
		boost::sregex_iterator m2(messagesString.begin(), messagesString.end(), r2);
		for(; m2!= mEnd; ++m2)
		{
			messagesVec.push_back((*m2)[1].str());
		}
		if(messagesVec.size() != numMessages)
		{
            TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - messages number is not correct"));
		}
		
		for(std::vector<std::string>::iterator it = messagesVec.begin(); it != messagesVec.end(); ++it)
		{
            boost::shared_ptr<TA_Base_Core::PredefinedMessage>
				message(parseTTISMessage(*it, static_cast<TA_Base_Core::ELibrarySection>( TA_Base_Core::NORMAL_SECTION )));
			messages.push_back(message);
		}
		// save to database
        // delete old library, if exist
        m_dbOperator.deleteLibrary( version, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS );

        // Write the library to the database
        std::auto_ptr<TA_Base_Core::IConfigPredefinedMessageLibrary> messageLibraryDB;

        // Create a new Predefined Message Library database access object
        try
        {
            messageLibraryDB = m_dbOperator.createNewPredefinedMessageLibrary( version, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS, m_fileContent );
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Created new predefined message library in database, attempting to add messages..." );
        }
        catch ( ... )
        {
            TA_THROW( TA_IRS_Core::TISAgentException( "Could not create a new predefined message library in the database" ) );
        }

        // Now add the messages to the database
        std::vector< boost::shared_ptr<TA_Base_Core::PredefinedMessage> >::iterator iter;

        try
        {
            for ( iter=messages.begin() ; iter != messages.end() ; iter++ )
            {
                // Add this predefined message to the database
                messageLibraryDB->addMessage( *( *iter ) );
            }
        }
        catch ( ... )
        {
            // Couldn't store the messages - delete what we have stored so far (the predefined message library table entry)
			try
			{
                messageLibraryDB->deleteThisLibrary();
			}
			catch( ... )
			{
			}
            TA_THROW( TA_IRS_Core::TISAgentException( "Could not add the predefined messages to the message table" ) );
        }

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Successfully added predefined messages to database" );
		FUNCTION_EXIT;
    }

    TA_Base_Core::PredefinedMessage* TtisLibraryParser::parseTTISMessage( const std::string& ttisMessageStr,
                                                                          TA_Base_Core::ELibrarySection librarySection )
    {
		FUNCTION_ENTRY("parseTTISMessage");
		std::auto_ptr<TA_Base_Core::PredefinedMessage> message(new TA_Base_Core::PredefinedMessage());
        message->librarySection = librarySection;
		// get current line
		std::string currentLine = ttisMessageStr;
		// use boost regx
		boost::regex regX("(?!000)([0-9A-Fa-f]{3})" // message tag, not zero
			              ","
						  "(.*?)" // description
						  ",.*?\""
						  "(.*?)" // message content
						  "\""
						  );
		boost::sregex_iterator matchIte(currentLine.begin(), currentLine.end(), regX);
		boost::sregex_iterator endIte;
		if(matchIte == endIte)
		{
            TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted") );
		}
		// message id
        std::string messageTagString = (*matchIte)[1].str();
        message->messageTag = getHexNumber( messageTagString );

        // description
        std::string description = (*matchIte)[2].str();
        // sometimes the names have leading whitespace within the library file - no point keeping it
        message->description = description;

        // ttis library file not contain priority, repeatInterval, perrun information, give it a default
        message->priority = 4;
        message->perRun= 1;
		message->repeatInterval = 0;

        // ttis only english text
        std::string messageStr = (*matchIte)[3].str();

        // truncate to the maximum message length
        if ( messageStr.size() > MAX_MESSAGE_LENGTH )
        {
            messageStr = messageStr.substr( 0, MAX_MESSAGE_LENGTH );
            // log the truncation
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "The message in section %d with tag %d was truncated because it was too long.",
                         librarySection, message->messageTag );
        }
        message->message = messageStr;

        // check to see if the messages other than english were empty
        message->englishOnly = true;
		FUNCTION_EXIT;
        return message.release();
    }

    unsigned long TtisLibraryParser::getLibraryVersion()
    {
        if ( 0 == m_libraryVersion )
        {
			std::string versionStr = getValue(m_ttisFile, VERSION_TAG);
			std::string::size_type beginMark = 0;
			std::string::size_type endMark = beginMark;
			std::stringstream ss;
			while( endMark != std::string::npos )
			{
				endMark = versionStr.find('.', beginMark);
				if(endMark == std::string::npos)
				{
					ss << std::setw(3) << std::setfill('0') << versionStr.substr(beginMark);
					break;
				}
				else
				{
					ss << std::setw(3) << std::setfill('0') << versionStr.substr(beginMark, endMark - beginMark);
					beginMark = endMark + 1;
				}			
			}			
			m_libraryVersion = StringHelper::libraryVersionStrToNumber(ss.str());
        }
        return m_libraryVersion;
    }
}
