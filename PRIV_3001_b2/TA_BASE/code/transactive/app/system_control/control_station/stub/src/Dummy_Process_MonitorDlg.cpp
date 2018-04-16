/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/control_station/stub/src/Dummy_Process_MonitorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// Dummy_Process_MonitorDlg.cpp : implementation file
//

#pragma warning (disable:4786)

#include "stdafx.h"
#include "Dummy_Process_Monitor.h"
#include "Dummy_Process_MonitorDlg.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/RunParams.h"
#include "AddEditParameters.h"
#include "IManagedAgentCorbaDef.h"
#include "IManagedGUICorbaDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorDlg dialog

DummyProcessMonitorDlg::DummyProcessMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DummyProcessMonitorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(DummyProcessMonitorDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    TA_Base_Core::CorbaUtil::getInstance().initialise(6666);
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "ProcessManager");
    TA_Base_Core::CorbaUtil::getInstance().activate();

    m_runParams = new TA_Base_Core::RunParamSeq;

    m_runParams->length(5);
    m_runParams[0].name = CORBA::string_dup(RPARAM_DBCONNECTION);
    m_runParams[0].value = CORBA::string_dup("TRANSACT:ITS:ITS");

    m_runParams[1].name = CORBA::string_dup(RPARAM_SESSIONID);
    m_runParams[1].value = CORBA::string_dup("10");

    m_runParams[2].name = CORBA::string_dup(RPARAM_NOTIFYHOSTS);
    m_runParams[2].value = CORBA::string_dup("jupiter");

    m_runParams[3].name = CORBA::string_dup(RPARAM_DEBUGLEVEL);
    m_runParams[3].value = CORBA::string_dup("INFO");

    m_runParams[4].name = CORBA::string_dup("DebugLogDir");
    m_runParams[4].value = CORBA::string_dup("c:\\transactive\\log\\");
}

DummyProcessMonitorDlg::~DummyProcessMonitorDlg()
{
    TA_Base_Core::CorbaUtil::getInstance().cleanup();
}


void DummyProcessMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DummyProcessMonitorDlg)
	DDX_Control(pDX, IDC_LIST2, m_listParameters);
	DDX_Control(pDX, IDC_LIST1, m_listApps);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(DummyProcessMonitorDlg, CDialog)
	//{{AFX_MSG_MAP(DummyProcessMonitorDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, OnButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE, OnButtonTerminate)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnDblclkParameterList)
	ON_BN_CLICKED(IDC_MAXIMISE, OnMaximise)
	ON_BN_CLICKED(IDC_MINIMISE, OnMinimise)
	ON_BN_CLICKED(IDC_RESTORE, OnRestore)
	ON_BN_CLICKED(IDC_SERVERDOWN, OnServerDown)
	ON_BN_CLICKED(IDC_SERVERUP, OnServerUp)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, OnButtonLoad)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DummyProcessMonitorDlg message handlers

BOOL DummyProcessMonitorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
    m_listApps.InsertColumn(0,"Id",LVCFMT_LEFT,40);
    m_listApps.InsertColumn(1,"Entity",LVCFMT_LEFT,100);
    m_listApps.InsertColumn(2,"AppType",LVCFMT_LEFT,65);
    m_listApps.InsertColumn(3,"Status",LVCFMT_LEFT,150);
    m_listApps.InsertColumn(4,"Type",LVCFMT_LEFT,50);
    m_listApps.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );


    m_listParameters.InsertColumn(0,"Parameter",LVCFMT_LEFT,150);
    m_listParameters.InsertColumn(1,"Value",LVCFMT_LEFT,150);
    m_listParameters.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

    // --param-file=<file path> command line parameter can be used to set a default parameter file
    std::string paramFile = TA_Base_Core::RunParams::getInstance().get("ParamFile");
    if (paramFile.empty() || !loadParamsFromFile(paramFile))
    {
        for (unsigned int i = 0; i < m_runParams->length(); ++i)
        {
            int item = m_listParameters.InsertItem(0,m_runParams[i].name);
            m_listParameters.SetItemText(item,1,m_runParams[i].value);
            m_listParameters.SetItemData(item,i);
        }
    }
   
    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void DummyProcessMonitorDlg::OnPaint() 
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
HCURSOR DummyProcessMonitorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void DummyProcessMonitorDlg::terminating( TA_Base_Core::ProcessId p_processID, 
                                          TA_Base_Core::EManagedProcessTerminateCode p_code)
{
    ProcessMap::iterator iter = m_currentRunningApps.find( p_processID );
    if (iter == m_currentRunningApps.end() )
    {
        return;
    }

    m_currentRunningApps.erase(iter);

    AppMap::iterator iter2 = m_currentRunningGUIAgentApps.find( p_processID );
    if (iter2 != m_currentRunningGUIAgentApps.end() )
    {
        m_currentRunningGUIAgentApps.erase(iter2);
    }

    CString idString;
    idString.Format("%d",p_processID);

    // Update the app to say it has terminated
    LVFINDINFO searchDetails;
    searchDetails.flags = LVFI_STRING;
    searchDetails.psz = idString;
    int item = m_listApps.FindItem(&searchDetails);

    if (item != -1)
    {

        CString status;
        switch(p_code)
        {
        case(TA_Base_Core::RequestedTerminate):
                status = "Terminated - Requested";
                break;
        case(TA_Base_Core::InitError):
                status = "Terminated - Init Error";
                break;
        case(TA_Base_Core::CommsError):
                status = "Terminated - Comms Error";
                break;
        case(TA_Base_Core::UserExit):
                status = "Terminated - User Exit";
                break;
        case(TA_Base_Core::DependencyFailure):
                status = "Terminated - Dependency Failure";
                break;
        case(TA_Base_Core::FatalError):
                status = "Terminated - Fatal Error";
                break;
        default:
                status = "Terminated - Unknown";
                break;
        }

        m_listApps.SetItemText(item,3,status);
    }
}


void DummyProcessMonitorDlg::registerManagedProcess( TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
                                                     TA_Base_Core::ProcessId p_processID, 
                                                     CORBA::ULong applicationType,
                                                     const char* entity)

{
    // DO a quick poll to check it is alive
    try
    {
        if (p_managedProcess != NULL)
        {
           TA_Base_Core::IManagedProcessCorbaDef_ptr app = TA_Base_Core::IManagedProcessCorbaDef::_narrow(p_managedProcess);

            if ( CORBA::is_nil(app) )
            {
                return;
            }
            app->poll();
        }
    }
    catch (...)
    {
    }

	m_currentRunningApps.insert(ProcessMap::value_type(p_processID, p_managedProcess) );

    CString appTypeStr;
    appTypeStr.Format("%d",applicationType);
    CString idString;
    idString.Format("%d",p_processID);

    int pos = m_listApps.InsertItem(0,idString);
    m_listApps.SetItemText(pos,1,entity);
    m_listApps.SetItemText(pos,2,appTypeStr);
    m_listApps.SetItemText(pos,3,"Running");
    m_listApps.SetItemData(pos,p_processID);
}

void DummyProcessMonitorDlg::registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
                                                        TA_Base_Core::ProcessId p_processId)
{
	m_currentRunningGUIAgentApps.insert(AppMap::value_type(p_processId, p_managedApp) );
    
    CString idString;
    idString.Format("%d",p_processId);

    LVFINDINFO searchDetails;
    searchDetails.flags = LVFI_STRING;
    searchDetails.psz = idString;
    
    int item = m_listApps.FindItem(&searchDetails);
    if (item == -1)
    {
        return;
    }

    try
    {
        if ( !CORBA::is_nil(TA_Base_Core::IManagedAgentCorbaDef::_narrow(p_managedApp)) )
        {
            m_listApps.SetItemText(item,4,"Agent");
        }
        else if (!CORBA::is_nil(TA_Base_Core::IManagedGUICorbaDef::_narrow(p_managedApp)) )
        {
            m_listApps.SetItemText(item,4,"GUI");
        }
    }
    catch ( ... )
    {
        return;
    }
}


TA_Base_Core::RunParamSeq*  DummyProcessMonitorDlg::getParams(TA_Base_Core::ProcessId id, const char* hostname)
{
    TA_Base_Core::RunParamSeq* params = new TA_Base_Core::RunParamSeq;

    int size = m_runParams->length();

    int insertPosition = 0;
    bool foundADebugFile = false;
    bool foundADebugPath = false;
    std::string debugFile = "";
    for (int i = 0; i < size; ++i)
    {
        std::string name = m_runParams[i].name;
        if( 0 == name.compare("DebugLogDir") )
        {
            CString idString;
            idString.Format("%d",id);
            LVFINDINFO searchDetails;
            searchDetails.flags = LVFI_STRING;
            searchDetails.psz = idString;
    
            int item = m_listApps.FindItem(&searchDetails);
            if (item == -1)
            {
                continue;
            }
            CString entity = m_listApps.GetItemText(item,1);

            debugFile += m_runParams[i].value;
            debugFile += "Log_";
            debugFile += entity;
            debugFile += ".log";

            foundADebugPath = true;
            continue;

        }
        if( 0 == name.compare("DebugFile") )
        {
            foundADebugFile = true;
        }

        params->length(insertPosition+1);
        (*params)[insertPosition].name = CORBA::string_dup( name.c_str() );
        (*params)[insertPosition].value = CORBA::string_dup( m_runParams[i].value );
        ++insertPosition;
    }

    if ( (!foundADebugFile) && (foundADebugPath) )
    {
        params->length(insertPosition+1);
        (*params)[insertPosition].name = CORBA::string_dup( RPARAM_DEBUGFILE );
        (*params)[insertPosition].value = CORBA::string_dup( debugFile.c_str() );
        ++insertPosition;
    }

    return params;
}

void DummyProcessMonitorDlg::poll()
{

}

void DummyProcessMonitorDlg::statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId)
{

}

void DummyProcessMonitorDlg::OnButtonRemove() 
{
    POSITION pos = m_listApps.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
         return;
    }
    int selected = m_listApps.GetNextSelectedItem(pos);

    CString text = m_listApps.GetItemText(selected,3);
    if ( -1 != text.Find("Terminated") )
    {
        m_listApps.DeleteItem(selected);
    }
    else
    {
       int response = MessageBox("This application is still running do you really want to remove it?","Dummy Process Monitor",MB_YESNO|MB_ICONEXCLAMATION);
       if (response == IDYES)
       {
            ProcessMap::iterator iter = m_currentRunningApps.find( m_listApps.GetItemData(selected) );
            if (iter != m_currentRunningApps.end() )
            {
                m_currentRunningApps.erase(iter);
            }

            AppMap::iterator iter2 = m_currentRunningGUIAgentApps.find( m_listApps.GetItemData(selected) );
            if (iter2 != m_currentRunningGUIAgentApps.end() )
            {
                m_currentRunningGUIAgentApps.erase(iter2);
            }

            m_listApps.DeleteItem(selected);
       }
    }
}

void DummyProcessMonitorDlg::OnButtonClear() 
{
	m_listApps.DeleteAllItems();
    m_currentRunningApps.clear();
    m_currentRunningGUIAgentApps.clear();
}


void DummyProcessMonitorDlg::OnButtonTerminate() 
{
    POSITION pos = m_listApps.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
         return;
    }
    int selected = m_listApps.GetNextSelectedItem(pos);
	
    ProcessMap::iterator iter = m_currentRunningApps.find( m_listApps.GetItemData(selected) );
    if (iter == m_currentRunningApps.end() )
    {
        return;
    }

    try
    {
        if (iter->second != NULL)
        {
           TA_Base_Core::IManagedProcessCorbaDef_ptr app = TA_Base_Core::IManagedProcessCorbaDef::_narrow(iter->second);

            if ( CORBA::is_nil(app) )
            {
                return;
            }

            app->terminate();
        }
    }
    catch (...)
    {
    }
}


/////////////////////////////////////////////
// Parameter Buttons
/////////////////////////////////////////////

void DummyProcessMonitorDlg::OnButtonAdd() 
{
	AddEditParameters dlg;
    if( IDOK == dlg.DoModal() )
    {
        int newLength = m_runParams->length() + 1;
        m_runParams->length(newLength);
        m_runParams[newLength - 1].name = CORBA::string_dup(dlg.m_name);
        m_runParams[newLength - 1].value = CORBA::string_dup(dlg.m_value);

        int item = m_listParameters.InsertItem(0,dlg.m_name);
        m_listParameters.SetItemText(item,1,dlg.m_value);
        m_listParameters.SetItemData(item,newLength - 1);

        updateSelectedApp((LPCTSTR)dlg.m_name, (LPCTSTR)dlg.m_value);
    }

}

void DummyProcessMonitorDlg::OnButtonDelete() 
{
    POSITION pos = m_listParameters.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
         return;
    }
    int selected = m_listParameters.GetNextSelectedItem(pos);
    int indexToRemove = m_listParameters.GetItemData(selected);

    // Copy current runparams
    // this is not efficient but since this is only a dummy app we don't really care.
    TA_Base_Core::RunParamSeq* temp = new TA_Base_Core::RunParamSeq(m_runParams);

    // Get old length
    int oldLength = m_runParams->length();
    // Readjust length to remove the old item
    m_runParams->length( oldLength - 1 );
    int position = 0;
    for ( int i=0; i < oldLength; ++i)
    {
        if (i != indexToRemove)
        {
            m_runParams[position].name = CORBA::string_dup((*temp)[i].name);
            m_runParams[position].value = CORBA::string_dup((*temp)[i].value);
            ++position;
        }
        else
        {
            updateSelectedApp((*temp)[i].name.in(), "");
        }
    }

    delete temp;

    // Delete all items and add back into the list
    m_listParameters.DeleteAllItems();
    for ( i = 0; i < m_runParams->length(); ++i)
    {
        int item = m_listParameters.InsertItem(0,m_runParams[i].name);
        m_listParameters.SetItemText(item,1,m_runParams[i].value);
        m_listParameters.SetItemData(item,i);
    }
}


void DummyProcessMonitorDlg::OnButtonEdit() 
{
    POSITION pos = m_listParameters.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
         return;
    }
    int selected = m_listParameters.GetNextSelectedItem(pos);

    int index = m_listParameters.GetItemData(selected);

    AddEditParameters dlg;
    dlg.m_name = m_runParams[index].name;
    dlg.m_value = m_runParams[index].value;

    if( IDOK == dlg.DoModal() )
    {
        m_runParams[index].name = CORBA::string_dup(dlg.m_name);
        m_runParams[index].value = CORBA::string_dup(dlg.m_value);

        m_listParameters.SetItemText(selected, 0, dlg.m_name);
        m_listParameters.SetItemText(selected, 1, dlg.m_value);

        updateSelectedApp((LPCTSTR)dlg.m_name, (LPCTSTR)dlg.m_value);
    }
}


void DummyProcessMonitorDlg::OnDblclkParameterList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnButtonEdit();
	
	*pResult = 0;
}


void DummyProcessMonitorDlg::OnButtonLoad() 
{
	static char BASED_CODE szFilter[] = "Parameter Files (*.par)|*.par||";

	CFileDialog dlg (TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		".par",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);

	if( IDOK == dlg.DoModal() )
    {
        loadParamsFromFile((LPCTSTR) dlg.GetPathName());
    }
}


void DummyProcessMonitorDlg::OnButtonSave() 
{
	static char BASED_CODE szFilter[] = "Parameter Files (*.par)|*.par||";

	CFileDialog dlg (FALSE, // TRUE for FileOpen, FALSE for FileSaveAs
		".par",
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		szFilter);

	if( IDOK == dlg.DoModal() )
    {
		CFile file ( dlg.GetPathName(), CFile::modeCreate | CFile::modeWrite );
		
		for (unsigned int index = 0; index < m_runParams->length(); index++)
		{
			// Save all the parameters in "parameter=value" format
			CString line = m_runParams[index].name;
			line += '=';
			line += m_runParams[index].value;
			line += "\r\n";

			file.Write(line, line.GetLength());
		}
	
		file.Close();
	}
}


/////////////////////////////////////////////
// GUI Functions
/////////////////////////////////////////////

void DummyProcessMonitorDlg::OnMaximise() 
{
    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = getGUI();

        if (gui == NULL)
        {
            return;
        }

        gui->changeFocus( TA_Base_Core::Maximise );
    }
    catch ( ... )
    {
        return;
    }
}

void DummyProcessMonitorDlg::OnMinimise() 
{
    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = getGUI();

        if (gui == NULL)
        {
            return;
        }

        gui->changeFocus( TA_Base_Core::Minimise );
    }
    catch ( ... )
    {
        return;
    }
}

void DummyProcessMonitorDlg::OnRestore() 
{
    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = getGUI();

        if (gui == NULL)
        {
            return;
        }

        gui->changeFocus( TA_Base_Core::GetFocus );
    }
    catch ( ... )
    {
        return;
    }
}

void DummyProcessMonitorDlg::OnServerDown() 
{
    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = getGUI();

        if (gui == NULL)
        {
            return;
        }

        gui->serverStateChange(false);
    }
    catch ( ... )
    {
        return;
    }
}

void DummyProcessMonitorDlg::OnServerUp() 
{
    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = getGUI();

        if (gui == NULL)
        {
            return;
        }

        gui->serverStateChange(true);
    }
    catch ( ... )
    {
        return;
    }
}


TA_Base_Core::IManagedGUICorbaDef_ptr DummyProcessMonitorDlg::getGUI()
{
    POSITION pos = m_listApps.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
         return NULL;
    }
    int selected = m_listApps.GetNextSelectedItem(pos);

    if ( 0 != m_listApps.GetItemText(selected,4).Compare("GUI") )
    {
        // not a gui
        return NULL;
    }
    if ( 0 != m_listApps.GetItemText(selected,3).Compare("Running") )
    {
        // not running
        return NULL;
    }

    int id = m_listApps.GetItemData(selected);

    AppMap::iterator iter = m_currentRunningGUIAgentApps.find( id );
    if (iter == m_currentRunningGUIAgentApps.end() )
    {
        return NULL;
    }

    try
    {
        TA_Base_Core::IManagedGUICorbaDef_ptr gui = TA_Base_Core::IManagedGUICorbaDef::_narrow(iter->second);

        if ( CORBA::is_nil(gui) )
        {
            return NULL;
        }

        return gui;
    }
    catch ( ... )
    {
        return NULL;
    }
}


void DummyProcessMonitorDlg::updateSelectedApp(std::string runParamName, std::string runParamValue)
{
    if (runParamName.empty())
    {
        return;
    }

    POSITION pos = m_listApps.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        return;
    }
    int selected = m_listApps.GetNextSelectedItem(pos);
	
    ProcessMap::iterator iter = m_currentRunningApps.find( m_listApps.GetItemData(selected) );
    if (iter == m_currentRunningApps.end() )
    {
        return;
    }
    
    try
    {
        if (iter->second != NULL)
        {
            TA_Base_Core::IManagedProcessCorbaDef_ptr app = TA_Base_Core::IManagedProcessCorbaDef::_narrow(iter->second);

            if ( CORBA::is_nil(app) )
            {
                return;
            }

            TA_Base_Core::RunParam parameter;
            parameter.name =  CORBA::string_dup(runParamName.c_str());
            parameter.value = CORBA::string_dup(runParamValue.c_str());

            app->setParam(parameter);
        }
    }
    catch (...)
    {
        CString msg;
        msg.Format("Failed to set runtime parameter %s.", runParamName.c_str());
        AfxMessageBox(msg, MB_ICONWARNING);
    }	
}


bool DummyProcessMonitorDlg::loadParamsFromFile(std::string paramFile)
{
    try
    {
        CFile file ( paramFile.c_str(), CFile::modeRead );

	    char lastChar = '\0';
	    bool ignoreLine = '\0';
	    unsigned int index = 0;
	    enum { readParameter, readValue } readState = readParameter;
	    CString parameter, value;

	    // While the end of file is not reached
	    while (file.GetPosition() < file.GetLength())
	    {
		    char readChar;

		    file.Read(&readChar, 1);
		    
		    switch (readChar) {
			    
		    case '=':
			    // '=' denotes the end of the parameter if it is not in a comment "//"
			    if (ignoreLine == false)
			    {	
				    if (readState == readParameter)
				    {
					    readState = readValue;
				    }
				    else 
				    {	
					    value += readChar;
				    }
			    }
			    break;
			    
		    case '/':
			    if (lastChar == '/')
			    {
				    // Found a "//" - ignore the rest of the line
				    ignoreLine = true;	

				    // Delete the trailing '/'
				    if (readState == readParameter)
				    {
					    parameter.Delete(parameter.GetLength()-1, 1);
				    }
				    else
				    {
					    value.Delete(value.GetLength()-1, 1);
				    }
			    }
			    else
			    {
				    // This could be a valid character - add it
				    if (readState == readParameter)
				    {
					    parameter += readChar;
				    }
				    else
				    {
					    value += readChar;
				    }
			    }
			    break;

		    case '\r':
			    // Ignore
			    break;

		    case '\n':
			    // Found CR LF pair
			    if (lastChar == '\r')
			    {
				    // Add the parameter if it meets some sanity checks
				    if (parameter.GetLength() > 0)
				    {
					    index++;
					    m_runParams->length(index);
					    m_runParams[index-1].name = CORBA::string_dup(parameter);
					    m_runParams[index-1].value = CORBA::string_dup(value);
				    }

				    // Reset for a new line
				    readState = readParameter;
				    ignoreLine = false;
				    parameter.Empty();
				    value.Empty();
			    }
			    break;

		    default:
			    // Add other characters if the line isn't supposed to be ignored.
			    if (ignoreLine == false)
			    {
				    if (readState == readParameter)
				    {
					    parameter += readChar;
				    }
				    else
				    {
					    value += readChar;
				    }
			    }
			    break;
		    }

		    // Remember the last character
		    lastChar = readChar;
	    }

		    
	    file.Close();

	    // If we didn't have end of line before the end of the file
	    // add the parameter anyway.
	    if (parameter.GetLength() > 0)
	    {
		    index++;
		    m_runParams->length(index);
		    m_runParams[index-1].name = CORBA::string_dup(parameter);
		    m_runParams[index-1].value = CORBA::string_dup(value);
	    }

	    // Delete all items and add back into the list
	    m_listParameters.DeleteAllItems();
	    for ( index = 0; index < m_runParams->length(); ++index)
	    {
		    int item = m_listParameters.InsertItem(0,m_runParams[index].name);
		    m_listParameters.SetItemText(item,1,m_runParams[index].value);
		    m_listParameters.SetItemData(item,index);

            updateSelectedApp(m_runParams[index].name.in(), m_runParams[index].value.in());
	    }
    }
    catch (CFileException* ex)
    {
        ex->ReportError();
        ex->Delete();

        return false;
    }
    catch (...)
    {
        return false;
    }

    return true;
}
