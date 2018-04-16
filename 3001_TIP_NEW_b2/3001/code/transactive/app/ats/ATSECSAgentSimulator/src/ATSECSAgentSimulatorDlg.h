/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulatorDlg.h $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/15 18:11:22 $
  * Last modified by:  $Author: qi.huang $
  *
  */

#if !defined(AFX_ATSECSAGENTSIMULATORDLG_H__C9EDBD85_98E0_4679_AA54_04004B678195__INCLUDED_)
#define AFX_ATSECSAGENTSIMULATORDLG_H__C9EDBD85_98E0_4679_AA54_04004B678195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "bus/generic_gui/src/TransActiveDialog.h"


class RandomGenerator;

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_IRS_Core
{
    class IECSZone;
}


// ATSECSAgentSimulatorDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ATSECSAgentSimulatorDlg dialog

class ATSECSAgentSimulatorDlg : public TA_Base_Bus::TransActiveDialog
{
// Construction
public:
	ATSECSAgentSimulatorDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL);   // standard constructor

    virtual ~ATSECSAgentSimulatorDlg();

    void setState(unsigned long ECSZoneId, 
                  unsigned long maxExternalTemperatureInCelsius, 
                  unsigned long stallTimeInSeconds,
                  bool          isInEmergency);


// Dialog Data
	//{{AFX_DATA(ATSECSAgentSimulatorDlg)
	enum { IDD = IDD_ATSECSAGENTSIMULATOR_DIALOG };
	CButton	m_resetBtn;
	CListCtrl	m_ECSZoneLst;
	CEdit	m_scheduleNumberTxt;
	CEdit	m_serviceNumberTxt;
	CEdit	m_ATCTrainNumberTxt;
	CEdit	m_physicalTrainIdTxt;
	CEdit	m_temperatureTxt;
	CEdit	m_stallTimeTxt;
	CButton	m_randomiseBtn;
	CButton	m_executeBtn;
	CButton	m_emergencyChk;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ATSECSAgentSimulatorDlg)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    // Internal structure for passing data between threads
    struct ATSECSPacket
    {
        unsigned long ECSZoneId; 
        unsigned long maxExternalTemperatureInCelsius; 
        unsigned long stallTimeInSeconds;
        bool          isInEmergency;
    };

    void updateList(unsigned long ECSZoneId, 
                    unsigned long maxExternalTemperatureInCelsius, 
                    unsigned long stallTimeInSeconds,
                    bool          isInEmergency);


    RandomGenerator*    m_randomGenerator;
    HWND                m_viewHandle;
    std::vector<TA_IRS_Core::IECSZone*>    m_zoneIds;

	// Generated message map functions
	//{{AFX_MSG(ATSECSAgentSimulatorDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRandomiseBtn();
	virtual void OnCancel();
    afx_msg LRESULT OnUpdateList(UINT wParam, LONG lParam);
	afx_msg void OnExecuteBtn();
	afx_msg void OnResetBtn();
	afx_msg void OnItemchangedEcsZoneLst(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATSECSAGENTSIMULATORDLG_H__C9EDBD85_98E0_4679_AA54_04004B678195__INCLUDED_)
