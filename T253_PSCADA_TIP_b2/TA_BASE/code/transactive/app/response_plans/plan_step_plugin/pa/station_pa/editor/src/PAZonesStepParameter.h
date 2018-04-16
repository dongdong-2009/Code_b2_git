/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PAZonesStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the PaZones parameter of a Station PA step.
  *
  **/

#if !defined(AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PaZonesStepParameter

    class PaZonesStepParameter : public PlanStepParameter<std::vector<unsigned long>>
    {
         // Operations
    public:
         PaZonesStepParameter(PlanStep& parent, const std::string& name, std::vector<unsigned long>& zones);

         virtual ~PaZonesStepParameter();

 	     virtual std::vector<std::string> getStepParameterValues();
		 virtual const std::string displayValue() const;

		 std::vector<unsigned long> getPAZones () const { return m_refSrcValue; }
         void setPaZones(const std::vector<unsigned long>& zones);
         void addPaZoneID (const unsigned long zoneId);
         void resetZones ();
     protected:
         virtual IPlanStepParameterForm* getEditDialog(bool canEdit);

    private:
         PaZonesStepParameter& operator=(const PaZonesStepParameter& parameter);

    };
}

#endif // !defined(AFX_PAZONESSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
