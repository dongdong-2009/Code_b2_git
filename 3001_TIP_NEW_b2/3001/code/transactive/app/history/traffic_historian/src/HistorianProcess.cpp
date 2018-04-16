/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/traffic_historian/src/HistorianProcess.cpp $
  * @author Craig Grant
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: Katherine Thomson
  *
  * The HistorianProcess controls the monitoring and logging of traffic detector
  * statistics and sets up the EventConsumer thread.
  *
  */ 


// Ignore STL warnings on Windows.

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <CorbaUtil.h>
#include <DebugUtil.h>
#include "DBConnection.h"
#include "EventListener.h"
#include "HistorianProcess.h"
#include "TDMap.h"

// This object will not be deleted by the class as ownership
// is passed to a CORBA reference counted object that will do 
// the deletion.

HistorianProcess* HistorianProcess::m_instance = 0;


/////////////////////////// HistorianProcess Class ///////////////////////////


/**
 * getInstance()
 *
 * This method returns a reference to the HistorianProcess, since
 * there can only be one running at a time.
 *
 * @return A reference to the HistorianProcess.
 *
 */
HistorianProcess& HistorianProcess::getInstance()
{
    if( 0 == m_instance )
    {
        m_instance = new HistorianProcess;
    }
    return * m_instance;
}


/** 
  * HistorianProcess
  *
  * This method is the standard constructor.
  *
  */
HistorianProcess::HistorianProcess()
  : m_eventConsumer( 0 ),
    m_monitorController( 0 )
{
    // Do nothing.
}


/** 
  * ~HistorianProcess
  *
  * This method is the standard destructor.
  *
  */
HistorianProcess::~HistorianProcess()
{
    delete m_eventConsumer;
    m_eventConsumer = 0;
    delete m_monitorController;
    m_monitorController = 0;
}


/**
  * CheckDriverConfig
  *
  * This method checks that the configuration data for the Historian
  * is present and correct.
  *
  * @param EntityConfig& cfg - the config data for the Historian.
  *
  * @return bool - True if all the config data is present and correct.
  *
  */
bool HistorianProcess::CheckDriverConfig(EntityConfig& cfg)
{
    RunParams::GetInstance().Set("SubsystemName", 
                              cfg.GetParam(ENT_CFG_SUBSYSTEMNAME).c_str());
    RunParams::GetInstance().Set("TypeName", 
                              cfg.GetParam(ENT_CFG_TYPENAME).c_str());
    return true;
}


/** 
  * DriverInit
  *
  * This method initialises the application by setting up the database 
  * connection, retrieving the traffic stations from the database and 
  * registering for events.  It implements a pure virtual method 
  * from GenericDriverProcess.
  *
  * @return A bool that is true if the initialisation was successful.
  *
  */
bool HistorianProcess::DriverInit()
{
    // Instantiate our application-specific CORBA objects here

    // Test that we can get a database connection

    if( !DBConnection::Instance()->TestConnection() )
        return false;

    // Retrieve the list of traffic detectors

    if( !TDMap::GetInstance()->RetrieveAllTrafficDetectors() )
        return false;

    // Create a historian object and register it as a consumer of "Current"
    // events

    m_eventConsumer = new CEventConsumer();
    EventListener::registerForEvents(CorbaUtil::EVT_CH_CURRENT, m_eventConsumer);

    // Set up the controller that will process CORBA requests to start and
    // stop logging specific statistics.

    m_monitorController = new CMonitorController();

    return true;
}


/** 
  * DriverRun
  *
  * This method starts the EventConsumer thread running that processes received 
  * events and starts servicing CORBA requests.  It implements a pure virtual 
  * method from GenericDriverProcess.
  *
  * @return A bool that is always true.
  *
  */
bool HistorianProcess::DriverRun()
{
    // If our process were purely CORBA-driven we could just call run(), but
    // if we want to do any other work we need to spawn a thread

    m_eventConsumer->start();
    
    return true;
}


/** 
  * DriverTerminate
  *
  * This method stops the EventConsumer thread and stops servicing CORBA requests.  
  * It implements a pure virtual method from GenericDriverProcess.
  *
  */
void HistorianProcess::DriverTerminate()
{
    if( m_eventConsumer )
    {
        m_eventConsumer->terminate();
    }
}