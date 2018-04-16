/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_manager/src/AdminManager.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/08/01 10:17:43 $
  * Last modified by:  $Author: weikun.lin $
  *
  * AdminManager is a singleton class that is the central class of the 
  * System Manager. It retreives data from a System Controller and
  * sends the GUI's orders to that System Controller.
  */

#pragma warning(disable:4786)

#include "stdafx.h"
#include <winsock2.h> 

#include "SystemManager.h"
#include "AdminManager.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IProcess.h"

#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/IDL/src/IHeartbeatReceiverCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"

// Forward declarations
using TA_Base_Core::SystemControllerException;

// Define statics.
TA_Base_App::AdminManager* TA_Base_App::AdminManager::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::AdminManager::m_singletonLock;

namespace TA_Base_App
{
    //TD8617 Mintao++
    const unsigned int AdminManager::DEFAULT_REFRESH_RATE = 0; 

    AdminManager::AdminManager()
        : m_hostName(""), 
          m_locationName(""),  	// TD16592 CF++
          m_sessionId(""),
          m_operatorName(""),
		  m_isRunning(false),
		  m_errorOccurred(false)
    {
        // Get the entity key.
        std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
        TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
        m_entityKey = entityData->getKey();
        delete entityData;

        // Set the possible refresh intervals.
        m_refreshIntervals.push_back(1);
        m_refreshIntervals.push_back(5);
        m_refreshIntervals.push_back(10);
        m_refreshIntervals.push_back(20);
        m_refreshIntervals.push_back(60);
        //TD8617 Mintao++
        //default refresh rate is 1 sec
        m_currentRefreshRate = DEFAULT_REFRESH_RATE;
		nThreshhold = 0;
    }


    AdminManager::~AdminManager()
    {
        logout();
    }


    AdminManager& AdminManager::getInstance()
    {
        // Make this threadsafe.
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

	    if (s_instance == NULL)
        {
            s_instance = new AdminManager();
            if (s_instance == NULL)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,
                    "New failed to allocate AdminManager");
                exit(1);
            }
        }

       return *s_instance;
       }


    void AdminManager::registerObserver(const AdminObserver* observer)
    {
        if (observer != NULL)
        {
            {
                TA_Base_Core::ThreadGuard guard(m_observerLock);
                m_observers.push_back((AdminObserver*)observer);
            }

			notifyObservers();
        }
    }


    void AdminManager::unregisterObserver(const AdminObserver* observer)
    {
        TA_Base_Core::ThreadGuard guard(m_observerLock);

        if (observer == NULL)
        {
            return;
        }

        std::list<AdminObserver*>::iterator it;
        for (it = m_observers.begin(); it != m_observers.end(); it++)
        {
            if (observer == *it)
            {
                m_observers.erase(it);
                break;
            }
        }
    }


    int AdminManager::login(const std::string& hostName,
                              const std::string& portNumber,
                              const std::string& sessionId,
                              const std::string& operatorName)
    {
        if (atoi(portNumber.c_str()) < 1024)
        {
            return ERR_AUTH_SERVICE_ERROR;
        }

        m_hostName = hostName;
        m_sessionId = sessionId;
        m_operatorName = operatorName;

        // TD16592 ++
        // Get the login station name
        TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(hostName);
        m_locationName = entityData->getLocationName();
        delete entityData;
        // ++ TD16592

        // Get the local machine
        const int nameLength = 512;
        char localHostName[nameLength];
        if ((gethostname(localHostName, nameLength) != 0))
        {
            return ERR_AUTH_SOCKET_ERROR;
        }

        std::string localMachine(hostName);
        
        CORBA::Object_var tmpVar;
        TA_Base_Core::IHeartbeatReceiverCorbaDef_var heartbeatReceiverVar;
        TA_Base_Core::IProcessManagerCorbaDef_var processManagerVar;

        // First, attempt to resolve the two basic System Controller objects
        std::string connectionString;
        try
        {
            connectionString = "corbaloc::";
            connectionString += hostName + ":";
            connectionString += portNumber + "/SystemControllerMonitor";
            tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
            heartbeatReceiverVar = TA_Base_Core::IHeartbeatReceiverCorbaDef::_narrow(tmpVar);
      
            connectionString = "corbaloc::";
            connectionString += hostName + ":";
            connectionString += portNumber + "/ProcessManager";
            tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
            processManagerVar = TA_Base_Core::IProcessManagerCorbaDef::_narrow(tmpVar);
        }
        catch(...)  // Doesn't seem to be one on the host specified
        {
            return ERR_NO_SYSTEM_CONTROLLER_ERROR;  
        }

        connectionString = "corbaloc::";
        connectionString += hostName + ":";
        connectionString += portNumber + "/SystemControllerAdmin";
        tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
        m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_narrow(tmpVar);

        if (CORBA::is_nil(m_systemController))
        {
            return ERR_GET_ADMIN_ERROR ;
        }

        m_entityNameToDetails.clear();
        m_entityDescriptionToDetails.clear();
			
        // Start thread
		start();

        return ERR_AUTH_NO_ERROR;
    }


    void AdminManager::logout()
    {
		TA_Base_Core::ThreadGuard guard(m_processLock);

        terminateAndWait();
        m_processes.clear();
    }


	void AdminManager::run()
	{
		const int SLEEP_TIME_MSECS = 500;

		int periodMsecs = m_refreshIntervals[m_currentRefreshRate] * 1000;
		int nIterations = periodMsecs / SLEEP_TIME_MSECS;
		int currIteration;

		m_isRunning = true;


		while (m_isRunning)
		{
			try
			{
				// Fetch process data.
				refreshData();
				m_errorOccurred = false;
			}
			catch(...)
			{
				// Couldn't retrieve process data. Notify observers, so
				// they get an exception when they call getProcesses().
				m_errorOccurred = true;
				notifyObservers();
			}

			// Sleep for the required amount of time.
			currIteration = 0;
			while (m_isRunning && currIteration < nIterations)
			{
				sleep(SLEEP_TIME_MSECS);
				currIteration++;
			}
		}
	}


	void AdminManager::terminate()
	{
		m_isRunning = false;
	}


    void AdminManager::refreshData()
    {
        ProcessDataSeq_var pProcessSeq;
    
        try
        {
	        // Get new process data.
            pProcessSeq = m_systemController->getProcessData();
			nThreshhold = 0;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException",
                                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			nThreshhold += 1;
			// Pass the exception on as a SystemControllerException.
			if(nThreshhold >= 3)
			{
				TA_THROW(SystemControllerException("Error calling getProcessData"));
			}
			return;
        }
        catch( ... )
        {
			// Pass the exception on as a SystemControllerException.
            TA_THROW(SystemControllerException("Error calling getProcessData"));
        }

		TA_Base_Core::ThreadGuard guard(m_processLock);

        if (pProcessSeq->length() != m_processes.size())
        {
			// Size of sequence doesn't match vector. Repopulate vector.
            buildData(pProcessSeq);
            notifyObservers();
        }
        else
        {
            for (int i = 0; i < pProcessSeq->length(); i++)
            {
                if (!compareProcesses(pProcessSeq[i], m_processes[i]))
                {
					// Process vector out of date. Update it.
                    buildData(pProcessSeq);
                    notifyObservers();
                    return;
                }
            }
        }
    }


    std::vector<int> AdminManager::getRefreshIntervals()
    {
        return m_refreshIntervals;
    }


    unsigned int AdminManager::getCurrentRefreshRate()
    {
        return m_currentRefreshRate;
    }


    void AdminManager::setRefreshRate(unsigned int newRate)
    {
        if (newRate < m_refreshIntervals.size() && 
			newRate != m_currentRefreshRate)
        {
            m_currentRefreshRate = newRate;
            
			// Stop the thread and start a new one.
			terminateAndWait();
			start();
        }
    }


    void AdminManager::startProcess(const std::string& processId)
    {
		FUNCTION_ENTRY("startProcess");

        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;

            m_systemController->startProcess(entityName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling startProcess"));
        }

		FUNCTION_EXIT;
    }


    void AdminManager::changeProcessOperationMode(const std::string& processId,
                                                  TA_Base_Core::EOperationMode opMode)
    {
		FUNCTION_ENTRY("changeProcessOperationMode");

        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;

            m_systemController->changeAgentOperationMode(entityName.c_str(), opMode);
        }
        catch(TA_Base_Core::ISystemControllerAdminCorbaDef::ModeChangeException& ex)
        {

            TA_THROW(SystemControllerException(ex.reason.in()));
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Unknown error."));
        }

		FUNCTION_EXIT;
    }


    void AdminManager::stopProcess(const std::string& processId)
    {
		FUNCTION_ENTRY("stopProcess");

        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;

            m_systemController->stopProcess(entityName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling stopProcess"));
        }

		FUNCTION_EXIT;
    }


    void AdminManager::shutdownSystemController()
    {
		FUNCTION_ENTRY("shutdownSystemController");

        try
        {
            m_systemController->shutdown(m_hostName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling shutdown"));
        }

		FUNCTION_EXIT;
    }


    std::vector<ProcessData> AdminManager::getProcesses()
    {  
		if (m_errorOccurred)
		{
			TA_THROW(SystemControllerException("Error calling getProcessData"));
		}

		TA_Base_Core::ThreadGuard guard(m_processLock);
		return m_processes;
    }


    std::string AdminManager::getEntityDescription(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);

        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.entityDescription;
        }
        else
        {
            // Entity name not found. Just return "".
            return "";
        }
    }


    bool AdminManager::isManagedProcessFromName(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);

        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.isManagedProcess;
        }

        // Entity name not found. Just return false to be safe
        return false;
    }


    bool AdminManager::isManagedProcessFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);

        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isManagedProcess;
        }

        // Entity description not found. Just return false to be safe
        return false;
    }


    bool AdminManager::isServiceProcessFromName(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);

        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.isServiceProcess;
        }

        // Entity name not found. Just return false to be safe
        return false;
    }


    bool AdminManager::isServiceProcessFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);

        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isServiceProcess;
        }

        // Entity description not found. Just return false to be safe
        return false;
    }
	bool AdminManager::isAutoFailBackFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);

        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isAutoFailBack;
        }

        // Entity description not found. Just return false to be safe
        return false;
    }

    void AdminManager::buildData(ProcessDataSeq_var processes)
    {
        m_processes.clear();
        std::string entityName;

        for (int i = 0; i < processes->length(); i++)
        {
            entityName = processes[i].ProcessId;

            ProcessInformationMap::iterator iter = m_entityNameToDetails.find(entityName);
            if (iter == m_entityNameToDetails.end())
            {
                // No details for this entity name. Try to fetch it from
                // the database.
                TA_Base_Core::IProcess* entityData = NULL;
                ProcessInformation info;

                try
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "Fetching entity details for %s from database",
                        entityName.c_str());

                    entityData = dynamic_cast<TA_Base_Core::IProcess*>(TA_Base_Core::EntityAccessFactory::getInstance().getEntity2(entityName));
                    
                    if (entityData != NULL)
                    {
                        info.entityName = entityName;
                        info.entityDescription = entityData->getDescription();
                        info.isManagedProcess = entityData->isManagedProcess();
                        info.isServiceProcess = entityData->isServiceProcess();
						info.isAutoFailBack = entityData->shouldAutoFailback();
                    }
                    else
                    {
                        // Error getting the details. Just use the default instead.
                        info.entityName = entityName;
                        info.entityDescription = entityName;
                        info.isManagedProcess = false;
                        info.isServiceProcess = false;
						info.isAutoFailBack = false;
                    }
                }
                catch(...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Error retrieving entity details for entity %s",
                        entityName.c_str());

                    // Error getting the details. Just use the default instead.
                    info.entityName = entityName;
                    info.entityDescription = entityName;
                    info.isManagedProcess = false;
                    info.isServiceProcess = false;
                }

                m_entityNameToDetails[entityName] = info;
                m_entityDescriptionToDetails[info.entityDescription] = info;

                if (entityData != NULL)
                {
                    delete entityData;
                    entityData = NULL;
                }
            }

            m_processes.push_back(processes[i]);

        }
    }


    void AdminManager::notifyObservers()
    {
        TA_Base_Core::ThreadGuard guard(m_observerLock);

        // Notify the registered observers
        std::list<AdminObserver*>::iterator it;
        for (it = m_observers.begin(); it != m_observers.end(); it++)
        {
            (*it)->onNewData();
        }
    }


    void AdminManager::setProcessRunParam(const std::string& processId, const TA_Base_Core::RunParam& param)
    {
		FUNCTION_ENTRY("setProcessRunParam");

        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;

            m_systemController->setRunParam(entityName.c_str(), param);
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling setRunParam"));
        }

		FUNCTION_EXIT;
    }


    std::string AdminManager::getProcessDebugLevel(const std::string& processId)
    {
		TA_Base_Core::ThreadGuard guard(m_processLock);

        // Convert the ProcessId (entity description) into an entity name.
        std::string entityName = m_entityDescriptionToDetails[processId].entityName;

        for (std::vector<ProcessData>::iterator iter = m_processes.begin();
            iter != m_processes.end(); iter++)
        {
            if (iter->ProcessId.in() == entityName)
            {
                return iter->debugLevel;
            }
        }
        return "";
    }


    bool AdminManager::compareProcesses(const ProcessData& process1, const ProcessData& process2) const
    {
        if (strcmp(process1.ProcessId.in(), process2.ProcessId.in()) != 0)
        {
            return false;
        }
        if (process1.state != process2.state)
        {
            return false;
        }
		if (process1.requestedState != process2.requestedState)
		{
			return false;
		}
        if (strcmp(process1.host.in(), process2.host.in()) != 0)
        {
            return false;
        }
        if (process1.started != process2.started)
        {
            return false;
        }
        if (process1.restarts != process2.restarts)
        {
            return false;
        }
        if (process1.operationMode != process2.operationMode)
        {
            return false;
        }
        if (strcmp(process1.debugLevel.in(), process2.debugLevel.in()) != 0)
        {
            return false;
        }

        return true;
    }

} // namespace TA_Base_App
