/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/MasterModesDataPointManager.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/ecs/ecs_manager/src/MasterModesDataPointManager.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/IProposedMasterModeObserver.h"
#include "bus/scada/proxies/src/ScadaProxyFactory.h"
#include "bus/scada/proxies/src/ProxyDefs.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/ECSManager.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include <algorithm>

using TA_IRS_App::TA_ECSManagerCommonDefs::ZoneDirectionKeys;

using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataPointAccessFactory;
using TA_Base_Core::ScadaProxyException;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ThreadGuard;

namespace TA_IRS_App
{

    //
    // Constructor
    //
    MasterModesDataPointManager::MasterModesDataPointManager()
        : m_proposedMasterMode1Key( 0 ), m_proposedMasterMode2Key( 0 ), m_currentMasterModeKey( 0 ),
        m_areProposedAndCurrentInitialised( false ), m_areZoneDirectionInitialised( false )
    {
        FUNCTION_ENTRY( "Constructor" );

        int pollTime = 0;
        try
        {
            pollTime = ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().
                getDataPointHealthPollInterval();
        }
        catch ( DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve DataPointHealthInterval parameter value." );
        }
        catch ( DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve DataPointHealthInterval parameter value." );
        }

        if ( pollTime < 0 )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "DataPointPollInterval configured with negative value." );
            TA_Base_Bus::ScadaProxyFactory::getInstance()->setProxiesToMonitorMode();
        }
        else
        {
            TA_Base_Bus::ScadaProxyFactory::getInstance()->setHealthMonitorPollTime( 
                static_cast< unsigned int >( pollTime ) );
			TA_Base_Bus::ScadaProxyFactory::getInstance()->setProxiesToControlMode();
        }

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    MasterModesDataPointManager::~MasterModesDataPointManager()
    {
        FUNCTION_ENTRY( "Destructor" );

        cleanUp();

        FUNCTION_EXIT;
    }


    //
    // refreshProposedAndCurrentDataPointProxies
    //
    void MasterModesDataPointManager::refreshProposedAndCurrentDataPointProxies()
    {
        FUNCTION_ENTRY( "refreshProposedAndCurrentDataPointProxies" );

        // Clean-up any existing.
        // proposed 1
        EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt 
            = m_entityKeyToDataPointProxyAndType.find( m_proposedMasterMode1Key );
        if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            TA_Base_Bus::ScadaProxyFactory::getInstance()->destroyDataPointProxy( ( *entityKeyToDataPointProxyAndTypeIt ).second.first);
            m_entityKeyToDataPointProxyAndType.erase( entityKeyToDataPointProxyAndTypeIt );
        }
        // proposed 2
        entityKeyToDataPointProxyAndTypeIt = m_entityKeyToDataPointProxyAndType.find( m_proposedMasterMode2Key );
        if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            TA_Base_Bus::ScadaProxyFactory::getInstance()->destroyDataPointProxy( (*entityKeyToDataPointProxyAndTypeIt ).second.first);
            m_entityKeyToDataPointProxyAndType.erase( entityKeyToDataPointProxyAndTypeIt );
        }
        // current
        entityKeyToDataPointProxyAndTypeIt = m_entityKeyToDataPointProxyAndType.find( m_currentMasterModeKey );
        if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            TA_Base_Bus::ScadaProxyFactory::getInstance()->destroyDataPointProxy( ( *entityKeyToDataPointProxyAndTypeIt ).second.first );
            m_entityKeyToDataPointProxyAndType.erase( entityKeyToDataPointProxyAndTypeIt );
        }
        m_areProposedAndCurrentInitialised = false;

        // Call helper method to populate new DataPointProxies for the proposed and current MasterMode
        // DataPoints.
        populateProposedAndCurrentMasterModeDataPointProxies();
        m_areProposedAndCurrentInitialised = true;

        FUNCTION_EXIT;
    }


    //
    // refreshZoneDirectionDataPointProxies
    //
    void MasterModesDataPointManager::refreshZoneDirectionDataPointProxies( const ZoneDirectionKeys& zoneDirectionKeys )
    {
        FUNCTION_ENTRY( "refreshProposedAndCurrentDataPointProxies" );

        // Clean-up any existing.
        EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt 
            = m_entityKeyToDataPointProxyAndType.begin();
        while ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {

            if ( ( *entityKeyToDataPointProxyAndTypeIt ).second.second == DIRECTION )
            {
                TA_Base_Bus::ScadaProxyFactory::getInstance()->destroyDataPointProxy( ( *entityKeyToDataPointProxyAndTypeIt ).second.first );
                m_entityKeyToDataPointProxyAndType.erase( entityKeyToDataPointProxyAndTypeIt );
            }
            entityKeyToDataPointProxyAndTypeIt++;
        }
        m_areZoneDirectionInitialised = false;
        

        // Call helper method to populate new DataPointProxies for the Zone Direction
        // DataPoint keys passed in.
        populateZoneDirectionDataPointProxies( zoneDirectionKeys );
        m_areZoneDirectionInitialised = true;

        FUNCTION_EXIT;
    }


    //
    // getProposedMasterMode
    //
    int MasterModesDataPointManager::getProposedMasterMode1()
    {
        FUNCTION_ENTRY( "getProposedMasterMode1" );

        int returnVal = getMasterModeDataPointValue( m_proposedMasterMode1Key );

        FUNCTION_EXIT;
        return returnVal;
    }


    //
    // getProposedMasterMode2
    //
    int MasterModesDataPointManager::getProposedMasterMode2()
    {
        FUNCTION_ENTRY( "getProposedMasterMode2" );

        int returnVal = getMasterModeDataPointValue( m_proposedMasterMode2Key );

        FUNCTION_EXIT;
        return returnVal;
    }


    //
    // getCurrentMasterMode
    //
    int MasterModesDataPointManager::getCurrentMasterMode()
    {
        FUNCTION_ENTRY( "getCurrentMasterMode" );

        int returnVal = getMasterModeDataPointValue( m_currentMasterModeKey );

        FUNCTION_EXIT;
        return returnVal;
    }


    //
    // setCurrentMasterMode
    //
    void MasterModesDataPointManager::setCurrentMasterMode( int value )
    {
        FUNCTION_ENTRY( "setCurrentMasterMode" ); 

        setMasterModeDataPointValue( m_currentMasterModeKey, value );

        FUNCTION_EXIT;
    }


    //
    // getDirectionValue
    //
    int MasterModesDataPointManager::getDirectionValue( unsigned long directionKey )
    {
        FUNCTION_ENTRY( "getDirectionValue" );

        FUNCTION_EXIT;
        return getMasterModeDataPointValue( directionKey );
    }


    //
    // setDirectionValue
    //
    void MasterModesDataPointManager::setDirectionValue( unsigned long directionKey, int value )
    {
        FUNCTION_ENTRY( "setDirectionValue" );

        setMasterModeDataPointValue( directionKey, value );

        FUNCTION_EXIT;
    }


    //
    // getMasterModeDataPointValue
    //
    int MasterModesDataPointManager::getMasterModeDataPointValue( unsigned long entityKey )
    {
        FUNCTION_ENTRY( "getMasterModeDataPointValue" );

        EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt 
            = m_entityKeyToDataPointProxyAndType.find( entityKey );
        if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            int returnVal = static_cast< int >( 
                ( *entityKeyToDataPointProxyAndTypeIt ).second.first->getValueAsDouble() );
            FUNCTION_EXIT;
            return returnVal;
        }

        FUNCTION_EXIT;
        TA_THROW( ScadaProxyException( "No Corresponding ECS Master Mode DataPoint could be located from current configuration." ) );
    }


    //
    // setMasterModeDataPointValue
    //
    void MasterModesDataPointManager::setMasterModeDataPointValue( unsigned long entityKey, int value )
    {
        FUNCTION_ENTRY( "setMasterModeDataPointValue" );

        EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt 
            = m_entityKeyToDataPointProxyAndType.find( entityKey );
        if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            std::ostringstream ostr;
            ostr << value;
            ( *entityKeyToDataPointProxyAndTypeIt ).second.first->setValue( ostr.str() );     
        }

        TA_THROW( ScadaProxyException( "No Corresponding ECS Master Mode DataPoint could be located from current configuration." ) );
        FUNCTION_EXIT;
    }


    //
    // populateProposedAndCurrentMasterModeDataPointProxies
    //
    void MasterModesDataPointManager::populateProposedAndCurrentMasterModeDataPointProxies()
    {
        FUNCTION_ENTRY( "populateProposedAndCurrentMasterModeDataPointProxies" );

        DataPointEntityDataVector proposedMasterMode1DataPointEntities;
        DataPointEntityDataVector proposedMasterMode2DataPointEntities;
        DataPointEntityDataVector currentMasterModeDataPointEntities;

		TA_Base_Core::DataPointAccessFactory* p_dpFactory = 0;
		p_dpFactory = DataPointAccessFactory::getDummyInstance();

		if(p_dpFactory != NULL)
		{
			p_dpFactory->getAllDataPointEntitiesWithNameLikeToken( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getProposedMasterMode1Token(),
                proposedMasterMode1DataPointEntities );
			
			p_dpFactory->getAllDataPointEntitiesWithNameLikeToken( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getProposedMasterMode2Token(),
                proposedMasterMode2DataPointEntities );
			
			p_dpFactory->getAllDataPointEntitiesWithNameLikeToken( ECSManagerConfigurationAccess::getInstance().getEntityConfiguration().getCurrentMasterModeToken(),
                currentMasterModeDataPointEntities );

		}

		DataPointAccessFactory::removeInstance ( p_dpFactory );
        
        DataPointProxyAndType dataPointProxyAndType;
        if ( false == proposedMasterMode1DataPointEntities.empty() )
        {
            m_proposedMasterMode1Key = proposedMasterMode1DataPointEntities[ 0 ]->getKey();
            dataPointProxyAndType.first = TA_Base_Bus::ScadaProxyFactory::getInstance()->createDataPointProxyWithoutAlarmAckStatus
                    ( EntityAccessFactory::getInstance().getCorbaNameOfEntity( m_proposedMasterMode1Key ), *this );
            dataPointProxyAndType.second = PROPOSED_MASTER_MODE_1;
            m_entityKeyToDataPointProxyAndType.insert( EntityKeyToDataPointProxyAndType::value_type( m_proposedMasterMode1Key,
                dataPointProxyAndType ) );
            cleanUpDataPointEntityDataVector( proposedMasterMode1DataPointEntities );
        }
        if ( false == proposedMasterMode2DataPointEntities.empty() )
        {
            m_proposedMasterMode2Key = proposedMasterMode2DataPointEntities[ 0 ]->getKey();
            dataPointProxyAndType.first = TA_Base_Bus::ScadaProxyFactory::getInstance()->createDataPointProxyWithoutAlarmAckStatus
                    ( EntityAccessFactory::getInstance().getCorbaNameOfEntity( m_proposedMasterMode2Key ), *this );
            dataPointProxyAndType.second = PROPOSED_MASTER_MODE_2;
            m_entityKeyToDataPointProxyAndType.insert( EntityKeyToDataPointProxyAndType::value_type( m_proposedMasterMode2Key,
                dataPointProxyAndType ) );
            cleanUpDataPointEntityDataVector( proposedMasterMode2DataPointEntities );
        }
        if ( false == currentMasterModeDataPointEntities.empty() )
        {
            m_currentMasterModeKey = currentMasterModeDataPointEntities[ 0 ]->getKey();
            dataPointProxyAndType.first = TA_Base_Bus::ScadaProxyFactory::getInstance()->createDataPointProxyWithoutAlarmAckStatus
                    ( EntityAccessFactory::getInstance().getCorbaNameOfEntity( m_currentMasterModeKey ), *this );
            dataPointProxyAndType.second = CURRENT_MASTER_MODE;
            m_entityKeyToDataPointProxyAndType.insert( EntityKeyToDataPointProxyAndType::value_type( m_currentMasterModeKey,
                dataPointProxyAndType ) );
            cleanUpDataPointEntityDataVector( currentMasterModeDataPointEntities );
        }

        FUNCTION_EXIT;
    }


    //
    // populateZoneDirectionDataPointProxies
    //
    void MasterModesDataPointManager::populateZoneDirectionDataPointProxies( const ZoneDirectionKeys& zoneDirectionKeys )
    {
        FUNCTION_ENTRY( "populateZoneDirectionDataPointProxies" );

        DataPointProxyAndType dataPointProxyAndType;

        for ( ZoneDirectionKeys::const_iterator zoneDirectionKeysIt = zoneDirectionKeys.begin(); 
        zoneDirectionKeysIt < zoneDirectionKeys.end(); zoneDirectionKeysIt++ )
        {

            dataPointProxyAndType.first = TA_Base_Bus::ScadaProxyFactory::getInstance()->createDataPointProxyWithoutAlarmAckStatus
                    ( EntityAccessFactory::getInstance().getCorbaNameOfEntity( ( *zoneDirectionKeysIt ) ), *this );
            dataPointProxyAndType.second = DIRECTION;
            m_entityKeyToDataPointProxyAndType.insert( EntityKeyToDataPointProxyAndType::value_type( ( *zoneDirectionKeysIt ),
                dataPointProxyAndType ) );          
        }

        FUNCTION_EXIT;
    }


    //
    // cleanUpDataPointEntityDataVector
    //
    void MasterModesDataPointManager::cleanUpDataPointEntityDataVector( DataPointEntityDataVector& dataPointEntityVector )
    {
        FUNCTION_ENTRY( "cleanUpDataPointEntityDataVector" );
        
        for ( DataPointEntityDataVector::iterator dataPointEntityVectorIt = dataPointEntityVector.begin(); 
        dataPointEntityVectorIt < dataPointEntityVector.end(); dataPointEntityVectorIt++ )
        {
            delete ( *dataPointEntityVectorIt );
        }
        dataPointEntityVector.clear();

        FUNCTION_EXIT;
    }


    //
    // registerProposedMasterModeObserver
    //
    void MasterModesDataPointManager::registerProposedMasterModeObserver( IProposedMasterModeObserver* observer )
    {
        FUNCTION_ENTRY( "registerProposedMasterModeObserver" );

        ThreadGuard guard( m_observersLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "registerProposedMasterModeObserver() obtained ThreadLockable." );

        m_registeredProposedMasterModeObservers.push_back( observer );
        std::sort( m_registeredProposedMasterModeObservers.begin(), m_registeredProposedMasterModeObservers.end() );

        FUNCTION_EXIT;
    }


    //
    // deregisterProposedMasterModeObserver
    //
    void MasterModesDataPointManager::deregisterProposedMasterModeObserver( IProposedMasterModeObserver* observer )
    {
        FUNCTION_ENTRY( "deregisterProposedMasterModeObserver" );

        ThreadGuard guard( m_observersLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "deregisterProposedMasterModeObserver() obtained ThreadLockable." );

        if( std::binary_search( m_registeredProposedMasterModeObservers.begin(), m_registeredProposedMasterModeObservers.end(),  observer ) )
        {
            ProposedMasterModeObservers::iterator proposedMasterModeObserversIt
                =  std::lower_bound( m_registeredProposedMasterModeObservers.begin(), m_registeredProposedMasterModeObservers.end(),  observer );
            m_registeredProposedMasterModeObservers.erase( proposedMasterModeObserversIt );
        }
        else
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn, "MasterModesDataPointManager::deregisterProposedMasterModeObserver called with pointer to non registered observer." );
        }

        FUNCTION_EXIT;
    }


    //
    // cleanUp
    //
    void MasterModesDataPointManager::cleanUp()
    {
        FUNCTION_ENTRY( "cleanUp" );        

        EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt 
            = m_entityKeyToDataPointProxyAndType.begin();
        while ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
        {
            TA_Base_Bus::ScadaProxyFactory::getInstance()->destroyDataPointProxy( ( *entityKeyToDataPointProxyAndTypeIt ).second.first );
            entityKeyToDataPointProxyAndTypeIt++;
        }

        m_entityKeyToDataPointProxyAndType.clear();

        m_proposedMasterMode1Key = 0;
        m_proposedMasterMode2Key = 0;
        m_currentMasterModeKey = 0;

        m_areProposedAndCurrentInitialised = false;
        m_areZoneDirectionInitialised = false;

        FUNCTION_EXIT;
    }


    //
    // processEntityUpdateEvent
    //
    void MasterModesDataPointManager::processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType )
    {
        FUNCTION_ENTRY( "processEntityUpdateEvent" );

        ThreadGuard guard( m_observersLock );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "processEntityUpdateEvent() obtained ThreadLockable." );

        switch( updateType )
        {
        case ( TA_Bus::VALUE_STATE_UPDATE ):
            {
                EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt =
                    m_entityKeyToDataPointProxyAndType.find( entityKey );
                if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
                {
                    updateObserversWithDataPointValue( entityKeyToDataPointProxyAndTypeIt, true );
                }
            }
            break;
        case ( TA_Bus::INITIALISED ):
            {
                EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt =
                    m_entityKeyToDataPointProxyAndType.find( entityKey );
                if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
                {
                    updateObserversWithDataPointValue( entityKeyToDataPointProxyAndTypeIt, true );
                }
            }
            break;
        case ( TA_Bus::INVALIDATED ):
            {
                EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt =
                    m_entityKeyToDataPointProxyAndType.find( entityKey );
                if ( entityKeyToDataPointProxyAndTypeIt != m_entityKeyToDataPointProxyAndType.end() )
                {
                    updateObserversWithDataPointValue( entityKeyToDataPointProxyAndTypeIt, false );
                }
            }
            break;
        default:
            break;
        }
        
        FUNCTION_EXIT;
    }


    //
    // updateObserversWithDataPointValue
    //
    void MasterModesDataPointManager::updateObserversWithDataPointValue( EntityKeyToDataPointProxyAndType::iterator entityKeyToDataPointProxyAndTypeIt, bool isValid )
    {
        FUNCTION_ENTRY( "updateObserversWithDataPointValue" );

        int value = 0;
        switch ( ( *entityKeyToDataPointProxyAndTypeIt ).second.second )
        {
        case PROPOSED_MASTER_MODE_1:
        {
            if ( isValid )
            {
                try
                {
                    value = static_cast< int >( ( *entityKeyToDataPointProxyAndTypeIt ).
                        second.first->getValueAsDouble() );
                }
                catch ( const ScadaProxyException& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Proposed MasterMode 1." );
                    isValid = false;
                }
            }
            for ( ProposedMasterModeObservers::iterator observersIt = m_registeredProposedMasterModeObservers.begin(); 
                observersIt < m_registeredProposedMasterModeObservers.end(); observersIt++ )
            {
                ( *observersIt )->proposedMasterMode1Updated( isValid, value );
            }
            FUNCTION_EXIT;
            return;
        }
        case PROPOSED_MASTER_MODE_2:
        {
            if ( isValid )
            {
                try
                {
                    value = static_cast< int >( ( *entityKeyToDataPointProxyAndTypeIt ).
                        second.first->getValueAsDouble() );
                }
                catch ( const ScadaProxyException& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Could not retrieve value of Proposed MasterMode 2." );
                    isValid = false;
                }
            }
            for ( ProposedMasterModeObservers::iterator observersIt = m_registeredProposedMasterModeObservers.begin(); 
                observersIt < m_registeredProposedMasterModeObservers.end(); observersIt++ )
            {
                ( *observersIt )->proposedMasterMode2Updated( isValid, value );
            }
            FUNCTION_EXIT;
            return;
        }
        case CURRENT_MASTER_MODE:
        {
            if ( isValid )
            {
                try
                {
                    value = static_cast< int >( ( *entityKeyToDataPointProxyAndTypeIt ).
                        second.first->getValueAsDouble() );
                }
                catch ( const ScadaProxyException& )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "ScadaProxyException", "Cound not retrieve value of Current MasterMode." );
                    isValid = false;
                }
            }
            for ( ProposedMasterModeObservers::iterator observersIt = m_registeredProposedMasterModeObservers.begin(); 
                observersIt < m_registeredProposedMasterModeObservers.end(); observersIt++ )
            {
                ( *observersIt )->currentMasterModeUpdated( isValid, value );
            }
            FUNCTION_EXIT;
            return;
        }
        default:
            FUNCTION_EXIT;
            return;
        }
    }

} // namespace TA_IRS_App

