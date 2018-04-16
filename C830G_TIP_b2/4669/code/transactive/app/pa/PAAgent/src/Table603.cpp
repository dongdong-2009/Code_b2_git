/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table603.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "app/pa/PAAgent/src/Table603.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/PasTableManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/CachedConfig.h"


using namespace TA_Base_Core;
using namespace TA_IRS_Bus;

const unsigned long TABLE_603_LABEL_SUB_OFFSET = 0;
const unsigned long TABLE_603_TYPE_SUB_OFFSET  = 40;

const unsigned long TABLE_603_RECORD_SIZE      = 41;
const unsigned long TABLE_603_LABEL_SIZE       = 40;

namespace TA_IRS_App
{

// ExceptionChecked
Table603::Table603 ()
: AbstractPASTable(603, 20254),
  m_paAgentCommsSender(NULL)
{
    FUNCTION_ENTRY("Constructor");

    m_messageLabelTypes.resize(TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC);

    if (!m_paAgentCommsSender)
    {
        m_paAgentCommsSender = 
            MessagePublicationManager::getInstance().getCommsMessageSender( PAAgentComms::Context );
    }

    // Cached these throughout the life of the application to save accessing 
    // the CachedConfig locks all the time

    if (CachedConfig::getInstance()->getIsMaster() || CachedConfig::getInstance()->getIsDepot())
    {
        // 7-500
        m_minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1;
        m_maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC +
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC;
    }
    else
    {
        // Station 507-1000
        m_minMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 1;
        m_maxMessageId = TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC + 
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC +
                         TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC;
    }

    m_locationKey = CachedConfig::getInstance()->getAgentLocationKey();

	FUNCTION_EXIT;
}

// ExceptionChecked
Table603::~Table603()
{
    FUNCTION_ENTRY("Destructor");

    if (m_paAgentCommsSender)
    {
        delete m_paAgentCommsSender;
        m_paAgentCommsSender = NULL;
    }

    FUNCTION_EXIT;
}

void Table603::processRead()
    //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException)
{
    unsigned long offset(0);

    ThreadGuard guard( m_lock ); // ReEntrant 

    for (unsigned long tableRecord=0; tableRecord < TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC; ++tableRecord)
    {
        std::string bufferFor40Chars(reinterpret_cast<char*>(&m_buffer[offset + TABLE_603_LABEL_SUB_OFFSET]), TABLE_603_LABEL_SIZE); 
        
        // Important that we use the c_str() below otherwise the m_label will be copied to 
        // contain 40 characters.  This can cause future string comparisons to fail.
        m_messageLabelTypes[tableRecord].m_label = bufferFor40Chars.c_str();

        char messageType = PasHelpers::getInstance()->get8bit (m_buffer, offset + TABLE_603_TYPE_SUB_OFFSET);

        // Verify type
        if (messageType != 'E' &&
            messageType != 'N' &&
            messageType != 'X')
        {
            // Force to the default normal type in case of error
            messageType = 'N';
        }

        m_messageLabelTypes[tableRecord].m_type  = messageType;

        offset += TABLE_603_RECORD_SIZE;
    }

    updateDatabaseAndBroadcastUpdates();
}

void Table603::updateDatabaseAndBroadcastUpdates()
    //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException)
{
    // We check if there is any inconsistency between the new data and the
    // data in the CachedMaps

    TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData* disposableUpdateData = NULL;
    int numberOfChanges = 0;

    try
    {      
        for (int messageId = m_minMessageId ; messageId <= m_maxMessageId; ++messageId)
        {
            unsigned long messageKey(0);
            unsigned long tableRecord = messageId - m_minMessageId;

            StationDvaMessageRecord record;

            try
            {
                messageKey = CachedMaps::getInstance()->getKeyFromStationDvaMessageIdAndLocation(messageId, m_locationKey);
                record = CachedMaps::getInstance()->getStationDvaMessageRecordFromKey( messageKey );
            }
            catch (TA_Base_Core::CachedMappingNotFoundException)
            {
                // Create a new record

                record.m_id = messageId;
                record.m_locationKey = m_locationKey;
                record.m_pKey = TA_Base_Core::DatabaseKey::getInvalidKey();
                record.m_tisMessageTag = TA_Base_Core::DatabaseKey::getInvalidKey();
                record.m_tisLibrarySection = TA_Base_Core::DatabaseKey::getInvalidKey();
                record.m_tisLibraryVersion = TA_Base_Core::DatabaseKey::getInvalidKey();
                record.m_label = "";
                record.m_type = 0;
                // We deliberately nullify record.m_type and m_label here so the code below will be called
            }
            

            if ( m_messageLabelTypes[tableRecord].m_type  != record.m_type || 
                 m_messageLabelTypes[tableRecord].m_label != record.m_label )
            {
                char oldType = record.m_type;
                std::string oldLabel = record.m_label;

                record.m_type  = m_messageLabelTypes[tableRecord].m_type;
                record.m_label = m_messageLabelTypes[tableRecord].m_label;

                try
                {
                    CachedMaps::getInstance()->setStationDvaMessageRecord(record, true);
                }
                catch (...)
                {
                    // Only expecting DataException or DataBaseException 
                    // Failed to write the changes - see if we can process the rest

                    // ToDo: submit alarm
                    continue;
                }

                // submit an audit message
                PasHelpers::getInstance()->submitPaAgentDVAMessageUpdateAuditMessage(
                    oldType,
                    record.m_type,
                    oldLabel,
                    record.m_label,
                    record.m_pKey );

                // Detected at least one change
                ++numberOfChanges;

                if (!disposableUpdateData)
                {
                    // First time so we initialise with the max possible size 
                    // expected and then later crop the sequence
                    disposableUpdateData = new TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData(TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC);
                    disposableUpdateData->length(TA_Base_Bus::IPAAgentCorbaDef::PA_NBPREREC);
                }
                    


                (*disposableUpdateData)[numberOfChanges-1].m_pKey               = record.m_pKey;
                (*disposableUpdateData)[numberOfChanges-1].m_id                 = record.m_id;
                (*disposableUpdateData)[numberOfChanges-1].m_locationKey        = record.m_locationKey;
                (*disposableUpdateData)[numberOfChanges-1].m_label              = record.m_label.c_str();
                (*disposableUpdateData)[numberOfChanges-1].m_type               = record.m_type;
                (*disposableUpdateData)[numberOfChanges-1].m_tisMessageTag      = record.m_tisMessageTag;
                (*disposableUpdateData)[numberOfChanges-1].m_tisLibrarySection  = record.m_tisLibrarySection;
                (*disposableUpdateData)[numberOfChanges-1].m_tisLibraryVersion  = record.m_tisLibraryVersion;

            }
        } // for loop

        if (disposableUpdateData)
        {
            // Detected at least one change, so we broadcast it to all open PA Managers

            // Reset the length of the disposableUpdateData vector so it only contains
            // the actual number of changes.  This is for the client to know.

            disposableUpdateData->length(numberOfChanges);

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Sending DvaMessagesUpdate message to subscribers");


            CORBA::Any data;

            data <<= disposableUpdateData;

            m_paAgentCommsSender->sendCommsMessage(
                PAAgentComms::DvaMessagesUpdate,                        // Context::Type
                CachedConfig::getInstance()->getAgentKey(),             // EntityKey
                data,                                                   // Data
                CachedConfig::getInstance()->getAgentSubsystemKey(),    // Subsystem
		        m_locationKey,                                          // LocationKey
                NULL);
        }
    }
    // Cleanup in case of exceptions thrown
    catch (TA_Base_Core::DataException&)
    {
        // ToDo - make this impossible
        TA_ASSERT(false, "Should not get here");
    }
    catch (...)
    {
        // Expecting TA_Base_Core::DatabaseException and TA_Base_Core::PasConnectionException

        if (disposableUpdateData)
        {
            delete disposableUpdateData;
            disposableUpdateData = NULL;
        }    
        // To be caught by the FlaggedTableWorker and be retried in the next update
        throw;
    }
}

ReadTable603::ReadTable603( Table603& table,
                            Table560& table560,
                            TA_IRS_Bus::Scheduler& socketScheduler,
                            TA_IRS_Bus::Scheduler& processScheduler,
                            IPasEventSource& eventSource )
    : PASReadEvent( socketScheduler, processScheduler, eventSource )
    , m_table( table )
    , m_table560( table560 )
{
}

ReadTable603::~ReadTable603()
{
}

void ReadTable603::readTable()
{
    ThreadGuard guard( this->m_table.m_lock );
    
    PASConnection::getInstance()->readTable( this->m_table.TABLE_NUMBER,
        this->m_table.m_buffer,
        this->m_table.BUFFER_SIZE);
    
    this->m_processScheduler.post( new ProcessTable603( this->m_table ) );
    
    this->m_table560.resetFlags( Table560::TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET,
        Table560::TABLE_560_TABLE_603_FLAG_B_BIT_OFFSET );
}

ProcessTable603::ProcessTable603( Table603& table )
    : m_table( table )
{
}

ProcessTable603::~ProcessTable603()
{
}

void ProcessTable603::consume()
{
    this->m_table.processRead();

    delete this;
}

void ProcessTable603::cancel()
{
    delete this;
}



} // namespace TA_IRS_Bus

