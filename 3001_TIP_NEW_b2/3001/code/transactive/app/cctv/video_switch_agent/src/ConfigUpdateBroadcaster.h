#if !defined(CONFIG_UPDATE_BROADCASTER_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
#define CONFIG_UPDATE_BROADCASTER_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/ConfigUpdateBroadcaster.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This class broadcasts configuration update messages that would normally only be sent from the Configuration
  * Editor, but needs to be broadcasted from the Video Switch Agent whenever it changes online updateable
  * entity configuration. For example, when the description of a Sequence is edited.
  * This class is a singleton.
  *
  */

#include <vector>
#include <string>

#include "core/message/types/MessageTypes.h"
#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{
    class ConfigUpdateMessageSender;
    class ReEntrantThreadLockable;
}


namespace TA_IRS_App
{
    class ConfigUpdateBroadcaster
    {

    public:

 		/**
          * getInstance
          *
          * Gets the one and only ConfigUpdateBroadcaster.
          *
          * @return     ConfigUpdateBroadcaster&
          *             The only StateUpdateBroadcaster.
          */
		static ConfigUpdateBroadcaster& getInstance();


        /** 
          * notifyEntityDescriptionUpdated
          *
          * Sends appropriate ConfigUpdate message to inform both GUI's and the
          * 
          *
          * @param 
          */
        void notifyEntityDescriptionUpdated( unsigned long entityKey ); 

        
        /**
         * Destructor
         */
        virtual ~ConfigUpdateBroadcaster();

    private:

        /**
         * Constructor
         *
         * This class is a singleton, so use getInstance() to get the one and only 
         * instance of this class.
         */
        ConfigUpdateBroadcaster();


        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ConfigUpdateBroadcaster( const ConfigUpdateBroadcaster& theConfigUpdateBroadcaster){ };


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ConfigUpdateBroadcaster& operator=(const ConfigUpdateBroadcaster& pm);

        // This is used to send configuration update messages indicating changes to the configuration data.
        TA_Base_Core::ConfigUpdateMessageSender* m_configSender;

		static ConfigUpdateBroadcaster* m_theClassInstance; // The one and only instance of this class.
		
		static TA_Base_Core::ReEntrantThreadLockable m_singletonLock; // Protect singleton creation

        static const std::string DESCRIPTION_PARAM_NAME;
    };
}

#endif // !defined(CONFIG_UPDATE_BROADCASTER_40D09158_85E4_464f_81D1_E7B9225238D9__INCLUDED_)
