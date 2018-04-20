/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWatchdog.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableIscsWatchdog.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWatchdog.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

//
// the m_tableData is set to its maximum value, so that the first real
// value that comes in is properly assessed, as are subsequent overflows.
ATSSimTableIscsWatchdog::ATSSimTableIscsWatchdog(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(IscsWatchdog,parent, loc)
, m_tableData(0xFFFF)
{
    m_numberOfRecords=1;
}


ATSSimTableIscsWatchdog::~ATSSimTableIscsWatchdog()
{
}


unsigned short ATSSimTableIscsWatchdog::getTableSize() const
{
    return (m_numberOfRecords * 2);
}



UserResponse * ATSSimTableIscsWatchdog::processUserRead(UserQuery * ur)
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


UserResponse * ATSSimTableIscsWatchdog::processUserWrite(UserQuery * ur)
{
    return 0;
}


void ATSSimTableIscsWatchdog::toStream(std::ostream& os) const
{
}


void ATSSimTableIscsWatchdog::fromStream(const std::ostream& os)
{
}


void ATSSimTableIscsWatchdog::toNetwork(ByteVector & n)
{
    ATSSimUtility::putWord(m_tableData,n);
}


void ATSSimTableIscsWatchdog::fromNetwork(const ByteVector & s)
{
	int x=0;
    Word newCounter = ATSSimUtility::getWord(s,x);

    if (m_tableData == 0xFFFF)
    {
        // do nothing
    }
    else if (newCounter == m_tableData)
    {
        // the AtsAgent hasn't updated it. Naughty naughty.
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
        // must have skipped a few beats.  Nothing too harmful, maybe.
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter incremented by %d", newCounter-m_tableData);
    }
    else if (newCounter != m_tableData+4)
    {
        // must have incremented by 1,2,3,5,6.  Not so bad, but worth noting
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Iscs Watchdog counter incremented by %d", newCounter-m_tableData);
    }

    m_tableData = newCounter;
}


void ATSSimTableIscsWatchdog::getByteVector(ByteVector & bv) const
{
}

