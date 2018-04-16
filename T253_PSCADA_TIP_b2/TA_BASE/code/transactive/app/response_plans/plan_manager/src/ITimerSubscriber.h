#if !defined(AFX_ITIMERSUBSCRIBER_H__E5662890_D835_43E2_9EE4_EEF81F57B4C4__INCLUDED_)
#define AFX_ITIMERSUBSCRIBER_H__E5662890_D835_43E2_9EE4_EEF81F57B4C4__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ITimerSubscriber.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Pure Virtual class to provide interface for Timer callback.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class Timer;


    /////////////////////////////////////////////////////////////////////////////
    // ITimerSubscriber Interface

    class ITimerSubscriber
    {
    // Operations
    public:

        /**
          * timerExpired
          *
          * Implement this method to perform some action at
          * regular Timer expiration cycles.
          *
          */
        virtual void timerExpired(Timer& timer) = 0;

        /**
          * Destructor
          *
          */
        virtual ~ITimerSubscriber() = 0 {};
    };

} // namespace TA_Base_App

#endif // !defined(AFX_ITIMERSUBSCRIBER_H__E5662890_D835_43E2_9EE4_EEF81F57B4C4__INCLUDED_)
