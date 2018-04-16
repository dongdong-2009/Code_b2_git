/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsWatchdog.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTableIscsWatchdog.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/PA/PASimulatorLibrary/src/PASimTableIscsWatchdog.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

//
// the m_tableData is set to its maximum value, so that the first real
// value that comes in is properly assessed, as are subsequent overflows.
PASimTableIscsWatchdog::PASimTableIscsWatchdog(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(IscsWatchdog,parent, loc)
, m_tableData(0xFFFF)
{
    m_numberOfRecords=1;
}


PASimTableIscsWatchdog::~PASimTableIscsWatchdog()
{
}


unsigned short PASimTableIscsWatchdog::getTableSize() const
{
    return (m_numberOfRecords * 2);
}



UserResponse * PASimTableIscsWatchdog::processUserRead(UserQuery * ur)
{
    if ( (ur->getType() == Show) || (ur->getType() == Dump) )
    {
        UserResponse * response = new UserResponse(ur);

        if (ur->getOutputFormat() == f_tabular)
        {
            char buffStr[256];
            sprintf(buffStr,"+------------------+-------+\n| Watchdog Counter | %.5u |\n+------------------+-------+\n", m_tableData);

            response->m_responseData = buffStr;
        }
        else
        {
            char buffStr[56];
            sprintf(buffStr,"Iscs Watchdog Counter = %.4X", m_tableData);
            response->m_responseData = buffStr;
        }

        return response;
    }

    return 0;
}


UserResponse * PASimTableIscsWatchdog::processUserWrite(UserQuery * ur)
{
    return 0;
}


void PASimTableIscsWatchdog::toStream(std::ostream& os) const
{
}


void PASimTableIscsWatchdog::fromStream(const std::ostream& os)
{
}


void PASimTableIscsWatchdog::toNetwork(ByteVector & n)
{
    PASimUtility::putWord(m_tableData,n);
}


void PASimTableIscsWatchdog::fromNetwork(const ByteVector & s)
{
	int x=0;
    Word newCounter = PASimUtility::getWord(s,x);

    if (m_tableData == 0xFFFF)
    {
        // do nothing
    }
    else if (newCounter == m_tableData)
    {
        // the PAAgent hasn't updated it. Naughty naughty.
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter NOT incremented!");
    }
    else if (newCounter < 6)
    {
        // the counter has recently rolled over to zero
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter rollover to zero");
    }
    else if (newCounter < m_tableData)
    {
        // the counter is less than previous, but hasn't rolled over
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter decremented!");
    }
    else if (newCounter > m_tableData+6)
    {
        // must have skipped a few bePA.  Nothing too harmful, maybe.
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter incremented by %d", newCounter-m_tableData);
    }
    else if (newCounter != m_tableData+4)
    {
        // must have incremented by 1,2,3,5,6.  Not so bad, but worth noting
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter incremented by %d", newCounter-m_tableData);
    }

    m_tableData = newCounter;
}


void PASimTableIscsWatchdog::getByteVector(ByteVector & bv) const
{
}

