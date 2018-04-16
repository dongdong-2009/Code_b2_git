/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  **/

#if !defined(AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_)
#define AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET) || defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#include <string>
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepParameter.h"
#include "PlanStepParameterListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointStepParameter

    class DataPointStepParameter : public PlanStepParameter
    {
    // Operations
    public:
        DataPointStepParameter(PlanStep& parent, std::string name, std::string dataPoint);
        DataPointStepParameter(PlanStep& parent, DataPointStepParameter& parameter);
        DataPointStepParameter(DataPointStepParameter& parameter);
        virtual ~DataPointStepParameter();

        virtual void addToParameterList(CPlanStepParameterListCtrl& listCtrl);

        std::string getDataPoint() const { return m_dataPoint; }
        void setDataPoint(std::string dataPoint);

    protected:
        virtual CPlanStepParameterEditDlg* getEditDialog(bool canEdit);

    private:
        DataPointStepParameter& operator=(const DataPointStepParameter& parameter);


    // Attributes
    private:
        std::string m_dataPoint;
    };
}

#endif // defined (STEPTYPE_DATA_POINT_CHECK) || defined (STEPTYPE_DATA_POINT_SET) || defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#endif // !defined(AFX_DATAPOINTSTEPPARAMETER_H__E0865C1C_94EC_4AEF_813B_3749443C60D6__INCLUDED_)
