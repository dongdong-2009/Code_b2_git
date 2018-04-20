#if !defined(AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_)
#define AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_

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

#include <vector>
#include "core/message/src/NameValuePair.h"
#include "core/alarm/src/AlarmConstants.h"

namespace TA_Base_Core
{
    class StructuredEventSupplier;
    class MessageType;

    // AuditMessage header:

    // domain_name = message context name
    // type_name = message type
    // variable_header:
    // - (CosNotification::Priority, priority), where priority is provided by the
    //   MessageTypeTableAccessor for the given message type.
    // filterable_data:
    // - client supplied (name, value) pairs

    class AuditMessageSender
    {
    public:

        /**
        * constructor
        *
        * @param    eventSupplier    StructuredEventSupplier used to send events
        */
        AuditMessageSender(StructuredEventSupplier* eventSupplier);

        /**
        * destructor
        */
        virtual ~AuditMessageSender();

        /**
        * sendAuditMessageWithAsset
        *
        * Used to send an AuditMessage. 
        *
        * @return   std::string     The message's unique identifier
        * @param    messageType        The specific type of message
        * @param    entityKey        The key of an entity that is associated with this Message
        * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
        * @param    details            Any additional information associated with the Message.
        * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
        * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
        * @param    incidentID        The UUID of an incident that this event should be associated with
        * @param    parentEventID   If this event needs to be associated with another audit event (ie plan steps), to display a related event tree in the viewer, put the key here
        * @param    assetName       The name of the asset associated with the audit event.
        * @param    filterableData  Any additional filterable information that you wish to include in the event.
        * @param    timeStamp       The time at which the event took place (current time if unspecified)
        */
        std::string sendAuditMessageWithAsset( const MessageType& messageType, 
                                               unsigned long entityKey,
                                               const DescriptionParameters& description,
                                               const std::string& details,
                                               const std::string& sessionID,
                                               const std::string& alarmID,
                                               const std::string& incidentID,
                                               const std::string& parentEventID,
                                               const std::string& assetName,
                                               const FilterData* filterableData = NULL,
                                               const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );

        /**
        * sendAuditMessage
        *
        * Used to send an AuditMessage.  The asset name will be unspecified.  
        *
        * @return   std::string     The message's unique identifier
        * @param    messageType        The specific type of message
        * @param    entityKey        The key of an entity that is associated with this Message
        * @param    description        The strings that will fill in the parameterised description string that is stored in the database. There must be one string per parameter.
        * @param    details            Any additional information associated with the Message.
        * @param    sessionID            The sessionID of the operator whose action initiated this Audit Message
        * @param    alarmID            The UUID of an alarm whose instance required that this audit message be generated.
        * @param    incidentID        The UUID of an incident that this event should be associated with
        * @param    parentEventID   If this event needs to be associated with another audit event (ie plan steps), to display a related event tree in the viewer, put the key here
        * @param    assetName       The name of the asset associated with the audit event.
        * @param    filterableData  Any additional filterable information that you wish to include in the event.
        * @param    timeStamp       The time at which the event took place (current time if unspecified)
        */
        std::string sendAuditMessage( const MessageType& messageType,
                                      unsigned long entityKey,
                                      const DescriptionParameters& description,
                                      const std::string& details,
                                      const std::string& sessionID,
                                      const std::string& alarmID,
                                      const std::string& incidentID,
                                      const std::string& parentEventID,
                                      const FilterData* filterableData = NULL,
                                      const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );

        // CL-21508 begin
        std::string sendAuditMessageEx( const MessageType& messageType,             
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
                               const FilterData* filterableData = NULL,
                               const timeb& timeStamp = TA_Base_Core::AlarmConstants::defaultTime );
        // CL-21508 end
    private:
        // NOT CLONEABLE
        AuditMessageSender( const AuditMessageSender& theAuditMessageSender);

        StructuredEventSupplier* m_supplier;

    }; // AuditMessageSender


}; // TA_Base_Core

#endif // !defined(AuditMessageSender_BACE0F1A_E9B2_4825_9714_8BA3A498E266__INCLUDED_)
