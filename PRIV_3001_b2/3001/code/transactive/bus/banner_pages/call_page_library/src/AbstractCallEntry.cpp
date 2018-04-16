/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntry.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class implements the ICallEntry interface.
  * It is provided as a convenience class so that subclasses of ICallEntry do
  * not have to implement its accessors and mutators.
  * 
  */


// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // _MSC_VER


#include <sstream>
#include <time.h>

#include "core/utilities/src/DebugUtil.h"
#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"

#include "ace/OS.h"

#include "core/utilities/src/DateFormat.h" // TD12474 

using TA_Base_Core::DebugUtil;


namespace TA_IRS_Bus
{
    AbstractCallEntry::~AbstractCallEntry()
    {
    }


    AbstractCallEntry::AbstractCallEntry(
        unsigned long id,
        const std::string& source,
        const std::string& information)
        :
        m_id(id),
        m_source(source),
        m_information(information)
    {
        std::ostringstream message;
        message <<
            "Abstract Call Entry created with " <<
            "source '" << source << "' " <<
            "and information '" << information << "'";
        
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s", message.str().c_str () );
    }


    void AbstractCallEntry::setSource(const std::string& source)
    {
        std::string message = "Source set to '";
        message            += source;
        message            += "'";
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"%s", message.c_str() );

        m_source = source;
    }


    void AbstractCallEntry::setInformation(const std::string& information)
    {
        std::string message = "Information set to '";
        message            += information;
        message            += "'";
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"%s", message.c_str() );

        m_information = information;
    }


    std::string AbstractCallEntry::getSource() const
    {
        return m_source;
    }


    std::string AbstractCallEntry::getInformation() const
    {
        return m_information;
    }


    unsigned long AbstractCallEntry::getId() const
    {
        return m_id;
    }


    std::string AbstractCallEntry::getTimeStr() const
    {
        time_t createTime = getCreateTime();
        
        // Want format "12-JUN-04 02:02:02" - note uppercase..
        enum { BUF_MAX = 256 };
        static char buf[BUF_MAX];
        //const std::string format("%d-%b-%y %H:%M:%S"); // TD12474 

        struct tm timeStruct;
        time_t date = time( NULL );
        
       
        if (0 != ACE_OS::localtime_r(&createTime, &timeStruct))
        {
			// TD12474 ++
            //strftime(buf, BUF_MAX, format.c_str(), &timeStruct);
			TA_Base_Core::DateFormat dateFormat; 
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);
			strftime(buf, BUF_MAX, dateFormatStr.c_str(), &timeStruct);
			// ++ TD12474 
        }
        else
        {
            return "??";
        }    

        // Make uppercase as required
        return _strupr(buf);
    }
}// TA_IRS_Bus
