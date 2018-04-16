/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DvaMessageChangeSubscriber.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * DvaMessageChangeSubscriber subscriber to listen out for changes to
  * DVA message changes
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "core/message/types/PAAgentComms_MessageTypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "app/pa/PAManager/src/CachedMapLoader.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/DvaMessageChangeSubscriber.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;


DvaMessageChangeSubscriber::DvaMessageChangeSubscriber() :
GenericMessageSubscriber<DvaMessagesUpdateData, DvaMessageUpdateData>
    (WM_PA_DVA_MESSAGE_UPDATE, CachedConfig::getInstance()->getAgentLocKey()),
m_subscribedToCachedMapChanges(false)
{
}


DvaMessageChangeSubscriber::~DvaMessageChangeSubscriber()
{
    resetCachedMapDvaMessageChangeObserverRegistration(false);
}


const TA_Base_Core::MessageType& DvaMessageChangeSubscriber::getMessageType() const
{
    return TA_Base_Core::PAAgentComms::DvaMessagesUpdate;
}


void DvaMessageChangeSubscriber::resetCachedMapDvaMessageChangeObserverRegistration(bool subscribe)
{
    if (!subscribe)
    {
        // Unsubscribe (if not done so already)
        if (m_subscribedToCachedMapChanges)
        {
            TA_IRS_Bus::CachedMaps::getInstance()->deRegisterPaStationDvaMessageChangeObserver(this);
            m_subscribedToCachedMapChanges = false;
        }
    }
    else
    {
        // Subscribe (if not already subscribed)
        if (!m_subscribedToCachedMapChanges)
        {
            TA_IRS_Bus::CachedMaps::getInstance()->registerPaStationDvaMessageChangeObserver(this);
            m_subscribedToCachedMapChanges = true;
        }
    }
}


void DvaMessageChangeSubscriber::onSubscriptionSuccessful()
{
    // Begin listening to changes from cached map (as well as from PAAgent)
    resetCachedMapDvaMessageChangeObserverRegistration(true);

    // Each time AFTER we begin a new subscription, we must refresh the
    // messages from the database - to ensure cachedMaps continues to be up to date 
    // (if we did this before subscribing, might miss some updates)
    CachedMapLoader::refreshKeyToStationDvaMessageRecordMap(
                        CachedConfig::getInstance()->getAgentLocKey());
}


void DvaMessageChangeSubscriber::onPreProcessMessage(const DvaMessagesUpdateData& updateData)
{        
    // Post the update to cached maps (to keep it up to date)
    // This is ideally done before (pre) the message notification being sent out    
    // because if the window acknowledges it's received bad data, it will
    // possibly attempt to reload from TA_IRS_Bus::CachedMaps (so update TA_IRS_Bus::CachedMaps first)
    TA_IRS_Bus::CachedMaps::getInstance()->processDvaMessageUpdates(updateData);   
}


DvaMessageUpdateData DvaMessageChangeSubscriber::getNotificationData(const DvaMessagesUpdateData& updateData)
{
    // DVA Message changes only contain a set of KEY information
    DvaMessageUpdateData result;

    for (unsigned int i = 0; i < updateData.length(); i ++)
    {
        result.insert(updateData[i].m_pKey);
    }

    return result;
}


void DvaMessageChangeSubscriber::updateMessageQueue(
                                        UpdateBuffer<DvaMessageUpdateData>& msgQueue,
                                        const std::set<HWND>& windowHwnds,
                                        const DvaMessageUpdateData& updateData) const
{
    std::set<HWND> windowsRequiringNewNotification;

    for (std::set<HWND>::const_iterator itr = windowHwnds.begin();
            itr != windowHwnds.end();
            itr ++)
    {
        HWND currentWnd = (*itr);

        unsigned long numInQueue = msgQueue.getNumberOfQueuedUpdates(currentWnd);

        // We check to see if window has a update waiting to be collected
        // If it doesn't, we simply add the new update
        if (numInQueue > 0)
        {
            // The DVA Message change subscriber should never have more
            // than one item queued (it always merges into existing queue if present)
			if ( 1 != numInQueue)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unexpected number of queued items" );
				return;
			}

            // May add to existing queue item
            LockedPacket<DvaMessageUpdateData> lockedData = 
                        msgQueue.obtainLockedDataOnFirstItemInQueue(currentWnd);

            // Add in all of the additional key data
            for (DvaMessageUpdateData::const_iterator itr = updateData.begin();
                            itr != updateData.end();
                            itr ++)
            {
                // It is possible for the new data to already be present -
                // in which case we save time double processing the updated key
                lockedData.getMutableData().insert(*itr);
            }
        }
        else
        {
            // Otherwise must add in new item to queue (in which case we must
            // send out a notification)
            windowsRequiringNewNotification.insert(currentWnd);
        }
    }

    // If any windows didn't have items left in their queues, send out a new notification
    if (windowsRequiringNewNotification.size() > 0)
    {
        addItemToQueueAndNotifyWindows(msgQueue, windowsRequiringNewNotification, updateData);
    }
}


void DvaMessageChangeSubscriber::onPostProcessMessage(const DvaMessagesUpdateData& updateData)
{
}


void DvaMessageChangeSubscriber::onCachedMapChanged(unsigned long pKey, bool changesMade)
{
    // Some external source (ie config editor) has made a change
    // to the DVA messages, manually dispatch to update to windows
    // attached to this subscriber
    FUNCTION_ENTRY("DvaMessageChangeSubscriber::onCachedMapChange");

    try
    {
        DvaMessageUpdateData data;
        data.insert(pKey);

        // This will send the update to all the windows
        queueDataAndDispatchNotification(data);
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Caught in onCachedMapChanged");
    }

    FUNCTION_EXIT;
}
