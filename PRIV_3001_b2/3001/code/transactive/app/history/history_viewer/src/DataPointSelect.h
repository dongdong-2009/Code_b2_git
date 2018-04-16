#if !defined(AFX_DATAPOINTSELECT_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)
#define AFX_DATAPOINTSELECT_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_

/**
  * DataPointSelect.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/DataPointSelect.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This dialog is launched by the main dialog when Add or Remove is clicked. It lists the data points
  * that can be added or removed.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/history/history_viewer/src/DataPointInfo.h"

namespace TA_App
{
    //
    // Forward declaration
    //
    class CHistoryViewerDlg;

    //
    // CDataPointSelect
    //
    class CDataPointSelect : public CDialog
    {
    public:

        /**
		  * CDataPointSelect
		  * 
		  * Constructor.
		  *
		  */
        CDataPointSelect(bool isDelete = false, CWnd* pParent = NULL);

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CDataPointSelect)
    protected:

        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CDataPointSelect)
        virtual BOOL OnInitDialog();
        virtual void OnOK();
	    afx_msg void OnFindButton();
	    afx_msg void OnClearButton();
	    //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        
        void InitialiseDataPointList();
        void SortDatapointList();
        void LoadDataPointList();
        static int CALLBACK CompareEvents(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

    public:

        const std::vector <unsigned long>& GetDataPointKeyList() 
        {
            return m_DataPointKeyList;
        }

    public:

        // Dialog Data
        //{{AFX_DATA(CDataPointSelect)
	    enum { IDD = IDD_DATAPOINT_SELECT };
        CListCtrl	m_DataPointList;
	    CString	m_searchString;
	    //}}AFX_DATA

    private:

        bool m_IsDelete;                                // True if deleting, False if adding
        DataPointMap m_DPMap;                           // Datapoint list
        CString m_DialogTitle;                          // Title for the dialog box
        std::vector <unsigned long> m_DataPointKeyList; // Array of the selected datapoint keys
        CHistoryViewerDlg* m_pParent;   
        int m_maxTrendsDisplayed;
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAPOINTSELECT_H__04E07008_870A_11D6_B248_0050BAB25370__INCLUDED_)




