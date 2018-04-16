/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTablePAWatchdog.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTablePAWatchdog.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/PA/PASimulatorLibrary/src/PASimTablePAWatchdog.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"
#include <iostream>
#include <stdexcept>
#include <time.h>
#include <limits.h>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

PASimTablePAWatchdog::PASimTablePAWatchdog(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(PAWatchdog,parent, loc)
, m_counter(0)
, m_offset(0)
, m_pause(false)
{
    m_numberOfRecords = 1;
    m_counter = (time(0) & 0x0000ffff);
}


PASimTablePAWatchdog::~PASimTablePAWatchdog()
{
}


unsigned short PASimTablePAWatchdog::getTableSize() const
{
    return (m_numberOfRecords * 2);
}



UserResponse * PASimTablePAWatchdog::processUserRead(UserQuery * ur)
{
    if ( (ur->getType() == Show) || (ur->getType() == Dump) )
    {
        UserResponse * response = new UserResponse(ur);


        char buffStr[256];
        sprintf(buffStr,"+------------------+-------+\n| Watchdog Counter | %.5u |\n", m_counter);
        response->m_responseData = buffStr;

        sprintf(buffStr,"|    time() Offset | %.5u |\n", m_offset);
        response->m_responseData+= buffStr;

        std::string pauseStr;
        PASimUtility::bool2string(m_pause, pauseStr);
        sprintf(buffStr,"|            Pause | %s |\n+------------------+-------+\n", pauseStr.c_str());
        response->m_responseData+= buffStr;

        return response;
    }
    return 0;
}


UserResponse * PASimTablePAWatchdog::processUserWrite(UserQuery * qry)
{
    if (qry->getType() == Set)
    {
        UserResponse * response = new UserResponse(qry);
        while (qry->moreArgs())
        {
            UserQuery::FieldValuePair fvp = qry->popArg();

            unsigned long field = fvp.first;
			UserQuery::FieldValueType value = fvp.second;

            if (field == sValueField)
            {
                // can't really change the counter per se, as it is set from
                // the time() - it is only kept as a data member for printing
                // it in processUserRead. However the offset will be set
                // relative to the current counter. Which has all got to be
                // correctly wrapped around USHRT_MAX (=65535)
                if (m_counter > value)
                {
                    m_offset = (value - m_counter + USHRT_MAX);
                }
                else
                {
                    m_offset = value - m_counter;
                }

                std::cout << "Modified counter value=" << value << " (m_counter+m_offset)" << std::endl;
            }
            else if (field == sOffsetField)
            {
                m_offset = value;
                std::cout << "Modified offset value=" << value << std::endl;
            }
            else if (field == sPauseResumeField)
            {
                m_pause = (value != 0);
                std::cout << "Modified pause/resume value=" << value << std::endl;
            }
            else
            {
                response->m_responseData+= "ERROR: invalid field byte number: ";
                response->m_responseData+= PASimUtility::uitoa(field);
            }
        }

        return response;
    }
    return 0;
}


void PASimTablePAWatchdog::toStream(std::ostream& os) const
{
}


void PASimTablePAWatchdog::fromStream(const std::ostream& os)
{
}


void PASimTablePAWatchdog::toNetwork(ByteVector & n)
{
    // only update the counter if we're not paused
    if (!m_pause)
    {
        m_counter = (time(0) & 0x0000ffff) + m_offset;
    }

    PASimUtility::putWord(m_counter,n);
}


void PASimTablePAWatchdog::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("PA Watchdog Table should not be updated fromNetwork");
}


void PASimTablePAWatchdog::getByteVector(ByteVector & bv) const
{
}

Word PASimTablePAWatchdog::getValueFieldNumber()
{
    return sValueField;
}

Word PASimTablePAWatchdog::getOffsetFieldNumber()
{
    return sOffsetField;
}

Word PASimTablePAWatchdog::getPauseResumeFieldNumber()
{
    return sPauseResumeField;
}

const Word PASimTablePAWatchdog::sValueField = 0;
const Word PASimTablePAWatchdog::sOffsetField = 2;
const Word PASimTablePAWatchdog::sPauseResumeField = 4;
