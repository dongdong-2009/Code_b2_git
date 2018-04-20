/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanHouseKeepingMethodObject.cpp $
  *  @author:  jiang.huang
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanHouseKeepingMethodObject.
  */

#include "PlanAgent.h"

namespace TA_Base_App
{

    const int WORKING_INTERVAL = 100;

    PlanHouseKeepingMethodObject::PlanHouseKeepingMethodObject( PlanExecutionEngine& task ) : 
    m_engine( task )
    {
    }

    PlanHouseKeepingMethodObject::~PlanHouseKeepingMethodObject()
    {
    }

    int PlanHouseKeepingMethodObject::call()
    {
        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "Run plan house keeping method object");

        ACE_Time_Value tmWorkingInterval = ACE_Time_Value::zero;
        tmWorkingInterval.msec( WORKING_INTERVAL );

        while( !ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
        {
            try
            {
                ActivePlanBulletinBoard::instance()->pauseExpiredInteractivePlans();

                ACE_OS::sleep( tmWorkingInterval );
                if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
                {
                    break;
                }

                ActivePlanBulletinBoard::instance()->cullStoppedPlans();

                ACE_OS::sleep( tmWorkingInterval );
                if ( ActivePlanBulletinBoard::instance()->wasHangupMessageReceived() )
                {
                    break;
                }

                m_engine.consolidateMq();

                ACE_OS::sleep( tmWorkingInterval );
            }
            catch( const std::exception& e )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, typeid(e).name(), e.what() );
            }
            catch( ... )
            {
                LOG_EXCEPTION_CATCH( SOURCE_INFO, "Unknown", "Unhandled." );
            }
        }

        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "Exit plan house keeping method object");
	    
        return 0;
    }
}