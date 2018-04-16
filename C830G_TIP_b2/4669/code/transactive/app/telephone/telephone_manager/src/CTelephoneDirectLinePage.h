/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneDirectLinePage.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  */

#if !defined(CTelephoneDirectLinePage_5E6C9B90_C37F_4439_8B06_2D6F9475A9F7__INCLUDED_)
#define CTelephoneDirectLinePage_5E6C9B90_C37F_4439_8B06_2D6F9475A9F7__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <vector>

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.h"

class ItaTelephoneManagerSummary;
class CTelephoneManagerDlg;
class CTelephoneDirectLinePageInitialThread;

class CTelephoneDirectLinePage : public CPropertyPage
{
    DECLARE_DYNCREATE(CTelephoneDirectLinePage)
    
    friend class CTelephoneDirectLinePageInitialThread;

// Construction
public:
    CTelephoneDirectLinePage();
    virtual ~CTelephoneDirectLinePage();

    /**
      * setItaSummary
      *
      * Assigne pTransactiveInterfaces to m_pTransactiveInterfaces
      *
      * @return void
      * @param : ItaTelephoneManagerSummary* iSummary
      *
      * @exception <exceptions> Optional
      */
    void setItaSummary(ItaTelephoneManagerSummary *ptransactiveInterfaces);

    /**
      * DisplayButtons
      *
      * This method will setup the buttons for the selected tab page Note:  It assumes
      * that the buttons on the dialog are sequentially numbered starting with
      * IDC_DIRECT_LINE_BUTTON_1
      *
      * @return void
      */
    void DisplayButtons();

// Dialog Data
    // {{AFX_DATA(CTelephoneDirectLinePage)
    enum { IDD = IDD_DIRECT_LINE };
    CTabCtrl    m_TabControl;
    // }}AFX_DATA


// Overrides
    // ClassWizard generate virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneDirectLinePage)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    // {{AFX_MSG(CTelephoneDirectLinePage)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()

    /**
      * UpdateRights
      *
      * This method will allow/disallow access to features based on the operator's
      * profile. Do any enabling/disabling of GUI components in here. This method will
      * utilise the isFunctionPermitted method of the CTelephoneManagerDlg to determine
      * the appropriate rights.
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void UpdateRights( CTelephoneManagerDlg* pMainWnd );

    /**
      * OnSpeedDialButtonModified
      *
      * This will modify the TelephoneDirectoryEntryData item associated with this
      * button. The associated TelephoneDirectoryEntryData will be stored in the
      * applicable std::map that matches the "SetName" attribute of the
      * TelephoneSpeedDialButtonDetails parameter, and the
      * TelephoneDirectoryEntryData's "number" attribute will match the
      * TelephoneSpeedDialButtonDetails' "telephoneNumber" attribute. It will also
      * update the applicable button's label on the display. This method is invoked by
      * the message loop on receipt of a WM_DIRECTLINES_MODIFIED message.
      *
      * @return virtual afx_msg LRESULT
      * @param : WPARAM pdetails
      *          This contains a pointer to a TelephoneSpeedDialButtonDetail
      *          object. It should be cast to this before being used.
      * @param : LPARAM unused
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnSpeedDialButtonModified(WPARAM pdetails, LPARAM unused);

    /**
      * OnHotButtonPress
      *
      * This method is called when a hot button is pressed.  It will identify which hot
      * button was activated and get the TSI number associated with it.  The details
      * will then be displayed in the SelectedArea. This message handler must be mapped
      * to a contiguous range of command IDs e.g. ON_COMMAND_RANGE(IDC_HOT_BUTTON1,
      * IDC_HOT_BUTTON8,OnHotButtonPress).
      *
      * @return virtual afx_msg
      * @param : UINT ctrlID
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg void OnHotButtonPress(UINT ctrlID);

    /**
      * SetupTabControl
      *
      * Load the headings for the tab control
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void SetupTabControl();

    /**
      * SetupHotButtons
      *
      * This method will setup the hot buttons Note:  It assumes that the buttons on
      * the dialog are sequentially numbered starting with IDC_HOT_BUTTON1
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void SetupHotButtons();

    /**
      * OnDirectButtonPress
      *
      * This method is called when a direct line button is pressed.  It will identify
      * which direct line button was activated and get the TSI number associated with
      * it.  The details will then be displayed in the SelectedArea. This message
      * handler must be mapped to a contiguous range of command IDs e.g.
      * ON_COMMAND_RANGE(IDC_DIRECT_LINE_BUTTON_1,IDC_DIRECT_LINE_BUTTON_36,
      * OnDirectButtonPress)
      *
      * @return virtual afx_msg
      * @param : UINT ctrlID
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg void OnDirectButtonPress(UINT ctrlID);

private:
    std::map<int, TelephoneDirectoryEntryData> m_Hotlines;
    std::map<int, TelephoneDirectoryEntryData> m_userEntries;
    std::map<int, TelephoneDirectoryEntryData> m_stationsEntries;
    std::map<int, TelephoneDirectoryEntryData> m_OCREntries;
    std::map<int, TelephoneDirectoryEntryData> m_DCREntries;
    std::map<int, TelephoneDirectoryEntryData> m_otherEntries;
    std::map<int, TelephoneDirectoryEntryData> m_PCPEntries;

    ItaTelephoneManagerSummary* m_pTransactiveInterfaces;

    CTelephoneDirectLinePageInitialThread* m_initialThread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(CTelephoneDirectLinePage_5E6C9B90_C37F_4439_8B06_2D6F9475A9F7__INCLUDED_)
