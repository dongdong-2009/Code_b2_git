#if !defined(AFX_MODIFYGRAPH_H__45170E7C_188F_45A7_93EE_149A628405C9__INCLUDED_)
#define AFX_MODIFYGRAPH_H__45170E7C_188F_45A7_93EE_149A628405C9__INCLUDED_

/**
  * ModifyGraph.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/ModifyGraph.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This dialog is launched from the main dialog. It allows modification to the mode of the 
  * graph (Live, Non-live), the representation (data type displayed), the start time and the
  * x-axis range.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

//{{AFX_INCLUDES()
//#include "bcgdurationctrl.h"
//}}AFX_INCLUDES

namespace TA_App
{
    struct DATATYPE
    {
        DWORD itemData;
        CString itemText;
    };

    //
    // CModifyGraph
    //
    class CModifyGraph : public CDialog
    {
    public:
        
        /**
		  * CModifyGraph
		  * 
		  * Constructor.
		  *
		  */
        CModifyGraph(BOOL nowCheck, int historyType, COleDateTime startDateTime, int rangeDays, 
	        COleDateTime rangeTime, const CComboBox& dataTypeCtrl, CWnd* pParent = NULL);

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CModifyGraph)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(CModifyGraph)
        virtual BOOL OnInitDialog();
        afx_msg void OnNowCheck();
        //afx_msg void OnUpdateRangedays();
        afx_msg void OnStartDateTimeChange(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnRangeDaysChange(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        void calculateRangeSeconds();

    protected:

	    std::vector <DATATYPE> m_DataTypes;

    public:

        // Dialog Data
        //{{AFX_DATA(CModifyGraph)
        enum { IDD = IDD_MODIFY };
        CDateTimeCtrl	m_rangeTimeCtrl;
        //CEdit	m_rangeDaysCtrl;
        CDateTimeCtrl m_rangeDaysCtrl;
        CButton	m_NowCtrl;
        CDateTimeCtrl	m_StartCtrl;
        CComboBox	m_DataTypeListCtrl;
        //CBCGDurationCtrl	m_RangeCtrl;
        COleDateTime	m_Start;
        BOOL	m_Now;
        int		m_DataType;
        long	m_RangeSeconds;
        int		m_rangeDays;
        COleDateTime	m_rangeTime;
        //}}AFX_DATA
    };

} // TA_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODIFYGRAPH_H__45170E7C_188F_45A7_93EE_149A628405C9__INCLUDED_)




