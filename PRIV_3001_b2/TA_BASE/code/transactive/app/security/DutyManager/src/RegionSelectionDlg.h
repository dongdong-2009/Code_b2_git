#if !defined(AFX_REGIONSELECTIONDLG_H__4EDFE482_14A8_497D_B9AA_B1FD01E71139__INCLUDED_)
#define AFX_REGIONSELECTIONDLG_H__4EDFE482_14A8_497D_B9AA_B1FD01E71139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

    /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/security/DutyManager/src/RegionSelectionDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This dialog enables the operator to select from a list of regions for which
  * they currently hold region duties.
  *
  */

namespace TA_Base_App
{
    class RegionSelectionDlg : public CDialog
    {
    // Operations
    public:
	    RegionSelectionDlg(CWnd* pParent = NULL);
        virtual ~RegionSelectionDlg();

        TA_Base_Bus::RegionKeyList getSelectedRegionKeys() const;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RegionSelectionDlg)
    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(RegionSelectionDlg)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    afx_msg void OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(RegionSelectionDlg)
	    enum { IDD = IDD_REGION_SELECT_DLG };
	    CListCtrl	m_assignedRegionList;
	    //}}AFX_DATA

        TA_Base_Bus::RegionKeyList m_selectedRegionKeys;

        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
        TA_Base_Bus::RegionDutyMatrix& m_regionDutyMatrix;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONSELECTIONDLG_H__4EDFE482_14A8_497D_B9AA_B1FD01E71139__INCLUDED_)
