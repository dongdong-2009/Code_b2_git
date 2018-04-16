// PDS ClientSimDlg.h : header file
//

#if !defined(AFX_PDSCLIENTSIMDLG_H__F713C8D6_B408_4B7C_BC8F_3F57B32688D2__INCLUDED_)
#define AFX_PDSCLIENTSIMDLG_H__F713C8D6_B408_4B7C_BC8F_3F57B32688D2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ParamReader.h"
#include "IParamReader.h"
#include "FrameHandler.h"
#include "UDPSocketManager.h"
#include "IPDSClientConnectionObserver.h"

#include "FrameRepeatDlg.h"
#include "ConfigDlg.h"

#define WM_UIUPDATE		WM_APP + 1000

#include <vector>

#include "INotifyFrameRepetition.h"
#include "IConfigServerObserver.h"
#include "DlgDataValidator.h"
/////////////////////////////////////////////////////////////////////////////
// CPDSClientSimDlg dialog

class CPDSClientSimDlg : public CDialog , TA_IRS_App::IPDSClientConnectionObserver,
										  INotifyFrameRepetition,
										  IConfigServerObserver
{
// Construction
public:
	CPDSClientSimDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPDSClientSimDlg();	// standard constructor
	void UIUpdate (std::vector <unsigned char> vctrData,long lFrameID);

// Dialog Data
	//{{AFX_DATA(CPDSClientSimDlg)
	enum { IDD = IDD_PDSCLIENTSIM_DIALOG };
	CListBox	m_lstSendEvent;
	CListBox	m_lstRecvData;
	CComboBox	m_cmboTrainID;
	CEdit	m_edtPORT;
	CEdit	m_edtIP;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPDSClientSimDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPDSClientSimDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBeginDownload();
	afx_msg void OnBtnrepeat();
	virtual void OnOK();
	afx_msg void OnSelchangeCMBOTrainID();
	afx_msg void OnDblclkLstSendevent();
	afx_msg void OnDblclkLstRecvData();
	afx_msg void OnBtnConf();

	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
	afx_msg LRESULT OnUIUpdate (WPARAM wParam,LPARAM lParam);	

	

private:

	TA_IRS_App::IParamReader * m_paramReader;
	std::vector <unsigned char> m_vctrControlFrame;
	TA_IRS_App::CFrameHandler	 m_frameHandler;
	TA_IRS_App::CUDPSocketManager * m_socketManager;
	DWORD						m_dwDlgThreadID;
	CFrameRepeatDlg				m_frameRepeatDlg;
	unsigned int				m_nTotalFrame;
	int							m_nCounterFrame;
	FILE *						m_fileHandler;
	static	long				m_lStatFileCounter;
	CString						m_strFileName;
	CDlgDataValidator			m_dataValidator;
	CString						m_strTrainID;
	CConfigDlg					m_configDlg;
	BOOL						m_bCheckSumOveride;

	BOOL                         m_windowsMessage;
	unsigned char                m_trainID;
public:
	virtual void notifyFrameRepeat (std::vector <long> vctrFrameRepeat);
	virtual void notifyCheckSumOverRide (const BOOL & bStatus);
	virtual void notifyServerSettingChange (std::string strIpAddress, std::string strServicePort);
	virtual void connectionEstablish (::sockaddr* clientAddress, std::vector<unsigned char> socketData);
	::sockaddr_in 		m_sockClientInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PDSCLIENTSIMDLG_H__F713C8D6_B408_4B7C_BC8F_3F57B32688D2__INCLUDED_)
