/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "app/trains/train_agent/src/AlarmUtility.h"
#include "app/trains/train_agent/src/AtsAgentInterface.h"
#include "app/trains/train_agent/src/AtsAgentObserver.h"
#include "app/trains/train_agent/src/AtsMessageProcessor.h"
#include "app/trains/train_agent/src/AuditSender.h"
#include "app/trains/train_agent/src/CctvAgentInterface.h"
#include "app/trains/train_agent/src/DepotOperationModeManager.h"
#include "app/trains/train_agent/src/DepotTimsCctvManager.h"
#include "app/trains/train_agent/src/DutyAgentInterface.h"
#include "app/trains/train_agent/src/DutyAgentObserver.h"
#include "app/trains/train_agent/src/MessageSender.h"
#include "app/trains/train_agent/src/OccOperationModeManager.h"
#include "app/trains/train_agent/src/OperationModeManager.h"
#include "app/trains/train_agent/src/RadioAgentInterface.h"
#include "app/trains/train_agent/src/RadioAgentObserver.h"
#include "app/trains/train_agent/src/RadioGroupCoordinator.h"
#include "app/trains/train_agent/src/RadioMessageProcessor.h"
#include "app/trains/train_agent/src/RightsChecker.h"
#include "app/trains/train_agent/src/StateUpdateManager.h"
#include "app/trains/train_agent/src/StationOperationModeManager.h"
#include "app/trains/train_agent/src/TimsCctvManager.h"
#include "app/trains/train_agent/src/TimsDriverCallManager.h"
#include "app/trains/train_agent/src/TimsModeManager.h"
#include "app/trains/train_agent/src/TimsPaManager.h"
#include "app/trains/train_agent/src/TimsPecManager.h"
#include "app/trains/train_agent/src/TimsStatusManager.h"
#include "app/trains/train_agent/src/TimsTisManager.h"
#include "app/trains/train_agent/src/TrainAgent.h"
#include "app/trains/train_agent/src/TrainAgentConfiguration.h"
#include "app/trains/train_agent/src/TrainAgentStatusManager.h"
#include "app/trains/train_agent/src/TrainCctvCorbaServant.h"
#include "app/trains/train_agent/src/TrainDriverCallCorbaServant.h"
#include "app/trains/train_agent/src/TrainInformation.h"
#include "app/trains/train_agent/src/TrainInformationCorbaServant.h"
#include "app/trains/train_agent/src/TrainOperationModeCorbaServant.h"
#include "app/trains/train_agent/src/TrainPaCorbaServant.h"
#include "app/trains/train_agent/src/TrainPecCorbaServant.h"
#include "app/trains/train_agent/src/TrainRadioGroupCorbaServant.h"
#include "app/trains/train_agent/src/TrainStateUpdateCorbaServant.h"
#include "app/trains/train_agent/src/TrainTisCorbaServant.h"

#include "bus/security/rights_library/src/CtaRights.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainTransactionModel/src/TrainTransactionManager.h"

#include "core/utilities/src/RunParams.h"
#include "core/alarm/src/CtaAlarms.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/message/src/CtaAuditing.h"
#include "core/message/types/TrainAudit_MessageTypes.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/threads/src/ThreadMemberFunction.h"

#include "app/trains/train_agent/src/RadioAccess.h"
using TA_Base_Core::StateUpdateMessageCorbaDef;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{

    TrainAgent::TrainAgent(int argc, char* argv[])
        : m_genericAgent( NULL ),
		  m_firstRunMode( true ),
          m_threadPool( *TA_Base_Core::ThreadPoolSingletonManager::getInstance() ),
          m_configuration( NULL ),
          m_messageSender( NULL ),
          m_ctaRights( NULL ),
          m_rightsChecker( NULL ),
          m_ctaAuditing( NULL ),
          m_auditSender( NULL ),
          m_ctaAlarms( NULL ),
          m_alarmUtility( NULL ),
          m_trainAgentStatusManager( NULL ),
          m_stateUpdateManager( NULL ),
          m_atsAgentInterface( NULL ),
          m_atsMessageProcessor( NULL ),
          m_dutyAgentInterface( NULL ),
          m_cctvAgentInterface( NULL ),
          m_radioAgentInterface( NULL ),
          m_trainTransactionManager( NULL ),
          m_radioMessageProcessor( NULL ),
          m_operationModeManager( NULL ),
          m_radioGroupCoordinator( NULL ),
          m_trainInformation( NULL ),
          m_timsModeManager( NULL ),
          m_timsStatusManager( NULL ),
          m_timsCctvManager( NULL ),
          m_timsPecManager( NULL ),
          m_timsDriverCallManager( NULL ),
          m_timsTisManager( NULL ),
          m_timsPaManager( NULL ),
          m_trainOperationModeCorbaServant( NULL ),
          m_trainStateUpdateCorbaServant( NULL ),
          m_trainRadioGroupCorbaServant( NULL ),
          m_trainInformationCorbaServant( NULL ),
          m_trainCctvCorbaServant( NULL ),
          m_trainPecCorbaServant( NULL ),
          m_trainDriverCallCorbaServant( NULL ),
          m_trainTisCorbaServant( NULL ),
          m_trainPaCorbaServant( NULL ),
          m_dutyAgentObserver( NULL ),
          m_atsAgentObserver( NULL ),
          m_radioAgentObserver( NULL )
    {
        FUNCTION_ENTRY( "TrainAgent" );

		m_genericAgent = new TA_Base_Bus::GenericAgent(argc,argv,this);
		TA_ASSERT( NULL != m_genericAgent, "m_genericAgent == NULL" );
		unsigned long nRadioInterval = 1;
		std::map< int, unsigned long> paLiveResources;
		m_radioAccess = new RadioAccess(paLiveResources, nRadioInterval);
        // initialise all pointer members to NULL in the initialiser list
        // get an instance of the threadpool in the initialiser list
        // using boost::dynamic_pointer_cast, cast m_agentEntityData to a TrainAgentEntityDataPtr
        TA_Base_Core::TrainAgentEntityDataPtr trainAgentEntityData =
            boost::dynamic_pointer_cast< TA_Base_Core::TrainAgentEntityData >(m_genericAgent->getAgentEntityConfiguration());
		
        // If the cast fails, throw a FatalConfigurationException
        if ( !trainAgentEntityData )
        {
            TA_THROW( TA_Base_Core::FatalConfigurationException( "Failed Cast to TrainAgentEntityDataPtr" ) );
        }
		
        // use this entity data to pass on to the required methods below
		
        try
        {
            // createGroupOne( trainAgentEntityData )
            createGroupOne( trainAgentEntityData );
			
            // createGroupTwo
            createGroupTwo();
			
            // createGroupThree
            createGroupThree();
			
            // createGroupFour
            createGroupFour();
			
            // createGroupFive
            createGroupFive();
			
            // createGroupSix( trainAgentEntityData )
            createGroupSix( trainAgentEntityData );
			
            // createGroupSeven
            createGroupSeven();
			
            // createGroupEight
            createGroupEight();
        }
        // catch any FatalConfigurationException
        catch ( TA_Base_Core::FatalConfigurationException& e )
        {
            // log the exception
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::FatalConfigurationException", e.what() );
			
            // call terminateWithoutRestart
            m_genericAgent->terminateWithoutRestart();
        }

        FUNCTION_EXIT;
    }

		  RadioAccess* TrainAgent::getRadioAccess()
		  {
			  return m_radioAccess;
		  }
    TrainAgent::~TrainAgent()
    {
        FUNCTION_ENTRY( "~TrainAgent" );

        // remove the instance of the threadpool
        TA_Base_Core::ThreadPoolSingletonManager::removeInstance( &m_threadPool );

        FUNCTION_EXIT;
    }


    bool TrainAgent::createAllEntities( const TA_Base_Core::SharedIEntityDataList& entityDataList,
                                        TA_Base_Bus::IEntityList& createdEntities )
    {
        FUNCTION_ENTRY( "createAllEntities" );

        // The train agent should have no entites.

        // However, iterate through the list of entites and log the name and type of each entity
        for ( TA_Base_Core::SharedIEntityDataList::const_iterator iter = entityDataList.begin();
              iter != entityDataList.end(); ++iter )
        {
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         "The train agent should have no entities, the entity [%d] must be wrong",
                         ( *iter )->getKey() );
        }

        FUNCTION_EXIT;
        return true;
    }


    TA_Base_Bus::IEntity* TrainAgent::createEntity( TA_Base_Core::IEntityDataPtr entityData )
    {
        FUNCTION_ENTRY( "createEntity" );

        // do nothing

        FUNCTION_EXIT;
        return NULL;
    }

	/**
	* agentSetMonitor
	*
	* This method is called when the agent is placed into monitor mode. It is 
	* responsible for making any necessary state changes in the agent to place it in
	* monitor mode.
	*/
	void TrainAgent::agentSetMonitor()
	{
		FUNCTION_ENTRY( "agentSetMonitor" );

		m_operationModeManager->agentSetMonitor();
//		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        // This will terminate all train transations in progress.
        // use m_trainTransactionManager.cancelTransactions (with both lists empty)
        m_trainTransactionManager->cancelTransactions( TA_IRS_Bus::CommonTypes::TrainIdList(),
			TA_IRS_Bus::TrainMessageTypeList() );
		m_radioGroupCoordinator->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_timsCctvManager->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_timsDriverCallManager->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_timsModeManager->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_timsPecManager->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_timsStatusManager->notifyOperationStateChange(TA_Base_Core::Monitor);
		m_trainInformation->notifyOperationStateChange(TA_Base_Core::Monitor);

//		registerForMonitorMessages();
		FUNCTION_EXIT;
	}

	/**
	* agentSetControl
	*
	* This method is called when the agent is placed into control mode. It is 
	* responsible for making any necessary state changes in the agent to place it in
	* control mode.
	*/
	void TrainAgent::agentSetControl()
	{
        FUNCTION_ENTRY( "agentSetControl" );

		m_operationModeManager->agentSetControl();
        if ( true == m_firstRunMode )
        {
            // then this is the first agent starting up at a location, all non unique alarms must be closed
            m_alarmUtility->closeAllAlarmsForThisAgent();
			
            // get the global states from agents at other locations
            m_stateUpdateManager->performFirstControlModeSync();
			
            m_firstRunMode = false;
        }

        typedef TA_Base_Core::ThreadedMemberFunction< TrainAgent > TrainAgentMemberFunction;
        TrainAgentMemberFunction& thrd = TrainAgentMemberFunction::alloc( *this );
        thrd.add( &TrainAgent::agentSetControlInThread );
        thrd.start();

		FUNCTION_EXIT;
	}


    void TrainAgent::agentSetControlInThread()
    {
        m_radioGroupCoordinator->notifyOperationStateChange(TA_Base_Core::Control);
        m_timsCctvManager->notifyOperationStateChange(TA_Base_Core::Control);
        m_timsDriverCallManager->notifyOperationStateChange(TA_Base_Core::Control);
        m_timsModeManager->notifyOperationStateChange(TA_Base_Core::Control);
        m_timsPecManager->notifyOperationStateChange(TA_Base_Core::Control);
        m_timsStatusManager->notifyOperationStateChange(TA_Base_Core::Control);
        m_trainInformation->notifyOperationStateChange(TA_Base_Core::Control);
    }


	/**
	* registerForStateUpdates
	*
	* This method is called upon initialisation. It is to be used to register the 
	* agent for any necessary state updates.
	*/
	void TrainAgent::registerForStateUpdates()
	{
		FUNCTION_ENTRY("registerForStateUpdates");
		FUNCTION_EXIT;
	}

	//
	// receiveSpecialisedMessage
	//
	void TrainAgent::receiveSpecialisedMessage(const StateUpdateMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		
		FUNCTION_EXIT;
	}

	/**
	* run
	*
	* This method is called by the agent mainline, and is used start any necessary
	* threads before calling GenericAgent::run which will block until the process
	* is terminated.
	*/
    void TrainAgent::run()
	{
		FUNCTION_ENTRY("run");

        // set the m_threadPool upper limit to the max threadpool size from m_configuration
        m_threadPool.setUpperLimit( m_configuration->getMaxThreadPoolSize() );

        // load initial saved train states from the database and set up train states for 99 trains by calling
        // m_trainAgentStatusManager->initialise()
        m_trainAgentStatusManager->initialise();

        // load all radio groups from the database by calling
        // m_radioGroupCoordinator->initialise()
        m_radioGroupCoordinator->initialise();
		
		m_servant = new TrainAgentServant(*this);

        // create servants:
        // m_trainOperationModeCorbaServant
        m_trainOperationModeCorbaServant = new TrainOperationModeCorbaServant( *m_operationModeManager,
                                                                               *m_timsModeManager );

        // m_trainStateUpdateCorbaServant
        m_trainStateUpdateCorbaServant = new TrainStateUpdateCorbaServant( *m_stateUpdateManager );

        // m_trainRadioGroupCorbaServant
        m_trainRadioGroupCorbaServant = new TrainRadioGroupCorbaServant( *m_radioGroupCoordinator );

        // m_trainInformationCorbaServant
        m_trainInformationCorbaServant = new TrainInformationCorbaServant( *m_trainInformation,
                                                                           *m_operationModeManager );

        // m_trainCctvCorbaServant
        m_trainCctvCorbaServant = new TrainCctvCorbaServant( *m_timsCctvManager );

        // m_trainDriverCallCorbaServant
        m_trainDriverCallCorbaServant = new TrainDriverCallCorbaServant( *m_timsDriverCallManager );

        // m_trainPaCorbaServant
        m_trainPaCorbaServant = new TrainPaCorbaServant( *m_timsPaManager );

        // m_trainPecCorbaServant
        m_trainPecCorbaServant = new TrainPecCorbaServant( *m_timsPecManager );

        // m_trainTisCorbaServant
        m_trainTisCorbaServant = new TrainTisCorbaServant( *m_timsTisManager );

        // start the subscriptions to other agents and load initial state, create:
        // m_dutyAgentObserver
        m_dutyAgentObserver = new DutyAgentObserver( *m_configuration,
                                                     *m_operationModeManager );

        // m_radioAgentObserver
        m_radioAgentObserver = new RadioAgentObserver( *m_configuration,
                                                       *m_radioAgentInterface,
                                                       *m_radioMessageProcessor );

        // m_atsAgentObserver
        m_atsAgentObserver = new AtsAgentObserver( *m_configuration,
                                                   *m_atsMessageProcessor,
                                                   *m_atsAgentInterface );

        try
        {
            // start the operation mode manager (get its initial state)
            // m_operationModeManager->start()
            m_operationModeManager->start();

            // activate all servants using the constant for the servant name for each servant:
            // m_trainOperationModeCorbaServant (TrainAgentObjectNames::TrainOperationModeObjectName )
            m_trainOperationModeCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainOperationModeObjectName );

            // m_trainStateUpdateCorbaServant ( TrainAgentObjectNames::TrainStateUpdateObjectName )
            m_trainStateUpdateCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainStateUpdateObjectName );

            // m_trainRadioGroupCorbaServant ( TrainAgentObjectNames::TrainRadioGroupObjectName )
            m_trainRadioGroupCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainRadioGroupObjectName );

            // m_trainInformationCorbaServant ( TrainAgentObjectNames::TrainInformationObjectName )
            m_trainInformationCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainInformationObjectName );

            // m_trainCctvCorbaServant ( TrainAgentObjectNames::TrainCctvObjectName )
            m_trainCctvCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainCctvObjectName );

            // m_trainDriverCallCorbaServant ( TrainAgentObjectNames::TrainDriverCallObjectName )
            m_trainDriverCallCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainDriverCallObjectName );

            // m_trainPaCorbaServant ( TrainAgentObjectNames::TrainPaObjectName )
            m_trainPaCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainPaObjectName );

            // m_trainPecCorbaServant ( TrainAgentObjectNames::TrainPecObjectName )
            m_trainPecCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainPecObjectName );

            // m_trainTisCorbaServant ( TrainAgentObjectNames::TrainTisObjectName )
            m_trainTisCorbaServant->activateServantWithName( TA_Base_Bus::TrainAgentObjectNames::TrainTisObjectName );
        }
        // catch any FatalConfigurationException
        catch ( TA_Base_Core::FatalConfigurationException& e )
        {
            // log the exception
            LOG_EXCEPTION_CATCH( SourceInfo,
                                 "TA_Base_Core::FatalConfigurationException",
                                 e.what() );

            // call terminateWithoutRestart
            m_genericAgent->terminateWithoutRestart();
        }

		//change agent to control or monitor state
		if (NULL != m_genericAgent)
		{
			if (RunParams::getInstance().get(RPARAM_OPERATIONMODE)==RPARAM_CONTROL)
			{
				agentSetControl();
			}
			else if (RunParams::getInstance().get(RPARAM_OPERATIONMODE)==RPARAM_MONITOR)
			{
				agentSetMonitor();
				m_stateUpdateManager->performFullSync();
				
				// m_firstRunMode = false
				m_firstRunMode = false;
			}
			else 
			{
				agentSetControl();
			}
		}

		m_genericAgent->run();

		FUNCTION_EXIT;
	}
		

    void TrainAgent::notifyGroupOffline( const std::string& group )
    {
        FUNCTION_ENTRY( "notifyGroupOffline" );

        // if the group is the "OCC" group name
        if ( "OCC" == group )
        {
            // notify the operation mode manager:
            m_operationModeManager->processOccStateChange( false );
        }

        FUNCTION_EXIT;
    }


    void TrainAgent::notifyGroupOnline( const std::string& group )
    {
        FUNCTION_ENTRY( "notifyGroupOnline" );

        // if the group is the "OCC" group name
        if ( "OCC" == group )
        {
            // notify the operation mode manager:
            m_operationModeManager->processOccStateChange( true );
        }

        FUNCTION_EXIT;
    }


    void TrainAgent::agentTerminate()
    {
        FUNCTION_ENTRY( "agentTerminate" );

        // cancel any ongoing operations
        if ( NULL != m_trainTransactionManager )
        {
            // use m_trainTransactionManager.cancelTransactions (with both lists empty)
            m_trainTransactionManager->cancelTransactions( TA_IRS_Bus::CommonTypes::TrainIdList(),
                                                           TA_IRS_Bus::TrainMessageTypeList() );
        }

        // stopAllActivity
        stopAllActivity();

        // deleteAllObjects
        deleteAllObjects();

        FUNCTION_EXIT;
    }


    void TrainAgent::processOnlineUpdate( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
        FUNCTION_ENTRY( "processOnlineUpdate" );

        // no online updates are supported. do nothing

        FUNCTION_EXIT;
    }


    void TrainAgent::stopAllActivity()
    {
        FUNCTION_ENTRY( "stopAllActivity" );

        // delete the agent observers:
        // m_dutyAgentObserver
        delete m_dutyAgentObserver;
        m_dutyAgentObserver = NULL;

        // m_radioAgentObserver
        delete m_radioAgentObserver;
        m_radioAgentObserver = NULL;

        // m_atsAgentObserver
        delete m_atsAgentObserver;
        m_atsAgentObserver = NULL;

        // stop the operation mode manager
        // m_operationModeManager->terminateAndWait()
        m_operationModeManager->terminateAndWait();

        // deactivate and delete all servants (and set all to NULL):
        // m_trainOperationModeCorbaServant
        m_trainOperationModeCorbaServant->deactivateAndDeleteServant();
        m_trainOperationModeCorbaServant = NULL;

        // m_trainStateUpdateCorbaServant
        m_trainStateUpdateCorbaServant->deactivateAndDeleteServant();
        m_trainStateUpdateCorbaServant = NULL;

        // m_trainRadioGroupCorbaServant
        m_trainRadioGroupCorbaServant->deactivateAndDeleteServant();
        m_trainRadioGroupCorbaServant = NULL;

        // m_trainInformationCorbaServant
        m_trainInformationCorbaServant->deactivateAndDeleteServant();
        m_trainInformationCorbaServant = NULL;

        // m_trainCctvCorbaServant
        m_trainCctvCorbaServant->deactivateAndDeleteServant();
        m_trainCctvCorbaServant = NULL;

        // m_trainDriverCallCorbaServant
        m_trainDriverCallCorbaServant->deactivateAndDeleteServant();
        m_trainDriverCallCorbaServant = NULL;

        // m_trainPaCorbaServant
        m_trainPaCorbaServant->deactivateAndDeleteServant();
        m_trainPaCorbaServant = NULL;

        // m_trainPecCorbaServant
        m_trainPecCorbaServant->deactivateAndDeleteServant();
        m_trainPecCorbaServant = NULL;

        // m_trainTisCorbaServant
        m_trainTisCorbaServant->deactivateAndDeleteServant();
        m_trainTisCorbaServant = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteAllObjects()
    {
        FUNCTION_ENTRY( "deleteAllObjects" );

        // deleteGroupEight
        deleteGroupEight();

        // deleteGroupSeven
        deleteGroupSeven();

        // deleteGroupSix
        deleteGroupSix();

        // deleteGroupFive
        deleteGroupFive();

        // deleteGroupFour
        deleteGroupFour();

        // deleteGroupThree
        deleteGroupThree();

        // deleteGroupTwo
        deleteGroupTwo();

        // deleteGroupOne
        deleteGroupOne();

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupOne( TA_Base_Core::TrainAgentEntityDataPtr trainAgentEntityData )
    {
        FUNCTION_ENTRY( "createGroupOne" );

        // create m_configuration
        m_configuration = new TrainAgentConfiguration( trainAgentEntityData );

        // initialise TrainAgentProxyFactory with the entity data
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise( trainAgentEntityData );

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupTwo()
    {
        FUNCTION_ENTRY( "createGroupTwo" );

        // create:
        // m_cctvAgentInterface
        m_cctvAgentInterface = new CctvAgentInterface( *m_configuration );

        // m_dutyAgentInterface
        m_dutyAgentInterface = new DutyAgentInterface( *m_configuration );

        // m_atsAgentInterface
        m_atsAgentInterface = new AtsAgentInterface( *m_configuration );

        // m_messageSender
        m_messageSender = new MessageSender( *m_configuration );

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupThree()
    {
        FUNCTION_ENTRY( "createGroupThree" );

        // create:
        // m_trainAgentStatusManager
        m_trainAgentStatusManager = new TrainAgentStatusManager( *m_configuration,
                                                                 *m_messageSender,
                                                                 *m_messageSender );

        // m_radioGroupCoordinator
        m_radioGroupCoordinator = new RadioGroupCoordinator( *m_configuration,
                                                             *m_messageSender,
                                                             *m_messageSender );

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupFour()
    {
        FUNCTION_ENTRY( "createGroupFour" );

        // create:
        // m_radioAgentInterface
        m_radioAgentInterface = new RadioAgentInterface( *m_configuration,
                                                         *m_trainAgentStatusManager );

        // m_atsMessageProcessor
        m_atsMessageProcessor = new AtsMessageProcessor( *m_trainAgentStatusManager );

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupFive()
    {
        FUNCTION_ENTRY( "createGroupFive" );

        // create m_trainTransactionManager
        m_trainTransactionManager = new TA_IRS_Bus::TrainTransactionManager( m_configuration->getDefaultTimsMessageTimeout(),
                                                                             m_configuration->getTimsRetryLimit(),
                                                                             *m_radioAccess,
                                                                             m_trainEventFactoryImpl,
                                                                             *m_trainAgentStatusManager );
		
		m_radioAccess->registerForSDS(*m_trainTransactionManager);
        // get the agent location type from m_configuration
        TA_Base_Core::ILocation::ELocationType locationType = m_configuration->getLocationType( m_configuration->getLocationKey() );

        // create m_operationModeManager as one of StationOperationModeManager, OccOperationModeManager, or DepotOperationModeManager
        switch ( locationType )
        {
            case TA_Base_Core::ILocation::STATION:
                m_operationModeManager = new StationOperationModeManager( *m_configuration,
                                                                          *m_messageSender,
                                                                          *m_messageSender,
                                                                          *m_trainAgentStatusManager,
                                                                          *m_dutyAgentInterface,
                                                                          *m_atsAgentInterface,
                                                                          *m_radioAgentInterface );
                break;

            case TA_Base_Core::ILocation::OCC:
                m_operationModeManager = new OccOperationModeManager( *m_configuration,
                                                                      *m_messageSender,
                                                                      *m_messageSender,
                                                                      *m_trainAgentStatusManager,
                                                                      *m_dutyAgentInterface );
                break;

            case TA_Base_Core::ILocation::DPT:
            default:
                m_operationModeManager = new DepotOperationModeManager( *m_configuration,
                                                                        *m_messageSender,
                                                                        *m_messageSender,
                                                                        *m_trainAgentStatusManager,
                                                                        *m_dutyAgentInterface );
                break;
        }

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupSix( TA_Base_Core::TrainAgentEntityDataPtr trainAgentEntityData )
    {
        FUNCTION_ENTRY( "createGroupSix" );

        // create:
        // m_ctaRights,hongzhi
        m_ctaRights = new TA_Base_Bus::CtaRights();

        // m_rightsChecker
        m_rightsChecker = new RightsChecker( *m_configuration,
                                             *m_ctaRights,
                                             *m_operationModeManager );

        // m_ctaAuditing
        m_ctaAuditing = new TA_Base_Core::CtaAuditing( m_configuration->getEntityKey(),
                                                       TA_Base_Core::TrainAudit::Context );

        // m_auditSender
        m_auditSender = new AuditSender( *m_configuration,
                                         *m_trainAgentStatusManager,
                                         *m_ctaAuditing );

        // m_ctaAlarms
        //m_ctaAlarms = new TA_Base_Core::CtaAlarms( m_agentEntityData );
		m_ctaAlarms = new TA_Base_Core::CtaAlarms( m_genericAgent->getAgentEntityConfiguration() );
        // m_alarmUtility
        m_alarmUtility = new AlarmUtility( *m_configuration,
                                           *m_trainAgentStatusManager,
                                           *m_ctaAlarms );

        // m_radioMessageProcessor
        m_radioMessageProcessor = new RadioMessageProcessor( *m_trainTransactionManager,
                                                             *m_trainAgentStatusManager );
        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupSeven()
    {
        FUNCTION_ENTRY( "createGroupSeven" );

        // m_timsModeManager
        m_timsModeManager = new TimsModeManager( *m_configuration,
                                                 *m_trainAgentStatusManager,
                                                 *m_trainTransactionManager,
                                                 *m_operationModeManager,
                                                 *m_alarmUtility,
                                                 *m_rightsChecker,
                                                 *m_auditSender,
                                                 *m_atsMessageProcessor );

        // get the agent location type from m_configuration
        TA_Base_Core::ILocation::ELocationType locationType = m_configuration->getLocationType( m_configuration->getLocationKey() );

        // create:
        // m_trainInformation
        m_trainInformation = new TrainInformation( *m_rightsChecker,
                                                   *m_messageSender,
                                                   *m_auditSender,
                                                   *m_trainAgentStatusManager );

        // create m_timsCctvManager as a TimsCctvManager (OCC, Station) or a DepotTimsCctvManager (Depot)
        switch ( locationType )
        {
            case TA_Base_Core::ILocation::OCC:
            case TA_Base_Core::ILocation::STATION:
                m_timsCctvManager = new TimsCctvManager( *m_configuration,
                                                         *m_trainAgentStatusManager,
                                                         *m_trainTransactionManager,
                                                         *m_operationModeManager,
                                                         *m_alarmUtility,
                                                         *m_rightsChecker,
                                                         *m_messageSender,
                                                         *m_auditSender,
                                                         *m_atsMessageProcessor,
                                                         *m_cctvAgentInterface );
                break;

            case TA_Base_Core::ILocation::DPT:
            default:
                m_timsCctvManager = new DepotTimsCctvManager( *m_configuration,
                                                              *m_trainAgentStatusManager,
                                                              *m_trainTransactionManager,
                                                              *m_operationModeManager,
                                                              *m_alarmUtility,
                                                              *m_rightsChecker,
                                                              *m_messageSender,
                                                              *m_auditSender,
                                                              *m_atsMessageProcessor,
                                                              *m_cctvAgentInterface );
                break;
        }

        // m_timsDriverCallManager
        m_timsDriverCallManager = new TimsDriverCallManager( *m_configuration,
                                                             *m_trainAgentStatusManager,
                                                             *m_trainTransactionManager,
                                                             *m_operationModeManager,
                                                             *m_messageSender,
                                                             *m_rightsChecker,
                                                             *m_auditSender,
                                                             *m_radioAgentInterface,
                                                             *m_radioMessageProcessor,
                                                             *m_radioGroupCoordinator,
															 *m_radioAccess);

        // m_timsPaManager
        m_timsPaManager = new TimsPaManager( *m_configuration,
                                             *m_trainAgentStatusManager,
                                             *m_trainTransactionManager,
                                             *m_operationModeManager,
                                             *m_messageSender,
                                             *m_rightsChecker,
                                             *m_radioAgentInterface,
                                             *m_radioMessageProcessor,
                                             *m_radioGroupCoordinator,
											 *m_radioAccess
											 );

        // m_timsTisManager
        m_timsTisManager = new TimsTisManager( *m_configuration,
                                               *m_trainAgentStatusManager,
                                               *m_trainTransactionManager,
                                               *m_operationModeManager,
                                               *m_messageSender,
                                               *m_rightsChecker,
                                               *m_radioAgentInterface,
                                               *m_radioMessageProcessor );

        FUNCTION_EXIT;
    }


    void TrainAgent::createGroupEight()
    {
        FUNCTION_ENTRY( "createGroupEight" );

        // create:
        // m_timsPecManager
        m_timsPecManager = new TimsPecManager( *m_configuration,
                                               *m_trainAgentStatusManager,
                                               *m_trainTransactionManager,
                                               *m_operationModeManager,
                                               *m_messageSender,
                                               *m_rightsChecker,
                                               *m_auditSender,
                                               //*m_radioAgentInterface,
											   *m_radioAccess,
                                               *m_radioMessageProcessor,
                                               *m_atsMessageProcessor,
                                               *m_timsCctvManager );

        // m_timsStatusManager
        m_timsStatusManager = new TimsStatusManager( *m_configuration,
                                                     *m_trainAgentStatusManager,
                                                     *m_trainTransactionManager,
                                                     *m_operationModeManager,
                                                     *m_timsModeManager,
                                                     *m_alarmUtility,
                                                     *m_messageSender,
                                                     *m_messageSender,
                                                     *m_atsMessageProcessor,
                                                     *m_atsAgentInterface );

        // m_stateUpdateManager
        m_stateUpdateManager = new StateUpdateManager( *m_configuration,
                                                       *m_trainAgentStatusManager,
                                                       *m_operationModeManager,
                                                       *m_radioGroupCoordinator,
                                                       *m_timsModeManager,
                                                       *m_messageSender,
                                                       *m_messageSender );

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupOne()
    {
        FUNCTION_ENTRY( "deleteGroupOne" );

        // delete m_configuration (and set to NULL)
        delete m_configuration;
        m_configuration = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupTwo()
    {
        FUNCTION_ENTRY( "deleteGroupTwo" );

        // delete (and set to NULL):
        // m_cctvAgentInterface
        delete m_cctvAgentInterface;
        m_cctvAgentInterface = NULL;

        // m_dutyAgentInterface
        delete m_dutyAgentInterface;
        m_dutyAgentInterface = NULL;

        // m_atsAgentInterface
        delete m_atsAgentInterface;
        m_atsAgentInterface = NULL;

        // m_messageSender
        delete m_messageSender;
        m_messageSender = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupThree()
    {
        FUNCTION_ENTRY( "deleteGroupThree" );

        // delete (and set to NULL):
        // m_trainAgentStatusManager
        delete m_trainAgentStatusManager;
        m_trainAgentStatusManager = NULL;

        // m_radioGroupCoordinator
        delete m_radioGroupCoordinator;
        m_radioGroupCoordinator = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupFour()
    {
        FUNCTION_ENTRY( "deleteGroupFour" );

        // delete (and set to NULL):
        // m_radioAgentInterface
        delete m_radioAgentInterface;
        m_radioAgentInterface = NULL;

        // m_atsMessageProcessor
        delete m_atsMessageProcessor;
        m_atsMessageProcessor = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupFive()
    {
        FUNCTION_ENTRY( "deleteGroupFive" );

        // delete (and set to NULL):
        // m_trainTransactionManager
        delete m_trainTransactionManager;
        m_trainTransactionManager = NULL;

        // m_operationModeManager
        delete m_operationModeManager;
        m_operationModeManager = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupSix()
    {
        FUNCTION_ENTRY( "deleteGroupSix" );

        // delete (and set to NULL):
        // m_rightsChecker
        delete m_rightsChecker;
        m_rightsChecker = NULL;

        // m_ctaRights
        delete m_ctaRights;
        m_ctaRights = NULL;

        // m_auditSender
        delete m_auditSender;
        m_auditSender = NULL;

        // m_ctaAuditing
        delete m_ctaAuditing;
        m_ctaAuditing = NULL;

        // m_alarmUtility
        delete m_alarmUtility;
        m_alarmUtility = NULL;

        // m_ctaAlarms
        delete m_ctaAlarms;
        m_ctaAlarms = NULL;

        // m_radioMessageProcessor
        delete m_radioMessageProcessor;
        m_radioMessageProcessor = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupSeven()
    {
        FUNCTION_ENTRY( "deleteGroupSeven" );

        // delete (and set to NULL):
        // m_trainInformation
        delete m_trainInformation;
        m_trainInformation = NULL;

        // m_timsModeManager
        delete m_timsModeManager;
        m_timsModeManager = NULL;

        // m_timsCctvManager
        delete m_timsCctvManager;
        m_timsCctvManager = NULL;

        // m_timsDriverCallManager
        delete m_timsDriverCallManager;
        m_timsDriverCallManager = NULL;

        // m_timsPaManager
        delete m_timsPaManager;
        m_timsPaManager = NULL;

        // m_timsTisManager
        delete m_timsTisManager;
        m_timsTisManager = NULL;

        FUNCTION_EXIT;
    }


    void TrainAgent::deleteGroupEight()
    {
        FUNCTION_ENTRY( "deleteGroupEight" );

        // delete (and set to NULL):
        // m_timsPecManager
        delete m_timsPecManager;
        m_timsPecManager = NULL;

        // m_timsStatusManager
        delete m_timsStatusManager;
        m_timsStatusManager = NULL;

        // m_stateUpdateManager
        delete m_stateUpdateManager;
        m_stateUpdateManager = NULL;

        FUNCTION_EXIT;
    }

} // TA_IRS_App
