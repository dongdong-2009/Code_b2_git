/**
  * DataPointAddDlg.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/DataPointAddDlg.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the dialog launched when the Add button is clicked from the main dialog.
  * It allows the user to select data points to add to the graph.
  *
  */

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/DataPointAddDlg.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/DataPointInfo.h"
#include "app/history/history_viewer/src/globals.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_App
{

    //
    // CDataPointAddDlg
    //
    CDataPointAddDlg::CDataPointAddDlg(CWnd* pParent /* = NULL */)
    : CDialog(CDataPointAddDlg::IDD, pParent),
      m_currentDataPointsSelected(false)
    {
	    //{{AFX_DATA_INIT(CDataPointAddDlg)
	    m_dataPointName = "";
	    //}}AFX_DATA_INIT

        std::string maxTrends = TA_Core::RunParams::getInstance().get(RPARAM_MAX_TRENDS_DISPLAYED);
        if(maxTrends.empty())
        {
            m_maxTrendsDisplayed = CFG_MAX_TRENDS_DISPLAYED_DEFAULT;
        }
        else
        {
            m_maxTrendsDisplayed = atoi(maxTrends.c_str());
        }
    }


    //
    // DoDataExchange
    //
    void CDataPointAddDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CDataPointAddDlg)
	    DDX_Control(pDX, IDC_EQUIPMENT_TREE, m_ScadaTree);
        DDX_Control(pDX, IDC_DATA_POINT_NAME, m_dataPointNameEditCtrl);
	    DDX_Text(pDX, IDC_DATA_POINT_NAME, m_dataPointName);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CDataPointAddDlg, CDialog)
	    //{{AFX_MSG_MAP(CDataPointAddDlg)
	    ON_BN_CLICKED(IDOK, onOK)
        ON_BN_CLICKED(IDCANCEL, onCancel)
	    ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIPMENT_TREE, OnSelchangedEquipmentTree)
        ON_EN_CHANGE(IDC_DATA_POINT_NAME, OnChangeEditCtrl)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // CDataPointAddDlg message handlers


    //
    // onOK
    //
    void CDataPointAddDlg::onOK() 
    {
	    UpdateData(true);

        int currentDisplayCnt = (dynamic_cast <CHistoryViewerDlg*> (GetParent()))->getCurrentGraphCount();

        //
        // Limit the maximum number of trends to be displayed
        // If more than the maximum allowed amount is selected, then
        // display a warning dialog and only display the maximum
        // allowed amount.
        //
        if(currentDisplayCnt == m_maxTrendsDisplayed)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << m_maxTrendsDisplayed;
            userMsg.showMsgBox(IDS_UW_120011);
            return;
        }

        //
        // Get the datapoint key and trending state for the selected datapoint
        //
        m_dataPointKeyList.clear();
        unsigned long dataPointKey;
        bool trendingEnabled;
        if(!retrieveKeyAndTrendingState(dataPointKey, trendingEnabled))
        {
            return;
        }
    
        m_currentDataPointsSelected |= (!trendingEnabled);

        (dynamic_cast <CHistoryViewerDlg*> (GetParent()))->addDataPoint(dataPointKey);
    }


    //
    // onCancel
    //
    void CDataPointAddDlg::onCancel() 
    {
        if(m_currentDataPointsSelected && !(dynamic_cast <CHistoryViewerDlg*> (GetParent()))->m_NowCheck)
        {
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_120012);
        }

        CDialog::OnCancel();
    }


    //
    // OnSelchangedEquipmentTree
    //
    void CDataPointAddDlg::OnSelchangedEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
    
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        //
	    // Get currently selected item
        //
	    bool isDataPoint = false;
        TA_Bus::CEntityTreeItem* selectedItem = dynamic_cast<TA_Bus::CEntityTreeItem*>(m_ScadaTree.getSelectedItem());
	    if (selectedItem)
	    {
		    isDataPoint = (selectedItem->getEntityData()->getType()==TA_Core::DataPointEntityData::getStaticType());
		    if (isDataPoint)
		    {
			    m_dataPointName = selectedItem->getEntityData()->getName().c_str();
                UpdateData(FALSE);
		    }

	    }

	    CWnd* okButton = GetDlgItem(IDOK);
	    okButton->EnableWindow(isDataPoint);

	    *pResult = 0;
    }


    //
    // OnInitDialog
    //
    BOOL CDataPointAddDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

        m_dataPointNameEditCtrl.SetLimitText(MAX_DATAPOINT_NAME_LENGTH);
    
	    // TODO: Add extra initialization here
		    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    //
    // OnChangeEditCtrl
    //
    void CDataPointAddDlg::OnChangeEditCtrl()
    {
        char buffer[250];
        m_dataPointNameEditCtrl.GetLine(0, buffer, 250);

        std::string name(buffer);
        CWnd* okButton = GetDlgItem(IDOK);
	    okButton->EnableWindow(!name.empty());
    }


    //
    // retrieveKeyAndTrendingState
    //
    bool CDataPointAddDlg::retrieveKeyAndTrendingState(unsigned long& dataPointKey, bool& trendingEnabled)
    {
        TA_Core::DataPointEntityData* dpData = NULL;
        try
        {
            TA_Core::IEntityData* iData = TA_Core::EntityAccessFactory::getInstance().getEntity(m_dataPointName.GetBuffer(0));

            if(iData->getType() == TA_Core::DataPointEntityData::getStaticType())
            {
                dpData = dynamic_cast <TA_Core::DataPointEntityData*> (iData);
            }
            else
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << m_dataPointName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120016);

                if(iData != NULL)
                {
                    delete iData;
                }
                return false;
            }
        }
        catch(const TA_Core::DatabaseException&)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << m_dataPointName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120015);

            return false;
        }
        catch(const TA_Core::DataException& ex)
        {
            if(ex.getFailType() == TA_Core::DataException::NO_VALUE)
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << m_dataPointName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120016);
            }
            else
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << m_dataPointName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120015);
            }

            return false;
        }

        dataPointKey = dpData->getKey();
    
        //
        // Check if the data point is a trendable datapoint and has been loaded from the database
        //
        try
        {
            CDataPointInfo::getInstance().GetDataPoint(dataPointKey);
        }
        catch(const CDataPointInfo::DatapointNotFoundException&)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << m_dataPointName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120016);
            return false;
        }

        //
        // Check if data point is already being trended
        //
        DataPointMap dataPointMap = CDataPointInfo::getInstance().GetGraphingDataPointMap();
        if(dataPointMap.find(dataPointKey) != dataPointMap.end())
        {
            if(dpData != NULL)
            {
                delete dpData;
            }

            TA_Bus::TransActiveMessage userMsg;
            userMsg << m_dataPointName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120016);
            return false;
        }

        trendingEnabled = dpData->getEnableTrending();

        if(dpData != NULL)
        {
            delete dpData;
        }

        return true;
    }

} // TA_App

