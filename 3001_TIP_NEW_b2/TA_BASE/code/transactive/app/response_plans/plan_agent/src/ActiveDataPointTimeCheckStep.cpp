/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointTimeCheckStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #9 $
  *
  *  Last modification: $DateTime: 2013/09/25 13:26:41 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of .
  */

#include "app/response_plans/common/src/ProjectDefinition.h"

#if defined ( STEPTYPE_DATA_POINT_TIME_CHECK )

#include <memory>

#include "ace/OS.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "app/response_plans/plan_agent/src/ActiveDataPointTimeCheckStep.h"
#include "app/response_plans/plan_agent/src/IActivePlanFlow.h"
#include "app/response_plans/plan_agent/src/ActiveStepParamException.h"
#include "app/response_plans/common/src/StringConstants.h"
#include "app/response_plans/plan_agent/src/GlobalConstant.h"

namespace TA_Base_App
{
	ActiveDataPointTimeCheckStep::ActiveDataPointTimeCheckStep( const TA_Base_Core::StepDetail& stepDetail,
        const TA_Base_Core::DataPointTimeCheckParameters& params, IActivePlanFlow& flowControl ) : ActiveStep(stepDetail, flowControl),
    m_params(params),
    m_dataPoint(),
    m_ulDpKey( 0u ),
    m_bLastQualityGood( false )
	{
	}

	TA_Base_Core::EActiveStepState ActiveDataPointTimeCheckStep::executeImpl( void )
	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data Point Check time step %lu", getStepNumber() );

		//TA_Base_Bus::DataPointCorbaDef::ClientDataPointState_var dataPointState;
		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		try
		{
            // get named object of datapoint agent
            if ( m_dataPoint.getObjectName().empty() )
            {
                std::auto_ptr<TA_Base_Core::IEntityData> dpEntity(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint.in()));
                std::string dpAgentName = dpEntity->getAgentName();
                m_ulDpKey = dpEntity->getKey();
                m_dataPoint.setCorbaName(dpAgentName, dpAgentName + TA_Base_Bus::DP_ACCESS_INTERFACE);
            }

			TA_Base_Bus::ScadaCorbaTypes::UTag* dataPointState;
			CORBA_CALL_RETURN( dataPointState, m_dataPoint, getFieldValue, ( m_ulDpKey ) );

			bool success = false;
			// Do comparisons.

            unsigned long ulDpTime = 0u;
            int eQuality = 0;

			switch( dataPointState->_d() )
			{
            case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType:
                ulDpTime = dataPointState->analogueTag().timeStamp.seconds;
                eQuality = dataPointState->analogueTag().quality;
                break;
            case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType:
                ulDpTime = dataPointState->booleanTag().timeStamp.seconds;
                eQuality = dataPointState->booleanTag().quality;
                break;
			case TA_Base_Bus::ScadaCorbaTypes::EnumTagType:
                ulDpTime = dataPointState->enumTag().timeStamp.seconds;
                eQuality = dataPointState->enumTag().quality;
                break;
            case TA_Base_Bus::ScadaCorbaTypes::TextTagType:
                ulDpTime = dataPointState->textTag().timeStamp.seconds;
                eQuality = dataPointState->textTag().quality;
                break;
			default:
			    {
                    m_remark = RemarkInvalidDatapointValue;
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Bad data point value: %s", m_params.dataPoint.in() );
                    break;
			    }
            }

            if ( !m_bLastQualityGood )
            {
                m_bLastQualityGood = (( TA_Base_Bus::ScadaCorbaTypes::QualityGoodNoSpecificReason == eQuality ) || 
                    ( TA_Base_Bus::ScadaCorbaTypes::QualityGoodLocalOverride == eQuality ));
            }

            unsigned long ulNow = ACE_OS::time();

            if ( ulNow < ulDpTime )
            {
                if (( ulDpTime - ulNow ) > MAX_TOLERANT_TIME_MISMATCH )
                {
                    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                        "Time is mismatch too much between equipment [%d] and server [%d]", ulDpTime, ulNow );

                    m_remark = RemarkDataPointTimeLaterThanNow;
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                }
                else
                {
                    // success = false in default
                }
            }
            else
            {
                success = ((ulNow - ulDpTime) >= m_params.timeSpan) && m_bLastQualityGood;
            }

            if ( stepState != TA_Base_Core::FAILED_ACTIVE_STEP_STATE )
            {
                if ( !m_flowControl.setNextStep( success ? m_params.successStep : m_params.failureStep ))
                {
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
                }
            }
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": " << e.what() << " for data point " << m_params.dataPoint.in() << ".";
            m_remark = RemarkInvalidDatapoint;

			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const ActiveStepParamException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": " << e.what() << " for data point " << m_params.dataPoint.in() << ".";
            m_remark = RemarkInvalidDatapoint;

			LOG_EXCEPTION_CATCH( SourceInfo, "ActiveStepParamException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Bus::ScadaCorbaTypes::NotWriteable&)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": No access to data point " << m_params.dataPoint.in() << ".";
            m_remark = RemarkInvalidDatapoint;

			LOG_EXCEPTION_CATCH( SourceInfo,  "nexus::DataPointDmIf::NoAccess", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( ... )
		{
			std::ostringstream msg;
            msg << dumpContext() << ": Unknown exception caught: " << m_params.dataPoint.in() << ".";
            m_remark = RemarkInvalidDatapoint;

			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		return stepState;
	}

} // TA_Base_App


#endif //  STEPTYPE_DATA_POINT_TIME_CHECK

