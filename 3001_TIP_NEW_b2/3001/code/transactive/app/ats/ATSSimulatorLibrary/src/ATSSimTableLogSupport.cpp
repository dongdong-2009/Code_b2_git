/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableLogSupport.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * \class ATSSimTableLogSupport
  * Implementation file for the ATSSimTableLogSupport class.
  *
  * ATSSimTableLogSupport supports the
  * to return an instance of the ATSSimulator class.
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include <list>
#include <string>

#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableLogSupport.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableLogData.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTable.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableQueryProcessor.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

#include "core/utilities/src/DebugUtil.h"

// using namespace TA_IRS_App;
using namespace TA_IRS_App::ATS_Sim;

/* ----------------------------------------------------------------------------
 * Construction/Destruction
 * --------------------------------------------------------------------------*/
ATSSimTableLogSupport::ATSSimTableLogSupport(ATSSimTableQueryProcessor * parent)
: m_parent(parent)
, m_outputFormat(parent->getTableOutputFormat())
{

}

ATSSimTableLogSupport::~ATSSimTableLogSupport()
{
}

UserResponse * ATSSimTableLogSupport::processLogQuery( UserQuery * query )
{
    UserResponse * response = new UserResponse(query);

    unsigned short tableAddress = query->getTableAddress();
    char tAddrStr_c[12];
    sprintf(tAddrStr_c,"%d",query->getTableAddress());
    std::string tAddrStr = tAddrStr_c;

    if (query->getType() == LogStart)
    {
        LogDataMap::iterator found = m_logDataMap.find(tableAddress);

        if ( found == m_logDataMap.end() )
        {
            ATSSimTable * t = m_parent->getTable(tableAddress);
            if ( t != 0 )
            {
                ATSSimTableLogData * logData = new ATSSimTableLogData(query, this, t);
				m_logDataMap[tableAddress] = logData;
                response->m_responseData+= "Logging successfully activated for table ";
            }
            else
            {
                response->m_responseData+= "Logging Error: could not find table: ";
            }
            response->m_responseData+= tAddrStr;
        }
        else // LogData already exists. Just add the records
        {
            ((*found).second)->addRecordRange(query->getRecords());
            response->m_responseData+= "Records successfully added to logging for table ";
            response->m_responseData+= tAddrStr;
        }

    }

    else if (query->getType() == LogStop)
    {
        LogDataMap::iterator fnd = m_logDataMap.find(query->getTableAddress());
        if (fnd != m_logDataMap.end())
        {
            ATSSimTableLogData * logData = (*fnd).second;

            if (!(query->getRecords().empty()))
            {
                // 
                // If there are any records, they will specify
                // the record numbers to stop logging
                logData->removeRecordRange(query->getRecords());

                response->m_responseData = "Records successfully removed from logging for table ";
                response->m_responseData+= tAddrStr;

                //
                // once all the records are gone, the query is interpreted as
                // "show ALL records".  This is probably not intended, so we
                // catch that and remove the entire log request
                if ( (logData->getRecordRange()).empty() )
                {
                    response->m_responseData+= "All records removed, stopping logging for table ";
                    response->m_responseData+= tAddrStr;
                    m_logDataMap.erase(query->getTableAddress());
                }
            }
            else
            {
                //
                // if there are NO paramters, we just remove that entire
                // table from logging. We could use the "fnd" iterator that
                // we already found, but i've experienced grief with that
                // overloaded version of erase(). For no apparent reason.
                m_logDataMap.erase(query->getTableAddress());

                response->m_responseData = "Table logging successfully stopped for table ";
                response->m_responseData+= tAddrStr;
            }
        }
        else
        {
            response->m_responseData = "WARNING: Table ";
            response->m_responseData+= tAddrStr;
            response->m_responseData+= " not logged. Request ignored.";
        }
    }

    else if (query->getType() == LogMode)
    {
        ATSSimTableLogData * logData = 0;
        LogDataMap::iterator fnd = m_logDataMap.find(query->getTableAddress());
        if (fnd != m_logDataMap.end())
        {
            logData = (*fnd).second;
        }

        TableOutputFormat newFormat = query->getOutputFormat();

        if (logData != 0)
        {
            //
            // as agreed with ../cmd/LogModeCommand.cpp the presence of a
            // parameter indicates that the format is to be set, otherwise
            // the current format is output as the response.
            if ( query->moreArgs() )
            {
                logData->setOutputFormat(newFormat);

                response->m_responseData = "Log output format for table ";
                response->m_responseData+= tAddrStr;
                response->m_responseData+= " set to ";
                response->m_responseData+= ATSSimUtility::outputFormat2String(newFormat);
            }
            else
            {
                response->m_responseData = "Log output format for table ";
                response->m_responseData+= tAddrStr;
                response->m_responseData+= " is currently ";
                response->m_responseData+= ATSSimUtility::outputFormat2String(logData->getOutputFormat());
            }
        }
        else if (query->getTableAddress() == TableAddressRegister::NOTINUSE_ADDRESS)
        {
            if ( query->moreArgs() )
            {
                m_outputFormat = newFormat;

                response->m_responseData = "Default log output format set to: ";
                response->m_responseData+= ATSSimUtility::outputFormat2String(m_outputFormat);
            }
            else
            {
                response->m_responseData = "Default log output format is currently: ";
                response->m_responseData+= ATSSimUtility::outputFormat2String(m_outputFormat);
            }
        }
        else if (query->getTableAddress() == TableAddressRegister::INVALID_ADDRESS)
        {
            response->m_responseData = "Default log output format is: ";
            response->m_responseData+= ATSSimUtility::outputFormat2String(m_outputFormat);
        }
        else
        {
            response->m_responseData = "Table ";
            response->m_responseData+= tAddrStr;
            response->m_responseData+= " is not being logged.  Ignoring request";
        }
    }
    else
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Invalid User Log Request parameter for table %d: must specify either 'start', 'stop' or 'mode'",query->getTableAddress());
        std::cout << "Invalid User Log Request parameter for table " << query->getTableAddress() << ": must specify either 'start', 'stop' or 'mode'" << std::endl;
    }


    return response;
}

void ATSSimTableLogSupport::doLogTable( ATSSimTable * t )
{
    LogDataMap::iterator fnd = m_logDataMap.find(t->getTableAddress());
    if (fnd != m_logDataMap.end())
    {
        ATSSimTableLogData * logData = (*fnd).second;
        logData->doLog();
    }
}
