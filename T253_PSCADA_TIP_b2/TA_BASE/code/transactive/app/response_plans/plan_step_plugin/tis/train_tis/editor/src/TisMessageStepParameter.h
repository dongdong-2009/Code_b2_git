/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TisMessageStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Message parameter of a Station/Train Traveller Info System step.
  *
  **/

#if !defined(AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_)
#define AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "TisConfigAccess.h"

namespace TA_Base_App
{
	typedef enum
	{
		MT_STATION,
		MT_TRAIN,
		MT_UNDEFINED
	}
	EMessageType;

    /////////////////////////////////////////////////////////////////////////////
    // TisMessageStepParameter

    class TisMessageStepParameter : public PlanStepParameter<EMessageType>
    {

    // Operations
    public:
        TisMessageStepParameter(PlanStep& parent, const std::string& name, TISMessage message, EMessageType messageType);
        virtual ~TisMessageStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
        virtual bool editParameter(unsigned short libraryVersion, TisConfigAccess::PredefinedTisMessage& selectedMessage);
		virtual const std::string displayValue() const;

        TISMessage getMessage() const { return m_message; }
        EMessageType getMessageType() const          { return m_refSrcValue; }
        std::string getMessageText() const           { return m_messageText; }

		void setMessage(const TISMessage& message);
        void resetMessage();

    protected:
        virtual IPlanStepParameterForm* getEditDialog(bool canEdit);

    private:
        TisMessageStepParameter& operator=(const TisMessageStepParameter& parameter);

    // Attributes
    private:
        TISMessage m_message;
        std::string m_messageText;
    };
}

#endif // !defined(AFX_TISMESSAGEPARAMETER_H__4F9F0E3D_F61D_464C_8EE4_2B3EB5CAB10F__INCLUDED_)
