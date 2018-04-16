/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_mft_processor/src/MftMessageActiveStep.h $
  *  @author:  Huang Jian
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/10/30 19:47:26 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  A thread-safe Singleton factory for converting database data into
  *  application data and vice-versa.
  */

#ifndef MFTMESSAGEACTIVESTEP_H
#define MFTMESSAGEACTIVESTEP_H

#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStep.h"
#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftMessageStepParameter.h"

namespace TA_Base_Bus
{
    class MessageDialogHelper;
}

namespace TA_Base_App
{

class MftMessageActiveStep : public InteractiveStep
{
    // Operations
public:
	MftMessageActiveStep( const unsigned int unId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
		const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters );

    // InteractiveStep interface
    virtual void launch();
    virtual void close();

    virtual ~MftMessageActiveStep();

private:
	void reportCompletionStatus(const int completionStatus);

private:
    TA_Base_Bus::MessageDialogHelper* m_pMessageDialog;
	PopupMftMessageStepParameter* m_params;

	int m_eType;
	unsigned int m_ulLocation;
	unsigned int m_ulPlanId; 
	unsigned int m_ulInstanceNum;
	unsigned int m_ulStepNum;
	std::vector<std::string> m_parameters;
};

}

#endif //MFTMESSAGEACTIVESTEP_H