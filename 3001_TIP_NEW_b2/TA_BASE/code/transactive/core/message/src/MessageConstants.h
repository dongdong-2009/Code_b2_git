#ifndef MESSAGECONSTANTS_H
#define MESSAGECONSTANTS_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/message/src/MessageConstants.h $
  * @author:  Jade Welton
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/12/17 14:00:24 $
  * Last modified by:  $Author: shurui.liu $
  * 
  * Common constant values for the Messages subsystem
  */

namespace TA_Base_Core
{
    //
    // Names used for Filterable Body Fields
    //
    const char* const ENTITYKEY_FILTER_NAME       = "EntityKey";
    const char* const ASSOC_ENTITYKEY_FILTER_NAME = "AssocEntityKey";
    const char* const SUBSYSTEM_FILTER_NAME       = "Subsystem";
    const char* const REGION_FILTER_NAME          = "Region";
    const char* const AGENTNAME_FILTER_NAME       = "AgentName";

    const char* const MESSAGETYPE_FILTER_NAME     = "MessageType";
}

#endif // MESSAGECONSTANTS_H
