/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/StdAfx.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAManager/src/PaManagerConsts.h"
#include "app/pa/PAManager/src/ICurSelDVAMsgObserver.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRightsManager.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/HMISelectionListener.h"
#include "app/pa/PAManager/src/PaAgentInterface.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PaRuntimeModel dialog

PaRuntimeModel::PaRuntimeModel() : 
m_pHMISelectionListener( NULL ),
m_pirCurSelEmergencyDVAMsg( std::make_pair(INVALID_DVA_MSG_KEY, static_cast<DVAMsgInfo*>(NULL)) ),
m_pirCurSelNormalDVAMsg( std::make_pair(INVALID_DVA_MSG_KEY, static_cast<DVAMsgInfo*>(NULL)) ),
m_pirCurSelOtherDVAMsg( std::make_pair(INVALID_DVA_MSG_KEY, static_cast<DVAMsgInfo*>(NULL)) ),
m_pirActiveSelDVAMsg( std::make_pair(INVALID_DVA_MSG_KEY, static_cast<DVAMsgInfo*>(NULL)) ),
m_vecEmergencyCurSelDVAMsgObservers(),
m_vecNormalCurSelDVAMsgObservers(),
m_vecOtherCurSelDVAMsgObservers(),
m_ulCurSelZoneGroupKey( ACTIVE_CUR_SEL_ZONE_GROUP_KEY ),
m_bZoneListLocked( false ),
m_lstCachedZoneList()
{
    FUNCTION_ENTRY( "PaRuntimeModel" );

    FUNCTION_EXIT;
}

PaRuntimeModel::~PaRuntimeModel()
{
    FUNCTION_ENTRY( "~PaRuntimeModel" );

    FUNCTION_EXIT;
}

void PaRuntimeModel::registerZoneSelectionNotify( HWND hWnd )
{
    if ( NULL == m_pHMISelectionListener )
    {
        m_pHMISelectionListener = new HmiSelectionListener( hWnd );
    }
    else
    {
        // Log here;
    }
}

void PaRuntimeModel::deregiseterZoneSelectionNotify()
{
    if ( NULL != m_pHMISelectionListener )
    {
        delete m_pHMISelectionListener;
        m_pHMISelectionListener = NULL;
    }
}

std::string PaRuntimeModel::getCurDVAMsgName( int nType )
{
    std::string strName = "";
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            if ( NULL != m_pirCurSelEmergencyDVAMsg.second )
            {
                strName = m_pirCurSelEmergencyDVAMsg.second->strName;
            }
            break;
        }
    case NORMAL_DVA_MSG:
        {
            if ( NULL != m_pirCurSelNormalDVAMsg.second )
            {
                strName = m_pirCurSelNormalDVAMsg.second->strName;
            }
            break;
        }
    case OTHER_DVA_MSG:
        {
            if ( NULL != m_pirCurSelOtherDVAMsg.second )
            {
                strName = m_pirCurSelOtherDVAMsg.second->strName;
            }
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }

    return strName;
}

std::string PaRuntimeModel::getCurDVAMsgDesc( int nType )
{
    std::string strContent = "";
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            if ( NULL != m_pirCurSelEmergencyDVAMsg.second )
            {
                strContent = m_pirCurSelEmergencyDVAMsg.second->strDesc;
            }
            break;
        }
    case NORMAL_DVA_MSG:
        {
            if ( NULL != m_pirCurSelNormalDVAMsg.second )
            {
                strContent = m_pirCurSelNormalDVAMsg.second->strDesc;
            }
            break;
        }
    case OTHER_DVA_MSG:
        {
            if ( NULL != m_pirCurSelOtherDVAMsg.second )
            {
                strContent = m_pirCurSelOtherDVAMsg.second->strDesc;
            }
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }

    return strContent;
}

int PaRuntimeModel::modifyCurDVAMsgDesc( int nType, const std::string& strContent )
{
    // Get local cached DVA message
    unsigned long ulDVAMsgToBeModified = INVALID_DVA_MSG_KEY;
    DVAMsgInfo* pDVAMsgToBeModified = NULL;
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            ulDVAMsgToBeModified = m_pirCurSelEmergencyDVAMsg.first;
            pDVAMsgToBeModified = m_pirCurSelEmergencyDVAMsg.second;
            break;
        }
    case NORMAL_DVA_MSG:
        {
            ulDVAMsgToBeModified = m_pirCurSelNormalDVAMsg.first;
            pDVAMsgToBeModified = m_pirCurSelNormalDVAMsg.second;
            break;
        }
    case OTHER_DVA_MSG:
        {
            ulDVAMsgToBeModified = m_pirCurSelOtherDVAMsg.first;
            pDVAMsgToBeModified = m_pirCurSelOtherDVAMsg.second;
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }

    // Check whether DVA message exist
    if (( NULL == pDVAMsgToBeModified ) || ( INVALID_DVA_MSG_KEY == ulDVAMsgToBeModified ))
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
        return ERROR_DVA_MSG_NOT_EXIST;
    }

    if ( 0 == strContent.compare( pDVAMsgToBeModified->strDesc ))
    {
        // Do nothing if no changes
        return SUCCESS_NO_ERROR;
    }

    int nModifyRet = PaDataManager::getInstance().modifyDvaMsgDesc( ulDVAMsgToBeModified, strContent );
    if ( SUCCESS_NO_ERROR != nModifyRet )
    {
        return nModifyRet;
    }

    // Update local cached data
    pDVAMsgToBeModified->strDesc = strContent;

    // Need to synchronize to other MFTs?
    // If so, send an config update message and subscribe this message for update

    // All successfully and return
    return SUCCESS_NO_ERROR;
}

void PaRuntimeModel::updateCurSelDVAMsg( int nType, unsigned long ulDVAMsgKey, bool bForceNotify )
{
    DVAMsgInfo* pNewSel = PaDataManager::getInstance().getDVAMsgByTypeAndKey( nType, ulDVAMsgKey );
    if ( NULL == pNewSel )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
        return;
    }
    DVAMsgInfo* pOldSel = NULL;

    std::vector<ICurSelDVAMsgObserver*>::iterator itLoop;
    std::vector<ICurSelDVAMsgObserver*>::const_iterator itEnd;
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            pOldSel = m_pirCurSelEmergencyDVAMsg.second;
            m_pirCurSelEmergencyDVAMsg.first = ulDVAMsgKey;
            m_pirCurSelEmergencyDVAMsg.second = pNewSel;
            itLoop = m_vecEmergencyCurSelDVAMsgObservers.begin();
            itEnd = m_vecEmergencyCurSelDVAMsgObservers.end();
            break;
        }
    case NORMAL_DVA_MSG:
        {
            pOldSel = m_pirCurSelNormalDVAMsg.second;
            m_pirCurSelNormalDVAMsg.first = ulDVAMsgKey;
            m_pirCurSelNormalDVAMsg.second = pNewSel;
            itLoop = m_vecNormalCurSelDVAMsgObservers.begin();
            itEnd = m_vecNormalCurSelDVAMsgObservers.end();
            break;
        }
    case OTHER_DVA_MSG:
        {
            pOldSel = m_pirCurSelOtherDVAMsg.second;
            m_pirCurSelOtherDVAMsg.first = ulDVAMsgKey;
            m_pirCurSelOtherDVAMsg.second = pNewSel;
            itLoop = m_vecOtherCurSelDVAMsgObservers.begin();
            itEnd = m_vecOtherCurSelDVAMsgObservers.end();
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            return;
        }
    }

    m_pirActiveSelDVAMsg.first = ulDVAMsgKey;
    m_pirActiveSelDVAMsg.second = pNewSel;

    if (( pOldSel != pNewSel ) || bForceNotify )
    {
        for ( ; itEnd != itLoop; ++itLoop )
        {
            if ( NULL != (*itLoop) )
            {
                (*itLoop)->processDVAMsgSelChanged( nType );
            }
        }
    }
}

void PaRuntimeModel::clearCurSelDVAMsg( int nType, bool bForceNotify )
{
    std::vector<ICurSelDVAMsgObserver*>::iterator itLoop;
    std::vector<ICurSelDVAMsgObserver*>::const_iterator itEnd;

    DVAMsgInfo* pOldSel = NULL;
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            pOldSel = m_pirCurSelEmergencyDVAMsg.second;
            m_pirCurSelEmergencyDVAMsg.first = INVALID_DVA_MSG_KEY;
            m_pirCurSelEmergencyDVAMsg.second = NULL;
            itLoop = m_vecEmergencyCurSelDVAMsgObservers.begin();
            itEnd = m_vecEmergencyCurSelDVAMsgObservers.end();
            break;
        }
    case NORMAL_DVA_MSG:
        {
            pOldSel = m_pirCurSelNormalDVAMsg.second;
            m_pirCurSelNormalDVAMsg.first = INVALID_DVA_MSG_KEY;
            m_pirCurSelNormalDVAMsg.second = NULL;
            itLoop = m_vecNormalCurSelDVAMsgObservers.begin();
            itEnd = m_vecNormalCurSelDVAMsgObservers.end();
            break;
        }
    case OTHER_DVA_MSG:
        {
            pOldSel = m_pirCurSelOtherDVAMsg.second;
            m_pirCurSelOtherDVAMsg.first = INVALID_DVA_MSG_KEY;
            m_pirCurSelOtherDVAMsg.second = NULL;
            itLoop = m_vecOtherCurSelDVAMsgObservers.begin();
            itEnd = m_vecOtherCurSelDVAMsgObservers.end();
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            return;
        }
    }

    m_pirActiveSelDVAMsg.first = INVALID_DVA_MSG_KEY;
    m_pirActiveSelDVAMsg.second = NULL;

    if (( pOldSel != NULL ) || bForceNotify )
    {
        for ( ; itEnd != itLoop; ++itLoop )
        {
            if ( NULL != (*itLoop) )
            {
                (*itLoop)->processDVAMsgSelChanged( nType );
            }
        }
    }
}

void PaRuntimeModel::registerCurSelDVAMsgObserver( int nType, ICurSelDVAMsgObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        return;
    }

    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            m_vecEmergencyCurSelDVAMsgObservers.push_back( pObserver );
            break;
        }
    case NORMAL_DVA_MSG:
        {
            m_vecNormalCurSelDVAMsgObservers.push_back( pObserver );
            break;
        }
    case OTHER_DVA_MSG:
        {
            m_vecOtherCurSelDVAMsgObservers.push_back( pObserver );
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }
}

void PaRuntimeModel::deregisterCurSelDVAMsgObserver( int nType, ICurSelDVAMsgObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        return;
    }

    std::vector<ICurSelDVAMsgObserver*>::iterator itFound;
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            itFound = std::find( m_vecEmergencyCurSelDVAMsgObservers.begin(), m_vecEmergencyCurSelDVAMsgObservers.end(), pObserver );
            if ( m_vecEmergencyCurSelDVAMsgObservers.end() != itFound )
            {
                m_vecEmergencyCurSelDVAMsgObservers.erase( itFound );
            }
            break;
        }
    case NORMAL_DVA_MSG:
        {
            itFound = std::find( m_vecNormalCurSelDVAMsgObservers.begin(), m_vecNormalCurSelDVAMsgObservers.end(), pObserver );
            if ( m_vecNormalCurSelDVAMsgObservers.end() != itFound )
            {
                m_vecNormalCurSelDVAMsgObservers.erase( itFound );
            }
            break;
        }
    case OTHER_DVA_MSG:
        {
            itFound = std::find( m_vecOtherCurSelDVAMsgObservers.begin(), m_vecOtherCurSelDVAMsgObservers.end(), pObserver );
            if ( m_vecOtherCurSelDVAMsgObservers.end() != itFound )
            {
                m_vecOtherCurSelDVAMsgObservers.erase( itFound );
            }
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }
}

DVAMsgInfo* PaRuntimeModel::getCurSelDVAMsg( int nType )
{
    DVAMsgInfo* pInfo = NULL;
    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            pInfo = m_pirCurSelEmergencyDVAMsg.second;
            break;
        }
    case NORMAL_DVA_MSG:
        {
            pInfo = m_pirCurSelNormalDVAMsg.second;
            break;
        }
    case OTHER_DVA_MSG:
        {
            pInfo = m_pirCurSelOtherDVAMsg.second;
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            break;
        }
    }

    return pInfo;
}

DVAMsgInfo* PaRuntimeModel::getActiveSelDVAMsg()
{
    return m_pirActiveSelDVAMsg.second;
}

unsigned long PaRuntimeModel::getActiveSelDVAMsgKey()
{
    return m_pirActiveSelDVAMsg.first;
}

void PaRuntimeModel::updateCurSelZoneGroupKey( unsigned long ulKey )
{
    m_ulCurSelZoneGroupKey = ulKey;
}

bool PaRuntimeModel::isSelectedZoneEmpty()
{
    std::vector<unsigned long> vecZones;
    getCurZones( vecZones );

    return vecZones.empty();
}

void PaRuntimeModel::getActiveSelZoneNames( std::vector<std::string>& vecZoneNames )
{
    vecZoneNames.clear();
    if ( NULL == m_pHMISelectionListener )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    std::vector<unsigned long> lstZoneKeys = getActiveSelZoneKeysList( );
    PaDataManager::getInstance().getStationZoneNameByZoneKey( vecZoneNames, lstZoneKeys );
}

std::vector<unsigned long> PaRuntimeModel::getActiveSelZoneKeysList()
{
    if ( NULL == m_pHMISelectionListener )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return m_lstCachedZoneList;
    }

    if ( !m_bZoneListLocked )
    {
        m_lstCachedZoneList = m_pHMISelectionListener->getZoneList();
    }

    return m_lstCachedZoneList;
}

void PaRuntimeModel::getCurZones( std::vector<unsigned long>& vecZones ) 
{
    if ( ACTIVE_CUR_SEL_ZONE_GROUP_KEY == m_ulCurSelZoneGroupKey )
    {
        vecZones = getActiveSelZoneKeysList();
    }
    else
    {
        vecZones = PaDataManager::getInstance().getZoneKeysByGroupKey( m_ulCurSelZoneGroupKey );
    }
}

bool PaRuntimeModel::checkCyclicTimesRange( int nCyclicTimes )
{
    if (( DVA_MIN_CYCLE_TIMES > nCyclicTimes ) || ( DVA_MAX_CYCLE_TIMES < nCyclicTimes ))
    {
        return false;
    }

    return true;
}

void PaRuntimeModel::lockZoneList( bool bLock )
{
    m_bZoneListLocked = bLock;
}