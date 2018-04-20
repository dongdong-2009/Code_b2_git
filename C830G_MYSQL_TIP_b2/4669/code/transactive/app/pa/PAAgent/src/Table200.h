/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table200.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table200_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table200_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "core/synchronisation/src/Condition.h"

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_Bus
{
    class CachedMaps;
}

class PasHelpers;

namespace TA_IRS_App
{
class IPasEventSource;
class Table202;
class Table302;
class Table303;

struct ZoneStatus
{
    ZoneStatus()
        : m_sourceId(0), m_status(false), m_priority(0) 
    {}

    CORBA::Octet m_sourceId;
    bool m_status;
    CORBA::Octet m_priority;
};


class Table200 : public AbstractPASTable
{

    friend class ReadTable200;
    friend class ProcessTable200;
    friend class InvalidateTable200;

public:
	// jeffrey++ Bug457
	int m_readIndex;
	// ++jeffrey Bug457

    Table200(Table202* table202, Table302* table302, Table303* table303);

    ZoneStatus getZoneStatus(unsigned int zoneId);

    //liuyu++ TD11035
    TA_Base_Core::Condition& getCondition() { return m_readWait; }
    //++liuyu TD11035

	// ++ Bug456
	void readTimedWait();
	// ++ Bug456

	// TD 17013
	void setInitalThreadValue(bool initialThreadValue);
	// TD 17013


private:
    Table200( const Table200& theTable200);
    virtual ~Table200();

protected:

    virtual void processRead();
    virtual void invalidateAllDataPoints();


    std::vector<ZoneStatus> m_zoneStatuses;
    TA_IRS_Bus::CachedMaps* m_cachedMapsInstance;
    PasHelpers*             m_pasHelpersInstance;

	bool m_initialThread; // TD17013

    // These tables are needed for cross-referencing to determine the 'cyclic' status of a zone
    Table202* m_table202;
    Table302* m_table302;
    Table303* m_table303;

    TA_Base_Core::Condition m_readWait;
};


/**
 * Event encapsulation to read Table 200 from PAS.
 */
class ReadTable200 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class. 
     * @param table Table 200 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable200( Table200& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable200();
    /**
     * readTable called by PASReadEvent::consume().  In this case, the event will merely read the table
     * and then submit another instance of this event to expire the configured
     * poll cycle.
     */
    virtual void readTable();

private:
    /**
     * Constructs an instance of this class. 
     * @param expiryTime The time the event will expire.
     * @param table Table 200 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable200( ACE_Time_Value expiryTime,
                  Table200& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** The table that this is reading for. */
    Table200& m_table;
};

/**
 * Event encapsulation to process Table 200.
 */
class ProcessTable200 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class. 
     * @param table Table 200 instance.
     */
    ProcessTable200( Table200& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable200();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table200& m_table;
};


/**
 * Event encapsulation to invalidate all datapoints associated with Table 200.
 */
class InvalidateTable200 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 200 instance.
     */
    InvalidateTable200( Table200& table );
    /** Destructs this instance of this class. */
    virtual ~InvalidateTable200();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table200& m_table;
};



}

#endif // !defined(Table200_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
