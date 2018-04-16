/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/ScadaHistorianAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include <string>
#include <map>
#include "app/history/scada_historian/src/ScadaHistorianAgent.h"
#include "app/history/scada_historian/src/HistoryConfig.h"
#include "app/history/scada_historian/src/ScadaHistorianAgentProcess.h"
#include "app/history/scada_historian/src/SampleConfig.h"
#include "app/history/scada_historian/src/AverageConfig.h"
#include "app/history/scada_historian/src/MaximumConfig.h"
#include "app/history/scada_historian/src/MinimumConfig.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h"

using TA_Bus::GenericAgent;
using TA_Core::IEntityData;
using TA_Core::ScadaHistoryConfigData;
using TA_Core::ConfigUpdateDetails;
using TA_Core::StateUpdateMessageCorbaDef;
using TA_Core::DebugUtil;
using TA_Core::RunParams;


namespace TA_App
{
    //
    // Constructor
    //
    ScadaHistorianAgent::ScadaHistorianAgent(int argc, char* argv[])
        : m_genericAgent(NULL), 
          m_scadaHistorianAgentProcess(NULL)
    {
		LOG ( SourceInfo, DebugUtil::FunctionEntry, "ScadaHistorianAgent");

        try
        {
            m_genericAgent = new GenericAgent(argc, argv, this);
            m_scadaHistorianAgentProcess = new ScadaHistorianAgentProcess();

            if(RunParams::getInstance().get(RPARAM_OPERATIONMODE) == RPARAM_CONTROL)
            {
                agentSetControl();
            }
            else
            {
                agentSetMonitor();
            }
        }
        catch(...)
        {
            if(m_genericAgent != NULL)
            {
                delete m_genericAgent;
                m_genericAgent = NULL;
            }

            if(m_scadaHistorianAgentProcess != NULL)
            {
                delete m_scadaHistorianAgentProcess;
                m_scadaHistorianAgentProcess = NULL;
            }
        }

		LOG ( SourceInfo, DebugUtil::FunctionExit, "ScadaHistorianAgent");
    }


    //
    // Destructor
    //
    ScadaHistorianAgent::~ScadaHistorianAgent()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "~ScadaHistorianAgent");

        try
        {
            if( m_genericAgent != NULL )
            {
                m_genericAgent->deactivateServant();
                delete m_genericAgent;
                m_genericAgent = NULL;
            }
        }
        catch(...)
        {
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "~ScadaHistorianAgent");
    }


    //
    // agentTerminate
    //
    void ScadaHistorianAgent::agentTerminate()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "agentTerminate");
        
        if(m_scadaHistorianAgentProcess != NULL)
        {
            m_scadaHistorianAgentProcess->terminateAndWait();
            //m_scadaHistorianAgentProcess->deactivateAndDeleteServant();
            delete m_scadaHistorianAgentProcess;
            m_scadaHistorianAgentProcess = NULL;
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "agentTerminate");
    }


    //
    // createEntity
    //
    TA_Bus::IEntity* ScadaHistorianAgent::createEntity(TA_Core::IEntityData& EntityData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "createEntity");

        std::string type = EntityData.getType();
        
        try
        {
		    std::string alarmInfo("");
		    std::string errorMessage("");

            if(type.compare("HistoryConfig") == 0)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Type is History Config");

                ScadaHistoryConfigData& historyEntityData = dynamic_cast <ScadaHistoryConfigData&> (EntityData);
                std::string dataType = historyEntityData.getDataType();

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Got data type");

                HistoryConfig* histCfg = NULL;

#if defined(WIN32)
                if(_strnicmp(dataType.c_str(), "sample", dataType.size()) == 0)
#else
                if(strcasecmp(dataType.c_str(), "sample") == 0)
#endif
                {
                    histCfg = new SampleConfig(EntityData);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Created SampleConfig");
                }
#if defined(WIN32)
	            else if(_strnicmp(dataType.c_str(), "average", dataType.size()) == 0)
#else
                else if(strcasecmp(dataType.c_str(), "average") == 0)
#endif
                {
                    histCfg = new AverageConfig(EntityData);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Created AverageConfig");
                }
#if defined(WIN32)
	            else if(_strnicmp(dataType.c_str(), "minimum", dataType.size()) == 0)
#else
                else if(strcasecmp(dataType.c_str(), "minimum") == 0)
#endif    
                {
                    histCfg = new MinimumConfig(EntityData);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Created MinimumConfig");
                }
#if defined(WIN32)
	            else if(_strnicmp(dataType.c_str(), "maximum", dataType.size()) == 0)
#else
                else if(strcasecmp(dataType.c_str(), "maximum") == 0)
#endif
                {
                    histCfg = new MaximumConfig(EntityData);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Created MaximumConfig");
                }
		        else
		        {
                    TA_ASSERT(false, "Invalid data type");
		        }
                
			    if ( histCfg != NULL )
			    {
				    //
				    // If these objects are deleted via the GenericDriverProcess
				    // OnTerminate() function then Corba will crash upon shutdown.
				    // 
                    
            	    LOG(SourceInfo, DebugUtil::FunctionExit, "createEntity - Added IEntity"); 
				    return histCfg;
			    }
            }
		    else
		    {
			    // Entity creation failed because entity is not
			    // one of the predefined types.

			    TA_ASSERT(false, "Invalid data type");
		    }
        }
        catch(HistoryConfig::HistorianCfgErrorException &ex)
        {
            // The entity configuration data is incomplete, so 
            // the entity is not created.  Generate an alarm to notify the
            // operators that the configuration data is incomplete.

			// TODO: This currently doesn't do anything
            ex.GenerateAlarm();

		    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Exception caught while creating HistoryConfig");
        }
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
			    "Unknown exception caught when creating Historian entities.");
        }
        
        LOG(SourceInfo, DebugUtil::FunctionExit, "createEntity");

        // Failed to create an entity with the given configuration.
	    return NULL;
    }


    //
    // agentSetMonitor
    //
	void ScadaHistorianAgent::agentSetMonitor()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "agentSetMonitor");

        m_scadaHistorianAgentProcess->setToControl(false);

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm, "Operation Mode: MONITOR");
    }


    //
    // agentSetControl
    //
	void ScadaHistorianAgent::agentSetControl()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "agentSetControl");

        m_scadaHistorianAgentProcess->setToControl(true);

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm, "Operation Mode: CONTROL");
    }


    //
    // registerForStateUpdates
    //
	void ScadaHistorianAgent::registerForStateUpdates()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "registerForStateUpdates" );

        LOG ( SourceInfo, DebugUtil::FunctionExit, "registerForStateUpdates" );
    }


    //
    // receiveSpecialisedMessage
    //
    void ScadaHistorianAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
    {
    }

    
    //
    // processOnlineUpdate
    //
    void ScadaHistorianAgent::processOnlineUpdate(const ConfigUpdateDetails& updateEvent)
    {
    }


    //
    // run
    //
    void ScadaHistorianAgent::run()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "run");
        
        const std::map<unsigned long, TA_Bus::IEntity*>* entityMap; 
        entityMap = m_genericAgent->getEntityMap();

        std::map<unsigned long, TA_Bus::IEntity*>::const_iterator it;
		it = entityMap->begin();

		//
		// Pass pointers to the History Configs to the agent process so that
		// it can access them later
		//
        while(it != entityMap->end())
        {
            m_scadaHistorianAgentProcess->addHistoryConfig(dynamic_cast <HistoryConfig*> (it->second));
            ++it;
        }

        m_scadaHistorianAgentProcess->start();
        m_genericAgent->run();
    }

    
    //
    // notifyGroupOffline
    //
    void ScadaHistorianAgent::notifyGroupOffline( const std::string& group )
    {
    }

	
    //
    // notifyGroupOnline
    //
    void ScadaHistorianAgent::notifyGroupOnline( const std::string& group )
    {
    }
}
