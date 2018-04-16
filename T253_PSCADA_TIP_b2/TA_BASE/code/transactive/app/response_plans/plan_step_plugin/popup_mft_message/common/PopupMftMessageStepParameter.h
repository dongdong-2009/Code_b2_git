#ifndef POPUP_MFT_MESSAGE_STEP_PARAMETER_INCLUDE_H
#define POPUP_MFT_MESSAGE_STEP_PARAMETER_INCLUDE_H

#include <string>
#include "app/response_plans/plan_step_plugin/common/src/BaseStepParameter.h"
//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_Core
{
struct StepDetail;
}

namespace TA_Base_App
{

const char* const ParamEventName = "Event Name";
const char* const ParamDecisionMessage = "Decision Message";
const char* const ParamMessageBoxType = "MessageBox Style";
const char* const ParamOperatorProfileNumber = "Operator Profile";
const char* const ParamTimeSpan = "Timespan";

const char* const ParamYesStep = "Yes Step";
const char* const ParamNoStep = "No Step";
const char* const ParamPendingStep = "Pending Step";
const char* const ParamAbortStep = "Abort Step";
const char* const ParamIgnoreStep = "Ignore Step";
const char* const ParamRetryStep = "Retry Step";
const char* const ParamOkStep = "Ok Step";
const char* const ParamCancelStep = "Cancel Step";

class PopupMftMessageStepParameter: public BaseStepParameter
{
public:
    PopupMftMessageStepParameter(const BasicParameter& detail)
        : BaseStepParameter(detail),
		  decisionMessage(""),
          messageBoxType(0),
          operatorProfileNumber(-1),
          timeSpan(1),
		  abortStep(0),
		  retryStep(0),
		  ignoreStep(0),
		  okStep(0),
		  cancelStep(0),
		  yesStep(0),
		  noStep(0),
		  pendingStep(0)
    {
    }

    PopupMftMessageStepParameter(const unsigned long stepId, const int stepType)
        : BaseStepParameter(stepId, stepType),
		  decisionMessage(""),
          messageBoxType(0),
          operatorProfileNumber(-1),
          timeSpan(1),
		  abortStep(0),
		  retryStep(0),
		  ignoreStep(0),
		  okStep(0),
		  cancelStep(0),
		  yesStep(0),
		  noStep(0),
		  pendingStep(0)
    {
    }
    virtual ~PopupMftMessageStepParameter() {}

    std::string decisionMessage;
    unsigned long messageBoxType;
    unsigned long operatorProfileNumber;
    unsigned long timeSpan;
	unsigned int abortStep;
	unsigned int retryStep;
	unsigned int ignoreStep;
	unsigned int okStep;
	unsigned int cancelStep;
	unsigned int yesStep;
	unsigned int noStep;
	unsigned int pendingStep;

protected:
    virtual void serializeImp(TA_Base_Bus::Serialization& ioSerialization)
    {
        //ioSerialization.serialize(eventName, ParamEventName);
        ioSerialization.serialize(decisionMessage, ParamDecisionMessage);
		ioSerialization.serialize(yesStep, ParamYesStep);
		ioSerialization.serialize(noStep, ParamNoStep);
		ioSerialization.serialize(pendingStep, ParamPendingStep);
        ioSerialization.serialize(operatorProfileNumber, ParamOperatorProfileNumber);
        ioSerialization.serialize(messageBoxType, ParamMessageBoxType);
		ioSerialization.serialize(abortStep, ParamAbortStep);
		ioSerialization.serialize(ignoreStep, ParamIgnoreStep);
		ioSerialization.serialize(retryStep, ParamRetryStep);
		ioSerialization.serialize(okStep, ParamOkStep);
		ioSerialization.serialize(cancelStep, ParamCancelStep);
        ioSerialization.serialize(timeSpan, ParamTimeSpan);
    }

private:
    //PopupMftMessageStepParameter(const PopupMftMessageStepParameter&);
    PopupMftMessageStepParameter& operator=(const PopupMftMessageStepParameter&);
};
}

#endif //POPUP_MFT_MESSAGE_STEP_PARAMETER_INCLUDE_H
