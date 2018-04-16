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
    FUNCTION_ENTRY( "updateSchedulesFromDatabase" );
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
		FUNCTION_EXIT;
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

    FUNCTION_EXIT;
    return SUCCESS_NO_ERROR;
}

int ScheduleBroadcastModel::insertScheduleToList( ScheduleBroadcastDetail* pSchedule )
{
    FUNCTION_ENTRY( "insertScheduleToList" );

    if ( NULL == pSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "pSchedule" );
        return ERROR_INVALID_INPUT_PARAMETER;
    }

    std::string strStatus = getScheduleStatus( pSchedule );

    FUNCTION_EXIT;
    return m_refGUI.insertScheduleToList( pSchedule->ulPkeyInDb, pSchedule->strName, 
        VariableConvertor::timeToString( pSchedule->tmStartTime, E_TIME_FORMAT_FULL ), strStatus );
}

int ScheduleBroadcastModel::updateScheduleList()
{
    FUNCTION_ENTRY( "updateScheduleList" );
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

    FUNCTION_EXIT;
    return SUCCESS_NO_ERROR;
}

void ScheduleBroadcastModel::onScheduleCreate( ScheduleBroadcastDetail* pDetail )
{
    FUNCTION_ENTRY( "onScheduleCreate" );
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
    FUNCTION_EXIT;
}

void ScheduleBroadcastModel::onScheduleRemove( ScheduleBroadcastDetail* pDetail )
{
    FUNCTION_ENTRY( "onScheduleRemove" );
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
    FUNCTION_EXIT;
}

void ScheduleBroadcastModel::onScheduleModify( ScheduleBroadcastDetail* pDetail )
{
    FUNCTION_ENTRY( "onScheduleModify" );
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
    FUNCTION_EXIT;
}

void ScheduleBroadcastModel::scheduleBroadcastUpdate( ScheduleBroadcastDetail* pDetail, int nUpdateType )
{
    FUNCTION_ENTRY( "scheduleBroadcastUpdate" );
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
    FUNCTION_EXIT;
}

int ScheduleBroadcastModel::updateScheduleListCurSel( int nCurSel )
{
    FUNCTION_ENTRY( "updateScheduleListCurSel" );
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

    FUNCTION_EXIT;
    return SUCCESS_NO_ERROR;
}

void ScheduleBroadcastModel::formatBroadcastParams( ScheduleBroadcastDetail* pSchedule, 
    std::string& strMsg, std::string& strCylicTimes, std::vector<std::string>& vecZones )
{
    FUNCTION_ENTRY( "formatBroadcastParams" );
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
    FUNCTION_EXIT;
}

void ScheduleBroadcastModel::initialise()
{
    FUNCTION_ENTRY( "initialise" );
    updateSchedulesFromDatabase();
    FUNCTION_EXIT;
}

int ScheduleBroadcastModel::removeSchedule()
{
    FUNCTION_ENTRY( "removeSchedule" );
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

    FUNCTION_EXIT;
    return SUCCESS_NO_ERROR;
}

std::string ScheduleBroadcastModel::getScheduleStatus( ScheduleBroadcastDetail* pSchedule )
{
    FUNCTION_ENTRY( "getScheduleStatus" );
    std::string strStatus = SCHEDULE_IN_DAY_STATUS_UNKNOWN;

    if ( NULL == pSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "pSchedule" );
		FUNCTION_EXIT;
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

    FUNCTION_EXIT;
    return strStatus;
}

void ScheduleBroadcastModel::clearLocalScheduleMap()
{
    FUNCTION_ENTRY( "clearLocalScheduleMap" );
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
    FUNCTION_EXIT;
}

int ScheduleBroadcastModel::onScheduleListItemDeleted( int nItemDeleted )
{
    FUNCTION_ENTRY( "onScheduleListItemDeleted" );
    if (( LIST_NO_SEL != nItemDeleted ) && ( LIST_NO_SEL != m_nCurSelScheduleIndex ) && ( m_nCurSelScheduleIndex > nItemDeleted ))
    {
        --m_nCurSelScheduleIndex;
    }

    FUNCTION_EXIT;
    return SUCCESS_NO_ERROR;
}

int ScheduleBroadcastModel::getScheduleListCurSel()
{
    FUNCTION_ENTRY( "getScheduleListCurSel" );

    FUNCTION_EXIT;
    return m_nCurSelScheduleIndex;
}