/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/DirectLineConfigurePage.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// DirectLineConfigurePage.h : header file
//

#ifndef DIRECT_LINE_CONFIGURE_PAGE_H
#define DIRECT_LINE_CONFIGURE_PAGE_H



/////////////////////////////////////////////////////////////////////////////
// CConfigurePage dialog

class CDirectLineConfigurePage : public CDialog
{
	DECLARE_DYNCREATE(CDirectLineConfigurePage)

// Construction
public:
	CDirectLineConfigurePage();
	~CDirectLineConfigurePage();

    // Allow refresh from external (comms update) refreshes
	void DisplayButtons();
	void SetupHotButtons();

	void onUpdateRights();

	void Init();

// Dialog Data
	//{{AFX_DATA(CDirectLineConfigurePage)
	enum { IDD = IDD_FACILITYCONFIG };
	CTabCtrl	m_TabControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDirectLineConfigurePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnCancel();
	afx_msg void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDirectLineConfigurePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSetButton();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnDirectButtonPress(UINT ctrlID);
	afx_msg void OnHotButtonPress(UINT ctrlID);

private:
	UINT m_buttonNumber; // The number of the currently selected button
						 // (in the range of 0 to 35).
	bool m_isHotButton;  // Idicates if selected button is hotline or normal
};


#endif // DIRECT_LINE_CONFIGURE_PAGE_H
