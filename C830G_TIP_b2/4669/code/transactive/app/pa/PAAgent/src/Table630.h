/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table630.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table630_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table630_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"

#include "ace/Future.h"


namespace TA_IRS_App
{
class IPasEventSource;

class Table630 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class WriteTable630;

public:

    Table630( TA_IRS_Bus::Scheduler& socketScheduler,
              TA_IRS_Bus::Scheduler& processScheduler,
              IPasEventSource& eventSource );

    void setTableDataAndWrite( unsigned int messageKey, CORBA::Octet sourceId ) ;
        //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException );

private:
    Table630( const Table630& theTable630 );
    virtual ~Table630();

    /** The scheduler responsible for socket bound events. */
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    /** The scheduler responsible for process events. */
    TA_IRS_Bus::Scheduler& m_processScheduler;
    /** The source of the ConnectToPAFEPC event instance. */
    IPasEventSource& m_pasEventSource;
};


/**
 * Event that will write data to Table 630 in PAS.
 */
class WriteTable630 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this event.
     * @param table The reference to Table 630.
     * @param future The reference to the future object that will store the result of the write.
     * @param messageKey The key of the message to record.
     * @param sourceId The source of the recording as defined in List 50.
     */
    WriteTable630( Table630& table,
                   ACE_Future<int>& future,
                   unsigned int messageKey,
                   CORBA::Octet sourceId,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /**
     * Constructs an instance of this event.
     * @param timeToExpire The time the event is consumed in the scheduler.
     * @param table The reference to Table 630.
     * @param future The reference to the future object that will store the result of the write.
     * @param messageKey The key of the message to record.
     * @param sourceId The source of the recording as defined in List 50.
     */
    WriteTable630( ACE_Time_Value timeToExpire,
                   Table630& table,
                   ACE_Future<int>& future,
                   unsigned int messageKey,
                   CORBA::Octet sourceId,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable630();
    /**
     * writeTable called by PASWriteEvent::consume().
     * In this case, the data will be written to PAS
     */
    virtual void writeTable();

    /* This function is used to recreate the PASWriteEvent with a new
     * timeToExpire.
     */
    virtual PASWriteEvent* recreateEvent(ACE_Time_Value timeToExpire);

private:
    /** The reference to Table 630. */
    Table630& m_table;
    /** The key of the message to record. */
    unsigned int m_messageKey;
    /** The source of the recording as defined in List 50. */
    CORBA::Octet m_sourceId;
};

} // namespace TA_IRS_App

#endif // !defined(Table630_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

