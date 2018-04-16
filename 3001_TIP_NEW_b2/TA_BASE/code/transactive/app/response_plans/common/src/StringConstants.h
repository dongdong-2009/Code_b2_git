//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/common/src/StringConstants.h $
// @author:  Mahaveer Pareek
// @version: $Revision: #8 $
//
// Last modification: $DateTime: 2014/04/21 18:21:35 $
// Last modified by:  $Author: qi.huang $
//
// This file contains project-specific preprocessor definitions used 
// for conditional compilation of Plan Agent and Plan Manager.
//
// Each project should uncomment the relevant PROJECT_XXXX definition.
//

#ifndef STRING_CONSTANTS_HEADER_INCLUDED
#define STRING_CONSTANTS_HEADER_INCLUDED

const char* const RemarkPlanManagerIsNotRegistered = "Plan Manager is not registered";
const char* const RemarkWaitingForOperatorResponse = "Waiting for Operator's response";
const char* const RemarkPlanPausedByOperator = "Plan paused by operator";
const char* const RemarkPlanCompletedWithFailedSteps = "Plan completed with failed step(s)";
const char* const RemarkPlanCompletedSuccess = "Plan completed successfully";
const char* const RemarkStepExecutionSucceed = "Step execution successful";
const char* const RemarkStepExecutionFailed = "Step execution failed";
const char* const RemarkStepExecutionTimeout = "Step execution timeout";
const char* const RemarkWaitingForAppToBeLaunched = "Waiting for application to be launched";
const char* const RemarkNoPlanManagerForOperatorInput = "No Plan Manager running for operator input to interactive step";
const char* const RemarkExecutingStep = "Executing step";
const char* const RemarkExecutingNoWaitSteps = "Executing step(s) without waiting";
const char* const RemarkPlanStepExecuted = "Step executed";
const char* const RemarkPlanAbort = "Plan aborted";
const char* const RemarkPlanPaused = "Plan paused";
const char* const RemarkPlanResumed = "Plan resumed";
const char* const RemarkPlanExpired = "Plan expired";
const char* const RemarkCanNotFoundVideoSwitchAgent = "Error in resolving Video Switch Agent";
const char* const RemarkCanNotLockCamera = "Could not get lock on Camera";
const char* const RemarkAttemptCompareDPwithNonfloatValue = "Attempt to compare float data point with non-float value for data point";
const char* const RemarkAttemptCompareDPwithNonBoolValue = "Attempt to compare boolean data point with non-boolean value for data point";
const char* const RemarkAttemptCompareDPwithNonTextValue = "Attempt to compare text data point with non-text value for data point";
const char* const RemarkInvalidDatapointValue = "Invalid data point value";
const char* const RemarkBadDatapointQuality = "Bad data point quality";
const char* const RemarkDataPointTimeLaterThanNow = "DataPoint time is later than ISCS time";
const char* const RemarkInvalidDatapoint = "Data point not accessible";
const char* const RemarkInvalidStepParameter = "Wrong step parameter";
const char* const RemarkPlanManagerInteractiveActionError = "Plan Manager Interactive Action failed";
const char* const RemarkInvalidExpress = "Invalid expression";
const char* const RemarkApplicationUnableToLaunch = "Application unable to launch";
const char* const RemarkWaitingSchematicLaunch = "Waiting for schematic to be launched";
const char* const RemarkSchematicCanNotLaunch = "Schematic unable to launch";
const char* const RemarkWaitPlanControllerLaunch = "Waiting for Plan Controller to be launched";
const char* const RemarkPlanControllerChildPlanCanNotLaunch = "Plan Controller for child plan unable to launch";
const char* const RemarkNoFoundPlan = "Plan not found";
const char* const RemarkRunPlanFailed = "Failed to run Plan";
const char* const RemarkPlanStepSkipped = "Step skipped";
const char* const RemarkWriteDataPointFailed = "Data point write failed";
const char* const RemarkAccessDataPointDenied = "Write Access to Data Point write denied";
const char* const RemarkCustomizationInProgress = "Customization in progress";
const char* const RemarkPlanStarted = "Plan started";
const char* const RemarkNoMftResponse = "No MFT response to the request";
const char* const RemarkOperatorStopPlan = "has stopped the plan";
const char* const RemarkOperatorTookControl = "has taken the plan";
const char* const RemarkDssEngineStopPlan = "stop according to Plan DSS rule";
const char* const RemarkSystemStopPlan = "Hang up message received from System Controller";

#endif // STRING_CONSTANTS_HEADER_INCLUDED
