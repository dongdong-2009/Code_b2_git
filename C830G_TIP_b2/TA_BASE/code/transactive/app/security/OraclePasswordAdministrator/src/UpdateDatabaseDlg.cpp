// UpdateDatabaseDlg.cpp : implementation file
//

#include "stdafx.h"
#include "OraclePasswordAdministrator.h"
#include "UpdateDatabaseDlg.h"
#include "OraclePasswordAdministratorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUpdateDatabaseDlg dialog
const static int PKEY_DB_COL = 0;
const static int NAME_DB_COL = 1;
const static int PASSWD_DB_COL = 2;

CUpdateDatabaseDlg::CUpdateDatabaseDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdateDatabaseDlg::IDD, pParent),m_dbConn(NULL),m_exeSql(NULL)
{
	COraclePasswordAdministratorDlg passwordAdminDlg(this);
	int nResponse = passwordAdminDlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
		m_isAlreadyGeneratedSQL = FALSE;
		m_isAlreadyPerformedUpdate = FALSE;
		m_noticeSaveRevertSQLWhenExit = FALSE;

		m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

		initConnectDatabase();
	}
	else
	{
		exit(0);
	}
	//{{AFX_DATA_INIT(CUpdateDatabaseDlg)
	//}}AFX_DATA_INIT
}


void CUpdateDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdateDatabaseDlg)
	DDX_Control(pDX, IDC_BTN_EXPORT, m_exportBtn);
	DDX_Control(pDX, IDC_BTN_COPY, m_copyBtn);
	DDX_Control(pDX, IDC_EDIT_UPDATESQL_STATEMENT, m_updateStatementEdit);
	DDX_Control(pDX, IDC_STATIC_CONNECTiNFO, m_connectInfoStatic);
	DDX_Text(pDX, IDC_EDIT_UPDATESQL_STATEMENT, m_updateStatementValue);
	DDX_Text(pDX, IDC_STATIC_CONNECTiNFO, m_connectInfoValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdateDatabaseDlg, CDialog)
	//{{AFX_MSG_MAP(CUpdateDatabaseDlg)
	ON_BN_CLICKED(IDC_BTN_GENERATE, OnBtnGenerate)
	ON_BN_CLICKED(IDC_BTN_PERFORM, OnBtnPerform)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_COPY, OnBtnCopy)
	ON_EN_CHANGE(IDC_EDIT_UPDATESQL_STATEMENT, OnChangeEditUpdatesqlStatement)
	ON_BN_CLICKED(IDC_BTN_EXPORT, OnBtnExport)
	ON_BN_CLICKED(IDC_BTN_REVERT, OnBtnRevert)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdateDatabaseDlg message handlers

void CUpdateDatabaseDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if ((m_revertSQLs.empty() == false)
		&& (m_noticeSaveRevertSQLWhenExit == TRUE)
		&& (IDYES == AfxMessageBox("Save Revert SQL Statement?", MB_YESNO|MB_ICONQUESTION)))
	{
		CTime currentTime=CTime::GetCurrentTime();

		saveSQLFile(Revert, currentTime);
	}

	if (m_dbConn != NULL)
	{
		delete m_dbConn;
		m_dbConn = NULL;
	}
	if (m_exeSql != NULL)
	{
		delete m_exeSql;
		m_exeSql = NULL;
	}
	
	CDialog::OnCancel();
	//CDialog::OnClose();
}

void CUpdateDatabaseDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if ((m_revertSQLs.empty() == false)
		&& (m_noticeSaveRevertSQLWhenExit == TRUE)
		&& (IDYES == AfxMessageBox("Save Revert SQL Statement?", MB_YESNO|MB_ICONQUESTION)))
	{
		CTime currentTime=CTime::GetCurrentTime();

		saveSQLFile(Revert, currentTime);
	}
	
	if (m_dbConn != NULL)
	{
		delete m_dbConn;
		m_dbConn = NULL;
	}
	if (m_exeSql != NULL)
	{
		delete m_exeSql;
		m_exeSql = NULL;
	}

	CDialog::OnCancel();
}

void CUpdateDatabaseDlg::initConnectDatabase()
{
	m_connectInfoValue = _T("");
	m_updateStatementValue = _T("");		

	HRESULT hr;
	
	try
	{
		m_dbConn = new DataBase(m_username, m_password, m_servername);
		hr = m_dbConn->_DB_Connect();
		if( hr != OCI_SUCCESS )
		{
			AfxMessageBox("Connect to database failed.");
			exit(0);
		}
		else
		{
			m_connectInfoValue = _T("Connected to: Server Name: " + m_servername + "; Username: " + m_username + ".");

			if (m_dbConn != NULL)
			{
				m_exeSql = new ExecuteSql(m_dbConn);
			}
		}		
	}
	catch(exception e)
	{
		CString errMsg = e.what();
		AfxMessageBox(errMsg);
		//exit(0);
	}
}

void CUpdateDatabaseDlg::clearOperatorDataRelated()
{
	m_updateStatementValue.Empty();

	for (std::vector<OperatorData*>::iterator operDataIter = m_operatorDatas.begin(); operDataIter != m_operatorDatas.end(); ++operDataIter)
	{
		if (*operDataIter != NULL)
		{
			delete *operDataIter;
			*operDataIter = NULL;
		}
	}
	m_operatorDatas.clear();

	m_updateSQLs.clear();
	m_revertSQLs.clear();

	UpdateData(FALSE);
}

void CUpdateDatabaseDlg::generateSQL()
{
	HRESULT hr;

	clearOperatorDataRelated();

	if (m_exeSql == NULL)
	{
		initConnectDatabase();
	}
	
	//std::string selectSQL = "select * from operator where pkey in (1426,293)";
	//std::string selectSQL = "select pkey, name, passwd from operator where pkey in (254,255,257)";
	std::string selectSQL = "select pkey, name, passwd from operator";
	tRecordSet data;
	data.Init();

	if (m_exeSql != NULL)
	{
		hr = m_exeSql->Execute(selectSQL, data, true);
		if (FAILED(hr))
		{
			AfxMessageBox("Execute select SQL failed");
			return;
		}
		
		int iterRow;
		int rows = data.rownum;
		if ( PASSWD_DB_COL > data.colnum -1)
		{
			AfxMessageBox("Data from Database incorrect!");
		}
		
		CString pkey;
		CString name;
		CString passwd;
		
		for (iterRow = 0; iterRow < rows; ++iterRow)
		{
			pkey = data.vRecord[iterRow][PKEY_DB_COL];
			name = data.vRecord[iterRow][NAME_DB_COL];
			passwd = data.vRecord[iterRow][PASSWD_DB_COL];
			
			OperatorData* _operData = new OperatorData(pkey, name, passwd);
			m_operatorDatas.push_back(_operData);
			
			CString updateSQL = _operData->GetUpdateSQL();
			m_updateSQLs.push_back(updateSQL);
			
			CString revertSQL = _operData->GetRevertSQL();
			m_revertSQLs.push_back(revertSQL);
			
			m_updateStatementValue += updateSQL + ";" +"\r\n";
		}
		m_updateStatementValue += "Commit;";
		UpdateData(false);
	}
	else
	{
		AfxMessageBox("Recordset did not initialize!");
	}
	
}

void CUpdateDatabaseDlg::OnBtnGenerate() 
{
	// TODO: Add your control notification handler code here
	if (m_isAlreadyGeneratedSQL == TRUE)
	{
		if(IDNO == AfxMessageBox("Oracle Password Manager have generated SQL statements before. Are you sure generate again?\r\n\nNotice: This may give up the current update and revert SQL statements. ", MB_YESNO|MB_ICONQUESTION))
		{
			return;
		}
	}

	try
	{
		generateSQL();

		m_isAlreadyGeneratedSQL = TRUE;
		//m_noticeSaveSQLWhenExit = TRUE;
	}
	catch(exception err1)
	{
		CString errMsg1 = err1.what();
		AfxMessageBox(errMsg1);

		// Try again
		try
		{
			//m_pConn->Close();
			initConnectDatabase();
			generateSQL();

			m_isAlreadyGeneratedSQL = TRUE;
			//m_noticeSaveSQLWhenExit = TRUE;
		}
		catch(exception err2)
		{
			CString errMsg2 = err2.what();
			AfxMessageBox("Try again!\r\n\n Still got error: " + errMsg2 + ".\r\n\n Please try again after network and database connection clear.");

			m_isAlreadyGeneratedSQL = FALSE;
			//m_noticeSaveSQLWhenExit = FALSE;
		}
		//exit(0);
	}

	CString updateStatementValue;
	m_updateStatementEdit.GetWindowText(updateStatementValue);
	
	m_copyBtn.EnableWindow(!(updateStatementValue.IsEmpty()));
	m_exportBtn.EnableWindow(!(updateStatementValue.IsEmpty()));

	//CDialog::OnOK();
}

void CUpdateDatabaseDlg::updateSQLStatement()
{
	tRecordSet data;
	data.Init();

	if (m_exeSql != NULL)
	{
		for (std::vector<CString>::iterator updateIter = m_updateSQLs.begin(); updateIter != m_updateSQLs.end(); ++updateIter)
		{
			std::string updateSQLStr = (*updateIter).GetBuffer(0);
			m_exeSql->Execute(updateSQLStr, data, false);
			(*updateIter).ReleaseBuffer();
		}
		m_exeSql->Execute("commit", data, false);
	}

}

void CUpdateDatabaseDlg::OnBtnPerform() 
{
	// TODO: Add your control notification handler code here
	if (m_isAlreadyGeneratedSQL == FALSE)
	{
		AfxMessageBox("Before perform update statement, please generate SQL first.");
	} 
	else
	{
		if (m_isAlreadyPerformedUpdate == FALSE)
		{
			AfxMessageBox("Successfully Performed");
			
			try
			{
				updateSQLStatement();
				
				m_isAlreadyPerformedUpdate = TRUE;
				m_noticeSaveRevertSQLWhenExit = TRUE;
			}
			catch(exception err1)
			{
				CString errMsg1 = err1.what();
				try
				{
					initConnectDatabase();
					updateSQLStatement();
					
					m_isAlreadyPerformedUpdate = TRUE;
					m_noticeSaveRevertSQLWhenExit = TRUE;
				}
				catch(exception err2)
				{
					CString errMsg2 = err2.what();
					AfxMessageBox(errMsg2 + "\r\n\nPlease make sure network and oracle connection is fine.");
					m_noticeSaveRevertSQLWhenExit = FALSE;
				}
			}
		}
		else
		{
			AfxMessageBox("Updated already, ignore perform");
		}
	}
}

void CUpdateDatabaseDlg::revertSQLStatement()
{
	tRecordSet data;
	data.Init();
	
	if (m_exeSql != NULL)
	{
		for (std::vector<CString>::iterator revertIter = m_revertSQLs.begin(); revertIter != m_revertSQLs.end(); ++revertIter)
		{
			std::string revertSQLStr = (*revertIter).GetBuffer(0);
			m_exeSql->Execute(revertSQLStr, data, false);
			(*revertIter).ReleaseBuffer();
		}
		m_exeSql->Execute("commit", data, false);
	}
}

void CUpdateDatabaseDlg::OnBtnRevert() 
{
	// TODO: Add your control notification handler code here
	if (m_isAlreadyPerformedUpdate == FALSE)
	{
		AfxMessageBox("Did not perform update yet, no need revert.");
	}
	else
	{
		AfxMessageBox("Successfully Reverted");

		try
		{
			revertSQLStatement();

			m_isAlreadyPerformedUpdate = FALSE;
			m_noticeSaveRevertSQLWhenExit = FALSE;
		}
		catch(exception err1)
		{
			CString errMsg1 = err1.what();

			try
			{
				initConnectDatabase();
				revertSQLStatement();
				
				m_isAlreadyPerformedUpdate = FALSE;
				m_noticeSaveRevertSQLWhenExit = FALSE;
			}
			catch(exception err2)
			{
				CString errMsg2 = err2.what();
				AfxMessageBox(errMsg2 + ".  " + "\r\n\nPlease make sure network and oracle connection is fine.");
				m_noticeSaveRevertSQLWhenExit = TRUE;
			}
		}
	}
}

void CUpdateDatabaseDlg::OnBtnCopy() 
{
	// TODO: Add your control notification handler code here
	if (OpenClipboard())
	{
		HANDLE hClip;
		CString updateStatementString;
		char* pBuf;
		EmptyClipboard();
		updateStatementString = m_updateStatementValue;
		hClip = GlobalAlloc(GMEM_MOVEABLE,updateStatementString.GetLength() + 1);
		pBuf = (char*)GlobalLock(hClip);
		strcpy(pBuf,updateStatementString);
		GlobalUnlock(hClip);
		SetClipboardData(CF_TEXT, hClip);
		CloseClipboard();

		AfxMessageBox("Copy to Clipboard!");
	}
	else
	{
		AfxMessageBox("Failed to open Clipboard, stop copy!");
	}
}

void CUpdateDatabaseDlg::OnChangeEditUpdatesqlStatement() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CString updateStatementValue;
	m_updateStatementEdit.GetWindowText(updateStatementValue);

	m_copyBtn.EnableWindow(!(updateStatementValue.IsEmpty()));
	m_exportBtn.EnableWindow(!(updateStatementValue.IsEmpty()));

	UpdateData(FALSE);
}

CString CUpdateDatabaseDlg::getDefaultSaveFileName(CString sqlTypeStr, CTime currentTime)
{
	//CTime CurrentTime=CTime::GetCurrentTime();
	CString strTime = "";   
	strTime.Format("%d%d%d%d%d%d",currentTime.GetYear(),currentTime.GetMonth(),currentTime.GetDay(),currentTime.GetHour(),currentTime.GetMinute(),currentTime.GetSecond());
	
	CString defaultSaveFileName = toLower(sqlTypeStr) + "SQL_" + strTime;

	return defaultSaveFileName;
}

void CUpdateDatabaseDlg::saveSQLFile(SQLTYPE sqlType, CTime currentTime)
{
	CString saveSQLType = "";
	std::vector<CString> saveSQLs;
	saveSQLs.clear();

	if (sqlType == Update)
	{
		saveSQLType = "Update";
		saveSQLs = m_updateSQLs;
	}
	else if (sqlType == Revert)
	{
		saveSQLType = "Revert";
		saveSQLs = m_revertSQLs;
	}

	CString saveFilePath;

	CString defaultSaveFileName = getDefaultSaveFileName(saveSQLType, currentTime);

	CFileDialog saveFileDlg(FALSE,NULL,_T(defaultSaveFileName),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"All Files(*.sql)|*.sql|",AfxGetMainWnd()); 
	saveFileDlg.m_ofn.lpstrInitialDir = ".\\";
	CString saveDlgTitle = "Save " + saveSQLType + " SQL File";
	saveFileDlg.m_ofn.lpstrTitle = _T(saveDlgTitle);
	
	if (saveFileDlg.DoModal() == IDOK)
	{
		CString writeSQL = "";
		
		saveFilePath = saveFileDlg.GetPathName();
		if (saveFilePath.Right(4) != ".sql")
		{
			saveFilePath += ".sql";
		}
		
		CFile saveSQLFile(_T(saveFilePath), CFile::modeCreate|CFile::modeWrite);
		
		for (std::vector<CString>::iterator saveIter = saveSQLs.begin(); saveIter != saveSQLs.end(); ++saveIter)
		{
			writeSQL += *saveIter + ";" + "\r\n";
		}

		writeSQL += "Commit;";
		
		saveSQLFile.Write(writeSQL, writeSQL.GetLength());
		saveSQLFile.Close();

		if (saveSQLType == Revert)
		{
			m_noticeSaveRevertSQLWhenExit = FALSE;
		}
	}
}

void CUpdateDatabaseDlg::OnBtnExport() 
{
	// TODO: Add your control notification handler code here
	CTime currentTime=CTime::GetCurrentTime();

	saveSQLFile(Update, currentTime);
	saveSQLFile(Revert, currentTime);
}

CString CUpdateDatabaseDlg::toUpper(CString str)
{
	CString s(str); 
	s.MakeUpper(); 
	return s;
	
}

CString CUpdateDatabaseDlg::toLower(CString str)
{
	CString s(str);
	s.MakeLower();
	return s;
}


BOOL CUpdateDatabaseDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
