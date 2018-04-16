// TagDlg.cpp : implementation file
//

#include "stdafx.h"
#include "opcda.h"
#include "opc_ae.h"
#include "opcaedef.h"
#include "OPCERROR.h"
#include "WTOPCsvrAPI.h"
#include "WTOPCsvrextAPI.h"
#include "WTSvrTest.h"
#include "TagDlg.h"
#include "aelimsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTagDlg dialog


CTagDlg::CTagDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTagDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTagDlg)
	m_name = _T("");
	m_description = _T("");
	m_units = _T("");
	m_doArray = FALSE;
	m_arraylength = 0;
	//}}AFX_DATA_INIT
	VariantInit (&m_value);
}


void CTagDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTagDlg)
	DDX_Text(pDX, IDC_NAME, m_name);
	DDX_Text(pDX, IDC_DESCR, m_description);
	DDX_Text(pDX, IDC_UNITS, m_units);
	DDX_Check(pDX, IDC_ARRAY, m_doArray);
	DDX_Text(pDX, IDC_LENGTH, m_arraylength);
	DDV_MinMaxInt(pDX, m_arraylength, 2, 512);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTagDlg, CDialog)
	//{{AFX_MSG_MAP(CTagDlg)
	ON_BN_CLICKED(IDC_ALARMS, OnAlarms)
	ON_BN_CLICKED(IDC_BOOLEAN, OnBoolean)
	ON_BN_CLICKED(IDC_FLOAT, OnFloat)
	ON_BN_CLICKED(IDC_INTEGER, OnInteger)
	ON_BN_CLICKED(IDC_STRING, OnString)
	ON_BN_CLICKED(IDC_ARRAY, OnArray)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTagDlg message handlers

void CTagDlg::OnAlarms() 
{
	CAELimsDlg	dlg;

	dlg.m_lolo_enable = m_enabled[0];
	dlg.m_lo_enable = m_enabled[1];
	dlg.m_hi_enable = m_enabled[2];
	dlg.m_hihi_enable = m_enabled[3];

	dlg.m_lolo_level = m_alarms[0];
	dlg.m_lo_level = m_alarms[1];
	dlg.m_hi_level = m_alarms[2];
	dlg.m_hihi_level = m_alarms[3];

	dlg.m_lolo_severity = m_severity[0];
	dlg.m_lo_severity = m_severity[1];
	dlg.m_hi_severity = m_severity[2];
	dlg.m_hihi_severity = m_severity[3];

	if (dlg.DoModal() == IDOK)
		{
		m_enabled[0] = dlg.m_lolo_enable;
		m_enabled[1] = dlg.m_lo_enable; 
		m_enabled[2] = dlg.m_hi_enable;
		m_enabled[3] = dlg.m_hihi_enable;

		m_alarms[0] = dlg.m_lolo_level;
		m_alarms[1] = dlg.m_lo_level;
		m_alarms[2] = dlg.m_hi_level;
		m_alarms[3] = dlg.m_hihi_level;

		m_severity[0] = dlg.m_lolo_severity;
		m_severity[1] = dlg.m_lo_severity;
		m_severity[2] = dlg.m_hi_severity;
		m_severity[3] = dlg.m_hihi_severity;
		}
}

BOOL CTagDlg::OnInitDialog() 
{
	CButton	*pRadio;
	CEdit *pEdit;
	char	buf[100];
	CWTSvrTestApp	*pApp;
	CComboBox	*pBox;


	pApp = (CWTSvrTestApp *)AfxGetApp();

	CDialog::OnInitDialog();

	pArrayLen = (CButton *)GetDlgItem (IDC_LENGTH);
	pLenTxt = (CStatic *)GetDlgItem(IDC_LENTEXT);
	pArrayLen->EnableWindow(m_doArray);
	pLenTxt->EnableWindow(m_doArray);

	pBox = (CComboBox *)GetDlgItem (IDC_READSECURITY);
	switch (m_ReadAccess)
		{
		case ACCESSLEVEL1:
			pBox->SetCurSel(1);
			break;
		case ACCESSLEVEL2:
			pBox->SetCurSel(2);
			break;
		case ACCESSLEVEL3:
			pBox->SetCurSel(3);
			break;
		case ACCESSLEVEL4:
			pBox->SetCurSel(4);
			break;
		case ACCESSLEVEL5:
			pBox->SetCurSel(5);
			break;
		case ACCESSLEVEL6:
			pBox->SetCurSel(6);
			break;
		case ACCESSLEVEL7:
			pBox->SetCurSel(7);
			break;
		case ACCESSLEVEL8:
			pBox->SetCurSel(8);
			break;
		case ACCESSLEVEL9:
			pBox->SetCurSel(9);
			break;
		case ACCESSLEVEL10:
			pBox->SetCurSel(10);
			break;
		case ACCESSLEVEL11:
			pBox->SetCurSel(11);
			break;
		case ACCESSLEVEL12:
			pBox->SetCurSel(12);
			break;
		case ACCESSLEVEL13:
			pBox->SetCurSel(13);
			break;
		case ACCESSLEVEL14:
			pBox->SetCurSel(14);
			break;
		case ACCESSLEVEL15:
			pBox->SetCurSel(15);
			break;
		case ACCESSLEVEL16:
			pBox->SetCurSel(16);
			break;
		case ACCESSLEVELNONE:
		default:
			pBox->SetCurSel(0);
			break;
		}
	pBox = (CComboBox *)GetDlgItem (IDC_WRITESECURITY);
	switch (m_WriteAccess)
		{
		case ACCESSLEVEL1:
			pBox->SetCurSel(1);
			break;
		case ACCESSLEVEL2:
			pBox->SetCurSel(2);
			break;
		case ACCESSLEVEL3:
			pBox->SetCurSel(3);
			break;
		case ACCESSLEVEL4:
			pBox->SetCurSel(4);
			break;
		case ACCESSLEVEL5:
			pBox->SetCurSel(5);
			break;
		case ACCESSLEVEL6:
			pBox->SetCurSel(6);
			break;
		case ACCESSLEVEL7:
			pBox->SetCurSel(7);
			break;
		case ACCESSLEVEL8:
			pBox->SetCurSel(8);
			break;
		case ACCESSLEVEL9:
			pBox->SetCurSel(9);
			break;
		case ACCESSLEVEL10:
			pBox->SetCurSel(10);
			break;
		case ACCESSLEVEL11:
			pBox->SetCurSel(11);
			break;
		case ACCESSLEVEL12:
			pBox->SetCurSel(12);
			break;
		case ACCESSLEVEL13:
			pBox->SetCurSel(13);
			break;
		case ACCESSLEVEL14:
			pBox->SetCurSel(14);
			break;
		case ACCESSLEVEL15:
			pBox->SetCurSel(15);
			break;
		case ACCESSLEVEL16:
			pBox->SetCurSel(16);
			break;
		case ACCESSLEVELNONE:
		default:
			pBox->SetCurSel(0);
			break;
		}

	
	switch (m_value.vt)
		{
		case VT_I2:	pRadio = (CButton *)GetDlgItem(IDC_INTEGER);
				break;
		case VT_BOOL: pRadio = (CButton *)GetDlgItem(IDC_BOOLEAN);
				break;
		case VT_BSTR: pRadio = (CButton *)GetDlgItem(IDC_STRING);
				break;
		default: pRadio = (CButton *)GetDlgItem(IDC_FLOAT);
				break;
		}


	pRadio->SetCheck(1);
	pEdit = (CEdit *)GetDlgItem (IDC_VALUE);
	pApp->FormatVariant (&m_value, buf, 100);
	pEdit->SetWindowText(buf);

	pRadio = (CButton *)GetDlgItem (IDC_100);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTagDlg::OnBoolean() 
{
	m_value.vt = VT_BOOL;	
}

void CTagDlg::OnFloat() 
{
	m_value.vt = VT_R4;	
}

void CTagDlg::OnInteger() 
{
	m_value.vt = VT_I2;	
}

void CTagDlg::OnString() 
{
	m_value.vt = VT_BSTR;	
}

void CTagDlg::OnOK() 
{
	char	buf[100];
	CEdit	*pEdit;
	int		tempi=0;
	float	tempf=0.0;
	WCHAR	*pWCHAR;
	CWTSvrTestApp	*pApp;
	CComboBox	*pBox;

	pApp = (CWTSvrTestApp *)AfxGetApp();
	pBox = (CComboBox *)GetDlgItem (IDC_READSECURITY);
	switch (pBox->GetCurSel())
		{
		case 1:
			m_ReadAccess = ACCESSLEVEL1;
			break;
		case 2:
			m_ReadAccess = ACCESSLEVEL2;
			break;
		case 3:
			m_ReadAccess = ACCESSLEVEL3;
			break;
		case 4:
			m_ReadAccess = ACCESSLEVEL4;
			break;
		case 5:
			m_ReadAccess = ACCESSLEVEL5;
			break;
		case 6:
			m_ReadAccess = ACCESSLEVEL6;
			break;
		case 7:
			m_ReadAccess = ACCESSLEVEL7;
			break;
		case 8:
			m_ReadAccess = ACCESSLEVEL8;
			break;
		case 9:
			m_ReadAccess = ACCESSLEVEL9;
			break;
		case 10:
			m_ReadAccess = ACCESSLEVEL10;
			break;
		case 11:
			m_ReadAccess = ACCESSLEVEL11;
			break;
		case 12:
			m_ReadAccess = ACCESSLEVEL12;
			break;
		case 13:
			m_ReadAccess = ACCESSLEVEL13;
			break;
		case 14:
			m_ReadAccess = ACCESSLEVEL14;
			break;
		case 15:
			m_ReadAccess = ACCESSLEVEL15;
			break;
		case 16:
			m_ReadAccess = ACCESSLEVEL16;
			break;
		default:
			m_ReadAccess = ACCESSLEVELNONE;
			break;
		}

	pBox = (CComboBox *)GetDlgItem (IDC_WRITESECURITY);
	switch (pBox->GetCurSel())
		{
		case 1:
			m_WriteAccess = ACCESSLEVEL1;
			break;
		case 2:
			m_WriteAccess = ACCESSLEVEL2;
			break;
		case 3:
			m_WriteAccess = ACCESSLEVEL3;
			break;
		case 4:
			m_WriteAccess = ACCESSLEVEL4;
			break;
		case 5:
			m_WriteAccess = ACCESSLEVEL5;
			break;
		case 6:
			m_WriteAccess = ACCESSLEVEL6;
			break;
		case 7:
			m_WriteAccess = ACCESSLEVEL7;
			break;
		case 8:
			m_WriteAccess = ACCESSLEVEL8;
			break;
		case 9:
			m_WriteAccess = ACCESSLEVEL9;
			break;
		case 10:
			m_WriteAccess = ACCESSLEVEL10;
			break;
		case 11:
			m_WriteAccess = ACCESSLEVEL11;
			break;
		case 12:
			m_WriteAccess = ACCESSLEVEL12;
			break;
		case 13:
			m_WriteAccess = ACCESSLEVEL13;
			break;
		case 14:
			m_WriteAccess = ACCESSLEVEL14;
			break;
		case 15:
			m_WriteAccess = ACCESSLEVEL15;
			break;
		case 16:
			m_WriteAccess = ACCESSLEVEL16;
			break;
		default:
			m_WriteAccess = ACCESSLEVELNONE;
			break;
		}

	pEdit = (CEdit *)GetDlgItem(IDC_VALUE);
	pEdit->GetWindowText (buf, 100);
	switch (m_value.vt)
		{
		case VT_I2:	sscanf (buf, "%d", &tempi);
				m_value.intVal = tempi;
				break;
		case VT_BOOL: if (_stricmp (buf, "true") == 0)
						m_value.boolVal = TRUE;
					else
						{
						sscanf (buf, "%d", &tempi);
						if (tempi > 0)
							m_value.boolVal = TRUE;
						else
							m_value.boolVal = FALSE;
						}
				break;
		case VT_BSTR: 	pWCHAR = pApp->WSTRFromCString (buf);
					m_value.bstrVal = SysAllocString (pWCHAR);
					pApp->WSTRFree (pWCHAR);
				break;
		default: sscanf (buf, "%f", &tempf);
				m_value.fltVal = tempf;
				break;
		}

	CDialog::OnOK();
}


void CTagDlg::OnArray() 
{
	m_doArray = !m_doArray;
	pArrayLen->EnableWindow(m_doArray);
	pLenTxt->EnableWindow(m_doArray);

}
