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


#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/utilities/src/RunParams.h"

//#include <boost/lexical_cast.hpp>
#include "core/utilities/src/DataConversion.h"

namespace TA_IRS_Bus
{
    
    TrainAgentProxyFactory::TrainAgentProxyFactory()
        : m_initialised( false ),
          m_localTrainAgentEntityName( "" ),
          m_localTrainAgentEntityKey( 0 ),
          m_localTrainAgentLocationKey( 0 ),
          m_occTrainAgentEntityName( "" ),
          m_depotTrainAgentEntityName( "" )
    {
        FUNCTION_ENTRY( "TrainAgentProxyFactory" );
        FUNCTION_EXIT;
    }
    
    
    TrainAgentProxyFactory::~TrainAgentProxyFactory()
    {
        FUNCTION_ENTRY( "~TrainAgentProxyFactory" );
        FUNCTION_EXIT;
    }


    TrainAgentProxyFactory& TrainAgentProxyFactory::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        FUNCTION_EXIT;
        return *( TrainAgentProxyFactorySingleton::instance() );
    }


    void TrainAgentProxyFactory::agentInitialise( TA_Base_Core::IEntityDataPtr agentEntityData )
    {
        FUNCTION_ENTRY( "agentInitialise" );

        TA_THREADGUARD( m_localTrainAgentEntityLock );

        // prevent multiple calls
        if ( true == m_initialised )
        {
            FUNCTION_EXIT;
            return;
        }

        m_localTrainAgentLocationKey = agentEntityData->getLocation();

        // if agentEntityData's type is TrainAgentEntityData::getStaticType
        // then more information can be set from the entity data rather than just the location

        if ( agentEntityData->getType() == TA_Base_Core::TrainAgentEntityData::getStaticType() )
        {
            m_localTrainAgentEntityKey = agentEntityData->getKey();
            m_localTrainAgentEntityName = agentEntityData->getAgentName();
        }

        // minimal initialisation complete
        m_initialised = true;

        FUNCTION_EXIT;
    }


    void TrainAgentProxyFactory::guiInitialise( const std::string& operatorSession )
    {
        FUNCTION_ENTRY( "guiInitialise" );

        TA_THREADGUARD( m_localTrainAgentEntityLock );

        // prevent multiple calls
        if ( true == m_initialised )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Initialising with sessionId %s",
                     operatorSession.c_str() );

		//2008-07-24 try to get locationId from params
		TA_Base_Bus::SessionInfo sessionInfo;	

		try {
			std::string strLocationKey = (const std::string&)TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			
			std::istringstream strStream(strLocationKey);
			strStream >> sessionInfo.LocationId;
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				 "Exception in guiInitialise() - getlocationId ");

			sessionInfo.LocationId = 0;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Info in guiInitialise() - location id is: %ld",
                     sessionInfo.LocationId );

		//2008-07-24 if locationId is '0', means not valid access right
        TA_ASSERT( 0 != sessionInfo.LocationId,
                   "Location Key could not be converted to a number, or it was 0");
		

        // Set m_localTrainAgentLocationKey from the session info
        m_localTrainAgentLocationKey = sessionInfo.LocationId;

        // set m_initialised = true
        m_initialised = true;

        FUNCTION_EXIT;
    }


    TrainInformationCorbaProxyPtr TrainAgentProxyFactory::createTrainInformationCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainInformationCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainInformationCorbaProxy )
        {
            m_trainInformationCorbaProxy = TrainInformationCorbaProxyPtr(
                                               new TrainInformationCorbaProxy(
                                                   m_localTrainAgentEntityName,
                                                   m_localTrainAgentEntityKey,
                                                   m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return m_trainInformationCorbaProxy;
    }


    TrainOperationModeCorbaProxyPtr TrainAgentProxyFactory::createTrainOperationModeCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainOperationModeCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainOperationModeCorbaProxy )
        {
            m_trainOperationModeCorbaProxy = TrainOperationModeCorbaProxyPtr(
                                                 new TrainOperationModeCorbaProxy( m_localTrainAgentEntityName ) );
        }

        FUNCTION_EXIT;
        return  m_trainOperationModeCorbaProxy;
    }


    TrainRadioGroupCorbaProxyPtr TrainAgentProxyFactory::createTrainRadioGroupCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainRadioGroupCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainRadioGroupCorbaProxy )
        {
            m_trainRadioGroupCorbaProxy = TrainRadioGroupCorbaProxyPtr(
                                              new TrainRadioGroupCorbaProxy(
                                                  m_localTrainAgentEntityName,
                                                  m_localTrainAgentEntityKey,
                                                  m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainRadioGroupCorbaProxy;
    }


    TrainCctvCorbaProxyPtr TrainAgentProxyFactory::createTrainCctvCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainCctvCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainCctvCorbaProxy )
        {
            m_trainCctvCorbaProxy = TrainCctvCorbaProxyPtr(
                                        new TrainCctvCorbaProxy(
                                            m_localTrainAgentEntityName,
                                            m_localTrainAgentEntityKey,
                                            m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainCctvCorbaProxy;
    }


    TrainDriverCallCorbaProxyPtr TrainAgentProxyFactory::createTrainDriverCallCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainDriverCallCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainDriverCallCorbaProxy )
        {
            m_trainDriverCallCorbaProxy = TrainDriverCallCorbaProxyPtr(
                                              new TrainDriverCallCorbaProxy(
                                                  m_localTrainAgentEntityName,
                                                  m_localTrainAgentEntityKey,
                                                  m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainDriverCallCorbaProxy;
    }


    TrainPaCorbaProxyPtr TrainAgentProxyFactory::createTrainPaCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainPaCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainPaCorbaProxy )
        {
            m_trainPaCorbaProxy = TrainPaCorbaProxyPtr(
                                      new TrainPaCorbaProxy(
                                          m_localTrainAgentEntityName,
                                          m_localTrainAgentEntityKey,
                                          m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainPaCorbaProxy;
    }


    TrainPecCorbaProxyPtr TrainAgentProxyFactory::createTrainPecCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainPecCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainPecCorbaProxy )
        {
            m_trainPecCorbaProxy = TrainPecCorbaProxyPtr(
                                       new TrainPecCorbaProxy(
                                           m_localTrainAgentEntityName,
                                           m_localTrainAgentEntityKey,
                                           m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainPecCorbaProxy;
    }


    TrainTisCorbaProxyPtr TrainAgentProxyFactory::createTrainTisCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainTisCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_trainTisCorbaProxy )
        {
            m_trainTisCorbaProxy = TrainTisCorbaProxyPtr(
                                       new TrainTisCorbaProxy(
                                           m_localTrainAgentEntityName,
                                           m_localTrainAgentEntityKey,
                                           m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_trainTisCorbaProxy;
    }


    TrainStateUpdateCorbaProxyPtr TrainAgentProxyFactory::createTrainStateUpdateCorbaProxy()
    {
        FUNCTION_ENTRY( "createTrainStateUpdateCorbaProxy" );

        loadLocalTrainAgentEntityInfo();

        if ( !m_localTrainStateUpdateCorbaProxy )
        {
            m_localTrainStateUpdateCorbaProxy = TrainStateUpdateCorbaProxyPtr(
                                                    new TrainStateUpdateCorbaProxy(
                                                        m_localTrainAgentEntityName,
                                                        m_localTrainAgentEntityKey,
                                                        m_localTrainAgentLocationKey ) );
        }

        FUNCTION_EXIT;
        return  m_localTrainStateUpdateCorbaProxy;
    }


    TrainStateUpdateCorbaProxyPtr TrainAgentProxyFactory::createOccTrainStateUpdateCorbaProxy()
    {
        FUNCTION_ENTRY( "createOccTrainStateUpdateCorbaProxy" );

        loadOtherTrainAgentEntityInfo();

        if ( !m_occTrainStateUpdateCorbaProxy )
        {
            m_occTrainStateUpdateCorbaProxy = TrainStateUpdateCorbaProxyPtr(
                                                  new TrainStateUpdateCorbaProxy( m_occTrainAgentEntityName ) );
        }

        FUNCTION_EXIT;
        return  m_occTrainStateUpdateCorbaProxy;
    }


    TrainStateUpdateCorbaProxyPtr TrainAgentProxyFactory::createDepotTrainStateUpdateCorbaProxy()
    {
        FUNCTION_ENTRY( "createDepotTrainStateUpdateCorbaProxy" );

        loadOtherTrainAgentEntityInfo();

        if ( !m_depotTrainStateUpdateCorbaProxy )
        {
            m_depotTrainStateUpdateCorbaProxy = TrainStateUpdateCorbaProxyPtr(
                                                    new TrainStateUpdateCorbaProxy( m_depotTrainAgentEntityName ) );
        }

        FUNCTION_EXIT;
        return  m_occTrainStateUpdateCorbaProxy;
    }


    void TrainAgentProxyFactory::loadLocalTrainAgentEntityInfo()
    {
        FUNCTION_ENTRY( "loadLocalTrainAgentEntityInfo" );

        // assert that m_initialised == true, agentInitialise or guiInitialise must be called first
        TA_ASSERT( true == m_initialised, "agentInitialise or guiInitialise must be called first" );

        // acquire m_localTrainAgentEntityLock
        TA_THREADGUARD( m_localTrainAgentEntityLock );

        // if m_localTrainAgentEntityName is not empty, return (data already loaded)

        if ( m_localTrainAgentEntityName.empty() == false )
        {
            FUNCTION_EXIT;
            return;
        }

        // otherwise, the data has not yet been loaded

        // assert m_localTrainAgentLocationKey != 0, this must be set from either agentInitialise or guiInitialise
        TA_ASSERT( m_localTrainAgentLocationKey != 0, "this must be set from either agentInitialise or guiInitialise" );

        TA_Base_Core::EntityAccessFactory& entityAccessFactory = TA_Base_Core::EntityAccessFactory::getInstance();

        //using EntityAccessFactory load the train agent for the m_localTrainAgentLocationKey
        TA_Base_Core::IEntityDataList iEntityDataList =
            entityAccessFactory.getEntitiesOfTypeAtLocation( TA_Base_Core::TrainAgentEntityData::getStaticType(),
                                                             m_localTrainAgentLocationKey );
        
        TA_ASSERT( iEntityDataList.size() == 1, "returns only one entry" );

        // populate m_localTrainAgentEntityName and m_localTrainAgentEntityKey from the entity data
        m_localTrainAgentEntityName = iEntityDataList[0]->getName();
        m_localTrainAgentEntityKey = iEntityDataList[0]->getKey();
        
        delete iEntityDataList[0];

        FUNCTION_EXIT;
    }


    void TrainAgentProxyFactory::loadOtherTrainAgentEntityInfo()
    {
        FUNCTION_ENTRY( "loadOtherTrainAgentEntityInfo" );

        // assert that m_initialised == true,
        TA_ASSERT( m_initialised == true, "agentInitialise or guiInitialise must be called first" );

        // acquire m_otherTrainAgentEntityLock
        TA_THREADGUARD( m_otherTrainAgentEntityLock );

        // if m_occTrainAgentEntityName and m_depotTrainAgentEntityName are not empty,
        // return (data already loaded),otherwise, the data has not yet been loaded

        if ( ( false == m_occTrainAgentEntityName.empty() ) &&
             ( false == m_depotTrainAgentEntityName.empty() ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // assert m_localTrainAgentLocationKey != 0, this must be set from either agentInitialise or guiInitialise
        TA_ASSERT( m_localTrainAgentLocationKey != 0, "this must be set from either agentInitialise or guiInitialise" );

        // Using LocationAccessFactory, get the depotLocationKey and the occLocationKey
        TA_Base_Core::LocationAccessFactory& locationAccessFactory = TA_Base_Core::LocationAccessFactory::getInstance();
        unsigned long depotLocationKey = locationAccessFactory.getDepotLocationKey();
        unsigned long occLocationKey = locationAccessFactory.getOccLocationKey();

        // if m_localTrainAgentLocationKey is the same as occLocationKey m_occTrainAgentEntityName = m_localTrainAgentEntityName
        if ( m_localTrainAgentLocationKey == occLocationKey )
        {
            m_occTrainAgentEntityName = m_localTrainAgentEntityName;
        }

        // else if m_localTrainAgentLocationKey is the same as depotLocationKey  m_depotTrainAgentEntityName = m_localTrainAgentEntityName
        else if ( m_localTrainAgentLocationKey == depotLocationKey )
        {
            m_depotTrainAgentEntityName = m_localTrainAgentEntityName;
        }

        // now load the missing entity name(s)
        TA_Base_Core::EntityAccessFactory& entityAccessFactory = TA_Base_Core::EntityAccessFactory::getInstance();

        if ( true == m_occTrainAgentEntityName.empty() )
        {
            // using EntityAccessFactory load the train agent for the occLocationKey
            TA_Base_Core::IEntityDataList iEntityDataList = 
                entityAccessFactory.getEntitiesOfTypeAtLocation( TA_Base_Core::TrainAgentEntityData::getStaticType(),
                                                                 occLocationKey );
            
            TA_ASSERT( iEntityDataList.size() == 1, "returns only one entry" );

            // populate m_occTrainAgentEntityName from the entity data
            m_occTrainAgentEntityName = iEntityDataList[0]->getName();

            delete iEntityDataList[0];
            iEntityDataList[0] = NULL;

        }

        if ( true == m_depotTrainAgentEntityName.empty() )
        {
            // using EntityAccessFactory load the train agent for the depotLocationKey
            TA_Base_Core::IEntityDataList iEntityDataList =
                entityAccessFactory.getEntitiesOfTypeAtLocation( TA_Base_Core::TrainAgentEntityData::getStaticType(),
                                                                 depotLocationKey );

            TA_ASSERT( iEntityDataList.size() == 1, "returns only one entry" );

            // populate m_depotTrainAgentEntityName from the entity data
            m_depotTrainAgentEntityName = iEntityDataList[0]->getName();

            delete iEntityDataList[0];
            iEntityDataList[0] = NULL;
        }

        FUNCTION_EXIT;
    }
}
