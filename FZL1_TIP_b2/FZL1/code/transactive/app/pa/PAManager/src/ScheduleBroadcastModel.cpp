/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/StdAfx.h"

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PaAgentInterface.h"
#include "app/pa/PAManager/src/ScheduleBroadcastModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScheduleBroadcastModel dialog

ScheduleBroadcastModel::ScheduleBroadcastModel( IScheduleBroadcastGUI& refGUI ) : 
m_mapKeyToSchedule(),
m_refGUI( refGUI ),
m_nCurSelScheduleIndex( LIST_NO_SEL )
{
    FUNCTION_ENTRY( "ScheduleBroadcastModel" );

    FUNCTION_EXIT;
}

ScheduleBroadcastModel::~ScheduleBroadcastModel()
{
    FUNCTION_ENTRY( "~ScheduleBroadcastModel" );

    clearLocalScheduleMap();

    FUNCTION_EXIT;
}

int ScheduleBroadcastModel::updateSchedulesFromDatabase()
{
    clearLocalScheduleMap();

    TA_Base_Core::IPaScheduleBroadcasts vecSchedules;
    try
    {
        vecSchedules = TA_Base_Core::PaScheduleBroadcastAccessFactory::getInstance().getPaScheduleBroadcastsByLocationKey( 
            CachedConfig::getInstance()->getPhysicalLocationKey() );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "getPaScheduleBroadcastsByLocationKey" );
        return ERROR_CATCHED_UNKNOWN_EXCEPTION;
    }

    TA_Base_Core::IPaScheduleBroadcast* pSchedule = NULL;
    for ( TA_Base_Core::IPaScheduleBroadcastsIt itLoop = vecSchedules.begin(); 
        vecSchedules.end() != itLoop; ++itLoop )
    {
        pSchedule = (*itLoop);
        if ( NULL == pSchedule )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "IPaScheduleBroadcast" );
            continue;
        }

        ScheduleBroadcastDetail* pNewSchedule = new ScheduleBroadcastDetail;
        if ( NULL != pNewSchedule )
        {
            pNewSchedule->ulPkeyInDb = pSchedule->getKey();
            pNewSchedule->strName = pSchedule->getScheduleName();
            pNewSchedule->ulDvaMsgKey = pSchedule->getDvaMsgKey();
            pNewSchedule->tmStartTime = pSchedule->getStartTime();
            pNewSchedule->ulCyclicTimes = pSchedule->getCyclicTimes();
            pNewSchedule->lstZoneKeys = pSchedule->getZoneKeys();
            pNewSchedule->strStatus = pSchedule->getStatus();

            m_mapKeyToSchedule[pNewSchedule->ulPkeyInDb] = pNewSchedule;
        }
        else
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "ScheduleBroadcastDetail" );
        }

        delete pSchedule;
        (*itLoop) = NULL;
    }

    vecSchedules.clear();

    return SUCCESS_NO_ERROR;
}

int ScheduleBroadcastModel::insertScheduleToList( ScheduleBroadcastDetail* pSchedule )
{
    if ( NULL == pSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "pSchedule" );
        return ERROR_INVALID_INPUT_PARAMETER;
    }

    std::string strStatus = getScheduleStatus( pSchedule );

    return m_refGUI.insertScheduleToList( pSchedule->ulPkeyInDb, pSchedule->strName, 
        VariableConvertor::timeToString( pSchedule->tmStartTime, E_TIME_FORMAT_FULL ), strStatus );
}

int ScheduleBroadcastModel::updateScheduleList()
{
    m_refGUI.clearScheduleList();

    for ( KeyToScheduleMapIt itLoop = m_mapKeyToSchedule.begin(); 
            m_mapKeyToSchedule.end() != itLoop; ++itLoop )
    {
        if ( NULL == itLoop->second )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "IPaScheduleBroadcast" );
            continue;
        }

        insertScheduleToList( itLoop->second );
    }

    return SUCCESS_NO_ERROR;
}

void ScheduleBroadcastModel::onScheduleCreate( ScheduleBroadcastDetail* pDetail )
{
    if ( NULL == pDetail )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return;
    }

    KeyToScheduleMapIt itFound = m_mapKeyToSchedule.find( pDetail->ulPkeyInDb );
    if ( m_mapKeyToSchedule.end() != itFound )
    {
        if ( NULL != itFound->second )
        {
            (*itFound->second) = (*pDetail);
        }
    }
    else
    {
        // Copy a new one
        ScheduleBroadcastDetail* pNewDetail = new ScheduleBroadcastDetail();
        (*pNewDetail) = *pDetail;
        m_mapKeyToSchedule[pNewDetail->ulPkeyInDb] = pNewDetail;
    }

    insertScheduleToList( m_mapKeyToSchedule[pDetail->ulPkeyInDb] );
}

void ScheduleBroadcastModel::onScheduleRemove( ScheduleBroadcastDetail* pDetail )
{
    if ( NULL == pDetail )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return;
    }

    KeyToScheduleMapIt itFound = m_mapKeyToSchedule.find( pDetail->ulPkeyInDb );
    if ( m_mapKeyToSchedule.end() != itFound )
    {
        if ( NULL != itFound->second )
        {
            delete itFound->second;
            itFound->second = NULL;
        }
    }

    m_refGUI.deleteScheduleFromList( pDetail->ulPkeyInDb );
}

void ScheduleBroadcastModel::onScheduleModify( ScheduleBroadcastDetail* pDetail )
{
    if ( NULL == pDetail )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return;
    }

    KeyToScheduleMapIt itFound = m_mapKeyToSchedule.find( pDetail->ulPkeyInDb );
    if ( m_mapKeyToSchedule.end() != itFound )
    {
        if ( NULL != itFound->second )
        {
            (*itFound->second) = (*pDetail);
        }
    }
    else
    {
        // Copy a new one
        ScheduleBroadcastDetail* pNewDetail = new ScheduleBroadcastDetail();
        (*pNewDetail) = (*pDetail);
        m_mapKeyToSchedule[pNewDetail->ulPkeyInDb] = pNewDetail;
    }

    m_refGUI.updateScheduleStateToList( pDetail->ulPkeyInDb, getScheduleStatus( pDetail ));
}

void ScheduleBroadcastModel::scheduleBroadcastUpdate( ScheduleBroadcastDetail* pDetail, int nUpdateType )
{
    switch ( nUpdateType )
    {
    case SCHEDULE_UPDATE_CREATE:
        onScheduleCreate( pDetail );
        break;
    case SCHEDULE_UPDATE_REMOVE:
        onScheduleRemove( pDetail );
        break;
    case SCHEDULE_UPDATE_MODIFY:
        onScheduleModify( pDetail );
        break;
    default:
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC, "scheduleBroadcastUpdate" );
        break;
    }

    if ( NULL != pDetail )
    {
        delete pDetail;
        pDetail = NULL;
    }
}

int ScheduleBroadcastModel::updateScheduleListCurSel( int nCurSel )
{
    m_nCurSelScheduleIndex = nCurSel;

    unsigned long ulScheduleKey = m_refGUI.getScheduleKeyByListItemIndex( nCurSel );

    std::string strMsg = EMPTY_STRING;
    std::string strCylicTimes = EMPTY_STRING;
    std::vector<std::string> vecZones;

    KeyToScheduleMapIt itFound = m_mapKeyToSchedule.find( ulScheduleKey );
    if ( m_mapKeyToSchedule.end() != itFound )
    {
        ScheduleBroadcastDetail* pSchedule = itFound->second;
        if ( NULL != pSchedule )
        {
            formatBroadcastParams( pSchedule, strMsg, strCylicTimes, vecZones );
        }
    }

    m_refGUI.updateBroadcastParamContent( strMsg, strCylicTimes, vecZones );

    return SUCCESS_NO_ERROR;
}

void ScheduleBroadcastModel::formatBroadcastParams( ScheduleBroadcastDetail* pSchedule, 
    std::string& strMsg, std::string& strCylicTimes, std::vector<std::string>& vecZones )
{
    if ( NULL == pSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "pSchedule" );
        return;
    }

    DVAMsgInfo* pDVAMsg = PaDataManager::getInstance().getDVAMsgByKey( pSchedule->ulDvaMsgKey );
    if ( NULL == pDVAMsg )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, 
            VariableConvertor::unsignedLongToString( pSchedule->ulDvaMsgKey ).c_str() );
    }
    else
    {
        strMsg = pDVAMsg->strID + DIVIDE_FORMAT_MARK + pDVAMsg->strName;
    }

    strCylicTimes = VariableConvertor::unsignedLongToString( pSchedule->ulCyclicTimes );

    PaDataManager::getInstance().getStationZoneNameByZoneKey( vecZones, pSchedule->lstZoneKeys );
}

void ScheduleBroadcastModel::initialise()
{
    updateSchedulesFromDatabase();
}

int ScheduleBroadcastModel::removeSchedule()
{
    if ( LIST_NO_SEL == m_nCurSelScheduleIndex )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
        return ERROR_UNEXPECTED_LOGIC;
    }

    unsigned long ulScheduleKey = m_refGUI.getScheduleKeyByListItemIndex( m_nCurSelScheduleIndex );

    int nRet = PaAgentInterface::removeScheduleDVABroadcast( ulScheduleKey );
    if ( SUCCESS_NO_ERROR != nRet )
    {
        return nRet;
    }

    return SUCCESS_NO_ERROR;
}

std::string ScheduleBroadcastModel::getScheduleStatus( ScheduleBroadcastDetail* pSchedule )
{
    std::string strStatus = SCHEDULE_IN_DAY_STATUS_UNKNOWN;

    if ( NULL == pSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "pSchedule" );
        return strStatus;
    }

    if ( 0 == pSchedule->strStatus.compare( TA_Base_Core::SCHEDULE_STATUS_FAILED ))
    {
        strStatus = SCHEDULE_IN_DAY_STATUS_FAILED;
    }
    else if ( 0 == pSchedule->strStatus.compare( TA_Base_Core::SCHEDULE_STATUS_WAITING ))
    {
        strStatus = SCHEDULE_IN_DAY_STATUS_WAITING;
    }
    else if ( 0 == pSchedule->strStatus.compare( TA_Base_Core::SCHEDULE_STATUS_SUCCESSFUL ))
    {
        strStatus = SCHEDULE_IN_DAY_STATUS_SUCCESSFUL;
    }
    else
    {
        strStatus = SCHEDULE_IN_DAY_STATUS_UNKNOWN;
    }

    return strStatus;
}

void ScheduleBroadcastModel::clearLocalScheduleMap()
{
    for ( KeyToScheduleMapIt itLoop = m_mapKeyToSchedule.begin(); 
            m_mapKeyToSchedule.end() != itLoop; ++itLoop )
    {
        if ( NULL == itLoop->second )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "IPaScheduleBroadcast" );
            continue;
        }
        else
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }
    }

    m_mapKeyToSchedule.clear();
}

int ScheduleBroadcastModel::onScheduleListItemDeleted( int nItemDeleted )
{
    if (( LIST_NO_SEL != nItemDeleted ) && ( LIST_NO_SEL != m_nCurSelScheduleIndex ) && ( m_nCurSelScheduleIndex > nItemDeleted ))
    {
        --m_nCurSelScheduleIndex;
    }

    return SUCCESS_NO_ERROR;
}

int ScheduleBroadcastModel::getScheduleListCurSel()
{
    return m_nCurSelScheduleIndex;
}