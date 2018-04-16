/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWatchdog.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *
  * Definition file for class ATSSimTableAtsWatchdog.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAtsWatchdog.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include <iostream>
#include <stdexcept>
#include <time.h>
#include <limits.h>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

ATSSimTableAtsWatchdog::ATSSimTableAtsWatchdog(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(AtsWatchdog,parent, loc)
, m_counter(0)
, m_offset(0)
, m_pause(false)
{
    m_numberOfRecords = 1;
    m_counter = (time(0) & 0x0000ffff);
}


ATSSimTableAtsWatchdog::~ATSSimTableAtsWatchdog()
{
}


unsigned short ATSSimTableAtsWatchdog::getTableSize() const
{
    return (m_numberOfRecords * 2);
}



UserResponse * ATSSimTableAtsWatchdog::processUserRead(UserQuery * ur)
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
        ATSSimUtility::bool2string(m_pause, pauseStr);
        sprintf(buffStr,"|            Pause | %s |\n+------------------+-------+\n", pauseStr.c_str());
        response->m_responseData+= buffStr;

        return response;
    }
    return 0;
}


UserResponse * ATSSimTableAtsWatchdog::processUserWrite(UserQuery * qry)
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
                response->m_responseData+= ATSSimUtility::uitoa(field);
            }
        }

        return response;
    }
    return 0;
}


void ATSSimTableAtsWatchdog::toStream(std::ostream& os) const
{
}


void ATSSimTableAtsWatchdog::fromStream(const std::ostream& os)
{
}


void ATSSimTableAtsWatchdog::toNetwork(ByteVector & n)
{
    // only update the counter if we're not paused
    if (!m_pause)
    {
        m_counter = (time(0) & 0x0000ffff) + m_offset;
    }

    ATSSimUtility::putWord(m_counter,n);
}


void ATSSimTableAtsWatchdog::fromNetwork(const ByteVector & s)
{
    throw std::logic_error("Ats Watchdog Table should not be updated fromNetwork");
}


void ATSSimTableAtsWatchdog::getByteVector(ByteVector & bv) const
{
}

Word ATSSimTableAtsWatchdog::getValueFieldNumber()
{
    return sValueField;
}

Word ATSSimTableAtsWatchdog::getOffsetFieldNumber()
{
    return sOffsetField;
}

Word ATSSimTableAtsWatchdog::getPauseResumeFieldNumber()
{
    return sPauseResumeField;
}

const Word ATSSimTableAtsWatchdog::sValueField = 0;
const Word ATSSimTableAtsWatchdog::sOffsetField = 2;
const Word ATSSimTableAtsWatchdog::sPauseResumeField = 4;
