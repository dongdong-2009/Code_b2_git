/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table202.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table202_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table202_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "app/pa/PAAgent/src/Table560.h"

namespace TA_IRS_App
{
class IPasEventSource;

class Table202 : public AbstractPASTable
{

    friend class ReadTable202;
    friend class ProcessTable202;

public:

    Table202();

    CORBA::Octet getAnnounceId(unsigned int sourceId);
        //throw ( TA_Base_Core::PasConnectionException );

    /**
     * Returns the source id associated with the specified announceId.  A
     * return of 0 means the announce id does not exist in this table.
     * @param announceId The announce id to search for.
     * @return source id associated with provided announce id.
     */
    unsigned int getSourceId( CORBA::Octet announceId );

protected:

    virtual void processRead();

private:
    Table202( const Table202& theTable202 );
    virtual ~Table202();

};

/**
 * Event that will read Table 202 from PAS.
 */
class ReadTable202 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 202.
     * @param table560 The reference to Table 560.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param socketScheduler The scheduler responsible for process events.
     * @param eventSource The source of Pas events.
     */
    ReadTable202( Table202& table,
                  Table560& table560,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable202();
    /**
     * readTable called by PASReadEvent::consume().  In this case, this will simply read from PAS.
     */
    virtual void readTable();

private:
    /** The reference to Table202. */
    Table202& m_table;
    /** The reference to Table560. */
    Table560& m_table560;
};

} // namespace TA_IRS_App

#endif // !defined(Table202_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

