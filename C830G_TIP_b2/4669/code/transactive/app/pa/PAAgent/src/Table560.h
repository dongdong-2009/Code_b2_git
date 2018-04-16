/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table560.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */


#if !defined(Table560_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table560_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/PASWriteEvent.h"
#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"

#include "ace/Future.h"

class PasHelpers;

namespace TA_IRS_App
{
class IPasEventSource;

class Table560 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class ReadTable560;
    friend class WriteTable560;
    friend class ProcessTable560;

public:

    static const unsigned int TABLE_560_TABLE_100_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_100_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_202_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_202_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_204_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_204_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1204_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1204_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_303_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_303_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1303_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1303_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_550_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_550_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_570_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_570_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_590_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_590_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_603_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_603_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_604_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_604_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_610_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_610_FLAG_B_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1610_FLAG_A_BIT_OFFSET;
    static const unsigned int TABLE_560_TABLE_1610_FLAG_B_BIT_OFFSET;

    Table560( TA_IRS_Bus::Scheduler& socketScheduler,
              TA_IRS_Bus::Scheduler& processScheduler,
              AbstractPASTable* table100,
              AbstractPASTable* table202,
              AbstractPASTable* table204,
              AbstractPASTable* table1204,
              AbstractPASTable* table303,
              AbstractPASTable* table1303,
              AbstractPASTable* table550,
              AbstractPASTable* table570,
              AbstractPASTable* table590,
              AbstractPASTable* table603,
              AbstractPASTable* table604,
              AbstractPASTable* table610,
              AbstractPASTable* table1610,
              IPasEventSource& eventSource );
    void resetFlags( unsigned int tableFlagA, unsigned int tableFlagB );

    void resetTableFlags( unsigned int tableFlagA, unsigned int tableFlagB );
    //void resetInvalidFlags( unsigned int tableFlagA, unsigned int tableFlagB );

    bool isAnyFlagAset();


protected:

    virtual void processRead();
    void processTableFlag(unsigned int tableFlagA, unsigned int tableFlagB);


private:

    Table560( const Table560& theTable560);
    virtual ~Table560();

protected:

    struct FlagBitOffsets
    {
        FlagBitOffsets(unsigned int a, unsigned int b) : m_flagABitOffset(a), m_flagBBitOffset(b)
        {}
        FlagBitOffsets() : m_flagABitOffset(-1), m_flagBBitOffset(-1)
        {}

        unsigned int m_flagABitOffset;
        unsigned int m_flagBBitOffset;
    };

    PasHelpers* m_pasHelpersInstance;
    /** Reference to the scheduler that will deal with read events. */
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    /** Reference to the scheduler that will deal with process events. */
    TA_IRS_Bus::Scheduler& m_processScheduler;
    /** Point to Table100. */
    AbstractPASTable* m_table100;
    /** Point to Table202. */
    AbstractPASTable* m_table202;
    /** Point to Table204. */
    AbstractPASTable* m_table204;
    /** Point to Table1204. */
    AbstractPASTable* m_table1204;
    /** Point to Table303. */
    AbstractPASTable* m_table303;
    /** Point to Table1303. */
    AbstractPASTable* m_table1303;
    /** Point to Table550. */
    AbstractPASTable* m_table550;
    /** Point to Table570. */
    AbstractPASTable* m_table570;
    /** Point to Table590. */
    AbstractPASTable* m_table590;
    /** Point to Table603. */
    AbstractPASTable* m_table603;
    /** Point to Table604. */
    AbstractPASTable* m_table604;
    /** Point to Table610. */
    AbstractPASTable* m_table610;
    /** Point to Table1610. */
    AbstractPASTable* m_table1610;
    /** Boolean indicating that a WriteTable560 event is in existance. */
    bool m_writeTable560;
    /** The source of ConnectToPAFEPC events in case of disconnection. */
    IPasEventSource& m_pasEventSource;
};


/**
 * Event encapsulation to read Table 560 from PAS.
 */
class ReadTable560 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 560 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     * @param eventSource The source of connect events.
     */
    ReadTable560( Table560& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /**
     * Constructs an instance of this class.
     * @param expiryTime The time the event will expire.
     * @param table Table 560 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable560( ACE_Time_Value expiryTime,
                  Table560& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable560();
    /**
     * readTable called by PASReadEvent::consume()  In this case, the event will merely read the table
     * and then submit another instance of this event to expire the configured
     * poll cycle.
     */
    virtual void readTable();

private:
    /** The table that this is reading for. */
    Table560& m_table;
};


/**
 * Event encapsulation to process Table 560.
 */
class ProcessTable560 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 560 instance.
     */
    ProcessTable560( Table560& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable560();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table560& m_table;
};


/**
 * Event that will write this table to PAS.
 */
class WriteTable560 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 201 instance.
     */
    WriteTable560( Table560& table,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );
    /**
     * Constructs an instance of this class.
     * @param timeToExpire The time the event is consumed in the scheduler.
     * @param table The reference to Table 201 instance.
     */
    WriteTable560( ACE_Time_Value timeToExpire,
                   Table560& table,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable560();
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
    Table560& m_table;
};

} // namespace TA_IRS_App

#endif // !defined(Table560_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

