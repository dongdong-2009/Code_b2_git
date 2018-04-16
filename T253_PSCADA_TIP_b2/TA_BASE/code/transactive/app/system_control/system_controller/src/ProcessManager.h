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

#if !defined(ProcessManager_97805EE8_D1B8_456d_842F_1B5F5E45E6DB__INCLUDED_)
#define ProcessManager_97805EE8_D1B8_456d_842F_1B5F5E45E6DB__INCLUDED_

#include "PeerProcessManager.h"
#include "ProcessInfo.h"
#include "Utilities.h"
#include "bus/database_status_monitor/src/IDatabaseStatusObserver.h"
#include "bus/database_status_monitor/DsmCorbaDef/IDL/src/DatabaseStatusCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/threads/src/Thread.h"
#include "core/threads/src/ThreadPoolManager.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/utilities/src/RunParams.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IProcessObserverCorbaDef.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/IDL/src/IProcessSubjectCorbaDef.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/data_access_interface/entity_access/src/ServerEntityData.h"
#include "core/timers/src/MonotonicTimer.h"
#include <boost/algorithm/string.hpp>
#include <ace/Future.h>
#include <ace/Singleton.h>
#include <string>
#include <map>
#include <set>
#include <ace/Singleton.h>
#include <ace/RW_Mutex.h>

namespace TA_Base_Core
{
    class ServerEntityData;            // Forward declaration
    class SingleThreadBarrier;
    class IManagedProcessData;
    typedef boost::shared_ptr<IManagedProcessData> IManagedProcessDataPtr;
    typedef std::vector<IManagedProcessDataPtr> IManagedProcessDataPtrList;
}

namespace TA_Base_Bus
{
    class SimpleDbStatusChecker;// Forward declaration
}

namespace TA_Base_App
{
    class ManagedProcessInfo;    // Forward declaration

    typedef std::pair<std::string, std::string> DatabaseStatusPair;
    typedef std::vector<DatabaseStatusPair> DatabaseStatusList;
    typedef std::map<std::string, unsigned long, StriCmp> Console2LocationMap;
    typedef std::map<std::string, TA_Base_Core::IProcessSubjectCorbaDef_var> ProcessSubjectCorbaDefMap;

    class ProcessManager
        : public virtual POA_TA_Base_Core::IProcessObserverCorbaDef,
          public virtual TA_Base_Core::Thread,
          public virtual TA_Base_Core::ServantBase,
          public TA_Base_Bus::IDatabaseStatusObserver
    {
    public:

        /**
          * getInstance
          *
          * @return The one-and-only instance of this class.
          */
        static ProcessManager& getInstance();

        /**
          * Destructor
          *
          */
        virtual ~ProcessManager();

        /**
          * getAllProcesses
          *
          * Returns process information about all of the processes that
          * this System Controller manages.
          *
          * @return A vector of ProcessData.
          *
          */
        std::vector<TA_Base_Core::ProcessData> getAllProcesses();
        void getAllProcesses(TA_Base_Core::ProcessDataSeq&);

        /**
          * getAgentStatusSummary
          *
          * Returns an enum indicating the overall status of the agents.
          *
          * @return An AgentStatusEnum
          */
        TA_Base_Core::AgentStatusEnum getAgentStatusSummary();

        /**
          * haveAllAgentsStartedAtLeastOnce
          *
          * Checks to see if all agents have either started successfully
          * (and are therefore in RunningControl or RunningMonitor) or have
          * restarted at least once.
          *
          * @return True if all agents have started at least once.
          */
        bool haveAllAgentsStartedAtLeastOnce();

        /**
          * respondToPeerSCFailure
          *
          * Allows the Process Manager to respond to a peer System Controller
          * failing.
          *
          * @param host  The hostname of the peer that has failed.
          */
        void respondToPeerSCFailure(const std::string& host);

        /**
          * respondToPeerSCFailure
          *
          * Allows the Process Manager to respond to a peer System Controller
          * recovering.
          *
          * @param host  The hostname of the peer that has recovered.
          */
        void respondtoPeerSCRestored(const std::string& host);

        /**
          * respondToGroupOffline
          *
          * Allows the Process Manager to respond to a System Controller
          * group going offline.
          *
          * @param group  The name of the group.
          */
        void respondToGroupOffline(const std::string& group);

        /**
          * respondToGroupOnline
          *
          * Allows the Process Manager to respond to a System Controller
          * group coming online.
          *
          * @param group  The name of the group.
          */
        void respondToGroupOnline(const std::string& group);

        /**
          * reregisterWithPeer
          *
          * Instructs the Process Manager to re-register with a peer.
          * Called when a problem occurs communicating with a peer that
          * was previously registered with.
          *
          * @param host  The hostname of the peer to re-register with.
          */
        void reregisterWithPeer(const std::string& host);

        /**
          * notifyProcessData
          *
          * Notifies the System Controller that process data has changed
          * on a peer.
          *
          * @param peerProcessData  The process data that has changed on
          *                         the peer.
          */
        virtual void notifyProcessData(const TA_Base_Core::ProcessData& peerProcessData);

        /**
          * terminating
          *
          * Informs the System Controller that a process is terminating.
          *
          * @param p_processID  The PID of the process that is terminating.
          * @param p_code       The reason the process is terminating.
          *
          */
        virtual void terminating(TA_Base_Core::ProcessId p_processID, TA_Base_Core::EManagedProcessTerminateCode p_code);

        /**
          * registerManagedProcess
          *
          * Registers a managed process with this System Controller.
          *
          * @param p_managedProcess    The ManagedProcessCorbaDef implemented by
          *                            the process.
          * @param p_processId         The process's PID.
          * @param applicationType     Unused by the System Controller.
          * @param entity              The process's entity name.
          *
          */
        virtual void registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess, TA_Base_Core::ProcessId p_processId, CORBA::ULong applicationType, const char* entity);

        /**
          * registerManagedApplication
          *
          * Registers an application with this System Controller.
          *
          * @param p_managedApp  The ManagedApplicationCorbaDef implemented by the
          *                      application.
          * @param p_processId   The process's PID.
          *
          */
        virtual void registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp, TA_Base_Core::ProcessId p_processId);

        /**
          * getParams
          *
          * Called by agents and the Control Station to get their runtime
          * parameters.
          *
          * @return A sequence of runtime parameters.
          *
          * @param p_processID  The PID of the process retrieving the parameters.
          * @param hostname     If the PID is 0, the process's machine name (needed
                                to retrieve the location
          */
        virtual TA_Base_Core::RunParamSeq* getParams(TA_Base_Core::ProcessId p_processID, const char* hostname);

        /**
          * poll
          *
          * Allows this System Controller to be polled by managed processes,
          * the Control Station, etc.
          *
          */
        virtual void poll();

        /**
         * statusChanged
         *
         * This is called by agents to indicate when they move between
         * states. This method is not needed by GUI's.
         *
         * @param EProcessStatus - The state the agent is now in
         * @param ProcessId - The id of the process that has changed state
         */
        virtual void statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId);

        virtual TA_Base_Core::EOperationMode handleDoubleControlBaseOnCentralDb(const char* host, CORBA::Boolean isCentrolDbOnline, CORBA::ULong timeElapsedSinceOnline);

        /**
          * run
          *
          * The starting point of this thread. Starts processes, starts the
          * heartbeats, and registers with peers.
          *
          */
        void run();

        /**
          * terminate
          *
          * Terminates the thread.
          *
          */
        void terminate();

        /**
          * getHostName
          *
          * Returns the hostname that this System Controller is running on.
          *
          * @return std::string The hostname.
          *
          */
        static std::string getHostName();

        /**
          * handleProcessDeath
          *
          * Called when a process on this System Controller is detected
          * as having died abnormally. It restarts the process in
          * Monitor mode and informs peers.
          *
          * @param entityName
          */
        void handleProcessDeath(const std::string& entityName);

        /**
          * getProcess
          *
          * Returns a ProcessInfo object for the process with
          * the given ID.
          *
          * @return A ProcessInfo object.
          *
          * @param processId  The PID.
          *
          * @exception SystemControllerExceptin if the PID is
          *            unknown.
          */
        ProcessInfo& getProcess(TA_Base_Core::ProcessId processId) const;

        /**
          * startProcess
          *
          * Starts a stopped process.
          *
          * @param entityName  The entity name of the process to start.
          */
        void startProcess(const std::string& entityName);

        /**
          * stopProcess
          *
          * Stops a process.
          *
          * @param entityName  The entity name of the process to stop.
          */
        void stopProcess(const std::string& entityName);

        /**
          * signalReceived
          *
          * Called to indicate to the ProcessManager that a signal was
          * received for this process. It will update the actual process object.
          *
          * @param entityName  The entity name of the process that a signal
          *                    was received for.
          */
        void signalReceived(const std::string& entityName);

        /**
          * changeAgentOperationMode
          *
          * Changes the operation mode of a process.
          *
          * @param entityName     The entity name of the process to change.
          * @param operationMode  The operation mode to change to.
          *
          */
        void changeAgentOperationMode(const std::string& entityName,
                                      TA_Base_Core::EOperationMode operationMode);

        /**
          * setRunParam
          *
          * Sets a runtime parameter on a process.
          *
          * @param entityName  The entity name of the process to set the param on.
          * @param param       The name-value pair describing the run param.
          */
        void setRunParam(const std::string& entityName, const TA_Base_Core::RunParam& param);

        /**
          * getCommonParams
          *
          * Gets the runtime parameters that are common among all processes,
          * control stations, system managers, etc. (Runparams specific to processes
          * are fetched from the ManagedProcessInfo class, and then appended to
          * those returned from this method.)
          *
          * @return A vector of common runparams.
          *
          */
        TA_Base_Core::RunParams::ParamVector getCommonParams();

        /**
          * getEntityNameOfProcess
          *
          * Returns the entity name of the process with the given process
          * ID.
          *
          * @return The entity name of the process.
          *
          * @param processId  The PID.
          */
        std::string getEntityNameOfProcess(TA_Base_Core::ProcessId processId) const;

        /**
          * cleanUp
          *
          * Cleans up the Process Manager by deleting allocated memory
          * and terminating all processes.
          */
        void cleanUp();

        /**
          * getDebugLogDir
          *
          * Returns the directory where log files should go.
          *
          * @return The directory for log files.
          */
        std::string getDebugLogDir() const;

        //
        // getBinDir
        //
        std::string ProcessManager::getBinDir() const;

        /**
          * getServerEntityData
          *
          * Returns the entity data (of type Server) that is associated with
          * this System Controller.
          *
          * @return The Server entity data.
          */
        TA_Base_Core::ServerEntityDataPtr getServerEntityData() const;

        /**
          * terminateCodeToString
          *
          * Converts a terminate code to an equivalent string.
          *
          * @return A string representing the terminate code.
          *
          * @param code  The terminate code to convert.
          */
        static const char* terminateCodeToString(TA_Base_Core::EManagedProcessTerminateCode code);

        /**
          * notifyPeers
          *
          * Notifies the peers of this System Controller that the
          * process with the given entity name has changed.
          *
          * @param entity  The entity name of the process that has
          *                changed.
          */
        void notifyPeers(const std::string& entity);

        /**
          * waitUntilInitialised
          *
          * Returns when the ProcessManager is fully initialsed.
          */
        void waitUntilInitialised();

        /**
          * notifyStatusChange
          *
          * called when a database changes state
          */
        void notifyStatusChange(const std::string& DbConnectionInfo, TA_Base_Bus::DatabaseStatusCorbaDef::EState status);

        /**
          * getActiveDatabaseName
          *
          * Returns the active database name.
          */
        std::string getControlDatabaseName(std::string& dbStatus);

        /**
          * getMonitoredDatabaseName
          *
          * Returns the active database name.
          */
        std::string getMonitoredDatabaseName(std::string& dbStatus);

        /**
          * getCentralDatabaseName
          *
          * Returns the active database name.
          */
        std::string getCentralDatabaseName(std::string& dbStatus);

        static const int SLEEP_TIME_MSECS;
        static const std::string GROUP_ONLINE;

        //TD16585++
        /**
          * waitForOfflineDatabase
          *
          * Performs a wait on dbstatus monitor to wait for offline database
          *
          */
        //void waitForOfflineDatabase();
        //++TD16585

        //when do SC failover, set local database status as unavailable
        void setLocalDatabseStatusOffline();

        DatabaseStatusList getDatabaseStatus() const;
        std::vector<ManagedProcessInfo*> getAllManagedProcessInfo();
        TA_Base_Core::RunParamSeq& getLocationParams(unsigned long locationKey);

    protected:

        // Made private as this is a singleton
        ProcessManager();
        ProcessManager(const ProcessManager& ProcessManager);
        ProcessManager& operator=(const ProcessManager&);
        friend class ACE_Singleton<ProcessManager, ACE_Recursive_Thread_Mutex>;

        /**
          * setupDbMonitors
          *
          * Fetches and processes the database connection data.
          *
          * @param pollPeriod       Populated with the time, in milliseconds, between polls to
          *                         each process.
          *
          * @exception FatalConfigurationException  Thrown if required run param not set
          * @exception DbConnectionNoFile           Thrown if the database config file
          *                                         can not be opened
          */
        void setupDbMonitors(int& pollPeriod);

        /**
          * setupDbMonitors
          *
          * Fetches all consoles and their location keys.
          *
          */
        void setupConsoleLocationMap();

        /**
          * setupSystemControllerConfiguration
          *
          * Fetches and processes the System Controller's configuration
          * data.
          *
          * @param pollPeriod       Populated with the time, in milliseconds, between polls to
          *                         each process.
          * @param responseTimeout  Populated with the time each agent has to respond to state
          *                         change requests.
          *
          * @exception SystemControllerException  Thrown if there are problems reading the
          *                                       config data from the database.
          */
        void setupSystemControllerConfiguration(int& pollPeriod, int& responseTimeout);

        /**
          * setupProcessConfiguration
          *
          * Fetches and processes the process configuration for this System Controller.
          * Either a ProcessInfo or ManagedProcessInfo object is created for each process
          * that is configured to be run by this System Controller.
          *
          * @param pollPeriod       The time, in milliseconds, between polls to each process.
          * @param responseTimeout  The time each agent has to respond to state change requests.
          *
          * @exception SystemControllerException  Thrown if there are problems reading the
          *                                       config data from the database.
          */
        void setupProcessConfiguration(int pollPeriod, int responseTimeout);
        void createManagedProcessInfo(TA_Base_Core::IManagedProcessDataPtr managedProcessData,
                                      TA_Base_Core::IEntityDataPtr parentData,
                                      TA_Base_Core::IEntityDataList childrenData,
                                      int pollPeriod,
                                      int responseTimeout);

        /**
          * registerWithPeer
          *
          * Instructs the System Controller to register as an observer
          * with a peer. Called when a peer has been detected by the
          * SystemControllerMonitor.
          *
          * @param host The peer's hostname.
          */
        void registerWithPeer(const std::string& host);

        /**
          * terminateAllDatabaseMonitors
          *
          * This function terminates all database monitors. Called when
          * the System Controller must shutdown.
          *
          */
        void terminateAllDatabaseMonitors();

        /**
          * terminateAllProcesses
          *
          * This function terminates all processes. Called when the System
          * Controller must shutdown.
          *
          */
        void terminateAllProcesses();
        void terminateAllManagedProcesses();
        void deregisterProcessObserver();

        /**
          * getShutdownTime
          *
          * Gets the total amount of time, in milliseconds, to wait for all
          * agents to shutdown. This is equal to the shutdown time of the
          * slowest process.
          *
          * @return  The amount of time to wait for agents to die.
          */
        unsigned int getShutdownTime() const;

        /**
          * initRunParams
          *
          * Sets the run params that are common to more than one process.
          * Run params that are unique to a location are stored in
          * m_locationRunParams, while run params that are common among
          * locations are set as normal using the RunParams class.
          */
        void initRunParams();

        /**
          * getLocationOfHostname
          *
          * Returns the location key of the given hostname.
          *
          * @param hostname  The hostname of the desired location.
          *
          * @return The location key as an unsigned long.
          */
        unsigned long getLocationOfHostname(const std::string& hostname);

        /**
          * getNotificationHosts
          *
          * Returns a comma separated list of notification hosts running
          * at the given location.
          *
          * @param locationKey The location's key.
          *
          * @return A comma separated list.
          */
        std::string getNotificationHosts(unsigned long locationKey);

        /**
          * registerWithAllPeers
          *
          * Attempts to register with all peers. This is called when the System
          * Controller first starts up.
          */
        void registerWithAllPeers();

        /**
          * getInvalidNotifyHosts
          *
          * Returns the notify hosts that have been invalidated for an agent,
          * given that a peer has failed. This is determined by searching
          * through the notify hosts that were supplied to an agent and finding
          * all of the notify hosts that have the failed server's hostname in it.
          *
          * @param hostname     The hostname of the failed peer.
          * @param locationKey  The location key of the agent that has had
          *                     notify hosts invalidated.
          *
          * @return A vector of notify hosts strings, including port numbers.
          */
        std::vector<std::string> getInvalidNotifyHosts(const std::string& hostname, int locationKey);

        /**
          * handleTwoAgentsInControl
          *
          * Called when it is detected that there are two peer agents
          * that are both in Control mode. This will either tell the local
          * agent to switch to Monitor mode or tell the peer System Controller
          * to change the mode of its agent.
          *
          * @param agent  The agent that is in Control and has its peer in Control.
          */
        void handleTwoAgentsInControl(ManagedProcessInfo* agent, const std::string& peerHost);

        /**
          * handleTwoAgentsInMonitor
          *
          * Called when it is detected that there are two peer agents
          * that are both in Monitor mode. This will either tell the local
          * agent to switch to Control mode or tell the peer System Controller
          * to change the mode of its agent.
          *
          * @param agent  The agent that is in Monitor and has its peer in Monitor.
          */
        void handleTwoAgentsInMonitor(ManagedProcessInfo* agent);

        /**
          * updatePeersState
          *
          * Updates a ManagedProcessInfo object with its peer's state.
          *
          * @param agent            The ManagedProcessInfo object to update.
          * @param peerProcessData  The process update received from the peer.
          */
        void updatePeersState(ManagedProcessInfo* agent,
                              const TA_Base_Core::ProcessData& peerProcessData);

        /**
         * updateGroupsOfflineRunParam
         *
         * TD6972 This call sets a run parameter so that agents started in the future will be
         * notified of the group failure. This updates the RunParam RPARAM_GROUPSOFFLINE with a comma
         * separated list of all the groups in m_groupsOffline.
         */
        void updateGroupsOfflineRunParam();

        bool isCentralDbOnline();

        static const char* getString(TA_Base_Core::EOperationMode mode);
        static const char* getString(bool);
        void handlePeerProcessData(ManagedProcessInfo* process, const TA_Base_Core::ProcessData& peerProcessData);
        std::map<std::string, std::vector<TA_Base_Core::ProcessData> > m_peerProcessDataCache;
        std::string makeDbConnectionFile(unsigned long locationKey);
        void getLocationMapTask();

        /**
          * The name of the host that this System Controller is running on.
          */
        static std::string m_hostname;

        typedef std::map<std::string, ProcessInfo*> EntityToProcessMap;

        /**
          * A map of entity names to process information.
          */
        EntityToProcessMap m_processes;

        /**
          * A map of hostnames to the peers that this System Controller
          * is observing.
          */
        ProcessSubjectCorbaDefMap m_peers;

        /**
          * A set of peers that we have received a heartbeat from, but we
          * have not registered with yet.
          */
        std::set<std::string> m_peersToRegisterWith;

        /**
          * A map to store the run params specific to a location. The key
          * is the location key.
          */
        std::map<unsigned long, TA_Base_Core::RunParamSeq> m_locationRunParams;

        /**
          * The Peer Process Manager that will allow other System Controllers to
          * observe us.
          */
        PeerProcessManager* m_peerProcessManager;

        /**
          * The directory where log files will go.
          */
        std::string m_debugLogDir;

        /**
          * The directory where agent bins are found
          */
        std::string m_binDir;

        /**
          * Data about this System Controller, as stored in the
          * entity table under the Server type.
          */
        TA_Base_Core::ServerEntityDataPtr m_serverEntityData;

        /**
          * The message sender object used to send notifications that
          * a group is online or offline.
          */
        TA_Base_Core::CommsMessageSender* m_commsSender;

        /**
          * Used to synchronise access to the peer map.
          */
        TA_Base_Core::NonReEntrantThreadLockable m_peerLock;

        /**
          * Used to synchronise access to the set of peers to register with.
          */
        TA_Base_Core::NonReEntrantThreadLockable m_registerLock;

        /**
          * Used to lock dbname
          */
        TA_Base_Core::ReEntrantThreadLockable m_dbNameLock;

        /**
          * True if the thread is running, and false otherwise.
          */
        bool volatile m_isRunning;

        /**
          * Signalled when the ProcessManager is fully initialsed, and is
          * ready to receive register requests.
          */
        TA_Base_Core::Semaphore m_initialisedSemaphore;

        /**
          * Signalled when the ProcessManager has detected an active database
          */
        TA_Base_Core::Semaphore m_initialisedDBStatusMonitor;

        /*//TD16585++
        TA_Base_Core::Condition m_timedWaitForOfflineDataBase;
        bool m_isWaitingForOfflineDatabase;
        static const unsigned long POLL_WAIT_PERIOD_IN_MS;
        //++TD16585*/

        /**
          * true if at startup
          */
        bool volatile m_isStartup;

        /**
         * This contains the notify hosts parameters specified on the command line
         */
        std::string m_cmdLineNotifyHosts;

        /**
         * TD6972
         * Set of the names of offline groups.
         *
         * Used to inform agents not presently able to communicate
         * with that a group is offline.
         */
        std::set<std::string> m_groupsOffline;

        /**
         * Hold a list of connection strings for every database we have to monitor.
         * Must be unique, thus the map.
         */
        typedef std::map<std::string, TA_Base_Bus::SimpleDbStatusChecker*, StriCmp> MoniteredDbs;
        MoniteredDbs m_moniteredDatabases;

        /**
         * Hold a list of all consoles and their location keys.
         */
        Console2LocationMap m_console2LocationMap;
        ACE_Future<Console2LocationMap> m_console2LocationMapFuture;

        typedef std::map<unsigned long, std::string> LocationMap;
        ACE_Future<LocationMap> m_locationMapFuture;

        /**
          * Used to lock m_stoppingAgentToTimeoutMap
          */
        //TA_Base_Core::ReEntrantThreadLockable m_stoppingProcessIDToTimeCountMapLock;

        /*
        * used to save the stopping agents list.
        */
        //std::map< unsigned long, unsigned long > m_stoppingProcessIDToTimeCountMap;

        /**
          * The port to listen on.
          */
        int m_port;

        // Wenguang++
        std::string m_controlDbName;
        std::string m_monitoredDbName;
        std::string m_centralDbName;
        TA_Base_Core::MonotonicTimer m_centralDbOnlineTimer;
        TA_Base_Bus::DatabaseStatusCorbaDef::EState m_centralDbStatus;
        TA_Base_Core::IProcessObserverCorbaDef_var m_this;
        TA_Base_Core::ThreadPoolManager m_threads;
    };
} // namespace TA_Base_App

#endif // !defined(ProcessManager_97805EE8_D1B8_456d_842F_1B5F5E45E6DB__INCLUDED_)
