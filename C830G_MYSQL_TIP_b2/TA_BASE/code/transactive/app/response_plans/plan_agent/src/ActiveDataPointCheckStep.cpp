/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDataPointCheckStep.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PlanAgent.h"

#if defined ( STEPTYPE_DATA_POINT_CHECK )

// namespace alias for Corba structures/types

namespace TA_Base_App
{
	ActiveDataPointCheckStep::ActiveDataPointCheckStep( const TA_Base_Core::StepDetail& stepDetail,
														const TA_Base_Core::DataPointCheckParameters& params,
														IActivePlanFlow& flowControl ) :
		ActiveStep(stepDetail, flowControl),
		m_params(params),
		m_flowControl(flowControl)
	{
	}

	TA_Base_Core::EActiveStepState ActiveDataPointCheckStep::executeImpl( void )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Data Point Check step %lu", getStepNumber() );

		//TA_Base_Bus::DataPointCorbaDef::ClientDataPointState_var dataPointState;
		TA_Base_Core::EActiveStepState stepState = TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE;

		std::string message;

		try
		{
            // get named object of datapoint agent
            TA_Base_Core::IEntityData* dpEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_params.dataPoint.in());
            std::string dpAgentName = dpEntity->getAgentName();
            m_dataPoint.setCorbaName(dpAgentName, dpAgentName + TA_Base_Bus::DP_ACCESS_INTERFACE);

			TA_Base_Bus::ScadaCorbaTypes::UTag* dataPointState;
			CORBA_CALL_RETURN( dataPointState,
			                   m_dataPoint,
			                   getFieldValue,
			                   ( dpEntity->getKey() ) );
		
			bool success = false;
			// Do comparisons.

			switch( dataPointState->_d() )
			{
            case TA_Base_Bus::ScadaCorbaTypes::AnalogueTagType:
            //? why didn't deal with enum data type? case TA_Base_Bus::ScadaCorbaTypes::EnumTagType:
				{
					if( TA_Base_Core::NUMERIC_DATA_POINT != m_params.value._d() )
					{
						message = "Attempt to compare float data point with non-float value for data point: ";
						message += m_params.dataPoint.in();
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
					}
					else
					{
						success = test( m_params.value.numericValue().value,
										dataPointState->analogueTag().value,
										m_params.testType );
					}
				}
				break;

			case TA_Base_Bus::ScadaCorbaTypes::BooleanTagType:
				{
					if( TA_Base_Core::BOOLEAN_DATA_POINT != m_params.value._d() )
					{
						message = "Attempt to compare boolean data point with non-boolean value for data point: ";
						message += m_params.dataPoint.in();
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
					}
					else
					{
						success = test( m_params.value.booleanValue(),
				 						dataPointState->booleanTag().value,
										m_params.testType );
					}
				}
				break;

			case TA_Base_Bus::ScadaCorbaTypes::TextTagType:
				{
					if( TA_Base_Core::TEXT_DATA_POINT != m_params.value._d() )
					{
						message = "Attempt to compare text data point with non-text value for data point: ";
						message += m_params.dataPoint.in();
						LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
                        stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
					}
					else
					{
						std::string lhs( m_params.value.textValue()   );
                        TA_Base_Bus::ScadaCorbaTypes::ByteSequence readSeq = dataPointState->textTag().value;
			            std::string rhs = "";
                        int length = readSeq.length();

			            for ( int i = 0; i < length; i++ )
			            {
				            rhs += (char) readSeq[i];
			            }

						success = test( lhs, rhs, m_params.testType );
					}
				}
				break;

			default:
				{
					message = "Bad data point value: ";
					message += m_params.dataPoint.in();
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, message.c_str() );
                    stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
				}
				break;
			}

            if ( stepState == TA_Base_Core::FAILED_ACTIVE_STEP_STATE )
            {
                m_remark = message;
            }
			else if( true == success )
			{
				m_flowControl.setNextStep( m_params.successStep );
			}
			else
			{
				m_flowControl.setNextStep( m_params.failureStep );
			}
		}
		catch( const TA_Base_Core::ObjectResolutionException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what() << " for data point "
			    << m_params.dataPoint.in() << ".";

            m_remark = "Data point not accessible.";

			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::ObjectResolutionException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const ActiveStepParamException& e )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": "
			    << e.what() << " for data point "
			    << m_params.dataPoint.in() << ".";

            m_remark = "Wrong step parameter.";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_App::ActiveStepParamException", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		catch( const TA_Base_Bus::ScadaCorbaTypes::NotWriteable&)
		{
			std::ostringstream msg;
			msg << dumpContext() << ": No access to data point "
			    << m_params.dataPoint.in() << ".";

            m_remark = "Write access to data point denied";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "nexus::DataPointDmIf::NoAccess", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}

		catch( ... )
		{
			std::ostringstream msg;
			msg << dumpContext() << ": No access to data point "
			    << m_params.dataPoint.in() << ".";

            m_remark = "Data point not accessible.";
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown exception", msg.str().c_str() );
			stepState = TA_Base_Core::FAILED_ACTIVE_STEP_STATE;
		}
		return( stepState );
	}

	template<class T>
	bool ActiveDataPointCheckStep::test( const T& lhs, const T& rhs, TA_Base_Core::EDataPointTest testType )
	{
		switch( testType )
		{
			case TA_Base_Core::EQUAL_TO_TEST:
			{
				return( lhs == rhs );
			}
			break;

			case TA_Base_Core::NOT_EQUAL_TO_TEST:
			{
				return( lhs != rhs );
			}
			break;

			case TA_Base_Core::LESS_THAN_TEST:
			{
				return( lhs < rhs );
			}
			break;

			case TA_Base_Core::GREATER_THAN_TEST:
			{
				return( lhs > rhs );
			}
			break;

			case TA_Base_Core::LESS_THAN_OR_EQUAL_TO_TEST:
			{
				return( lhs <= rhs );
			}
			break;

			case TA_Base_Core::GREATER_THAN_OR_EQUAL_TO_TEST:
			{
				return( lhs >= rhs );
			}
			break;

			default:
			{
				return( false );
			}
			break;
		}
	}
} // TA_Base_App


#endif //  STEPTYPE_DATA_POINT_CHECK

