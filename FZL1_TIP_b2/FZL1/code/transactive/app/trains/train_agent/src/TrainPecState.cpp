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

#include "app/trains/train_agent/src/TrainPecState.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <algorithm>

// STL Helper function forward declarations
namespace
{
    // used to check if a PecPoint is answered by an operator
    bool isPecAnsweredByOperator( const TA_IRS_Bus::PecTypes::PecPoint& point );
    
    // used to check if a PecPoint is answered by a driver
    bool isPecAnsweredByDriver( const TA_IRS_Bus::PecTypes::PecPoint& point );
}

namespace TA_IRS_App
{

    TrainPecState::TrainPecState( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
            : TA_IRS_Bus::TrainState( trainId ),
              m_sessionId( "" ),
              m_consoleId( 0 ),
              m_callId( 0 ),
              m_autoContinueCounter( 0 ),
              m_continueRequired( false )
    {
        FUNCTION_ENTRY( "TrainPecState" );
        FUNCTION_EXIT;
    }


    TrainPecState::~TrainPecState()
    {
        FUNCTION_ENTRY( "~TrainPecState" );
        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::ETrainResource TrainPecState::getResourceCategory() const
    {
        FUNCTION_ENTRY( "getResourceCategory" );
        FUNCTION_EXIT;
        return TA_IRS_Bus::CommonTypes::TrainPecResource;
    }


    TA_Base_Core::IThreadLockable& TrainPecState::getCommandLock()
    {
        FUNCTION_ENTRY( "getCommandLock" );
        FUNCTION_EXIT;
        return m_commandLock;
    }


    const std::string& TrainPecState::getSessionId() const
    {
        FUNCTION_ENTRY( "getSessionId" );
        FUNCTION_EXIT;
        return m_sessionId;
    }


    unsigned long TrainPecState::getConsoleId() const
    {
        FUNCTION_ENTRY( "getConsoleId" );
        FUNCTION_EXIT;
        return m_consoleId;
    }


    unsigned long TrainPecState::getCallId() const
    {
        FUNCTION_ENTRY( "getCallId" );
        FUNCTION_EXIT;
        return m_callId;
    }


    bool TrainPecState::canAnswerPec( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                      TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec ) const
    {
        FUNCTION_ENTRY( "canAnswerPec" );

        // check if there is currently a session id set for this train, if there is, return false
        if ( false == m_sessionId.empty() )
        {
            FUNCTION_EXIT;
            return false;
        }

        // throw an exception if AllCars or AllPec are given

        if ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer PEC for all Cars", "Car", "AllCars" ) );
        }

        if ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer all PEC", "PEC", "AllPec" ) );
        }

        // create a PecPoint with the given pec and car number,
        TA_IRS_Bus::PecTypes::PecPoint givenPoint;
        givenPoint.car = car;
        givenPoint.pec = pec;

        // none of the other attributes are used in the comparison operators, so no need to populate any more

        // use std::find to find it in the list of active pec points, if it doesnt exist, throw an InvalidParameterException
        TA_IRS_Bus::PecTypes::ActivePecPointList::const_iterator findIter = m_activePecPoints.find( givenPoint );

        if ( m_activePecPoints.end() == findIter )
        {
            std::ostringstream pecStream;
            pecStream << car << ", " << pec;
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Given PEC is not active", "Car, PEC", pecStream.str() ) );
        }

        // if it exists check if it is assigned or not
        if ( TA_IRS_Bus::PecTypes::PecSourceUnassigned != findIter->source )
        {
            FUNCTION_EXIT;
            return false;
        }
        
        // now search through the active pec points, check if there is another one answered by a driver
        findIter = std::find_if( m_activePecPoints.begin(),
                                 m_activePecPoints.end(),
                                 isPecAnsweredByDriver );

        // if it found one, it cant be answered
        if ( m_activePecPoints.end() != findIter )
        {
            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }


    TA_IRS_Bus::PecTypes::PecPoint TrainPecState::getConnectedPecPoint() const
    {
        FUNCTION_ENTRY( "getConnectedPecPoint" );

        // search through the active pec points, and fine one with status == PecSourceOperator. Return that PEC.
        TA_IRS_Bus::PecTypes::ActivePecPointList::const_iterator findIter = std::find_if( m_activePecPoints.begin(),
                                                                                          m_activePecPoints.end(),
                                                                                          isPecAnsweredByOperator );

        // If none were found, throw an InvalidParameterException
        if ( m_activePecPoints.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "There are no PEC in progress", "", "" ) );
        }

        FUNCTION_EXIT;
        return ( *findIter );
    }


    TA_IRS_Bus::PecTypes::ActivePecPointList TrainPecState::getActivePecPoints() const
    {
        FUNCTION_ENTRY( "getActivePecPoints" );
        FUNCTION_EXIT;
        return m_activePecPoints;
    }


    unsigned char TrainPecState::getAutoContinueCount() const
    {
        FUNCTION_ENTRY( "getAutoContinueCount" );
        FUNCTION_EXIT;
        return m_autoContinueCounter;
    }


    bool TrainPecState::isContinueRequired()
    {
        FUNCTION_ENTRY( "isContinueRequired" );
        FUNCTION_EXIT;
        return m_continueRequired;
    }


    TA_IRS_Bus::PecTypes::PecPoint TrainPecState::pecActivated( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                time_t timestamp )
    {
        FUNCTION_ENTRY( "pecActivated" );

        // throw an exception if AllCars or AllPec are given

        if ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not activate PEC for all Cars", "Car", "AllCars" ) );
        }

        if ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not activate all PEC", "PEC", "AllPec" ) );
        }

        // create a PecPoint with the given pec and car number,
        // currently unassigned and valid
        TA_IRS_Bus::PecTypes::PecPoint newPoint;
        newPoint.car = car;
        newPoint.pec = pec;
        newPoint.valid = true;
        newPoint.source = TA_IRS_Bus::PecTypes::PecSourceUnassigned;
        newPoint.creationTime = timestamp;

        FUNCTION_EXIT;
        return updatePec( newPoint, timestamp );
    }


    void TrainPecState::setSessionId( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "setSessionId" );

        if ( sessionId != m_sessionId )
        {
            m_sessionId = sessionId;
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    void TrainPecState::resetSessionId()
    {
        FUNCTION_ENTRY( "resetSessionId" );

        if ( false == m_sessionId.empty() )
        {
            m_sessionId = "";
            setDataChangedWithTimestamp();
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::PecTypes::PecPoint TrainPecState::pecAnswered( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                               TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                               time_t timestamp,
                                                               const std::string& sessionId,
                                                               unsigned long consoleId,
                                                               unsigned long callId )
    {
        FUNCTION_ENTRY( "pecAnswered" );

        // throw an exception if AllCars or AllPec are given

        if ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer PEC for all Cars", "Car", "AllCars" ) );
        }

        if ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer all PEC", "PEC", "AllPec" ) );
        }

        // reset any auto continues
        resetAutoContinueCount();

        // set m_sessionId, m_consoleId, and m_callId
        // if anything changed from what was already there, setDataChangedWithTimestamp( timestamp)
        if ( ( sessionId != m_sessionId ) ||
             ( consoleId != m_consoleId ) ||
             ( callId != m_callId ) )
        {
            m_sessionId = sessionId;
            m_consoleId = consoleId;
            m_callId = callId;
            setDataChangedWithTimestamp( timestamp );
        }

        // create a PecPoint structure, currently assigned to the operator and valid
        TA_IRS_Bus::PecTypes::PecPoint newPoint;
        newPoint.car = car;
        newPoint.pec = pec;
        newPoint.valid = true;
        newPoint.source = TA_IRS_Bus::PecTypes::PecSourceOperator;
        newPoint.creationTime = timestamp;

        FUNCTION_EXIT;
        return updatePec( newPoint, timestamp );
    }


    TA_IRS_Bus::PecTypes::ActivePecPointList TrainPecState::pecAnsweredByDriver( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                                 TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                                 time_t timestamp )
    {
        FUNCTION_ENTRY( "pecAnsweredByDriver" );

        // throw an exception if AllCars or AllPec are given

        if ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer PEC for all Cars", "Car", "AllCars" ) );
        }

        if ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec )
        {
            TA_THROW( TA_IRS_Bus::InvalidParameterException( "Can not answer all PEC", "PEC", "AllPec" ) );
        }

        // reset any auto continues
        resetAutoContinueCount();

        // check if there was an operator answered PEC
        // if so the operator details must be reset and all points
        // are affected

        bool allPointsAffected = false;

        try
        {
            TA_IRS_Bus::PecTypes::PecPoint answeredPec = getConnectedPecPoint();

            // reset the source
            answeredPec.source = TA_IRS_Bus::PecTypes::PecSourceUnassigned;

            // update it
            updatePec( answeredPec, timestamp );

            // reset the call details and session id
            clearCall();
            resetSessionId();

            // all points should be updated
            allPointsAffected = true;
        }
        catch ( TA_IRS_Bus::InvalidParameterException& )
        {
            // means there was no operator answered pec
            // only the driver pec is affected
        }

        // create a PecPoint structure, currently assigned to the driver and valid
        TA_IRS_Bus::PecTypes::PecPoint newPoint;
        newPoint.car = car;
        newPoint.pec = pec;
        newPoint.valid = true;
        newPoint.source = TA_IRS_Bus::PecTypes::PecSourceDriver;
        newPoint.creationTime = timestamp;

        TA_IRS_Bus::PecTypes::PecPoint driverPec = updatePec( newPoint, timestamp );

        TA_IRS_Bus::PecTypes::ActivePecPointList affectedPoints;

        if ( true == allPointsAffected )
        {
            // all points are affected
            affectedPoints = getActivePecPoints();
        }
        else
        {
            // only update the single point
            affectedPoints.insert( driverPec );
        }
  
        FUNCTION_EXIT;
        return affectedPoints;
    }


    void TrainPecState::clearCall( time_t timestamp )
    {
        FUNCTION_ENTRY( "clearCall" );

        // clears m_callId, m_consoleId
        if ( ( 0 != m_consoleId ) ||
             ( 0 != m_callId ) )
        {
            m_consoleId = 0;
            m_callId = 0;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainPecState::setContinueRequired( time_t timestamp )
    {
        FUNCTION_ENTRY( "setContinueRequired" );

        if ( false == m_continueRequired )
        {
            m_continueRequired = true;
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    void TrainPecState::incrementAutoContinueCount()
    {
        FUNCTION_ENTRY( "incrementAutoContinueCount" );

        ++m_autoContinueCounter;

        setDataChangedWithTimestamp();

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::PecTypes::ActivePecPointList TrainPecState::pecReset( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
                                                                      TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec,
                                                                      time_t timestamp )
    {
        FUNCTION_ENTRY( "pecReset" );

        // if AllCars or AllPec are given

        if ( ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car ) ||
             ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec ) )
        {
            FUNCTION_EXIT;
            return resetAllPec( timestamp );
        }

        // create a PecPoint structure
        // search the active PEC points for this PEC (using find)
        TA_IRS_Bus::PecTypes::PecPoint givenPoint;
        givenPoint.car = car;
        givenPoint.pec = pec;

        // none of the other attributes are used in the comparison operators, so no need to populate any more

        // use std::find to find it in the list of active pec points
        TA_IRS_Bus::PecTypes::ActivePecPointList::iterator findIter = m_activePecPoints.find( givenPoint );

        TA_IRS_Bus::PecTypes::ActivePecPointList resetPec;

        if ( m_activePecPoints.end() != findIter )
        {
            // take a copy of the PEC
            resetPec.insert( *findIter );

            // remove the PEC from m_activePecPoints
            m_activePecPoints.erase( findIter );

            // if this was the active PEC point (assigned to operator)

            if ( TA_IRS_Bus::PecTypes::PecSourceOperator == resetPec.begin()->source )
            {
                clearCall();
                resetSessionId();
                resetAutoContinueCount();
            }

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
        return resetPec;
    }


    TA_IRS_Bus::PecTypes::ActivePecPointList TrainPecState::resetAllPec( time_t timestamp )
    {
        FUNCTION_ENTRY( "resetAllPec" );

        // create an ActivePecPointList, and switch it with m_activePecPoints
        TA_IRS_Bus::PecTypes::ActivePecPointList resetPec;

        // all pec will go to the reset list
        resetPec.swap( m_activePecPoints );

        // anything ongoing will be cleared
        clearCall();
        resetSessionId();
        resetAutoContinueCount();

        setDataChangedWithTimestamp( timestamp );

        FUNCTION_EXIT;
        return resetPec;
    }


    const void TrainPecState::resetAutoContinueCount( time_t timestamp )
    {
        FUNCTION_ENTRY( "resetAutoContinueCount" );

        if ( ( true == m_continueRequired ) ||
             ( 0 != m_autoContinueCounter ) )
        {
            m_continueRequired = false;
            m_autoContinueCounter = 0;

            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::PecTypes::ActivePecPointList TrainPecState::invalidateAllPec( time_t timestamp )
    {
        FUNCTION_ENTRY( "invalidateAllPec" );

        // go through all points in m_activePecPoints
        // mark each one invalid (unless the source is operator)

        TA_IRS_Bus::PecTypes::ActivePecPointList tempList;

        for ( TA_IRS_Bus::PecTypes::ActivePecPointList::iterator pointIter = m_activePecPoints.begin();
              m_activePecPoints.end() != pointIter; ++pointIter )
        {
            TA_IRS_Bus::PecTypes::PecPoint point( *pointIter );

            if ( TA_IRS_Bus::PecTypes::PecSourceOperator != point.source )
            {
                point.valid = false;
            }

            tempList.insert( point );
        }

        m_activePecPoints.swap( tempList );

        if ( false == m_activePecPoints.empty() )
        {
            setDataChangedWithTimestamp( timestamp );
        }

        FUNCTION_EXIT;
        return m_activePecPoints;
    }


    TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState TrainPecState::getSyncState() const
    {
        FUNCTION_ENTRY( "getSyncState" );

        TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState state;

        state.trainId = getTrainId();
        state.activePecPoints = m_activePecPoints;
        state.consoleId = m_consoleId;
        state.sessionId = m_sessionId;
        state.callId = m_callId;
        state.autoContinueCounter = m_autoContinueCounter;
        state.continueRequired = m_continueRequired;
        state.timestamp = getTimestamp();

        FUNCTION_EXIT;
        return state;
    }


    void TrainPecState::setSyncState( const TA_IRS_Bus::StateUpdateTypes::TrainPecSyncState& state )
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

        m_activePecPoints = state.activePecPoints;
        m_consoleId = state.consoleId;
        m_sessionId = state.sessionId;
        m_callId = state.callId;
        m_autoContinueCounter = state.autoContinueCounter;
        m_continueRequired = state.continueRequired;

        // the data has changed, but a sync is not required
        setDataChangedWithTimestamp( state.timestamp, false );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::PecTypes::PecPoint TrainPecState::updatePec( const TA_IRS_Bus::PecTypes::PecPoint& pec,
                                                             time_t timestamp )
    {
        FUNCTION_ENTRY( "updatePec" );

        // search the active PEC points for this PEC
        TA_IRS_Bus::PecTypes::ActivePecPointList::iterator findIter = m_activePecPoints.find( pec );

        // if it is not found
        if ( m_activePecPoints.end() == findIter )
        {
            // insert the PecPoint into the active pec points
            m_activePecPoints.insert( pec );

            setDataChangedWithTimestamp( timestamp );

            // this one can be returned straight away as it is unmodified
            FUNCTION_EXIT;
            return pec;
        }

        // the PEC was found, it must be updated

        if ( ( pec.source != findIter->source ) || // first check the source
             ( pec.valid != findIter->valid ) ) // second check the validity
        {
            // update it
            TA_IRS_Bus::PecTypes::PecPoint modifiedPoint( *findIter );
            m_activePecPoints.erase( findIter );
            
            modifiedPoint.source = pec.source;

            // now if this PEC was invalid, then this is a new activation
            // the timestamp should be updated
            if ( ( false == modifiedPoint.valid ) &&
                 ( true == pec.valid ) )
            {
                modifiedPoint.creationTime = pec.creationTime;
            }

            modifiedPoint.valid = pec.valid;

            m_activePecPoints.insert( modifiedPoint );

            setDataChangedWithTimestamp( timestamp );

            // return the modified PEC
            FUNCTION_EXIT;
            return modifiedPoint;
        }

        // return the unmodified PEC
        FUNCTION_EXIT;
        return ( *findIter );
    }

} // TA_IRS_App

// STL Helper function implementation

namespace
{
    // used to check if a PecPoint is answered
    bool isPecAnsweredByOperator( const TA_IRS_Bus::PecTypes::PecPoint& point )
    {
        return ( TA_IRS_Bus::PecTypes::PecSourceOperator == point.source );
    }

    // used to check if a PecPoint is answered
    bool isPecAnsweredByDriver( const TA_IRS_Bus::PecTypes::PecPoint& point )
    {
        return ( TA_IRS_Bus::PecTypes::PecSourceDriver == point.source );
    }
}
