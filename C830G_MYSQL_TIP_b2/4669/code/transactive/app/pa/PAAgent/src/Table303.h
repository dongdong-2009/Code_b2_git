/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table303.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table303_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table303_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "app/pa/PAAgent/src/Table560.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

namespace TA_Base_Bus
{
    class DataPoint;
}

namespace TA_IRS_Bus
{
    class CachedMaps;
}


namespace TA_IRS_App
{
class IPasEventSource;

struct MessageSequenceDescription
{
    MessageSequenceDescription()
        : m_hasChime(false),
          m_dwellInSecs(0),
          m_periodInSecs(0),
          m_startTime(0),
          m_stopTime(0),
          m_isEventTriggered(false),
          m_messages(),
          m_zones(),
          m_localCoverageBitmap(0)
    {}

    bool                                    m_hasChime;
    TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq m_messages;
    unsigned short                          m_dwellInSecs;
    unsigned short                          m_periodInSecs;
    TA_Base_Core::DateTime                       m_startTime;
    TA_Base_Core::DateTime                       m_stopTime;
    bool                                    m_isEventTriggered;
    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage  m_zones;
    ta_uint32                               m_localCoverageBitmap;
};




class Table303 : public AbstractPASTable
{

    friend class ReadTable303;
    friend class ProcessTable303;

public:

    Table303( unsigned long locationKey );

    bool hasStartTimeElapsed(unsigned long sequenceId);

    TA_Base_Core::DateTime getStartTime(unsigned long sequenceId);

    TA_Base_Core::DateTime getStopTime(unsigned long sequenceId);

    bool isMessageSequenceCyclic(unsigned long sequenceId);

protected:

    virtual void processRead();
    unsigned long getMessageKeyAndValidate(unsigned long messageId);

private:

    Table303( const Table303& theTable303 );
    virtual ~Table303();

protected:

    std::vector<MessageSequenceDescription> m_messageSequenceDescriptions;
    unsigned long                           m_locationKey;
};


/**
 * Event that will read Table 303 from PAS.
 */
class ReadTable303 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 303.
     * @param table560 The reference to Table 560.
     * @param processScheduler The reference to the scheduler responsible for process events.
     */
    ReadTable303( Table303& table,
                  Table560& table560,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable303();
    /**
     * readTable called by PASReadEvent::consume().  In this case, this will simply read from PAS.
     */
    virtual void readTable();

private:
    /** The reference to Table303. */
    Table303& m_table;
    /** The reference to Table560. */
    Table560& m_table560;
};


/**
 * Event encapsulation to process Table 303.
 */
class ProcessTable303 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table Table 303 instance.
     */
    ProcessTable303( Table303& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable303();
    /**
     * Consumes the event.  In this case, the event will update the
     * appropriate datapoints.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event deletes itself. */
    virtual void cancel();
private:
    /** The table that this is processing. */
    Table303& m_table;
};


} // namespace TA_IRS_App

#endif // !defined(Table303_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

