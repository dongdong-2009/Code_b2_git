/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/scada/Prototypes/OPC/TransActiveOPCServer/TransActiveOPCServerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// TransActiveOPCServerDlg.h : header file
//

#if !defined(AFX_TRANSACTIVEOPCSERVERDLG_H__28C3CB5A_9031_4C08_B3BD_431F85A596E8__INCLUDED_)
#define AFX_TRANSACTIVEOPCSERVERDLG_H__28C3CB5A_9031_4C08_B3BD_431F85A596E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ITransActiveDataSource;

/////////////////////////////////////////////////////////////////////////////
// CTransActiveOPCServerDlg dialog

class CTransActiveOPCServerDlg : public CDialog
{
// Construction
public:
	CTransActiveOPCServerDlg(CWnd* pParent = NULL);	// standard constructor

	void cleanUp();
	void writeNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError );
	void ReadNotificationHandler ( HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp );
	void UnknownTagNotificationHandler ( LPSTR path, LPSTR name );
	void RemoveTagNotificationHandler ( HANDLE handle, LPSTR path, LPSTR name );



// Dialog Data
	//{{AFX_DATA(CTransActiveOPCServerDlg)
	enum { IDD = IDD_TRANSACTIVEOPCSERVER_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransActiveOPCServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CTransActiveOPCServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

	// handle to the interface of the TransActiveDataSource sub system which does most of the
	// work behind the screen
	ITransActiveDataSource * m_TADS;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSACTIVEOPCSERVERDLG_H__28C3CB5A_9031_4C08_B3BD_431F85A596E8__INCLUDED_)
