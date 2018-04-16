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
  * This class maintains the processes. It implements the
  * IProcessManagerCorbaDef interface to allow processes to
  * register with it, and it creates a ProcessInfo object for
  * each process that registers.
  *
  */
#ifdef _MSC_VER
    #pragma warning(disable:4819 4786 4996 4503 4244)
#endif
#include "ProcessManager.h"
#include "ProcessInfo.h"
#include "ManagedProcessInfo.h"
#include "PeerProcessManager.h"
#include "ProcessUtilities.h"
#include "SystemController.h"
#include "Utilities.h"
#include "ProcessStartLagHandler.h"
#include "bus/database_status_monitor/src/SimpleDbStatusChecker.h"
#include "bus/event/src/SessionHelper.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/StdPubUtility.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/system_controller/src/ManagedProcessAccessFactory.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/data_access_interface/system_controller/src/IManagedProcessData.h"
#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IProcess.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/Hostname.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/message/src/TA_CosUtility.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/SystemControllerComms_MessageTypes.h"
#include "core/message/types/DatabaseStatusComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/event_processing/src/RealTimeEventSender.h"
#include "core/event_processing/src/ISessionHelper.h"
#include "core/threads/src/ThreadPoolManager.h"
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/format.hpp>
#include <boost/assign.hpp>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <queue>

using namespace TA_Base_Core;

namespace TA_Base_App
{
    const int ProcessManager::SLEEP_TIME_MSECS = 500;
    const std::string ProcessManager::GROUP_ONLINE = "GroupOnline";
    std::string ProcessManager::m_hostname;

    //TD16585++
    //const unsigned long ProcessManager::POLL_WAIT_PERIOD_IN_MS = 10000; //10 second wait
    //++TD16585

    //
    // getInstance
    //
    ProcessManager& ProcessManager::getInstance()
    {
        return *ACE_Unmanaged_Singleton<ProcessManager, ACE_Recursive_Thread_Mutex>::instance();
    }

    //
    // Constructor
    //
    ProcessManager::ProcessManager()
        : m_initialisedSemaphore(0),
          m_port(0),
          m_initialisedDBStatusMonitor(0),
          m_isStartup(true),
          m_commsSender(NULL),
          m_centralDbStatus(TA_Base_Bus::DatabaseStatusCorbaDef::statusUnknown),
          m_centralDbOnlineTimer(false),
          m_threads(0)
    {
        FUNCTION_ENTRY("Constructor");

        m_hostname = TA_Base_Core::Hostname::getHostname();

        int pollTime = 5;

        setupDbMonitors(pollTime);

        // Start the database monitors
        BOOST_FOREACH(MoniteredDbs::value_type& v, m_moniteredDatabases)
        {
            v.second->start();
        }

        //wait for database monitor to find a working database
        m_initialisedDBStatusMonitor.wait();

        int pollPeriod, responseTimeout;
        setupSystemControllerConfiguration(pollPeriod, responseTimeout);
        initRunParams();

        IWorkItemPtr item1(new SimpleWorkItem(boost::bind(&ProcessManager::setupProcessConfiguration, this, pollPeriod, responseTimeout)));
        IWorkItemPtr item2(new SimpleWorkItem(boost::bind(&ProcessManager::setupConsoleLocationMap, this)));
        IWorkItemPtr item3(new SimpleWorkItem(boost::bind(&ProcessManager::getLocationMapTask, this)));
        m_threads.queueWorkItems(item1, item2, item3);

        //set SessionHelper for RealtimeEventSender for sending real time events.
        TA_Base_Core::RealTimeEventSenderInstance::instance()->setSessionHelper(new TA_Base_Bus::SessionHelper);

        // Create a message sender object that is used to send events
        // about groups going online and offline.
        m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender(TA_Base_Core::SystemControllerComms::Context);
        m_peerProcessManager = new PeerProcessManager;
        m_peerProcessManager->start();

        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ProcessManager");
        activateServant();
        m_this = _this();

        FUNCTION_EXIT;
    }

    //
    // Destructor
    //
    ProcessManager::~ProcessManager()
    {
    }

    //
    // setupDbMonitors
    //
    void ProcessManager::setupDbMonitors(int& pollPeriod)
    {
        TA_ASSERT(pollPeriod != 0, "Poll period can not 0!");

        if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
        {
            throw TA_Base_Core::FatalConfigurationException("db-connection-file not set");
        }

        // can throw DbConnectionNoFile
        // no point in catching it cause there is nothing we can do with it
        TA_Base_Core::DbConnectionStrings connectStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
        TA_Base_Core::DatabaseConnectionMap& dbMap = connectStrings.getConnectionMap();
        std::set<std::string, StriCmp> dbConnectionStrings;

        BOOST_FOREACH(TA_Base_Core::DatabaseConnectionMap::value_type& v, dbMap)
        {
            BOOST_FOREACH(TA_Base_Core::DataConnection& connection, v.second)
            {
                dbConnectionStrings.insert(connection.second);
            }
        }

        // get the runtime parameter of central db and monitored db
        if (RunParams::getInstance().isSet(RPARAM_MONITOREDDATABASE))
        {
            std::string connectionString = RunParams::getInstance().get(RPARAM_MONITOREDDATABASE);
            dbConnectionStrings.insert(connectionString);
            m_monitoredDbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(connectionString);
        }

        if (RunParams::getInstance().isSet(RPARAM_CENTRALDATABASE))
        {
            std::string connectionString = RunParams::getInstance().get(RPARAM_CENTRALDATABASE);
            dbConnectionStrings.insert(connectionString);
            m_centralDbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(connectionString);
        }

        if (RunParams::getInstance().isSet(RPARAM_DBCONNECTION))
        {
            std::string connectionString = RunParams::getInstance().get(RPARAM_DBCONNECTION);
            dbConnectionStrings.insert(connectionString);
            m_controlDbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(connectionString);
        }

        if (m_centralDbName.empty())
        {
            BOOST_FOREACH(const std::string& connectionString, dbConnectionStrings)
            {
                if (connectionString.find("TRANSACT") != std::string::npos)
                {
                    m_centralDbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(connectionString);
                    break;
                }
            }
        }

        // wait at least 5 seconds so db is not overloaded
        int pollTime = pollPeriod < 5 ? 5 : pollPeriod;

        BOOST_FOREACH(const std::string& connectionString, dbConnectionStrings)
        {
            m_moniteredDatabases[connectionString] = new TA_Base_Bus::SimpleDbStatusChecker(connectionString, pollTime, this);
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Add database %s to the monitored database map", connectionString.c_str());
        }
    }

    //
    // setupConsoleLocationMap
    //
    void ProcessManager::setupConsoleLocationMap()
    {
        Console2LocationMap console2LocationMap;
        IConsolePtrList consoles = CStdPubUtility::toShared(ConsoleAccessFactory::getInstance().getAllConsoles());

        BOOST_FOREACH(TA_Base_Core::IConsolePtr console, consoles)
        {
            console2LocationMap.insert(std::make_pair(console->getName(), console->getLocation()));
        }

        m_console2LocationMapFuture.set(console2LocationMap);
        m_console2LocationMap.swap(console2LocationMap);
    }

    //
    // setupSystemControllerConfiguration
    //
    void ProcessManager::setupSystemControllerConfiguration(int& pollPeriod, int& responseTimeout)
    {
        try
        {
            // Get the agent poll period, response timeout, and debug log dir from the database.
            TA_Base_Core::ISystemControllerDataPtr sysControlData(TA_Base_Core::SystemControllerAccessFactory::getInstance().getSystemControllerDataByName(m_hostname));
            pollPeriod = sysControlData->getAgentPollPeriod();
            responseTimeout = sysControlData->getAgentResponseTimeout() * 1000;
            m_debugLogDir = sysControlData->getDebugLogDir();
            m_binDir = sysControlData->getBinDir();
            IEntityDataPtr entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(sysControlData->getServerEntityKey()));
            m_serverEntityData.reset(new ServerEntityData(entityData->getKey()));
            m_debugLogDir = TA_Base_Core::getRunParamValue("DebugLogDir", m_debugLogDir);
            m_binDir = TA_Base_Core::getRunParamValue("BinDir", m_binDir);
            TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCATIONKEY, m_serverEntityData->getLocation());
        }
        catch (TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating ProcessManager"));
        }
        catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating ProcessManager"));
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "");
            TA_THROW(TA_Base_Core::SystemControllerException("Configuration errors creating ProcessManager"));
        }
    }

    //
    // setupProcessConfiguration
    //
    void ProcessManager::setupProcessConfiguration(int pollPeriod, int responseTimeout)
    {
        IManagedProcessDataList managedProcessDatas = ManagedProcessAccessFactory::getInstance().getManagedProcesses(m_hostname);
        std::vector<unsigned long> managedProcessKeys;

        BOOST_FOREACH(IManagedProcessData* process, managedProcessDatas)
        {
            managedProcessKeys.push_back(process->getManagedProcessKey());
        }

        IEntityKeyToEntityMap parentKeyToEntityMap = EntityAccessFactory::getInstance().getKeyToEntityMap(managedProcessKeys);
        std::vector<IEntityDataList> childrenDatas = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(managedProcessKeys);

        for (size_t i = 0; i < managedProcessDatas.size(); ++i)
        {
            IManagedProcessDataPtr process(managedProcessDatas[i]);
            TA_Base_Core::IEntityDataPtr parentData(parentKeyToEntityMap[managedProcessDatas[i]->getManagedProcessKey()]);
            IEntityDataList childrenData(childrenDatas[i]);
            IWorkItemPtr item(new SimpleWorkItem(boost::bind(&ProcessManager::createManagedProcessInfo, this, process, parentData, childrenData, pollPeriod, responseTimeout)));
            m_threads.queueWorkItem(item);
        }

        new ProcessStartLagHandler(*this, managedProcessKeys.size());
    }

    void ProcessManager::createManagedProcessInfo(TA_Base_Core::IManagedProcessDataPtr managedProcessData, TA_Base_Core::IEntityDataPtr parentData, IEntityDataList childrenData, int pollPeriod, int responseTimeout)
    {
        try
        {
            // Cast the parent to an IProcess.
            IProcessPtr process = boost::dynamic_pointer_cast<IProcess>(parentData);

            if (!process)
            {
                TA_THROW(TA_Base_Core::SystemControllerException(str(boost::format("Process %s is not a MonitoredProcess entity type") % parentData->getName())));
            }

            std::string entityName = parentData->getName();

            // Now get whether or not it's a managed process.
            if (process->isManagedProcess())
            {
                if (childrenData.size() > 1)
                {
                    std::string error = "More than one child found for monitored process " + parentData->getName();
                    TA_THROW(TA_Base_Core::SystemControllerException(error));
                }
                else if (childrenData.size() < 1)
                {
                    std::string error = "No children found for monitored process " + parentData->getName();
                    // TD 12139 if no agent configured for this monitored process
                    // just log an error message instead of exit system controller
                    // TA_THROW(TA_Base_Core::SystemControllerException(error));
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, error.c_str());
                    return;
                }

                IEntityDataPtr childData(childrenData[0]);
                std::string childEntityName = childData->getName();

                // Get the notify port of the process, if it's a service process.
                std::string portString = childData->getAddress();
                int notifyPort = atoi(portString.c_str());

                // Get the peers for this process.
                std::vector<std::string> processPeers = TA_Base_Core::ManagedProcessAccessFactory::getInstance().getPeersOfManagedProcess(entityName);

                ManagedProcessInfo* processInfo = new ManagedProcessInfo(entityName,
                                                                         childEntityName,
                                                                         process.get(),
                                                                         processPeers,
                                                                         pollPeriod,
                                                                         managedProcessData->getDefaultOperatingMode(),
                                                                         notifyPort,
                                                                         responseTimeout, getBinDir(),
                                                                         managedProcessData->isStandbyProcess(),
                                                                         managedProcessData->getMonitorGroupName(),
                                                                         managedProcessData->getStartLag(),
                                                                         managedProcessData->hasStandbyProcess());
                processInfo->start();
                m_processes[entityName] = processInfo;

                BOOST_FOREACH(TA_Base_Core::ProcessData& update, m_peerProcessDataCache[entityName])
                {
                    handlePeerProcessData(processInfo, update);
                }

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Created ManagedProcessInfo object for entity %s", entityName.c_str());
            }
            else
            {
                m_processes[entityName] = new ProcessInfo(entityName, process.get(), pollPeriod, getBinDir());
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Created ProcessInfo object for entity %s", entityName.c_str());
            }
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
        }
        catch (TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "");
        }
    }

    //
    // cleanUp
    //
    void ProcessManager::cleanUp()
    {
        FUNCTION_ENTRY("cleanUp");

        {
            TA_Base_Core::ThreadPoolManager threads(3, 3);
            IWorkItemPtr item1(new SimpleWorkItem(boost::bind(&ProcessManager::terminateAllDatabaseMonitors, this)));
            IWorkItemPtr item2(new SimpleWorkItem(boost::bind(&ProcessManager::terminateAllProcesses, this)));
            IWorkItemPtr item3(new SimpleWorkItem(boost::bind(&ProcessManager::deregisterProcessObserver, this)));
            threads.queueWorkItems(item1, item2, item3);
            threads.waitForAll(10000);
        }

        terminateAllManagedProcesses();

        // Delete the comms sender.
        delete m_commsSender;

        // Clean up the peer process manager.
        m_peerProcessManager->terminateAndWait();
        m_peerProcessManager->deactivateAndDeleteServant();
        m_peerProcessManager = NULL;

        FUNCTION_EXIT;
    }

    void ProcessManager::deregisterProcessObserver()
    {
        ProcessSubjectCorbaDefMap peers;

        {
            TA_Base_Core::ThreadGuard guard(m_peerLock);
            peers = m_peers;
        }

        // Deregister from all peers
        BOOST_FOREACH(ProcessSubjectCorbaDefMap::value_type& v, peers)
        {
            try
            {
                if (!CORBA::is_nil(v.second))
                {
                    v.second->deregisterProcessObserver(m_hostname.c_str());
                }
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Error deregistering from peer %s while shutting down Process Manager", v.first.c_str());
            }
        }
    }

    //
    // getDebugLogDir
    //
    std::string ProcessManager::getDebugLogDir() const
    {
        return m_debugLogDir;
    }

    //
    // getBinDir
    //
    std::string ProcessManager::getBinDir() const
    {
        return m_binDir;
    }

    //
    // getAllProcesses
    //
    std::vector<TA_Base_Core::ProcessData> ProcessManager::getAllProcesses()
    {
        std::vector<TA_Base_Core::ProcessData> processes(m_processes.size());
        size_t i = 0;

        for (EntityToProcessMap::iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            iter->second->getProcessData(processes[i++]);
        }

        return processes;
    }

    void ProcessManager::getAllProcesses(TA_Base_Core::ProcessDataSeq& processes)
    {
        processes.length(m_processes.size());
        size_t i = 0;

        for (EntityToProcessMap::iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            iter->second->getProcessData(processes[i++]);
        }
    }

    //
    // getAgentStatusSummary
    //
    TA_Base_Core::AgentStatusEnum ProcessManager::getAgentStatusSummary()
    {
        ManagedProcessInfo* process;
        TA_Base_Core::EProcessStatus processState;
        TA_Base_Core::EOperationMode operationMode;                //record the operation mode of current process in process map
        TA_Base_Core::EOperationMode operationModeFirst;        //record the previous operation mode in process map
        TA_Base_Core::EOperationMode operationModeSecond;        //record the current operation mode in process map
        bool bLocFirstFirst = true;            //record if this is the first process of first location in process map
        bool bLocSecondFirst = true;            //record if this is the first process of second location in process map
        unsigned long nLocationKey;                    //assume there are two location agents running at most
        TA_Base_Core::AgentStatusEnum status = TA_Base_Core::Normal;

        if (m_processes.empty())
        {
            // Process map has been cleared, so we must be shutting down. Return AgentFault.
            return TA_Base_Core::AgentFault;
        }

        for (EntityToProcessMap::iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                processState = process->getProcessStatus();

                if (processState == TA_Base_Core::Unstarted
                        || processState == TA_Base_Core::Startup
                        || processState == TA_Base_Core::Terminating
                        || processState == TA_Base_Core::NotRunning
                        || processState == TA_Base_Core::Stopped)
                {
                    // ++AZ TES1045
                    // for agents that are configured for Control mode on both
                    // master/standby servers, e.g., the notification service agent,
                    // do not show as fault
                    if (process->getEntityName().find("Notification") == std::string::npos)
                    {
                        // A process isn't in a "normal state". No point continuing to check.
                        return TA_Base_Core::AgentFault;
                    }

                    // AZ++
                }
                else if (process->getEntityName().find("Notification") == std::string::npos)
                {
                    // it is not a Notification Agent
                    operationMode = process->getOperationMode();

                    if (bLocFirstFirst == true)
                    {
                        // it is first process of first location
                        nLocationKey = process->getLocationKey();            //record the first location key
                        operationModeFirst = operationMode;                    //record operation mode os first location
                        bLocFirstFirst = false;
                    }
                    else if (bLocFirstFirst == false && bLocSecondFirst == true && nLocationKey != process->getLocationKey())
                    {
                        // it is first process of second location
                        bLocSecondFirst = false;
                        operationModeSecond = operationMode;                    //record operation mode os first location
                    }
                    else if (bLocFirstFirst == false && nLocationKey == process->getLocationKey())   //
                    {
                        // it is not the first process, it for first site
                        if (operationMode != operationModeFirst)
                        {
                            // All process are not in the same operation mode, and so far all processes are running.
                            status = TA_Base_Core::Abnormal;
                        }
                    }
                    else if (bLocSecondFirst == false && nLocationKey != process->getLocationKey())
                    {
                        // it is not the first process, it for second site
                        if (operationMode != operationModeSecond)
                        {
                            // All process are not in the same operation mode, and so far all processes are running.
                            status = TA_Base_Core::Abnormal;
                        }
                    }
                }
                else if (process->getEntityName().find("Notification") != std::string::npos)
                {
                    //it is a notification Agent
                    if (process->getOperationMode() != process->getNormalOperationMode())
                    {
                        status = TA_Base_Core::Abnormal;
                    }
                }
            }
        }

        return status;
    }

    //
    // haveAllAgentsStartedAtLeastOnce
    //
    bool ProcessManager::haveAllAgentsStartedAtLeastOnce()
    {
        ManagedProcessInfo* process;
        TA_Base_Core::EProcessStatus processState;

        for (EntityToProcessMap::iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                if (process->getNumRestarts() == 0)
                {
                    // Agent hasn't restarted. Check if it has started up successfully, or
                    // has been stopped.
                    processState = process->getProcessStatus();

                    if (processState != TA_Base_Core::RunningControl
                            && processState != TA_Base_Core::RunningMonitor
                            && processState != TA_Base_Core::RunningStandby
                            && processState != TA_Base_Core::Stopped)
                    {
                        // Agent has not restarted, but has not started up or been stopped.
                        return false;
                    }
                }
            }
        }

        return true;
    }

    /*//TD16585++
    void ProcessManager::waitForOfflineDatabase()
    {
        FUNCTION_ENTRY("waitForOfflineDatabase");

        int waitResult = 0;
        m_isWaitingForOfflineDatabase = true;
        waitResult = m_timedWaitForOfflineDataBase.timedWait(POLL_WAIT_PERIOD_IN_MS);
        if( waitResult == 0 )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "SystemControllerMonitor::waitForOfflineDatabase no offline database found within dbstatus monitor cycle time");
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "SystemControllerMonitor::waitForOfflineDatabase Foud offline database within the status monitor cycle time");
        }
        FUNCTION_EXIT;
    }
    //++TD16585*/

    void ProcessManager::setLocalDatabseStatusOffline()
    {
        // set all db moniter to offline status first.
        MoniteredDbs::iterator dbIt = m_moniteredDatabases.begin();

        while (dbIt != m_moniteredDatabases.end())
        {
            if (dbIt->second != 0)
            {
                // only set local database failure in case of server failover.
                if (dbIt->first.find(m_centralDbName) == std::string::npos)
                {
                    LOG1(SourceInfo, DebugUtil::DebugInfo,
                         "ProcessManager::setAllUsedDatabseStatusOffline: set %s to offline", dbIt->first.c_str());
                    dbIt->second->setDatabaseOffLine();
                }
            }

            dbIt++;
        }
    }

    //
    // respondToPeerSCFailure
    //
    void ProcessManager::respondToPeerSCFailure(const std::string& host)
    {
        FUNCTION_ENTRY("respondToPeerSCFailure");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ProcessManager::respondToPeerSCFailure(host: %s)", host.c_str());
        {
            TA_Base_Core::ThreadGuard guard(m_registerLock);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ProcessManager::respondToPeerSCFailure, after registerLock");

            // If we haven't yet registered with the peer that has just failed, indicate
            // to this thread that we shouldn't bother trying.
            m_peersToRegisterWith.erase(host);
        }

        // Deregister the peer with the PeerProcessManger.
        if (m_peerProcessManager != NULL)
        {
            m_peerProcessManager->deregisterProcessObserver(host.c_str());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ProcessManager::respondToPeerSCFailure, call deregisterProcessObserver");
        }

        //std::set<ManagedProcessInfo*> failOverProcesses; // gongzong++ CL-21328

        for (EntityToProcessMap::iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* localProcess = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (localProcess != NULL)
            {
                // Tell the agent that a peer has failed.
                localProcess->setPeerNotRunning(host);
                localProcess->setPeerState(host, ManagedProcessInfo::NotInControl);
            }

            if (localProcess != NULL && localProcess->isAPeer(host))
            {
                // This agent is running a peer on the dead server. Need to
                // invalidate its notify hosts.
                std::vector<std::string> invalidNotifyHosts = getInvalidNotifyHosts(host, localProcess->getLocationKey());
                localProcess->invalidateNotifyHosts(invalidNotifyHosts);

                if (localProcess->getOperationMode() == TA_Base_Core::Monitor)
                {
                    // Our agent is running in Monitor. Need to failover.
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failing over %s to Control mode", (iter->first).c_str());

                    localProcess->setStatusToChangeTo(TA_Base_Core::RunningControl);

                    // raise alarm after agent switch to control.
                    //failOverProcesses.insert(localProcess); // gongzong++ CL-21328
                    // // Raise an alarm
                    // localProcess->raiseFailoverAlarm();
                }
            }
        }

        {
            TA_Base_Core::ThreadGuard guard(m_peerLock);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ProcessManager::respondToPeerSCFailure, after m_peerLock");
            // Reset to nil so that we reregister ourselves with the other System Controller when it is restored.
            m_peers[host] = TA_Base_Core::IProcessSubjectCorbaDef::_nil();
        }

        // gongzong++ CL-21328
        //// Raise an alarm
        //for(std::set<ManagedProcessInfo*>::iterator it = failOverProcesses.begin(); it!=failOverProcesses.end(); it++)
        //{
        //    (*it)->raiseFailoverAlarm();
        //}

        FUNCTION_EXIT;
    }

    //
    // respondToPeerSCRestored
    //
    void ProcessManager::respondtoPeerSCRestored(const std::string& host)
    {
        FUNCTION_ENTRY("ProcessManager::respondtoPeerSCRestored");

        // Need to check that there's not already a CORBA var assigned in
        // the m_peers map. This may be the case if we successfully
        // registered on startup - in which case we don't want to register
        // twice.
        {
            TA_Base_Core::ThreadGuard guard(m_peerLock);

            if (m_peers[host] != TA_Base_Core::IProcessSubjectCorbaDef::_nil())
            {
                return;
            }
        }
        {
            TA_Base_Core::ThreadGuard guard(m_registerLock);
            // Indicate to the ProcessManager thread that we should register with
            // the peer that is now up and running.
            m_peersToRegisterWith.insert(host);
        }
        FUNCTION_EXIT;
    }

    //
    // respondToGroupOffline
    //
    void ProcessManager::respondToGroupOffline(const std::string& group)
    {
        FUNCTION_ENTRY("respondToGroupOffline");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "GroupOffline: GroupName: %s", group.c_str());
        // TD6972 If an agent is off line when this change is made
        // or an agent is stated at a later time then it will not get
        // notified that the group has failed.  This call sets a run
        // parameter so that agents started in the future will be
        // notified of the group failure.
        m_groupsOffline.insert(group);
        updateGroupsOfflineRunParam();

        // Notify agents that the group is offline.
        for (EntityToProcessMap::const_iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                process->notifyGroupOffline(group);
            }
        }

        // Now need to send a message that the group is offline.

        // Inject the name of the group into a Corba Any.
        CORBA::Any commsData;
        commsData <<= group.c_str();

        // Send the message.
        if (m_commsSender != NULL)
        {
            m_commsSender->sendCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOffline, // Message Type
                m_serverEntityData->getKey(),       // entityKey
                commsData,                          // message Data
                m_serverEntityData->getSubsystem(), // subsystemKey
                m_serverEntityData->getLocation()   // locationKey
            );
        }

        FUNCTION_EXIT;
    }

    //
    // respondToGroupOnline
    //
    void ProcessManager::respondToGroupOnline(const std::string& group)
    {
        FUNCTION_ENTRY("respondToGroupOnline");

        // TD6972 This will erase this group from the list of those offline. It then
        // regenerates the stored RunParam so that any new agents starting will get
        // the correct list of groups
        m_groupsOffline.erase(group);
        updateGroupsOfflineRunParam();

        // Notify agents that the group is online.
        for (EntityToProcessMap::const_iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                process->notifyGroupOnline(group);
            }
        }

        // Now need to send a message that the group is online.

        // Inject the name of the group into a Corba Any.
        CORBA::Any commsData;
        commsData <<= group.c_str();

        // Send the message.
        if (m_commsSender != NULL)
        {
            m_commsSender->sendCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline, // Message Type
                m_serverEntityData->getKey(),       // entityKey
                commsData,                          // message Data
                m_serverEntityData->getSubsystem(), // subsystemKey
                m_serverEntityData->getLocation()   // locationKey
            );
        }

        FUNCTION_EXIT;
    }

    //
    // reregisterWithPeer
    //
    void ProcessManager::reregisterWithPeer(const std::string& host)
    {
        FUNCTION_ENTRY("reregisterWithPeer");

        TA_Base_Core::ThreadGuard guard(m_registerLock);

        // Indicate to the ProcessManager thread that we should register with
        // the peer. Add it to list of the peers to register with regardless
        // of whether or not we have a CORBA var for this peer.
        m_peersToRegisterWith.insert(host);

        FUNCTION_EXIT;
    }

    //
    // notifyProcessData
    //
    void ProcessManager::notifyProcessData(const TA_Base_Core::ProcessData& peerProcessData)
    {
        FUNCTION_ENTRY("notifyProcessData");

        std::string entity = peerProcessData.ProcessId.in();
        std::string peerHost = peerProcessData.host.in();

        // Log the fact that we've received process data from a peer.
        std::stringstream strm;
        strm << "Received data about process " << entity << " from peer " << peerHost << std::endl
             << "Process Id:      " << peerProcessData.ProcessId.in() << std::endl
             << "Time started:    " << TA_Base_Core::CStdPubUtility::getTimeStr(peerProcessData.started) << std::endl
             << "Host:            " << peerProcessData.host.in() << std::endl
             << "State:           " << peerProcessData.state << std::endl
             << "Requested State: " << peerProcessData.requestedState << std::endl
             << "Operation mode:  " << peerProcessData.operationMode << std::endl
             << "Restarts:        " << peerProcessData.restarts << std::endl
             << "Debug level:     " << peerProcessData.debugLevel.in();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s", strm.str().c_str());

        if (m_processes.find(entity) != m_processes.end())
        {
            // Process is in map

            ManagedProcessInfo* localProcess =  dynamic_cast<ManagedProcessInfo*>(m_processes[entity]);

            if (localProcess == NULL)
            {
                // Process isn't a managed process. Nothing to do then.
                FUNCTION_EXIT;
                return;
            }

            handlePeerProcessData(localProcess, peerProcessData);
        }
        else
        {
            m_peerProcessDataCache[entity].push_back(peerProcessData);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "cache process data %s", entity.c_str());
        }

        FUNCTION_EXIT;
    }

    void ProcessManager::handlePeerProcessData(ManagedProcessInfo* localProcess, const TA_Base_Core::ProcessData& peerProcessData)
    {
        std::string entity = peerProcessData.ProcessId.in();
        std::string peerHost = peerProcessData.host.in();

        if (localProcess->isServiceProcess())
        {
            return;
        }

        bool isPeerProcessRunning = false;

        if (peerProcessData.state == TA_Base_Core::RunningControl ||
                peerProcessData.state == TA_Base_Core::RunningMonitor ||
                peerProcessData.state == TA_Base_Core::GoingToControl ||
                peerProcessData.state == TA_Base_Core::GoingToMonitor)
        {
            isPeerProcessRunning = true;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Update Peer State[%s], local State: %d, peer Sate: %d",
                    localProcess->getEntityName().c_str(), localProcess->getProcessStatus(), peerProcessData.state);
        updatePeersState(localProcess, peerProcessData);

        // Are we the peer for the process we just received information about?
        if (localProcess->isAPeer(peerHost) && localProcess->isRunning())
        {
            TA_Base_Core::EOperationMode localMode;

            if (localProcess->getProcessStatus() == TA_Base_Core::Startup)
            {
                localMode = TA_Base_Core::NotApplicable;
            }
            else
            {
                localMode = localProcess->getOperationMode();
            }

            if (localMode == TA_Base_Core::Control &&
                    isPeerProcessRunning &&
                    peerProcessData.operationMode == TA_Base_Core::Control)
            {
                // Both agents in Control!
                handleTwoAgentsInControl(localProcess, peerHost);
            }
            else if (localMode == TA_Base_Core::Monitor &&
                     peerProcessData.operationMode == TA_Base_Core::Monitor)
            {
                // Both agents in Monitor!
                handleTwoAgentsInMonitor(localProcess);
            }
            else if (localMode == TA_Base_Core::Monitor &&
                     peerProcessData.operationMode == TA_Base_Core::Control &&
                     localProcess->shouldAutoFailback() &&
                     localProcess->getNormalOperationMode() == TA_Base_Core::Control)
            {
                //Double Control is now handled by this function: handleDoubleControlBaseOnCentralDb
                //LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                //    "Failing back %s", localProcess->getEntityName().c_str());
                //localProcess->setStatusToChangeTo(TA_Base_Core::RunningControl);
            }
            else if (localMode == TA_Base_Core::Monitor && !isPeerProcessRunning)
            {
                // We're in monitor, peer isn't running. Failover.
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Peer %s process not running. Failing over.",
                            localProcess->getEntityName().c_str());

                localProcess->setStatusToChangeTo(TA_Base_Core::RunningControl);
                notifyPeers(localProcess->getEntityName());
            }
        }
    }

    //
    // handleTwoAgentsInControl
    //
    void ProcessManager::handleTwoAgentsInControl(ManagedProcessInfo* agent, const std::string& peerHost)
    {
        FUNCTION_ENTRY("handleTwoAgentsInControl");

        TA_Base_Core::RunParam param;
        param.name = static_cast<const char*>(RPARAM_DOUBLEAGENTSCONTROLRESTORED);
        param.value = TA_Base_Core::CStdPubUtility::getTimeStr(time(NULL)).c_str();
        TA_Base_Core::IProcessObserverCorbaDef_var peer = m_peerProcessManager->getProcessObserver(peerHost);

        if (CORBA::is_nil(peer))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot locate peer %s", peerHost.c_str());
        }
        else
        {
            try
            {
                bool online = isCentralDbOnline();
                unsigned long time = m_centralDbOnlineTimer.elapsed_s();
                TA_Base_Core::EOperationMode mode = peer->handleDoubleControlBaseOnCentralDb(m_hostname.c_str(), online, time);
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                            "Both %s processes in Control, calling handleDoubleControlBaseOnCentralDb to peer %s with (central-db=%s, time-elapsed-sine-online=%lu(seconds)), result mode is %s",
                            agent->getEntityName().c_str(), peerHost.c_str(), getString(online), time, getString(mode));

                if (TA_Base_Core::Control == mode)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Both %s processes in Control, Telling peer %s to switch modes.", agent->getEntityName().c_str(), peerHost.c_str());
                    notifyPeers(agent->getEntityName());
                    agent->setParam(param);
                    return;
                }

                if (TA_Base_Core::Monitor == mode)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Both %s processes in Control, peer is %s. Switching to Monitor", agent->getEntityName().c_str(), peerHost.c_str());
                    agent->setStatusToChangeTo(TA_Base_Core::RunningMonitor);
                    return;
                }
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Caught exception while calling peer %s on handleDoubleControlBaseOnCentralDb", peerHost.c_str());
            }
        }

        // Both agents in Control...
        if (//agent->shouldAutoFailback() && no need to set this condition,TD19118
            agent->getNormalOperationMode() == TA_Base_Core::Control)
        {
            // ...but we're configured to be in Control. Tell the peer
            // to switch operation modes.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Both %s processes in Control. Telling peer to switch modes.", agent->getEntityName().c_str());
            notifyPeers(agent->getEntityName());
            agent->setParam(param);
        }
        else
        {
            // ...and there's no reason why we can't switch modes.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Both %s processes in Control. Switching to Monitor", agent->getEntityName().c_str());
            agent->setStatusToChangeTo(TA_Base_Core::RunningMonitor);

            // gongzong++ CL-21328
            //if (agent->getNormalOperationMode() == TA_Base_Core::Monitor)
            //{
            //    // Close the FailoverOccurred alarm.
            //    agent->closeFailoverAlarm();
            //}
        }

        FUNCTION_EXIT;
    }

    //
    // handleTwoAgentsInMonitor
    //
    void ProcessManager::handleTwoAgentsInMonitor(ManagedProcessInfo* agent)
    {
        FUNCTION_ENTRY("handleTwoAgentsInMonitor");

        // Both agents are in Monitor...
        if (agent->shouldAutoFailback() &&
                agent->getNormalOperationMode() == TA_Base_Core::Monitor)
        {
            // ...but we're configured to be in Monitor. Tell the peer
            // to switch operation modes.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Both %s processes in Monitor. Telling peer to switch modes.",
                        agent->getEntityName().c_str());
            notifyPeers(agent->getEntityName());
        }
        else
        {
            // ...and there's no reason why we can't switch modes.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                        "Both %s processes in Monitor. Failing over",
                        agent->getEntityName().c_str());
            agent->setStatusToChangeTo(TA_Base_Core::RunningControl);

            // gongzong++ CL-21328
            //if (agent->getNormalOperationMode() != TA_Base_Core::Control)
            //{
            //    // Raise a FailoverOccurred alarm.
            //    agent->raiseFailoverAlarm();
            //}
        }

        FUNCTION_EXIT;
    }

    //
    // updatePeerState
    //
    void ProcessManager::updatePeersState(ManagedProcessInfo* agent, const TA_Base_Core::ProcessData& peerProcessData)
    {
        // True if the peer agent is up and running.
        bool isPeerProcessRunning = (peerProcessData.state == TA_Base_Core::RunningControl
                                     || peerProcessData.state == TA_Base_Core::RunningMonitor
                                     || peerProcessData.state == TA_Base_Core::GoingToControl
                                     || peerProcessData.state == TA_Base_Core::GoingToMonitor
                                     || peerProcessData.state == TA_Base_Core::GoingToStandby
                                     || peerProcessData.state == TA_Base_Core::RunningStandby);

        // True if the peer agent is up and running, or is starting up.
        bool hasPeerProcessStarted = (isPeerProcessRunning
                                      || peerProcessData.state == TA_Base_Core::Startup);

        // Update which peers are running.
        if (isPeerProcessRunning)
        {
            // Tell the agent that there's a peer running.
            agent->setPeerRunning(peerProcessData.host.in());
        }
        else
        {
            // Tell the agent that a peer isn't running.
            agent->setPeerNotRunning(peerProcessData.host.in());
        }

        // Update the state of the peers.
        if (hasPeerProcessStarted)
        {
            if (peerProcessData.operationMode == TA_Base_Core::Control)
            {
                if (peerProcessData.state == TA_Base_Core::Startup)
                {
                    // Tell the agent that there's a peer starting in Control mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::StartingInControl);
                }
                else
                {
                    // Tell the agent that there's a peer running in Control mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::RunningInControl);
                }
            }
            else if (peerProcessData.operationMode == TA_Base_Core::Monitor)
            {
                if (peerProcessData.state == TA_Base_Core::Startup)
                {
                    // Tell the agent that there's a peer starting in Monitor mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::StartingInMonitor);
                }
                else
                {
                    // Tell the agent that there's a peer running in Monitor mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::RunningInMonitor);
                }
            }
            else if (peerProcessData.operationMode == TA_Base_Core::Standby)
            {
                if (peerProcessData.state == TA_Base_Core::Startup)
                {
                    // Tell the agent that there's a peer starting in Standby mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::StartingInStandby);
                }
                else
                {
                    // Tell the agent that there's a peer running in Standby mode.
                    agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::RunningInStandby);
                }
            }
            else
            {
                // Tell the agent that there's a peer not in Control mode.
                agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::NotInControl);
            }
        }
        else
        {
            // Tell the agent that a peer isn't running.
            agent->setPeerState(peerProcessData.host.in(), ManagedProcessInfo::NotInControl);
        }
    }

    //
    // terminating
    //
    void ProcessManager::terminating(TA_Base_Core::ProcessId p_processID, TA_Base_Core::EManagedProcessTerminateCode p_code)
    {
        FUNCTION_ENTRY("terminating");

        std::string entityName;

        try
        {
            ProcessInfo& process = getProcess(p_processID);
            entityName = process.getEntityName();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            // A process with this PID is unknown.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown managed application with PID %lu told System Controller it is terminating.", p_processID);

            FUNCTION_EXIT;
            return;
        }

        ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(m_processes[entityName]);
        process->processTerminating(p_code);

        FUNCTION_EXIT;
    }

    //
    // registerManagedProcess
    //
    void ProcessManager::registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess, TA_Base_Core::ProcessId p_processId, CORBA::ULong applicationType, const char* entity)
    {
        FUNCTION_ENTRY("registerManagedProcess");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Managed process %s registering with System Controller", entity);

        std::string entityName;

        try
        {
            ProcessInfo& process = getProcess(p_processId);
            entityName = process.getEntityName();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            // A process with this PID is unknown.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown managed application with PID %lu tried to register with System Controller.", p_processId);
            FUNCTION_EXIT;
            return;
        }

        ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(m_processes[entityName]);
        process->setManagedProcess(p_managedProcess);

        FUNCTION_EXIT;
    }

    //
    // registerManagedApplication
    //
    void ProcessManager::registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp, TA_Base_Core::ProcessId p_processId)
    {
        FUNCTION_ENTRY("registerManagedApplication");

        std::string entityName;

        try
        {
            ProcessInfo& process = getProcess(p_processId);
            entityName = process.getEntityName();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            // A process with this PID is unknown.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown managed application with PID %lu tried to register with System Controller.", p_processId);
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Managed agent %s registering with System Controller", entityName.c_str());

        ManagedProcessInfo* managedProcess = dynamic_cast<ManagedProcessInfo*>(m_processes[entityName]);

        managedProcess->setManagedAgent(p_managedApp);

        // If an alarm was raised for the process because it died, close it now.
        managedProcess->closeFailureAlarm();

        notifyPeers(entityName);

        FUNCTION_EXIT;
    }

    // getParams

    TA_Base_Core::RunParamSeq* ProcessManager::getParams(TA_Base_Core::ProcessId p_processID, const char* hostname)
    {
        FUNCTION_ENTRY("getParams");

        unsigned long locationKey = 0;
        std::string entity;

        // First get the run params that are common to all processes, regardless
        // of location or which process it is.
        TA_Base_Core::RunParams::ParamVector params;
        params = getCommonParams();

        if (p_processID == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Anonymous process fetching RunParams.");

            // Add the System Controller's debug level.
            TA_Base_Core::RunParams::ParamNameValue pnv;
            pnv.name = RPARAM_DEBUGLEVEL;
            pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
            params.push_back(pnv);

            // Get the location.
            //locationKey = getLocationOfHostname(hostname);

            if (m_console2LocationMap.empty())
            {
                m_console2LocationMapFuture.get(m_console2LocationMap);
            }

            std::map< std::string, unsigned long, StriCmp>::iterator it = m_console2LocationMap.find(hostname);

            if (it != m_console2LocationMap.end())
            {
                locationKey = it->second;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Get location key = %lu from hostname %s .", locationKey, hostname);
        }
        else
        {
            try
            {
                entity = getEntityNameOfProcess(p_processID);
            }
            catch (TA_Base_Core::SystemControllerException&)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Process with unknown PID %lu attempting to retrieve RunParams.", p_processID);

                // Send no params back.
                return new TA_Base_Core::RunParamSeq;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s is fetching its RunParams", entity.c_str());

            ManagedProcessInfo* localProcess = dynamic_cast<ManagedProcessInfo*>(m_processes[entity]);

            if (localProcess == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Non-managed process %s attempting to retrieve RunParams.", entity.c_str());
            }
            else
            {
                locationKey = localProcess->getLocationKey();
                TA_Base_Core::RunParams::ParamVector processParams = localProcess->getParams();
                params.insert(params.end(), processParams.begin(), processParams.end());
            }
        }

        // Add the process's location key.
        params.push_back(RunParams::ParamNameValue(RPARAM_LOCATIONKEY, locationKey));

        // Now get the run params that are common for this particular location.
        TA_Base_Core::RunParamSeq& locationParams = getLocationParams(locationKey);

        // Add the run params for this location to the vector.
        for (unsigned int i = 0; i < locationParams.length(); i++)
        {
            params.push_back(RunParams::ParamNameValue(locationParams[i].name.in(), locationParams[i].value.in()));
        }

        // Now make sure we have a notify hosts parameter. If not then we will
        // use the same one that we were given on our command line
        bool wasFound = false;

        for (unsigned int param = 0; param < params.size(); param++)
        {
            if (params[param].name == RPARAM_NOTIFYHOSTS)
            {
                wasFound = true;
                break;
            }
        }

        if (!wasFound)
        {
            params.push_back(RunParams::ParamNameValue(RPARAM_NOTIFYHOSTS, m_cmdLineNotifyHosts));
        }

        // Make sure the information is correctly duplicated.
        TA_Base_Core::RunParamSeq* result = new TA_Base_Core::RunParamSeq;

        result->length(params.size());
        std::stringstream strm;
        strm << str(boost::format("Sending following runparams to %s (PID=%d): ") % entity % p_processID);

        for (unsigned int i = 0; i < params.size(); i++)
        {
            strm << str(boost::format("\n%i - (%s, %s)") % i % params[i].name % params[i].value);
            (*result)[i].name = params[i].name.c_str();
            (*result)[i].value = params[i].value.c_str();
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s", strm.str().c_str());
        FUNCTION_EXIT;
        return result;
    }

    //
    // poll
    //
    void ProcessManager::poll()
    {
        return;
    }

    //
    // statusChanged
    //
    void ProcessManager::statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId)
    {
        FUNCTION_ENTRY("statusChanged");

        std::string entityName = "";

        try
        {
            ProcessInfo& process = getProcess(p_processId);
            entityName = process.getEntityName();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            // A process with this PID is unknown.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown managed application with PID %lu told System Controller it has changed state.", p_processId);

            FUNCTION_EXIT;
            return;
        }

        ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(m_processes[entityName]);
        process->statusChanged(status);

        FUNCTION_EXIT;
    }

    //
    // notifyStatusChanged
    //
    void ProcessManager::notifyStatusChange(const std::string& dbConnectionInfo, TA_Base_Bus::DatabaseStatusCorbaDef::EState newStatus)
    {
        std::string dbStatus;
        std::string dbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(dbConnectionInfo);

        //TD16585++
        //bool isOfflineNotification=false;

        switch (newStatus)
        {
        case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnknown:
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify agents that monitored db %s status is UNKNOWN", dbName.c_str());
            dbStatus = RPARAM_DBUNKNOWN;
            break;

        case TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable:
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify agents that monitored db %s status is ONLINE", dbName.c_str());
            dbStatus = RPARAM_DBONLINE;
            break;

        case TA_Base_Bus::DatabaseStatusCorbaDef::statusUnAvailable:
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Notify agents that monitored db %s status is OFFLINE", dbName.c_str());
            dbStatus = RPARAM_DBOFFLINE;
            //TD16585++
            //isOfflineNotification = true;
            //++TD16585
            break;

        default:
            TA_ASSERT(false, "Unknown or new TA_Base_Bus::DatabaseStatusCorbaDef::State");
        }

        if (m_centralDbName == dbName)
        {
            if (TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable == newStatus)
            {
                if (m_centralDbStatus != TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Central DB %s status changed from OFFLINE to ONLINE, restart timers", dbName.c_str());
                    m_centralDbOnlineTimer.start();
                }
            }
            else
            {
                m_centralDbOnlineTimer.clear();
            }

            m_centralDbStatus = newStatus;
        }

        TA_Base_Core::RunParams::getInstance().set(dbName.c_str(), dbStatus.c_str());

        // now let everyone know about the change;
        TA_Base_Core::RunParam runParam;
        runParam.name = dbName.c_str();
        runParam.value = dbStatus.c_str();

        BOOST_FOREACH(EntityToProcessMap::value_type& v, m_processes)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(v.second);

            if (process != NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                            "Runtime param of process %s run param %s = %s is being updated", process->getEntityName().c_str(), dbName.c_str(), dbStatus.c_str());
                IWorkItemPtr item(new SimpleWorkItem(boost::bind(&ManagedProcessInfo::setParam, process, runParam)));
                m_threads.queueWorkItem(item);
            }
        }

        if (false == m_isStartup)
        {
            // Inject the db name and new status into a Corba Any.
            TA_Base_Bus::DatabaseStatusCorbaDef::DatabaseStatus status;
            status.status = newStatus;
            status.database = dbName.c_str();
            CORBA::Any commsData;
            commsData <<= status;

            // Send a message to any one else who cares
            if (m_commsSender != NULL && m_serverEntityData != NULL)
            {
                m_commsSender->sendCommsMessage(
                    TA_Base_Core::DatabaseStatusComms::DatabaseStatusChange, // Message Type
                    m_serverEntityData->getKey(),       // entityKey
                    commsData,                          // message Data
                    m_serverEntityData->getSubsystem(), // subsystemKey
                    m_serverEntityData->getLocation()   // locationKey
                );
            }
        }

        //monitored db online and another db online
        if (m_isStartup == true && newStatus == TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable)
        {
            m_isStartup = false;

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Sending signal to continue system controller startup of processes...");
            m_initialisedDBStatusMonitor.post();
        }

        /* //TD16585++
        if(isOfflineNotification)
        {
            if(m_isWaitingForOfflineDatabase)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "ProcessManager::notifyStatusChange Signalling for offline database");

                m_timedWaitForOfflineDataBase.signal();
                m_isWaitingForOfflineDatabase = false;
            }
        }
        //TD16585++ */
    }

    //
    // run
    //
    void ProcessManager::run()
    {
        m_isRunning = true;

        // Register with those peers that are up.
        // gongzong++ CL-21301 here do not need lock.
        //{
        //    TA_Base_Core::ThreadGuard guard(m_registerLock);
        //    registerWithAllPeers();
        //}
        registerWithAllPeers();
        // ++gongzong

        // Now that all of the initialising has been done and the process threads
        // have been started, indicate to the SystemController class that it can
        // start the heartbeats.
        m_initialisedSemaphore.post();

        while (m_isRunning)
        {
            // Register with the peers that are up and running, and that we haven't
            // registered with yet.
            {
                std::set<std::string> peersToRegister;
                {
                    TA_Base_Core::ThreadGuard guard(m_registerLock);
                    peersToRegister = m_peersToRegisterWith;
                }

                std::set<std::string>::iterator peer = peersToRegister.begin();

                while ((peer != peersToRegister.end()) && m_isRunning)
                {
                    std::string hostPeer(*peer);
                    ++peer;
                    registerWithPeer(hostPeer);
                }
            }

            // {
            //     // TA_Base_Core::ThreadGuard  stoppingAgentGuard(m_stoppingProcessIDToTimeCountMapLock);
            //     TA_THREADGUARD( m_stoppingProcessIDToTimeCountMapLock );
            //
            //     std::map< unsigned long, unsigned long >::iterator processIt;
            //
            //     for ( processIt = m_stoppingProcessIDToTimeCountMap.begin(); processIt != m_stoppingProcessIDToTimeCountMap.end(); )
            //     {
            //         unsigned long processID = processIt->first;
            //
            //         LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
            //             "ProcessID %lu is in stopping list with time count %d seconds", processID, processIt->second);
            //
            //         unsigned int shutdownTime = 0;
            //
            //         bool found = false;
            //
            //         EntityToProcessMap::iterator iter = m_processes.begin();
            //         for ( ; iter != m_processes.end(); ++iter )
            //         {
            //             if ( iter->second->getProcessId() == processID )
            //             {
            //                 found = true;
            //                 break;
            //             }
            //         }
            //
            //
            //         if ( !found )
            //         {
            //             processIt++;
            //             continue;
            //         }
            //
            //
            //         shutdownTime = iter->second->getShutdownTime() / 1000;
            //
            //         if ( processIt->second <= shutdownTime )
            //         {
            //             m_stoppingProcessIDToTimeCountMap[processID] = processIt->second + 2;
            //
            //             processIt++;
            //             continue;
            //         }
            //
            //
            //         // There's still a chance the processs aren't all dead yet. This is
            //         // possible if a process claimed it was terminating, and then lied. It's
            //         // also possible if the process was just restarted. So if a signal hasn't
            //         // been received for the process, SIGKILL the bastard.
            //
            //         if ( !(iter->second->hasBeenSignalled()) )
            //         {
            //             LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
            //                 "process %lu was supposed to have terminated, but hasn't. Sending a SIGKILL",
            //                 processID);
            //             iter->second->killProcess();
            //             iter->second->setProcessStatus(TA_Base_Core::Stopped);
            //
            //             m_stoppingProcessIDToTimeCountMap.erase(processIt++);
            //             LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            //                 "process %lu killed, remove from the map ", processID);
            //         }
            //         else
            //         {
            //             m_stoppingProcessIDToTimeCountMap.erase(processIt++);
            //             LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            //                 "received process %lu signal, remove from the map ", processID);
            //         }
            //
            //
            //     }
            // }

            for (size_t i = 0, STEP = 100; i < 2000 && m_isRunning; i += STEP)
            {
                sleep(STEP);
            }
        }
    }

    //
    // terminate
    //
    void ProcessManager::terminate()
    {
        m_isRunning = false;
    }

    //
    // getHostName
    //
    std::string ProcessManager::getHostName()
    {
        return m_hostname;
    }

    //
    // getServerEntityData
    //
    TA_Base_Core::ServerEntityDataPtr ProcessManager::getServerEntityData() const
    {
        return m_serverEntityData;
    }

    //
    // terminateCodeToString
    //
    const char* ProcessManager::terminateCodeToString(TA_Base_Core::EManagedProcessTerminateCode code)
    {
        const char* s[] =
        {
            "Terminate requested",
            "Initialisation error",
            "Communication error",
            "User exit",
            "Dependency Failure",
            "Timeout Error",
            "Fatal Error",
            "Access Denied",
            "NoActive Session",
            "Rights Error",
            "Config Error",
            "NotKnown"
        };

        return s[code];
    }

    //
    // registerWithPeer
    //
    void ProcessManager::registerWithPeer(const std::string& host)
    {
        FUNCTION_ENTRY("registerWithPeer");
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer - begin");

        // Second need to contact the peer's process observer(ProcessManager)

        try
        {
            // Attempt to resolve an IProcessSubjectCorbaDef object
            // Create a string in the form:
            // "corbaloc::hostname:port/PeerProcessManager"
            std::string corbaloc = str(boost::format("corbaloc::%s:%d/ProcessManager") % host % Utilities::getMgrPort(host));
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer, %s", corbaloc.c_str());
            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject(corbaloc);

            if (!CORBA::is_nil(obj))
            {
                TA_Base_Core::IProcessObserverCorbaDef_var observer = TA_Base_Core::IProcessObserverCorbaDef::_narrow(obj);
                m_peerProcessManager->addProcessObserver(host, observer);
            }
        }
        catch (CORBA::SystemException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", str(boost::format("System exception occurred while registering with peer: %s - %s") % host % CorbaUtil::exceptionToString(e)));
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", str(boost::format("Error occurred narrowing %s ProcessManager") % host));
        }

        if (CORBA::is_nil(m_peerProcessManager->getProcessObserver(host)))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer - end");
            return;
        }

        // Second need to contact the peer's PeerProcessManager.

        try
        {
            // Attempt to resolve an IProcessSubjectCorbaDef object
            // Create a string in the form:
            // "corbaloc::hostname:port/PeerProcessManager"
            std::string corbaloc = str(boost::format("corbaloc::%s:%d/PeerProcessManager") % host % Utilities::getMgrPort(host));
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer, %s", corbaloc.c_str());
            CORBA::Object_var obj = CorbaUtil::getInstance().stringToObject(corbaloc);

            if (!CORBA::is_nil(obj))
            {
                TA_Base_Core::IProcessSubjectCorbaDef_var peer = TA_Base_Core::IProcessSubjectCorbaDef::_narrow(obj);
                TA_Base_Core::ThreadGuard guard(m_peerLock);
                m_peers[host] = peer;
            }
        }
        catch (CORBA::SystemException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", str(boost::format("System exception occurred while registering with peer: %s - %s") % host % CorbaUtil::exceptionToString(e)));
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", str(boost::format("Error occurred narrowing %s PeerProcessManager") % host));
        }

        TA_Base_Core::IProcessSubjectCorbaDef_var peer;
        {
            TA_Base_Core::ThreadGuard guard(m_peerLock);
            ProcessSubjectCorbaDefMap::iterator iter = m_peers.find(host);

            if (iter == m_peers.end() || CORBA::is_nil(iter->second))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s's PeerProcessManager is nil", host.c_str());
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer - end");
                return;
            }
            else
            {
                peer = iter->second;
            }
        }
        // Now register with the peer
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Registering with peer %s", host.c_str());

        try
        {
            // Register lock not needed - this method is called from run().

            // Attempt to register with the peer
            //TA_Base_Core::IProcessObserverCorbaDef_ptr observer
            TA_Base_Core::IProcessObserverCorbaDef_var observer = peer->registerProcessObserver(m_hostname.c_str(), m_this);

            // The peer has returned a pointer to itself, so register on its behalf.
            m_peerProcessManager->registerPeer(host.c_str(), observer);

            // Registering successful - delete this peer from set.

            // gongzong++ CL-21238. add a lock here.
            //m_peersToRegisterWith.erase(host);
            {
                TA_Base_Core::ThreadGuard guard(m_registerLock);
                m_peersToRegisterWith.erase(host);
            }
            // ++gongzong
        }
        catch (CORBA::SystemException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", str(boost::format("System exception occurred while registering with peer: %s - %s") % host % CorbaUtil::exceptionToString(e)));
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithPeer - end");
        FUNCTION_EXIT;
    }

    //
    // handleProcessDeath
    //
    void ProcessManager::handleProcessDeath(const std::string& entityName)
    {
        FUNCTION_ENTRY("handleProcessDeath");

        EntityToProcessMap::iterator processIter = m_processes.find(entityName);

        if (processIter == m_processes.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Received signal for unknown entity %s", entityName.c_str());
        }
        else
        {
            ProcessInfo* process = dynamic_cast<ProcessInfo*>(processIter->second);

            if (process != NULL)
            {
                // If there was a request from an operator to terminate the process, the System
                // Controller is shutting down, or there's a config error with the agent, then
                // the signal should be ignored. Otherwise, the process should be restarted.
                if (process->getTerminateStatus() != TA_Base_Core::RequestedTerminate
                        && process->getTerminateStatus() != TA_Base_Core::UserExit
                        && process->getTerminateStatus() != TA_Base_Core::FatalError)
                {
                    process->handleProcessFailure(TA_Base_Core::FatalError);
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "process is NULL for entity %s", entityName.c_str());
            }
        }

        FUNCTION_EXIT;
    }

    //
    // getProcess
    //
    ProcessInfo& ProcessManager::getProcess(TA_Base_Core::ProcessId processId) const
    {
        for (EntityToProcessMap::const_iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            if (iter->second->getProcessId() == processId)
            {
                return *(iter->second);
            }
        }

        TA_THROW(TA_Base_Core::SystemControllerException(str(boost::format("Process with PID %d not found.") % processId)));
    }

    //
    // startProcess
    //
    void ProcessManager::startProcess(const std::string& entityName)
    {
        EntityToProcessMap::iterator iter = m_processes.find(entityName);

        if (iter != m_processes.end())
        {
            if (iter->second->getProcessStatus() == TA_Base_Core::Stopped)
            {
                iter->second->startProcess();
                // Don't need to notify peers yet - that'll be done
                // when the process registers.
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to start process %s that is not managed by the System Controller", entityName.c_str());
        }
    }

    //
    // stopProcess
    //
    void ProcessManager::stopProcess(const std::string& entityName)
    {
        FUNCTION_ENTRY("stopProcess");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "want to stop agent %s", entityName.c_str());

        EntityToProcessMap::iterator iter = m_processes.find(entityName);

        if (iter != m_processes.end())
        {
            unsigned long processID = iter->second->getProcessId();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "processID %lu is asked to stop.", processID);

            iter->second->stopProcess(TA_Base_Core::RequestedTerminate);

            // save to the map, check it in the run thread.

            // // TA_Base_Core::ThreadGuard  stoppingAgentGuard(m_stoppingProcessIDToTimeCountMapLock);
            // TA_THREADGUARD( m_stoppingProcessIDToTimeCountMapLock );
            //
            // std::map< unsigned long, unsigned long >::iterator  posIt = m_stoppingProcessIDToTimeCountMap.find(processID);
            //
            // if ( posIt == m_stoppingProcessIDToTimeCountMap.end() )
            // {
            //     // the initial value for time count is 0.
            //
            //     m_stoppingProcessIDToTimeCountMap[processID] = 0;
            //     LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            //     "processID %lu is put into stopping list",
            //     processID);
            // }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to stop process %s that is not managed by the System Controller", entityName.c_str());
        }

        FUNCTION_EXIT;
    }

    //
    // signalReceived
    //
    void ProcessManager::signalReceived(const std::string& entityName)
    {
        FUNCTION_ENTRY("signalReceived");

        EntityToProcessMap::iterator processIter = m_processes.find(entityName);

        if (processIter == m_processes.end())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Received signal for unknown entity %s", entityName.c_str());
        }
        else
        {
            ProcessInfo* process = processIter->second;
            process->signalReceived();
        }

        FUNCTION_EXIT;
    }

    //
    // changeAgentOperationMode
    //
    void ProcessManager::changeAgentOperationMode(const std::string& entityName, TA_Base_Core::EOperationMode operationMode)
    {
        EntityToProcessMap::iterator iter = m_processes.find(entityName);

        if (iter != m_processes.end())
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                // First check if we're changing the operation mode of an agent
                // to Monitor when there is no peer to switch to Control. This
                // should not be allowed.
                if (operationMode == TA_Base_Core::Monitor && !(process->isPeerRunning()))
                {
                    TA_THROW(TA_Base_Core::SystemControllerException("There is no System Controller running a peer for this agent."));
                }

                if (process->isServiceProcess())
                {
                    TA_THROW(TA_Base_Core::SystemControllerException("The operation mode of a service cannot be changed"));
                }

                if (operationMode == TA_Base_Core::Standby)
                {
                    process->setStatusToChangeTo(TA_Base_Core::RunningStandby);
                    process->GroupStartup(true);
                }
                else if (operationMode == TA_Base_Core::Control)
                {
                    process->setStatusToChangeTo(TA_Base_Core::RunningControl);
                }
                else
                {
                    process->setStatusToChangeTo(TA_Base_Core::RunningMonitor);

                    // gongzong++ CL-21328
                    //if (process->getNormalOperationMode() == TA_Base_Core::Monitor)
                    //{
                    //    // Close the FailoverOccurred alarm.
                    //    process->closeFailoverAlarm();
                    //}
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to change operation mode of non-managed process %s", entityName.c_str());
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to change operation mode of process %s that is not managed by the System Controller", entityName.c_str());
        }
    }

    //
    // setRunParam
    //
    void ProcessManager::setRunParam(const std::string& entityName, const TA_Base_Core::RunParam& param)
    {
        if (entityName.empty() || boost::iequals(entityName, "SystemController"))
        {
            TA_Base_Core::RunParams::getInstance().set(param.name.in(), param.value.in());
            return;
        }

        EntityToProcessMap::iterator iter = m_processes.find(entityName);

        if (iter != m_processes.end())
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                process->setParam(param);
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to set run param on non-managed process %s", entityName.c_str());
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Attempting to set run param on process %s that is not managed by the System Controller", entityName.c_str());
        }
    }

    //
    // getCommonParams
    //
    TA_Base_Core::RunParams::ParamVector ProcessManager::getCommonParams()
    {
        FUNCTION_ENTRY("getCommonParams");

        TA_Base_Core::RunParams::ParamVector params;
        TA_Base_Core::RunParams::ParamNameValue pnv;

        pnv.name = RPARAM_GROUPSOFFLINE;
        pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
        params.push_back(pnv);

        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGMAXFILES))
        {
            pnv.name = RPARAM_DEBUGMAXFILES;
            pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);
            params.push_back(pnv);
        }

        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DEBUGFILEMAXSIZE))
        {
            pnv.name = RPARAM_DEBUGFILEMAXSIZE;
            pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);
            params.push_back(pnv);
        }

        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_RESOLVE_TIMEOUT_MS))
        {
            pnv.name = RPARAM_RESOLVE_TIMEOUT_MS;
            pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_RESOLVE_TIMEOUT_MS);
            params.push_back(pnv);
        }

        // add monitored database status

        MoniteredDbs::iterator itr;

        for (itr = m_moniteredDatabases.begin(); itr != m_moniteredDatabases.end(); itr++)
        {
            std::string dbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(itr->first);

            if (TA_Base_Core::RunParams::getInstance().isSet(dbName.c_str()))
            {
                pnv.name = dbName;
                pnv.value = TA_Base_Core::RunParams::getInstance().get(dbName.c_str());
                params.push_back(pnv);
            }
        }

        FUNCTION_EXIT;
        return params;
    }

    //
    // getEntityNameOfProcess
    //
    std::string ProcessManager::getEntityNameOfProcess(TA_Base_Core::ProcessId processId) const
    {
        for (EntityToProcessMap::const_iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            if (iter->second != NULL && iter->second->getProcessId() == processId)
            {
                return iter->first;
            }
        }

        // PID not found.
        TA_THROW(TA_Base_Core::SystemControllerException(str(boost::format("No entity with processId %d found.") % processId)));
    }

    //
    // terminateAllMoniters
    //
    void ProcessManager::terminateAllDatabaseMonitors()
    {
        FUNCTION_ENTRY("terminateAllMonitors");

        BOOST_FOREACH(MoniteredDbs::value_type& v, m_moniteredDatabases)
        {
            v.second->terminate();
        }

        BOOST_FOREACH(MoniteredDbs::value_type& v, m_moniteredDatabases)
        {
            v.second->terminateAndWait();
        }
    }

    //
    // terminateAllProcesses
    //
    void ProcessManager::terminateAllProcesses()
    {
        FUNCTION_ENTRY("terminateAllProcesses");

        unsigned int shutdownTime = getShutdownTime();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Will be allowing processes %d milliseconds to shutdown", shutdownTime);

        EntityToProcessMap::const_iterator iter;

        // Tell the processes to stop.
        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager::terminateAllProcesses - stop process %s", iter->first.c_str());
            iter->second->stopProcess(TA_Base_Core::UserExit);
        }

        // Now wait until they are stopped.
        int nIterations = shutdownTime / 100;
        int currIteration = 0;

        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            while (iter->second->getProcessStatus() != TA_Base_Core::Stopped
                    && currIteration < nIterations)
            {
                sleep(SLEEP_TIME_MSECS);
                currIteration++;
            }

            if (currIteration >= nIterations)
            {
                // We've been waiting long enough for them to shut themselves
                // down. Break out of the for loop.
                break;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s terminated", iter->first.c_str());
        }

        // There's still a chance the processs aren't all dead yet. This is
        // possible if a process claimed it was terminating, and then lied. It's
        // also possible if the process was just restarted. So if a signal hasn't
        // been received for the process, SIGKILL the bastard.

        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            if (!(iter->second->hasBeenSignalled()))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s was supposed to have terminated, but hasn't. Sending a SIGKILL", iter->first.c_str());
                iter->second->killProcess();
                iter->second->setProcessStatus(TA_Base_Core::Stopped);
            }
        }

        FUNCTION_EXIT;
    }

    void ProcessManager::terminateAllManagedProcesses()
    {
        // Copy the process map to a temporary map, and then clear the old map.
        // This is done in case a System Manager is still connected - the normal
        // process map will be cleared and so there will no chance of dereferencing
        // null pointers.
        EntityToProcessMap temp;
        temp.swap(m_processes);

        // Terminate all of the process threads.
        for (EntityToProcessMap::iterator processIter = temp.begin(); processIter != temp.end(); processIter++)
        {
            processIter->second->terminate();
        }

        for (EntityToProcessMap::iterator processIter = temp.begin(); processIter != temp.end(); processIter++)
        {
            processIter->second->terminateAndWait();
            delete processIter->second;
            processIter->second = NULL;
        }
    }

    //
    // getShutdownTime
    //
    unsigned int ProcessManager::getShutdownTime() const
    {
        // Find the process that will take the longest to shutdown.
        unsigned int maxShutdownTime = 0;
        unsigned int currentShutdownTime;

        for (EntityToProcessMap::const_iterator iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            currentShutdownTime = iter->second->getShutdownTime();

            if (currentShutdownTime > maxShutdownTime)
            {
                maxShutdownTime = currentShutdownTime;
            }
        }

        return maxShutdownTime;
    }

    //
    // initRunParams
    //
    void ProcessManager::initRunParams()
    {
        // Set our own notify hosts run param
        m_cmdLineNotifyHosts = TA_Base_Core::RunParams::getInstance().get(RPARAM_NOTIFYHOSTS);
        std::string notifyHosts = getNotificationHosts(getServerEntityData()->getLocation());
        TA_Base_Core::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS, Utilities::mergeCsvStrings(m_cmdLineNotifyHosts, notifyHosts).c_str());

        // Set our own location key
        TA_Base_Core::RunParams::getInstance().set(RPARAM_LOCATIONKEY, getServerEntityData()->getLocation());

        // get location name for next code block
        std::string locationName = getServerEntityData()->getLocationName();

        // get the manager port

        m_port = TA_Base_Core::getRunParamValue(RPARAM_MGRPORT, TA_Base_Core::DEFAULT_MGR_PORT);

        return;

        // Retrieve the active database name
        // Wenguang++
        std::string dbConnectionStr;
        TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::SysController_Cd, TA_Base_Core::Read, dbConnectionStr);
        m_controlDbName = TA_Base_Core::DbConnectionStrings::getDatabaseName(dbConnectionStr);

        // Find the service processes that we're running.
        EntityToProcessMap::iterator iter;

        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                if (process->isServiceProcess())
                {
                    // Construct part of the notify hosts string based on which
                    // NotificationServiceAgents are running at this location,
                    // and append that to the notify hosts set on the command line.
                    std::string notifyHostsByLocation = getNotificationHosts(process->getLocationKey());
                    std::string processLocation = process->getLocationName();
                    std::string dbConnectionString = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);

                    // replace our location with this processes location if required
                    if (locationName.compare(processLocation) != 0)
                    {
                        // find last occurrence of location in case it also appears in the path
                        int loc = dbConnectionString.rfind(locationName);

                        if (loc != std::string::npos)
                        {
                            // TD15704 should not use replace if locationName doesn't have the same length
                            // as processLocation e.g. OCC_scada and R03
                            // dbConnectionString.replace(loc, locationName.size(), processLocation);
                            dbConnectionString.erase(loc, locationName.size());
                            dbConnectionString.insert(loc, processLocation);
                        }

                        // else just use what was passed in. ??ASSERT??
                    }

                    TA_Base_Core::RunParamSeq runParams;
                    runParams.length(2);
                    runParams[0].name = CORBA::string_dup(RPARAM_NOTIFYHOSTS);
                    runParams[0].value = Utilities::mergeCsvStrings(notifyHostsByLocation, m_cmdLineNotifyHosts).c_str();
                    runParams[1].name = CORBA::string_dup(RPARAM_DBCONNECTIONFILE);
                    runParams[1].value = dbConnectionString.c_str();
                    m_locationRunParams.insert(std::make_pair(process->getLocationKey(), runParams));
                }

                if (true == process->isStandbyProcess())
                {
                    TA_Base_Core::RunParamSeq runParams;
                    runParams.length(1);
                    runParams[0].name = CORBA::string_dup(RPARAM_NOTIFYHOSTS);
                    runParams[0].value = notifyHosts.c_str();
                    m_locationRunParams.insert(std::make_pair(process->getLocationKey(), runParams));
                }
            }
        }
    }

    //
    // getLocationOfHostname
    //
    unsigned long ProcessManager::getLocationOfHostname(const std::string& hostname)
    {
        TA_Base_Core::IConsole* consoleData = NULL;
        unsigned long locationKey = 0;

        try
        {
            consoleData = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress(hostname);
            locationKey = consoleData->getLocation();
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", str(boost::format("Could not retrieve the console from the database - %s") % hostname));
        }

        if (consoleData != NULL)
        {
            delete consoleData;
            consoleData = NULL;
        }

        return locationKey;
    }

    //
    // getNotificationHosts
    //
    std::string ProcessManager::getNotificationHosts(unsigned long locationKey)
    {
        FUNCTION_ENTRY("getNotificationHosts");

        static std::map<unsigned long, std::string> map = TA_Base_Core::ManagedProcessAccessFactory::getInstance().getLocation2NotifyHostsMap();
        return map[locationKey];

        std::ostringstream hostList;

        // Add all of our service processes running at the given location to
        // to the list.
        EntityToProcessMap::iterator iter;

        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                if (process->isServiceProcess() &&
                        process->getLocationKey() == locationKey)
                {
                    std::string serviceAddr = TA_Base_Core::gMakeServiceAddr(m_hostname, process->getNotifyPort());
                    LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "ProcessManager::getNotificationHosts(): Adding managed (service) process %s at location %d to NotifyHosts",
                         serviceAddr.c_str(), locationKey);

                    hostList << serviceAddr << ",";
                }
            }
        }

        // Now add all of the peers of each service process running at the
        // given location.
        for (iter = m_processes.begin(); iter != m_processes.end(); iter++)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(iter->second);

            if (process != NULL)
            {
                if (process->isServiceProcess() &&
                        process->getLocationKey() == locationKey)
                {
                    std::vector<std::string> peers = process->getPeerHosts();

                    for (std::vector<std::string>::iterator iter = peers.begin(); iter != peers.end(); iter++)
                    {
                        // Don't add ourselves again.
                        if (*iter != m_hostname)
                        {
                            std::string serviceAddr = TA_Base_Core::gMakeServiceAddr(*iter, process->getNotifyPort());
                            LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                 "ProcessManager::getNotificationHosts(): Adding peer managed (service) process %s at location %d to NotifyHosts",
                                 serviceAddr.c_str(), locationKey);

                            hostList << serviceAddr << ",";
                        }
                    }
                }
            }
        }

        std::string hostListString = hostList.str();

        if (hostListString.length() == 0)
        {
            FUNCTION_EXIT;
            return "";
        }
        else
        {
            FUNCTION_EXIT;
            // Strip the last ','
            return (hostListString.substr(0, hostListString.length() - 1));
        }
    }

    //
    // registerWithAllPeers
    //
    void ProcessManager::registerWithAllPeers()
    {
        FUNCTION_ENTRY("registerWithAllPeers");
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithAllPeers - begin");

        // Get the hostnames of my peers from the database.
        std::vector<std::string> peerHostnames = TA_Base_Core::ManagedProcessAccessFactory::getInstance().getPeersOfSystemController(m_hostname);

        BOOST_FOREACH(const std::string& peer, peerHostnames)
        {
            IWorkItemPtr item(new SimpleWorkItem(boost::bind(&ProcessManager::registerWithPeer, this, peer)));
            m_threads.queueWorkItem(item);
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "ProcessManager::registerWithAllPeers - end");
        FUNCTION_EXIT;
    }

    //
    // getInvalidNotifyHosts
    //
    std::vector<std::string> ProcessManager::getInvalidNotifyHosts(const std::string& hostname, int locationKey)
    {
        FUNCTION_ENTRY("getInvalidNotifyHosts");

        static std::map<std::pair<std::string, int>, std::vector<std::string> > cache;
        std::pair<std::string, int> key(hostname, locationKey);

        if (cache.find(key) != cache.end())
        {
            return cache[key];
        }

        std::vector<std::string> result;
        std::string agentNotifyHosts;
        TA_Base_Core::RunParamSeq& runparams = getLocationParams(locationKey);

        for (unsigned int i = 0; i < runparams.length(); i++)
        {
            if (boost::equals(RPARAM_NOTIFYHOSTS, runparams[i].name.in()))
            {
                agentNotifyHosts = runparams[i].value.in();
                break;
            }
        }

        std::set<std::string> notifyHosts;
        boost::split(notifyHosts, agentNotifyHosts, boost::is_any_of(","));

        BOOST_FOREACH(const std::string& notifyHost, notifyHosts)
        {
            if (notifyHost.find(hostname) != std::string::npos)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding %s to failed notify hosts", notifyHost.c_str());
                result.push_back(notifyHost);
            }
        }

        cache[key] = result;

        FUNCTION_EXIT;
        return result;
    }

    //
    // notifyPeers
    //
    void ProcessManager::notifyPeers(const std::string& entity)
    {
        FUNCTION_ENTRY("notifyPeers");

        EntityToProcessMap::iterator iter = m_processes.find(entity);

        if (iter != m_processes.end() && iter->second != NULL)
        {
            // Process is in map
            TA_Base_Core::ProcessData_var data = iter->second->getProcessData();

            if (m_peerProcessManager != NULL)
            {
                m_peerProcessManager->notify(data);
            }
        }
        else
        {
            // Cannot notify peers as entity is not in map.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Cannot notify peers of entity %s", entity.c_str());
        }

        FUNCTION_EXIT;
    }

    //
    // waitUntilInitialised
    //
    void ProcessManager::waitUntilInitialised()
    {
        FUNCTION_ENTRY("waitUntilInitialised");

        m_initialisedSemaphore.wait();

        FUNCTION_EXIT;
    }

    void ProcessManager::updateGroupsOfflineRunParam()
    {
        FUNCTION_ENTRY("updateGroupsOfflineRunParam");

        std::string offlineGroups("");

        // Step through all offline groups and add them into the RunParam as a comma separated list
        for (std::set<std::string>::iterator iter = m_groupsOffline.begin(); iter != m_groupsOffline.end(); ++iter)
        {
            if (iter != m_groupsOffline.begin())
            {
                offlineGroups += ",";
            }

            offlineGroups += *iter;
        }

        if (offlineGroups.empty())
        {
            offlineGroups = GROUP_ONLINE;
        }

        // Set the new run param value. This will only be passed to agents at startup so we do not
        // need to let already running agents know that this has changed.
        RunParams::getInstance().set(RPARAM_GROUPSOFFLINE, offlineGroups.c_str());

        FUNCTION_EXIT;
    }

    std::string ProcessManager::getControlDatabaseName(std::string& dbStatus)
    {
        if (!m_controlDbName.empty())
        {
            std::string dbName(RPARAM_DBPREFIX);
            dbName += m_controlDbName;

            if (RunParams::getInstance().isSet(dbName.c_str()))
            {
                dbStatus = RunParams::getInstance().get(dbName.c_str());
            }
        }

        return m_controlDbName;
    }

    std::string ProcessManager::getMonitoredDatabaseName(std::string& dbStatus)
    {
        //threadguard
        //TA_Base_Core::ThreadGuard guardDbName(m_dbNameLock);

        if (!m_monitoredDbName.empty())
        {
            std::string dbName(RPARAM_DBPREFIX);
            dbName += m_monitoredDbName;

            if (RunParams::getInstance().isSet(dbName.c_str()))
            {
                dbStatus = RunParams::getInstance().get(dbName.c_str());
            }
        }

        return m_monitoredDbName;
    }

    std::string ProcessManager::getCentralDatabaseName(std::string& dbStatus)
    {
        //threadguard
        //TA_Base_Core::ThreadGuard guardDbName(m_dbNameLock);

        if (!m_centralDbName.empty())
        {
            std::string dbName(RPARAM_DBPREFIX);
            dbName += m_centralDbName;

            if (RunParams::getInstance().isSet(dbName.c_str()))
            {
                dbStatus = RunParams::getInstance().get(dbName.c_str());
            }
        }

        return m_centralDbName;
    }

    DatabaseStatusList ProcessManager::getDatabaseStatus() const
    {
        DatabaseStatusList databaseStatusList;

        BOOST_FOREACH(const MoniteredDbs::value_type& v, m_moniteredDatabases)
        {
            std::string dbRunParam = TA_Base_Core::DbConnectionStrings::getDatabaseName(v.first);
            std::string dbStatus = TA_Base_Core::getRunParamValue(dbRunParam, RPARAM_DBUNKNOWN);
            databaseStatusList.push_back(DatabaseStatusPair(dbRunParam, dbStatus));
        }

        return databaseStatusList;
    }

    bool ProcessManager::isCentralDbOnline()
    {
        return (TA_Base_Bus::DatabaseStatusCorbaDef::statusAvailable == m_centralDbStatus);
    }

    TA_Base_Core::EOperationMode ProcessManager::handleDoubleControlBaseOnCentralDb(const char* host, CORBA::Boolean centralOnline, CORBA::ULong timeElapsedSinceOnline)
    {
        TA_Base_Core::EOperationMode result = TA_Base_Core::NotApplicable;
        bool centralOnlineLocal = isCentralDbOnline();
        unsigned long timeElapsedSinceOnlineLocal = m_centralDbOnlineTimer.elapsed_s();

        if (!centralOnline && !centralOnlineLocal)
        {
            result = TA_Base_Core::NotApplicable;
        }
        else if (!centralOnline && centralOnlineLocal)
        {
            result = TA_Base_Core::Monitor;
        }
        else if (centralOnline && !centralOnlineLocal)
        {
            result = TA_Base_Core::Control;
        }
        else if (centralOnline && centralOnlineLocal)
        {
            if (timeElapsedSinceOnline == timeElapsedSinceOnlineLocal)
            {
                result = TA_Base_Core::NotApplicable;
            }
            else
            {
                result = (timeElapsedSinceOnline < timeElapsedSinceOnlineLocal ? TA_Base_Core::Monitor : TA_Base_Core::Control);
            }
        }

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "ProcessManager::handleDoubleControlBaseOnCentralDb, "
                    "result=%s, peer-host=%s, central-db-online=%s, time-elapsed-since-online=%lu(seconds), central-db-online-local=%s, time-elapsed-since-online-local=%lu(seconds)",
                    getString(result), host, getString(centralOnline), timeElapsedSinceOnline, getString(centralOnlineLocal), timeElapsedSinceOnlineLocal);
        return result;
    }

    const char* ProcessManager::getString(TA_Base_Core::EOperationMode mode)
    {
        const char* s[] = { "Control", "Monitor", "NotApplicable", "Standby" };
        return s[mode];
    }

    const char* ProcessManager::getString(bool b)
    {
        return b ? "true" : "false";
    }

    TA_Base_Core::RunParamSeq& ProcessManager::getLocationParams(unsigned long locationKey)
    {
        static std::map<unsigned long, TA_Base_Core::RunParamSeq> cache;
        TA_Base_Core::RunParamSeq& runParams = cache[locationKey];

        if (0 == runParams.length())
        {
            runParams.length(2);
            runParams[0].name = CORBA::string_dup(RPARAM_NOTIFYHOSTS);
            runParams[0].value = Utilities::mergeCsvStrings(getNotificationHosts(locationKey), m_cmdLineNotifyHosts).c_str();
            runParams[1].name = CORBA::string_dup(RPARAM_DBCONNECTIONFILE);
            runParams[1].value = makeDbConnectionFile(locationKey).c_str();
        }

        return runParams;
    }

    std::vector<ManagedProcessInfo*> ProcessManager::getAllManagedProcessInfo()
    {
        std::vector<ManagedProcessInfo*> processes;

        BOOST_FOREACH(EntityToProcessMap::value_type& v, m_processes)
        {
            ManagedProcessInfo* process = dynamic_cast<ManagedProcessInfo*>(v.second);

            if (process)
            {
                processes.push_back(process);
            }
        }

        return processes;
    }

    std::string ProcessManager::makeDbConnectionFile(unsigned long locationKey)
    {
        static std::map<unsigned long, std::string> cache;
        std::string& dbConnectionFile = cache[locationKey];

        if (dbConnectionFile.size())
        {
            return dbConnectionFile;
        }

        dbConnectionFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);

        if (locationKey == m_serverEntityData->getLocation())
        {
            return dbConnectionFile;
        }

        static LocationMap locationMap;

        if (locationMap.empty())
        {
            m_locationMapFuture.get(locationMap);
        }

        const std::string& locationName = locationMap[locationKey];
        static const std::string localLocationName = m_serverEntityData->getLocationName();

        if (boost::starts_with(dbConnectionFile, localLocationName))
        {
            boost::replace_first(dbConnectionFile, localLocationName, locationName);
        }
        else
        {
            dbConnectionFile = locationName + "ConnectionStrings.csv";
        }

        return dbConnectionFile;
    }

    void ProcessManager::getLocationMapTask()
    {
        std::map<unsigned long, std::string> locationMap;

        try
        {
            std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

            BOOST_FOREACH(TA_Base_Core::ILocation* location, locations)
            {
                locationMap[location->getKey()] = location->getName();
            }
        }
        catch (...)
        {
        }

        m_locationMapFuture.set(locationMap);
    }
}
