//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterDataPointEditDlg.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates selecting a single data point from all configured data points.

#include "StdAfx.h"

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "Resource.h"
#include "DataPointStepParameter.h"
#include "PlanStepParameterDataPointEditDlg.h"

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterDataPointEditDlg dialog

CPlanStepParameterDataPointEditDlg::CPlanStepParameterDataPointEditDlg(DataPointStepParameter& parameter,bool canEdit)
: CPlanStepParameterEditDlg( parameter, IDD_PLAN_STEP_PARAMETER_DATA_POINT_EDIT_DLG, canEdit )
{
    FUNCTION_ENTRY( "CPlanStepParameterDataPointEditDlg" );

    // {{AFX_DATA_INIT(CPlanStepParameterDataPointEditDlg)
    m_dataPointName = parameter.value().c_str();
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CPlanStepParameterDataPointEditDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepParameterEditDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepParameterDataPointEditDlg)
    DDX_Control(pDX, IDC_EQUIPMENT_TREE, m_ScadaTree);
    DDX_Text(pDX, IDC_DATA_POINT_NAME, m_dataPointName);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepParameterDataPointEditDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPlanStepParameterDataPointEditDlg)
    ON_BN_CLICKED(IDOK, OnOK)
    ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIPMENT_TREE, OnScadaTreeItemSelectionChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepParameterDataPointEditDlg message handlers

void CPlanStepParameterDataPointEditDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    DataPointStepParameter* parameter = dynamic_cast<DataPointStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");

    UpdateData();
    parameter->value((LPCSTR)m_dataPointName);

    EndDialog(IDOK);

    FUNCTION_EXIT;
}


void CPlanStepParameterDataPointEditDlg::OnScadaTreeItemSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnScadaTreeItemSelectionChanged" );

    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

    // Get currently selected item
    bool isDataPoint = false;
    TA_Base_Bus::CEntityTreeItem* selectedItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(m_ScadaTree.getSelectedItem());
    if (selectedItem)
    {
        isDataPoint = (selectedItem->getEntityData()->getType()==TA_Base_Core::DataPointEntityData::getStaticType());
        m_dataPointName = (isDataPoint? _T(selectedItem->getEntityData()->getName().c_str()) : _T(""));

        UpdateData(FALSE);
    }

    CWnd* okButton = GetDlgItem(IDOK);
    if (okButton)
    {
        okButton->EnableWindow(isDataPoint);
    }

    // TODO: Add your control notification handler code here

    *pResult = 0;

    FUNCTION_EXIT;
}


BOOL CPlanStepParameterDataPointEditDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CPlanStepParameterEditDlg::OnInitDialog();

    // TODO: Add extra initialization here
    CWaitCursor waitCursor;

    m_ScadaTree.ModifyStyle(TVS_LINESATROOT, 0);
    m_ScadaTree.Expand(m_ScadaTree.GetRootItem(), TVE_EXPAND);

    DataPointStepParameter* parameter = dynamic_cast<DataPointStepParameter*>(GetStepParameter());
    TA_ASSERT(parameter!=NULL,"Incorrect parameter type.");
    m_ScadaTree.setInitialDataPoint(parameter->value());

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

}