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

#include "app/trains/train_agent/src/StationOperationModeManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IDutyAgentInterface.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/IOperationModeObserver.h"

#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/security/duty_monitor_framework/src/DutyMonitorFrameworkException.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
//#include "bus/managed_agent/src/AgentManager.h"
//#include "bus/managed_agent/src/SystemControlledManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    StationOperationModeManager::StationOperationModeManager( ITrainAgentConfiguration& configuration,
                                                              IStateUpdateMessageSender& stateUpdateSender,
                                                              IMessageSender& messageSender,
                                                              ITrainStatusManager& statusManager,
                                                              IDutyAgentInterface& dutyAgent,
                                                              IAtsAgentInterface& atsAgent,
                                                              IRadioAgentInterface& radioAgent )
            : OperationModeManager( configuration,
                                    stateUpdateSender,
                                    messageSender,
                                    statusManager,
                                    dutyAgent ),
              m_atsAgent( atsAgent ),
              m_radioAgent( radioAgent ),
              m_fallbackMode( false ),
              m_doubleAtsFailure( false ),
              m_groupOffline( false ),
              m_localDuty( false )
    {
        FUNCTION_ENTRY( "StationOperationModeManager" );

        initialiseLocationControlMap();

        FUNCTION_EXIT;
    }


    StationOperationModeManager::~StationOperationModeManager()
    {
        FUNCTION_ENTRY( "~StationOperationModeManager" );
        FUNCTION_EXIT;
    }


    bool StationOperationModeManager::radioInFallback() const
    {
        FUNCTION_ENTRY( "radioInFallback" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        FUNCTION_EXIT;
        return ( m_fallbackMode || m_localDuty );
    }


    void StationOperationModeManager::processOccStateChange( bool occOnline )
    {
        FUNCTION_ENTRY( "processOccStateChange" );

        // check the agent is in the control state using AgentManager, if not return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        bool groupOffline = !occOnline;

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // log the OCC state change
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "OCC state change, setting Group Offline from %s to %s",
                     m_groupOffline ? "True" : "False",
                     groupOffline ? "True" : "False" );

        bool changes = false;

        m_groupOffline = groupOffline;

        logFailureStates();

        if ( true == m_groupOffline )
        {
            // then duty will be auto degraded
            m_localDuty = true;

            // take control of trains at this station:
            changes = takeControlOfLocation( m_configuration.getLocationKey() );
		}

        // if there were no changes to overall control,
        // a state and client update must be triggered:
        if ( false == changes )
        {
            m_lastUpdateTime = time( NULL );

            notifyStateChange();
        }

        FUNCTION_EXIT;
    }


    void StationOperationModeManager::inDoubleAtsFailure( bool doubleAtsFailure )
    {
        FUNCTION_ENTRY( "inDoubleAtsFailure" );

        // check the agent is in the control state using AgentManager, if not return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // log the OCC state change
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "ATS state change, setting Double ATS Failure from %s to %s",
                     m_doubleAtsFailure ? "True" : "False",
                     doubleAtsFailure ? "True" : "False" );

        bool changes = false;

        m_doubleAtsFailure = doubleAtsFailure;

        logFailureStates();

        if ( true == m_doubleAtsFailure )
        {
            // if local duty is not applied
            // if ( false == m_localDuty ) //limin++
            {
                // then duty will must be degraded
                m_localDuty = true;

                try
                {
                    m_dutyAgent.degradeLocalTrainSubsystem();
                }
                catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
                }
            }

            // take control of trains at this station:
            changes = takeControlOfLocation( m_configuration.getLocationKey() );
        }

        // if there were no changes to overall control,
        // a state and client update must be triggered:
        if ( false == changes )
        {
            m_lastUpdateTime = time( NULL );

            notifyStateChange();
        }

        FUNCTION_EXIT;
    }


    void StationOperationModeManager::inFallback( bool fallback )
    {
        FUNCTION_ENTRY( "inFallback" );

        // check the agent is in the control state using AgentManager, if not return
        if ( false == OperationModeManager::isRunningControl() )
        {
            FUNCTION_EXIT;
            return;
        }

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Radio state change, setting Radio Fallback from %s to %s",
                     m_fallbackMode ? "True" : "False",
                     fallback ? "True" : "False" );

        bool changes = false;

        m_fallbackMode = fallback;

        logFailureStates();

        // notify ATS of the state change
        try
        {
            m_atsAgent.radioInFallback( radioInFallback() );
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        if ( true == m_fallbackMode )
        {
            // if local duty is not applied
            if ( false == m_localDuty )
            {
                // then duty will must be degraded
                m_localDuty = true;

                try
                {
                    m_dutyAgent.degradeLocalTrainSubsystem();
                }
                catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
                }
            }

            // take control of trains at this station:
            changes = takeControlOfLocation( m_configuration.getLocationKey() );
        }

        // if there were no changes to overall control,
        // a state and client update must be triggered:
        if ( false == changes )
        {
            m_lastUpdateTime = time( NULL );

            notifyStateChange();
        }

        FUNCTION_EXIT;
    }


    bool StationOperationModeManager::isIscsDegradedMode() const
    {
        FUNCTION_ENTRY( "isIscsDegradedMode" );
        FUNCTION_EXIT;
        return m_groupOffline;
    }


    bool StationOperationModeManager::isLocalRadioInFallback() const
    {
        FUNCTION_ENTRY( "isLocalRadioInFallback" );
        FUNCTION_EXIT;
        return m_fallbackMode;
    }


    TA_IRS_Bus::TrainInformationTypes::AgentOperationMode StationOperationModeManager::getCurrentOperationMode() const
    {
        FUNCTION_ENTRY( "getCurrentOperationMode" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // get the AgentOperationMode structure from the parent
        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode operationMode = OperationModeManager::getCurrentOperationMode();

        // set the local members
        operationMode.groupOffline = m_groupOffline;
        operationMode.fallbackMode = m_fallbackMode;
        operationMode.doubleAtsFailure = m_doubleAtsFailure;
        operationMode.localDuty = m_localDuty;
        FUNCTION_EXIT;
        return operationMode;
    }



	void StationOperationModeManager::processSubsystemStateChange( const SubsystemDutyState& dutyState )
	{
		FUNCTION_ENTRY( "processSubsystemStateChange" );

		// check the agent is in the control state using AgentManager, if not return

		if ( false == OperationModeManager::isRunningControl() )
		{
			FUNCTION_EXIT;
			return;
		}

		// check the state change is for this location, if not return
		if ( m_configuration.getLocationKey() != dutyState.locationKey )
		{
			FUNCTION_EXIT;
			return;
		}

		logControlledLocations();

		// acquire m_stateLock
		TA_THREADGUARD( m_stateLock );
		m_lastUpdateTime = time( NULL );
		 
		bool localDuty = ( dutyState.dutyState != TA_Base_Bus::E_SS_NORMAL );

		if ( localDuty == m_localDuty )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "no duty change, [local duty=%d]", m_localDuty );            
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Duty state change, setting Local Duty from %s to %s",
				m_localDuty ? "True" : "False", localDuty ? "True" : "False" );
		}

		bool changes = false;

		// if this agent has gained duty
		if ( true == localDuty )
		{
			m_localDuty = true;

			logFailureStates();

			// if the duty state is E_SS_DELEGATED then tell the ATS agent
			try
			{
				m_atsAgent.radioInFallback( true );
			}
			catch ( TA_IRS_Bus::AgentCommunicationException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
			}

			// take control of trains at this station:
			changes = takeControlOfLocation( m_configuration.getLocationKey() );
		}

		// otherwise, check if this agent has lost duty (ie it had it and it now doesnt)
		else if ( true == m_localDuty )
		{
			// local duty and group offline have been restored...
			m_localDuty = false;
			m_groupOffline = false;

			logFailureStates();

			// now test whether this loss of duty can actually remain...

			// If still in Radio Fallback
			if ( true == m_fallbackMode )
			{
				// local duty must be re-gained:
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Duty taken back while radio still in fallback, duty being degraded again." );

				m_localDuty = true;

				try
				{
					m_dutyAgent.degradeLocalTrainSubsystem();
				}
				catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
				{
					LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
				}

				m_lastUpdateTime = time( NULL );

				logFailureStates();

				notifyStateChange();

				// do not process any further,
				FUNCTION_EXIT;
				return;
			}

			// Now trigger an ATS recovery attempt
			m_doubleAtsFailure = false;

			try
			{
				m_atsAgent.resetAts();
			}
			catch ( TA_IRS_Bus::AgentCommunicationException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
			}

			// now if everything is in normal mode, release control of this location...
			if ( ( false == m_localDuty ) &&
				( false == m_fallbackMode ) &&
				( false == m_doubleAtsFailure ) &&
				( false == m_groupOffline ) )
			{
				changes = releaseControlOfLocation( m_configuration.getLocationKey() );
			}

			logFailureStates();
		}

		// if there were no changes to overall control,
		// a state and client update must be triggered:
		if ( false == changes )
		{
			m_lastUpdateTime = time( NULL );

			notifyStateChange();
		}

		FUNCTION_EXIT;
	}


    bool StationOperationModeManager::syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode )
    {
        FUNCTION_ENTRY( "syncCurrentOperationMode" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // if the parent class syncs
        if ( true == OperationModeManager::syncCurrentOperationMode( operationMode ) )
        {
            // set the 4 boolean members from the state object
            m_groupOffline = operationMode.groupOffline;
            m_fallbackMode = operationMode.fallbackMode;
            m_doubleAtsFailure = operationMode.doubleAtsFailure;
            m_localDuty = operationMode.localDuty;

            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;

        return false;
    }


    void StationOperationModeManager::controlModeInitialisation()
    {
        FUNCTION_ENTRY( "controlModeInitialisation" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        //
        // first, check the failure states:
        //

        // 1. OCC Offline
        // Check the RunParam RPARAM_GROUPSOFFLINE
        // if it is not empty, and its value doesnt contain TA_Base_Bus::SystemControlledManager::GROUP_ONLINE
        // then m_groupOffline = true
        if ( true == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_GROUPSOFFLINE ) )
        {
            std::string offlineGroups = TA_Base_Core::RunParams::getInstance().get( RPARAM_GROUPSOFFLINE );

			/*		hongzhi
            if ( ( offlineGroups.size() > 0 ) &&
                 ( offlineGroups != TA_Base_Bus::SystemControlledManager::GROUP_ONLINE ) )
            {
                m_groupOffline = true;
            }*/
            if ( ( offlineGroups.size() > 0 ) &&
				( offlineGroups != "GroupOnline" ) )
            {
                m_groupOffline = true;
            }

        }

        // 2. Radio Fallback
        // Check the radio agent for fallback mode
        // if this call fails, log the exception and leave the value of m_fallbackMode as false
        try
        {
            m_fallbackMode = m_radioAgent.inFallback();
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        // 3. Ats Failure
        // Check the local ATS agent for OCC failure
        // if this call fails, log the exception and leave the value of m_doubleAtsFailure as false
        try
        {
            m_doubleAtsFailure = m_atsAgent.inOccDoubleAtsFailure();
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }


        // 4. Local Duty
        // Check the subsystem state from the local duty agent
        // if the subsystem state is E_SS_NORMAL: m_localDuty = false
        // if the subsystem state is E_SS_DEGRADED or E_SS_DELEGATED: m_localDuty = true
        try
        {
            TA_Base_Bus::ESubsystemState localSubsystemState = m_dutyAgent.getLocalSubsystemState();
			
            if ( TA_Base_Bus::E_SS_NORMAL == localSubsystemState )
            {
                // the OCC Agent is in control
                m_localDuty = false;
            }
            else
            {
                // this agent is in control
                m_localDuty = true;
            }

        }
        catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }


        //
        // second, if there is no local duty and there should be, then it must be requested:
        //

        // if Local_Duty OR In_Radio_Fallback OR In_Double_OCC_ATS_Failure OR OCC_is_Offline
        if ( ( false == m_localDuty ) &&
             ( ( true == m_fallbackMode ) ||
               ( true == m_doubleAtsFailure ) ||
               ( true == m_groupOffline ) ) )
        {
            m_localDuty = true;

            try
            {
                m_dutyAgent.degradeLocalTrainSubsystem();
            }
            catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
            }
        }

        // Now the states are correct, ATS must be informed of the current state

        try
        {
            // Local_Duty OR In_Radio_Fallback OR OCC_is_Offline
            if ( ( true == m_fallbackMode ) ||
                 ( true == m_localDuty ) ||
                 ( true == m_groupOffline ) )
            {
                m_atsAgent.radioInFallback( true );
            }
            else // this means normal mode
            {
                m_doubleAtsFailure = false;

                // ATS must be reset to ensure it is running in local mode
                m_atsAgent.resetAts();

                // if there are problems for the ats returning to normal mode, it will call back this agent later...
            }
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }


        // now if there is any failure state true
        if ( ( true == m_fallbackMode ) ||
             ( true == m_localDuty ) ||
             ( true == m_doubleAtsFailure ) ||
             ( true == m_groupOffline ) )
        {
            // then local control must be assumed
            takeControlOfLocation( m_configuration.getLocationKey() );
        }

        // now log everything for info purposes
        logFailureStates();

        // and notify clients
        notifyStateChange();

        FUNCTION_EXIT;
    }


    void StationOperationModeManager::initialiseLocationControlMap()
    {
        FUNCTION_ENTRY( "initialiseLocationControlMap" );

        // Set this Station object up to be initially not in control of its current location,
        // insert one entry into m_locationControlMap
        m_locationControlMap[ m_configuration.getLocationKey() ] = false;

        FUNCTION_EXIT;
    }


    void StationOperationModeManager::logFailureStates() const
    {
        FUNCTION_ENTRY( "logFailureStates" );

        // acquire m_stateLock
     //   TA_THREADGUARD( m_stateLock );

        // log an info message indicating the failure modes
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Failure State: [Group Offline: %s][Local Duty: %s][Radio Fallback: %s][Double ATS Failure: %s]",
                     m_groupOffline ? "True" : "False",
                     m_localDuty ? "True" : "False",
                     m_fallbackMode ? "True" : "False",
                     m_doubleAtsFailure ? "True" : "False" );

        FUNCTION_EXIT;
    }


    void StationOperationModeManager::logControlledLocations() const
    {
        FUNCTION_ENTRY( "logControlledLocations" );

        OperationModeManager::logControlledLocations();

        logFailureStates();

        FUNCTION_EXIT;
    }


    //limin++, ECP-338 CL-21050
    bool StationOperationModeManager::isInControlOfTrain( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                          bool allowDelocalised /* = true */ ,
                                                          bool allowInvalid /* = true */ ) const
    {
        FUNCTION_ENTRY( "isInControlOfTrain" );
        
        try
        {
            // get the train comms state from m_statusManager
            TrainCommsStatePtr commsState = m_statusManager.getTrainCommsState( trainId );
            
            bool atsValid = false;
            bool localisationValid = false;
            unsigned long trainLocation = 0;
            unsigned long trainCommunicationLocation = 0;
            unsigned long location = m_configuration.getLocationKey();
            bool isTrainCommunicationValid = false;

            {
                // acquire the lock for the train state
                TA_IRS_Bus::TrainStateLock commsStateLock( commsState );
                
                // get the train's ATS validity and localisation validity
                atsValid = commsState->isAtsValid();
                localisationValid = commsState->isLocalisationValid();
                
                // get the train's location
                trainLocation = commsState->getLocation();
                trainCommunicationLocation = commsState->getCommunicatingLocation();
                isTrainCommunicationValid = commsState->isTrainCommunicationValid();
                
                logControlledLocations();
                
                LOG_GENERIC( SourceInfo,
                    TA_Base_Core::DebugUtil::DebugInfo,
                    "[atsValid=%d][localisationValid=%d][trainLocation=%d][trainId=%d][trainCommunicationLocation=%d][isTrainCommunicationValid=%d]",
                    atsValid,
                    localisationValid,
                    trainLocation,
                    trainId,
                    trainCommunicationLocation,
                    isTrainCommunicationValid );

                // release the lock for the train state
            }
            
            if ( true == atsValid )
            {
                // if the trains ATS validity is true, and its localisation is true
                if ( true == localisationValid )
                {
                    if ( false == isInControlOfLocation( trainLocation ) )
                    {
                        FUNCTION_EXIT;
                        return ( ( true == isTrainCommunicationValid ) &&
                                 ( trainCommunicationLocation == location ) &&
                                 ( true == isInControlOfLocation( location ) ) );
                    }

                    FUNCTION_EXIT;
                    return true;
                }
                
                // if the trains ATS validity is true, and its localisation is false,
                // and allowDelocalised is true, and its location is not 0
                else if ( ( true == allowDelocalised ) &&
                          ( 0 != trainLocation ) )
                {
                    if ( false == isInControlOfLocation( trainLocation ) )
                    {
                        FUNCTION_EXIT;
                        return ( ( true == isTrainCommunicationValid ) &&
                                 ( trainCommunicationLocation == location ) &&
                                 ( true == isInControlOfLocation( location ) ) );
                    }
                    
                    FUNCTION_EXIT;
                    return true;
                }
                
                // if the trains ATS validity is true, and its localisation is false
                else
                {
                    FUNCTION_EXIT;
                    return allowDelocalised;
                }
            }
            
            // if the trains ATS validity is false
            else
            {
                FUNCTION_EXIT;
                return allowInvalid;
            }
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }
        
        // if the train is not found, return false
        
        FUNCTION_EXIT;
        return false;
    }
    //++limin, ECP-338 CL-21050
	
	
    //limin++, CL-21120
    void StationOperationModeManager::syncWithDutyAgent()
    {
        FUNCTION_ENTRY( "syncWithDutyAgent" );
             
        try
        {        
			time_t updateTime=time( NULL );		
			TA_Base_Bus::ESubsystemState localSubsystemState = m_dutyAgent.getLocalSubsystemState();
			// acquire m_stateLock  
			TA_THREADGUARD( m_stateLock );
			if (m_lastUpdateTime>=updateTime)  //DTL-2198
			{
				//ignore the status,due to it already received latest status
				//otherwise, it may get an old result than the message notification
				std::ostringstream logMsg;
				logMsg << "syncWithDutyAgent:ignore agent status, due to received Duty Message Notification " 
					<< "[lastUpdateTime: " << m_lastUpdateTime			
					<< "][Begin Time: " << updateTime			
					<< "]";			 
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, logMsg.str().c_str());
				return;
			}

			//DTL-2186 : Station TBS Local duty does not match to dutyAgent
			bool localDuty;
			// the OCC Agent is in control
			if(TA_Base_Bus::E_SS_NORMAL == localSubsystemState)
			{			  
				localDuty=false;				
			}
			else 
			{
				// this agent is in control
				localDuty=true;
			}
			
			if ( localDuty == m_localDuty )
			{
				return;          
			}   
			else
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[old dutystate=%d] [new dutystate=%d]", m_localDuty, localSubsystemState );
				m_localDuty = localDuty;
			}
			
		}
		catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
			return;
		}

		//
		// second, if there is no local duty and there should be, then it must be requested:
		//

		// if Local_Duty OR In_Radio_Fallback OR In_Double_OCC_ATS_Failure OR OCC_is_Offline
		if ( ( false == m_localDuty ) &&
			( ( true == m_fallbackMode ) ||
			( true == m_doubleAtsFailure ) ||
			( true == m_groupOffline ) ) )
		{
			m_localDuty = true;

			try
			{
				m_dutyAgent.degradeLocalTrainSubsystem();
			}
			catch ( TA_Base_Bus::DutyMonitorFrameworkException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
			}
		}

		// now if there is any failure state true
		if ( ( true == m_fallbackMode ) ||
			( true == m_localDuty ) ||
			( true == m_doubleAtsFailure ) ||
			( true == m_groupOffline ) )
		{
			// then local control must be assumed
			takeControlOfLocation( m_configuration.getLocationKey() );
		}
		// now if everything is in normal mode, release control of this location...
		if ( ( false == m_localDuty ) &&
			( false == m_fallbackMode ) &&
			( false == m_doubleAtsFailure ) &&
			( false == m_groupOffline ) )
		{
			releaseControlOfLocation( m_configuration.getLocationKey() );
		}

		// now log everything for info purposes
		logFailureStates();

		// and notify clients
		notifyStateChange();

		FUNCTION_EXIT;
	}
	//limin++, CL-21120

} // TA_IRS_App
