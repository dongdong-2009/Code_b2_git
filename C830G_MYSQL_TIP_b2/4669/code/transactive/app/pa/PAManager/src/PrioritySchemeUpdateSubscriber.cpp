/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/PrioritySchemeUpdateSubscriber.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * PrioritySchemeUpdateSubscriber subscriber to listen out for changes to
  * the PriorityScheme data set (used to determine estimated priorities)
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PrioritySchemeUpdateSubscriber.h"
#include "app/pa/PAManager/src/CachedConfig.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_IRS_Bus;

PrioritySchemeUpdateSubscriber::PrioritySchemeUpdateSubscriber()
:
GenericMessageSubscriber<PrioritySchemeUpdateData>  
    (WM_PA_PRIORITY_SCHEME_UPDATE, CachedConfig::getInstance()->getEntityLocationKey())
{

}

const TA_Base_Core::MessageType& PrioritySchemeUpdateSubscriber::getMessageType() const
{
    return TA_Base_Core::PAAgentComms::PrioritySchemeUpdate;
}


void PrioritySchemeUpdateSubscriber::onSubscriptionSuccessful()
{
    // DO NOTHING
    return;
}


void PrioritySchemeUpdateSubscriber::onPreProcessMessage(const PrioritySchemeUpdateData& updateData)
{
    // DO NOTHING
    return;
}


void PrioritySchemeUpdateSubscriber::onPostProcessMessage(const PrioritySchemeUpdateData& updateData)
{
    // DO NOTHING
    return;
}
