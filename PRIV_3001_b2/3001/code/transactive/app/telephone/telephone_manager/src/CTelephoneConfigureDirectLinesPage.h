/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/CTelephoneConfigureDirectLinesPage.h $
  * @author:  Wong Soh Sum
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * User will be able configure the button settings on the direct line
  * config page. This class will write any user-set information to the 
  * telephoneDirectLineLibrary, to be subsequently written into the database
  * It handles the "Configure Direct Line" Property Page.
  */

#if !defined(AFX_TELEPHONECONFIGUREDIRECTLINESPAGE_H__52C42A39_0651_4541_A90E_44640FB9BAE8__INCLUDED_)
#define AFX_TELEPHONECONFIGUREDIRECTLINESPAGE_H__52C42A39_0651_4541_A90E_44640FB9BAE8__INCLUDED_

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <map>
#include <vector>

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "bus/telephone/telephone_direct_lines_library/src/TelephoneSpeedDialButtonDetails.h"
#include "core/threads/src/Thread.h"

class ItaTelephoneManagerSummary;
class CTelephoneManagerDlg;

class CTelephoneConfigureDirectLinesPageInitialThread;

class CTelephoneConfigureDirectLinesPage : public CPropertyPage
{
    friend class CTelephoneConfigureDirectLinesPageInitialThread;

    DECLARE_DYNCREATE(CTelephoneConfigureDirectLinesPage)

// Construction
public:
    CTelephoneConfigureDirectLinesPage();
    ~CTelephoneConfigureDirectLinesPage();


// Dialog Data
    // {{AFX_DATA(CTelephoneConfigureDirectLinesPage)
    enum { IDD = IDD_FACILITY_CONFIG_LINES };
    CTabCtrl    m_TabControl;
    CString    m_Label;
    CString    m_Number;
    // }}AFX_DATA

// Overrides
    // ClassWizard generate virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneConfigureDirectLinesPage)
    public:
    virtual BOOL OnSetActive();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

public:
    /**
      * setItaSummary
      *
      * Assigne iSummary to m_Summary
      *
      * @return void
      * @param : ItaTelephoneManagerSummary* iSummary
      *
      * @exception <exceptions> Optional
      */
    void setItaSummary(ItaTelephoneManagerSummary* iSummary);

        /**
      * DisplayButtons
      *
      * This method will setup the buttons for the selected tab page. Note:  It assumes
      * that the buttons on the dialog are sequentially numbered starting with
      * IDC_DIRECT_LINE_BUTTON_1
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void DisplayButtons();

protected:
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
      * SetupHotButtons
      *
      * This method will setup the hot buttons. Note:  It assumes that the buttons on
      * the dialog are sequentially numbered starting with IDC_HOT_BUTTON1
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void SetupHotButtons();

protected:
    // Generated message map functions
    // {{AFX_MSG(CTelephoneConfigureDirectLinesPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSetButton();
    // }}AFX_MSG

    /**
      * LRESULT OnSpeedDialButtonModified
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
      * @return virtual afx_msg
      * @param : WPARAM pdetails
      *          This contains a pointer to a TelephoneSpeedDialButtonDetails
      *          object. It should be cast to this before being used.
      * @param : LPARAM unused
      *          This parameter is not used by this method.
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg LRESULT OnSpeedDialButtonModified(WPARAM pdetails, LPARAM unused);

    /**
      * void OnHotButtonPress
      *
      * This method is called when a hot button is pressed.  It will identify which hot
      * button was activated and get the number associated with it.  The details will
      * then be displayed in the SelectedArea.
      *
      * @return virtual afx_msg
      * @param : UINT ctrlID
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg void OnHotButtonPress(UINT ctrlID);

    /**
      * void OnDirectButtonPress
      *
      * This method is called when a direct button is pressed.  It will identify which
      * hot button was activated and then display that buttons details.
      *
      * @return virtual afx_msg
      * @param : UINT ctrlID
      *
      * @exception <exceptions> Optional
      */
    virtual afx_msg void OnDirectButtonPress(UINT ctrlID);


    DECLARE_MESSAGE_MAP()

private:

    /***********************************
     * Helper functions
    /***********************************
    /**
      * updateLabelButtons
      *
      * Function will update the button map as well as label all the buttons
      *
      * @return void
      * @param :  unsigned int resourceBaseIndex
      * @param : std::map<int
      * @param : TelephoneDirectoryEntryData>&   iButtonMap
      * @param : std::vector<TelephoneSpeedDialButtonDetails>& iBtnSet
      *
      * @exception <exceptions> Optional
      */
    void updateLabelButtons( unsigned int resourceBaseIndex,
                                std::map<int, TelephoneDirectoryEntryData>&   iButtonMap,
                                std::vector<TelephoneSpeedDialButtonDetails>& iBtnSet);

    /**
      * clearTabBtns
      *
      * Function will clear all the buttons on the tabs
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void clearTabBtns();

    /**
      * clearHotlinesBtns
      *
      * Function will clear all the Hotline buttons
      *
      * @return void
      *
      * @exception <exceptions> Optional
      */
    void clearHotlinesBtns();

    /**
      * The number of the currently selected button (in the range of 0 to 35).
      */
    UINT m_buttonNumber;

    /**
      * Idicates if selected button is hotline or normal
      */
    bool m_isHotButton;

    /**
      * Each map stores the respective button resource index and TelephoneDirectoryEntryData
      */
    std::map<int, TelephoneDirectoryEntryData> m_Hotlines;
    std::map<int, TelephoneDirectoryEntryData> m_userEntries;
    std::map<int, TelephoneDirectoryEntryData> m_stationsEntries;
    std::map<int, TelephoneDirectoryEntryData> m_OCREntries;
    std::map<int, TelephoneDirectoryEntryData> m_DCREntries;
    std::map<int, TelephoneDirectoryEntryData> m_otherEntries;
    std::map<int, TelephoneDirectoryEntryData> m_PCPEntries;

    /**
      * Holds a reference to the CtaTelephoneManagerSummary.
      * Used by search page to retrieve session ID
      */
    ItaTelephoneManagerSummary* m_Summary;

    static const unsigned short MAX_HOTLINE_BUTTONS;
    static const unsigned short MAX_TAB_BUTTONS;

    static const unsigned int TAB_BUTTONS_BASE_RESOURCE_ID;
    static const unsigned int HOTLINE_BUTTONS_BASE_RESOURCE_ID;

    static const std::string SETNAME_HOTLINES;
    static const std::string SETNAME_USER;
    static const std::string SETNAME_STATIONS;
    static const std::string SETNAME_OCR;
    static const std::string SETNAME_DCR;
    static const std::string SETNAME_OTHERS;
    static const std::string SETNAME_PCP;

    CTelephoneConfigureDirectLinesPageInitialThread* m_initialThread;
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TELEPHONECONFIGUREDIRECTLINESPAGE_H__52C42A39_0651_4541_A90E_44640FB9BAE8__INCLUDED_)
