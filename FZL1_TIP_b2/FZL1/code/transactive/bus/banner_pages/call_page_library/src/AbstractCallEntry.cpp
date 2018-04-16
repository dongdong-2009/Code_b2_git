/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntry.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
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
#include "core/utilities/src/DateFormat.h" // TD12474

#include "ace/OS.h"


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
        m_information(information),
		m_isSoundClosed(false)
    {
        std::ostringstream message;
        message <<
            "Abstract Call Entry created with " <<
            "source '" << source << "' " <<
            "and information '" << information << "'";
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, message.str().c_str());
    }


    void AbstractCallEntry::setSource(const std::string& source)
    {
        std::string message = "Source set to '";
        message            += source;
        message            += "'";
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, message.c_str());

        m_source = source;
    }


    void AbstractCallEntry::setInformation(const std::string& information)
    {
        std::string message = "Information set to '";
        message            += information;
        message            += "'";
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, message.c_str());

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
        // Get the create time, and covert it to a struct tm
        time_t createTime = getCreateTime();

        tm ptm;
        if(ACE_OS::localtime_r(&createTime, &ptm) == NULL)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Received an invalid timestamp");
			return "";
        }
		// TD16151
		COleDateTime dateModified( createTime );
		
		TA_Base_Core::DateFormat dateFormat;
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);
		std::string timeStr = dateModified.Format(dateFormatStr.c_str());
		// TD16151

        return timeStr;
    }


    std::vector<std::string> AbstractCallEntry::getContextMenuItems()
	{
		// Stub implementation for Call entries not using this feature
		std::vector<std::string> emptylist;
        return emptylist;
    }


	void AbstractCallEntry::onContextMenuCommand(std::string command)
	{
		// Stub implementation for Call entries not using this feature
		return;
	}


    bool AbstractCallEntry::isEmergency() const
    {
		return false;
    }

}// TA_IRS_Bus
