/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/Table590.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(Table590_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)
#define Table590_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_

#include "app/pa/PAAgent/src/PASReadEvent.h"
#include "app/pa/PAAgent/src/AbstractPASTable.h"
#include "app/pa/PAAgent/src/Table560.h"


namespace TA_IRS_App
{
class IPasEventSource;

class Table590 : public AbstractPASTable
{

    friend class ReadTable590;

public:

    Table590();

    bool isPttStatusOn(unsigned int pagingConsoleSourceId);
        //throw ( TA_Base_Core::PasConnectionException );

protected:

    virtual void processRead();

private:
    Table590( const Table590& theTable590 );
    virtual ~Table590();

protected:

};

/**
 * Event that will read Table 590 from PAS.
 */
class ReadTable590 : public TA_IRS_App::PASReadEvent
{
public:
    /**
     * Constructs an instance of this class.
     * @param table The reference to Table 590.
     * @param table560 The reference to Table 560.
     */
    ReadTable590( Table590& table,
                  Table560& table560,
                  TA_IRS_Bus::Scheduler& socketScheduler,
                  TA_IRS_Bus::Scheduler& processScheduler,
                  IPasEventSource& eventSource );
    /** Destructs this instance of this class. */
    virtual ~ReadTable590();
    /**
     * readTable called by PASReadEvent::consume().  In this case, Table 590 is read from PAS.
     */
    virtual void readTable();

private:
    /** The reference to Table 590. */
    Table590& m_table;
    /** The reference to Table 560. */
    Table560& m_table560;
};

} // namespace TA_IRS_App

#endif // !defined(Table590_B323CC80_49BA_4d42_98A5_8CE9884F7924__INCLUDED_)

