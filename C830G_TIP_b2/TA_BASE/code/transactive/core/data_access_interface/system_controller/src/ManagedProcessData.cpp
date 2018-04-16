/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/ManagedProcessData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#include "ManagedProcessData.h"
#include "ManagedProcessHelper.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    ManagedProcessData::ManagedProcessData(unsigned long systemControllerKey,
                                           unsigned long managedProcessKey)
    {
        m_helper = new ManagedProcessHelper(systemControllerKey, managedProcessKey);
    }

    ManagedProcessData::~ManagedProcessData()
    {
        delete m_helper;
        m_helper = NULL;
    }

    unsigned long ManagedProcessData::getSystemControllerKey()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getSystemControllerKey();
    }

    unsigned long ManagedProcessData::getManagedProcessKey()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getManagedProcessKey();
    }

    EOperationMode ManagedProcessData::getDefaultOperatingMode()
    {
        TA_ASSERT(m_helper != NULL, "The ManagedProcessHelper pointer is null.");
        return m_helper->getDefaultOperatingMode();
    }

} // namespace TA_Base_Core
