/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table603.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table603_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table603_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "app/pa/PAAgent/src/Table560.h"

namespace TA_Base_Core
{
    class CommsMessageSender;
}

namespace TA_IRS_App
{
class IPasEventSource;

struct MessageLabelType
{
    MessageLabelType()
        : m_label(""),
          m_type('N')
    {}

    std::string m_label;
    char        m_type;
};

class Table603 : public AbstractPASTable
{

    friend class ReadTable603;
    friend class ProcessTable603;

public:

    Table603();

protected:

    virtual void processRead();
        //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException);

    void updateDatabaseAndBroadcastUpdates();
        //throw (TA_Base_Core::DatabaseException, TA_Base_Core::PasConnectionException);

private:
    Table603( const Table603& theTable603 );
    virtual ~Table603();

protected:

    std::vector<MessageLabelType>   m_messageLabelTypes;
    TA_Base_Core::CommsMessageSender*    m_paAgentCommsSender;
    unsigned long                   m_minMessageId;
    unsigned long                   m_maxMessageId;
    unsigned long                   m_locationKey;

};


/**
 * Event that will read Table 603 from PAS.
 */
class ReadTable603 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 603.
     * @param table The reference to Table 560.
     * @param processScheduler The reference to the scheduler that will deal with process
     *                         events.
     */
    ReadTable603( Table603& table,
                  Table560& table560,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable603();
    /**
     * readTable called by PASReadEvent::consume().  In this case, the table will be read from PAS.
     */
    virtual void readTable();

private:
    /** The reference to Table 603. */
    Table603& m_table;
    /** The reference to Table 560. */
    Table560& m_table560;
};


/**
 * Event that will process Table 603.
 */
class ProcessTable603 : public TA_IRS_Bus::IEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 603.
     */
    ProcessTable603( Table603& table );
    /** Destructs this instance of this class. */
    virtual ~ProcessTable603();
    /**
     * Consumes this event.  In this case, the table will be read from PAS.
     * The event will then delete itself.
     */
    virtual void consume();
    /** Cancels the event.  In this case, the event will delete itself. */
    virtual void cancel();
private:
    /** The reference to Table 603. */
    Table603& m_table;
};


} // namespace TA_IRS_App

#endif // !defined(Table603_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

