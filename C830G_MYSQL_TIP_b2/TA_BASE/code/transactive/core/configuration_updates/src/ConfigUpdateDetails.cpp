/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/configuration_updates/src/ConfigUpdateDetails.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds all the details of a configuration update ready for processing.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include "core/configuration_updates/src/ConfigUpdateDetails.h"

namespace TA_Base_Core
{
    ConfigUpdateDetails::ConfigUpdateDetails(const TA_Base_Core::ConfigUpdateMessageCorbaDef& updateDetails, const EObjectType type) :
        m_modification(updateDetails.modificationType),
        m_key(updateDetails.assocEntityKey),
        m_type(type)
    {

        unsigned int size = updateDetails.changes.length();
        for (unsigned int i = 0; i < size; ++i)
        {
            std::string element( updateDetails.changes[i] );
            m_changedParams.push_back( element );
        }
    }
}
