/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveStationPaStep.cpp $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )
 
namespace TA_Base_App
{

    ActiveStationPaStep::ActiveStationPaStep( const TA_Base_Core::StepDetail& stepDetail, 
                                              const TA_Base_Core::StationPublicAddressParameters& params,
                                              IActivePlanFlow& flowControl ) :
        ActiveStep(stepDetail, flowControl),
        m_params(params),
        m_flowControl(flowControl)
    {
    }

    TA_Base_Core::EActiveStepState ActiveStationPaStep::executeImpl( void )
    {
        LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Set step %lu", getStepNumber() );

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
		  /* catch (const TA_Base_Core::BadParameterException& e)
		  {
			  std::ostringstream msg;
			  msg << dumpContext() << ": "
				  << e.reason;
			  
			  LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::BadParameterException", msg.str().c_str() );
			  stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		  }
		  catch (TA_Base_Core::AccessDeniedException& e)
		  {
			  std::ostringstream msg;
			  msg << dumpContext() << ": "
				  << e.reason;
			  
			  LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::AccessDeniedException", msg.str().c_str() );
			  stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		  }*/
        catch( const TA_Base_Core::ObjectResolutionException& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch( const ActiveStepParamException& e )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": "
                << e.what();

            LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }
        catch( ... )
        {
            std::ostringstream msg;
            msg << dumpContext() << ": ";

            LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown Exception", msg.str().c_str() );
            stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
        }

        return( stepState );
    }
} // TA_Base_App

#endif //  STEPTYPE_STATION_PUBLIC_ADDRESS
