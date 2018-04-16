 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/MessageLibraryManager.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4503)
#endif

#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryData.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"

#include "core/ftp/src/FTPManager.h"
#include "core/ftp/src/FTPException.h"

#include "app/signs/tis_agent/src/Exceptions.h"
#include "core/exceptions/src/DataException.h"

#include "core/message/types/TISAlarms_MessageTypes.h"

//TES831 Events not logged!
#include "core/message/types/TISAudit_MessageTypes.h"
//TES831 Events not logged!


#include "boost/regex.hpp"

namespace
{
    //
    // Constants for message library parsing
    //

    static const std::string    DATABASE_LENGTH_TAG    = "[Database Length]";
    static const std::string    VERSION_TAG            = "[Version]";
    static const std::string    CREATED_BY_TAG         = "[Created By]";
    static const std::string    CREATE_DATE_TAG        = "[Create Date]";
    static const std::string    DESCRIPTION_TAG        = "[Description]";
    static const std::string    MESSAGE_TYPE_TAG       = "[Message Type]";
    static const std::string    NUMBER_MESSAGES_TAG    = "[Number of Predefined Message]";
    static const std::string    DEFAULT_ATTRIBUTE_TAG  = "[Default Attribute for Ad hoc Message]";
    static const std::string    CHECKSUM_TAG           = "[Database Checksum]";
    static const std::string    LINE_BREAK             = "\n";
    static const std::string    EQUALS                 = "=";
    static const std::string    DELIMITER              = ",";
    static const char           MESSAGE_BREAK[2]       = { char (0xFF), char (0xFF) };
    static const int            MAX_MESSAGE_LENGTH     = 256;

    //
    // Utility for trimming a string (uses boost)
    //
    static const boost::regex e("\\A\\s+|\\s+\\z");
    std::string trim(const std::string & s)
    {
        return boost::regex_replace(s, e, "");
    }

}

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

	void MessageLibraryManager::parseMessageLibrary( EMessageLibraryType type, const std::string& filename )
	{
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::parseMessageLibrary() is only available at the OCC" );

		// Parse the 'file'  (stored in a buffer at this stage)
		// store to the database
		//m_libraryFileBuffer;

		if( m_libraryFileBuffer.size() == 0 )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "The retrieved predefined message library file is empty" );
			TA_THROW( TA_IRS_Core::TISAgentException( "The retrieved predefined message library file is empty" ) );
		}

		// Make a copy of the file content vector into a std::stringstream
		std::stringstream fileStream;

		unsigned int i;
		for( i=0 ; i < m_libraryFileBuffer.size() ; i++ )
		{
			fileStream << m_libraryFileBuffer[i];
		}
	
		std::string file = fileStream.str();


        //
        // Read all the name-value pair attributes at the top of the file
        //

        // get the database length
        std::string databaseLengthString = getValue(file, DATABASE_LENGTH_TAG);

        // get the version
        std::string versionString = getValue(file, VERSION_TAG);

        // get created by
        std::string createdByString = getValue(file, CREATED_BY_TAG);

        // get the description
        std::string descriptionString = getValue(file, DESCRIPTION_TAG);

        // get the create date
        std::string createDateString = getValue(file, CREATE_DATE_TAG);

        // the only mandatory field above is the version
        // check the version is valid
        int version = getNumber(versionString);
        if (version <= 0)
        {
            m_libraryFileBuffer.clear();
            TA_THROW( TA_IRS_Core::TISAgentException( "The version number could not be extracted from the predefined message library file" ) );
        }

        switch( type )
		{
		case STATION_LIBRARY:

			m_nextStationLibraryVersion = version;

			if( m_nextStationLibraryVersion != m_indicatedVersion )
			{
				m_libraryFileBuffer.clear();
				TA_THROW( TA_IRS_Core::TISAgentException( "The version number in the predefined STIS message library file is different to the version indicated by the STIS server" ) );
			}
			break;

		case TRAIN_LIBRARY:
			m_nextTrainLibraryVersion = version;

			if( m_nextTrainLibraryVersion != m_indicatedVersion )
			{
				m_libraryFileBuffer.clear();
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "The version number in the predefined TTIS message library file is different to the version indicated by the STIS server.$$ The version should be [%d], version in file [%d]",
                    m_indicatedVersion,  m_nextTrainLibraryVersion);
				TA_THROW( TA_IRS_Core::TISAgentException( "The version number in the predefined TTIS message library file is different to the version indicated by the STIS server" ) );
			}
		}


        //
        // Read all the messages
        //



        // the normal and emergency messages
        std::vector<TA_Base_Core::PredefinedMessage*> normalMessages;
        std::vector<TA_Base_Core::PredefinedMessage*> emergencyMessages;

        // flags to indicate the required sections were found
        // cannot simply use normalMessages.size() because what if the section contained no messages?
        // that is a valid case.
        bool normalMessageSectionParsed = false;
        bool emergencyMessageSectionParsed = false;

        // this try catch will clean up memory if an exception is thrown
        try
        {
            // find the first message block
            std::string::size_type messageBlockStart = file.find(MESSAGE_TYPE_TAG);

            // for each message block
            while (messageBlockStart != std::string::npos)
            {
                // find the end of this message block
                // (the start of the next message block)
                std::string::size_type messageBlockEnd = file.find(MESSAGE_TYPE_TAG, messageBlockStart + 1);
                // or the start of the default adhoc attributes (for the last message block)
                bool lastBlock = false;
                if (messageBlockEnd == std::string::npos)
                {
                    messageBlockEnd = file.find(DEFAULT_ATTRIBUTE_TAG, messageBlockStart + 1);
                    lastBlock = true;
                }

                // now parse this message block
        
                // check the message type.
                std::string messageTypeString = getValue(file, MESSAGE_TYPE_TAG, messageBlockStart);
                int messageType = getNumber(messageTypeString);
                if (messageType < 0)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Error parsing predefined message library at byte %d Unable to extract message type.",
                        messageBlockStart );

			        TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - an invalid message type was specified" ) );
                }

                // put the messages into the correct section
                std::vector<TA_Base_Core::PredefinedMessage*>* messages = NULL;
                if (messageType == TA_Base_Core::EMERGENCY_SECTION)
                {
                    messages = &emergencyMessages;
                    emergencyMessageSectionParsed = true;
                }
                else if (messageType == TA_Base_Core::NORMAL_SECTION)
                {
                    messages = &normalMessages;
                    normalMessageSectionParsed = true;
                }

                // only parse the block if it is a message type we care about
                if ( messages != NULL )
                {
                    // check the number of messages
                    std::string numberOfMessagesString = getValue(file, NUMBER_MESSAGES_TAG, messageBlockStart);

                    int numMessages = getNumber(numberOfMessagesString);
                    if (numMessages < 0)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Error parsing predefined message library at byte %d Unable to extract the number of messages.",
                            messageBlockStart );

			            TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - an invalid number of messages was specified" ) );
                    }

                    // now parse the messages

                    int messagesRead = 0;
                    std::string::size_type currentPosition = file.find(NUMBER_MESSAGES_TAG, messageBlockStart + 1);
                    currentPosition = file.find(LINE_BREAK, currentPosition) + 1;

                    // for each message within the current block
                    while ( (messagesRead < numMessages) && (currentPosition < messageBlockEnd) )
                    {
                        // parse the next message
                        TA_Base_Core::PredefinedMessage* message = parseMessage(
                            file,
                            static_cast<TA_Base_Core::ELibrarySection>(messageType),
                            type,
                            currentPosition );
                    
                        // add the message to the list of messages
                        messages->push_back(message);
                    
                        messagesRead++;
                    }

                    if (messagesRead < numMessages)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                                    "Error parsing predefined message library at message section %d, Only %d of %d messages have been specified.",
                                    messageType, messagesRead, numMessages );

                        TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - not all indicated messages were specified" ) );
                    }
                }


                // reset for the next block
                if ( !lastBlock )
                {
                    messageBlockStart = messageBlockEnd;
                }
                else
                {
                    messageBlockStart = std::string::npos;
                }
            }

            // make sure both library sections were parsed
            if (!normalMessageSectionParsed)
            {
                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - The normal section was not specified" ) );
            }

            if (!emergencyMessageSectionParsed)
            {
                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - The emergency section was not specified" ) );
            }


            //
            // read the default adhoc attributes
            //

            std::string::size_type defaultAttributesStart = file.find(DEFAULT_ATTRIBUTE_TAG);
            if (defaultAttributesStart == std::string::npos)
            {
                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - The default adhoc attributes section was not specified" ) );
            }
            defaultAttributesStart = file.find(LINE_BREAK, defaultAttributesStart);

            // store the stis led attributes
            std::string::size_type defaultAttributesEnd = file.find(LINE_BREAK, defaultAttributesStart+1);
	        std::string stisLedAttributesString = file.substr( defaultAttributesStart+1,
                                                               (defaultAttributesEnd-1)-(defaultAttributesStart+1) );
            TA_Base_Core::STISLedAttributes stisLedAttributes = 
                parseStisLedAttributeString(stisLedAttributesString);

            // store the ttis led attributes
            defaultAttributesStart = defaultAttributesEnd + 1;
            defaultAttributesEnd = file.find(LINE_BREAK, defaultAttributesStart);
            std::string ttisLedAttributesString = file.substr( defaultAttributesStart,
                                                               (defaultAttributesEnd-1) - defaultAttributesStart );
            TA_Base_Core::TTISLedAttributes ttisLedAttributes = 
                parseTtisLedAttributeString(ttisLedAttributesString);


            // store the stis plasma attributes
            defaultAttributesStart = defaultAttributesEnd + 1;
            defaultAttributesEnd = file.find(LINE_BREAK, defaultAttributesStart);
            std::string stisPlasmaAttributesString = file.substr( defaultAttributesStart,
                                                               (defaultAttributesEnd-1) - defaultAttributesStart );
            TA_Base_Core::STISPlasmaAttributes stisPlasmaAttributes =
                parseStisPlasmaAttributeString(stisPlasmaAttributesString);


            //
            // Before writing the library to the database - remove any old message libraries
            // that have the same version as the current message library
            //

            try
			{
				TA_Base_Core::IPredefinedMessageLibrary* oldLibrary = NULL;

                switch( type )
			    {
			    case STATION_LIBRARY:
				    oldLibrary = 
					    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
                            m_nextStationLibraryVersion,
                            TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
			    break;

			    case TRAIN_LIBRARY:
				    oldLibrary = 
					    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
                            m_nextTrainLibraryVersion,
                            TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS);
				    break;
			    }

				if( oldLibrary )
				{
					oldLibrary->deleteThisLibrary();
				}

				delete oldLibrary;
                oldLibrary = NULL;
			}
			catch( const TA_Base_Core::DataException& de)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", de.what());
			}
			catch( const TA_Base_Core::DatabaseException& dbe)
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", dbe.what());
			}
            catch (...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "...", "while attempting to delete the old predefined message library");
            }


            //
            //  Write the library to the database
            //

		    TA_Base_Core::IConfigPredefinedMessageLibrary* messageLibraryDB = NULL;

		    // Create a new Predefined Message Library database access object
		    try
		    {
			    // Create a new message library based on the type
			    switch( type )
			    {
			    case STATION_LIBRARY:
				    messageLibraryDB = 
					    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().createNewPredefinedMessageLibrary(m_nextStationLibraryVersion,TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS,m_libraryFileBuffer);
			    break;

			    case TRAIN_LIBRARY:
				    messageLibraryDB = 
					    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().createNewPredefinedMessageLibrary(m_nextTrainLibraryVersion,TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS,m_libraryFileBuffer);
				    break;
			    }

			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Created new predefined message library in database, attempting to add messages..." );
			    
		    }
		    catch( ... )
		    {
			    TA_THROW( TA_IRS_Core::TISAgentException( "Could not create a new predefined message library in the database" ) );	
		    }

		    // Now add the messages to the database
		    std::vector<TA_Base_Core::PredefinedMessage*>::iterator iter;

		    try
		    {
		        for( iter=emergencyMessages.begin() ; iter != emergencyMessages.end() ; iter++ )
		        {
			        // Add this predefined message to the database
			        messageLibraryDB->addMessage(*(*iter));

			        // and delete the local object
			        delete (*iter);
			        (*iter) = NULL;
		        }

		        for( iter=normalMessages.begin() ; iter != normalMessages.end() ; iter++ )
		        {
			        // Add this predefined message to the database
			        messageLibraryDB->addMessage(*(*iter));

			        // and delete the local object
			        delete (*iter);
			        (*iter) = NULL;
		        }
		    }
		    catch( ... )
		    {
			    // Couldn't store the messages - delete what we have stored so far (the predefined message library table entry)
			    messageLibraryDB->deleteThisLibrary();
			    TA_THROW( TA_IRS_Core::TISAgentException( "Could not add the predefined messages to the message table" ) );	
		    }


            // now set the default attributes
            try
		    {
		        // Add this predefined message to the database
			    messageLibraryDB->setDefaultDisplayAttributes( stisLedAttributes,
                                                               stisPlasmaAttributes,
                                                               ttisLedAttributes);
		    }
		    catch( ... )
		    {
			    // Couldn't store the attributes - delete what we have stored so far
			    messageLibraryDB->deleteThisLibrary();
			    TA_THROW( TA_IRS_Core::TISAgentException( "Could not add the default adhoc attributes for the message library" ) );	
		    }


		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Successfully added predefined messages to database" );

		    // Only set the next version now, since the new predefined message library has been added successfully 
		    // to the database.
		    switch( type )
		    {
			    case STATION_LIBRARY:
					//hongran++ TD17500
					//We just set the version of lib version of central DB. Before, we set the occ lib version
				    StatusMonitor::getInstance()->setNextISCSStationLibraryVersion_central( m_nextStationLibraryVersion );
					//++hongran TD17500
				    break;

			    case TRAIN_LIBRARY:
					//hongran++ TD17500
					//We just set the version of lib version of central DB. Before, we set the occ lib version
				    StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion_central( m_nextTrainLibraryVersion );
					//++hongran TD17500
				    break;

			    default:
				    break;
		    }
		    
		    m_libraryFileBuffer.clear();

		    delete messageLibraryDB;
		    messageLibraryDB = NULL;
        }
        catch (...)
        {
            // clean up the messages that have been stored so far to prevent memory leaks
            std::vector<TA_Base_Core::PredefinedMessage*>::iterator iter;
            for( iter=emergencyMessages.begin() ; iter != emergencyMessages.end() ; iter++ )
		    {
			    // delete the local object
			    delete (*iter);
			    (*iter) = NULL;
		    }
            emergencyMessages.clear();

		    for( iter=normalMessages.begin() ; iter != normalMessages.end() ; iter++ )
		    {
    		    // delete the local object
			    delete (*iter);
			    (*iter) = NULL;
		    }
            normalMessages.clear();

            m_libraryFileBuffer.clear();

            throw;
        }
	}


    TA_Base_Core::PredefinedMessage* MessageLibraryManager::parseMessage( const std::string& file,
                                                                         TA_Base_Core::ELibrarySection librarySection,
                                                                         EMessageLibraryType type,
                                                                         std::string::size_type& currentPosition )
    {
        TA_Base_Core::PredefinedMessage* message = new TA_Base_Core::PredefinedMessage();
        message->librarySection = librarySection;

        try
        {
            // read the message tag (3 bytes - but only the second two are used)
            std::string messageTagString = file.substr(currentPosition + 1, 2);
            int messageTag = getHexNumber(messageTagString);
            if (messageTag < 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message tag.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - an invalid message tag was specified" ) );
            }
            message->messageTag = messageTag;

            // description - after the DELIMITER
            currentPosition += 4;
            std::string::size_type endMarker = file.find(DELIMITER, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message description, DELIMITER expected.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - an invalid message description was specified" ) );
            }
            std::string description = file.substr(currentPosition, endMarker - currentPosition);
            // sometimes the names have leading whitespace within the library file - no point keeping it
            description = trim( description );
            message->description = description;


            // priority - after the description, and a DELIMITER
            currentPosition = endMarker + 1;
            std::string priorityString = file.substr(currentPosition, 1);
            int priority = getNumber(priorityString);
            if (priority < 0)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message priority.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - an invalid message priority was specified" ) );
            }
            message->priority = priority;

            // validate the priority according to the current section
            if ( (librarySection == TA_Base_Core::NORMAL_SECTION) &&
                 ( (message->priority < 4) || (message->priority > 8) ) )
            {
                std::stringstream error;
                error << "The predefined message library has been incorrectly formatted "
                      << "- A message in the normal section had a non normal priority "
                      << priority;

                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.str().c_str() );
			    TA_THROW( TA_IRS_Core::TISAgentException( error.str().c_str() ) );
            }
            else if ( (librarySection == TA_Base_Core::EMERGENCY_SECTION) &&
                      ( (message->priority < 1) || (message->priority > 3) ) )
            {
                std::stringstream error;
                error << "The predefined message library has been incorrectly formatted "
                      << "- A message in the emergency section had a non emergency priority "
                      << priority;

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, error.str().c_str() );
			    TA_THROW( TA_IRS_Core::TISAgentException( error.str().c_str() ) );
            }
    
            // display attributes - start after the DELIMITER
            currentPosition += 2;
            // STIS LED attributes
            endMarker = file.find(DELIMITER, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message display attributes.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - invalid message display attributes were specified" ) );
            }
            std::string stisLedAttrStr = file.substr(currentPosition, endMarker - currentPosition);
			// TTIS LED attributes
            currentPosition = endMarker + 1;
            endMarker = file.find(DELIMITER, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message display attributes.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - invalid message display attributes were specified" ) );
            }
            std::string ttisLedAttrStr = file.substr(currentPosition, endMarker - currentPosition);
			// STIS Plasma attributes
            currentPosition = endMarker + 1;
            endMarker = file.find(DELIMITER, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Unable to extract message display attributes.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - invalid message display attributes were specified" ) );
            }
            std::string stisPlasmaAttrStr = file.substr(currentPosition, endMarker - currentPosition);

            switch( type )
		    {
		    case STATION_LIBRARY:
                // populate the repeat interval from the STIS Plasma attributes
                message->repeatInterval = parseStisPlasmaAttributeString(stisPlasmaAttrStr).repeatInterval;
       		    break;
		    case TRAIN_LIBRARY:
                // populate the repeat interval from the TTIS LED attributes
                message->repeatInterval = parseTtisLedAttributeString(ttisLedAttrStr).repeatInterval;
                break;
		    }


            // english text - starts after the next DELIMITER
            currentPosition = endMarker + 1;
            endMarker = file.find(MESSAGE_BREAK, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Cannot find the end of the english message.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - An invalid english message content was specified" ) );
            }
            std::string englishUnicodeMessageString = file.substr(currentPosition, endMarker - currentPosition);

            // convert to ascii
            std::stringstream tempstream;
			for( unsigned int j = 1 ; j < englishUnicodeMessageString.size() ; j+=2 )
			{
				tempstream << englishUnicodeMessageString[j];	
			}
            std::string englishMessageString = tempstream.str();

            // truncate to the maximum message length
            if (englishMessageString.length() > MAX_MESSAGE_LENGTH)
            {
                englishMessageString = englishMessageString.substr(0, MAX_MESSAGE_LENGTH);

                // log the truncation
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "The message in section %d with tag %d was truncated because it was too long.",
                    librarySection, messageTag );
            }
            message->message = englishMessageString;


            // chinese text
            currentPosition = endMarker + 2;
            endMarker = file.find(MESSAGE_BREAK, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Cannot find the end of the chinese message.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - An invalid chinese message content was specified" ) );
            }
            std::string::size_type otherlanguagesSize = endMarker - currentPosition;
            // malay text
            currentPosition = endMarker + 2;
            endMarker = file.find(MESSAGE_BREAK, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Cannot find the end of the malay message.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - An invalid malay message content was specified" ) );
            }
            otherlanguagesSize += endMarker - currentPosition;
            // tamil text
            currentPosition = endMarker + 2;
            endMarker = file.find(MESSAGE_BREAK, currentPosition);
            if (endMarker == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Cannot find the end of the tamil message.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - An invalid tamil message content was specified" ) );
            }
            otherlanguagesSize += endMarker - currentPosition;

            // check to see if the messages other than english were empty
            message->englishOnly = (otherlanguagesSize == 0); 

            // After a newline, this should be the start of the next message
            currentPosition = file.find(LINE_BREAK, endMarker);
            if (currentPosition == std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Error parsing predefined message library at byte %d Cannot find the end of the message line.",
                    currentPosition );

                TA_THROW( TA_IRS_Core::TISAgentException( "The predefined message library has been incorrectly formatted - An invalid message definition was found" ) );
            }

            // the position of the next message
            currentPosition++;
        }
        catch (...)
        {
            // prevent memory leaks
            delete message;
            message = NULL;

            throw;
        }

        return message;
    }


    std::string MessageLibraryManager::getValue( const std::string& file,
                                                 const std::string& tag,
                                                 std::string::size_type findFrom ) const
    {
        std::string value = "";

        // find the tag
        std::string::size_type start = file.find(tag, findFrom);

        // if it was found
        if (start != std::string::npos)
        {
            // find the end of the line
            std::string::size_type end = file.find(LINE_BREAK, start + 1);

            // find the equals
            start = file.find(EQUALS, start+1);

            // trim the leading and trailing whitespace (if any)
            value = trim( file.substr(start + 1, end - (start+1)) );
        }

        return value;
    }


    int MessageLibraryManager::getNumber( const std::string& numberStr ) const
    {
        std::stringstream numberStream;
        numberStream << numberStr;

        int number = -1;
        numberStream >> number;

        return number;
    }


    int MessageLibraryManager::getHexNumber( const std::string& numberStr ) const
    {
        std::stringstream numberStream;
        numberStream << numberStr;

        int number = -1;
        numberStream >> std::hex >> number;

        return number;
    }


    TA_Base_Core::STISLedAttributes MessageLibraryManager::parseStisLedAttributeString(const std::string& attributeString)
    {
        if (attributeString.length() != 24)
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED attributes are not 24 bytes.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        TA_Base_Core::STISLedAttributes attributes;

        std::stringstream tempStream;
        int temp = 0;

        // Display mode
        // 0 to 7
        // force the buffer flushed with ends - flush doesnt work
        // this is so this will work on windows and not just solaris
        tempStream << attributeString[0] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 7))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED display mode is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayMode = static_cast<TA_Base_Core::EDisplayMode>(temp);

        // scroll speed
        // 0 instant on
        // 8 medium
        // 15 (F) fast
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[1] << std::ends;
        tempStream >> std::hex >> temp;
        switch( temp )
        {
        // can be 0 to F
        // only 3 values are catered for here
        // pick the closest
        case 0: case 1: case 2: case 3: case 4: case 5:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_SLOW;
            break;
        case 6: case 7: case 8: case 9: case 0xA:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
            break;
        case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_FAST;
            break;
        default:
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED scroll speed is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        // validity period
        // 4 bytes - unused (0000 to 1440)
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[2] 
            << attributeString[3]
            << attributeString[4]
            << attributeString[5] << std::ends;
        tempStream >> std::dec >> temp;
        // no validation - not used
        attributes.validityPeriod = temp;

        // repeat interval
        // 3 bytes 000 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[6] 
            << attributeString[7]
            << attributeString[8] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED repeat interval is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.repeatInterval = temp;

        // display time
        // 3 bytes 001 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[9] 
            << attributeString[10]
            << attributeString[11] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED display time is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayTime = temp;

        // justification
        // 1 byte 0-2
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[12] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 2))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED justification is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.justification = static_cast<TA_Base_Core::EJustification>(temp);

        // font size
        // 0-3
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[13] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 3))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED font size is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.fontSize = static_cast<TA_Base_Core::ELEDFontSize>(temp);

        // intensity
        // 0-8
        // 1-low
        // 4- Medium
        // 8- High
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[14] << std::ends;
        tempStream >> std::dec >> temp;
        switch( temp )
        {
        // can be 0 to 8
        // only 3 values are catered for here
        // pick the closest
        case 0: case 1: case 2:
            attributes.intensity = TA_Base_Core::INTENSITY_LOW;
            break;
         case 3: case 4: case 5:
            attributes.intensity = TA_Base_Core::INTENSITY_MEDIUM;
            break;
         case 6: case 7: case 8:
            attributes.intensity = TA_Base_Core::INTENSITY_HIGH;
            break;
        default:
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED intensity is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        // font colour
        // 0-2 (0 unused)
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[15] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 2))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED font colour is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.fontColour = static_cast<TA_Base_Core::ELEDColour>(temp);

        // background colour
        // same format as font colour
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[16] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 2))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS LED background colour is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.backgroundColour = static_cast<TA_Base_Core::ELEDColour>(temp);

        return attributes;
    }


    TA_Base_Core::TTISLedAttributes MessageLibraryManager::parseTtisLedAttributeString(const std::string& attributeString)
    {
        if (attributeString.length() != 24)
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED attributes are not 24 bytes.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        TA_Base_Core::TTISLedAttributes attributes;

        std::stringstream tempStream;
        int temp = 0;

        // Display mode
        // 0 to 7
        // force the buffer flushed with ends - flush doesnt work
        // this is so this will work on windows and not just solaris
        tempStream << attributeString[0] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 7))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED display mode is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayMode = static_cast<TA_Base_Core::EDisplayMode>(temp);

        // scroll speed
        // 0 instant on
        // 8 medium
        // 15 (F) fast
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[1] << std::ends;
        tempStream >> std::hex >> temp;
        switch( temp )
        {
        // can be 0 to F
        // only 3 values are catered for here
        // pick the closest
        case 0: case 1: case 2: case 3: case 4: case 5:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_SLOW;
            break;
        case 6: case 7: case 8: case 9: case 0xA:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
            break;
        case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_FAST;
            break;
        default:
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED scroll speed is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        // validity period
        // 4 bytes - unused (0000 to 1440)
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[2] 
            << attributeString[3]
            << attributeString[4]
            << attributeString[5] << std::ends;
        tempStream >> std::dec >> temp;
        // no validation - not used
        attributes.validityPeriod = temp;

        // repeat interval
        // 3 bytes 000 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[6] 
            << attributeString[7]
            << attributeString[8] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED repeat interval is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.repeatInterval = temp;

        // display time
        // 3 bytes 001 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[9] 
            << attributeString[10]
            << attributeString[11] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED display time is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayTime = temp;

        // font size
        // 1 byte 0 or 1
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[12] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 1))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED font size is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.latinFontSize = static_cast<TA_Base_Core::ELEDFontSize>(temp);

        // justification
        // 1 byte 0-2
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[13] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 2))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED justification is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.justification = static_cast<TA_Base_Core::EJustification>(temp);

        // intensity
        // 0-8
        // 4- Normal
        // 8- High
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[14] << std::ends;
        tempStream >> std::dec >> temp;
        switch( temp )
        {
        // can be 0 to 8
        // only 2 values are catered for here
        // pick the closest
        case 0: case 1: case 2: case 3: case 4:
            attributes.intensity = TA_Base_Core::NORMAL;
            break;
         case 6: case 7: case 8:
            attributes.intensity = TA_Base_Core::HIGH;
            break;
        default:
            std::string error("The predefined message library has been incorrectly formatted - The default TTIS LED intensity is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        return attributes;
    }


    TA_Base_Core::STISPlasmaAttributes MessageLibraryManager::parseStisPlasmaAttributeString(const std::string& attributeString)
    {
        if (attributeString.length() != 24)
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS plasma attributes are not 24 bytes.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        
        TA_Base_Core::STISPlasmaAttributes attributes;

        std::stringstream tempStream;
        int temp = 0;

        // Display mode
        // 0 to 7
        // force the buffer flushed with ends - flush doesnt work
        // this is so this will work on windows and not just solaris
        tempStream << attributeString[0] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 7))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma display mode is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayMode = static_cast<TA_Base_Core::EDisplayMode>(temp);

        // scroll speed
        // 0 instant on
        // 8 medium
        // 15 (F) fast
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[1] << std::ends;
        tempStream >> std::hex >> temp;
        switch( temp )
        {
        // can be 0 to F
        // only 3 values are catered for here
        // pick the closest
        case 0: case 1: case 2: case 3: case 4: case 5:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_SLOW;
            break;
        case 6: case 7: case 8: case 9: case 0xA:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
            break;
        case 0xB: case 0xC: case 0xD: case 0xE: case 0xF:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_FAST;
            break;
        default:
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma scroll speed is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }

        // validity period
        // 4 bytes - unused (0000 to 1440)
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[2] 
            << attributeString[3]
            << attributeString[4]
            << attributeString[5] << std::ends;
        tempStream >> std::dec >> temp;
        // no validation - not used
        attributes.validityPeriod = temp;

        // repeat interval
        // 3 bytes 000 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[6] 
            << attributeString[7]
            << attributeString[8] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma repeat interval is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.repeatInterval = temp;

        // display time
        // 3 bytes 001 to 999
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[9] 
            << attributeString[10]
            << attributeString[11] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 999))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma display time is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.displayTime = temp;

        // justification
        // 1 byte 0-2
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[12] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 2))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma justification is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.justification = static_cast<TA_Base_Core::EJustification>(temp);

        // font size
        // 1-9
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[13] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 9))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma font size is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.fontSize = static_cast<TA_Base_Core::EPlasmaFontSize>(temp);

        // font type 0-9
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[14] << std::ends;
        tempStream >> std::dec >> temp;
        if ((temp < 0) || (temp > 9))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma font type is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.fontType = static_cast<TA_Base_Core::EFontType>(temp);

        // font colour
        // 0-F
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[15] << std::ends;
        tempStream >> std::hex >> temp;
        if ((temp < 0) || (temp > 0xF))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma font colour is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.fontColour = static_cast<TA_Base_Core::EPlasmaColour>(temp);

        // background colour
        // same format as font colour
        tempStream.str("");
        temp = 0;
        tempStream << attributeString[16] << std::ends;
        tempStream >> std::hex >> temp;
        if ((temp < 0) || (temp > 0xF))
        {
            std::string error("The predefined message library has been incorrectly formatted - The default STIS Plasma background colour is out of range.");
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, error.c_str() );
			m_libraryFileBuffer.clear();
			TA_THROW( TA_IRS_Core::TISAgentException( error.c_str() ) );
        }
        attributes.backgroundColour = static_cast<TA_Base_Core::EPlasmaColour>(temp);

        return attributes;
    }


	void MessageLibraryManager::processNewMessageLibrary( EMessageLibraryType type, const std::string& filename, int version  )
	{
		FUNCTION_ENTRY("processNewMessageLibrary");
		TA_Base_Core::ThreadGuard guard( m_libraryProcessLock );

		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::processNewMessageLibrary() is only available at the OCC" );
		
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Process new message library");

		int currentLibraryVersion;
		int parsedNextLibraryVersion;

		m_indicatedVersion = version;

		//TES831 Events not logged!

		//TD 15899
		//zhou yuan++
		TA_Base_Core::DescriptionParameters desc;
		std::stringstream libraryVersionStream;

		switch(type)
		{
		case STATION_LIBRARY:
			libraryVersionStream << StatusMonitor::getInstance()->getNextSTISStationLibraryVersion();
			break;
		case TRAIN_LIBRARY:
			libraryVersionStream << StatusMonitor::getInstance()->getNextSTISTrainLibraryVersion();
		}

		TA_Base_Core::NameValuePair libraryVersion("Version", libraryVersionStream.str());		
		desc.push_back(&libraryVersion);
		//++zhou yuan

		//TES831 Events not logged!

		try
		{
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Retrieving and Parsing new message library");

			retrieveMessageLibrary( type, filename );

			parseMessageLibrary( type, filename );

			switch( type )
			{
			case STATION_LIBRARY:
				{
				parsedNextLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
				currentLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();

				if( currentLibraryVersion == StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central() )
				{
					TA_THROW( TA_IRS_Core::TISAgentException( "New parsed Station message library file has the same version number as the current message library" ) );
				}

				//TES831 Events not logged!
				TA_Base_Core::NameValuePair valid("Valid/Invalid", "Valid");
				desc.push_back(&valid);

				STISManager::getInstance()->submitAuditMessage(TA_Base_Core::TISAudit::STISLibraryReceived,desc,"");
				//TES831 Events not logged!

				// Presume that none of the stations have received this new library version yet..
				StatusMonitor::getInstance()->setStationLibrariesSynchronised( false );

				// Let the STISManager know that the new message library version is ready to go
				// so it can begin examining the stations
				STISManager::getInstance()->newMessageLibraryReady( parsedNextLibraryVersion, type );
				}
				break;

			case TRAIN_LIBRARY:
				{
				parsedNextLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
				currentLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();

				// TES831 Events not logged!
				libraryVersionStream << StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();

				// TES831 Events not logged!

				if( currentLibraryVersion == parsedNextLibraryVersion )
				{
					TA_THROW( TA_IRS_Core::TISAgentException( "New parsed Train message library file has the same version number as the current message library" ) );
				}

				//TD19279
				TA_Base_Core::NameValuePair valid("Valid/Invalid", "Valid");
				desc.push_back(&valid);
				
				STISManager::getInstance()->submitAuditMessage(TA_Base_Core::TISAudit::TTISLibraryReceived,desc,"");
				//TD19279

				// Presume that none of the stations have received this new library version yet..
				StatusMonitor::getInstance()->setTrainLibrariesSynchronised( false );

				// Let the STISManager know that the new message library version is ready to go
				// so it can begin examining the stations
				STISManager::getInstance()->newMessageLibraryReady( parsedNextLibraryVersion, type );
				}
				break;
			}

			//TD15198 azenitha++
			if (m_raisedMessageLibraryDownloadErrorAlarm == true)
			{
				STISManager::getInstance()->closeAlarm(TA_Base_Core::TISAlarms::MessageLibraryDownloadError);
				m_raisedMessageLibraryDownloadErrorAlarm = false;
			}
			//TD15198 ++azenitha			

		}
		catch ( const TA_IRS_Core::TISAgentException& tae  )
		{	
			TA_Base_Core::DescriptionParameters dp;
			
			//TD 15899
			//zhou yuan++
			
			switch( type )
			{
			
			case STATION_LIBRARY:
			     {

					 std::string description(tae.what());
					 TA_Base_Core::NameValuePair descVP("Description", description );
					 TA_Base_Core::NameValuePair typeVP("Type", "Station" );					 
					 dp.push_back( &typeVP );
					 dp.push_back( &descVP );

					 STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::MessageLibraryDownloadError, dp );
					 m_raisedMessageLibraryDownloadErrorAlarm = true; //TD15198 azenitha

					 //TES831 Events not logged!
					 TA_Base_Core::NameValuePair valid("Valid/Invalid", "Invalid");
					 desc.push_back(&valid);
					 STISManager::getInstance()->submitAuditMessage(TA_Base_Core::TISAudit::STISLibraryReceived,desc,"");
					 //TES831 Events not logged!
					 
					 break;
				 }
			
			case TRAIN_LIBRARY:
				{
					std::string description(tae.what());
					TA_Base_Core::NameValuePair descVP("Description", description );
					TA_Base_Core::NameValuePair typeVP("Type", "Train" );
					dp.push_back( &descVP );
					dp.push_back( &typeVP );
					STISManager::getInstance()->submitAlarm( TA_Base_Core::TISAlarms::MessageLibraryDownloadError, dp );
					m_raisedMessageLibraryDownloadErrorAlarm = true; //TD15198 azenitha

					//TES831 Events not logged!
					TA_Base_Core::NameValuePair valid("Valid/Invalid", "Invalid");
					desc.push_back(&valid);
					STISManager::getInstance()->submitAuditMessage(TA_Base_Core::TISAudit::TTISLibraryReceived,desc,"");
					//TES831 Events not logged!
					break;
				}
			}

			throw;

			//++zhou yuan
		}
	}

	void MessageLibraryManager::retrieveMessageLibrary( EMessageLibraryType type, const std::string& filename )
	{
		TA_ASSERT( STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC,
                   "MessageLibraryManager::retrieveMessageLibrary() is only available at the OCC" );
        try
        {
			 switch( type )
				{

				case STATION_LIBRARY:
					TA_Base_Core::FTPManager::getInstance().retrieveFile(m_serverIPAddress,"anonymous","",filename, m_libraryFileBuffer);
					break;

				case TRAIN_LIBRARY:
					TA_Base_Core::FTPManager::getInstance().retrieveFile(m_serverIPAddress,"anonymous","",filename, m_libraryFileBuffer);
					break;

				}

        }
        catch ( const TA_Base_Core::FTPException& ftpex )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, ftpex.what() );
			// TODO - Raise an alarm to indicate that the library could not be retrievd

			TA_THROW( TA_IRS_Core::TISAgentException( "Caught an FTPException while attempt to retrieve new message library" ) );
        }
	}

	MessageLibraryManager::MessageLibraryManager( const std::string& stationMessageFilename, const std::string& stationMessagePath,
        const std::string& trainMessageFilename, const std::string& trainMessagePath, const std::string& serverIP )
	{
		m_stationMessageLibraryFilename = stationMessageFilename;
		m_stationMessageLibraryPath = stationMessagePath;
        m_trainMessageLibraryFilename = stationMessageFilename;
		m_trainMessageLibraryPath = stationMessagePath;
		
		m_serverIPAddress = serverIP;
		
		m_raisedMessageLibraryDownloadErrorAlarm = false; //TD15198 azenitha
	}

	MessageLibraryManager::~MessageLibraryManager()
	{

	}

}  // namespace TA_IRS_App



