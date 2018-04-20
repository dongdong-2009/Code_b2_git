/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table302.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table302_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table302_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "core/exceptions/src/PasExceptions.h"
#include "core/synchronisation/src/Condition.h"

class PasHelpers;


namespace TA_IRS_App
{
class IPasEventSource;

struct MessageSequenceStatus
{    
    enum EState
    {
        NoFault                                         = 0,
        CyclicPlayingNow                                = 1,
        NotUsed2                                        = 2,
        CyclicInDwellPeriod                             = 3,
        NoDvaChannelAvailableAtStart                    = 4,
        DvaMessageCurrentlyRecording                    = 5,
        DvaChannelRequestedForHigherPriorityBroadcast   = 6,
        NotUsed3                                        = 7,
        BroadcastFinishedAbnormally                     = 8,
        BroadcastFinishedNormally                       = 9
    } m_status;
    CORBA::Octet m_announceId;
	
	// jeffrey++ TES712
    //CORBA::Octet m_priority;
    CORBA::Octet m_sourceid;

    MessageSequenceStatus()
        : m_status(NoFault), 
          m_announceId(0),
          //m_priority(0)
		  m_sourceid(0)
    {}
	// ++jeffrey TES712
};



class Table302 : public AbstractPASTable
{

    friend class ReadTable302;
    friend class ProcessTable302;

public:
    Table302();

    MessageSequenceStatus getMessageSequenceStatus(int messageSequenceId);
        //throw ( TA_Base_Core::PasConnectionException );

    bool isRunning(int messageSequenceId);

    int getNextAvailableMessageSequenceId();
        //throw ( TA_Base_Core::NoFreeMessageSequenceIdException, TA_Base_Core::PasConnectionException );

    /**
     * Returns the sequence id associated with the specified announceId.  A
     * return of 0 means the announce id does not exist in this table.
     * @param announceId The announce id to search for.
     * @return sequence id associated with provided announce id.
     */
    unsigned int getSequenceId( CORBA::Octet announceId );



protected:

    virtual void processRead();

    void killBroadcast(int sequenceId);

private:
    Table302( const Table302& theTable302);
    virtual ~Table302();

    std::vector<MessageSequenceStatus>  m_messageSequenceStatuses;
    PasHelpers*                         m_pasHelpersInstance;

    /** Condition that will be signalled when a read has taken place. */
    TA_Base_Core::Condition m_readWait;    
    /** Condition that will be signalled when a read has taken place. */
    TA_Base_Core::Condition m_processWait;    
};


/**
 * Event encapsulation to read Table 302 from PAS.
 */
class ReadTable302 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class. 
     * @param table Table 302 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable302( Table302& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /**
     * Constructs an instance of this class. 
     * @param expiryTime The time the event will expire.
     * @param table Table 302 instance.
     * @param socketScheduler The scheduler to resubmit a read event.
     * @param processScheduler The scheduler to resubmit a process event.
     */
    ReadTable302( ACE_Time_Value expiryTime,
                  Table302& table,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable302();
    /**
     * readTable called by PASReadEvent::consume().  In this case, the event will merely read the table
     * and then submit another instance of this event to expire the configured
     * poll cycle.
     */
    virtual void readTable();

private:
    /** The table that this is reading for. */
    Table302& m_table;
};

/**
 * Event encapsulation to process Table 302.
 */
class ProcessTable302 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class. 
     * @param table Table 302 instance.
     */
    ProcessTable302( Table302& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable302();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table302& m_table;
};

}

#endif // !defined(Table302_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
