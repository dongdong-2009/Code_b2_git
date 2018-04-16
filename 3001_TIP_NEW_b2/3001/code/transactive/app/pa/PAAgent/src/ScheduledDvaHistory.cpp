/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/ScheduledDvaHistory.cpp $
  * @author:  HuangQi
  * @version: $Revision: #12 $
  *
  * Last modification: $DateTime: 2015/11/27 17:46:47 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#include "ace/Guard_T.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa_4669/src/IConfigPaScheduleBroadcast.h"
#include "core/data_access_interface/pa_4669/src/PaScheduleBroadcastAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"

#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/GlobalStructDefine.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/ScheduledDvaHistory.h"
#include <memory>

namespace TA_IRS_App
{

ScheduledDvaHistory::ScheduledDvaHistory() :
    m_mapDvaParams(),
    m_mtxDvaParamMap()
{
    reloadScheduledHistory();
}

ScheduledDvaHistory::~ScheduledDvaHistory()
{
}

void ScheduledDvaHistory::convertScheduleDvaType( TA_Base_Core::IPaScheduleBroadcast& refSource, ScheduleDvaConfig& refTarget )
{
    refTarget.usScheduleId = static_cast<unsigned short>( refSource.getScheduleId() );
    refTarget.strScheuleName = refSource.getScheduleName();
    refTarget.lstZoneKeys = refSource.getZoneKeys();
    refTarget.lstMsgKeys = refSource.getDvaMsgKeys();
    refTarget.ulStartTime = refSource.getStartTime();
    refTarget.ulEndTime = refSource.getEndTime();
    refTarget.ulInterval = refSource.getIntervalTime();
    refTarget.bSun = refSource.getIsSun();
    refTarget.bMon = refSource.getIsMon();
    refTarget.bTue = refSource.getIsTue();
    refTarget.bWed = refSource.getIsWed();
    refTarget.bThu = refSource.getIsThu();
    refTarget.bFri = refSource.getIsFri();
    refTarget.bSat = refSource.getIsSat();
    refTarget.bChime = refSource.getIsSun();
}

void ScheduledDvaHistory::convertScheduleDvaType( const ScheduleDvaConfig& refSource, TA_Base_Core::IConfigPaScheduleBroadcast& refTarget )
{
    refTarget.setScheduleId( refSource.usScheduleId );
    refTarget.setScheduleName( refSource.strScheuleName );
    refTarget.setZoneKeys( refSource.lstZoneKeys );
    refTarget.setDvaMsgKeys( refSource.lstMsgKeys );
    refTarget.setStartTime( refSource.ulStartTime );
    refTarget.setEndTime( refSource.ulEndTime );
    refTarget.setIntervalTime( refSource.ulInterval );
    refTarget.setIsSun( refSource.bSun );
    refTarget.setIsMon( refSource.bMon );
    refTarget.setIsTue( refSource.bTue );
    refTarget.setIsWed( refSource.bWed );
    refTarget.setIsThu( refSource.bThu );
    refTarget.setIsFri( refSource.bFri );
    refTarget.setIsSat( refSource.bSat );
    refTarget.setIsChime( refSource.bChime );
}

void ScheduledDvaHistory::reloadScheduledHistory()
{
    const unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();
    TA_Base_Core::IPaScheduleBroadcasts scheduleBroadcasts;
    try
    {
        scheduleBroadcasts =
            TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcastsByLocationKey( locationKey, false );
    }
    catch( const TA_Base_Core::DatabaseException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", e.what() );
    }
    catch( const TA_Base_Core::DataException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", e.what() );
    }

    ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxDvaParamMap );
    for( TA_Base_Core::IPaScheduleBroadcasts::iterator it = scheduleBroadcasts.begin(); it != scheduleBroadcasts.end(); ++it )
    {
        ScheduleDvaConfig stConfig;
        TA_Base_Core::IPaScheduleBroadcast& tmp = *( *it );

        EventIdToScheduledDvaMapIt itInserted = m_mapDvaParams.end();
        try
        {
            itInserted = m_mapDvaParams.insert( std::make_pair( static_cast<unsigned short>( tmp.getScheduleId() ), stConfig ) ).first;

            if( m_mapDvaParams.end() != itInserted )
            {
                convertScheduleDvaType( tmp, itInserted->second );
            }
        }
        catch( ... )
        {
            if( m_mapDvaParams.end() != itInserted )
            {
                m_mapDvaParams.erase( itInserted );
            }
        }

        delete *it;
    }
}

void ScheduledDvaHistory::storeScheduledDva( const ScheduleDvaConfig& refParam )
{
    const unsigned short usEventId = refParam.usScheduleId;

    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxDvaParamMap );

        EventIdToScheduledDvaMapIt itFound = m_mapDvaParams.find( usEventId );
        if( m_mapDvaParams.end() != itFound )
        {
            m_mapDvaParams.erase( itFound );
        }

        m_mapDvaParams.insert( EventIdToScheduledDvaMap::value_type( usEventId, refParam ) );
    }

    const unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();

    std::auto_ptr<TA_Base_Core::IConfigPaScheduleBroadcast> configPaScheduleBroadcast;

    try
    {
        configPaScheduleBroadcast.reset(
            dynamic_cast<TA_Base_Core::IConfigPaScheduleBroadcast*>(
                TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcastByScheduleIdAndLocationKey( usEventId, locationKey, true ) ) );
    }
    catch( const TA_Base_Core::DatabaseException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", e.what() );
		return;
    }
    catch( const TA_Base_Core::DataException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", e.what() );
    }

    if( configPaScheduleBroadcast.get() == NULL )
    {
        configPaScheduleBroadcast.reset( TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().createPaScheduleBroadcast() );
    }


    configPaScheduleBroadcast->setLocationKey( locationKey );

    try
    {
        convertScheduleDvaType( refParam, *configPaScheduleBroadcast );
        configPaScheduleBroadcast->applyChanges();
    }
    catch( const TA_Base_Core::DatabaseException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", e.what() );
    }
    catch( const TA_Base_Core::DataException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", e.what() );
    }
    catch( const TA_Base_Core::TransactiveException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
    }
}

void ScheduledDvaHistory::removeScheduledDva( const unsigned short usEventId )
{
    unsigned short usRevertEvertId = INVALID_SCHEDULE_EVENT_ID;

    {
        ACE_Write_Guard<ACE_RW_Mutex> guardCurLock( m_mtxDvaParamMap );

        EventIdToScheduledDvaMapIt itFound = m_mapDvaParams.find( usEventId );
        if( m_mapDvaParams.end() == itFound )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                         "The DVA with event id %d is not in cache", usEventId );
            return;
        }

        usRevertEvertId = itFound->first;
        m_mapDvaParams.erase( itFound );
    }

    const unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();
    try
    {
        std::auto_ptr<TA_Base_Core::IConfigPaScheduleBroadcast> configPaScheduleBroadcast(
            dynamic_cast<TA_Base_Core::IConfigPaScheduleBroadcast*>(
                TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcastByScheduleIdAndLocationKey( usEventId, locationKey, true )));

        if ( NULL != configPaScheduleBroadcast.get() )
        {
            configPaScheduleBroadcast->deleteThisObject();
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "The DVA with event id %d is not in database", usEventId );
        }
    }
    catch( const TA_Base_Core::DatabaseException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", e.what() );
    }
    catch( const TA_Base_Core::DataException& e )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", e.what() );
    }
}

std::vector<ScheduleDvaConfig> ScheduledDvaHistory::getAllScheduledDva()
{
    std::vector<ScheduleDvaConfig> vecScheduledDva;
    {
        ACE_Read_Guard<ACE_RW_Mutex> guardCurLock( m_mtxDvaParamMap );
        for( EventIdToScheduledDvaMapIt it = m_mapDvaParams.begin(); it != m_mapDvaParams.end(); ++it )
        {
            vecScheduledDva.push_back( it->second );
        }
    }
    return vecScheduledDva;
}

}
