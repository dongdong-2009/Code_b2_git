/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PAMessageStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PaMessage parameter of a Station PA step.
  *
  **/

#if !defined(AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/pa/common/PaCommonDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PaMessageStepParameter

    class PaMessageStepParameter : public PlanStepParameter<bool>
    {
         // Operations
    public:
         PaMessageStepParameter(PlanStep& parent, const std::string& name, bool bPATrainMsg = false);
         PaMessageStepParameter(PlanStep& parent, const std::string& name, PAMessage destination, bool bPATrainMsg = false);
         virtual ~PaMessageStepParameter();

		 virtual std::vector<std::string> getStepParameterValues();
		 virtual const std::string displayValue() const;

         PAMessage getPAMessage() const { return m_paMessage; }
         void setPaMessageId(const unsigned long& messageId);
         void setPAMessageLibVersion (const unsigned short& libVersion);
         void setPAMessageLibSection (const unsigned short& libSection);
         unsigned long  getPAMessageId () { return m_paMessage.messageId; }
         unsigned short getPAMessageLibVersion () { return m_paMessage.libVersion; }
         unsigned short getPAMessageLibSection () { return m_paMessage.libSection; }
		 bool getIsTrain() const { return m_refSrcValue; }
     protected:
         virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
         PaMessageStepParameter& operator=(const PaMessageStepParameter& parameter);

         // Attributes
    private:
         PAMessage m_paMessage;
    };
}

#endif // !defined(AFX_PAMESSAGESTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
