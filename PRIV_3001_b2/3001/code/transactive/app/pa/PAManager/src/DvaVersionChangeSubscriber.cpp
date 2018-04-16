/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/DvaVersionChangeSubscriber.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * DvaVersionChangeSubscriber subscriber to listen out for changes to
  * DVA version changes (for the DVA Versions tab)
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "core/message/types/PAAgentBroadcastComms_MessageTypes.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAManager/src/DvaVersionChangeSubscriber.h"
#include "app/pa/PAManager/src/CachedConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;

DvaVersionChangeSubscriber::DvaVersionChangeSubscriber()
:
GenericMessageSubscriber<TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords>
    (WM_PA_DVA_VERSION_UPDATE, CachedConfig::getInstance()->getAgentLocKey())
{

}

const TA_Base_Core::MessageType& DvaVersionChangeSubscriber::getMessageType() const
{
    return TA_Base_Core::PAAgentBroadcastComms::DvaVersionsUpdate;
}


void DvaVersionChangeSubscriber::onSubscriptionSuccessful()
{
    // DO NOTHING
    return;
}


void DvaVersionChangeSubscriber::onPreProcessMessage(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData)
{
    // Post the update to cached maps (to keep it up to date)
    // This is ideally done before (pre) the message notification being sent out    
    // because if the window acknowledges it's received bad data, it will
    // possibly attempt to reload from TA_IRS_Bus::CachedMaps (so update TA_IRS_Bus::CachedMaps first)
    TA_IRS_Bus::CachedMaps::getInstance()->processDvaVersionUpdates(updateData);
}


void DvaVersionChangeSubscriber::onPostProcessMessage(const TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords& updateData)
{
    // DO NOTHING
    return;
}
