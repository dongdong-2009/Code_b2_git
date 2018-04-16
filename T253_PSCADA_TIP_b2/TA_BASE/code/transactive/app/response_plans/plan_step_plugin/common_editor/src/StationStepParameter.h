/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/StationStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents the Station parameter of a Station TIS/PA step.
  *
  **/

#if !defined(AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
#define AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // StationStepParameter

    class PLAN_COMM_DLL StationStepParameter : public PlanStepParameter<unsigned long>
    {
    // Operations
    public:
		StationStepParameter(PlanStep& parent, const std::string& name, unsigned long& locationKey, const std::string& station, const bool allStations);
        virtual ~StationStepParameter();

		virtual std::vector<std::string> getStepParameterValues();
		virtual const std::string displayValue() const;

		std::string getStation() const;
		bool getAllStations() const;

        void setStation(const std::string& station, unsigned long ulLocationkey );
        void setAllStations();

    protected:
        virtual IPlanStepParameterForm* getEditDialog( bool canEdit );

    private:
        StationStepParameter& operator=(const StationStepParameter& parameter);

    // Attributes
    private:
		std::string m_station;
		bool m_allStations;
    };
}

#endif // !defined(AFX_STATIONSTEPPARAMETER_H__CD2466BF_0C74_49D6_9DD7_2296BED7A60A__INCLUDED_)
