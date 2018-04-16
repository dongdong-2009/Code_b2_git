/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table1201.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table1201_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table1201_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/AbstractWriteablePASTable.h"
#include "app/pa/PAAgent/src/PASWriteEvent.h"

#include "ace/Future.h"


namespace TA_IRS_App
{
class IPasEventSource;

class Table1201 : public TA_IRS_App::AbstractWriteablePASTable
{

    friend class WriteTable1201;

public:

    Table1201( TA_IRS_Bus::Scheduler& socketScheduler,
               TA_IRS_Bus::Scheduler& processScheduler,
               IPasEventSource& eventSource );

    void setTableDataAndWrite( CORBA::Octet sourceId, 
                               CORBA::Octet announceId, 
                               const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones );
        //throw ( TA_Base_Core::PasWriteErrorException, TA_Base_Core::PasConnectionException );

private:
    Table1201( const Table1201& theTable1201);
    virtual ~Table1201();

    /** The scheduler responsible for socket bound events. */
    TA_IRS_Bus::Scheduler& m_socketScheduler;
    /** The scheduler responsible for process events. */
    TA_IRS_Bus::Scheduler& m_processScheduler;
    /** The source of the ConnectToPAFEPC event instance. */
    IPasEventSource& m_pasEventSource;
};


/**
 * Event that will write this request to PAS.
 */
class WriteTable1201 : public TA_IRS_App::PASWriteEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 1201 instance.
     * @param future The Future's object that will carry the result.
     * @param sourceId The source of the broadcast request.
     * @param announceId The user id that PAS will use for identifying the
     *                   broadcast in table 1301.
     * @param zones The bit map that indicates the zone coverage.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process bound events.
     * @param eventSource The source of PAS events.
     */
    WriteTable1201( Table1201& table,
                   ACE_Future<int>& future,
                   CORBA::Octet sourceId,
                   CORBA::Octet announceId,
                   const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );

    /**
     * Constructs an instance of this class.
     * @param timeToExpire The time the event should be consumed in the scheduler
     * @param table The reference to Table 1201 instance.
     * @param future The Future's object that will carry the result.
     * @param sourceId The source of the broadcast request.
     * @param announceId The user id that PAS will use for identifying the
     *                   broadcast in table 1301.
     * @param zones The bit map that indicates the zone coverage.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process bound events.
     * @param eventSource The source of PAS events.
     */
    WriteTable1201( ACE_Time_Value timeToExpire,
                    Table1201& table,
                    ACE_Future<int>& future,
                    CORBA::Octet sourceId,
                    CORBA::Octet announceId,
                    const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
                    TA_IRS_Bus::Scheduler& socketScheduler,
                    TA_IRS_Bus::Scheduler& processScheduler,
                    IPasEventSource& eventSource );

    /** Destructs this instance of this class. */
    virtual ~WriteTable1201();
    /**
     * writeTable called by PASWriteEvent::consume()  In this case, the event will write out the necessary data.
     */
    virtual void writeTable();

    /* This function is used to recreate the PASWriteEvent with a new
     * timeToExpire.
     */
    virtual PASWriteEvent* recreateEvent(ACE_Time_Value timeToExpire);

private:
    /** The reference to the Table 1201 instance. */
    Table1201& m_table;
    /** The source of the broadcast request. */
    CORBA::Octet m_sourceId;
    /** The user id that PAS will use for identifying the broadcast in table 301. */
    CORBA::Octet m_announceId;
    /** The bit map that indicates the zone coverage. */
    const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage m_zones;
};



} // namespace TA_IRS_App

#endif // !defined(Table11201_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

