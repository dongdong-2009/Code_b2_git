/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Jade Welton
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * Used to send Audit messages
  */
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <ctime>

#include "core/message/src/AuditMessageSender.h"
#include "core/message/IDL/src/AuditMessageCorbaDef.h"
#include "core/message/src/MessageConstants.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/types/MessageTypes.h"
#include "core/uuid/src/TAuuid.h"
#include "core/event_processing/src/EventProcessingLibrary.h"
#include "core/message/src/AuditEvent.h"
#include "core/message/src/StructuredEventSupplier.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Core
{
    AuditMessageSender::AuditMessageSender( StructuredEventSupplier* eventSupplier )
        : m_supplier(eventSupplier)
    {
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
    }


    AuditMessageSender::~AuditMessageSender()
    {
    }


    std::string AuditMessageSender::sendAuditMessageWithAsset( const MessageType& messageType,
                                                               unsigned long entityKey,
                                                               const DescriptionParameters& description,
                                                               const std::string& details,
                                                               const std::string& sessionID,
                                                               const std::string& alarmID,
                                                               const std::string& incidentID,
                                                               const std::string& parentEventID,
                                                               const std::string& assetName,
                                                               const FilterData* filterableData /* = 0 */,
                                                               const timeb& timeStamp /* = 0 */)
    {
        // Collate Variable Header fields
        FilterData varHeader;

        // as priority is currently not used and MessageTypeTableAccessor is expensive, disable it and assign to 0;
        //short priority = MessageTypeTableAccessor::getInstance().getPriority( messageType );
        //short priority = 0;
        std::string priorityString( "0" );
        NameValuePair priorityField( CosNotification::Priority, priorityString );
        varHeader.push_back( &priorityField );

        // Create AuditMessageCorbaDef and fill in details
        AuditMessageCorbaDef* auditMessage = new AuditMessageCorbaDef;

        // If timestamp is specified, use it.
        if ( timeStamp.time != 0 )
        {
            auditMessage->createTime.time = timeStamp.time;
            auditMessage->createTime.millitm = timeStamp.millitm;
        }
        else
        {
            // else use the current time.
            timeb currentTime;
            ftime(&currentTime);
            auditMessage->createTime.time = currentTime.time;
            auditMessage->createTime.millitm = currentTime.millitm;
        }

        auditMessage->logTime.time = 0; // unknown
        auditMessage->logTime.millitm = 0;
        auditMessage->messageTypeKey = messageType.getTypeKey().c_str();
        auditMessage->assocEntityKey = entityKey;

        // Create the message description
        std::string messageDescription;
        gGetStringFromParameters( description, messageDescription );
        auditMessage->messageParameterList = messageDescription.c_str();
        auditMessage->messageDescription = MessageTypeTableAccessor::getInstance().getExpandedDescription( messageType, description ).c_str();

        auditMessage->messageDetails = details.c_str();
        auditMessage->sessionID = sessionID.c_str();
        auditMessage->assocAlarmID = alarmID.c_str();
        auditMessage->assocIncidentID = incidentID.c_str();
        auditMessage->parentEventID = parentEventID.c_str();
        auditMessage->assetName = assetName.c_str();
        // TD13673
        unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
        auditMessage->locationKey = locationKey;

        // generate the unique identifier
        std::string uuid = TA_Base_Core::TA_Uuid::generateUUID();
        auditMessage->eventID = uuid.c_str();

        // TD8684: The event processing library is now notified of events. It in turn
        //         will ensure that the events are logged.
        Event auditEvent( new AuditEvent( *auditMessage ) );
        EventProcessingLibrary::instance()->processEvent( auditEvent );

        // Create Structured Event, inject message into Any field
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= auditMessage;
        gPopulateStructuredEventHeader( *event,
                                        messageType.getContextObj().getContextName(),
                                        messageType.getTypeKey(),
                                        &varHeader,
                                        filterableData );

        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);

        // return the message's uuid
        return uuid;
    }


    std::string AuditMessageSender::sendAuditMessage( const MessageType& messageType,
                                                      unsigned long entityKey,
                                                      const DescriptionParameters& description,
                                                      const std::string& details,
                                                      const std::string& sessionID,
                                                      const std::string& alarmID,
                                                      const std::string& incidentID,
                                                      const std::string& parentEventID,
                                                      const FilterData* filterableData /* = 0 */,
                                                      const timeb& timeStamp /* = 0 */)
    {
        // TODO: The asset name could be set to the entity name here, similar to how alarms
        // are handled.  But the entity name is not specified as an argument, and it is not
        // clear how we could efficiently get it without requiring a database access.

        return sendAuditMessageWithAsset( messageType,
                                          entityKey,
                                          description,
                                          details,
                                          sessionID,
                                          alarmID,
                                          incidentID,
                                          parentEventID,
                                          "",
                                          filterableData,
                                          timeStamp );
    }

    // CL-21508 begin
    std::string AuditMessageSender::sendAuditMessageEx( const MessageType& messageType, 
        const unsigned long entityKey, 
        const DescriptionParameters&description, 
        const std::string& details, 
        const std::string& sessionID, 
        const std::string& alarmID, 
        const std::string& incidentID, 
        const std::string& parentEventID, 
        const std::string& assetName, 
        const std::string& alarmDescription, 
        const unsigned long eventLelOrSeverityKey, 
        const std::string& alarmvalue, 
        const unsigned long alarmTypeKey, 
        const std::string& alarmComments, 
        const std::string& alarmMMSState, 
        const std::string& alarmDSSState,
        const std::string& alarmAVLstate,
        const std::string& alarmACKedBy,
        const std::string& alarmStatus, 
        const FilterData* filterableData/* = NULL*/,
        const timeb& timeStamp/* = TA_Base_Core::AlarmConstants::defaultTime*/ )
    {
        // Collate Variable Header fields
        FilterData varHeader;

        // as priority is currently not used and MessageTypeTableAccessor is expensive, disable it and assign to 0;
        std::string priorityString( "0" );
        NameValuePair priorityField( CosNotification::Priority, priorityString );
        varHeader.push_back( &priorityField );
        
        // Create AuditMessageCorbaDef and fill in details
        AuditMessageCorbaDef* auditMessage = new AuditMessageCorbaDef;
        
        // If timestamp is specified, use it.
        if (timeStamp.time != 0)
        {
            auditMessage->createTime.time = timeStamp.time;
            auditMessage->createTime.millitm = timeStamp.millitm;
        }
        else
        {
            // else use the current time.
            timeb currentTime;
            ftime(&currentTime);
            auditMessage->createTime.time = currentTime.time;
            auditMessage->createTime.millitm = currentTime.millitm;
        }
        
        auditMessage->logTime.time = 0; // unknown
        auditMessage->logTime.millitm = 0;
        auditMessage->messageTypeKey = CORBA::string_dup( messageType.getTypeKey().c_str() );
        auditMessage->assocEntityKey = entityKey;
        
        // Create the message description
        std::string messageDescription;
        gGetStringFromParameters( description, messageDescription );
        auditMessage->messageParameterList = CORBA::string_dup( messageDescription.c_str() );
        auditMessage->messageDescription = CORBA::string_dup( 
            MessageTypeTableAccessor::getInstance().getExpandedDescription( messageType, description ).c_str() );
        
        auditMessage->messageDetails = CORBA::string_dup( details.c_str() );
        auditMessage->sessionID = CORBA::string_dup( sessionID.c_str() );
        auditMessage->assocAlarmID = CORBA::string_dup( alarmID.c_str() );
        auditMessage->assocIncidentID = CORBA::string_dup( incidentID.c_str() );
        auditMessage->parentEventID = CORBA::string_dup( parentEventID.c_str() );
        auditMessage->assetName = CORBA::string_dup( assetName.c_str() );
        unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str(), NULL, 10);
        auditMessage->locationKey = locationKey;
        
        // generate the unique identifier
        std::string uuid = TA_Base_Core::TA_Uuid::generateUUID();
        auditMessage->eventID = CORBA::string_dup( uuid.c_str() );

        Event auditEvent( new AuditEvent( *auditMessage, alarmDescription, eventLelOrSeverityKey, alarmvalue,
            alarmTypeKey, alarmComments, alarmMMSState, alarmDSSState, alarmAVLstate, alarmACKedBy, alarmStatus ) );
        EventProcessingLibrary::instance()->processEvent( auditEvent );
        
        // Create Structured Event, inject message into Any field
        CosNotification::StructuredEvent* event = new CosNotification::StructuredEvent;
        event->remainder_of_body <<= auditMessage;
        gPopulateStructuredEventHeader( 
            *event, 
            messageType.getContextObj().getContextName(), 
            messageType.getTypeKey(), 
            &varHeader, 
            filterableData );
        
        TA_ASSERT(m_supplier != NULL, "m_supplier != NULL");
        m_supplier->publishEvent(*event);
        
        // return the message's uuid
        return uuid;
    }
    // CL-21508 end
}; // TA_Base_Core
