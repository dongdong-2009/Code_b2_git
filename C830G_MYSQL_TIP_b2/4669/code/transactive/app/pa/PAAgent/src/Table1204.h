/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table1204.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table1204_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table1204_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_


#include <sys/types.h> 
#include <sys/timeb.h>
#include <string>

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "app/pa/PAAgent/src/Table560.h"


namespace TA_IRS_App
{
class IPasEventSource;

class Table1204 : public AbstractPASTable
{

    friend class ReadTable1204;

public:

    Table1204();

    TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage getLocalCoverage(unsigned int sourceId);
        //throw ( TA_Base_Core::PasConnectionException );

protected:

    virtual void processRead();

private:
    Table1204( const Table1204& theTable1204 );
    virtual ~Table1204();

};


/**
 * Event that will read Table 1204 from PAS.
 */
class ReadTable1204 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 1204.
     * @param table560 The reference to Table 560.
     * @param socketScheduler The scheduler responsible for socket bound events.
     * @param processScheduler The scheduler responsible for process events.
     * @param eventSource The source of Pas events.
     */
    ReadTable1204( Table1204& table,
                   Table560& table560,
                   TA_IRS_Bus::Scheduler& socketScheduler,
                   TA_IRS_Bus::Scheduler& processScheduler,
                   IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable1204();
    /**
     * readTable called by PASReadEvent::consume(). In this case, this will simply read from PAS.
     */
    virtual void readTable();

private:
    /** The reference to Table1204. */
    Table1204& m_table;
    /** The reference to Table560. */
    Table560& m_table560;
};

} // namespace TA_IRS_App

#endif // !defined(Table11204_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

