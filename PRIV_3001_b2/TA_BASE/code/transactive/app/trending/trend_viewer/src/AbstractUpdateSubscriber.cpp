#if !defined(AbstractUpdateSubscriberCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)
#define AbstractUpdateSubscriberCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_

/**
  * AbstractUpdateSubscriber.cpp
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/AbstractUpdateSubscriber.cpp $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class implements IUpdateSubscriber and represents a subscriber.
  * A subscriber listens out for messages subscribed to by the inheriting
  * class and notifies the graphed item when a data source changes value.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <sstream>

#include "app/trending/trend_viewer/src/AbstractUpdateSubscriber.h"
#include "app/trending/trend_viewer/src/IDataUpdateObserver.h"


namespace TA_Base_App
{
    //
    // newDataReceived
    //
    void AbstractUpdateSubscriber::newDataReceived(time_t timestamp, double value, bool isGoodStatus)
    {
        m_dataUpdateObserver->newDataReceived(timestamp, value, isGoodStatus);
    }

    //
    // isObserverSet
    //
    bool AbstractUpdateSubscriber::isObserverSet()
    {
        if(m_dataUpdateObserver == NULL)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

#endif // !defined(AbstractUpdateSubscriberCpp_C2C5A94A_4967_4ecf_9F1D_7CF0949A4785__INCLUDED_)




