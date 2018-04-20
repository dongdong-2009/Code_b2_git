/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table350.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table350_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table350_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"

#include "ace/Future.h"


namespace TA_IRS_App
{
class IPasEventSource;


class Table350 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class WriteTable350;

public:

    Table350( TA_IRS_Bus::Scheduler& socketScheduler,
              TA_IRS_Bus::Scheduler& processScheduler,
              IPasEventSource& eventSource );

    void setTableDataAndWrite( CORBA::Octet messageSeqId, 
                               bool hasChime, 
                               TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                               unsigned short dwellTimeInSecs );

private:
    Table350( const Table350& theTable350);
    virtual ~Table350();

    /** The scheduler responsible for socket bound events. */
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    /** The scheduler responsible for process events. */
    TA_IRS_Bus::Scheduler& m_processScheduler;
    /** The source of the ConnectToPAFEPC event instance. */
    IPasEventSource& m_pasEventSource;
};


/**
 * Event that will write data to Table 350 in PAS.
 */
class WriteTable350 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to table 350.
     * @param future The future instance that will store the result of the write.
     * @param messageSeqId The message sequence ID.
     * @param hasChime Whether the message sequence will have a chime.
     * @param messages The sequence of messages.
     * @param dwellTimeInSecs The dwell time between messages in seconds.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process events.
     * @param eventSource The source of PAS events.
     */
    WriteTable350( Table350& table,
                   ACE_Future<int>& future,
                   CORBA::Octet messageSeqId, 
                   bool hasChime, 
                   TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                   unsigned short dwellTimeInSecs,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /**
     * Constructs an instance of this class.
     * @param timeToExpire The time the event is consumed in the scheduler.
     * @param table The reference to table 350.
     * @param future The future instance that will store the result of the write.
     * @param messageSeqId The message sequence ID.
     * @param hasChime Whether the message sequence will have a chime.
     * @param messages The sequence of messages.
     * @param dwellTimeInSecs The dwell time between messages in seconds.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process events.
     * @param eventSource The source of PAS events.
     */
    WriteTable350( ACE_Time_Value timeToExpire,
                   Table350& table,
                   ACE_Future<int>& future,
                   CORBA::Octet messageSeqId, 
                   bool hasChime, 
                   TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages,
                   unsigned short dwellTimeInSecs,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable350();
    /**
     * writeTable called by PASWriteEvent::consume().  In this case, the data will be written to PAS. 
     */
    virtual void writeTable();
    
    /* This function is used to recreate the PASWriteEvent with a new
     * timeToExpire.
     */
    virtual PASWriteEvent* recreateEvent(ACE_Time_Value timeToExpire);

private:
    /** The reference to table 350. */
    Table350& m_table;
    /** The message sequence ID. */
    CORBA::Octet m_messageSeqId;
    /** Whether the message sequence will have a chime. */
    bool m_hasChime;
    /** The sequence of messages. */
    TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq m_messages;
    /** The dwell time between messages in seconds. */
    unsigned short m_dwellTimeInSecs;
};

} // namespace TA_IRS_App

#endif // !defined(Table350_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

