/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.cpp $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This class broadcasts configuration update messages that would normally only be sent from the Confi
  * Editor, but needs to be broadcasted from the Video Switch Agent whenever it changes online updateable
  * entity configuration. For example, when the description of a Sequence is edited.
  * This class is a singleton.
  *
  */

#pragma warning(disable:4786)

#include "app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/types/ConfigUpdate_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/synchronisation/src/ThreadGuard.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    //
    // Initialise statics.
    //
    const std::string ConfigUpdateBroadcaster::DESCRIPTION_PARAM_NAME = "Description";


    //
    // getInstance
    //
    ConfigUpdateBroadcaster& ConfigUpdateBroadcaster::getInstance()
    {
		return *(ACE_Singleton<ConfigUpdateBroadcaster,ACE_Recursive_Thread_Mutex>::instance());
    }


    //
    // notifyEntityDescriptionUpdated
    //
    void ConfigUpdateBroadcaster::notifyEntityDescriptionUpdated(unsigned long entityKey)
    {
        FUNCTION_ENTRY("notifyEntityDescriptionUpdated");
        TA_ASSERT(m_configSender != NULL, "The config online update message sender cannot be NULL");
        m_configSender->sendConfigUpdateMessage(TA_Base_Core::ConfigUpdate::ConfigEntityOwner,
                                                entityKey,
                                                TA_Base_Core::Update,
                                                std::vector< std::string >(1, DESCRIPTION_PARAM_NAME));
        FUNCTION_EXIT;
    }


    //
    // Constructor
    //
    ConfigUpdateBroadcaster::ConfigUpdateBroadcaster() : m_configSender(NULL)
    {
        FUNCTION_ENTRY("Constructor");
        // We haven't set up the message sender yet and we need to.
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Setting up the config update message sender for the first time.");
        m_configSender = TA_Base_Core::MessagePublicationManager::getInstance().getConfigUpdateMessageSender(TA_Base_Core::ConfigUpdate::Context);
        TA_ASSERT(m_configSender != NULL, "The config update message sender received is NULL");
        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    ConfigUpdateBroadcaster::~ConfigUpdateBroadcaster()
    {
        FUNCTION_ENTRY("Destructor");
		delete m_configSender;
		m_configSender = NULL;
        FUNCTION_EXIT;
    }

}
