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

#include "app/trains/train_agent/src/DepotOperationModeManager.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IDutyAgentInterface.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

//#include "bus/managed_agent/src/SystemControlledManager.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    DepotOperationModeManager::DepotOperationModeManager( ITrainAgentConfiguration& configuration,
                                                          IStateUpdateMessageSender& stateUpdateSender,
                                                          IMessageSender& messageSender,
                                                          ITrainStatusManager& statusManager,
                                                          IDutyAgentInterface& dutyAgent )
            : OperationModeManager( configuration,
                                    stateUpdateSender,
                                    messageSender,
                                    statusManager,
                                    dutyAgent ),
              m_groupOffline( false )

    {
        FUNCTION_ENTRY( "DepotOperationModeManager" );

        initialiseLocationControlMap();

        FUNCTION_EXIT;
    }


    DepotOperationModeManager::~DepotOperationModeManager()
    {
        FUNCTION_ENTRY( "~DepotOperationModeManager" );
        FUNCTION_EXIT;
    }


    void DepotOperationModeManager::processOccStateChange( bool occOnline )
    {
        FUNCTION_ENTRY( "processOccStateChange" );

        // set the state, and the update time
        m_groupOffline = !occOnline;
        m_lastUpdateTime = time( NULL );

        // notify observers
        notifyStateChange();

        FUNCTION_EXIT;

    }


    bool DepotOperationModeManager::isIscsDegradedMode()
    {
        FUNCTION_ENTRY( "isIscsDegradedMode" );
        FUNCTION_EXIT;
        return m_groupOffline;
    }


    TA_IRS_Bus::TrainInformationTypes::AgentOperationMode DepotOperationModeManager::getCurrentOperationMode() const
    {
        FUNCTION_ENTRY( "getCurrentOperationMode" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // get the AgentOperationMode structure from the parent
        TA_IRS_Bus::TrainInformationTypes::AgentOperationMode operationMode = OperationModeManager::getCurrentOperationMode();

        // set the local member
        operationMode.groupOffline = m_groupOffline;

        FUNCTION_EXIT;
        return operationMode;
    }


    bool DepotOperationModeManager::syncCurrentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& operationMode )
    {
        FUNCTION_ENTRY( "syncCurrentOperationMode" );

        // acquire m_stateLock
        TA_THREADGUARD( m_stateLock );

        // if the parent class syncs
        if ( true == OperationModeManager::syncCurrentOperationMode( operationMode ) )
        {
            m_groupOffline = operationMode.groupOffline;

            FUNCTION_EXIT;
            return true;
        }

        FUNCTION_EXIT;
        return false;
    }


    void DepotOperationModeManager::initialiseLocationControlMap()
    {
        FUNCTION_ENTRY( "initialiseLocationControlMap" );

        // Set this depot object up to be always in control of the current location
        // insert one entry into m_locationControlMap for this location
        m_locationControlMap[ m_configuration.getLocationKey() ] = true;

        // Check the RunParam RPARAM_GROUPSOFFLINE
        if ( true == TA_Base_Core::RunParams::getInstance().isSet( RPARAM_GROUPSOFFLINE ) )
        {
            std::string offlineGroups = TA_Base_Core::RunParams::getInstance().get( RPARAM_GROUPSOFFLINE );

/*			hongzhi
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

        FUNCTION_EXIT;
    }

} // TA_IRS_App
