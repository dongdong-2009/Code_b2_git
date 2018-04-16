/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/TrainCctvState.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{

    TrainCctvState::TrainCctvState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
                : TA_IRS_Bus::TrainState( trainId ),
                  m_cctvState( TA_IRS_Bus::CctvTypes::CctvInactive ),
                  m_preConflictTrain( 0 )
    {
        FUNCTION_ENTRY( "TrainCctvState" );

        m_items[0] = 0;
        m_items[1] = 0;
        m_items[2] = 0;
        m_items[3] = 0;
        
        FUNCTION_EXIT;
    }

    
    TrainCctvState::~TrainCctvState()
    {
        FUNCTION_ENTRY( "~TrainCctvState" );
        FUNCTION_EXIT;
    }
    

    TA_IRS_Bus::CommonTypes::ETrainResource TrainCctvState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainCctvResource;
    }


    TA_Base_Core::IThreadLockable& TrainCctvState::getCommandLock()
    {
        FUNCTION_ENTRY( "getCommandLock" );
        FUNCTION_EXIT;
        return m_commandLock;
    }


    bool TrainCctvState::isCctvActivated() const
    {
        FUNCTION_ENTRY( "isCctvActivated" );
        FUNCTION_EXIT;
        return ( m_cctvState != TA_IRS_Bus::CctvTypes::CctvInactive );
    }


    TA_IRS_Bus::CommonTypes::TrainIdType TrainCctvState::getPreConflictTrain() const
    {
        FUNCTION_ENTRY( "getPreConflictTrain" );
        FUNCTION_EXIT;
        return m_preConflictTrain;
    }


    bool TrainCctvState::isPreConflict() const
    {
        FUNCTION_ENTRY( "isPreConflict" );
        FUNCTION_EXIT;
        return ( m_preConflictTrain != 0 );
    }


    TA_IRS_Bus::CctvTypes::ECctvSwitchMode TrainCctvState::getCctvMode() const
    {
        FUNCTION_ENTRY( "getCctvMode" );
        FUNCTION_EXIT;
        return m_cctvState;
    }


    boost::array<unsigned char, 4> TrainCctvState::getCctvState() const
    {
        FUNCTION_ENTRY( "getCctvState" );
        FUNCTION_EXIT;
        return m_items;
    }


    TA_IRS_Bus::CctvTypes::CctvSwitchState TrainCctvState::getCctvSwitchState()
    {
        FUNCTION_ENTRY( "getCctvSwitchState" );

        // Create and populate a CctvSwitchState structure
        TA_IRS_Bus::CctvTypes::CctvSwitchState state;

        state.trainId = getTrainId();
        state.preConflictTrain = m_preConflictTrain;
        state.stageId = 0;  // this will be set elsewhere
        state.cctvState = m_cctvState;
        state.item1 = m_items[0];
        state.item2 = m_items[1];
        state.item3 = m_items[2];
        state.item4 = m_items[3];

        FUNCTION_EXIT;
        return state;
    }


    void TrainCctvState::setActivated( bool activated, time_t timestamp )
    {
        FUNCTION_ENTRY( "setActivated" );

        // initialise it to carrier on/off state
        setToDefaultCctvState( activated, timestamp );

        // if the train is being deactivated, clear the conflict state
        if ( false == activated )
        {
            clearPreConflict();
        }

        FUNCTION_EXIT;
    }


    void TrainCctvState::setPreConflict( TA_IRS_Bus::CommonTypes::TrainIdType conflictTrain )
    {
        FUNCTION_ENTRY( "setPreConflict" );
        
        if ( conflictTrain != m_preConflictTrain )
        {
            m_preConflictTrain = conflictTrain;
            
            setDataChangedWithTimestamp();
        }
        
        FUNCTION_EXIT;
    }


    void TrainCctvState::clearPreConflict()
    {
        FUNCTION_ENTRY( "clearPreConflict" );
        
        if ( 0 != m_preConflictTrain )
        {
            m_preConflictTrain = 0;
        }

		//Bohong++ for CL-21041
		setDataChangedWithTimestamp();
        
        FUNCTION_EXIT;
    }


    void TrainCctvState::setSingleCamera( TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera,
                                          time_t timestamp )
    {
        FUNCTION_ENTRY( "setSingleCamera" );

        // if the switch mode is not a single camera, and the camera number doesnt match, set the camera.
        if ( ( TA_IRS_Bus::CctvTypes::CctvCamera != m_cctvState ) ||
             ( camera != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( m_items[0] ) ) ||
             ( 0 != m_items[1] ) ||
             ( 0 != m_items[2] ) ||
             ( 0 != m_items[3] ) )
        {
            m_cctvState = TA_IRS_Bus::CctvTypes::CctvCamera;
            m_items[0] = static_cast< unsigned char >( camera );
            m_items[1] = 0;
            m_items[2] = 0;
            m_items[3] = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCctvState::setSequence( TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequence,
                                      time_t timestamp )
    {
        FUNCTION_ENTRY( "setSequence" );
        
        // if the switch mode is not a sequence, and the sequence number doesnt match, set the sequence.
        if ( ( TA_IRS_Bus::CctvTypes::CctvSequence != m_cctvState ) ||
            ( sequence != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber >( m_items[0] ) ) ||
            ( 0 != m_items[1] ) ||
            ( 0 != m_items[2] ) ||
            ( 0 != m_items[3] ) )
        {
            m_cctvState = TA_IRS_Bus::CctvTypes::CctvSequence;
            m_items[0] = static_cast< unsigned char >( sequence );
            m_items[1] = 0;
            m_items[2] = 0;
            m_items[3] = 0;
            
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainCctvState::setQuad( TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraOne,
                                  TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraTwo,
                                  TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraThree,
                                  TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraFour,
                                  time_t timestamp )
    {
        FUNCTION_ENTRY( "setQuad" );
        
        // if the switch mode is not a quad, and the quad number doesnt match, set the quad.
        if ( ( TA_IRS_Bus::CctvTypes::CctvQuad != m_cctvState ) ||
            ( cameraOne != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( m_items[0] ) ) ||
            ( cameraTwo != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( m_items[1] ) ) ||
            ( cameraThree != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( m_items[2] ) ) ||
            ( cameraFour != static_cast< TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera >( m_items[3] ) ) )
        {
            m_cctvState = TA_IRS_Bus::CctvTypes::CctvQuad;
            m_items[0] = static_cast< unsigned char >( cameraOne );
            m_items[1] = static_cast< unsigned char >( cameraTwo );
            m_items[2] = static_cast< unsigned char >( cameraThree );
            m_items[3] = static_cast< unsigned char >( cameraFour );
            
            setDataChangedWithTimestamp( timestamp );
        }
        
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState TrainCctvState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState state;
        
        state.trainId = getTrainId();
        state.cctvState = m_cctvState;
        state.items = m_items;
        state.preConflictTrain = m_preConflictTrain;
        state.timestamp = getTimestamp();
        
        FUNCTION_EXIT;
        return state;
    }


    void TrainCctvState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainCctvSyncState& state )
    {
        FUNCTION_ENTRY( "setSyncState" );

        // If the timestamp in the given state structure is less than the getTimestamp() of this state,
        // discard the sync state structure - log an error and return.
        
        if ( state.timestamp < getTimestamp() )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                        "Received old sync state for train %d [sync time: %d][state time: %d]",
                        getTrainId(), state.timestamp, getTimestamp() );
            
            FUNCTION_EXIT;
            return;
        }

        m_cctvState = state.cctvState;
        m_items = state.items;
        m_preConflictTrain = state.preConflictTrain;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );
        
        FUNCTION_EXIT;
    }


    void TrainCctvState::setToDefaultCctvState( bool activated,
                                                time_t timestamp )
    {
        FUNCTION_ENTRY( "setToDefaultCctvState" );
        
        // set up the desired state based on the activation state
        TA_IRS_Bus::CctvTypes::ECctvSwitchMode desiredState = TA_IRS_Bus::CctvTypes::CctvInactive;
        unsigned char firstItem = 0;
        
        if ( true == activated )
        {
            desiredState = TA_IRS_Bus::CctvTypes::CctvSequence;
            firstItem = static_cast< unsigned char >( TA_IRS_Bus::ProtocolCctvTypes::SequenceOne );
        }
        
        // apply the desired state if the current one differs 
        if ( ( desiredState != m_cctvState ) ||
             ( firstItem != m_items[0] ) ||
             ( 0 != m_items[1] ) ||
             ( 0 != m_items[2] ) ||
             ( 0 != m_items[3] ) )
        {
            m_cctvState = desiredState;
            m_items[0] = firstItem;
            m_items[1] = 0;
            m_items[2] = 0;
            m_items[3] = 0;
            
            setDataChangedWithTimestamp( timestamp );
        }
        
        FUNCTION_EXIT;
    }

} // TA_IRS_App
