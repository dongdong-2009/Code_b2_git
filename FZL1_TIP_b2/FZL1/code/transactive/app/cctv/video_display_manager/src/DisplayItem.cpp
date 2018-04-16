/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/DisplayItem.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is a generic display item.
  * Display items can be inputs or outputs but this is the
  * common part of those.
  *
  * This is used to locally store state and association information
  * so it can easily be retrieved for displaying in the GUI.
  */


#include "DisplayItem.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include <sstream>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{


    DisplayItem::DisplayItem(unsigned long entityKey,
                             const std::string& name,
                             const std::string& description)
        : m_entityKey(entityKey),
          m_name(name),
          m_description(description)
    {
        FUNCTION_ENTRY("DisplayItem");
        FUNCTION_EXIT;
    }

   DisplayItem::~DisplayItem()
   {
   }

    unsigned long DisplayItem::getKey()
    {
        FUNCTION_ENTRY("getKey");
        FUNCTION_EXIT;
        return m_entityKey;
    }


    std::string DisplayItem::getName()
    {
        FUNCTION_ENTRY("getName");
        FUNCTION_EXIT;
        return m_name;
    }


    std::string DisplayItem::getDescription()
    {
        FUNCTION_ENTRY("getDescription");
        ThreadGuard guard(m_lock);
        FUNCTION_EXIT;
        return m_description;
    }


    void DisplayItem::setDescription(const std::string& newDescription)
    {
        FUNCTION_ENTRY("setDescription");
        ThreadGuard guard(m_lock);
        m_description = newDescription;
        FUNCTION_EXIT;
    }


    std::string DisplayItem::getDetails()
    {
        FUNCTION_ENTRY("getDetails");
        ThreadGuard guard(m_lock);
        std::ostringstream detailsStr;
        detailsStr << m_name << " - " << m_description;
        FUNCTION_EXIT;
        return detailsStr.str();
    }
} // TA_IRS_App


