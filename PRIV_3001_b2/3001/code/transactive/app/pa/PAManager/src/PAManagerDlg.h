/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/pa/PAManager/src/PAManagerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_)
#define AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PAManagerDlg.h : header file
//

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "bus/pa_4669/pa_agent/IDL/src/IPAAgentCorbaDef.h"
#include "app/pa/PAManager/src/IRunParamChangedListener.h"
#include "app/pa/PAManager/src/IRestartApplicationCallback.h"
#include "app/pa/PAManager/src/PaTabCtrl.h"

class PaManagerModel;
class DvaVersionsPage;
class StationBroadcastPage;
class RightsChangedWindowNotificationHandler;


/////////////////////////////////////////////////////////////////////////////
// PAManagerDlg dialog

class PAManagerDlg 
: 
public TA_Base_Bus::TransActiveDialog, public IRunParamChangedListener, public IRestartApplicationCallback
{
// Construction
public:
	//PAManagerDlg(CWnd* pParent = NULL);   // standard constructor
	PAManagerDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
    
    virtual ~PAManagerDlg();

	void init();
    /**
     * enableExecuteButton
     *
     * Updates the state of the execute button, only active pages should enable it
     *
     * @param isEnable the new state of the execute button
     *
     */
	void enableExecuteButton(bool isEnabled);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PAManagerDlg)
	public:
		virtual BOOL DestroyWindow();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	//}}AFX_VIRTUAL

// Implementation

public:
    /**
     * processRunParamChange (interface implementation)
     *
     * @see IRunParamChangedListener::processRunParamChange for more details
     *
     * @pre the input identifier (name) is a recognized command
     *
     */
    virtual void processRunParamChange(const std::string& name, const std::string& value);

    /**
     * restartApplication (interface implementation)
     *
     * @see IRestartApplicationCallback::restartApplication for more details
     *
     */
    virtual void restartApplication(int leftPos);

    /**
     * @return true if the application is attempting to restart itself
     *
     */
    static bool isApplicationRestartInProgress();

    /**
     * @return the left pixel location defining the startup postiion for the PA Manager when
     *          it is restarted
     *
     * @pre isApplicationRestartInProgress is true
     *
     */
    static int getAppplicationRestartTargetPosition();

protected:
	// Generated message map functions
	//{{AFX_MSG(PAManagerDlg)
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnCloseBtn();
	virtual afx_msg void OnExecuteBtn();
    virtual afx_msg void OnHelpBtn();
  	virtual afx_msg LRESULT OnStatusNameChange(UINT wParam, LONG lParam);    
    virtual afx_msg LRESULT OnDisplayModeChange(UINT wParam, LONG lParam);    
    virtual afx_msg LRESULT OnErrorMessage(UINT wParam, LONG lParam);
	virtual afx_msg void OnSelchangeStatusCb();
	virtual afx_msg void OnRestoreStatusBtn();
	virtual afx_msg void OnKillStatusBtn();
	virtual afx_msg void OnKillAllStatusBtn();
	virtual afx_msg void OnDestroy();
	virtual afx_msg void OnAboutbutton();
    virtual afx_msg LRESULT OnRightsChanged(UINT wParam, LONG lParam);
    virtual afx_msg LRESULT OnAttemptRestart(UINT wParam, LONG lParam);
    virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);
    virtual afx_msg LRESULT OnActivateSchematic(UINT wParam, LONG lParam); 
    virtual afx_msg void OnTabSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	virtual afx_msg void OnKillAllGoingDVA();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    /**
     * Intercept all messages to allow provision of top level exception handler
     *
     */
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
    /**
     * terminateAndRemoveBroadcastInCombo
     *
     * terminates, then removes, the broadcast ID specified in the local status name
     * combo box 
     *
     * @param nItem the index into the combo box defining the broadcast Id to remove
     *
     */

   void terminateAndRemoveBroadcastInCombo(int nItem);

    /**
     * updateComboBoxOnNameChange
     *
     * Updates the combo box after a name change notification received
     *
     * @param id the identifier of the name change that took place
     *
     * @return true if the name change was propogated successfully to the combo box
     *
     */
    bool updateComboBoxOnNameChange(UINT id);

    /**
     * refreshStatusNameCombo
     *
     * Refreshes contents of the status name combo - updated dynamically as 
     * subscriber notifications received (giving updated information for this list)
     *
     * @param bInitialization true if we are to initial subscription connection in model
     *                          (called once off on startup)
     */
    void refreshStatusNameCombo(bool bInitialization);


    /**
     * Attempts to set the schematic into an active state
     *  If fails, application must be automatically closed (for safety reasons)
     * 
     */
    void activateSchematic();

    void initWindowPos();

    // Model for this dialog view
    PaManagerModel*  m_managerModel;
	PaTabCtrl m_mainTab;
	DvaVersionsPage* m_dvaVersionsPage;
	StationBroadcastPage* m_stationPage;

// Dialog Data
	//{{AFX_DATA(PAManagerDlg)
    enum { IDD = IDD_MAIN_DLG };
	CButton	m_aboutButton;
	CButton	m_helpButton;
	CButton	m_executeButton;
	CButton	m_closeButton;
	CButton	m_killAllStatusButton;
	CButton	m_killStatusButton;
	CButton	m_restoreStatusButton;
	CComboBox	m_statusNameList;
	CButton m_killAllGoingDVA;
	//}}AFX_DATA

    // Indicates whether the GUI is meant to be visible at the moment
	bool m_bShouldRespone;

    static TA_Base_Core::PrimitiveWrapper<int>  s_restartTargetPosition;

    RightsChangedWindowNotificationHandler* m_rightsChangeHandler;

    TA_Base_Core::PrimitiveWrapper<TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType> m_currentStatusNameBroadcastType;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_PAMANAGERDLG_H__D55DB830_1342_4D9E_BA4C_C554E25FC74B__INCLUDED_)

