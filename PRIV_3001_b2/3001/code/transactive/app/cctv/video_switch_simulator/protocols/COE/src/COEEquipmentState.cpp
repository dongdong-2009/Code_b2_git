/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Singleton to store state of COE Hardware.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/cctv/video_switch_simulator/protocols/COE/src/COEEquipmentState.h"
#include "app/cctv/video_switch_simulator/protocols/COE/src/COESimulator.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{
    const COEEquipmentState::QuadState INITITAL_QUAD_STATE( 8, 0x00 );
    
    // Initialise singleton and threadlock Statics
    COEEquipmentState* COEEquipmentState::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable COEEquipmentState::m_singletonLock;
    TA_Base_Core::ReEntrantThreadLockable COEEquipmentState::m_commandLineOutLock;


    //
    // getInstance
    //
    COEEquipmentState& COEEquipmentState::getInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance == 0 )
        {
            m_theInstance = new COEEquipmentState();
        }

        return *m_theInstance;
    }


    //
    // removeInstance
    //
    void COEEquipmentState::removeInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance != 0 )
        {
            delete m_theInstance;
            m_theInstance = 0;
        }
    }


    //
    // registerSimulator
    //
    void COEEquipmentState::registerSimulator( const std::string& location, COESimulator& sim )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        COESimulator* pSim = dynamic_cast< COESimulator* >( &sim );
        TA_ASSERT( pSim, "Cast from ref to pointer failed!" );
        
        m_locationToSimulator.insert( LocationToSimulator::value_type( location, pSim ) );
    }


    //
    // deRegisterSimulator
    //
    void COEEquipmentState::deRegisterSimulator( const std::string& location, COESimulator& sim )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        COESimulator* pSim = dynamic_cast< COESimulator* >( &sim );
        TA_ASSERT( pSim, "Cast from ref to pointer failed!" );

        std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
            m_locationToSimulator.equal_range( location );
        while ( range.first != range.second )
        {
            if ( pSim == ( *range.first ).second )
            {
                m_locationToSimulator.erase( range.first ); 
                return;
            }
            range.first++;
        }
    }


    //
    // acquireCommandLineOutLock
    //
    void COEEquipmentState::acquireCommandLineOutLock()
    {
        m_commandLineOutLock.acquire();
    }


    //
    // releaseCommandLineOutLock
    //
    void COEEquipmentState::releaseCommandLineOutLock()
    {
        m_commandLineOutLock.release();
    }


    //
    // getSummaryAlarm
    //
    bool COEEquipmentState::getSummaryAlarm( ESummaryAlarmType type )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        return m_summaryAlarms.test( type );
    }


    //
    // setSummaryAlarm
    // 
    void COEEquipmentState::setSummaryAlarm( ESummaryAlarmType type, bool alarmVal )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_summaryAlarms.set( type, alarmVal );
    }


    //
    // getMonitorState
    //
    COEEquipmentState::MonitorState& COEEquipmentState::getMonitorState( const std::string& location, unsigned char monitorID )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

		if(monitorID == 11)
		{
			int hello=0;
		}
        // If exists return the state.
        LocationAndMonitorIDToState::iterator locAndMonIDToStateIt
            = m_locationAndMonitorIDToState.find( location );
        if ( locAndMonIDToStateIt != m_locationAndMonitorIDToState.end() )
        {
            MonitorIDToState::iterator monIDToStateIt 
                = ( *locAndMonIDToStateIt ).second.find( monitorID );
            if ( monIDToStateIt != ( *locAndMonIDToStateIt ).second.end() )
            {
                return ( *monIDToStateIt ).second;
            }
        }

        // Else set an initial state and return it.
        MonitorState state;
		TriggeringEventList list;
		state.monitorId = monitorID;
		state.assignmentType = COEEquipmentState::PARKED;
		state.assignedDeviceID = 0x00;
		state.assignedDeviceLocationKey = 0x00;
		state.alarmStack = list;
		m_locationAndMonitorIDToState[ location ][ monitorID ] = state;
		
        return state;
    }


    //
    // setMonitorState
    //
    void COEEquipmentState::setMonitorState( const std::string& location, unsigned char monitorID, const MonitorState& state, bool sendUpdate )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if(monitorID == 11)
		{
			int hello=0;
		}
        m_locationAndMonitorIDToState[ location ][ monitorID ] = state;

        std::cout << location << " Monitor " << static_cast< unsigned short >( monitorID ) << " ";
        std::cout << "assigned with ";
        switch ( state.assignmentType )
        {
        case NOTHING:
            {
                std::cout << "nothing.";
            }
            break;
        case CAMERA:
            {
                std::cout << "Camera " << static_cast< unsigned short >( state.assignedDeviceLocationKey ) << ":";
                std::cout << static_cast< unsigned short >( state.assignedDeviceID ) << ".";
            }
            break;
        case SEQUENCE:
            {
                std::cout << "Sequence " << static_cast< unsigned short >( state.assignedDeviceID ) << ".";
            }
            break;
        case QUAD:
            {
                std::cout << "Quad " << static_cast< unsigned short >( state.assignedDeviceID ) << ".";
            }
            break;
        case TRAIN:
            {
                std::cout << "Active Train for BVS Stage " << static_cast< unsigned short >( state.assignedDeviceLocationKey ) << ".";
            }
            break;
        case ALARM:
            {
                std::cout << "Alarm/s.";
            }
            break;
        default:
            {
                std::cout << "unknown!";
            }
            break;
        }
        std::cout << std::endl;

        if(sendUpdate)
		{
			std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
				m_locationToSimulator.equal_range( location );
			while ( range.first != range.second )
			{
				( *( *range.first ).second ).updateObserversWithMonitorState( monitorID, state );
				range.first++;
			}
		}
    }


    //
    // getSequenceState
    //
    COEEquipmentState::SequenceState COEEquipmentState::getSequenceState( const std::string& location, unsigned char sequenceID )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        // If exists return the state.
        LocationAndSequenceIDToState::iterator locAndSeqIDToStateIt
            = m_locationAndSequenceIDToState.find( location );
        if ( locAndSeqIDToStateIt != m_locationAndSequenceIDToState.end() )
        {
            SequenceIDToState::iterator seqIDToStateIt 
                = ( *locAndSeqIDToStateIt ).second.find( sequenceID );
            if ( seqIDToStateIt != ( *locAndSeqIDToStateIt ).second.end() )
            {
                return ( *seqIDToStateIt ).second;
            }
        }

        // Else set an initial state and return it.
        SequenceState initialState;
        initialState.dwellTime = 0;
        initialState.orderedCameras.clear();
        m_locationAndSequenceIDToState[ location ][ sequenceID ] = initialState;
        return initialState;
    }


    //
    // setSequenceState
    //
    void COEEquipmentState::setSequenceState( const std::string& location, unsigned char sequenceID, const SequenceState& state )
    {
        TA_ASSERT( ( ( state.orderedCameras.size() % 2 ) == 0 ), "Not even number of Station ID to Camera ID count!" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_locationAndSequenceIDToState[ location ][ sequenceID ] = state;

        std::cout << location << " Sequence " << static_cast< unsigned short >( sequenceID ) << " ";
        std::cout << "programmed with:" << std::endl;
        std::cout << "Dwell Time - " << state.dwellTime << std::endl;
        unsigned short camNumber = 1;
        std::vector< unsigned char >::const_iterator it = state.orderedCameras.begin();
        while ( it != state.orderedCameras.end() )
        {
            std::cout << "Sequence Camera " << camNumber << " - " <<  static_cast< unsigned short >( *it );
            it++;
            std::cout << ":" << static_cast< unsigned short >( *it ) << std::endl;
            it++;
            camNumber++;
        }

        std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
            m_locationToSimulator.equal_range( location );
        while ( range.first != range.second )
        {
            ( *( *range.first ).second ).updateObserversWithSequenceState( sequenceID, state );
            range.first++;
        }
    }


    //
    // getQuadState
    //
    COEEquipmentState::QuadState COEEquipmentState::getQuadState( const std::string& location, unsigned char quadID )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        // If exists return the state.
        LocationAndQuadIDToState::iterator locAndQuadIDToStateIt
            = m_locationAndQuadIDToState.find( location );
        if ( locAndQuadIDToStateIt != m_locationAndQuadIDToState.end() )
        {
            QuadIDToState::iterator quadIDToStateIt 
                = ( *locAndQuadIDToStateIt ).second.find( quadID );
            if ( quadIDToStateIt != ( *locAndQuadIDToStateIt ).second.end() )
            {
                return ( *quadIDToStateIt ).second;
            }
        }

        // Else set an initial state and return it.
        m_locationAndQuadIDToState[ location ][ quadID ] = INITITAL_QUAD_STATE;
        return INITITAL_QUAD_STATE;
    }


    //
    // setQuadState
    //
    void COEEquipmentState::setQuadState( const std::string& location, unsigned char quadID, const QuadState& state )
    {
        TA_ASSERT( ( state.size() == 8 ), "Need 4 Station:Camera pairs to capture Quad State!" );

        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_locationAndQuadIDToState[ location ][ quadID ] = state;

        std::cout << location << " Quad " << static_cast< unsigned short >( quadID ) << " ";
        std::cout << "set with following cameras as in inputs:" << std::endl;
        unsigned short segNumber = 1;
        std::vector< unsigned char >::const_iterator it = state.begin();
        while ( it != state.end() )
        {
            std::cout << "Quad Segment " << segNumber << " Camera" << " - " <<  static_cast< unsigned short >( *it );
            it++;
            std::cout << ":" << static_cast< unsigned short >( *it ) << std::endl;
            it++;
            segNumber++;
        }

        std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
            m_locationToSimulator.equal_range( location );
        while ( range.first != range.second )
        {
            ( *( *range.first ).second ).updateObserversWithQuadState( quadID, state );
            range.first++;
        }
    }


    //
    // getRecordingUnitState
    //
    COEEquipmentState::ERecordingUnitState COEEquipmentState::getRecordingUnitState( const std::string& location, unsigned char recordingUnitID )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        // If exists return the state.
        LocationAndRecordingUnitIDToState::iterator locAndRecIDToStateIt
            = m_locationAndRecordingUnitIDToState.find( location );
        if ( locAndRecIDToStateIt != m_locationAndRecordingUnitIDToState.end() )
        {  
            RecordingUnitIDToState::iterator recIDToStateIt 
                = ( *locAndRecIDToStateIt ).second.find( recordingUnitID );
            if ( recIDToStateIt != ( *locAndRecIDToStateIt ).second.end() )
            {
                return ( *recIDToStateIt ).second;
            }
        }

        // Else set an initial state and return it.
        m_locationAndRecordingUnitIDToState[ location ][ recordingUnitID ] = TIME_LAPSE_RECORDING;
        return TIME_LAPSE_RECORDING;
    }


    //
    // setMonitorState
    //
    void COEEquipmentState::setRecordingUnitState( const std::string& location, unsigned char recordingUnitID, ERecordingUnitState state )
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        m_locationAndRecordingUnitIDToState[ location ][ recordingUnitID ] = state;

        std::cout << location << " VCR " << static_cast< unsigned short >( recordingUnitID ) << " ";
        std::cout << "set to ";
        switch ( state )
        {
        case NO_RECORDING:
            {
                std::cout << "no recording.";
            }
            break;
        case REAL_TIME_RECORDING:
            {
                std::cout << "real-time recording mode.";
            }
            break;
        case TIME_LAPSE_RECORDING:
            {
                std::cout << "time-lapse recording mode.";
            }
            break;
        default:
            {
                std::cout << "unknown!";
            }
            break;
        }
        std::cout << std::endl;

        std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
            m_locationToSimulator.equal_range( location );
        while ( range.first != range.second )
        {
            ( *( *range.first ).second ).updateObserversWithRecordingUnitState( recordingUnitID, state );
            range.first++;
        }
    }


	TriggeringEventList& COEEquipmentState::getTriggeringEvents( const std::string& location)
    {
		// Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );
		
		if(m_locationToTriggeringEventList.find(location) == m_locationToTriggeringEventList.end())
		{
			TriggeringEventList list;
			m_locationToTriggeringEventList[location] = list;
		}
		
		LocationToTriggeringEventList::iterator it = m_locationToTriggeringEventList.find(location);
		
		return (*it).second;
	}
    
    void COEEquipmentState::openTriggeringEvent( const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId )
    {
		// Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );
		
		if(m_locationToTriggeringEventList.find(location) == m_locationToTriggeringEventList.end())
		{
			TriggeringEventList list;
			m_locationToTriggeringEventList[location] = list;
		}
		
		LocationToTriggeringEventList::iterator it = m_locationToTriggeringEventList.find(location);
		
		TriggeringEventList& eventList = (*it).second;
		TriggeringEventList::iterator itr;
		bool exsisting = false;
		
		for(itr=eventList.begin(); itr!=eventList.end(); itr++)
		{
			if((*itr).stationId == stationId && (*itr).eventId == eventId)
			{
				exsisting=true;
			}
		}
		if(exsisting!=true && eventId!=0)
		{
			//if the event id is 0, this is a train alarm and doesnt go in the main stack
			// they only go on monitor stacks
			//only add it to the list if it doesnt already exsist
			TriggeringEvent trig;
			trig.stationId = stationId;
			trig.eventId = eventId;
			eventList.push_back(trig);
		}
		
		MonitorState& monitor = getMonitorState(location, monitorId);

		if(monitor.assignmentType == ALARM)
		{
			bool monExsisting=false;
			//check if this monitor already has this alarm;
			TriggeringEventList::iterator monItr;
			{
				for(monItr=monitor.alarmStack.begin(); monItr!=monitor.alarmStack.end(); monItr++)
				{
					if((*monItr).eventId == eventId && (*monItr).stationId == stationId)
					{
						monExsisting=true;
					}
				}
				if(!monExsisting)
				{
					TriggeringEvent alarmState;
					alarmState.stationId=stationId;
					alarmState.eventId=eventId;
					monitor.alarmStack.push_back(alarmState);

					std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
					m_locationToSimulator.equal_range( location );
					while ( range.first != range.second )
					{
						( *( *range.first ).second ).updateObserversWithMonitorState( monitorId, monitor );
						range.first++;
					}
				}
			}
		}
		else
		{
			MonitorState state;
			state.monitorId = monitorId;
			state.assignmentType=ALARM;
			state.assignedDeviceID=static_cast< unsigned char >(0);
			state.assignedDeviceLocationKey=static_cast< unsigned char >(0);
			TriggeringEvent alarmState;
			alarmState.stationId=stationId;
			alarmState.eventId=eventId;
			state.alarmStack.push_back(alarmState);
			setMonitorState(location,monitorId,state,true);
		}

		//not sure if hardware will resend but cctv agent can cope with this so we will
        std::cout << location << " stationId " << static_cast< unsigned short >( stationId );
        std::cout << ", eventId " << static_cast< unsigned short >( eventId );
		std::cout << " triggering event opened." << std::endl;
        
        std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
            m_locationToSimulator.equal_range( location );
        while ( range.first != range.second )
        {
            ( *( *range.first ).second ).updateObserversWithTriggeringEvents( eventList );
            range.first++;
        }
	}
    
    void COEEquipmentState::closeTriggeringEvent( const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId /* =0 */)
    {
		// Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );
		
		bool exsisting = false;
		
		LocationToTriggeringEventList::iterator it = m_locationToTriggeringEventList.find(location);
		
		if(it != m_locationToTriggeringEventList.end())
		{
			TriggeringEventList& eventList = (*it).second;
			TriggeringEventList::iterator itr;

			for(itr=eventList.begin(); itr!=eventList.end(); itr++)
			{
				if((*itr).stationId == stationId && (*itr).eventId == eventId)
				{
					exsisting=true;
					eventList.erase(itr);
					std::cout << "closed triggering event stationId: ";
					std::cout << static_cast< unsigned short >( stationId ) << ", EventId: ";
					std::cout << static_cast< unsigned short >( eventId ) << std::endl;

					std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
						m_locationToSimulator.equal_range( location );
					while ( range.first != range.second )
					{
						// If exists return the state.
						( *( *range.first ).second ).updateObserversWithTriggeringEvents( eventList );
						range.first++;
					}
					break;
				}
			}
	
			LocationAndMonitorIDToState::iterator locAndMonIDToStateIt;
			for(locAndMonIDToStateIt = m_locationAndMonitorIDToState.begin();
					locAndMonIDToStateIt != m_locationAndMonitorIDToState.end();
					locAndMonIDToStateIt++)
			{//for every location
				MonitorIDToState::iterator monIDToStateIt; 
				for(monIDToStateIt = ( *locAndMonIDToStateIt ).second.begin();
						monIDToStateIt != ( *locAndMonIDToStateIt ).second.end();
						monIDToStateIt++)
				{//for every monitor at that location
					//if monitorId is not set we do all monitors
					if( monitorId == 0 || ( *monIDToStateIt ).second.monitorId == monitorId )
					{
						TriggeringEventList::iterator triggItr;
						for(triggItr = ( *monIDToStateIt ).second.alarmStack.begin();
								triggItr != ( *monIDToStateIt ).second.alarmStack.end();
								triggItr++)
						{//check every alarm in each monitors alarm stack
							if((*triggItr).stationId == stationId && (*triggItr).eventId == eventId)
							{
								( *monIDToStateIt ).second.alarmStack.erase(triggItr);
								if(( *monIDToStateIt ).second.alarmStack.size()==0)
								{
									( *monIDToStateIt ).second.assignmentType = COEEquipmentState::PARKED;
									( *monIDToStateIt ).second.assignedDeviceLocationKey = static_cast <unsigned char>(0);
									( *monIDToStateIt ).second.assignedDeviceID = static_cast <unsigned char>(0);
								}
								std::pair< LocationToSimulator::iterator, LocationToSimulator::iterator > range =
								m_locationToSimulator.equal_range( location );
								while ( range.first != range.second )
								{
									( *( *range.first ).second ).updateObserversWithMonitorState( ( *monIDToStateIt ).second.monitorId, ( *monIDToStateIt ).second );
									range.first++;
								}
								break;
							}
						}
					}
				}
			}
		}
		if(!exsisting || eventId!=0)
		{
			std::cout << "cant close an event that isn't open" << std::endl;
		}
	}

    void COEEquipmentState::listOpenTriggeringEvents( const std::string& location)
	{
		// Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );
		
		LocationToTriggeringEventList::iterator it = m_locationToTriggeringEventList.find(location);
		if(it != m_locationToTriggeringEventList.end())
		{
			TriggeringEventList& eventList = (*it).second;
			TriggeringEventList::iterator itr;

			if(eventList.begin() != eventList.end())
			{
				std::cout << "Triggering events for location: " << location << std::endl;

				for(itr=eventList.begin(); itr!=eventList.end(); itr++)
				{
					std::cout << "StationId: " << static_cast< unsigned short >( (*itr).stationId );
					std::cout << ", EventId: " << static_cast< unsigned short >( (*itr).eventId );
					std::cout << std::endl;
				}
			}
			else
			{
				std::cout << " no events for location: " << location << std::endl;
			}
		}
		else
		{
			std::cout << " no events for location: " << location << std::endl;
		}
	}

	void COEEquipmentState::ackTriggeringEvent(const std::string& location, unsigned char stationId, unsigned char eventId, unsigned char monitorId)
	{
		openTriggeringEvent( location, stationId, eventId, monitorId );
	}
    
    //
    // Constructor
    //
    COEEquipmentState::COEEquipmentState()
        : m_summaryAlarms()
    {
	}


} // namespace TA_IRS_App