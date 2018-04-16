/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table604.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table604_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table604_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"
#include "app/pa/PAAgent/src/Table560.h"

namespace TA_Base_Core
{
    class CommsMessageSender;
}

namespace TA_IRS_App
{
class IPasEventSource;

class Table604 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class ReadTable604;
    friend class ProcessTable604;
    friend class WriteTable604;

public:

    Table604( TA_IRS_Bus::Scheduler& socketScheduler,
              TA_IRS_Bus::Scheduler& processScheduler,
              unsigned long locationKey ,
              IPasEventSource& eventSource);

    void setTableDataAndWrite( unsigned int messageKey, char messageType ) ;
        //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException, TA_Base_Core::DatabaseException );

protected:

    virtual void processRead();
        //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException);

    void updateDatabaseAndBroadcastUpdates();
        //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException);

    char getAdhocType(unsigned int messageId);
        //throw ( TA_Base_Core::PasConnectionException );

private:

    void synchronise();
        //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException, TA_Base_Core::DatabaseException );

    Table604( const Table604& theTable604);
    virtual ~Table604();

protected:

    TA_Base_Core::CommsMessageSender*    m_paAgentCommsSender;
    unsigned long                   m_minMessageId;
    unsigned long                   m_maxMessageId;
    unsigned long                   m_locationKey;
    bool                            m_isSynchronised;
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    TA_IRS_Bus::Scheduler& m_processScheduler;
    IPasEventSource& m_pasEventSource;
};


/**
 * Event encapsulation to read Table 604 from PAS.
 */
class ReadTable604 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 604 instance.
     * @param table560 Table 560 instance.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable604( Table604& table,
                  Table560& table560,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable604();
    /**
     * readTable called by PASReadEvent::consume()  In this case, the event will merely read the table
     * and then submit another instance of this event to expire the configured
     * poll cycle.
     */
    virtual void readTable();

private:
    /** The table that this is reading for. */
    Table604& m_table;
    /** The reference to Table 560. */
    Table560& m_table560;
};


/**
 * Event encapsulation to process Table 604.
 */
class ProcessTable604 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 604 instance.
     */
    ProcessTable604( Table604& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable604();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table604& m_table;
};


/**
 * Event that will write this table to PAS.
 */
class WriteTable604 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 604 instance.
     */
    WriteTable604( Table604& table,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /**
     * Constructs an instance of this class.
     * @param timeToExpire The time the event is consumed in the scheduler.
     * @param table The reference to Table 604 instance.
     */
    WriteTable604( ACE_Time_Value timeToExpire,
                   Table604& table,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable604();
    /**
     * writeTable called by PASWriteEvent::consume().
     * In this case, the event will write out the necessary data.
     */
    virtual void writeTable();

    /* This function is used to recreate the PASWriteEvent with a new
     * timeToExpire.
     */
    virtual PASWriteEvent* recreateEvent(ACE_Time_Value timeToExpire);

private:
    /** The reference to the Table 201 instance. */
    Table604& m_table;
};

} // namespace TA_IRS_App


#endif // !defined(Table604_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

