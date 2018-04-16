#if !defined(CONFIG_UPDATE_BROADCASTER_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define CONFIG_UPDATE_BROADCASTER_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This class broadcasts configuration update messages that would normally only be sent from the Configuration
  * Editor, but needs to be broadcasted from the Video Switch Agent whenever it changes online updateable
  * entity configuration. For example, when the description of a Sequence is edited.
  * This class is a singleton.
  *
  */

#include "core/message/types/MessageTypes.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"
#include <boost/noncopyable.hpp>
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <vector>
#include <string>

namespace TA_Base_Core
{
    class ConfigUpdateMessageSender;
}

namespace TA_IRS_App
{
    class ConfigUpdateBroadcaster
    {
			friend class ACE_Singleton<ConfigUpdateBroadcaster,ACE_Recursive_Thread_Mutex>; 
        public:
            static ConfigUpdateBroadcaster& getInstance();
            void notifyEntityDescriptionUpdated(unsigned long entityKey);
        private:
            virtual ~ConfigUpdateBroadcaster();
            ConfigUpdateBroadcaster();
            // This is used to send configuration update messages indicating changes to the configuration data.
            TA_Base_Core::ConfigUpdateMessageSender* m_configSender;
            static const std::string DESCRIPTION_PARAM_NAME;
    };
}

#endif
