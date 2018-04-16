/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulatorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

// ATSECSAgentSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/ats/ATSECSAgentSimulator/src/atsecsagentsimulator.h"
#include "app/ats/ATSECSAgentSimulator/src/ATSECSAgentSimulatorDlg.h"
#include "app/ats/ATSECSAgentSimulator/src/RandomGenerator.h"
#include "app/ecs/ECSAgent/src/CachedMaps.h"
#include "app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ATSECSAgentSimulatorDlg dialog



ATSECSAgentSimulatorDlg::ATSECSAgentSimulatorDlg(TA_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
: TransActiveDialog(genericGUICallback, IDD_ATSECSAGENTSIMULATOR_DIALOG, pParent),
  m_randomGenerator(NULL)
{
    m_zoneIds.clear();

	// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
    TA_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minWidth = -1;
    properties.minHeight = -1;
    setResizingProperties(properties);

    // Initialise the cached map class;
    try
    {
        CachedMaps::getInstance()->refreshZoneIdToRecordMap();
        m_zoneIds = CachedMaps::getInstance()->getAllZoneIds();
    }
    catch (TA_Core::InvalidECSConfigurationException&)
    {
        AfxMessageBox("Invalid ECS configuration detected", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        throw;
    }
    catch (...)
    {
        AfxMessageBox("Error loading ECS configuration", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
        throw;
    }

    m_randomGenerator = new RandomGenerator(this, m_zoneIds.size());
}

ATSECSAgentSimulatorDlg::~ATSECSAgentSimulatorDlg()
{
    if (m_randomGenerator)
    {
        delete m_randomGenerator;
        m_randomGenerator = NULL;
    }

}

void ATSECSAgentSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ATSECSAgentSimulatorDlg)
	DDX_Control(pDX, IDC_RESET_BTN, m_resetBtn);
	DDX_Control(pDX, IDC_ECS_ZONE_LST, m_ECSZoneLst);
	DDX_Control(pDX, IDC_SCHEDULE_NUMBER_TXT, m_scheduleNumberTxt);
	DDX_Control(pDX, IDC_SERVICE_NUMBER_TXT, m_serviceNumberTxt);
	DDX_Control(pDX, IDC_ATC_TRAIN_NUMBER_TXT, m_ATCTrainNumberTxt);
	DDX_Control(pDX, IDC_PHYSICAL_TRAIN_ID_TXT, m_physicalTrainIdTxt);
	DDX_Control(pDX, IDC_TEMPERATURE_TXT, m_temperatureTxt);
	DDX_Control(pDX, IDC_STALL_TIME_TXT, m_stallTimeTxt);
	DDX_Control(pDX, IDC_RANDOMISE_BTN, m_randomiseBtn);
	DDX_Control(pDX, IDC_EXECUTE_BTN, m_executeBtn);
	DDX_Control(pDX, IDC_EMERGENCY_CHK, m_emergencyChk);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ATSECSAgentSimulatorDlg, CDialog)
	//{{AFX_MSG_MAP(ATSECSAgentSimulatorDlg)
	ON_BN_CLICKED(IDC_RANDOMISE_BTN, OnRandomiseBtn)
    ON_MESSAGE(WM_UPDATE_LIST, OnUpdateList)
	ON_BN_CLICKED(IDC_EXECUTE_BTN, OnExecuteBtn)
	ON_BN_CLICKED(IDC_RESET_BTN, OnResetBtn)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ECS_ZONE_LST, OnItemchangedEcsZoneLst)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ATSECSAgentSimulatorDlg message handlers

BOOL ATSECSAgentSimulatorDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Initialise control data
    m_physicalTrainIdTxt.SetWindowText("0");
    m_ATCTrainNumberTxt.SetWindowText("0");
    m_serviceNumberTxt.SetWindowText("0");
    m_scheduleNumberTxt.SetWindowText("0");
    m_stallTimeTxt.SetWindowText("0");
    m_temperatureTxt.SetWindowText("0");

  	/////////////////////////
	// Setup ECS Zone List
	/////////////////////////

	m_ECSZoneLst.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_ECSZoneLst.InsertColumn(0,"ID", LVCFMT_LEFT, 25);
	m_ECSZoneLst.InsertColumn(1,"Track", LVCFMT_LEFT, 200);
	m_ECSZoneLst.InsertColumn(2,"ECS Location", LVCFMT_LEFT, 90);
    m_ECSZoneLst.InsertColumn(3,"Stall Time", LVCFMT_LEFT, 75);
    m_ECSZoneLst.InsertColumn(4,"Temperature", LVCFMT_LEFT, 75);
    m_ECSZoneLst.InsertColumn(5,"Emergency?", LVCFMT_LEFT, 75);

    char zoneId[20];
    std::vector<int>::iterator it = m_zoneIds.begin();
    for ( ; it != m_zoneIds.end() ; ++it)
    {
        sprintf(zoneId, "%ld", (*it));
        std::string track       = CachedMaps::getInstance()->getTrackFromZoneId(*it);
        std::string ECSLocation = CachedMaps::getInstance()->getECSLocationFromZoneId(*it);

        int nextIndex=m_ECSZoneLst.InsertItem(m_ECSZoneLst.GetItemCount(), zoneId); 
	    m_ECSZoneLst.SetItemText(nextIndex,1,track.c_str());
	    m_ECSZoneLst.SetItemText(nextIndex,2,ECSLocation.c_str());
        m_ECSZoneLst.SetItemData(nextIndex,static_cast<DWORD>(*it));
    }

    if (m_zoneIds.size() > 0)
    {
        m_ECSZoneLst.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, 0x000F);
        m_ECSZoneLst.EnsureVisible(0,true);
    }

    OnResetBtn();

    m_viewHandle = m_hWnd;
    
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL ATSECSAgentSimulatorDlg::DestroyWindow() 
{
    if (m_randomGenerator)
    {
        m_randomGenerator->terminateAndWait();
    }
	
	return CDialog::DestroyWindow();
}

void ATSECSAgentSimulatorDlg::updateList(   unsigned long ECSZoneId, 
                                            unsigned long temperature, 
                                            unsigned long stallTime,
                                            bool          isInEmergency ) 
{
    static char buffer[64];

    // Locate ECS Zone ID in list
    int nItem = 0;
    for ( ; nItem < m_ECSZoneLst.GetItemCount(); ++nItem)
    {
        int zoneId = m_ECSZoneLst.GetItemData(nItem);

        if (zoneId == ECSZoneId)
        {
            // found
            break;
        }
	}
    if (nItem == m_ECSZoneLst.GetItemCount())
    {
        // Invalid Zone Id
        return;
    }


    // Update stallTime
    sprintf(buffer, "%ld", stallTime);
    m_ECSZoneLst.SetItemText(nItem,3,buffer);

    // Update temperature
    sprintf(buffer, "%ld", temperature);
    m_ECSZoneLst.SetItemText(nItem,4,buffer);

    // Update Emergency condition
    if (isInEmergency)
    {
        m_ECSZoneLst.SetItemText(nItem,5,"TRUE");
    }
    else
    {
        m_ECSZoneLst.SetItemText(nItem,5,"FALSE");
    }

}

void ATSECSAgentSimulatorDlg::OnRandomiseBtn() 
{
    bool isInRandomiseMode = (m_randomiseBtn.GetCheck()==0)?false:true;
    
    m_executeBtn.EnableWindow(!isInRandomiseMode);
    m_resetBtn.EnableWindow(!isInRandomiseMode);
    m_physicalTrainIdTxt.EnableWindow(!isInRandomiseMode);
    m_ATCTrainNumberTxt.EnableWindow(!isInRandomiseMode);
    m_serviceNumberTxt.EnableWindow(!isInRandomiseMode);
    m_scheduleNumberTxt.EnableWindow(!isInRandomiseMode);
    m_ECSZoneLst.EnableWindow(!isInRandomiseMode);
    m_stallTimeTxt.EnableWindow(!isInRandomiseMode);
    m_temperatureTxt.EnableWindow(!isInRandomiseMode);
    m_emergencyChk.EnableWindow(!isInRandomiseMode);

    if (isInRandomiseMode)
    {
        m_randomGenerator->start();
    }
    else
    {
        m_randomGenerator->terminateAndWait();
    }
}

void ATSECSAgentSimulatorDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

void ATSECSAgentSimulatorDlg::OnExecuteBtn() 
{
    static char buffer[64];

    // Determine ECS Zone ID Selection
    unsigned long ECSZoneId(0);

    POSITION pos = m_ECSZoneLst.GetFirstSelectedItemPosition();
    if (pos == NULL)
    {
        AfxMessageBox("Please select an ECS Zone", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
        return;
    }      
    else
    {
        int nItem = m_ECSZoneLst.GetNextSelectedItem(pos);

        ECSZoneId = m_ECSZoneLst.GetItemData(nItem);

    }

    // Determine temperature
    unsigned long maxExternalTemperatureInCelsius(0);
    m_temperatureTxt.GetWindowText(buffer, 64);
    maxExternalTemperatureInCelsius = atol(buffer);

    // Determine stallTime
    unsigned long stallTimeInSeconds(0);
    m_stallTimeTxt.GetWindowText(buffer, 64);
    stallTimeInSeconds = atol(buffer);

    bool isInEmergency = (m_emergencyChk.GetCheck()==0)?false:true;

    // Send packet to ECS Agents
    ECSAgentUpdater::getInstance()->sendUpdate( ECSZoneId, 
                                                maxExternalTemperatureInCelsius, 
                                                stallTimeInSeconds,
                                                isInEmergency );
    updateList( ECSZoneId, 
                maxExternalTemperatureInCelsius, 
                stallTimeInSeconds,
                isInEmergency );

}

void ATSECSAgentSimulatorDlg::OnResetBtn() 
{
    // Send a NULL packet for every ECS Zone Id to the ECS Agents 

    std::vector<int>::iterator it = m_zoneIds.begin();
    for ( ; it != m_zoneIds.end() ; ++it)
    {
        // Send packet to ECS Agents
        ECSAgentUpdater::getInstance()->sendUpdate( *it, 0, 0, 0 );
        updateList( *it, 0, 0, 0 );
    }
    m_stallTimeTxt.SetWindowText("0");
    m_temperatureTxt.SetWindowText("0");
    m_emergencyChk.SetCheck(0);

}


void ATSECSAgentSimulatorDlg::OnItemchangedEcsZoneLst(NMHDR* pNMHDR, LRESULT* pResult) 
{
    static char buffer[64];

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
  	*pResult = 0;

    int nItem = pNMListView->iItem;

    if (nItem == -1)
    {
        // ignore this update
        return;
    }

	// Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
		return;
	}


	if (pNMListView->uNewState & 0x0002)
	{
		// Item selected
        
        // Update stallTime
        m_ECSZoneLst.GetItemText(nItem,3, buffer, 64);
        m_stallTimeTxt.SetWindowText(buffer);

		// Update temperature
        m_ECSZoneLst.GetItemText(nItem,4, buffer, 64);
        m_temperatureTxt.SetWindowText(buffer);

   		// Update emergency
        m_ECSZoneLst.GetItemText(nItem,5, buffer, 64);
        if (strcmp(buffer,"TRUE") == 0)
        {
            m_emergencyChk.SetCheck(1);
        }
        else
        {
            m_emergencyChk.SetCheck(0);
        }

	}

}

////////////////////////////////////////////////////
//
// Thread-safe message passing between threads...
//
////////////////////////////////////////////////////

//
// The following setX routines are called by the thread running in RandomGenerator
//


void ATSECSAgentSimulatorDlg::setState(   unsigned long ECSZoneId, 
                                          unsigned long maxExternalTemperatureInCelsius, 
                                          unsigned long stallTimeInSeconds,
                                          bool          isInEmergency)
{
    ATSECSPacket* packet = new ATSECSPacket;
    packet->ECSZoneId = ECSZoneId;
    packet->maxExternalTemperatureInCelsius = maxExternalTemperatureInCelsius;
    packet->stallTimeInSeconds = stallTimeInSeconds;
    packet->isInEmergency = isInEmergency;

    ::PostMessage(m_viewHandle, WM_UPDATE_LIST, 0, (LPARAM)packet);
}



//
// The following OnSetX routines are run in the main GUI thread when the message loop
// finds the USER messages in the queue.  These are indirectly triggered by the setX calls
// made by the thread running in RandomGenerator.  In all cases, the operations on the 
// control members calls SendMessage internally, which will not return until the operation 
// is completed.  This will not block as this is called from the main GUI thread.
//

afx_msg LRESULT ATSECSAgentSimulatorDlg::OnUpdateList(UINT wParam, LONG lParam)
{

    ATSECSPacket* packet = reinterpret_cast<ATSECSPacket*>(lParam);
    
    updateList(packet->ECSZoneId,packet->maxExternalTemperatureInCelsius, packet->stallTimeInSeconds, packet->isInEmergency);

    // Move hilight
    for (int nItem=0; nItem < m_ECSZoneLst.GetItemCount(); ++nItem)
    {
        int zoneId = m_ECSZoneLst.GetItemData(nItem);

        if (zoneId == packet->ECSZoneId)
        {
            m_ECSZoneLst.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, 0x000F);
            m_ECSZoneLst.EnsureVisible(nItem,true);
            break;
        }
	}

    delete packet;
    packet = NULL;

    return 0;
}




/******** REMOVED *************************************************************

void ATSECSAgentSimulatorDlg::setTemperature(unsigned long data)
{
    // Note: the following actually works.  Possibly because the class members contain 
    // the original m_hwnd values and therefore are copied across threads.  I am guessing
    // that storing a pointer to this class in another class running in another thread 
    // will have the h_wnd set to zero and cause the application to crash.  Also note that 
    // this method is not thread-safe between RandomGenerator and this class running in the
    // main GUI thread.

    // CString tempString;
    // tempString.Format("%ld",data);
    // m_temperatureTxt.SetWindowText(tempString.GetBuffer(64));

    CString* tempString = new CString();
    tempString->Format("%ld",data);
    ::PostMessage(m_viewHandle, WM_SETTEXT_TEMPERATURE, 0, (LPARAM)tempString);
}

void ATSECSAgentSimulatorDlg::setStallTime(unsigned long data)
{
    CString* tempString = new CString();
    tempString->Format("%ld",data);
    ::PostMessage(m_viewHandle, WM_SETTEXT_STALL_TIME, 0, (LPARAM)tempString);
}


void ATSECSAgentSimulatorDlg::setECSZoneId(unsigned long data)
{
      ::PostMessage(m_viewHandle, WM_SELECT_ECS_ZONE_ID, 0, (LPARAM) data);
}

void ATSECSAgentSimulatorDlg::setEmergencyState(bool isInEmergency)
{
    ::PostMessage(m_viewHandle, WM_SETCHECK_EMERGENCY, 0, (LPARAM)isInEmergency);
    //m_temperatureTxt.SendMessage(WM_SETTEXT,0,(LPARAM)tempString->GetBuffer(64));
    //m_emergencyChk.SetCheck((int) isInEmergency);
}


afx_msg LRESULT ATSECSAgentSimulatorDlg::OnSetTextTemperature(UINT wParam, LONG lParam)
{
    CString* tempString = reinterpret_cast<CString*>(lParam);
    //m_temperatureTxt.SendMessage(WM_SETTEXT,0,(LPARAM)tempString->GetBuffer(64));
    m_temperatureTxt.SetWindowText(tempString->GetBuffer(64));
    
    delete tempString;
    tempString = NULL;

    return 0;
}

afx_msg LRESULT ATSECSAgentSimulatorDlg::OnSetTextStallTime(UINT wParam, LONG lParam)
{
    CString* tempString = reinterpret_cast<CString*>(lParam);
    //m_stallTimeTxt.SendMessage(WM_SETTEXT,0,(LPARAM)tempString->GetBuffer(64));
    m_stallTimeTxt.SetWindowText(tempString->GetBuffer(64));
    
    delete tempString;
    tempString = NULL;

    return 0;
}

afx_msg LRESULT ATSECSAgentSimulatorDlg::OnSetCheckEmergency(UINT wParam, LONG lParam)
{
    int checkState = static_cast<int>(lParam);
    //m_ECSZoneIdTxt.SendMessage(WM_SETTEXT,0,(LPARAM)tempString->GetBuffer(64));
    m_emergencyChk.SetCheck(checkState);

    return 0;
}

afx_msg LRESULT ATSECSAgentSimulatorDlg::OnSelectECSZoneId(UINT wParam, LONG lParam)
{
    long selectedZoneId = lParam;

    for (int nItem=0; nItem < m_ECSZoneLst.GetItemCount(); ++nItem)
    {
        int zoneId = m_ECSZoneLst.GetItemData(nItem);

        if (zoneId == selectedZoneId)
        {
            m_ECSZoneLst.SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, 0x000F);
            m_ECSZoneLst.EnsureVisible(nItem,true);
            break;
        }
	}

    return 0;
}

********* REMOVED *************************************************************/


