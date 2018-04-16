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

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"

#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/BackupKeyMonitor.h"


BackupKeyMonitor::BackupKeyMonitor( unsigned long ulBackEntityKey ) : 
TA_Base_Bus::IEntityUpdateEventProcessor(),
m_spBackupKeyDatapoint(),
m_lockForObserver(),
m_hObserver( NULL ),
m_nBackupKeyStatus( BACKUP_KEY_UNKNOWN_STATUS ),
m_bProxyNotGot( true )
{
    createDatapointProxy( ulBackEntityKey );

    if ( !m_bProxyNotGot )
    {
        updateBackupKeyStatus();
    }
}

BackupKeyMonitor::~BackupKeyMonitor()
{
    try
    {
        m_spBackupKeyDatapoint.reset();
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "DataPointProxySmartPtr::reset" );
    }

    try
    {
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "ScadaProxyFactory::removeInstance" );
    }
}

void BackupKeyMonitor::processEntityUpdateEvent( unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType )
{
    // Update backup key status
    updateBackupKeyStatus();

    // Check is there any observer
    {
        TA_Base_Core::ThreadGuard thObserver( m_lockForObserver );
        // No observer yet, no need to notify
        if ( NULL == m_hObserver )
        {
            return;
        }
    }

    // Notify observer
    notifyObserver();
}

void BackupKeyMonitor::registerObserver( HWND hObserverWnd )
{
    {
        TA_Base_Core::ThreadGuard thObserver( m_lockForObserver );
        if ( NULL != m_hObserver )
        {
            return;
        }
        m_hObserver = hObserverWnd;
    }

    // Notify observer to update initial state
    notifyObserver();
}

void BackupKeyMonitor::deregisterObserver()
{
    TA_Base_Core::ThreadGuard thObserver( m_lockForObserver );
    m_hObserver = NULL;
}

void BackupKeyMonitor::notifyObserver()
{
    TA_Base_Core::ThreadGuard thObserver( m_lockForObserver );

    if ( NULL == m_hObserver )
    {
        return;
    }

    ::PostMessage( m_hObserver, WM_UPDATE_BACKUP_KEY_STATUS, static_cast<WPARAM>(m_nBackupKeyStatus), 0 );
}

void BackupKeyMonitor::updateBackupKeyStatus()
{
    if ( m_bProxyNotGot )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_LOGIC );
        return;
    }

    try
    {
        if ( TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON == m_spBackupKeyDatapoint->getStatus() )
        {
            if ( m_spBackupKeyDatapoint->getValueAsBool() )
            {
                m_nBackupKeyStatus = BACKUP_KEY_LOCK_MODE;
            }
            else
            {
                m_nBackupKeyStatus = BACKUP_KEY_UNLOCK_MODE;
            }
        }
        else
        {
            // Quality bad
            m_nBackupKeyStatus = BACKUP_KEY_UNKNOWN_STATUS;
        }
    }
    catch ( TA_Base_Core::ScadaProxyException& expProxy )
    {
        PaErrorHelper::logExceptionCatch( SourceInfo, expProxy.what() );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
    }
}

void BackupKeyMonitor::createDatapointProxy( unsigned long ulBackEntityKey )
{
    try
    {
        TA_Base_Bus::ScadaProxyFactory::getInstance().createDataPointProxy( ulBackEntityKey, *this, m_spBackupKeyDatapoint );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
    }

    try
    {
        m_spBackupKeyDatapoint.operator ->();
    }
    catch ( TA_Base_Core::ScadaProxyException& )
    {
        m_bProxyNotGot = true;
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, VariableConvertor::unsignedLongToString( ulBackEntityKey ).c_str() );
        PaErrorHelper::popupErrorMsgBox( ERROR_BACKUP_KEY_NOT_CONFIG );
    }

    m_bProxyNotGot = false;
}

bool BackupKeyMonitor::isBackupLockMode()
{
    return ( BACKUP_KEY_LOCK_MODE == m_nBackupKeyStatus );
}
