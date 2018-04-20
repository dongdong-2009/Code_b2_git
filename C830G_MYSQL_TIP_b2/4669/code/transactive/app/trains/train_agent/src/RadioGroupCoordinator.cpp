/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/RadioGroupCoordinator.h"

#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/IStateUpdateMessageSender.h"
#include "app/trains/train_agent/src/IMessageSender.h"

#include "bus/trains/TrainCommonLibrary/src/GroupInUseException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/RadioGroupUtils.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"

#include <ctime>

namespace TA_IRS_App
{

    using TA_IRS_Bus::RadioGroupTypes::RadioGroupUtils;

    RadioGroupCoordinator::RadioGroupCoordinator( ITrainAgentConfiguration& configuration,
                                                  IMessageSender& messageSender,
                                                  IStateUpdateMessageSender& stateUpdateSender )
            : m_configuration( configuration ),
              m_messageSender( messageSender ),
              m_stateUpdateSender( stateUpdateSender )
    {
        FUNCTION_ENTRY( "RadioGroupCoordinator" );

        // register for operation state callbacks
        //TA_Base_Bus::AgentManager::registerForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    RadioGroupCoordinator::~RadioGroupCoordinator()
    {
        FUNCTION_ENTRY( "~RadioGroupCoordinator" );

        // deregister for operation state callbacks
        //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::initialise()
    {
        FUNCTION_ENTRY( "initialise" );

        loadRadioGroups();

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap RadioGroupCoordinator::getRadioGroups()
    {
        FUNCTION_ENTRY( "getRadioGroups" );

        TA_THREADGUARD( m_radioGroupLock );

        FUNCTION_EXIT;
        return m_radioGroups;
    }


    TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup RadioGroupCoordinator::getRadioGroup( const std::string& groupTsi )
    {
        FUNCTION_ENTRY( "getRadioGroup" );

        // acquire the m_radioGroupLock
        TA_THREADGUARD( m_radioGroupLock );

        // get the group from m_radioGroups
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_radioGroups.find( groupTsi );

        // if it doesnt exist, throw an exception
        if ( m_radioGroups.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid group TSI",
                                                             "GroupTsi",
                                                             groupTsi ) );
        }

        // return the group

        FUNCTION_EXIT;
        return findIter->second;
    }


    void RadioGroupCoordinator::lockRadioGroup( const std::string& groupTsi )
    {
        FUNCTION_ENTRY( "lockRadioGroup" );

        // acquire the m_radioGroupLock
        TA_THREADGUARD( m_radioGroupLock );

        // first, check if the group is locked, using RadioGroupUtils::isGroupLocked
        // if it is locked, throw a GroupInUseException

        if ( true == RadioGroupUtils::isGroupLocked( groupTsi, m_radioGroups ) )
        {
            TA_THROW( TA_IRS_Bus::GroupInUseException( "Group is in use" ) );
        }

        // Otherwise, check if the group exists in m_radioGroups
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_radioGroups.find( groupTsi );

        // if it exists, we must lock it
        if ( m_radioGroups.end() != findIter )
        {
            // set the group to locked
            findIter->second.locked = true;

            findIter->second.lastUpdateTime = time( NULL );

            // using m_stateUpdateSender, send out the modified radio group
            m_stateUpdateSender.sendStateUpdate( findIter->second );

            // using m_messageSender, send out the modified radio group
            m_messageSender.sendClientUpdate( findIter->second );
        }

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::unlockRadioGroup( const std::string& groupTsi )
    {
        FUNCTION_ENTRY( "unlockRadioGroup" );

        // acquire the m_radioGroupLock
        TA_THREADGUARD( m_radioGroupLock );

        // check if the group exists in m_radioGroups
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_radioGroups.find( groupTsi );

        // if it exists, we must unlock it
        if ( ( m_radioGroups.end() != findIter ) &&
             ( true == findIter->second.locked ) )
        {
            // set the group to unlocked
            findIter->second.locked = false;

            findIter->second.lastUpdateTime = time( NULL );

            // using m_stateUpdateSender, send out the modified radio group
            m_stateUpdateSender.sendStateUpdate( findIter->second );

            // using m_messageSender, send out the modified radio group
            m_messageSender.sendClientUpdate( findIter->second );
        }

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::syncRadioGroup( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup& radioGroup )
    {
        FUNCTION_ENTRY( "syncRadioGroup" );

        // acquire the lock
        TA_THREADGUARD( m_radioGroupLock );

        // find the radio group in the map
        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::iterator findIter = m_radioGroups.find( radioGroup.groupTsi );


        if ( m_radioGroups.end() != findIter )
        {
            // if the last update time is greater than the last update time in the map
            if ( radioGroup.lastUpdateTime > findIter->second.lastUpdateTime )
            {
                // update it with the received information
                findIter->second = radioGroup;
            }
        }
        else
        {
            // if it doesnt exist in the map, add it
            m_radioGroups[ radioGroup.groupTsi ] = radioGroup;
        }

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::syncRadioGroupList( const TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap& radioGroups )
    {
        FUNCTION_ENTRY( "syncRadioGroupList" );

        for ( TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::const_iterator iter = radioGroups.begin();
              radioGroups.end() != iter; ++iter )
        {
            syncRadioGroup( iter->second );
        }

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::notifyOperationStateChange( TA_Base_Core::EOperationMode operationState )
    {
        FUNCTION_ENTRY( "notifyOperationStateChange" );

        // if the mode is control
        if ( TA_Base_Core::Control == operationState )
        {
            // deregister for operation state callbacks
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );

            // acquire the lock
            TA_THREADGUARD( m_radioGroupLock );

            for ( TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::const_iterator iter = m_radioGroups.begin();
                  m_radioGroups.end() != iter; ++iter )
            {
                m_messageSender.sendClientUpdate( iter->second );
            }
        }
        // otherwise if its monitor
        else if ( TA_Base_Core::Monitor == operationState )
        {
            // just unsubscribe for updates, this means another agent in control
            // has already broadcast states
            //TA_Base_Bus::AgentManager::deregisterForStateChangeCallbacks( this );
        }

        FUNCTION_EXIT;
    }


    void RadioGroupCoordinator::loadRadioGroups()
    {
        FUNCTION_ENTRY( "loadRadioGroups" );

        // using m_configuration, popluate the m_radioGroups
        m_radioGroups = m_configuration.getRadioGroups();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Loaded: %s",
                     TA_IRS_Bus::RadioGroupTypes::trainRadioGroupMapToString( m_radioGroups ).c_str() );

        FUNCTION_EXIT;
    }

} // TA_IRS_App
