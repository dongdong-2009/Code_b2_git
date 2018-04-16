/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/src/ICombinedEventData.h $
  * @author:  Bradley Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * ICombinedEventData is an interface to the combined data from the EVENT
  * and LO_DATAPT_STATE_CHANGE tables required by the event viewer. It provides a 
  * read-only view of select information from both of these tables.
  *
  * The data that is accessible from this interface is everything required by the
  * Event Viewer to populate its columns.
  *
  * NOTE: All references to 'event' in the comments below refer to entries from either
  * the event table or the lo_datapt_state_change table.
  *
  */

#if !defined(_I_COMBINED_EVENT_DATA_H_)
#define _I_COMBINED_EVENT_DATA_H

#include <string>
#include <sys/timeb.h>

namespace TA_Base_Core
{
    typedef enum
    {
        E_AUDIT_EVENT,
        E_DPSTATECHANGE_EVENT
    } ECombinedEventType;

    class ICombinedEventData
    {
    public:
        virtual ~ICombinedEventData() {}

        /**
         * getEventType
         *
         * Returns the type of the event, that is, whether it is an audit event or a datapoint state change
         * event.
         *
         * @return ECombinedEventType The type of the event.
         */
        virtual const ECombinedEventType getEventType() =0;

        /**
         * getKey
         *
         * Returns the key for this event.
         *
         * @return unsigned long The key for the event.
         */
        virtual const unsigned long getKey() =0;


        /**
         * getCreateTime
         *
         * Returns the creation time for this event.
         *
         * @return timeb The time at which the event was created.
         */
        virtual const timeb getCreateTime() =0;


        /**
         * getAlarmSeverityName
         *
         * Returns the severity name of the alarm associated with this event.
         *
         * @return std::string& The name of the severity of the alarm associated with
         *                      this event.
         */
        virtual const std::string& getAlarmSeverityName() =0;


        /**
         * getAssetName
         *
         * Returns the asset name for this event.
         *
         * @return std::string& The name of the asset that generated this event.
         */
        virtual const std::string& getAssetName() =0;


        /**
         * getDescription
         *
         * Returns the description for this event.
         *
         * @return std::string& The description for this event.
         */
        virtual const std::string& getDescription() =0;


        /**
         * getValue
         *
         * Returns the value for this event.
         *
         * @return std::string& The value for this event.
         */
        virtual const std::string& getValue() =0;


        /**
         * getAlarmMmsState
         *
         * Returns the MMS state for the alarm associated with this event.
         *
         * @return std::string& The MMS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmMmsState() =0;


        /**
         * getAlarmDssState
         *
         * Returns the DSS state for the alarm associated with this event.
         *
         * @return std::string& The DSS state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmDssState() =0;


        /**
         * getAlarmAvlState
         *
         * Returns the AVL state for the alarm associated with this event.
         *
         * @return std::string& The AVL state of the alarm associated with this event.
         */
        virtual const std::string& getAlarmAvlState() =0;


        /**
         * getOperatorName
         *
         * Returns the name of the operator directly responsible for generating
         * this event.
         *
         * @return std::string& The name of the operator.
         */
        virtual const std::string& getOperatorName() =0;


        /**
         * getAlarmComment
         *
         * Returns the comment of the alarm associated with this event.
         *
         * @return std::string& The comment.
         */
        virtual const std::string& getAlarmComment() =0;


        /**
         * getAlarmId
         *
         * Returns the ID of the alarm associated with this event.
         *
         * @return std::string& The alarm ID.
         */
        virtual const std::string& getAlarmId() =0;

        /**
         * getAlarmAck
         *
         * Returns the Ack status of the alarm associated with this event.
         *
         * @return std::string& The alarm ack status.
         */
        virtual const std::string& getAlarmAck() =0;

        /**
         * getAlarmStatus
         *
         * Returns the status ie. close, open or pending of the alarm associated with this event.
         *
         * @return std::string& The alarm status.
         */
        virtual const std::string& getAlarmStatus() =0;
    };
};

#endif // !defined(_I_COMBINED_EVENT_DATA_H_)