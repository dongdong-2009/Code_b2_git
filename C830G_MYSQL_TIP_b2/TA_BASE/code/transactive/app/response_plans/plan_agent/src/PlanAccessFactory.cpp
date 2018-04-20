/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAccessFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "PlanAgent.h"


namespace TA_Base_App
{

	PlanAccessFactoryImpl::PlanAccessFactoryImpl()
	{
	}
	
    // Improve performance, load all steps in one sql
	ActiveStepList* PlanAccessFactoryImpl::getActiveStepList( TA_Base_Core::NodeId plan, IActivePlanFlow& flowControl, std::string& reason ) const
	{
        LOG_GENERIC( SOURCE_INFO, INFO_LOG_LEVEL, "Begin to load steps" );
		std::auto_ptr<ActiveStepList> stepList( new ActiveStepList );

        // No need to check plan step count, there is always an end step at least.
// 		// find out how many steps there are
// 		TA_Base_Core::StepNumber stepCount = TA_Base_Bus::PlanDataReader::instance()->getStepCount( plan );
// 		
// 		// Return an empty step list.
// 		if( 0 == stepCount )
// 		{
// 			return( stepList.release() );
// 		}

//  		TA_Base_Core::StepDetails_var stepDetails = TA_Base_Bus::PlanDataReader::instance()->getAllStepDetails( plan );
        TA_Base_Core::Steps_var varAllSteps = TA_Base_Bus::PlanDataReader::instance()->getAllSteps( plan );

		// Return an empty step list.
//         if( 0 == stepDetails->length() )
        if( 0 == varAllSteps->length() )
		{
			return( stepList.release() );
		}

// 		TA_ASSERT( stepDetails->length() == stepCount, "Step details does not equal step count" ); 

		TA_Base_Core::StepNumber stepIndex = 1;

        bool stepDefined = false;

		//TD20350 xinsong++
		int GRC3PrevStepIndex = -1; //the previous GRC3 step index 
		GRC3DataPointSetComplexStep * currentGRCType3Step = NULL;
         

// 		for( stepIndex = 1; stepIndex<= stepCount; stepIndex++ )
		for( stepIndex = 1; stepIndex<= varAllSteps->length(); stepIndex++ )
		{
// 			TA_Base_Core::StepParameters_var stepParameters;
            stepDefined = false;

// 			stepParameters = TA_Base_Bus::PlanDataReader::instance()->getStepParameters( plan, stepIndex, true );

//             switch( stepParameters->_d() )
            switch( varAllSteps[stepIndex-1].parameters._d() )
			{
				case TA_Base_Core::END_STEP:
				{
//                     stepList->push_back( new ActiveEndStep(stepDetails[stepIndex-1], flowControl) );
                    stepList->push_back( new ActiveEndStep(varAllSteps[stepIndex-1].detail, flowControl) );
                    stepDefined = true;

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::END_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::ABORT_STEP:
				{
                    #if defined ( STEPTYPE_ABORT )

//                     stepList->push_back( new ActiveAbortStep(stepDetails[stepIndex-1], flowControl) );
                    stepList->push_back( new ActiveAbortStep(varAllSteps[stepIndex-1].detail, flowControl) );
                    stepDefined = true;

				    #endif // STEPTYPE_ABORT

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::ABORT_STEP has not been defined.";
                    }

                }
				break;

				case TA_Base_Core::ACTIVE_PLAN_CHECK_STEP:
				{
					#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )

//                     stepList->push_back( new ActiveActivePlanCheckStep( stepDetails[stepIndex-1], stepParameters->activePlanCheck(), flowControl) );
                    stepList->push_back( new ActiveActivePlanCheckStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.activePlanCheck(), flowControl) );
                    stepDefined = true;

				    #endif //STEPTYPE_ACTIVE_PLAN_CHECK

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::ACTIVE_PLAN_CHECK_STEP has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP:
				{
				    #if defined ( STEPTYPE_ASSIGN_CAMERA_TO_MONITOR )

//                     stepList->push_back( new ActiveAssignCameraToMonitorStep( stepDetails[stepIndex-1], stepParameters->cameraMonitor(), flowControl) );
                    stepList->push_back( new ActiveAssignCameraToMonitorStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.cameraMonitor(), flowControl) );
                    stepDefined = true;

                    #endif //ASSIGN_CAMERA_TO_MONITOR_STEP

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::ASSIGN_CAMERA_TO_MONITOR_STEP has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::DATA_POINT_CHECK_STEP:
				{
					#if defined ( STEPTYPE_DATA_POINT_CHECK )

//                     stepList->push_back( new ActiveDataPointCheckStep(stepDetails[stepIndex-1], stepParameters->dataPointCheck(), flowControl) );
                    stepList->push_back( new ActiveDataPointCheckStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.dataPointCheck(), flowControl) );
                    stepDefined = true;

				    #endif //STEPTYPE_DATA_POINT_CHECK

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::DATA_POINT_CHECK_STEP has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::DATA_POINT_SET_STEP:
				{
					#if defined ( STEPTYPE_DATA_POINT_SET )

					ActiveStep * pActiveStep = NULL;

//                     bool fGRCType3Step = isGRCType3Step(stepDetails[stepIndex-1]); 
                    bool fGRCType3Step = isGRCType3Step(varAllSteps[stepIndex-1].detail); 

//                     ActiveDataPointSetStep * pDataPointSetStep = new ActiveDataPointSetStep(stepDetails[stepIndex-1], stepParameters->dataPointSet(), flowControl, fGRCType3Step );
					ActiveDataPointSetStep * pDataPointSetStep = new ActiveDataPointSetStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.dataPointSet(), flowControl, fGRCType3Step );
					if ( fGRCType3Step )
					{
						if ( GRC3PrevStepIndex + 1 < stepIndex ) 
						{
//                             currentGRCType3Step = new GRC3DataPointSetComplexStep(stepDetails[stepIndex-1], flowControl);
                            currentGRCType3Step = new GRC3DataPointSetComplexStep(varAllSteps[stepIndex-1].detail, flowControl);
                            pActiveStep = currentGRCType3Step;
						}
						else
						{
							TA_ASSERT(GRC3PrevStepIndex == stepIndex-1, "it should be stepindex - 1");
							TA_ASSERT(currentGRCType3Step!=NULL, "the GRCType3Step pointer should not be NULL" );
//                             pActiveStep = new ActiveNullStep(stepDetails[stepIndex-1], flowControl);
                            pActiveStep = new ActiveNullStep(varAllSteps[stepIndex-1].detail, flowControl);
						}
						currentGRCType3Step->addChildStep(pDataPointSetStep);
						GRC3PrevStepIndex = stepIndex;
					}
					else
					{
						pActiveStep = pDataPointSetStep;
					}

                    stepList->push_back( pActiveStep );
                    stepDefined = true;
				    #endif //STEPTYPE_DATA_POINT_SET

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::DATA_POINT_SET_STEP has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::DECISION_STEP:
				{
					#if defined ( STEPTYPE_DECISION )
                    
//                     stepList->push_back( new ActiveDecisionStep(stepDetails[stepIndex-1], stepParameters->decision(), flowControl) );
                    stepList->push_back( new ActiveDecisionStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.decision(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_DECISION

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::DECISION_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::JUMP_STEP:
				{
					#if defined ( STEPTYPE_JUMP )

//                     stepList->push_back( new ActiveJumpStep(stepDetails[stepIndex-1], stepParameters->jump(), flowControl) );
                    stepList->push_back( new ActiveJumpStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.jump(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_JUMP

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::JUMP_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::LOG_STEP:
				{
					#if defined ( STEPTYPE_LOG )

//                     stepList->push_back( new ActiveLogStep(stepDetails[stepIndex-1], stepParameters->log(), flowControl) );
                    stepList->push_back( new ActiveLogStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.log(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_LOG

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::LOG_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::ACTIVATE_CAMERA_PRESET_STEP:
				{
				    #if defined ( STEPTYPE_ACTIVATE_CAMERA_PRESET )

//                     stepList->push_back( new ActiveAssignCameraToPresetStep( stepDetails[stepIndex-1], stepParameters->cameraPreset(), flowControl) );
                    stepList->push_back( new ActiveAssignCameraToPresetStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.cameraPreset(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_ACTIVATE_CAMERA_PRESET

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::ACTIVATE_CAMERA_PRESET has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::PROMPT_STEP:
				{
					#if defined ( STEPTYPE_PROMPT )

//                     stepList->push_back( new ActivePromptStep(stepDetails[stepIndex-1], stepParameters->prompt(), flowControl) );
                    stepList->push_back( new ActivePromptStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.prompt(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_PROMPT

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::PROMPT_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::RUN_PLAN_STEP:
				{
					#if defined ( STEPTYPE_RUN_PLAN )

//                     stepList->push_back( new ActiveRunPlanStep( stepDetails[stepIndex-1], stepParameters->runPlan(), flowControl) );
                    stepList->push_back( new ActiveRunPlanStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.runPlan(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_RUN_PLAN

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::RUN_PLAN_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::TERMINATE_PLAN_STEP:
				{
					#if defined ( STEPTYPE_TERMINATE_PLAN )

//                     stepList->push_back( new ActiveTerminatePlanStep( stepDetails[stepIndex-1], stepParameters->terminatePlan(), flowControl) );
                    stepList->push_back( new ActiveTerminatePlanStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.terminatePlan(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_TERMINATE_PLAN

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::TERMINATE_PLAN_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::VMS_SET_SCENE_STEP:
				{
                    #if defined ( STEPTYPE_VMS_SET_SCENE )
//                     stepList->push_back( new ActiveVmsSetSceneStep( stepDetails[stepIndex-1], stepParameters->vmsSetScene(), flowControl) );
                    stepList->push_back( new ActiveVmsSetSceneStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsSetScene(), flowControl) );
                    stepDefined = true;

                    #endif //STEPTYPE_VMS_SET_SCENE

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::VMS_SET_SCENE_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::LAUNCH_GUI_APP_STEP:
				{
                    #if defined ( STEPTYPE_LAUNCH_GUI_APP )
//                     stepList->push_back( new ActiveLaunchGuiAppStep( stepDetails[stepIndex-1], stepParameters->guiAppParameters(), flowControl) );
                    stepList->push_back( new ActiveLaunchGuiAppStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.guiAppParameters(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_LAUNCH_GUI_APP
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::LAUNCH_GUI_APP_STEP has not been defined.";
                    }
				}
				break;

                case TA_Base_Core::VMS_BLANKOUT_STEP:
                {
                    #if defined ( STEPTYPE_VMS_BLANKOUT )
//                     stepList->push_back( new ActiveVmsBlankoutStep( stepDetails[stepIndex-1], stepParameters->vmsBlankout(), flowControl) );
                    stepList->push_back( new ActiveVmsBlankoutStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsBlankout(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_VMS_BLANKOUT
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::STEPTYPE_VMS_BLANKOUT has not been defined.";
                    }
                }
                break;

                case TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP:
                {
                    #if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )
//                     stepList->push_back( new ActiveVmsRestoreScheduleStep( stepDetails[stepIndex-1], stepParameters->vmsRestoreSchedule(), flowControl) );
                    stepList->push_back( new ActiveVmsRestoreScheduleStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.vmsRestoreSchedule(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_VMS_RESTORE_SCHEDULE
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::VMS_RESTORE_SCHEDULE_STEP has not been defined.";
                    }

                }
                break;

                case TA_Base_Core::VIDEO_WALL_STEP:
                {
                    #if defined ( STEPTYPE_VIDEOWALL )
//                     stepList->push_back( new ActiveVideoWallStep( stepDetails[stepIndex-1], stepParameters->videoWallParams(), flowControl) );
                    stepList->push_back( new ActiveVideoWallStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.videoWallParams(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_VIDEOWALL
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::VIDEO_WALL_STEP has not been defined.";
                    }
                }
                break;

                case TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP:
                {
                    #if defined ( STEPTYPE_STATION_PUBLIC_ADDRESS )
//                     stepList->push_back( new ActiveStationPaStep( stepDetails[stepIndex-1], stepParameters->stationPublicAddress(), flowControl) );
                    stepList->push_back( new ActiveStationPaStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.stationPublicAddress(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_STATION_PUBLIC_ADDRESS
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::STATION_PUBLIC_ADDRESS_STEP has not been defined.";
                    }
                }
                break;

                case TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP:
                {
                    #if defined ( STEPTYPE_TRAIN_PUBLIC_ADDRESS )
//                     stepList->push_back( new ActiveTrainPaStep( stepDetails[stepIndex-1], stepParameters->trainPublicAddress(), flowControl) );
                    stepList->push_back( new ActiveTrainPaStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.trainPublicAddress(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_TRAIN_PUBLIC_ADDRESS
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::TRAIN_PUBLIC_ADDRESS_STEP has not been defined.";
                    }
                }
                break;

                case TA_Base_Core::SEND_SMS_MESSAGE_STEP:
                {
                    #if defined ( STEPTYPE_SEND_SMS_MESSAGE )
//                     stepList->push_back( new ActiveSendSmsMessageStep( stepDetails[stepIndex-1], stepParameters->sendSmsMessage(), flowControl ) );
                    stepList->push_back( new ActiveSendSmsMessageStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.sendSmsMessage(), flowControl ) );
                    stepDefined = true;
                    #endif //STEPTYPE_SEND_SMS_MESSAGE

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::SEND_SMS_MESSAGE_STEP has not been defined.";
                    }
                }
                break;

				case TA_Base_Core::LAUNCH_SCHEMATIC_STEP:
				{
                    #if defined ( STEPTYPE_LAUNCH_SCHEMATIC )
//                     stepList->push_back( new ActiveLaunchSchematicStep( stepDetails[stepIndex-1], stepParameters->launchSchematic(), flowControl) );
                    stepList->push_back( new ActiveLaunchSchematicStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.launchSchematic(), flowControl) );
                    stepDefined = true;
                    #endif //STEPTYPE_LAUNCH_SCHEMATIC
                    
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::LAUNCH_SCHEMATIC_STEP has not been defined.";
                    }
				}
				break;

				case TA_Base_Core::EVALUATE_EXPRESSION_STEP:
				{
					#if defined ( STEPTYPE_EVALUATE_EXPRESSION)
//                     stepList->push_back( new ActiveEvaluateExpressionStep( stepDetails[stepIndex-1], stepParameters->evaluateExpression(), flowControl) );
                    stepList->push_back( new ActiveEvaluateExpressionStep( varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.evaluateExpression(), flowControl) );
					stepDefined = true;
					#endif //STEPTYPE_EVALUATE_EXPRESSION
					
					if ( !stepDefined )
					{
						reason = "Error: TA_Base_Core::EVALUATE_EXPRESSION_STEP has not been defined.";
					}
				}
				break;

				case TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP:
				{
					#if defined ( STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM )
//                     stepList->push_back( new ActiveStationTisStep(stepDetails[stepIndex-1], stepParameters->stationTravellerInfoSystem(), flowControl) );
                    stepList->push_back( new ActiveStationTisStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.stationTravellerInfoSystem(), flowControl) );
                    stepDefined = true;
				    #endif //STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::STATION_TRAVELLER_INFO_SYSTEM_STEP has not been defined.";
                    }
                }
				break;

				case TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP:
				{
					#if defined ( STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM )
//                     stepList->push_back( new ActiveTrainTisStep(stepDetails[stepIndex-1], stepParameters->trainTravellerInfoSystem(), flowControl) );
                    stepList->push_back( new ActiveTrainTisStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.trainTravellerInfoSystem(), flowControl) );
                    stepDefined = true;
				    #endif //STEPTYPE_TRAIN_TRAVELLER_INFO_SYSTEM

                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::TRAIN_TRAVELLER_INFO_SYSTEM_STEP has not been defined.";
                    }
                }
				break;
				// lizettejl++ Failure Step Summary (DP-changes)
				case TA_Base_Core::FAILURE_SUMMARY_STEP:
				{
					#if defined ( STEPTYPE_FAILURE_SUMMARY_STEP )
//                     stepList->push_back( new  ActiveFailureSummaryStep(stepDetails[stepIndex-1], stepParameters->failureSummary(), flowControl) );
                    stepList->push_back( new  ActiveFailureSummaryStep(varAllSteps[stepIndex-1].detail, varAllSteps[stepIndex-1].parameters.failureSummary(), flowControl) );
					stepDefined = true;
					#endif // STEPTYPE_FAILURE_SUMMARY_STEP 
					
					if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::FAILURE_SUMMARY_STEP has not been defined.";
                    }
				}
				break;
				//++lizettejl
				case TA_Base_Core::UNDEFINED_STEP:
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "found undefined step" );
                    stepDefined = true;
                    if ( !stepDefined )
                    {
                        reason = "Error: TA_Base_Core::UNDEFINED_STEP has not been defined.";
                    }
				}
				break; 
			}

            if ( !stepDefined )
                break;
		}

        if ( stepDefined )
        {
            LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Loaded %lu steps", stepList->size() );
        }
        else
        {
		    LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Failed to load all steps, unusable steptype encountered: %s", reason.c_str() );
        }

		return( stepList.release() );
	}

	bool PlanAccessFactoryImpl::isGRCType3Step(const TA_Base_Core::StepDetail& stepDetail) const
	{
		if ( stepDetail.skip )
		{
			return false;
		}
		if ( ! stepDetail.ignoreFailure )
		{
			return false;
		}
		return stepDetail.nowait;
	}

} // TA_Base_App
