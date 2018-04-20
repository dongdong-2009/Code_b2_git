/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainAgentProxyFactory_H)
#define TrainAgentProxyFactory_H

#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainStateUpdateCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainPecCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainPaCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainRadioGroupCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainTisCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include <ace/Singleton.h>

namespace TA_IRS_Bus
{

    /**
     * This simply creates proxies for the appropriate train agent.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:51 PM
     */

    class TrainAgentProxyFactory
    {

        friend class ACE_Singleton< TrainAgentProxyFactory, ACE_Recursive_Thread_Mutex >;

    public:

        /**
         * Gets an instance of the singleton.
         */
        static TrainAgentProxyFactory& getInstance();


        /**
         * This will initialise the internal data so that proxies can be created.
         *
         * If this is called more than once it will assert, it should be called in the
         * initialisation of any agent using trainborne functions.
         *
         * @param agentEntityData    This is the entity data of the agent using the train
         * proxies
         */
        void agentInitialise( TA_Base_Core::IEntityDataPtr agentEntityData );


        /**
         * This will initialise the internal data so that proxies can be created.
         *
         * If this is called more than once it will assert, it should be called in the
         * initialisation of any GUI using trainborne functions.
         *
         * @param operatorSession    This is the operators session ID
         */
        void guiInitialise( const std::string& operatorSession );


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainInformationCorbaProxyPtr createTrainInformationCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainOperationModeCorbaProxyPtr createTrainOperationModeCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainRadioGroupCorbaProxyPtr createTrainRadioGroupCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainCctvCorbaProxyPtr createTrainCctvCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainDriverCallCorbaProxyPtr createTrainDriverCallCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainPaCorbaProxyPtr createTrainPaCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainPecCorbaProxyPtr createTrainPecCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainTisCorbaProxyPtr createTrainTisCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the local agent.
         */
        TrainStateUpdateCorbaProxyPtr createTrainStateUpdateCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Note that this proxy cannot be used to subscribe to state update messages.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the OCC agent.
         */
        TrainStateUpdateCorbaProxyPtr createOccTrainStateUpdateCorbaProxy();


        /**
         * This will create and return a new proxy object.
         *
         * Note that this proxy cannot be used to subscribe to state update messages.
         *
         * Either the agentInitialise or guiInitialise methods must have been called
         * previously (as part of application startup) or this will ASSERT.
         *
         * @return a new proxy to the depot agent.
         */
        TrainStateUpdateCorbaProxyPtr createDepotTrainStateUpdateCorbaProxy();

    private:

        
        /** 
         * Constructor
         */
        TrainAgentProxyFactory();


        /** 
         * Destructor
         */
        ~TrainAgentProxyFactory();


        /**
         * This will load the local train agent entity information, and store it in the
         * members if it is not already loaded
         */
        void loadLocalTrainAgentEntityInfo();


        /**
         * This will load the local train agent entity information, and store it in the
         * members if it is not already loaded
         */
        void loadOtherTrainAgentEntityInfo();


        /**
         * Indicates if it is initialized
         */
        bool m_initialised;


        /**
         * This is the entity name of the local train agent.
         */
        std::string m_localTrainAgentEntityName;


        /**
         * This is the entity key of the local train agent, used to subscribe to comms
         * messages and state update messages.
         */
        unsigned long m_localTrainAgentEntityKey;


        /**
         * This is the location key of the local train agent entity
         */
        unsigned long m_localTrainAgentLocationKey;


        /**
         * This is the entity name of the OCC train agent.
         */
        std::string m_occTrainAgentEntityName;


        /**
         * This is the entity name of the Depot train agent.
         */
        std::string m_depotTrainAgentEntityName;


        /**
         * Lock for loading and accessing the local entity info
         */
        TA_Base_Core::NonReEntrantThreadLockable m_localTrainAgentEntityLock;


        /**
         * Lock for loading and accessing the OCC/Depot entity info
         */
        TA_Base_Core::NonReEntrantThreadLockable m_otherTrainAgentEntityLock;


        /**
         * Cached proxy object.
         */
        TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainOperationModeCorbaProxyPtr m_trainOperationModeCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainStateUpdateCorbaProxyPtr m_localTrainStateUpdateCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainStateUpdateCorbaProxyPtr m_occTrainStateUpdateCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainStateUpdateCorbaProxyPtr m_depotTrainStateUpdateCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainRadioGroupCorbaProxyPtr m_trainRadioGroupCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainCctvCorbaProxyPtr m_trainCctvCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainPaCorbaProxyPtr m_trainPaCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainPecCorbaProxyPtr m_trainPecCorbaProxy;


        /**
         * Cached proxy object.
         */
        TrainTisCorbaProxyPtr m_trainTisCorbaProxy;

    };

    /**
     * Singleton instance of the TrainAgentProxyFactory. Let ACE take care of
     * singleton management and cleanup.
     * @author adamr
     * @version 1.0
     * @created 22-Oct-2007 4:46:52 PM
     */
    typedef ACE_Singleton< TrainAgentProxyFactory, ACE_Recursive_Thread_Mutex > TrainAgentProxyFactorySingleton;

}

#endif // !defined(TrainAgentProxyFactory_H)
