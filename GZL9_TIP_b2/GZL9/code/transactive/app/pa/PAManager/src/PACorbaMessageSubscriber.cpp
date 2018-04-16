/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File$
  * @author Jade Lee
  * @version $Revision$
  * Last modification: $DateTime$
  * Last modified by: $Author$
  * 
  * 
  * PACorbaMessageSubscriber bridges communications between various PA Manager components
  * and CORBA broadcasts - subscribes to any PA status name change notifications
  * Only a single model can attach to this subscriber at a time, and to ensure the client
  * model is kept up to date, must connect first, then update Status Name list (if we refresh
  * list before subscribing notifications, Status Name list may be out of sync)
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "app/pa/PAManager/src/StdAfx.h"

#include "core/message/types/PAAgentComms_MessageTypes.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/ScheduleBroadcastModel.h"
#include "app/pa/PAManager/src/PACorbaMessageSubscriber.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


PACorbaMessageSubscriber::PACorbaMessageSubscriber( CWnd* pNotifyWnd ) : 
TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>(),
m_pNotifyWnd( pNotifyWnd )
{
    subscribe();
}


PACorbaMessageSubscriber::~PACorbaMessageSubscriber()
{
    m_pNotifyWnd = NULL;
    unsubscribe();
}


void PACorbaMessageSubscriber::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
{
    try
    {
        if ( 0 == TA_Base_Core::PAAgentComms::CurrentBroadcastsUpdate.getTypeKey().compare( message.messageTypeKey._ptr ))
        {
            TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData* pUpdateData = NULL;
            if ( NULL == ( message.messageState >>= pUpdateData ))
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "CurrentBroadcastsUpdateData" );
                return;
            }

            processScheduleBroadcastUpdate( *pUpdateData );
        }
        else
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_RECEIVE_UNKNOWN_MESSAGE, message.messageTypeKey._ptr );
        }
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
    }
}

void PACorbaMessageSubscriber::subscribe()
{
    unsigned long ulLocationKey = CachedConfig::getInstance()->getEntityLocationKey();
    
    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
        TA_Base_Core::PAAgentComms::CurrentBroadcastsUpdate, this, 0, 0, ulLocationKey );
}

void PACorbaMessageSubscriber::unsubscribe()
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
}

void PACorbaMessageSubscriber::processScheduleBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ScheduleBroadcastsUpdateData& message )
{
    if ( TA_Base_Bus::IPAAgentCorbaDef::StationDva != message.broadcastType )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER, "message.broadcastType" );
        return;
    }

    int nUpdateType = SCHEDULE_UPDATE_UNKNOWN;
    switch ( message.eUpdateType )
    {
    case TA_Base_Bus::IPAAgentCorbaDef::ScheduleCreate:
        nUpdateType = SCHEDULE_UPDATE_CREATE;
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::ScheduleRemove:
        nUpdateType = SCHEDULE_UPDATE_REMOVE;
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::ScheduleUpdate:
        nUpdateType = SCHEDULE_UPDATE_MODIFY;
        break;
    default:
        nUpdateType = SCHEDULE_UPDATE_UNKNOWN;
        break;
    }

    ScheduleBroadcastDetail* pScheduleDetal = new ScheduleBroadcastDetail;
    if ( NULL == pScheduleDetal )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "ScheduleBroadcastDetail" );
        return;
    }
    pScheduleDetal->strName = message.strBroadcastId._ptr;
    pScheduleDetal->ulDvaMsgKey = message.ulDvaMsgKey;
    pScheduleDetal->ulPkeyInDb = message.ulScheduleKey;
    pScheduleDetal->ulCyclicTimes = message.ulCyclicTimes;
    pScheduleDetal->tmStartTime = message.tmStartTime;
    pScheduleDetal->strStatus = message.strStatus._ptr;
    pScheduleDetal->lstZoneKeys.clear();
    for ( unsigned long ulLoop = 0; ulLoop < message.seqZoneKeys.length(); ++ulLoop )
    {
        pScheduleDetal->lstZoneKeys.push_back( message.seqZoneKeys[ulLoop] );
    }

    if ( NULL != m_pNotifyWnd )
    {
        m_pNotifyWnd->PostMessage( WM_SCHEDULE_BROADCAST_UPDATE, reinterpret_cast<WPARAM>(pScheduleDetal), static_cast<LPARAM>(nUpdateType) );
    }
}