/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/security/DutyManager/src/RegionDutyPropertyPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:57:05 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the "Regions" (Locations) tab. It displays the active 
  * region duty state for logged in operators.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const CString OPERATOR_COLUMN_HEADING("Operator");
    const int FIXED_COLUMN_COUNT(1);
    const int FIXED_ROW_COUNT(1);
    const int OPERATOR_COLUMN(0);
    const int REGION_ROW(0);

    const int DEFAULT_ROW_HEIGHT(20);
    const int DEFAULT_COLUMN_WIDTH(70);

    const COLORREF DELEGATE_SUB_DUTY_BTN_TXT_COLOUR(::GetSysColor(COLOR_BTNTEXT));
    const COLORREF DELEGATE_SUB_DUTY_BTN_BK_COLOUR(RGB(255, 0, 0)); // red
    //const COLORREF CURRENT_SESSION_COLOUR(RGB(0, 0, 255)); // blue
	const COLORREF CURRENT_SESSION_COLOUR(RGB(0, 255, 0)); // green
}

using TA_Base_Bus::SessionCache;
using TA_Base_Bus::RegionDutyMatrixElement;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionKey;
using TA_Base_Bus::SessionId;
using TA_Base_Bus::SessionIdList;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::ProfileKey;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::RegionDutyList;
using TA_Base_Bus::RegionDutyUpdate;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

extern bool g_populated;
/////////////////////////////////////////////////////////////////////////////
// RegionDutyPropertyPage property page

IMPLEMENT_DYNCREATE(RegionDutyPropertyPage, CPropertyPage)

RegionDutyPropertyPage::RegionDutyPropertyPage() : 
    CPropertyPage(RegionDutyPropertyPage::IDD),
    ISessionObserver(DutyDataStore::getSessionCache()),
    IRegionDutyObserver(DutyDataStore::getRegionDutyMatrix()),
    m_delegateSubsystemDutyBtn(DELEGATE_SUB_DUTY_BTN_TXT_COLOUR, DELEGATE_SUB_DUTY_BTN_BK_COLOUR),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache()),
    m_regionDutyMatrix(DutyDataStore::getRegionDutyMatrix())
{
    FUNCTION_ENTRY("RegionDutyPropertyPage()");

	//{{AFX_DATA_INIT(RegionDutyPropertyPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USELOCATIONLABEL)) 
    {
        static const CString PAGE_CAPTION("Locations");
        m_psp.pszTitle = PAGE_CAPTION;
        m_psp.dwFlags |= PSP_USETITLE;
    }

    FUNCTION_EXIT;
}


RegionDutyPropertyPage::~RegionDutyPropertyPage()
{
    FUNCTION_ENTRY("~RegionDutyPropertyPage()");
	//DutyDataStore::cleanUp();
    FUNCTION_EXIT;
}


void RegionDutyPropertyPage::initialiseRegionDutyGrid()
{
    SessionIdList sessionIds(m_regionDutyMatrix.getRowIds());
    RegionKeyList regionKeys(m_regionDutyMatrix.getColumnIds());

	std::map<unsigned long, std::string> orderIdDisplayNameMap;
	std::vector<unsigned long> orderIds = TA_Base_Core::LocationAccessFactory::getInstance().getOrderIdsFromRegionKeys(regionKeys, orderIdDisplayNameMap);
	sort(orderIds.begin(),orderIds.end());

    m_regionDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_regionDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_regionDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_regionDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_regionDutyGrid.SetListMode(TRUE);
    m_regionDutyGrid.EnableTitleTips(TRUE);
    m_regionDutyGrid.EnableSelection(FALSE);
    m_regionDutyGrid.EnableDragAndDrop(FALSE);
    m_regionDutyGrid.SetEditable(FALSE);
    m_regionDutyGrid.SetHeaderSort(FALSE);

    m_regionDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_regionDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for operator names and login details
    m_regionDutyGrid.SetRowCount(m_regionDutyGrid.GetFixedRowCount());  // this will grow as operators are added to the grid
    m_regionDutyGrid.SetColumnCount(m_regionDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
	m_regionDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
    // Set the fixed column labels.
    m_regionDutyGrid.SetItemText(REGION_ROW, OPERATOR_COLUMN, OPERATOR_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(OPERATOR_COLUMN + 1);
	CString regionName = "";

    //for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
	for(std::vector<unsigned long>::const_iterator orderIt = orderIds.begin(); orderIt != orderIds.end(); orderIt++)
    {
		regionName = "";

        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = REGION_ROW;
            gridItem.col = column;
			gridItem.szText = orderIdDisplayNameMap[*orderIt].c_str();
			gridItem.lParam = *orderIt;


			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "orderIt: %ld, displayname: %s", 
				*orderIt, gridItem.szText.GetBuffer(10));

			if(0 == gridItem.szText.CompareNoCase(_T("All Locations")))
			{
				// Delete 'All Locations' column
				m_regionDutyGrid.DeleteColumn(column);
				continue;
			}
            m_regionDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_regionDutyGrid.DeleteColumn(m_regionDutyGrid.GetColumnCount() - 1);

            //MessageBox::error("Error while adding region to region duty grid.", ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while adding operator to region duty grid.");
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
        }
    }

    // Add active operators to the grid. Operator names are used as entries in the first column.
    for (SessionIdList::const_iterator sessionIt = sessionIds.begin(); sessionIt != sessionIds.end(); sessionIt++)
    {
        try
        {
            addOperator(*sessionIt);
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            //MessageBox::error("Error while adding operator to region duty grid.", ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while adding operator to region duty grid.");
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
        }
    }

    // Size operator column width to fit operator names.
    m_regionDutyGrid.AutoSizeColumn(OPERATOR_COLUMN);
//	m_regionDutyGrid.SetRedraw(TRUE,TRUE);
//	m_regionDutyGrid.ShowScrollBar(SB_HORZ,TRUE);
}

void RegionDutyPropertyPage::initialiseRegionDutyGridStatic()
{
	RegionKeyList regionKeys(m_regionDutyMatrix.getColumnIds());

	std::map<unsigned long, std::string> orderIdDisplayNameMap;
	std::vector<unsigned long> orderIds = TA_Base_Core::LocationAccessFactory::getInstance().getOrderIdsFromRegionKeys(regionKeys, orderIdDisplayNameMap);
	
	std::map<unsigned long, unsigned long> orderIdRegionMap;
	for(int orderIndex=0; orderIndex < orderIds.size();orderIndex++)
	{
		orderIdRegionMap[orderIds[orderIndex]] = regionKeys[orderIndex];
	}

	sort(orderIds.begin(),orderIds.end());

	m_regionDutyGrid.SetImageList(&GridAssistant::getGridImages());
	m_regionDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
	m_regionDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
	m_regionDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
	m_regionDutyGrid.SetListMode(TRUE);
	m_regionDutyGrid.EnableTitleTips(TRUE);
	m_regionDutyGrid.EnableSelection(FALSE);
	m_regionDutyGrid.EnableDragAndDrop(FALSE);
	m_regionDutyGrid.SetEditable(FALSE);
	m_regionDutyGrid.SetHeaderSort(FALSE);

	m_regionDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
	m_regionDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for operator names and login details
	m_regionDutyGrid.SetRowCount(m_regionDutyGrid.GetFixedRowCount());  // this will grow as operators are added to the grid
	m_regionDutyGrid.SetColumnCount(m_regionDutyGrid.GetFixedColumnCount() + regionKeys.size());

	m_regionDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
	// Set the fixed column labels.
	m_regionDutyGrid.SetItemText(REGION_ROW, OPERATOR_COLUMN, OPERATOR_COLUMN_HEADING);

	// Set the cells in the first row with the names of the regions.
	// Use cell item data to store the corresponding region keys so that the region
	// representing each column can be easily identified.
	int column(OPERATOR_COLUMN + 1);
	CString regionName = "";

	for(std::vector<unsigned long>::const_iterator orderIt = orderIds.begin(); orderIt != orderIds.end(); orderIt++)
    {
		regionName = "";

        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = REGION_ROW;
            gridItem.col = column;
			gridItem.szText = orderIdDisplayNameMap[*orderIt].c_str();
			//gridItem.lParam = *orderIt;
			gridItem.lParam = orderIdRegionMap[*orderIt];




			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "orderIt: %ld, regionid %ld displayname: %s",*orderIt, orderIdRegionMap[*orderIt],gridItem.szText.GetBuffer(10));

			if(0 == gridItem.szText.CompareNoCase(_T("All Locations")))
			{
				// Delete 'All Locations' column
				m_regionDutyGrid.DeleteColumn(column);
				continue;
			}
            m_regionDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_regionDutyGrid.DeleteColumn(m_regionDutyGrid.GetColumnCount() - 1);

            //MessageBox::error("Error while adding region to region duty grid.", ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while adding operator to region duty grid.");
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
        }
    }
}

void RegionDutyPropertyPage::updateRegionDutyGrid()
{
	SessionIdList sessionIds(m_regionDutyMatrix.getRowIds());
	// Add active operators to the grid. Operator names are used as entries in the first column.
	for (SessionIdList::const_iterator sessionIt = sessionIds.begin(); sessionIt != sessionIds.end(); sessionIt++)
	{
		try
		{
			addOperator(*sessionIt);
		}
		catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
		{
			//MessageBox::error("Error while adding operator to region duty grid.", ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while adding operator to region duty grid.");
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
		}
	}

	// Size operator column width to fit operator names.
	m_regionDutyGrid.AutoSizeColumn(OPERATOR_COLUMN);
}

void RegionDutyPropertyPage::enableDutyControlButtons()
{
    m_transferRegionDutyBtn.EnableWindow(m_dutyRights.canTransferRegions());
	// Due to [MW-181] on TAR0514_C955_Mysql_Win7_Issuelist.xls, need to hide the "Take Location Button"
	std::string showSessionlocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_SHOWSESSIONWORKSTATION).c_str();
	if ( showSessionlocation.compare("OPEN") != 0 )
	{
		m_transferRegionDutyBtn.ShowWindow(SW_HIDE);
	}

    m_delegateSubsystemDutyBtn.EnableWindow(m_dutyRights.canDegradeSubsystems());
}


void RegionDutyPropertyPage::addOperator(const TA_Base_Bus::SessionInfoEx& sessionInfoEx)
{
    // Append the new operator to the end of the grid and re-order rows by operator name.
    int row(m_regionDutyGrid.GetRowCount());

    m_regionDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_regionDutyGrid.SetRowCount(row + 1); // unlike InsertRow(), this will use the default row height
    updateOperator(row, sessionInfoEx);

    // updateOperator() sorts operators, so locate the relevant row following the reshuffle.
    row = GridAssistant::findGridRow(m_regionDutyGrid, sessionInfoEx.sessionKey);
    TA_ASSERT(row != GridAssistant::INVALID_GRID_ROW, "Row not found");

    // Initialise grid cells for the new operator based on his region duties.
    for (int column = OPERATOR_COLUMN + 1; column < m_regionDutyGrid.GetColumnCount(); column++)
    {
        RegionKey regionKey(m_regionDutyGrid.GetItemData(REGION_ROW, column));

        try
        {
            std::string profile(GridAssistant::generateProfileString(sessionInfoEx, regionKey));
            m_regionDutyGrid.SetItemText(row, column, profile.c_str());
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            std::ostringstream msg;
            msg << "Unable to initialise region duty grid cell for " << std::endl
                << "operator " << m_dataCache.getOperatorName(sessionInfoEx.UserId)
                << " and region " << m_dataCache.getRegionDisplayName(regionKey);

            //MessageBox::error(msg.str(), ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            std::ostringstream msg;
            msg << "Unable to initialise region duty grid cell for " << std::endl
                << "operator " << m_dataCache.getOperatorName(sessionInfoEx.UserId)
                << " and region " << m_dataCache.getRegionDisplayName(regionKey);

            //MessageBox::error(msg.str(), ex); //TD15901
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, msg.str().c_str());
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
    }

    // Request a redraw of the grid for the changes to become visible.
    m_regionDutyGrid.Invalidate();
}


void RegionDutyPropertyPage::addOperator(const TA_Base_Bus::SessionId& sessionId)
{
    addOperator(m_sessionCache.getSessionInfo(sessionId));
}


void RegionDutyPropertyPage::updateOperator(int row, const TA_Base_Bus::SessionInfoEx& sessionInfoEx)
{
    TA_ASSERT(row > REGION_ROW && row < m_regionDutyGrid.GetRowCount(), "Row index out of bounds");

    GV_ITEM gridItem;

    gridItem.mask = GVIF_TEXT | GVIF_IMAGE | GVIF_PARAM;
    gridItem.row = row;
    gridItem.col = OPERATOR_COLUMN;

	std::string text = m_dataCache.getOperatorDescription(sessionInfoEx.UserId);

	std::string showSessionlocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_SHOWSESSIONWORKSTATION).c_str();
	if ( showSessionlocation.compare("OPEN") == 0 )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[MW-154] Remove session's location information at normal type, but with a --show-sessionlocation=OPEN has been set, then show the location again now.");
		text+= "@";
		text+= m_dataCache.getWorkstationName(sessionInfoEx.WorkstationId);
	}

    gridItem.szText = text.c_str(); 
    gridItem.iImage = GridAssistant::isCentralProfileInList(sessionInfoEx.ProfileId)? GridAssistant::GI_CENTRAL : GridAssistant::GI_LOCAL;
    gridItem.lParam = sessionInfoEx.sessionKey;

    m_regionDutyGrid.SetItem(&gridItem);
    
    // Highlight the current session in a different colour.
    if (m_sessionCache.getCurrentSessionId() == sessionInfoEx.SessionId)
    {
        for (int column = 0; column < m_regionDutyGrid.GetColumnCount(); column++)
        {
            m_regionDutyGrid.SetItemFgColour(row, column, CURRENT_SESSION_COLOUR); 
        }
    }
    
    // Operator login or profile change may affect operator sort order.
    m_regionDutyGrid.SortItems(GridAssistant::operatorCompareProc, OPERATOR_COLUMN, TRUE);

    // Request a redraw of the grid for the changes to become visible.
    m_regionDutyGrid.Invalidate();
}


void RegionDutyPropertyPage::updateOperator(int row, const TA_Base_Bus::SessionId& sessionId)
{
    updateOperator(row, m_sessionCache.getSessionInfo(sessionId));
}


void RegionDutyPropertyPage::removeOperator(int row)
{
    TA_ASSERT(row > REGION_ROW && row < m_regionDutyGrid.GetRowCount(), "Row index out of bounds");

    m_regionDutyGrid.DeleteRow(row);
    m_regionDutyGrid.Invalidate();
}


// ISessionObserver interface
void RegionDutyPropertyPage::sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate)
{
    // Let the thread that created the window process the update.
    HWND wndHandle(GetSafeHwnd());

    if (wndHandle != NULL)
    {
        // Post the message so as not to hold up the notifying thread.
        // Clone the session update and throw onto the heap to make sure 
        // that by the time the message is processed we still have a valid
        // object. The message handler takes care of destroying it once consumed.
        ::PostMessage(wndHandle, WM_USER_RECEIVE_SESSION_UPDATE, 
                      reinterpret_cast<WPARAM>(new SessionUpdate(sessionUpdate)), 0);
    }
}


// IRegionDutyObserver interface
void RegionDutyPropertyPage::dutyChanged(const TA_Base_Bus::RegionDutyUpdate& dutyUpdate)
{
    // Let the thread that created the window process the update.
    HWND wndHandle(GetSafeHwnd());

    if (wndHandle != NULL)
    {
        // Post the message so as not to hold up the notifying thread.
        // Clone the duty update and throw onto the heap to make sure 
        // that by the time the message is processed we still have a valid
        // object. The message handler takes care of destroying it once consumed.
        ::PostMessage(wndHandle, WM_USER_RECEIVE_RGN_DUTY_UPDATE, 
                      reinterpret_cast<WPARAM>(new RegionDutyUpdate(dutyUpdate)), 0);
    }
}


void RegionDutyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(RegionDutyPropertyPage)
	DDX_Control(pDX, IDC_TRANSFER_RGN_DUTY_BTN, m_transferRegionDutyBtn);
	DDX_Control(pDX, IDC_DELEGATE_SUB_DUTY_BTN, m_delegateSubsystemDutyBtn);
	DDX_GridControl(pDX, IDC_RGN_DUTY_GRID, m_regionDutyGrid);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// RegionDutyPropertyPage message handlers

BEGIN_MESSAGE_MAP(RegionDutyPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(RegionDutyPropertyPage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_TRANSFER_RGN_DUTY_BTN, OnTransferRegionDutyBtn)
	ON_BN_CLICKED(IDC_DELEGATE_SUB_DUTY_BTN, OnDelegateSubDutyBtn)
    ON_MESSAGE(WM_USER_RECEIVE_SESSION_UPDATE, OnReceiveSessionUpdate)
    ON_MESSAGE(WM_USER_RECEIVE_RGN_DUTY_UPDATE, OnReceiveRegionDutyUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL RegionDutyPropertyPage::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog()");

	CPropertyPage::OnInitDialog();
	
    if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USELOCATIONLABEL)) 
    {
        m_transferRegionDutyBtn.SetWindowText(_T("Take location duty"));

		// Due to [MW-181] on TAR0514_C955_Mysql_Win7_Issuelist.xls, need to hide the "Take Location Button"
		std::string showSessionlocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_SHOWSESSIONWORKSTATION).c_str();
		if ( showSessionlocation.compare("OPEN") != 0 )
		{
			m_transferRegionDutyBtn.ShowWindow(SW_HIDE);
		}
	}

    //TD17082++ - initialization of regiondutygrid will be called right after dutydatastore has been initialized
    //initialiseRegionDutyGrid();
    enableDutyControlButtons();
  
    FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegionDutyPropertyPage::OnTransferRegionDutyBtn() 
{
    try
	{
		if ( TA_Base_App::DutyManagerDlg::m_isMartixDataInited == true )
		{
			CWaitCursor waitCursor;

			// Activate region duty transfer dialog to enable operator to take duties.
			RegionDutyTransferDlg rgnDutyXferDlg;
			rgnDutyXferDlg.DoModal();
		}
		else
		{
			std::stringstream messageInfo;
			messageInfo << "RightsManager is populating data, please try again later!";
			MessageBox::info(messageInfo.str());
		}
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
		std::string exStr(ex.what());
		size_t found;
		found=exStr.find("TRANSIENT_CallTimedout");
		if(found == std::string::npos)
		{
			MessageBox::error("Error while activating duty transfer dialog.", ex);
		}
		else
		{
			MessageBox::warning("Servers are busy, Please retry later.");
		}
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        MessageBox::error("Error while activating duty transfer dialog.", ex);
    }
    catch (...)
    {
        MessageBox::error("Error while activating duty transfer dialog.");
    }
}


void RegionDutyPropertyPage::OnDelegateSubDutyBtn() 
{
	// TD14164 azenitha++
	TA_Base_Bus::TransActiveMessage userMsg;
	CString errMsg = userMsg.constructMessage(IDS_UW_220001);
	// TD14164 ++azenitha

	if ( TA_Base_App::DutyManagerDlg::m_isMartixDataInited == true )
	{
		// TD14164 azenitha++
		/*if (MessageBox("Are you sure that you wish to release\n"
		"control from all central operators?", 
		"System-wide delegation",
		MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES)*/
		if (MessageBox(errMsg,
			"System-wide delegation",
			MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2) == IDYES)
			// TD14164 ++azenitha
		{
			try
			{
				CWaitCursor waitCursor;

				m_dutyAgent.degradeAllSubsystems(m_sessionCache.getCurrentSessionId());
			}
			catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
				MessageBox::error("Error while releasing control from central operators.", ex);
			}
		}
	}
	else
	{
		std::stringstream messageInfo;
		messageInfo << "RightsManager is populating data, please try again later!";
		MessageBox::info(messageInfo.str());
	}


}


LRESULT RegionDutyPropertyPage::OnReceiveSessionUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    // Make sure the session update gets destroyed when we leave here.
    std::auto_ptr<SessionUpdate> sessionUpdate(reinterpret_cast<SessionUpdate*>(wParam));

	if(!g_populated) return 0;

    try
    {
        int row(GridAssistant::findGridRow(m_regionDutyGrid, sessionUpdate->sessionKey));

        switch (sessionUpdate->updateType)
        {
            case TA_Base_Bus::TA_Security::SessionStart:
                if (row == GridAssistant::INVALID_GRID_ROW)
                {
                    addOperator(sessionUpdate->sessionId);
                }
                break;
                               
            case TA_Base_Bus::TA_Security::ProfileUpdate:
                if (row != GridAssistant::INVALID_GRID_ROW)
                {
                    updateOperator(row, sessionUpdate->sessionId);                    
                }
                break;

            case TA_Base_Bus::TA_Security::SessionEnd:
                if (row != GridAssistant::INVALID_GRID_ROW)
                {
                    removeOperator(row);
                }
                break;

            default:
                return 0;
        }

        // Session changes involving the current session may affect the current operators 
        // access rights, so the assign/unassign buttons' enable state needs to be reevaluated.
        if (sessionUpdate->sessionId == m_sessionCache.getCurrentSessionId())
        {
            enableDutyControlButtons();
        }
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        //MessageBox::error("Error while processing session update.", ex);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing session update");
		LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
        //MessageBox::error("Error while processing session update.", ex);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing session update");
		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
    }
    catch (...)
    {
        //MessageBox::error("Error while processing session update.");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing session update");
    }

    return 0;
}


LRESULT RegionDutyPropertyPage::OnReceiveRegionDutyUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    // Make sure the duty update gets destroyed when we leave here.
    std::auto_ptr<RegionDutyUpdate> dutyUpdate(reinterpret_cast<RegionDutyUpdate*>(wParam));

    for (RegionDutyList::const_iterator rgnDutyIt = dutyUpdate->duties.begin(); rgnDutyIt != dutyUpdate->duties.end(); rgnDutyIt++)
    {
        try
        {
            SessionInfoEx sessionInfoEx(m_sessionCache.getSessionInfo(rgnDutyIt->session.in()));

            // Get the row index corresponding to the session affected by duty change.
            // Add the operator if necessary.
            int row(GridAssistant::findGridRow(m_regionDutyGrid, sessionInfoEx.sessionKey));

            if (row == GridAssistant::INVALID_GRID_ROW)
            {
				// Yanrong++ TD20340
				// Shouldn't add the operator here otherwise the current session will not highlighted correctly 
				// in a different color. We should do this operation when session update after the current
				// sessionId has been updated. 
				// See RegionDutyPropertyPage::OnReceiveSessionUpdate(...).
                //addOperator(sessionInfoEx);
				// ++Yanrong
                continue;
            }
        
            // Get the column index corresponding to the region affected by duty change.
            // Ignore the update if no such column is found.
            int column(GridAssistant::findGridColumn(m_regionDutyGrid, rgnDutyIt->regionKey));  
        
            if (column == GridAssistant::INVALID_GRID_COLUMN)
            {
                continue;
            }

            // Update the relevant grid cell. 
            std::string profile(GridAssistant::generateProfileString(sessionInfoEx, rgnDutyIt->regionKey));
            m_regionDutyGrid.SetItemText(row, column, profile.c_str());
            m_regionDutyGrid.RedrawCell(row, column);
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            //MessageBox::error("Error while processing region duty update.", ex);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing region duty update");
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
        }
        catch (const TA_Base_Core::TransactiveException& ex)
        {
            //MessageBox::error("Error while processing region duty update.", ex);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing region duty update");
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch (...)
        {
            //MessageBox::error("Error while processing region duty update.");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing region duty update");
        }        
    }

    return 0;
}


void RegionDutyPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	if (::IsWindow(m_regionDutyGrid.GetSafeHwnd()))    
    {
        CRect delegateSubDutyBtnRect; 

        m_delegateSubsystemDutyBtn.GetWindowRect(delegateSubDutyBtnRect);
        ScreenToClient(delegateSubDutyBtnRect);
        int buttonWidth(delegateSubDutyBtnRect.Width());
        int buttonHeight(delegateSubDutyBtnRect.Height()); 
        delegateSubDutyBtnRect.right = cx - 8;
        delegateSubDutyBtnRect.left = delegateSubDutyBtnRect.right - buttonWidth;
        delegateSubDutyBtnRect.bottom = cy - 8;
        delegateSubDutyBtnRect.top = delegateSubDutyBtnRect.bottom - buttonHeight;
        m_delegateSubsystemDutyBtn.MoveWindow(delegateSubDutyBtnRect);
		m_delegateSubsystemDutyBtn.Invalidate();

        CRect xferRgnDutyBtnRect;

        m_transferRegionDutyBtn.GetWindowRect(xferRgnDutyBtnRect);
        ScreenToClient(xferRgnDutyBtnRect);
        xferRgnDutyBtnRect.right = delegateSubDutyBtnRect.left - 10;
        xferRgnDutyBtnRect.left = xferRgnDutyBtnRect.right - buttonWidth;
        xferRgnDutyBtnRect.bottom = delegateSubDutyBtnRect.bottom;
        xferRgnDutyBtnRect.top = delegateSubDutyBtnRect.top;
        m_transferRegionDutyBtn.MoveWindow(xferRgnDutyBtnRect);
        m_transferRegionDutyBtn.Invalidate();
		// Due to [MW-181] on TAR0514_C955_Mysql_Win7_Issuelist.xls, need to hide the "Take Location Button"
		std::string showSessionlocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_SHOWSESSIONWORKSTATION).c_str();
		if ( showSessionlocation.compare("OPEN") != 0 )
		{
			m_transferRegionDutyBtn.ShowWindow(SW_HIDE);
		}

        CRect gridRect;

        m_regionDutyGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = delegateSubDutyBtnRect.top - 8;
        m_regionDutyGrid.MoveWindow(gridRect); 
		//m_regionDutyGrid.Invalidate();
    }
}

void RegionDutyPropertyPage::redrawScrollBar()
{
	m_regionDutyGrid.ShowScrollBar(SB_HORZ,TRUE);
}
