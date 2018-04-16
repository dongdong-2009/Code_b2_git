/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/stub/src/ConnectionPointTestDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ConnectionPointTestDlg.cpp : implementation file
//

#pragma warning(disable:4786)

#include "stdafx.h"
#include "ConnectionPointTest.h"
#include "ConnectionPointTestDlg.h"

#include <iostream>
#include <string>

#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/SoundPlayerException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestDlg dialog

ConnectionPointTestDlg::ConnectionPointTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ConnectionPointTestDlg::IDD, pParent),
	m_soundManager(this)
{
	//{{AFX_DATA_INIT(ConnectionPointTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ConnectionPointTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ConnectionPointTestDlg)
	DDX_Control(pDX, IDC_LIST1, m_messages);
	DDX_Control(pDX, IDC_SPIN1, m_spinControl);
	DDX_Control(pDX, IDC_PRIORITY, m_priority);
	DDX_Control(pDX, IDC_CATEGORY, m_category);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ConnectionPointTestDlg, CDialog)
	//{{AFX_MSG_MAP(ConnectionPointTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SETSEVERITY, onSetSeverity)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_BN_CLICKED(IDC_SILENCE, OnSilence)
	ON_BN_CLICKED(IDC_UNSILENCE, OnUnsilence)
	ON_BN_CLICKED(IDC_UNSILENCE_ALL, OnUnsilenceAll)
	ON_BN_CLICKED(IDC_SILENCE_ALL, OnSilenceAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ConnectionPointTestDlg message handlers

BOOL ConnectionPointTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CoInitialize(NULL);
	CoUninitialize();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	m_priority.SetWindowText("0");
	m_spinControl.SetRange(0, 5);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void ConnectionPointTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR ConnectionPointTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void ConnectionPointTestDlg::onSetSeverity() 
{
	// Get the category
	CString category;
	m_category.GetWindowText(category);

	// Get the priority
	CString priority;
	m_priority.GetWindowText(priority);

	try
	{
		m_soundManager.setPriority(category.GetBuffer(0), atoi(priority.GetBuffer(0)));
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Error setting priority: ";
		error += ex.what();
		AfxMessageBox(error.c_str());
	}
}

// Event handlers
void ConnectionPointTestDlg::handleCategorySilenced(const std::string& category, bool isSilenced)
{
	std::ostringstream msg;
	msg << category << " ";
	if (isSilenced)
	{
		msg << "silenced";
	}
	else
	{
		msg << "unsilenced";
	}

	m_messages.AddString(msg.str().c_str());
}

void ConnectionPointTestDlg::handleAllSilenced(bool isSilenced)
{
	std::ostringstream msg;
	msg << "All categories ";
	if (isSilenced)
	{
		msg << "silenced";
	}
	else
	{
		msg << "unsilenced";
	}

	m_messages.AddString(msg.str().c_str());
}

void ConnectionPointTestDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int priority = pNMUpDown->iPos + pNMUpDown->iDelta;

	std::ostringstream priorityStream;
	priorityStream << priority;

	m_priority.SetWindowText(priorityStream.str().c_str());
	*pResult = 0;
}

void ConnectionPointTestDlg::OnSilence() 
{
	// Get the category
	CString category;
	m_category.GetWindowText(category);

	try
	{
		m_soundManager.setSilence(category.GetBuffer(0), true);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Error silencing category: ";
		error += ex.what();
		AfxMessageBox(error.c_str());
	}
}

void ConnectionPointTestDlg::OnUnsilence() 
{
	// Get the category
	CString category;
	m_category.GetWindowText(category);

	try
	{
		m_soundManager.setSilence(category.GetBuffer(0), false);
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Error unsilencing category: ";
		error += ex.what();
		AfxMessageBox(error.c_str());
	}
}

void ConnectionPointTestDlg::OnSilenceAll() 
{
	try
	{
		m_soundManager.setSilenceAll(true);	
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Error silencing all categories: ";
		error += ex.what();
		AfxMessageBox(error.c_str());
	}
}

void ConnectionPointTestDlg::OnUnsilenceAll() 
{
	try
	{
		m_soundManager.setSilenceAll(false);		
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Error unsilencing all categories: ";
		error += ex.what();
		AfxMessageBox(error.c_str());
	}
}
