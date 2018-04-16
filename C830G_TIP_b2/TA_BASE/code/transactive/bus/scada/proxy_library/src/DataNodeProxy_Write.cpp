/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/proxy_library/src/DataNodeProxy_Write.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides access to a piece of equipment and its associated data points.
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <string>
#include <limits>

#include "bus/scada/proxy_library/src/DataNodeProxy.h"
#include "bus/scada/proxy_library/src/DataNodeLocalAccess.h"
#include "bus/scada/proxy_library/src/DataNodeRemoteAccess.h"

#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Bus
{

	void DataNodeProxy::setTagState(const ETagState tagState, 
									const EOutputInhibitState outputInhibitState, 
									const EInputInhibitState inputInhibitState, 
									const std::string& ptwId, 
									const std::string& tagComment,
									const std::string& sessionId)
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "setTagState() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            TA_THROW(TA_Base_Core::ScadaProxyException (text));
        }
        
        getObjAccess()->changeDataNodeTag(tagState, 
            outputInhibitState, 
            inputInhibitState, 
            ptwId, 
            tagComment,
            sessionId);

	}

	void DataNodeProxy::setInhibitMms(bool inhibitMms, const std::string& sessionId)
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "setInhibitMms() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            TA_THROW(TA_Base_Core::ScadaProxyException (text));
        }
        
		getObjAccess()->setInhibitMms(sessionId, inhibitMms);
	}
	
	void DataNodeProxy::setNotes(const std::string& notes, std::string& sessionId)
	{
        if(!m_proxyData.isSynchronized)
        {
            std::string text = "Info: Agent is not connected yet for DataNode " + m_entityData->getName() + ".";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "setNotes() - %s - %s", m_entityData->getName().c_str(), text.c_str());
            
            TA_THROW(TA_Base_Core::ScadaProxyException (text));
        }
        
		getObjAccess()->setDataNodeNotes(sessionId, notes);
	}

}
