/**
  * The source code in this file is the property of
  * MI Services Group (Australia) and is not for
  * redistribution in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/history/traffic_historian/src/HistorianProcess.h $
  * @author Craig Grant
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: Katherine Thomson
  *
  * The HistorianProcess controls the monitoring and logging of traffic detector
  * statistics and sets up the EventConsumer thread.
  *
  */


#ifndef HISTORIAN_PROCESS_H
#define HISTORIAN_PROCESS_H

#include "GenericDriverProcess.h"
#include "EntityConfig.h"
#include "EventConsumer.h"
#include "MonitorController.h"

class HistorianProcess : public GenericDriverProcess
{
public:
    
    /**
     * getInstance()
     *
     * This method returns a reference to the HistorianProcess, since
     * there can only be one running at a time.
     *
     * @return A reference to the HistorianProcess.
     *
     */
    static HistorianProcess& getInstance();


    /** 
      * ~HistorianProcess
      *
      * This method is the standard destructor.
      *
      */
    virtual ~HistorianProcess();


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
	virtual bool CheckDriverConfig(EntityConfig& cfg);

    
    /**
     * CreateEntity()
     *
     * This method returns null because the Historian does not have any children.
     *
     * @param The configuration data for the entity.
     *
     * @return NULL.
     *
     */
    virtual IEntity* CreateEntity(EntityConfig& cfg) { return NULL; };

protected:
    
    // Methods to satisfy the pure virtuals in GenericDriverProcess
    
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
    virtual bool DriverInit();

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
    virtual bool DriverRun();

    /** 
      * DriverTerminate
      *
      * This method stops the EventConsumer thread and stops servicing CORBA requests.  
      * It implements a pure virtual method from GenericDriverProcess.
      *
      */
    virtual void DriverTerminate();

private:

    /** 
      * HistorianProcess
      *
      * This method is the standard constructor.
      *
      */
    HistorianProcess();

    // A pointer to the Historian process. This enforces the rule
    // that only one Historian process can be used at a time.

    static HistorianProcess * m_instance;

    // The EventConsumer processes received traffic detector events.

	CEventConsumer* m_eventConsumer;
    
    // The CMonitorController processes CORBA requests to start and
    // stop logging specific statistics for the traffic detectors.

    CMonitorController* m_monitorController;
};

#endif // !defined( HISTORIAN_PROCESS_H )
