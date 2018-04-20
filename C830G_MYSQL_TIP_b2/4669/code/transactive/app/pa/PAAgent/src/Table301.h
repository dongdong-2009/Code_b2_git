/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table301.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table301_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table301_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"

#include "ace/Future.h"


namespace TA_IRS_App
{
class IPasEventSource;

class Table301 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class WriteTable301;

public:

    Table301( TA_IRS_Bus::Scheduler& socketScheduler,
              TA_IRS_Bus::Scheduler& processScheduler,
              IPasEventSource& eventSource );

    void setTableDataAndWrite( CORBA::Octet messageSeqId, 
                               CORBA::Octet announceId,
                               unsigned short periodInSecs,
                               TA_Base_Core::DateTime startTime,
                               TA_Base_Core::DateTime stopTime,
                               bool isEventTriggered,
                               const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones );
        //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException );

private:
    Table301( const Table301& theTable301);
    virtual ~Table301();

    /** The scheduler responsible for socket bound events. */
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    /** The scheduler responsible for process events. */
    TA_IRS_Bus::Scheduler& m_processScheduler;
    /** The source of the ConnectToPAFEPC event instance. */
    IPasEventSource& m_pasEventSource;
};


/**
 * Event that will write data to Table 301 in the PAS.
 */
class WriteTable301 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 301 instance.
     * @param future The future object that will store the result of the write.
     * @param messageSeqId The message sequence id defining the index of the
     *                     message in Table 302.
     * @param announceId The ISCS specified identifier for the broadcast.
     * @param periodInSecs The period in seconds.  If set to 0, then the
     *                     broadcast is executed once, otherwise the broadcast
     *                     will cycle for the given amount of time.  The max
     *                     time period is 59 minutes and 59 seconds.
     * @param startTime The start time in GMT+8.
     * @param endTime The end time in GMT+8.
     * @param isEventTriggered Boolean indicating if this is an ATS event
     *                         triggered broadcast.
     * @param zones The coverage of zones.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process events.
     * @param eventSource The source of PAS events.
     */
    WriteTable301( Table301& table,
                   ACE_Future<int>& future,
                   CORBA::Octet messageSeqId,
                   CORBA::Octet announceId,
                   unsigned short periodInSecs,
                   TA_Base_Core::DateTime startTime,
                   TA_Base_Core::DateTime stopTime,
                   bool isEventTriggered,
                   const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );
    /**
     * Constructs an instance of this class.
     * @param timeToExpire The time the event is consumed in the scheduler.
     * @param table The reference to Table 301 instance.
     * @param future The future object that will store the result of the write.
     * @param messageSeqId The message sequence id defining the index of the
     *                     message in Table 302.
     * @param announceId The ISCS specified identifier for the broadcast.
     * @param periodInSecs The period in seconds.  If set to 0, then the
     *                     broadcast is executed once, otherwise the broadcast
     *                     will cycle for the given amount of time.  The max
     *                     time period is 59 minutes and 59 seconds.
     * @param startTime The start time in GMT+8.
     * @param endTime The end time in GMT+8.
     * @param isEventTriggered Boolean indicating if this is an ATS event
     *                         triggered broadcast.
     * @param zones The coverage of zones.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process events.
     * @param eventSource The source of PAS events.
     */
    WriteTable301( ACE_Time_Value timeToExpire,
                   Table301& table,
                   ACE_Future<int>& future,
                   CORBA::Octet messageSeqId,
                   CORBA::Octet announceId,
                   unsigned short periodInSecs,
                   TA_Base_Core::DateTime startTime,
                   TA_Base_Core::DateTime stopTime,
                   bool isEventTriggered,
                   const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable301();
    /**
     * writeTable called by PASWriteEvent::consume().
     * In this case, the data will be written out to PAS.
     */
    virtual void writeTable();

    /* This function is used to recreate the PASWriteEvent with a new
     * timeToExpire.
     */
    virtual PASWriteEvent* recreateEvent(ACE_Time_Value timeToExpire);

private:
    /** The reference to Table 301. */
    Table301& m_table;
    /** The message sequence id defining the index of the message in Table 302. */
    CORBA::Octet m_messageSeqId;
    /** The ISCS specified identifier for the broadcast. */
    CORBA::Octet m_announceId;
    /**
     * The period in seconds.  If set to 0, then the broadcast is executed
     * once, otherwise the broadcast will cycle for the given amount of time.
     * The max time period is 59 minutes and 59 seconds.
     */
    unsigned short m_periodInSecs;
    /** The start time in GMT+8. */
    TA_Base_Core::DateTime m_startTime;
    /** The end time in GMT+8. */
    TA_Base_Core::DateTime m_stopTime;
    /** Boolean indicating if this is an ATS event triggered broadcast. */
    bool m_isEventTriggered;
    /** The coverage of zones. */
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage m_zones;
};

} // namespace TA_IRS_App

#endif // !defined(Table301_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

