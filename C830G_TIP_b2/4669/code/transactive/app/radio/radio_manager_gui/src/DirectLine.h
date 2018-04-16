/*
 * The soiurce code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form
 *
 * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DirectLine.h $
 * @author:  Andy Parker
 * @version: $Revision: #1 $
 *
 * Last mofification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * This class manages the direct dial property page for the
 * radio and telephone sub systems.  It is implemented as 
 * six fixed buttons (hotlines) and 32 pageable buttons switched
 * using a tab control.
 *
 * Each button has a label and associated number.
 *
 */
#ifndef __DIRECT_LINE_H__
#define __DIRECT_LINE_H__

class CDirectLineCache;

// CDirectLinePage dialog Class
class CDirectLinePage : public CDialog
{
	DECLARE_DYNCREATE(CDirectLinePage)

	// Construction
	public:
		CDirectLinePage();
		~CDirectLinePage();

		void DisplayButtons(void);
        void SetupHotButtons(void);
		void onUpdateRights();
        void setRadioName(const std::string& radioName) { m_radioName = radioName; }
		void Init();

		// Dialog Data
		//{{AFX_DATA(CDirectLinePage)
	enum { IDD = IDD_FACILITYLINESv2 };
	CButton	m_automaticCallCheck;
			CTabCtrl	m_TabControl;
	//}}AFX_DATA

	protected:
		// Overrides
		// ClassWizard generate virtual function overrides
		//{{AFX_VIRTUAL(CDirectLinePage)
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			afx_msg void OnCancel();
			afx_msg void OnOK();
		//}}AFX_VIRTUAL

		// Implementation

		// Generated message map functions
		//{{AFX_MSG(CDirectLinePage)
			virtual BOOL OnInitDialog();
            afx_msg void OnDestroy();	
			afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
		//}}AFX_MSG

		afx_msg void OnHotButtonPress(UINT ctrlID);
		afx_msg void OnDirectButtonPress(UINT ctrlID);

		void SetupTabControl(void);

	
		DECLARE_MESSAGE_MAP()

    private:

        std::string m_radioName;
};

#endif // __DIRECT_LINE_H__
