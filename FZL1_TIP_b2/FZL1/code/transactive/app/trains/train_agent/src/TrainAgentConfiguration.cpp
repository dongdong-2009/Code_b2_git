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

#include "app/trains/train_agent/src/TrainAgentConfiguration.h"

#include "bus/trains/TrainCommonLibrary/src/AtsZoneUtils.h"
#include "bus/trains/TrainCommonLibrary/src/CctvException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/TrainStateDai/src/PersistedTrainStateAccessFactory.h"
#include "core/data_access_interface/TrainRadioGroupiDai/src/TrainRadioGroupAccessFactory.h"

#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "core/utilities/src/DebugUtil.h"

#include <map>
#include <set>
#include <math.h>

namespace TA_IRS_App
{

    TrainAgentConfiguration::TrainAgentConfiguration( TA_Base_Core::TrainAgentEntityDataPtr agentEntity )
            : m_agentEntityData( agentEntity )
    {
        FUNCTION_ENTRY( "TrainAgentConfiguration" );

        // load all data from the database
        loadAndValidateData();

        FUNCTION_EXIT;
    }


    TrainAgentConfiguration::~TrainAgentConfiguration()
    {
        FUNCTION_ENTRY( "~TrainAgentConfiguration" );
        FUNCTION_EXIT;
    }


    unsigned long TrainAgentConfiguration::getEntityKey()
    {
        FUNCTION_ENTRY( "getEntityKey" );
        FUNCTION_EXIT;
        return m_agentEntityData->getKey();
    }


    const std::string TrainAgentConfiguration::getEntityName()
    {
        FUNCTION_ENTRY( "getEntityName" );
        FUNCTION_EXIT;
        return m_agentEntityData->getName();
    }


    unsigned long TrainAgentConfiguration::getEntityTypeKey()
    {
        FUNCTION_ENTRY( "getEntityTypeKey" );
        FUNCTION_EXIT;
        return m_agentEntityData->getTypeKey();
    }


    unsigned long TrainAgentConfiguration::getLocationKey()
    {
        FUNCTION_ENTRY( "getLocationKey" );
        FUNCTION_EXIT;
        return m_agentEntityData->getLocation();
    }


    TA_Base_Core::ILocation::ELocationType TrainAgentConfiguration::getAgentLocationType()
    {
        FUNCTION_ENTRY( "getAgentLocationType" );

        // find the entry in m_locationInfoMap and return the location type

        LocationInfoMap::iterator findIter = m_locationInfoMap.find( getLocationKey() );

        // throw a DataException if the data isnt found

        if ( m_locationInfoMap.end() == findIter )
        {
            std::ostringstream locationStream;
            locationStream << getLocationKey();

            TA_THROW( TA_Base_Core::DataException( "Agent Location not found",
                                                   TA_Base_Core::DataException::NO_VALUE,
                                                   locationStream.str() ) );
        }

        // found it

        FUNCTION_EXIT;
        return findIter->second.type;
    }


    unsigned long TrainAgentConfiguration::getRightsSubsystem()
    {
        FUNCTION_ENTRY( "getRightsSubsystem" );

        // get the agent location type
        TA_Base_Core::ILocation::ELocationType agentLocationType = getAgentLocationType();

        switch ( agentLocationType )
        {
            // if this is the OCC, return the subsystem from m_agentEntityData
            case TA_Base_Core::ILocation::OCC:
                FUNCTION_EXIT;
                return m_agentEntityData->getSubsystem();

            // if this is a station or depot, return the physical subsystem from m_agentEntityData
            case TA_Base_Core::ILocation::STATION:

            case TA_Base_Core::ILocation::DPT:
                FUNCTION_EXIT;
                return m_agentEntityData->getPhysicalSubsystem();

            default:
                TA_ASSERT( false, "Invalid Location type" );
        }

        FUNCTION_EXIT;
        return 0;
    }


    unsigned long TrainAgentConfiguration::getDutyEntity()
    {
        FUNCTION_ENTRY( "getDutyEntity" );

        // get the agent location type
        TA_Base_Core::ILocation::ELocationType agentLocationType = getAgentLocationType();

        switch ( agentLocationType )
        {
                // if this is the OCC, return the entity key from m_globalEntityData

            case TA_Base_Core::ILocation::OCC:
                FUNCTION_EXIT;
                return m_globalEntityData->getKey();

                // if this is a station or depot, return the entity key from m_agentEntityData

            case TA_Base_Core::ILocation::STATION:

            case TA_Base_Core::ILocation::DPT:
                FUNCTION_EXIT;
                return m_agentEntityData->getKey();

            default:
                TA_ASSERT( false, "Invalid Location type" );
        }

        FUNCTION_EXIT;
        return 0;
    }


    unsigned long TrainAgentConfiguration::getDutySubsystem()
    {
        FUNCTION_ENTRY( "getDutySubsystem" );

        // get the agent location type
        TA_Base_Core::ILocation::ELocationType agentLocationType = getAgentLocationType();

        switch ( agentLocationType )
        {
            // if this is the OCC, return the physical subsystem from m_globalEntityData
            case TA_Base_Core::ILocation::OCC:
                FUNCTION_EXIT;
                return m_globalEntityData->getPhysicalSubsystem();

            // if this is a station or depot, return the physical subsystem from m_agentEntityData
            case TA_Base_Core::ILocation::STATION:

            case TA_Base_Core::ILocation::DPT:
                FUNCTION_EXIT;
                return m_agentEntityData->getPhysicalSubsystem();

            default:
                TA_ASSERT( false, "Invalid Location type" );
        }

        FUNCTION_EXIT;
        return 0;
    }


    std::string TrainAgentConfiguration::getAssetName()
    {
        FUNCTION_ENTRY( "getAssetName" );
        FUNCTION_EXIT;
        return m_agentEntityData->getAssetName();
    }


    unsigned char TrainAgentConfiguration::getAgentOrigin()
    {
        FUNCTION_ENTRY( "getAgentOrigin" );
        FUNCTION_EXIT;
        return static_cast< unsigned char >( m_agentEntityData->getAgentOrign() );
    }


    const std::string& TrainAgentConfiguration::getAgentSessionId()
    {
        FUNCTION_ENTRY( "getAgentSessionId" );
        FUNCTION_EXIT;
        return TA_Base_Core::SessionAccessFactory::getSuperSessionId();
    }


    unsigned long TrainAgentConfiguration::getAlarmSubsystem()
    {
        FUNCTION_ENTRY( "getAlarmSubsystem" );

        // get the agent location type
        TA_Base_Core::ILocation::ELocationType agentLocationType = getAgentLocationType();

        switch ( agentLocationType )
        {
            // if this is the OCC, return the logical subsystem from m_agentEntityData
            case TA_Base_Core::ILocation::OCC:
                FUNCTION_EXIT;
                return m_globalEntityData->getSubsystem();

            // if this is a station or depot, return the physical subsystem from m_agentEntityData
            case TA_Base_Core::ILocation::STATION:

            case TA_Base_Core::ILocation::DPT:
                FUNCTION_EXIT;
                return m_agentEntityData->getPhysicalSubsystem();

            default:
                TA_ASSERT( false, "Invalid Location type" );
        }

        FUNCTION_EXIT;
        return 0;
    }


    TA_IRS_Core::IPersistedTrainStateMap TrainAgentConfiguration::getPersistedTrainStates()
    {
        FUNCTION_ENTRY( "getPersistedTrainStates" );

        try
        {
            FUNCTION_EXIT;
            return TA_IRS_Core::PersistedTrainStateAccessFactory::loadAllTrains();
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        // the compiler doesnt like us not returning something
        FUNCTION_EXIT;
        return TA_IRS_Core::PersistedTrainStateAccessFactory::loadAllTrains();
    }


    unsigned long TrainAgentConfiguration::getConsoleFromSession( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "getConsoleFromSession" );

        try
        {
            // Use the authentication library to get the session information for the given session ID, and from there get the console ID
            TA_Base_Bus::SessionInfo sessionInfo = m_authenticationLibrary.getSessionInfo( sessionId, getAgentSessionId() );

            FUNCTION_EXIT;
            return sessionInfo.WorkstationId;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While retrieving session information" );
        }

        // If this fails, use ConsoleAccessFactory to load the console from the session ID
        try
        {
            boost::shared_ptr<TA_Base_Core::IConsole> console( TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionId ) );

            FUNCTION_EXIT;
            return console->getKey();
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "While retrieving console information from session" );
        }

        // If nothing is still found, return 0
        FUNCTION_EXIT;
        return 0;
    }


    unsigned long TrainAgentConfiguration::getLocationFromConsole( unsigned long consoleKey )
    {
        FUNCTION_ENTRY( "getLocationFromConsole" );

        // find the console key in m_consoleMap
        // return the location key for that console
        ConsoleInfoMap::iterator findIter = m_consoleMap.find( consoleKey );

        if ( m_consoleMap.end() != findIter )
        {
            FUNCTION_EXIT;
            return findIter->second.location;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Given console %d does not appear to be a valid console, not found in register",
                     consoleKey );

        // return getLocationKey() if not found

        FUNCTION_EXIT;
        return getLocationKey();
    }


    unsigned char TrainAgentConfiguration::getConsoleOrigin( unsigned long consoleId )
    {
        FUNCTION_ENTRY( "getConsoleOrigin" );


        // find the console key in m_consoleMap
        // return the origin for that console
        ConsoleInfoMap::iterator findIter = m_consoleMap.find( consoleId );

        if ( m_consoleMap.end() != findIter )
        {
            FUNCTION_EXIT;
            return findIter->second.trainOrigin;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                     "Given console %d does not appear to be a valid console, not found in register",
                     consoleId );

        // return getAgentOrigin() if not found

        FUNCTION_EXIT;
        return getAgentOrigin();
    }


    unsigned char TrainAgentConfiguration::getOperatorOrigin( const std::string& sessionId )
    {
        FUNCTION_ENTRY( "getOperatorOrigin" );
        FUNCTION_EXIT;
        return getConsoleOrigin( getConsoleFromSession( sessionId ) );
    }


    TA_Base_Core::ILocation::ELocationType TrainAgentConfiguration::getLocationType( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getLocationType" );

        // find the given location key in m_locationInfoMap
        // return the type of the location

        LocationInfoMap::iterator findIter = m_locationInfoMap.find( locationKey );

        // ASSERT if not found
        TA_ASSERT( m_locationInfoMap.end() != findIter, "Given location not found in register" )

        FUNCTION_EXIT;
        return findIter->second.type;
    }


    CctvZone TrainAgentConfiguration::getCctvZoneFromLocation( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "getCctvZoneFromLocation" );

        // Look up the m_cctvZoneMap for the given location
        CctvZoneMap::iterator findIter = m_cctvZoneMap.find( locationKey );

        // if the item is found return it

        if ( m_cctvZoneMap.end() != findIter )
        {
            FUNCTION_EXIT;
            return findIter->second;
        }

        // otherwise return an empty CctvZone object
        CctvZone nullZone;

        FUNCTION_EXIT;

        return nullZone;
    }


    unsigned long TrainAgentConfiguration::getTimsAutoContinueAckLimit()
    {
        FUNCTION_ENTRY( "getTimsAutoContinueAckLimit" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimsAutoContinueAckLimit();
    }


    LocationInfoMap TrainAgentConfiguration::getAllLocationsOfType( TA_Base_Core::ILocation::ELocationType locationType )
    {
        FUNCTION_ENTRY( "getAllLocationsOfType" );

        LocationInfoMap matchingLocations;

        // iterate through m_locationInfoMap,
        // find all those locations that match the given location type, add these to a new map

        for ( LocationInfoMap::iterator locationIter = m_locationInfoMap.begin();
              m_locationInfoMap.end() != locationIter; ++locationIter )
        {
            if ( locationType == locationIter->second.type )
            {
                matchingLocations.insert( *locationIter );
            }
        }

        FUNCTION_EXIT;
        return matchingLocations;
    }


    bool TrainAgentConfiguration::isNextToDepot( unsigned long locationKey )
    {
        FUNCTION_ENTRY( "isNextToDepot" );

        // get the location info
        LocationInfoMap::iterator matchingLocation = m_locationInfoMap.find( locationKey );

        if ( m_locationInfoMap.end() == matchingLocation )
        {
            FUNCTION_EXIT;
            return false;
        }

        // get all depot locations
        LocationInfoMap depotLocations = getAllLocationsOfType( TA_Base_Core::ILocation::DPT );

        // iterate through m_locationInfoMap,
        // find all those locations that match the given location type, add these to a new map

        for ( LocationInfoMap::iterator locationIter = depotLocations.begin();
              depotLocations.end() != locationIter; ++locationIter )
        {
            if ( ( (matchingLocation->second.orderId + 1) == locationIter->second.orderId ) ||
                 ( (matchingLocation->second.orderId - 1) == locationIter->second.orderId ) )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }


    TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap TrainAgentConfiguration::getRadioGroups()
    {
        FUNCTION_ENTRY( "getRadioGroups" );

        TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap radioGroups;

        try
        {
            // Using TrainRadioGroupAccessFactory, load all TrainRadioGroups
            typedef std::vector< TA_IRS_Core::ITrainRadioGroupPtr > ITrainRadioGroupPtrList;
            ITrainRadioGroupPtrList radioGroupConfiguration = TA_IRS_Core::TrainRadioGroupAccessFactory::loadAllTrainRadioGroups();

            // package them up to be returned

            for ( ITrainRadioGroupPtrList::iterator radioGroupIter = radioGroupConfiguration.begin();
                  radioGroupConfiguration.end() != radioGroupIter; ++radioGroupIter )
            {
                TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup newGroup;
                newGroup.groupName = ( *radioGroupIter )->getName();
                newGroup.groupTsi = ( *radioGroupIter )->getTsi();
                newGroup.locationCoverage = ( *radioGroupIter )->getLocationCoverage();
                newGroup.locked = false;
                newGroup.lastUpdateTime = 0;

                radioGroups.insert( TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap::value_type( newGroup.groupTsi, newGroup ) );
            }
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        FUNCTION_EXIT;
        return radioGroups;
    }


    unsigned long TrainAgentConfiguration::getTimsRetryLimit()
    {
        FUNCTION_ENTRY( "getTimsRetryLimit" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimsRetryLimit();
    }


    unsigned long TrainAgentConfiguration::getDefaultTimsMessageTimeout()
    {
        FUNCTION_ENTRY( "getDefaultTimsMessageTimeout" );
        FUNCTION_EXIT;
        return m_globalEntityData->getDefaultTimsMessageTimeout();
    }


    unsigned long TrainAgentConfiguration::getTimerISCS1()
    {
        FUNCTION_ENTRY( "getTimerISCS1" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerISCS1();
    }


    unsigned long TrainAgentConfiguration::getTimerISCS2()
    {
        FUNCTION_ENTRY( "getTimerISCS2" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerISCS2();
    }


    unsigned long TrainAgentConfiguration::getTimerCCTV1()
    {
        FUNCTION_ENTRY( "getTimerCCTV1" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerCCTV1();
    }


    unsigned long TrainAgentConfiguration::getTimerVC1()
    {
        FUNCTION_ENTRY( "getTimerVC1" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerVC1();
    }


    unsigned long TrainAgentConfiguration::getTimerVC2()
    {
        FUNCTION_ENTRY( "getTimerVC2" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerVC2();
    }


    unsigned long TrainAgentConfiguration::getTimerPEC2()
    {
        FUNCTION_ENTRY( "getTimerPEC2" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPEC2();
    }


    unsigned long TrainAgentConfiguration::getTimerPEC4()
    {
        FUNCTION_ENTRY( "getTimerPEC4" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPEC4();
    }


    unsigned long TrainAgentConfiguration::getTimerPA1()
    {
        FUNCTION_ENTRY( "getTimerPA1" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPA1();
    }


    unsigned long TrainAgentConfiguration::getTimerPA3()
    {
        FUNCTION_ENTRY( "getTimerPA3" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPA3();
    }


    unsigned long TrainAgentConfiguration::getTimerPA2()
    {
        FUNCTION_ENTRY( "getTimerPA2" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPA2();
    }


    unsigned long TrainAgentConfiguration::getTimerPA4()
    {
        FUNCTION_ENTRY( "getTimerPA4" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerPA4();
    }


    unsigned long TrainAgentConfiguration::getTimerTIS1()
    {
        FUNCTION_ENTRY( "getTimerTIS1" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerTIS1();
    }


    unsigned long TrainAgentConfiguration::getTimerTIS2()
    {
        FUNCTION_ENTRY( "getTimerTIS2" );
        FUNCTION_EXIT;
        return m_globalEntityData->getTimerTIS2();
    }


    unsigned long TrainAgentConfiguration::getOa1Timeout()
    {
        FUNCTION_ENTRY( "getOa1Timeout" );
        FUNCTION_EXIT;
        return m_globalEntityData->getOa1Timeout();
    }


    unsigned long TrainAgentConfiguration::getAtcTimeout()
    {
        FUNCTION_ENTRY( "getAtcTimeout" );
        FUNCTION_EXIT;
        return m_globalEntityData->getAtcTimeout();
    }


    unsigned long TrainAgentConfiguration::getMaxThreadPoolSize()
    {
        FUNCTION_ENTRY( "getMaxThreadPoolSize" );
        FUNCTION_EXIT;
        return m_globalEntityData->getMaxThreadPoolSize();
    }


    TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera TrainAgentConfiguration::getPecCamera( TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber car,
            TA_IRS_Bus::ProtocolPecTypes::EPecNumber pec )
    {
        FUNCTION_ENTRY( "getPecCamera" );

        // If Either all car or all pec is given, throw a CctvException

        if ( ( TA_IRS_Bus::ProtocolPecTypes::AllCars == car ) ||
             ( TA_IRS_Bus::ProtocolPecTypes::AllPec == pec ) )
        {
            TA_THROW( TA_IRS_Bus::CctvException( "Invalid PEC given for camera mapping", TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );
        }

        TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera;

        // Depending on the Car and Pec parameters,
        // call the appropriate method from m_globalEntityData, and convert the value into a ECctvCamera.

        switch ( car )
        {
            case TA_IRS_Bus::ProtocolPecTypes::CarOne:
            {
                switch ( pec )
                {
                    case TA_IRS_Bus::ProtocolPecTypes::PecOne:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarOnePecOneCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecTwo:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarOnePecTwoCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecThree:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarOnePecThreeCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecFour:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarOnePecFourCamera() );
                        break;

                    default:
                        std::ostringstream value;
                        value << pec;
                        TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid PEC number", "PEC Number", value.str() ) );
                        break;
                }
            }
            break;

            case TA_IRS_Bus::ProtocolPecTypes::CarTwo:
            {
                switch ( pec )
                {
                    case TA_IRS_Bus::ProtocolPecTypes::PecOne:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarTwoPecOneCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecTwo:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarTwoPecTwoCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecThree:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarTwoPecThreeCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecFour:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarTwoPecFourCamera() );
                        break;

                    default:
                        std::ostringstream value;
                        value << pec;
                        TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid PEC number", "PEC Number", value.str() ) );
                        break;
                }
            }
            break;

            case TA_IRS_Bus::ProtocolPecTypes::CarThree:
            {
                switch ( pec )
                {
                    case TA_IRS_Bus::ProtocolPecTypes::PecOne:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarThreePecOneCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecTwo:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarThreePecTwoCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecThree:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarThreePecThreeCamera() );
                        break;

                    case TA_IRS_Bus::ProtocolPecTypes::PecFour:
                        camera = static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>( m_globalEntityData->getCarThreePecFourCamera() );
                        break;

                    default:
                        std::ostringstream value;
                        value << pec;
                        TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid PEC number", "PEC Number", value.str() ) );
                        break;
                }
            }
            break;

            default:
            {
                std::ostringstream value;
                value << car;
                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid PEC car", "PEC Car", value.str() ) );
            }
            break;
        }

        // If it cannot be converted, throw an InvalidParameterException
        switch ( camera )
        {
            // valid values
            case TA_IRS_Bus::ProtocolCctvTypes::CameraOne:
                // fall through

            case TA_IRS_Bus::ProtocolCctvTypes::CameraTwo:
                // fall through

            case TA_IRS_Bus::ProtocolCctvTypes::CameraThree:
                // fall through

            case TA_IRS_Bus::ProtocolCctvTypes::CameraFour:
                // fall through

            case TA_IRS_Bus::ProtocolCctvTypes::CameraFive:
                // fall through

            case TA_IRS_Bus::ProtocolCctvTypes::CameraSix:
                // fall through
            
            case TA_IRS_Bus::ProtocolCctvTypes::CameraSeven:
                // fall through
            
            case TA_IRS_Bus::ProtocolCctvTypes::CameraEight:
                // fall through
                break;

            // invalid values
            case TA_IRS_Bus::ProtocolCctvTypes::CameraNone:
                // fall through

            default:
                TA_THROW( TA_IRS_Bus::CctvException( "Invalid PEC camera mapping configuration", TA_IRS_Bus::ProtocolCctvTypes::CctvFailed ) );
                break;
        }

        FUNCTION_EXIT;
        return camera;
    }


    void TrainAgentConfiguration::loadAndValidateData()
    {
        FUNCTION_ENTRY( "loadAndValidateData" );

        try
        {
            // from the database, load entities of type TrainSettingsEntityData (Using EntityAccessFactory)
            TA_Base_Core::IEntityDataList globalEntities =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType( TA_Base_Core::TrainSettingsEntityData::getStaticType() );

            // if there is not exactly one entity returned throw a FatalConfigurationException
            if ( globalEntities.size() != 1 )
            {
                for ( TA_Base_Core::IEntityDataList::iterator globalEntitiesiter = globalEntities.begin();
                      globalEntities.end() != globalEntitiesiter; ++globalEntitiesiter )
                {
                    delete ( *globalEntitiesiter );
                    ( *globalEntitiesiter ) = NULL;
                }

                TA_THROW( TA_Base_Core::FatalConfigurationException( "There must be exactly one TrainSettings entity in the database" ) );
            }
            
            TA_Base_Core::TrainSettingsEntityData* globalEntityData = dynamic_cast< TA_Base_Core::TrainSettingsEntityData* >( globalEntities[0] );
            
            TA_ASSERT( NULL != globalEntityData, "Cast failed for correct type" );

            // set m_globalEntityData to the data that was returned
            m_globalEntityData.reset( globalEntityData );
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        // Using LocationAccessFactory, load all locations from the database.
        // Populate m_locationInfoMap with the data
        try
        {
            typedef std::vector<TA_Base_Core::ILocation*> LocationList;
            LocationList allLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

            for ( LocationList::iterator locationIter = allLocations.begin();
                  allLocations.end() != locationIter; ++locationIter )
            {
                LocationInfo newLocation;
                newLocation.key = ( *locationIter )->getKey();
                newLocation.name = ( *locationIter )->getName();
                newLocation.type = ( *locationIter )->getLocationType();
                newLocation.orderId = ( *locationIter )->getOrderId();

                m_locationInfoMap.insert( LocationInfoMap::value_type( newLocation.key, newLocation ) );

                delete ( *locationIter );
                ( *locationIter ) = NULL;
            }
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        // Using ConsoleAccessFactory, load all consoles from the database
        // populate m_consoleMap with the console key -> ConsoleInfo struct
        // If a console does not have an origin, populate it with getAgentOrigin()
        try
        {
            typedef std::vector<TA_Base_Core::IConsole*> ConsoleList;
            ConsoleList allConsoles = TA_Base_Core::ConsoleAccessFactory::getInstance().getAllConsoles();

            for ( ConsoleList::iterator consoleIter = allConsoles.begin();
                  allConsoles.end() != consoleIter; ++consoleIter )
            {
                ConsoleInfo newConsole;
                newConsole.consoleKey = ( *consoleIter )->getKey();
                newConsole.location = ( *consoleIter )->getLocation();

                try
                {
                    newConsole.trainOrigin = static_cast< unsigned char >( ( *consoleIter )->getTrainOrigin() );
                }
                catch ( ... )
                {
                    newConsole.trainOrigin = 0;
                }

                if ( 0 == newConsole.trainOrigin )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                                 "Console %s (%d) does not have a Train Origin",
                                 ( *consoleIter )->getName().c_str(), newConsole.consoleKey );

                    // use the agent origin
                    newConsole.trainOrigin = getAgentOrigin();
                }

                m_consoleMap.insert( ConsoleInfoMap::value_type( newConsole.consoleKey, newConsole ) );

                delete ( *consoleIter );
                ( *consoleIter ) = NULL;
            }
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        // load the ATS CCTV Zone mapping
        loadAtsZoneData();

        // Now validate all parameters by calling every get method that gets data from the entity data,
        // (except getPersistedTrainStates, getLocationFromConsole, getLocationType, getAllLocationsOfType, getRadioGroups)
        // call getPecCamera for every combination of Car and Pec
        // ensure all values are > 0 or are not empty strings - test to see if they are all valid
        try
        {
            if ( getEntityKey() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid entity key" ) );
            }

            if ( true == getEntityName().empty() )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid entity name" ) );
            }

            if ( getEntityTypeKey() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid entity type key" ) );
            }

            if ( getLocationKey() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid location key" ) );
            }

            // just calling this is enough to cause an exception if its wrong
            getAgentLocationType();

            if ( getDutyEntity() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid duty entity key" ) );
            }

            if ( getDutySubsystem() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid duty subsystem" ) );
            }

            if ( getAlarmSubsystem() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid alarm subsystem" ) );
            }

            if ( true == getAssetName().empty() )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid asset name" ) );
            }

            if ( true == getAgentSessionId().empty() )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid session ID" ) );
            }

            // 0 and 1 are reserved for the MPU
            if ( getAgentOrigin() < 2 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid alarm subsystem" ) );
            }

            // just call this one, make sure it is present.
            // 0 is a valid number for retries if they only want 1 attempt at anything
            getTimsRetryLimit();

            // just call this one, make sure it is present.
            // 0 is a valid number for auto continue if they want to ack everythin every 20 seconds
            getTimsAutoContinueAckLimit();

            if ( getDefaultTimsMessageTimeout() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid TIMS Timeout" ) );
            }

            if ( getTimerISCS1() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer ISCS1" ) );
            }

            if ( getTimerISCS2() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer ISCS2" ) );
            }

            if ( getTimerCCTV1() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer CCTV1" ) );
            }

            if ( getTimerVC1() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer VC1" ) );
            }

            if ( getTimerVC2() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer VC2" ) );
            }

            if ( getTimerPEC2() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PEC2" ) );
            }

            if ( getTimerPEC4() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PEC4" ) );
            }

            if ( getTimerPA1() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PA1" ) );
            }

            if ( getTimerPA2() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PA2" ) );
            }

            if ( getTimerPA3() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PA3" ) );
            }

            if ( getTimerPA4() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer PA4" ) );
            }

            if ( getTimerTIS1() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer TIS1" ) );
            }

            if ( getTimerTIS2() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer TIS2" ) );
            }

            if ( getOa1Timeout() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer OA1" ) );
            }

            if ( getAtcTimeout() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Timer ATC" ) );
            }

            if ( getMaxThreadPoolSize() < 1 )
            {
                TA_THROW( TA_Base_Core::FatalConfigurationException( "Invalid Max Thread Pool Size " ) );
            }

            // just calling all these is enough
            // validation is performed internally
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecOne );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecTwo );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecThree );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarOne, TA_IRS_Bus::ProtocolPecTypes::PecFour );

            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecOne );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecTwo );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecThree );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarTwo, TA_IRS_Bus::ProtocolPecTypes::PecFour );

            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecOne );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecTwo );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecThree );
            getPecCamera( TA_IRS_Bus::ProtocolPecTypes::CarThree, TA_IRS_Bus::ProtocolPecTypes::PecFour );
        }
        catch ( const TA_Base_Core::FatalConfigurationException& )
        {
            throw;
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            // catch any exceptions and throw them on in a FatalConfigurationException
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            TA_THROW( TA_Base_Core::FatalConfigurationException( e.what() ) );
        }

        FUNCTION_EXIT;
    }


    void TrainAgentConfiguration::loadAtsZoneData()
    {
        FUNCTION_ENTRY( "loadAtsZoneData" );

        TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap locationToCctvZoneMap;
        TA_IRS_Bus::AtsZoneUtils::CctvZoneToLocationMap cctvZoneToLocationMap;

        // load the configuration
        TA_IRS_Bus::AtsZoneUtils::getCctvConfiguration( locationToCctvZoneMap, cctvZoneToLocationMap );

        // process the location to zone map
        for ( TA_IRS_Bus::AtsZoneUtils::LocationToCctvZoneMap::iterator iter = locationToCctvZoneMap.begin();
              locationToCctvZoneMap.end() != iter; ++iter )
        {
            m_cctvZoneMap[ iter->first ] = CctvZone( iter->second );
        }

        FUNCTION_EXIT;
    }
}
