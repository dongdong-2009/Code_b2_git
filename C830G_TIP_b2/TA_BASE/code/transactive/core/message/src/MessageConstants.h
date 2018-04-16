#ifndef MESSAGECONSTANTS_H
#define MESSAGECONSTANTS_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Jade Welton
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
