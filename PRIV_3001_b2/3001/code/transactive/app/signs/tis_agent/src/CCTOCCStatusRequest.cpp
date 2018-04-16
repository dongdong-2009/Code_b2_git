/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  */

#include "app/signs/tis_agent/src/CCTOCCStatusRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/MessageLibraryManager.h"
#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "StringHelper.h"
#include <iomanip>

namespace TA_IRS_App
{

    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;

    // initialise it to not having received an RXH message
    int CCTOCCStatusRequest::m_previousRXHMessageAck = 0;

    CCTOCCStatusRequest::CCTOCCStatusRequest()
    {
        m_messageIDString = "M31";
        setMessageIsSentToSTIS( false );
    }


    CCTOCCStatusRequest::~CCTOCCStatusRequest()
    {
    }

    void CCTOCCStatusRequest::log( bool sendSuccess )
    {
    }

    void CCTOCCStatusRequest::processResponse()
    {
        unsigned long nextSTISTrainLibraryVersion;

        unsigned long currentISCSTrainLibraryVersion;
        unsigned long nextISCSTrainLibraryVersion;

        bool iscsTrainLibrariesSynchronised;

        bool cctServerAlarm;

        // length (in bytes) of the actual data in the response message
        int dataLength;

        // Data length
        std::string dataLengthStr( m_responseMessage.begin()+22, m_responseMessage.begin()+26 );
        //  cct server status
        std::string cctServerStr( m_responseMessage.begin()+26, m_responseMessage.begin()+27 );
        // next ttis message library filename
		// In new icd, ttislibrary txt file name change to constant, comment this line
        //std::string ttisLibraryFilename( m_responseMessage.begin()+27, m_responseMessage.begin()+43 );
		static const std::string ttisLibraryFilename = "ISCSLibrary.txt";
        // current TTIS library version
        std::string currentTTISLibraryVersionStr( m_responseMessage.begin()+43, m_responseMessage.begin()+55 );
        // new TTIS library version
        std::string newTTISLibraryVersionStr( m_responseMessage.begin()+55, m_responseMessage.begin()+67 );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
             "Common info: \ndata length = %s, \ncct server link = %s, \nttis library[Current v%s,Next v%s], next file:%s",
             dataLengthStr.c_str(),
             cctServerStr.c_str(),
             currentTTISLibraryVersionStr.c_str(),
             newTTISLibraryVersionStr.c_str(),
             ttisLibraryFilename.c_str() );

        if ( AgentModeMonitor::getInstance().isInControlMode() )
        {
            dataLength = atoi( dataLengthStr.c_str() );
            cctServerAlarm = ( bool )atoi( cctServerStr.c_str() );
			nextSTISTrainLibraryVersion = StringHelper::libraryVersionStrToNumber(newTTISLibraryVersionStr);

            std::stringstream resourceStatus;
            resourceStatus << "Status of Resources: ";
            resourceStatus << " CCT server:";
            resourceStatus << cctServerAlarm;

            std::string resourceStr;

            resourceStr = resourceStatus.str();

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, resourceStr.c_str() );

            // Update the state of the resource datapoints


            // CCT Server Summary Point
			/* currently not available
            try
            {
                DpValue resourcePointValue = StatusMonitor::getInstance()->getCurrentDataPointValue( CCT_SERVER_STATUS_DP_NAME, OCC_STIS_SERVER_EQUIPMENT_NAME );
                DataPointState resourcePointState( resourcePointValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );

                resourcePointValue.setBoolean( cctServerAlarm );
                resourcePointState.setValue( resourcePointValue );
                resourcePointState.setStatus( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
                resourcePointState.setTimestamp( time( NULL ) );
                StatusMonitor::getInstance()->updateDataPointState( CCT_SERVER_STATUS_DP_NAME , resourcePointState, OCC_STIS_SERVER_EQUIPMENT_NAME, false );
            }
            catch ( TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", e.what() );
            }
			*/

            StatusMonitor::getInstance()->setNextSTISTrainLibraryVersion( nextSTISTrainLibraryVersion );

            // version of the TTIS library
            currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
            nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
            iscsTrainLibrariesSynchronised = StatusMonitor::getInstance()->getTrainLibrariesSynchronised();

            // log version information
            std::stringstream ttisVersionMessage;
            ttisVersionMessage << "\nNext STIS server train library version:    " << nextSTISTrainLibraryVersion << "\n";
            ttisVersionMessage << "Current ISCS train library version: " << currentISCSTrainLibraryVersion << "\n";
            ttisVersionMessage << "Next ISCS train library version: " << nextISCSTrainLibraryVersion << "\n";
            ttisVersionMessage << "Train libraries ";
            if ( iscsTrainLibrariesSynchronised )
            {
                ttisVersionMessage << "ARE synchronised \n";
            }
            else
            {
                ttisVersionMessage << "ARE NOT synchronised \n";
            }
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 ttisVersionMessage.str().c_str() );

            // We're only interested in a new library file,
            // if the previous one has been synchronised and upgraded, and the
            // version that the STIS is telling us about is different from our current library version
            if ( ( currentISCSTrainLibraryVersion != nextSTISTrainLibraryVersion ) &&
                    ( nextISCSTrainLibraryVersion != nextSTISTrainLibraryVersion ) )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "A new TTIS predefined message library is available for download" );

                try
                {
                    if ( ( STISManager::getInstance()->trainLibrarySynchInProgress() == false ) /*&&( iscsTrainLibrariesSynchronised )*/ )
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                             "Attempting to process new TTIS message library version" );

                        STISManager::getInstance()->processNewMessageLibrary( TA_IRS_App::TRAIN_LIBRARY, ttisLibraryFilename, nextSTISTrainLibraryVersion );
                        //then, send m70 message to CCT
                        STISManager::getInstance()->submitCCTLibraryReceivedNotiyRequest( nextSTISTrainLibraryVersion );
                    }
                    else
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                             "however, a previous train message library synchronisation is in progress" );
                    }
                }
                catch ( const TA_IRS_Core::TISAgentException& e )
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, e.what());
                }
                catch ( ... )
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                         "Could not process new TTIS predefined message library" );
                }
            }

        }
    }


    std::vector<unsigned char> CCTOCCStatusRequest::createProtocolMessage()
    {
        std::vector<unsigned char> messageVector;
        std::stringstream messageStream;

        // Add the sequence number
        messageStream << getSequenceNumberAsString();

        // Add the timestamp to the message
        messageStream << m_timestamp;

        // Now the message ID
        messageStream << m_messageIDString; // M31

        // Now the data length - in this case it will always be 3 byte
        messageStream << "0003";

        // Now the actual data
        messageStream << "OCC";

        std::string messageString = messageStream.str();

        messageVector.push_back( 0x02 );
        for ( unsigned long i=0;i < messageString.size() ; i++ )
        {
            messageVector.push_back( messageString[i] );
        }
        messageVector.push_back( 0x03 );

        return messageVector;
    }


    CCTLibraryReceivedNotifyRequest::CCTLibraryReceivedNotifyRequest( unsigned long libraryVersion )
            :m_libraryVersion( libraryVersion )
    {
        m_messageIDString = "M70";
        setMessageIsSentToSTIS( false );
    }
    CCTLibraryReceivedNotifyRequest::~CCTLibraryReceivedNotifyRequest()
    {

    }
    std::vector<unsigned char> CCTLibraryReceivedNotifyRequest::createProtocolMessage()
    {
        std::vector<unsigned char> messageVector;
        std::stringstream messageStream;

        // Add the sequence number
        messageStream << getSequenceNumberAsString();

        // Add the timestamp to the message
        messageStream << m_timestamp;

        // Now the message ID
        messageStream << m_messageIDString; // M70

        // Now the data length - in this case it will always be 12 byte
        messageStream << "0012";

        // Now the actual data
		messageStream << StringHelper::libraryVersionNumberToStr(m_libraryVersion);

        std::string messageString = messageStream.str();

        messageVector.push_back( 0x02 );
        for ( unsigned long i=0;i < messageString.size() ; i++ )
        {
            messageVector.push_back( messageString[i] );
        }
        messageVector.push_back( 0x03 );

        return messageVector;
    }

    void CCTLibraryReceivedNotifyRequest::log( bool sendSuccess )
    {

    }
    void CCTLibraryReceivedNotifyRequest::processResponse()
    {

    }
}