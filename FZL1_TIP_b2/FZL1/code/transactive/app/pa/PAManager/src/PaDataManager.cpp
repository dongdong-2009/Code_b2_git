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
#include "app/pa/PAManager/src/stdafx.h"

#include "core/data_access_interface/pa/src/PATableConsts.h"
#include "core/data_access_interface/pa/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa/src/PaBroadcastShortcutAccessFactory.h"
#include "core/data_access_interface/pa/src/PaZoneAccessFactory.h"
#include "core/data_access_interface/pa/src/PaZoneGroupAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/RunParams.h"

#include "bus/pa/cachedmaps/src/CachedMaps.h"

#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/IDVAMsgShortcutConfigObserver.h"
#include "app/pa/PAManager/src/PaScheduleManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaAgentInterface.h"
#include "app/pa/PAManager/src/PaDataManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PaDataManager* PaDataManager::s_pInstance = NULL;

PaDataManager::PaDataManager() :
m_refDVAMsgAccessFactory( TA_Base_Core::PaDvaMessageAccessFactory::getInstance() ),
m_refPaBrcScAccessFactory( TA_Base_Core::PaBroadcastShortcutAccessFactory::getInstance() ),
m_refPaZoneGroupAccessFactory( TA_Base_Core::PaZoneGroupAccessFactory::getInstance() ),
m_refPaZoneAccessFactory( TA_Base_Core::PaZoneAccessFactory::getInstance() ),
m_refRunParams( TA_Base_Core::RunParams::getInstance() ),
m_refCachedConfig( *(CachedConfig::getInstance()) ),
m_mapEmergencyDVAMsgInfos(),
m_mapNormalDVAMsgInfos(),
m_mapOtherDVAMsgInfos(),
m_vecDVAMsgShortcutObservers(),
m_mapBroadcastShortcuts(),
m_mapZoneGroups(),
m_mapGroupToZones(),
m_ulLocationkey( UNINITIALIZE_UNSIGNED_LONG ),
m_ulSigBroadcastDelayTime( 0 ),
m_bSigBroadcastEnable( false ),
m_bAgentDataInvalid( true )
{
}

PaDataManager::~PaDataManager()
{
    clearAllDVAMsgInfos();
    m_mapBroadcastShortcuts.clear();
    m_mapZoneGroups.clear();

    TA_Base_Core::PaZoneAccessFactory::removeInstance();
    TA_Base_Core::PaZoneGroupAccessFactory::removeInstance();
    TA_Base_Core::PaDvaMessageAccessFactory::removeInstance();
    TA_Base_Core::PaBroadcastShortcutAccessFactory::removeInstance();
    CachedConfig::removeInstance();
    TA_Base_Core::RunParams::removeInstance();
    TA_IRS_Bus::CachedMaps::removeInstance();
}

void PaDataManager::createInstance()
{
    if ( NULL == s_pInstance )
    {
        s_pInstance = new PaDataManager();
    }
}

void PaDataManager::destoryInstance()
{
    if ( NULL != s_pInstance )
    {
        delete s_pInstance;
        s_pInstance = NULL;
    }
}

PaDataManager& PaDataManager::getInstance()
{
    return (*s_pInstance);
}

void PaDataManager::getDVAMsgs( const std::string& strFilterWord, int nType, std::map<unsigned long, DVAMsgInfo*>& lstRequestedRecords )
{
    if ( strFilterWord.empty() )
    {
        switch( nType )
        {
            case EMERGENCY_DVA_MSG:
            {
                lstRequestedRecords = m_mapEmergencyDVAMsgInfos;
                break;
            }
            case NORMAL_DVA_MSG:
            {
                lstRequestedRecords = m_mapNormalDVAMsgInfos;
                break;
            }
            case OTHER_DVA_MSG:
            {
                lstRequestedRecords = m_mapOtherDVAMsgInfos;
                break;
            }
            default:
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
                break;
            }
        }
    }
    else
    {
        DvaMsgKeyToDVAMsgPtrMap::iterator itLoop;
        DvaMsgKeyToDVAMsgPtrMap::const_iterator itEnd;

        switch( nType )
        {
            case EMERGENCY_DVA_MSG:
            {
                itLoop = m_mapEmergencyDVAMsgInfos.begin();
                itEnd = m_mapEmergencyDVAMsgInfos.end();
                break;
            }
            case NORMAL_DVA_MSG:
            {
                itLoop = m_mapNormalDVAMsgInfos.begin();
                itEnd = m_mapNormalDVAMsgInfos.end();
                break;
            }
            case OTHER_DVA_MSG:
            {
                itLoop = m_mapOtherDVAMsgInfos.begin();
                itEnd = m_mapOtherDVAMsgInfos.end();
                break;
            }
            default:
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
                break;
            }
        }

        for ( ; itEnd != itLoop; ++itLoop )
        {
            DVAMsgInfo* pInfo = itLoop->second;
            if ( NULL == pInfo )
            {
                continue;
            }
            if ( std::string::npos != pInfo->strName.find( strFilterWord ))
            {
                lstRequestedRecords[itLoop->first] = pInfo;
            }
            else if ( std::string::npos != pInfo->strID.find( strFilterWord ))
            {
                lstRequestedRecords[itLoop->first] = pInfo;
            }
        }
    }
}

void PaDataManager::updateDVAMsgMap()
{
    clearAllDVAMsgInfos();
    getAllDVAMsgFromDatabase();
}

void PaDataManager::registerDVAMsgShortcutObserver( IDVAMsgShortcutConfigObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return;
    }

    m_vecDVAMsgShortcutObservers.push_back( pObserver );
}

void PaDataManager::deregisterDVAMsgShortcutObserver( IDVAMsgShortcutConfigObserver* pObserver )
{
    if ( NULL == pObserver )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return;
    }

    std::vector<IDVAMsgShortcutConfigObserver*>::iterator itFonnd;
    itFonnd = std::find( m_vecDVAMsgShortcutObservers.begin(), m_vecDVAMsgShortcutObservers.end(), pObserver );
    if ( m_vecDVAMsgShortcutObservers.end() != itFonnd )
    {
        m_vecDVAMsgShortcutObservers.erase( itFonnd );
    }
}

unsigned long PaDataManager::getDVAMsgKeyByShortcutIndex( unsigned long ulIndex )
{
    if (( INVALID_DVA_MSG_SHORTCUT_INDEX >= ulIndex ) || ( MAX_DVA_MSG_SHORTCUT_COUNT < ulIndex ))
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return INVALID_DVA_MSG_KEY;
    }

    std::map<unsigned long, unsigned long>::iterator itDVAMsgShortcut = m_mapBroadcastShortcuts.find( ulIndex );
    if ( m_mapBroadcastShortcuts.end() == itDVAMsgShortcut )
    {
        return INVALID_DVA_MSG_KEY;
    }
    return m_mapBroadcastShortcuts[ulIndex];
}

bool PaDataManager::getDVAMsgShortcutNameByIndex( std::string& strName, unsigned long ulIndex )
{
    unsigned long ulDVAMsgKey = getDVAMsgKeyByShortcutIndex( ulIndex );
    if ( INVALID_DVA_MSG_KEY == ulDVAMsgKey )
    {
        return false;
    }

    DvaMsgKeyToDVAMsgPtrMap::iterator itFound;
    strName = DVA_MSG_SHORTCUT_BTN_PRE_NAME;
    strName += VariableConvertor::unsignedLongToString( ulIndex );
    strName += DVA_MSG_SHORTCUT_FORMAT_AFT_PRE_NAME;

    itFound = m_mapNormalDVAMsgInfos.find( ulDVAMsgKey );
    if ( m_mapNormalDVAMsgInfos.end() != itFound )
    {
        DVAMsgInfo* pDVAMsgInfo = itFound->second;
        if ( NULL == pDVAMsgInfo )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            return false;
        }
        strName += ( pDVAMsgInfo->strID + DVA_MSG_SHORTCUT_FORMAT_MAKR + pDVAMsgInfo->strName );
        strName += DVA_MSG_SHORTCUT_FORMAT_END_NAME;
        return true;
    }

    itFound = m_mapEmergencyDVAMsgInfos.find( ulDVAMsgKey );
    if ( m_mapEmergencyDVAMsgInfos.end() != itFound )
    {
        DVAMsgInfo* pDVAMsgInfo = itFound->second;
        if ( NULL == pDVAMsgInfo )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            return false;
        }
        strName += ( pDVAMsgInfo->strID + DVA_MSG_SHORTCUT_FORMAT_MAKR + pDVAMsgInfo->strName );
        strName += DVA_MSG_SHORTCUT_FORMAT_END_NAME;
        return true;
    }

    itFound = m_mapOtherDVAMsgInfos.find( ulDVAMsgKey );
    if ( m_mapOtherDVAMsgInfos.end() != itFound )
    {
        DVAMsgInfo* pDVAMsgInfo = itFound->second;
        if ( NULL == pDVAMsgInfo )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            return false;
        }
        strName += ( pDVAMsgInfo->strID + DVA_MSG_SHORTCUT_FORMAT_MAKR + pDVAMsgInfo->strName );
        strName += DVA_MSG_SHORTCUT_FORMAT_END_NAME;
        return true;
    }

    return false;
}

int PaDataManager::getDVAMsgTypeByKey( unsigned long ulKey )
{
    DvaMsgKeyToDVAMsgPtrMap::iterator itFound;

    itFound = m_mapNormalDVAMsgInfos.find( ulKey );
    if (( m_mapNormalDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return NORMAL_DVA_MSG;
    }

    itFound = m_mapEmergencyDVAMsgInfos.find( ulKey );
    if (( m_mapEmergencyDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return EMERGENCY_DVA_MSG;
    }

    itFound = m_mapOtherDVAMsgInfos.find( ulKey );
    if (( m_mapOtherDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return OTHER_DVA_MSG;
    }

    PaErrorHelper::logErrorMsg( SourceInfo, ERROR_COULD_NOT_FIND_EXPECTED_DATA );
    return INVALID_DVA_MSG;
}

DVAMsgInfo* PaDataManager::getDVAMsgByKey( unsigned long ulKey )
{
    DvaMsgKeyToDVAMsgPtrMap::iterator itFound;

    itFound = m_mapNormalDVAMsgInfos.find( ulKey );
    if (( m_mapNormalDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return itFound->second;
    }

    itFound = m_mapEmergencyDVAMsgInfos.find( ulKey );
    if (( m_mapEmergencyDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return itFound->second;
    }

    itFound = m_mapOtherDVAMsgInfos.find( ulKey );
    if (( m_mapOtherDVAMsgInfos.end() != itFound ) && ( NULL != itFound->second ))
    {
        return itFound->second;
    }

    PaErrorHelper::logErrorMsg( SourceInfo, ERROR_COULD_NOT_FIND_EXPECTED_DATA );
    return NULL;
}

DVAMsgInfo* PaDataManager::getDVAMsgByTypeAndKey( int nType, unsigned long ulKey )
{
    DVAMsgInfo* pDvaMsgInfo = NULL;

    switch( nType )
    {
    case EMERGENCY_DVA_MSG:
        {
            pDvaMsgInfo = m_mapEmergencyDVAMsgInfos[ulKey];
            if ( NULL == pDvaMsgInfo )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
            }
            break;
        }
    case NORMAL_DVA_MSG:
        {
            pDvaMsgInfo = m_mapNormalDVAMsgInfos[ulKey];
            if ( NULL == pDvaMsgInfo )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
            }
            break;
        }
    case OTHER_DVA_MSG:
        {
            pDvaMsgInfo = m_mapOtherDVAMsgInfos[ulKey];
            if ( NULL == pDvaMsgInfo )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
            }
            break;
        }
    default:
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        }
    }

    return pDvaMsgInfo;
}

void PaDataManager::getAllDVAMsgFromDatabase()
{
    TA_Base_Core::IPaDvaMessages vecDVAMsgs = m_refDVAMsgAccessFactory.getPaDvaMessagesByLocationKey( m_ulLocationkey );
    if ( 0u == vecDVAMsgs.size() )
    {
        return;
    }

    for ( TA_Base_Core::IPaDvaMessagesIt itLoop = vecDVAMsgs.begin(); vecDVAMsgs.end() != itLoop; ++itLoop )
    {
        TA_Base_Core::IPaDvaMessage*& pIPaDvaMessage = (*itLoop);
        if ( NULL == pIPaDvaMessage )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            continue;
        }

        DVAMsgInfo* pNewDVAMsg = new DVAMsgInfo();
        if ( NULL == pNewDVAMsg )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_FAILED_TO_CREATE_INSTANCE );
            continue;
        }

        pNewDVAMsg->strID = VariableConvertor::unsignedLongToString( pIPaDvaMessage->getId(), 3 );
        pNewDVAMsg->strName = pIPaDvaMessage->getLabel();
        pNewDVAMsg->strDesc = pIPaDvaMessage->getDescription();

        unsigned short usMsgType = pIPaDvaMessage->getType();
        if ( usMsgType == TA_Base_Core::EMERGENCY_DVA_MSG_TYPE )
        {
            m_mapEmergencyDVAMsgInfos[pIPaDvaMessage->getKey()] = pNewDVAMsg;
        }
        else if ( usMsgType == TA_Base_Core::NORMAL_DVA_MSG_TYPE )
        {
            m_mapNormalDVAMsgInfos[pIPaDvaMessage->getKey()] = pNewDVAMsg;
        }
        else if ( usMsgType == TA_Base_Core::OTHER_DVA_MSG_TYPE )
        {
            m_mapOtherDVAMsgInfos[pIPaDvaMessage->getKey()] = pNewDVAMsg;
        }
        else
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC, "DVA Msg Type is not correct" );
        }

        delete pIPaDvaMessage;
        pIPaDvaMessage = NULL;
    }
}

void PaDataManager::clearAllDVAMsgInfos()
{
    for ( DvaMsgKeyToDVAMsgPtrMap::iterator itLoopE = m_mapEmergencyDVAMsgInfos.begin();
          m_mapEmergencyDVAMsgInfos.end() != itLoopE; ++itLoopE )
    {
        delete itLoopE->second;
        itLoopE->second = NULL;
    }
    m_mapEmergencyDVAMsgInfos.clear();

    for ( DvaMsgKeyToDVAMsgPtrMap::iterator itLoopN = m_mapNormalDVAMsgInfos.begin();
          m_mapNormalDVAMsgInfos.end() != itLoopN; ++itLoopN )
    {
        delete itLoopN->second;
        itLoopN->second = NULL;
    }
    m_mapNormalDVAMsgInfos.clear();

    for ( DvaMsgKeyToDVAMsgPtrMap::iterator itLoopO = m_mapOtherDVAMsgInfos.begin();
          m_mapOtherDVAMsgInfos.end() != itLoopO; ++itLoopO )
    {
        delete itLoopO->second;
        itLoopO->second = NULL;
    }
    m_mapOtherDVAMsgInfos.clear();
}

void PaDataManager::updateRunParams()
{
    m_ulLocationkey = m_refCachedConfig.getEntityLocationKey();
    m_refCachedConfig.setSessionId( m_refRunParams.get( RPARAM_SESSIONID ));
    m_refCachedConfig.registerRunParamUser();
}

void PaDataManager::updateDVAMsgShortcuts()
{
    m_mapBroadcastShortcuts.clear();

    TA_Base_Core::IPaBroadcastShortcuts vecShortcuts;
    try
    {
        vecShortcuts = m_refPaBrcScAccessFactory.getPaBroadcastShortcutsByLocationKey( m_ulLocationkey );
    }
    catch ( std::exception& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "getPaBroadcastShortcutsByLocationKey" );
    }

    if ( 0u == vecShortcuts.size() )
    {
        return;
    }
    
    for ( TA_Base_Core::IPaBroadcastShortcutsIt itLoop = vecShortcuts.begin(); vecShortcuts.end() != itLoop; ++itLoop )
    {
        TA_Base_Core::IPaBroadcastShortcut*& pIShortcut = (*itLoop);
        if ( NULL == pIShortcut )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            continue;
        }

        m_mapBroadcastShortcuts[pIShortcut->getShortcutNo()] = pIShortcut->getDVAMsgKey();

        delete pIShortcut;
        pIShortcut = NULL;
    }
}

void PaDataManager::updateZoneGroups()
{
    m_mapZoneGroups.clear();

    TA_Base_Core::IPaZoneGroups vecZoneGroups;
    try
    {
        vecZoneGroups = m_refPaZoneGroupAccessFactory.getPaZoneGroupsByLocationKey( m_ulLocationkey );
    }
    catch ( std::exception& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "getPaZoneGroupsByLocationKey" );
    }

    if ( 0u == vecZoneGroups.size() )
    {
        return;
    }

    for ( TA_Base_Core::IPaZoneGroupsIt itGroupLoop = vecZoneGroups.begin(); vecZoneGroups.end() != itGroupLoop; ++itGroupLoop )
    {
        TA_Base_Core::IPaZoneGroup*& pIZoneGroup = (*itGroupLoop);
        if ( NULL == pIZoneGroup )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            continue;
        }

        // If the Group is for event trigger setting, ignore it.
        if ( 1 == pIZoneGroup->getIsEventGroup() )
        {
            delete pIZoneGroup;
            pIZoneGroup = NULL;
            continue;
        }

        unsigned long ulZoneGroupKey = pIZoneGroup->getKey();
        m_mapZoneGroups[ulZoneGroupKey] = pIZoneGroup->getLabel();

        delete pIZoneGroup;
        pIZoneGroup = NULL;

        TA_Base_Core::IPaZones vecZones;
        try
        {
            vecZones = m_refPaZoneAccessFactory.getPaZonesByGroupId( ulZoneGroupKey, false );
        }
        catch ( std::exception& expWhat )
        {
            PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
        }
        catch (...)
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "getPaZonesByGroupId" );
        }

        std::vector< std::pair< unsigned long, std::string> > vecZoneIdNames;
        for ( TA_Base_Core::IPaZonesIt itZoneLoop = vecZones.begin(); vecZones.end() != itZoneLoop; ++itZoneLoop )
        {
            TA_Base_Core::IPaZone*& pIZone = (*itZoneLoop);
            if ( NULL == pIZone )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
                continue;
            }
            vecZoneIdNames.push_back( std::make_pair( pIZone->getKey(), pIZone->getLabel() ));

            delete pIZone;
            pIZone = NULL;
        }
        m_mapGroupToZones[ulZoneGroupKey] = vecZoneIdNames;
    }
}

std::map<unsigned long, std::string>& PaDataManager::getAllZoneGroups()
{
    return m_mapZoneGroups;
}

std::vector<std::string> PaDataManager::getZoneNamesByGroupKey( unsigned long ulGroupKey )
{
    GroupToZonesMap::iterator itFound = m_mapGroupToZones.find( ulGroupKey );
    std::vector<std::string> vecZoneNames;
    if ( m_mapGroupToZones.end() == itFound )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_COULD_NOT_FIND_EXPECTED_DATA );
        return vecZoneNames;
    }

    std::vector<unsigned long> lstZoneKeys;
    for ( ZonesKeyNamesVector::iterator itZoneLoop = itFound->second.begin(); 
          itFound->second.end() != itZoneLoop; ++itZoneLoop )
    {
        lstZoneKeys.push_back( itZoneLoop->first );
    }

    getStationZoneNameByZoneKey( vecZoneNames, lstZoneKeys );
    return vecZoneNames;
}

void PaDataManager::getStationZoneNameByZoneKey( std::vector<std::string>& vecZones, const std::vector<unsigned long>& listZoneKeys )
{
    TA_IRS_Bus::PaZoneRecord oZone;
    std::string strName = EMPTY_STRING;

    for ( std::vector<unsigned long>::const_iterator itLoop = listZoneKeys.begin(); listZoneKeys.end() != itLoop; ++itLoop )
    {
        try
        {
            oZone = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey( *itLoop );
        }
        catch (...)
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
            continue;
        }

        strName = oZone.m_strDispLocName + DIVIDE_FORMAT_MARK + oZone.m_label;
        if ( !strName.empty() )
        {
            vecZones.push_back( strName );
        }
    }
}

std::vector<unsigned long> PaDataManager::getZoneKeysByGroupKey( unsigned long ulGroupKey )
{
    GroupToZonesMap::iterator itFound = m_mapGroupToZones.find( ulGroupKey );
    std::vector<unsigned long> lstZoneKeys;
    if ( m_mapGroupToZones.end() == itFound )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_COULD_NOT_FIND_EXPECTED_DATA );
        return lstZoneKeys;
    }
    
    for ( ZonesKeyNamesVector::iterator itZoneLoop = itFound->second.begin(); 
            itFound->second.end() != itZoneLoop; ++itZoneLoop )
    {
        lstZoneKeys.push_back( itZoneLoop->first );
    }

    return lstZoneKeys;
}

bool PaDataManager::modifyDVAMsgShortcut( unsigned long ulIndex, unsigned long ulDVAMsgKey )
{
    if (( INVALID_DVA_MSG_SHORTCUT_INDEX >= ulIndex ) || ( MAX_DVA_MSG_SHORTCUT_COUNT < ulIndex ))
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return false;
    }

    unsigned long ulOldKey = INVALID_DVA_MSG_KEY;
    bool bIsNew = false;

    if ( m_mapBroadcastShortcuts.end() != m_mapBroadcastShortcuts.find( ulIndex ))
    {
        ulOldKey = m_mapBroadcastShortcuts[ulIndex];
    }
    else
    {
        bIsNew = true;
    }

    if ( !bIsNew && ( ulOldKey == ulDVAMsgKey ))
    {
        // Do nothing when no changes
        return true;
    }

    // Save to database
    TA_Base_Core::IConfigPaBroadcastShortcut* pModifiedShortcut = NULL;
    try
    {
        if ( bIsNew )
        {
            pModifiedShortcut = m_refPaBrcScAccessFactory.createPaBroadcastShortcut();
            if ( NULL == pModifiedShortcut )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
                return false;
            }

            pModifiedShortcut->setDVAMsgKey( ulDVAMsgKey );
            pModifiedShortcut->setShortcutNo( ulIndex );
            pModifiedShortcut->setLocationKey( m_ulLocationkey );
            pModifiedShortcut->applyChanges();
        }
        else
        {
            pModifiedShortcut = dynamic_cast<TA_Base_Core::IConfigPaBroadcastShortcut*>(
                m_refPaBrcScAccessFactory.getPaBroadcastShortcutByLocationKeyAndShortcutNo( m_ulLocationkey, ulIndex, true ));
            if ( NULL == pModifiedShortcut )
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
                return false;
            }

            pModifiedShortcut->setDVAMsgKey( ulDVAMsgKey );
            pModifiedShortcut->applyChanges();
            
            delete pModifiedShortcut;
            pModifiedShortcut = NULL;
        }
    }
    catch ( std::exception& expWhat )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
        if ( NULL != pModifiedShortcut )
        {
            delete pModifiedShortcut;
            pModifiedShortcut = NULL;
        }
        return false;
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        if ( NULL != pModifiedShortcut )
        {
            delete pModifiedShortcut;
            pModifiedShortcut = NULL;
        }
        return false;
    }

    m_mapBroadcastShortcuts[ulIndex] = ulDVAMsgKey;

    for ( std::vector<IDVAMsgShortcutConfigObserver*>::iterator itLoop = m_vecDVAMsgShortcutObservers.begin();
          m_vecDVAMsgShortcutObservers.end() != itLoop; ++itLoop )
    {
        if ( NULL != (*itLoop) )
        {
            (*itLoop)->processDVAMsgShortcutChanged( ulIndex );
        }
    }

    // All successfully and return
    return true;
}

std::map<unsigned long, unsigned long>& PaDataManager::getExistShortcutNos()
{
    return m_mapBroadcastShortcuts;
}

int PaDataManager::deleteZoneGroupByGroupKey( unsigned long ulGroupKey )
{
    std::map<unsigned long, std::string>::iterator itFound = m_mapZoneGroups.find( ulGroupKey );
    if ( m_mapZoneGroups.end() == itFound )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_COULD_NOT_FIND_EXPECTED_DATA );
        return ERROR_COULD_NOT_FIND_EXPECTED_DATA;
    }

    TA_Base_Core::IConfigPaZoneGroup* pToBeDeteled = NULL;
    try
    {
        pToBeDeteled = dynamic_cast<TA_Base_Core::IConfigPaZoneGroup*>(
            m_refPaZoneGroupAccessFactory.getPaZoneGroup( ulGroupKey, true ));
    }
    catch ( TA_Base_Core::DataException& eDataException ) 
    {
        if ( TA_Base_Core::DataException::NO_VALUE == eDataException.getFailType() )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_ZONE_GROUP_DELETED_ALREADY );
            // Data not exist in database anymore, continue to erase local cached data
            PaErrorHelper::logExceptionCatch( SourceInfo, eDataException.what() );
        }
        else
        {
            PaErrorHelper::logExceptionCatch( SourceInfo, eDataException.what() );
            return ERROR_FAILED_TO_READ_DATABASE;
        }
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        if ( NULL != pToBeDeteled )
        {
            delete pToBeDeteled;
            pToBeDeteled = NULL;
        }
        return ERROR_CATCHED_UNKNOWN_EXCEPTION;
    }

    try
    {
        if ( NULL != pToBeDeteled )
        {
            pToBeDeteled->destroyExistingPaZoneGroupHelperRecords();
            pToBeDeteled->deleteThisObject();

            delete pToBeDeteled;
            pToBeDeteled = NULL;
        }
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        if ( NULL != pToBeDeteled )
        {
            delete pToBeDeteled;
            pToBeDeteled = NULL;
        }
    	return ERROR_CATCHED_UNKNOWN_EXCEPTION;
    }

    m_mapZoneGroups.erase( itFound );
    GroupToZonesMap::iterator itGroupToZones = m_mapGroupToZones.find( ulGroupKey );
    if ( m_mapGroupToZones.end() != itGroupToZones )
    {
        m_mapGroupToZones.erase( itGroupToZones );
    }

    return SUCCESS_NO_ERROR;
}

int PaDataManager::createZoneGroupByZoneKeys( unsigned long& ulNewGroupKey, 
    const std::string& strGroupName, const std::vector<unsigned long>& vecZoneKeys )
{
    TA_Base_Core::IConfigPaZoneGroup* pToBeCreated = NULL;
    try
    {
        pToBeCreated = m_refPaZoneGroupAccessFactory.createPaZoneGroup();

        if ( NULL == pToBeCreated )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            return ERROR_UNEXPECTED_NULL_POINTER;
        }

        std::list<unsigned long> lstZoneKeys;
        for ( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); vecZoneKeys.end() != itLoop; ++itLoop )
        {
            lstZoneKeys.push_back( *itLoop );
        }
        pToBeCreated->queueAdditionOfPaZoneGroupHelperRecords( lstZoneKeys );
        pToBeCreated->setLabel( strGroupName );
        pToBeCreated->setLocationKey( m_ulLocationkey );
        pToBeCreated->setIsEventGroup( 0 ); // Not event group
        pToBeCreated->applyChanges();
        ulNewGroupKey = pToBeCreated->getKey();
        if ( NULL != pToBeCreated )
        {
            delete pToBeCreated;
            pToBeCreated = NULL;
        }
    }
    catch ( TA_Base_Core::DataException& eDataException )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, eDataException.what() );
        if ( NULL != pToBeCreated )
        {
            delete pToBeCreated;
            pToBeCreated = NULL;
        }
        return ERROR_FAILED_TO_SAVE_ZONG_GROUP;
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        if ( NULL != pToBeCreated )
        {
            delete pToBeCreated;
            pToBeCreated = NULL;
        }
        return ERROR_CATCHED_UNKNOWN_EXCEPTION;
    }

    ZonesKeyNamePair pirZoneIdName;
    ZonesKeyNamesVector vecZonesIdNames;
    for ( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); vecZoneKeys.end() != itLoop; ++itLoop )
    {
        try
        {
            pirZoneIdName = std::make_pair( *itLoop, TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordFromKey( *itLoop ).m_label );
            vecZonesIdNames.push_back( pirZoneIdName );
        }
        catch (const TA_Base_Core::CachedMappingNotFoundException& expWhat )
        {
            PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
            return ERROR_FAILED_TO_SAVE_ZONG_GROUP;
        }
        catch(...)
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
            return ERROR_CATCHED_UNKNOWN_EXCEPTION;
        }
    }

    m_mapZoneGroups[ulNewGroupKey] = strGroupName;
    m_mapGroupToZones[ulNewGroupKey] = vecZonesIdNames;

    return SUCCESS_NO_ERROR;
}

int PaDataManager::updateDataFromAgent()
{
    int nRet = SUCCESS_NO_ERROR;

    m_bAgentDataInvalid = false;
    return SUCCESS_NO_ERROR;
}

bool PaDataManager::isAgentDataInvalid()
{
    return m_bAgentDataInvalid;    
}

bool PaDataManager::isGroupNameExist( const std::string& strGroupName )
{
    for ( std::map<unsigned long, std::string>::iterator itLoop = m_mapZoneGroups.begin(); 
            m_mapZoneGroups.end() != itLoop; ++itLoop )
    {
        if ( 0 == itLoop->second.compare( strGroupName ))
        {
            return true;
        }
    }

    return false;
}

bool PaDataManager::isZoneGroupsCountOverLimit()
{
    unsigned long ulZoneGroupsCount = 0;
    try
    {
        TA_Base_Core::IPaZoneGroups vecGroups = m_refPaZoneGroupAccessFactory.getPaZoneGroupsByLocationKey( m_ulLocationkey );
        ulZoneGroupsCount = vecGroups.size();

        for ( TA_Base_Core::IPaZoneGroups::iterator itLoop = vecGroups.begin(); vecGroups.end() != itLoop; ++itLoop )
        {
            delete *itLoop;
        }
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "getPaZoneGroupsCountByLocationKey" );
    }

    if ( MAX_ZONE_GOURP_COUNT <= ulZoneGroupsCount )
    {
        return true;
    }

    return false;
}

int PaDataManager::modifyDvaMsgDesc( unsigned long ulMsgKey, const std::string& strMsgDesc )
{
    // Apply modification to database
    TA_Base_Core::IConfigPaDvaMessage* pDVAMsgDbObject = NULL;
    try
    {
        pDVAMsgDbObject = dynamic_cast<TA_Base_Core::IConfigPaDvaMessage*>(
            m_refDVAMsgAccessFactory.getPaDvaMessage( ulMsgKey, true ));

        if ( NULL == pDVAMsgDbObject )
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
            return ERROR_UNEXPECTED_NULL_POINTER;
        }

        pDVAMsgDbObject->setDescription( strMsgDesc );
        pDVAMsgDbObject->applyChanges();

        delete pDVAMsgDbObject;
        pDVAMsgDbObject = NULL;
    }
    catch( TA_Base_Core::DataException& expWhat )
    {
        if ( NULL != pDVAMsgDbObject )
        {
            delete pDVAMsgDbObject;
            pDVAMsgDbObject = NULL;
        }

        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );

        if ( TA_Base_Core::DataException::NO_VALUE == expWhat.getFailType() )
        {
            return ERROR_DVA_MSG_NOT_EXIST;
        }
        else if ( TA_Base_Core::DataException::NOT_UNIQUE == expWhat.getFailType() )
        {
            return ERROR_UNEXPECTED_LOGIC;
        }
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
        if ( NULL != pDVAMsgDbObject )
        {
            delete pDVAMsgDbObject;
            pDVAMsgDbObject = NULL;
        }
        return ERROR_CATCHED_UNKNOWN_EXCEPTION;
    }

    return SUCCESS_NO_ERROR;
}
