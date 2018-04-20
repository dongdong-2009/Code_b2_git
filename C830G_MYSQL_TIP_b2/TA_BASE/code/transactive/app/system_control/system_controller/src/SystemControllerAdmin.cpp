/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Implementation of the ISystemControllerAdminCorbaDef
  * interface, used by the System Manager.
  *
  */

#include "SystemControllerAdmin.h"
#include "ProcessManager.h"
#include "SystemController.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#include <vector>

namespace TA_Base_App
{
    //
    // Constructor
    //
    SystemControllerAdmin::SystemControllerAdmin(SystemController* systemController)
    {
		m_systemController = systemController;
        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerAdmin");
        activateServant();
    }
    
    //
    // Destructor
    //
    SystemControllerAdmin::~SystemControllerAdmin()
    {
    }
    
    //
    // getProcessData
    //
    TA_Base_Core::ProcessDataSeq* SystemControllerAdmin::getProcessData()
    {
        FUNCTION_ENTRY( "getProcessData" );

        // Get the process data from ProcessManager
        TA_Base_Core::ProcessDataSeq* dataSeq = new TA_Base_Core::ProcessDataSeq();

        TA_ASSERT( dataSeq != NULL, "New failed to allocate ProcessDataSeq" );

        EntityToProcessMap& processes = ProcessManager::getInstance().getProcesses();

        dataSeq->length( processes.size() );

        size_t i = 0;

        for ( EntityToProcessMap::iterator iter = processes.begin(); iter != processes.end(); ++iter, ++i )
        {
            iter->second->getProcessData( (*dataSeq)[ i ], false );
        }

        FUNCTION_EXIT;
        return dataSeq;
    }


    //
    // shutdown
    //
    void SystemControllerAdmin::shutdown(const char* hostname)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to shutdown");

        if (hostname == ProcessManager::getInstance().getHostName())
        {
            m_systemController->shutDownRequested();
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                "Instructed by System Manager to shutdown a different System Controller (%s)!",
                hostname);
        }
    }

    //
    // startProcess
    //
    void SystemControllerAdmin::startProcess(const char* entityName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to start %s", entityName);

        ProcessManager::getInstance().startProcess(entityName);
    }

    //
    // changeAgentOperationMode
    //
    void SystemControllerAdmin::changeAgentOperationMode(const char* entityName,
                                                TA_Base_Core::EOperationMode operationMode)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to change operation mode of %s to %d",
            entityName, operationMode);

        try
        {
            ProcessManager::getInstance().changeAgentOperationMode(entityName, operationMode);
        }
        catch(TA_Base_Core::SystemControllerException& sce)
        {
            // Operation mode cannot be changed. Notify the System Manager.
            LOG( SourceInfo, DebugUtil::ExceptionConstruction, "ISystemControllerAdminCorbaDef::ModeChangeException", "Operation mode of agent could be changed.");
            TA_Base_Core::ISystemControllerAdminCorbaDef::ModeChangeException mce;
            mce.reason = sce.what();
            throw mce; 
        }
    }

    //
    // stopProcess
    //
    void SystemControllerAdmin::stopProcess(const char* entityName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to stop %s", entityName);

        ProcessManager::getInstance().stopProcess(entityName);
    }

    //
    // setRunParam
    //
    void SystemControllerAdmin::setRunParam(const char* entityName, const TA_Base_Core::RunParam& param)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to set run param on %s", entityName);
        ProcessManager::getInstance().setRunParam(entityName, param);
    }

    //
	// setAgentsStatus
	//
	TA_Base_Core::AgentStatusEnum SystemControllerAdmin::getAgentsStatus()
	{
		return (ProcessManager::getInstance().getAgentStatusSummary());
	}

    // limin++ CL-21243
    void SystemControllerAdmin::deployProcesses( TA_Base_Core::EOperationMode normalOperationMode, CORBA::Boolean isStart )
    {
        ProcessManager::getInstance().deployProcesses( normalOperationMode, isStart );
    }
    // ++limin CL-21243

} // namespace TA_Base_App
