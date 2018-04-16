/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStationPaStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActivePlanBulletinBoard.h"
#include "app/response_plans/plan_agent/src/ActiveStationPaStep.h"

namespace TA_Base_App
{

    ActiveStationPaStep::ActiveStationPaStep( const TA_Base_Core::StepDetail& stepDetail, 
        const TA_Base_Core::StationPublicAddressParameters& params, IActivePlanFlow& flowControl ) :
    ActiveStep(stepDetail, flowControl),
    m_params(params)
    {
    }

    TA_Base_Core::EActiveStepState ActiveStationPaStep::executeImpl( void )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Set step %lu", getStepNumber() );

        TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

        try
        {

				
			   /*string broadcastStationDva 
            ( in MessageKeySeq messages, 
              in ZoneCoverage zones, 
              in unsigned short dwellInSecs, 
              in boolean hasChime, 
              in TA_Base_Core::DateTime startTime, 
              in TA_Base_Core::DateTime stopTime, 
              in unsigned short periodInSecs, 
              in boolean isSynchronisedWithTis, 
				  in string groupName,				// zone group selection for broadcast Id
              in string sessionId ) */
           
            //On successful completion of the step
            m_flowControl.setNextStep();
        }
        catch( const TA_Base_Core::ObjectResolutionException& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": " << e.what();
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": ";

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        return( stepState );
    }
} // TA_Base_App

#endif //  STEPTYPE_STATION_PUBLIC_ADDRESS
