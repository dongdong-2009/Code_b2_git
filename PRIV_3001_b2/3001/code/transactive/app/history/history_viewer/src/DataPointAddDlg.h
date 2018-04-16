#if !defined(AFX_DATAPOINTADDDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_)
#define AFX_DATAPOINTADDDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_

/**
  * DataPointAddDlg.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/DataPointAddDlg.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is the dialog launched when the Add button is clicked from the main dialog.
  * It allows the user to select data points to add to the graph.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/history/history_viewer/src/resource.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

namespace TA_App
{
	//
	// CDataPointAddDlg
    //
	class CDataPointAddDlg : public CDialog
	{
	public:

        /**
		  * CHistoryViewerDlg
		  * 
		  * Constructor.
		  *
		  */
		CDataPointAddDlg(CWnd* pParent = NULL);

        // Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CPlanStepParameterDataPointEditDlg)
    protected:
	
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

    protected:

		// Generated message map functions
		//{{AFX_MSG(CPlanStepParameterDataPointEditDlg)
        afx_msg void onOK();
        afx_msg void onCancel();
        afx_msg void OnSelchangedEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult);
        virtual BOOL OnInitDialog();
        afx_msg void OnChangeEditCtrl();
        //}}AFX_MSG
		DECLARE_MESSAGE_MAP()

    private:

        bool retrieveKeyAndTrendingState(unsigned long& dataPointKey, bool& trendingEnabled);

    public:

        const std::vector <unsigned long>& GetDataPointKeyList() 
        {
            return m_dataPointKeyList;
        }

    private:

        std::vector <unsigned long> m_dataPointKeyList; // Array of the selected datapoint keys
        int m_maxTrendsDisplayed;
        bool m_currentDataPointsSelected;

    public:

	    // Dialog Data
		//{{AFX_DATA(CPlanStepParameterDataPointEditDlg)
	    enum { IDD = IDD_DATA_POINT_ADD_DLG };
	    TA_Bus::CScadaTreeCtrl	m_ScadaTree;
	    CString	m_dataPointName;
        CEdit m_dataPointNameEditCtrl;
	    //}}AFX_DATA
	};

}  // End name space

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAPOINTADDDLG_H__DE0E53E6_7C74_41D8_9756_A0B9949D0806__INCLUDED_)
