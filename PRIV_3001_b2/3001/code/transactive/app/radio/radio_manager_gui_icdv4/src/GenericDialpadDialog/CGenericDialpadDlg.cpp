/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/GenericDialpadDialog/CGenericDialpadDlg.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/GenericDialpadDialog/CGenericDialpadDlg.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <iomanip> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenericDialpadDlg dialog

CGenericDialpadDlg::CGenericDialpadDlg(CWnd* pParent, 
            int maxDialLength, int maxHistoryLength,
            bool usedByRadio)
: CDialog(CGenericDialpadDlg::IDD, pParent), 
  m_precentlyDialed(NULL),
  m_maxDialLength(maxDialLength),
  m_maxHistoryLength(maxHistoryLength),
  m_usedInRadio(usedByRadio)
{
    if (m_maxHistoryLength == 0)
    {
        m_maxHistoryLength = 10;
    }

    if (m_maxDialLength == 0)
    {
        m_maxDialLength = 10;
    }
    
	//{{AFX_DATA_INIT(CGenericDialpadDlg)
	//}}AFX_DATA_INIT
}


void CGenericDialpadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenericDialpadDlg)
	DDX_Control(pDX, IDC_DIALEDNUM_CB, m_recentlyDialedCB);
	DDX_CBString(pDX, IDC_DIALEDNUM_CB, m_userDialedNumber);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenericDialpadDlg, CDialog)
	//{{AFX_MSG_MAP(CGenericDialpadDlg)
	ON_CBN_EDITCHANGE(IDC_DIALEDNUM_CB, OnEditChangeNumber)
	ON_COMMAND_RANGE(IDC_KEYPAD_0,IDC_KEYPAD_PND,OnKeyButtonClicked)
	ON_BN_CLICKED(ID_GENERIC_DIALPAD_OK, OnGenericDialpadOk)
	ON_BN_CLICKED(IDC_KEYPAD_BACKSP, OnKeyPadBackSpace)
	//}}AFX_MSG_MAP


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenericDialpadDlg message handlers

int CGenericDialpadDlg::DoModal() 
{
	
	return CDialog::DoModal();
}

BOOL CGenericDialpadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// load recently dialed numbers to ComboBox
	if (m_precentlyDialed != NULL)
	{
		std::list<std::string>::const_iterator iter;

		for(iter = m_precentlyDialed->begin();iter !=m_precentlyDialed->end() ;iter++)
		{
			CString tmp = (*iter).c_str();
			m_recentlyDialedCB.AddString( tmp );
		}
	}
	
	m_recentlyDialedCB.SetFocus();

	//Haipeng Jiang added this func based on the code reviewing.
	if (this->m_usedInRadio)
	{
		GetDlgItem(IDC_KEYPAD_ST)->EnableWindow(FALSE);
		GetDlgItem(IDC_KEYPAD_PND)->EnableWindow(FALSE);
	}

	m_recentlyDialedCB.LimitText(m_maxDialLength); 
	
	return FALSE;  
}


void CGenericDialpadDlg::OnEditChangeNumber() 
{
	CString tmp; 
	m_recentlyDialedCB.GetWindowText(tmp);
    std::string currentText(tmp.GetBuffer(0));

    if (currentText.find_first_not_of("0123456789") != std::string::npos)
    {
	    DWORD dwSel =  m_recentlyDialedCB.GetEditSel() - 1;
    	UpdateData(FALSE);
		m_recentlyDialedCB.SetEditSel( LOWORD( dwSel ), LOWORD( dwSel ) );
	}
    else
    {
    	UpdateData(TRUE);
    }
}



void CGenericDialpadDlg::OnKeyButtonClicked(UINT ctrlID) 
{
	int pressedButton = ctrlID - IDC_KEYPAD_0;
	
	std::ostringstream dialedNum;

    dialedNum << std::setw(1) << pressedButton;

    m_userDialedNumber += dialedNum.str().c_str();

	m_recentlyDialedCB.SetWindowText(m_userDialedNumber);

    UpdateData(TRUE);
}


void CGenericDialpadDlg::OnOK() 
{
	//fixed TD14299, leave this function blanl to hide the "Enter"
}


CString CGenericDialpadDlg::userDialedNumber()
{
	return m_userDialedNumber;
}


void CGenericDialpadDlg::setRecentlyDialedList(std::list<std::string>* pNumberList)
{
	m_precentlyDialed = pNumberList;
}


void CGenericDialpadDlg::OnGenericDialpadOk() 
{
	if (! isValidInput()) return;

	if ( m_precentlyDialed != NULL )
	{
		std::string sDialedNum = m_userDialedNumber;
		
		//Haipeng  added the codes below to ensure the list does not contain duplicated string.
		std::list<std::string>::iterator it = m_precentlyDialed->begin();
		for (; it != m_precentlyDialed->end(); ++it)
		{
			if (sDialedNum.compare( (*it).c_str() ) == 0)
			{
				CDialog::OnOK();
				return;
			}
		}
		//Haipeng ended

		if (m_precentlyDialed->size() < m_maxHistoryLength)
		{
			m_precentlyDialed->push_front(sDialedNum);
		}
		else
		{	
			m_precentlyDialed->pop_back();
			m_precentlyDialed->push_front(sDialedNum);
		}
	}
	
	CDialog::OnOK();	
}

//fixed TD14299
bool CGenericDialpadDlg::isValidInput()
{
	UpdateData();
	
	CString strInput;
	m_recentlyDialedCB.GetWindowText(strInput);

	if (strInput.IsEmpty())
	{
		::MessageBox(this->m_hWnd, "Please select or input the number that you want to call.", "Transactive", MB_OK|MB_ICONINFORMATION);
		return false;
	}
	return true;
}

void CGenericDialpadDlg::OnKeyPadBackSpace()
{
	m_userDialedNumber = m_userDialedNumber.Left(m_userDialedNumber.GetLength()-1);
	m_recentlyDialedCB.SetWindowText(m_userDialedNumber);
	UpdateData(TRUE);
}
