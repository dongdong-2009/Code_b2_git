// OtherSesSubDutyPropertyPage.cpp : implementation file
//

//#include "stdafx.h"
//#include "dutymanager.h"
//#include "OtherSesSubDutyPropertyPage.h"
#include "stdafx.h"
#include "app/security/DutyManager/src/DutyManager.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD15569

// Use boost tokenizer for splitting up RGB values.
#include "boost/tokenizer.hpp"

#include "core/data_access_interface/src/RightsAccessFactory.h" //TD15533
#include "core/data_access_interface/src/SessionAccessFactory.h" //TD15533
#include "core/data_access_interface/src/ProfileAccessFactory.h" //TD15533

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
namespace
{
    const unsigned int CHECK_SESSION_IN_DB_RETRY_TIMES = 3;
	const CString SUBSYSTEM_COLUMN_HEADING("Subsystem");
    const int FIXED_COLUMN_COUNT(1);
    const int FIXED_ROW_COUNT(1);
    const int SUBSYSTEM_COLUMN(0);
    const int REGION_ROW(0);
	
    const int DEFAULT_ROW_HEIGHT(20);
    const int DEFAULT_COLUMN_WIDTH(70);
	
    const int ALLOCATED_SUBSYSTEM(1);
    const int UNALLOCATED_SUBSYSTEM(~ALLOCATED_SUBSYSTEM);
	
    const COLORREF NORMAL_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF DELEGATED_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF DEGRADED_DUTY_COLOUR(CLR_DEFAULT); // white
    const COLORREF UNALLOCATED_SUBSYSTEM_COLOUR( RGB( 225, 225, 225 ) ); // light grey
	
	const int OCC_LOCATION = 1;
	const char* TCS_TRAIN_COMMS_STN_SUBSYSTEM = "TCS - Train Comms (Stn)";
	const char* PROFILE_GROUP_TCO = "TCO";
	const char* TCS_SPECIAL_PROJECT_NAME = "4669"; //
	
	const unsigned long CENTRAL_PROFILE_TYPE(1); //1 is central profile //TD15533
	const unsigned long NO_ACTION_GROUP(0xFFFFFFFF); //TD15533
	const TA_Base_Bus::LocationKey ALL_LOCATION(0); //0 is all location if central profile //TD15533
}

using TA_Base_Bus::SessionCache;
using TA_Base_Bus::SubsystemDutyMatrixElement;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::SubsystemKey;
using TA_Base_Bus::SubsystemKeyList;
using TA_Base_Bus::SubsystemDutyUpdate;
using TA_Base_Bus::SubsystemDutyList;
using TA_Base_Bus::SubsystemStateList;
using TA_Base_Core::DebugUtil;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

IMPLEMENT_DYNCREATE(OtherSesSubDutyPropertyPage, CPropertyPage)

OtherSesSubDutyPropertyPage::OtherSesSubDutyPropertyPage() 
	: CPropertyPage(OtherSesSubDutyPropertyPage::IDD),
	ISessionObserver(DutyDataStore::getSessionCache()),
	ISubsystemDutyObserver(DutyDataStore::getSubsystemDutyMatrix()),
	m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache()),
	m_regionDutyMatrix(DutyDataStore::getRegionDutyMatrix()),
    m_subsystemDutyMatrix(DutyDataStore::getSubsystemDutyMatrix()),
    m_currentSessionInfo(m_sessionCache.getSessionInfo(m_sessionCache.getCurrentSessionId())),
	m_selectSesSubDutyDlg(NULL),
	m_isShowAllSubsystems(false)
{
	//{{AFX_DATA_INIT(OtherSesSubDutyPropertyPage)
	//}}AFX_DATA_INIT
}

OtherSesSubDutyPropertyPage::~OtherSesSubDutyPropertyPage()
{
	FUNCTION_ENTRY("~OtherSesSubDutyPropertyPage()");
	m_actionGroupsDB.clear();

	if (m_selectSesSubDutyDlg != NULL)
	{
		delete m_selectSesSubDutyDlg;
		m_selectSesSubDutyDlg = NULL;
	}
	//DutyDataStore::cleanUp();
    FUNCTION_EXIT;
}

void OtherSesSubDutyPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OtherSesSubDutyPropertyPage)
	DDX_Control(pDX, IDC_SELECT_SESSION_INFO_EDIT, m_selectedSessionEdit);
	DDX_Control(pDX, IDC_SHOW_ALL_SUBS_CHECK_BTN, m_showAllSubsystemsOtherSesBtn);
	DDX_Control(pDX, IDC_RELOAD_SES_SUB_BTN, m_ReloadDataBtn);
	DDX_Control(pDX, IDC_SEL_SES_BTN, m_SelectSesBtn);
	DDX_GridControl(pDX, IDC_OTHER_SES_SUB_GRID, m_sesSubDutyGrid);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OtherSesSubDutyPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(OtherSesSubDutyPropertyPage)
	ON_WM_SIZE()
	ON_MESSAGE(WM_OTHER_SES_SUB_DUTY_UPDATE,OnUpdateOtherSesSubDutyMessage)
	ON_MESSAGE(WM_USER_RECEIVE_SUB_DUTY_UPDATE, OnReceiveOhterSesSubDutyUpdate)
	ON_MESSAGE(WM_USER_RECEIVE_SESSION_UPDATE, OnReceiveSessionUpdate)
	ON_BN_CLICKED(IDC_SEL_SES_BTN, OnSelectSessionBtn)
	ON_BN_CLICKED(IDC_RELOAD_SES_SUB_BTN, OnReloadSesSubBtn)
	ON_BN_CLICKED(IDC_SHOW_ALL_SUBS_CHECK_BTN, OnShowAllSubsCheckBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OtherSesSubDutyPropertyPage message handlers

BOOL OtherSesSubDutyPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// TODO: Add extra initialization here
	enableButtonsAndSessionInfoEdit(false);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void OtherSesSubDutyPropertyPage::OnSize(UINT nType, int cx, int cy) 
{
	CPropertyPage::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (::IsWindow(m_sesSubDutyGrid.GetSafeHwnd()))    
    {
        CRect reloadSelSesSubDutyBtnRect; 
		
        m_ReloadDataBtn.GetWindowRect(reloadSelSesSubDutyBtnRect);
        ScreenToClient(reloadSelSesSubDutyBtnRect);
        int buttonWidth(reloadSelSesSubDutyBtnRect.Width());
        int buttonHeight(reloadSelSesSubDutyBtnRect.Height()); 
        reloadSelSesSubDutyBtnRect.right = cx - 8;
        reloadSelSesSubDutyBtnRect.left = reloadSelSesSubDutyBtnRect.right - buttonWidth;
        reloadSelSesSubDutyBtnRect.bottom = cy - 8;
        reloadSelSesSubDutyBtnRect.top = reloadSelSesSubDutyBtnRect.bottom - buttonHeight;
        m_ReloadDataBtn.MoveWindow(reloadSelSesSubDutyBtnRect);
		m_ReloadDataBtn.Invalidate();
		
        CRect selectSessionSubDutyBtnRect;
		
        m_SelectSesBtn.GetWindowRect(selectSessionSubDutyBtnRect);
        ScreenToClient(selectSessionSubDutyBtnRect);
        selectSessionSubDutyBtnRect.right = reloadSelSesSubDutyBtnRect.left - 10;
        selectSessionSubDutyBtnRect.left = selectSessionSubDutyBtnRect.right - buttonWidth;
        selectSessionSubDutyBtnRect.bottom = reloadSelSesSubDutyBtnRect.bottom;
        selectSessionSubDutyBtnRect.top = reloadSelSesSubDutyBtnRect.top;
        m_SelectSesBtn.MoveWindow(selectSessionSubDutyBtnRect);
        m_SelectSesBtn.Invalidate();
		
		CRect showAllSubsBtnRect;
		
        m_showAllSubsystemsOtherSesBtn.GetWindowRect(showAllSubsBtnRect);
        ScreenToClient(showAllSubsBtnRect);
        showAllSubsBtnRect.top = selectSessionSubDutyBtnRect.top;
        showAllSubsBtnRect.bottom = selectSessionSubDutyBtnRect.bottom;
        m_showAllSubsystemsOtherSesBtn.MoveWindow(showAllSubsBtnRect);
		m_showAllSubsystemsOtherSesBtn.Invalidate();

		CRect selectedSessionInfoRect;
		m_selectedSessionEdit.GetWindowRect(selectedSessionInfoRect);
		ScreenToClient(selectedSessionInfoRect);
		selectedSessionInfoRect.right = cx - 8;
		selectedSessionInfoRect.bottom = reloadSelSesSubDutyBtnRect.top - 8;
		selectedSessionInfoRect.top = selectedSessionInfoRect.bottom - 32;
		m_selectedSessionEdit.MoveWindow(selectedSessionInfoRect);
		m_selectedSessionEdit.Invalidate();

        CRect gridRect;
		
        m_sesSubDutyGrid.GetWindowRect(gridRect);
        ScreenToClient(gridRect);
        gridRect.right = cx - 8;
        gridRect.bottom = selectedSessionInfoRect.top - 8;
        m_sesSubDutyGrid.MoveWindow(gridRect);
        m_sesSubDutyGrid.Invalidate();
	}
}

void OtherSesSubDutyPropertyPage::initialiseOhterSessionDutyGrid()
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::initialiseOtherSesSubDutyGrid");
    // Initialise the colours to used when rendering the grid.
    m_normalDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_NORMALDUTYCOLOUR ),
		NORMAL_DUTY_COLOUR );
    m_delegatedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DELEGATEDDUTYCOLOUR ),
		DELEGATED_DUTY_COLOUR );
    m_degradedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DEGRADEDDUTYCOLOUR ),
		DEGRADED_DUTY_COLOUR );
    m_unallocatedSubsystemColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR ),
		UNALLOCATED_SUBSYSTEM_COLOUR );
	
    // By default only those subsystems are shown which the session currently has access to.
    SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));
    RegionKeyList regionKeys(m_subsystemDutyMatrix.getColumnIds());
	
    m_sesSubDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_sesSubDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_sesSubDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_sesSubDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_sesSubDutyGrid.SetListMode(TRUE);
    m_sesSubDutyGrid.EnableTitleTips(TRUE);
    m_sesSubDutyGrid.EnableSelection(FALSE);
    m_sesSubDutyGrid.EnableDragAndDrop(FALSE);
    m_sesSubDutyGrid.SetEditable(FALSE);
    m_sesSubDutyGrid.SetHeaderSort(FALSE);
	
    m_sesSubDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_sesSubDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for subsystem names
    m_sesSubDutyGrid.SetRowCount(m_sesSubDutyGrid.GetFixedRowCount());  // this will grow as subsystems are added to the grid
    m_sesSubDutyGrid.SetColumnCount(m_sesSubDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
    m_sesSubDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
    // Set the fixed column labels.
	m_sesSubDutyGrid.SetItemText(REGION_ROW, SUBSYSTEM_COLUMN, SUBSYSTEM_COLUMN_HEADING);

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::initialiseOtherSesSubDutyGrid");
}

void OtherSesSubDutyPropertyPage::OnShowAllSubsCheckBtn() 
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::OnShowAllSubsCheckBtn");

	// TODO: Add your control notification handler code here
	CWaitCursor waitCursor;
	
    SubsystemKeyList allSubsystemKeys(m_subsystemDutyMatrix.getRowIds());
    SubsystemKeyList accessibleSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_selectSessionInfo.ProfileId));
	
    // When the Show All Subsystems button is released we go from the "all subsystems"
    // view to the "subsystems based on access rights" view, and vice versa.
    if (m_showAllSubsystemsOtherSesBtn.GetCheck() == BST_UNCHECKED)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::OnShowAllSubsCheckBtn, m_showAllSubsystemsOtherSesBtn.GetCheck() == BST_UNCHECKED");

		m_isShowAllSubsystems = false;
        updateSubsystems(allSubsystemKeys, accessibleSubsystemKeys, m_selectSessionInfo.ProfileId); //TD15533
    }
    else
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::OnShowAllSubsCheckBtn, m_showAllSubsystemsOtherSesBtn.GetCheck() == BST_CHECKED");

		m_isShowAllSubsystems = true;
        updateSubsystems(accessibleSubsystemKeys, allSubsystemKeys, m_selectSessionInfo.ProfileId); //TD15533
    }
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::OnShowAllSubsCheckBtn");
}

void OtherSesSubDutyPropertyPage::OnSelectSessionBtn() 
{
	// TODO: Add your control notification handler code here
	try
    {
        CWaitCursor waitCursor;
		
        // Activate Select Session Subsystem Duty dialog to enable operator to select other session's subsystem duty.
		
		//SelectSesSubDutyDlg selectSesSubDutyDlg;
		if (m_selectSesSubDutyDlg == NULL)
		{
			m_selectSesSubDutyDlg = new SelectSesSubDutyDlg;
		}

		if (m_selectSesSubDutyDlg->DoModal() == IDOK)
		{
			updateOtherSesSubDrid(m_selectSesSubDutyDlg->getSelectedSessionInfo());

			enableButtonsAndSessionInfoEdit(true);
		}
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while activating Select Session Subsystem Duty dialog. %s",ex.what());
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while activating Select Session Subsystem Duty dialog. %s",ex.what());
    }
    catch (...)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while activating Select Session Subsystem Duty dialog.");
	}

}

void OtherSesSubDutyPropertyPage::OnReloadSesSubBtn() 
{
	// TODO: Add your control notification handler code here
	try
    {
        CWaitCursor waitCursor;
		
		if ( m_selectSessionInfo.SessionId != "")
		{
			updateOtherSesSubDrid(m_selectSessionInfo);
		}
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while reload select Session Duty Info to Grid. %s",ex.what());
    }
    catch (...)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while reload select Session Duty Info to Grid.");
	}
}

COLORREF OtherSesSubDutyPropertyPage::convertColourStrToColorref( const std::string& colourStr,
															   const COLORREF& defaultColour ) const
{
    FUNCTION_ENTRY("convertColourStrToColorref");
	
    COLORREF colour = defaultColour;
	
    if ( !colourStr.empty() )
    {
        // Start the conversion.
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
		
        // Variables required for string->integer conversion.
        std::stringstream tempStream;
        unsigned long tempULong;
        unsigned char colourIndex = 0;
        unsigned char colourChannels[3];
		
        // Prepare the tokenizer and iterator.
        boost::char_separator<char> lineSep( "," );
        boost::tokenizer< boost::char_separator<char> > channelTokens( colourStr, lineSep );
		
        // For each channel...
        boost::tokenizer< boost::char_separator<char> >::iterator it;
        for ( it=channelTokens.begin(); ( it!=channelTokens.end() ) && ( 3>colourIndex ); it++ )
        {
            // Prepare the string stream for conversion.
            tempStream.str( *it );
            tempStream.clear();
			
            // Convert the channel to an integer and add it to the array of colour channel information if
            // the conversion successed.
            if ( !( tempStream >> tempULong ).fail() && ( 256 > tempULong ) )
            {
                colourChannels[colourIndex++] = static_cast<unsigned char>(tempULong);
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while tokenizing RGB value %s, %s is not a valid number for an 8-bit colour channel.",
                    colourStr.c_str(), (*it).c_str() );
                return defaultColour;
            }
        }
		
        // Create the colour.
        colour = RGB( colourChannels[0], colourChannels[1], colourChannels[2] );
    }
    else
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Colour string was empty, using default." );
    }
	
    FUNCTION_EXIT;
    return colour;
}

LRESULT OtherSesSubDutyPropertyPage::OnUpdateOtherSesSubDutyMessage(WPARAM wParam, LPARAM lParam)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::OnUpdateOtherSesSubDutyMessage");

	SessionInfoEx* sesInfEx = reinterpret_cast<SessionInfoEx*>(wParam);

	updateOtherSesSubDrid(*sesInfEx);
/*
	if ((!sesInfEx->OperatorId.empty()) && (!sesInfEx->ProfileId.empty()))
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
			"in OtherSesSubDutyPropertyPage::OnUpdateOtherSesSubDutyMessage, after reinterpret_cast SessionInfoEx, the SessionInfoEx in the parameter is: OperatorId=%ld, ProfileId=%ld, LocationId=%ld, WorkStationId=%ld",
			sesInfEx->OperatorId[0],sesInfEx->ProfileId[0],sesInfEx->LocationId,sesInfEx->WorkstationId);

		{
			m_selectSessionInfo.LocationId = sesInfEx->LocationId;
			m_selectSessionInfo.OperatorId = sesInfEx->OperatorId;
			m_selectSessionInfo.ProfileId = sesInfEx->ProfileId;
			m_selectSessionInfo.SessionId = sesInfEx->SessionId;
			m_selectSessionInfo.sessionKey = sesInfEx->sessionKey;
			m_selectSessionInfo.UserId = sesInfEx->UserId;
			m_selectSessionInfo.WorkstationId = sesInfEx->WorkstationId;
		}
	}

	    // Initialise the colours to used when rendering the grid.
    m_normalDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_NORMALDUTYCOLOUR ),
                                                     NORMAL_DUTY_COLOUR );
    m_delegatedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DELEGATEDDUTYCOLOUR ),
                                                     DELEGATED_DUTY_COLOUR );
    m_degradedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DEGRADEDDUTYCOLOUR ),
                                                     DEGRADED_DUTY_COLOUR );
    m_unallocatedSubsystemColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR ),
                                                     UNALLOCATED_SUBSYSTEM_COLOUR );

    // By default only those subsystems are shown which the session currently has access to.
    //SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));
	SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_selectSessionInfo.ProfileId));
    RegionKeyList regionKeys(m_subsystemDutyMatrix.getColumnIds());

    m_sesSubDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_sesSubDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_sesSubDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_sesSubDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_sesSubDutyGrid.SetListMode(TRUE);
    m_sesSubDutyGrid.EnableTitleTips(TRUE);
    m_sesSubDutyGrid.EnableSelection(FALSE);
    m_sesSubDutyGrid.EnableDragAndDrop(FALSE);
    m_sesSubDutyGrid.SetEditable(FALSE);
    m_sesSubDutyGrid.SetHeaderSort(FALSE);

    m_sesSubDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_sesSubDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for subsystem names
    m_sesSubDutyGrid.SetRowCount(m_sesSubDutyGrid.GetFixedRowCount());  // this will grow as subsystems are added to the grid
    m_sesSubDutyGrid.SetColumnCount(m_sesSubDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
    m_sesSubDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
    // Set the fixed column labels.
    m_sesSubDutyGrid.SetItemText(REGION_ROW, SUBSYSTEM_COLUMN, SUBSYSTEM_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(SUBSYSTEM_COLUMN + 1);
    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = REGION_ROW;
            gridItem.col = column;
            gridItem.szText = m_dataCache.getRegionName(*regionIt).c_str();
            gridItem.lParam = *regionIt;
			if(0 == gridItem.szText.CompareNoCase("All Locations"))
			{
				m_sesSubDutyGrid.DeleteColumn(column);
				continue;
			}
            m_sesSubDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_sesSubDutyGrid.DeleteColumn(m_sesSubDutyGrid.GetColumnCount() - 1);

            MessageBox::error("Error while adding region to subsystem duty grid.", ex);
        }
    }

	//TD15533++
	TA_Base_Bus::RegionKeyList regionKeysAccessible; 
	unsigned int retryTime = 0;
	//while (true) //if session not persisted in DB, retry
	while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		try
		{
			++retryTime;
			regionKeysAccessible = getLocations(); 
			break;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
		}
	}
	
	if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "initialiseSubsystemDutyGrid(): After retry %d times, Still no Session persisted in DB",
			CHECK_SESSION_IN_DB_RETRY_TIMES);
		
		CString errMsg = "During initialise Subsystem DutyGrid, no session persisted in DB";
		if (MessageBox(errMsg,
                   "Still failed after retry",
                   MB_ICONWARNING | MB_OK | MB_DEFBUTTON2) == IDOK)
		{
			throw;
		}
	}
	//++TD15533

    // Add individual subsystems to the grid. Subsystem names are used as entries in the first column.
    for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
    {
       addSubsystem(*subsystemIt, m_selectSessionInfo.ProfileId, regionKeysAccessible); //TD15533
    }

    // Sort subsystems by name.
    m_sesSubDutyGrid.SortTextItems(SUBSYSTEM_COLUMN, TRUE);

    // Size subsystem column width to fit subsystem names.
    m_sesSubDutyGrid.AutoSizeColumn(SUBSYSTEM_COLUMN);
*/
	delete sesInfEx;
	sesInfEx = NULL;

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::OnUpdateOtherSesSubDutyMessage");

	return 0;
}

void OtherSesSubDutyPropertyPage::addSubsystem(
	TA_Base_Bus::SubsystemKey subsystemKey, 
	TA_Base_Bus::ProfileKeyList profileKeys,
	TA_Base_Bus::RegionKeyList regionKeys) //TD15533
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::addSubsystem");

    // Append the subsystem to the end of the grid
    int row(m_sesSubDutyGrid.GetRowCount());

    m_sesSubDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_sesSubDutyGrid.SetRowCount(row + 1); // unlike InsertRow(), this will use the default row height
    
    try
    {
        // Set subsystem details.
        GV_ITEM gridItem;
        gridItem.mask = GVIF_TEXT | GVIF_PARAM;
        gridItem.row = row;
        gridItem.col = SUBSYSTEM_COLUMN;
        gridItem.szText = m_dataCache.getSubsystemName(subsystemKey).c_str();
        gridItem.lParam = subsystemKey;

        m_sesSubDutyGrid.SetItem(&gridItem);

        // Where a subsystem is not part of a region, grey out the corresponding grid cell.
        for (gridItem.col = SUBSYSTEM_COLUMN + 1; gridItem.col < m_sesSubDutyGrid.GetColumnCount(); gridItem.col++)
        {
            RegionKey regionKey(m_sesSubDutyGrid.GetItemData(REGION_ROW, gridItem.col));

			//TD15533++ 

			const SubsystemDutyMatrixElement& sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
			//m_subsystemDutyMatrix(DutyDataStore::getSubsystemDutyMatrix()),
			//TA_Base_Bus::SubsystemDutyMatrix& m_subsystemDutyMatrix;
			

            gridItem.mask = GVIF_PARAM;
            gridItem.lParam = (sdmElement.isSubsystemInRegion()? ALLOCATED_SUBSYSTEM : UNALLOCATED_SUBSYSTEM);

			if ( sdmElement.isSubsystemInRegion() == ALLOCATED_SUBSYSTEM )
			{
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::addSubsystem -- ALLOCATED_SUBSYSTEM");
			}
			else
			{
				LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::addSubsystem -- UNALLOCATED_SUBSYSTEM");
			}

			
			if (!isRegionAccessible(regionKey, regionKeys) ||  //requirement #4
				!isSubsystemAccessible(profileKeys, subsystemKey) || //requirement #2
				!sdmElement.isSubsystemInRegion()) 
			{	
				//force to gray cell if not accessible or not in region
				gridItem.mask |= GVIF_BKCLR;
				gridItem.crBkClr = determineCellBkColour( false, sdmElement.getSubsystemState() );

				gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
				gridItem.iImage = -1;
				gridItem.szText = "";
				gridItem.iImageS = -1;
				gridItem.szTextS = "";
				gridItem.bOverrided = false;

				m_sesSubDutyGrid.SetItem(&gridItem);
				m_sesSubDutyGrid.SetItemS(&gridItem); 

				continue;
			}

            try
            {
				//TD15533++
				//std::string actionGrp(GridAssistant::generateActionGroupString(m_currentSessionInfo, subsystemKey, regionKey));
				//int image(actionGrp.empty()? GridAssistant::GI_CROSS : GridAssistant::GI_TICK);

				//gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
				//gridItem.iImage = image;
				//gridItem.szText = actionGrp.c_str();
				//++TD15533

				TA_Base_App::CellData cellData = generateCellData(subsystemKey, regionKey, sdmElement.getSubsystemState());

				if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					gridItem.mask |= GVIF_BKCLR;
					gridItem.crBkClr = determineCellBkColour( false, sdmElement.getSubsystemState() );

					gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
					gridItem.iImage = -1;
					gridItem.szText = "";
					gridItem.iImageS = -1;
					gridItem.szTextS = "";
					gridItem.bOverrided = false;
				}
				else
				{
					gridItem.mask |= GVIF_BKCLR;
					gridItem.crBkClr = determineCellBkColour( sdmElement.isSubsystemInRegion(), sdmElement.getSubsystemState() );

					gridItem.mask |= GVIF_IMAGE | GVIF_TEXT;
					gridItem.iImage = cellData.baseImage;
					gridItem.szText = cellData.baseText.c_str();
					gridItem.iImageS = cellData.overrideImage;
					gridItem.szTextS = cellData.overrideText.c_str();
					gridItem.bOverrided = cellData.isOverrided;
				}

                m_sesSubDutyGrid.SetItem(&gridItem);
				m_sesSubDutyGrid.SetItemS(&gridItem); 
            }
            catch (const TA_Base_Core::MatrixException& ex)
            {
                std::ostringstream msg;
                msg << "Error while setting subsystem duty grid cell for" << std::endl
                    << "subsystem " << m_dataCache.getSubsystemName(subsystemKey)
                    << " and region " << m_dataCache.getRegionDisplayName(regionKey);

                MessageBox::error(msg.str(), ex);
            }

			//TD15533
        }
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        // Get rid of the newly added row.
        m_sesSubDutyGrid.DeleteRow(row);

        MessageBox::error("Error while adding subsystem to subsystem duty grid.", ex);
    }
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::addSubsystem");
}

void OtherSesSubDutyPropertyPage::removeSubsystem(TA_Base_Bus::SubsystemKey subsystemKey)
{
    int row(GridAssistant::findGridRow(m_sesSubDutyGrid, subsystemKey));
	
    if (row >= 0)
    {
        m_sesSubDutyGrid.DeleteRow(row);
        m_sesSubDutyGrid.Invalidate();
    }
}


void OtherSesSubDutyPropertyPage::updateSubsystems(TA_Base_Bus::SubsystemKeyList oldSubsystemKeys,
												   TA_Base_Bus::SubsystemKeyList newSubsystemKeys, 
												   TA_Base_Bus::ProfileKeyList profileKeys) 
{
    SubsystemKeyList affectedSubsystemKeys;
	
    std::sort(oldSubsystemKeys.begin(), oldSubsystemKeys.end());
    std::sort(newSubsystemKeys.begin(), newSubsystemKeys.end());
	
    if (oldSubsystemKeys.size() > newSubsystemKeys.size())
    {
        // The new subsystem list is shorter than the old one - remove from view all subsystems  
        // in the old list not found in the new list.
        std::set_difference(oldSubsystemKeys.begin(), oldSubsystemKeys.end(),
			newSubsystemKeys.begin(), newSubsystemKeys.end(),
			std::inserter(affectedSubsystemKeys, affectedSubsystemKeys.begin()));
		
        for (SubsystemKeyList::const_iterator subKeyIt = affectedSubsystemKeys.begin(); subKeyIt != affectedSubsystemKeys.end(); subKeyIt++)
        {
            removeSubsystem(*subKeyIt);
        }
    }
    else
    {
        // The new subsystem list is longer than the old one - add to view all subsystems  
        // in the new list not found in the old list.
        std::set_difference(newSubsystemKeys.begin(), newSubsystemKeys.end(),
			oldSubsystemKeys.begin(), oldSubsystemKeys.end(),
			std::inserter(affectedSubsystemKeys, affectedSubsystemKeys.begin()));
		
		//TD15533++
		TA_Base_Bus::RegionKeyList regionKeysAccessible; 
		//while (true) //if session not persisted in DB, retry
		unsigned int retryTime = 0;
		while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
		{
			try
			{
				++retryTime;
				regionKeysAccessible = getLocations(); 
				break;
			}
			catch (...)
			{
				LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
			}
		}
		//++TD15533
		if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "updateSubsystems(): After retry %d times, Still no Session persisted in DB",
				CHECK_SESSION_IN_DB_RETRY_TIMES);
			
			CString errMsg = "During update Subsystem, no session persisted in DB";
			if (MessageBox(errMsg,
				"Still failed after retry",
				MB_ICONWARNING | MB_OK | MB_DEFBUTTON2) == IDOK)
			{
				throw;
			}
		}
/*		
        for (SubsystemKeyList::const_iterator subKeyIt = affectedSubsystemKeys.begin(); subKeyIt != affectedSubsystemKeys.end(); subKeyIt++)
        {
            addSubsystem(*subKeyIt, profileKeys, regionKeysAccessible); //TD15533
        }
*/		
        // Where subsystems have been added to the grid, the rows need to be re-sorted.
        //m_sesSubDutyGrid.SortTextItems(SUBSYSTEM_COLUMN, TRUE);

		updateOtherSesSubDrid(m_selectSessionInfo);
				
    }
}

bool OtherSesSubDutyPropertyPage::isRegionAccessible(
												   const TA_Base_Bus::RegionKey regionKey,
												   const TA_Base_Bus::RegionKeyList regionKeys)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::isRegionAccessible");

	bool hasLocationDuty = false;
	
	for (int i=0; i<(int)regionKeys.size(); i++)
	{
		if (regionKeys[i] == ALL_LOCATION)
		{
			//if central location then grant duty for this region
			hasLocationDuty = true;
			break;
		}
		else if (regionKeys[i] == regionKey)
		{
			hasLocationDuty = true;
			break;
		}
	}
	std::string okOrnot = (hasLocationDuty==true)?"true":"false";

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::isRegionAccessible, hasLocationDuty=%s",okOrnot.c_str());

    return hasLocationDuty;
}

bool OtherSesSubDutyPropertyPage::isSubsystemAccessible(
													  const TA_Base_Bus::ProfileKeyList profileKeys,
													  const TA_Base_Bus::SubsystemKey subsystemKey)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::isSubsystemAccessible");

	bool isAccessible = false;
	
	SubsystemKeyList accessibleSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(profileKeys));
	for (int i=0; i<(int)accessibleSubsystemKeys.size(); i++)
	{
		if (accessibleSubsystemKeys[i] == subsystemKey)
		{
			isAccessible = true;
			break;
		}
	}

	std::string okOrnot = (isAccessible==true)?"true":"false";

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::isSubsystemAccessible, isAccessible=%s",okOrnot.c_str());
    return isAccessible;
}

TA_Base_App::CellData OtherSesSubDutyPropertyPage::generateCellData(
	const TA_Base_Bus::SubsystemKey subsystemKey, 
	const TA_Base_Bus::RegionKey regionKey,
	const TA_Base_Bus::ESubsystemState subsystemState)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::generateCellData");

	TA_Base_App::CellData cellData;
	cellData.baseImage = -1;
	cellData.baseText = "";
	cellData.overrideImage = -1;
	cellData.overrideText = "";
	cellData.isOverrided = false;

	//OLD
	//case 1: currentActionGroup is empty --> cross / cross (no duty)
	//case 2: currentActionGroup is not empty and has / --> tick / tick (both have duty)
	//case 3: currentActionGroup is not empty but no / --> tick / cross OR cross / tick (one profile has duty)

	//NEW
	//case 1: ProfilesWithDuties is empty --> cross / cross (no duty)
	//case 2: ProfilesWithDuties is 2 --> tick / tick (both have duty)
	//case 3: ProfilesWithDuties is 1 --> tick / cross OR cross / tick (one profile has duty)

	//other cases (handled by CGridCtrlS)
	//case 5: empty / tick (no right, no duty / with rights, with duty)
	//case 6: empty / cross (no right, no duty / with rights, no duty)
	//case 7: tick / empty (with right, with duty / no right, no duty)
	//case 8: cross / empty (with right, no duty / no right, no duty)
	//case 9: empty / empty (no right, no duty / no right, no duty) displayed as gray cell

	//always assume base is 0th element and override is 1st element

	try
	{
		TA_Base_App::ProfilesWithDuties profilesWithDuties = getProfilesWithDuties(subsystemKey, regionKey);

		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::generateCellData, profilesWithDuties.size() = %d",profilesWithDuties.size());

		if (profilesWithDuties.size() == 2)
		{
			cellData.baseImage = GridAssistant::GI_TICK;
			cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
			cellData.overrideImage = GridAssistant::GI_TICK;
			cellData.overrideText = getActionGroupNameWithErrorChecking(profilesWithDuties[1].actionGroupKey);
			cellData.isOverrided = true;
		}
		else if (profilesWithDuties.size() == 0)
		{
			if (m_selectSessionInfo.ProfileId.size() == 2)
			{
				TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
					getActionGroupDB(m_selectSessionInfo.ProfileId[0], subsystemKey, subsystemState);
				TA_Base_Bus::ActionGroupKey overrideActionGroupKey = 
					getActionGroupDB(m_selectSessionInfo.ProfileId[1], subsystemKey, subsystemState);

				cellData.baseImage = GridAssistant::GI_CROSS;
				cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
				cellData.overrideImage = GridAssistant::GI_CROSS;
				cellData.overrideText = getActionGroupNameWithErrorChecking(overrideActionGroupKey);
				cellData.isOverrided = true;
			}
			else if (m_selectSessionInfo.ProfileId.size() == 1)
			{
				TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
					getActionGroupDB(m_selectSessionInfo.ProfileId[0], subsystemKey, subsystemState);

				cellData.baseImage = GridAssistant::GI_CROSS;
				cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
			}
			//else... profile cant be 0
		}
		else if (profilesWithDuties.size() == 1)
		{
			if (m_selectSessionInfo.ProfileId.size() == 2)
			{
				if (profilesWithDuties[0].profileKey == m_selectSessionInfo.ProfileId[0])
				{
					TA_Base_Bus::ActionGroupKey overrideActionGroupKey = 
						getActionGroupDB(m_selectSessionInfo.ProfileId[1], subsystemKey, subsystemState);

					cellData.baseImage = GridAssistant::GI_TICK;
					cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
					cellData.overrideImage = GridAssistant::GI_CROSS;
					cellData.overrideText = getActionGroupNameWithErrorChecking(overrideActionGroupKey);
					cellData.isOverrided = true;
				}
				else if (profilesWithDuties[0].profileKey == m_selectSessionInfo.ProfileId[1])
				{
					TA_Base_Bus::ActionGroupKey baseActionGroupKey = 
						getActionGroupDB(m_selectSessionInfo.ProfileId[0], subsystemKey, subsystemState);

					cellData.baseImage = GridAssistant::GI_CROSS;
					cellData.baseText = getActionGroupNameWithErrorChecking(baseActionGroupKey);
					cellData.overrideImage = GridAssistant::GI_TICK;
					cellData.overrideText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);;
					cellData.isOverrided = true;
				}
			}
			else if (m_selectSessionInfo.ProfileId.size() == 1)
			{
				// okay to assume this profile has duty
				cellData.baseImage = GridAssistant::GI_TICK;
				cellData.baseText = getActionGroupNameWithErrorChecking(profilesWithDuties[0].actionGroupKey);
			}		
			//else... profile cant be 0
		}

		// Yanrong++ TD19076. 
		// Treat with "TCS_Train Comms(stn)" subsystem specially for 4669 project,
		// display yellow tick flag while the central profile has no duty in normal mode
		if( m_dataCache.getSubsystemName(subsystemKey).compare(TCS_TRAIN_COMMS_STN_SUBSYSTEM) == 0 // TCS subsystem
			&& TA_Base_Core::RunParams::getInstance().get( RPARAM_PROJECTNAME ).compare( TCS_SPECIAL_PROJECT_NAME ) == 0 // 4669 project
			&& subsystemState == TA_Base_Bus::E_SS_NORMAL // normal mode
			&& TA_Base_Core::SessionAccessFactory::getInstance().getSession( m_selectSessionInfo.SessionId )->getLocationKey() == OCC_LOCATION // is OCC location
			)
		{
			TA_Base_Bus::SessionInfoEx sessionInfoEx = m_sessionCache.getSessionInfo( m_selectSessionInfo.SessionId );

			// Duty manager will not judge location duty. As some profile will not have location duty when login, such as TCO group profiles.
			// In normal mode, if the profile has duty, show tick; otherwise, show box tick.
			if( profilesWithDuties.size() == 0 ) // has no duty
			{
				if( m_selectSessionInfo.ProfileId.size() == 1 ) // no override
				{
					cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
				}
			else if( m_selectSessionInfo.ProfileId.size() == 2 ) // override
				{
				cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
				cellData.overrideImage = GridAssistant::GI_DELEGATE_TICK;
				}
			}
			// gongzong++ CL-21319 replace "&" with "&&". yanrong make a clerical error.
			else if( profilesWithDuties.size() == 1 && m_selectSessionInfo.ProfileId.size() == 2 ) // override and only one profile has duty
			{
				if( profilesWithDuties[0].profileKey != m_selectSessionInfo.ProfileId[0] ) // base profile has no duty
				{
					cellData.baseImage = GridAssistant::GI_DELEGATE_TICK;
				}
				else if( profilesWithDuties[0].profileKey != m_selectSessionInfo.ProfileId[1] ) // override profile has no duty
				{
					cellData.overrideImage = GridAssistant::GI_DELEGATE_TICK;
				}
			}
		}
	}
	catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while generating Cell Data. %s",ex.what());
    }
    catch (const TA_Base_Core::TransactiveException& ex)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while generating Cell Data. %s",ex.what());
    }
    catch (...)
    {
		LOG_GENERIC(SourceInfo,DebugUtil::DebugError,"Error while generating Cell Data.");
	}


	// ++Yanrong
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::generateCellData");

    return cellData;
}

COLORREF OtherSesSubDutyPropertyPage::determineCellBkColour(  const bool inRegion, const TA_Base_Bus::ESubsystemState state  ) const
{
    COLORREF cellColour = m_unallocatedSubsystemColour;
	
    // See if the subsystem exists in the current region.
    if ( inRegion )
    {
        // It is in the region, so the colour will depend on subsystem state.
        switch ( state )
        {
        case TA_Base_Bus::E_SS_NORMAL:
            cellColour = m_normalDutyColour;
            break;
			
        case TA_Base_Bus::E_SS_DELEGATED:
            cellColour = m_delegatedDutyColour;
            break;
			
        case TA_Base_Bus::E_SS_DEGRADED:
            cellColour = m_degradedDutyColour;
            break;
			
        default:
            //TA_ASSERT( false, "Could not determine colour for undefined subsystem state." );
            break;
        };
    }
	
    return cellColour;
}

std::string OtherSesSubDutyPropertyPage::getActionGroupNameWithErrorChecking(
																		   const TA_Base_Bus::ActionGroupKey actionGroupKey)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::getActionGroupNameWithErrorChecking");

	std::string actionGroupName = "";
	
	if (actionGroupKey != NO_ACTION_GROUP)
	{
		actionGroupName = m_dataCache.getActionGroupName(actionGroupKey);
	}	
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::getActionGroupNameWithErrorChecking");

    return actionGroupName;
}
//++TD15533


TA_Base_Bus::ActionGroupKey OtherSesSubDutyPropertyPage::getActionGroupDB(
																		const TA_Base_Bus::ProfileKey profileKey,
																		const TA_Base_Bus::SubsystemKey subsystemKey,
																		const TA_Base_Bus::ESubsystemState subsystemState)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::getActionGroupDB");

	TA_Base_Bus::ActionGroupKey actionGroupKey = NO_ACTION_GROUP;
	
	try
	{
		// find in cache
		for (int i=0; i<(int)m_actionGroupsDB.size(); i++)
		{
			if ((m_actionGroupsDB[i].profileKey == profileKey) &&
				(m_actionGroupsDB[i].subsystemKey == subsystemKey) &&
				(m_actionGroupsDB[i].subsystemState == subsystemState))
			{
				actionGroupKey = m_actionGroupsDB[i].actionGroupKey;
				break;
			}
		}
		
		// not yet in cache
		if (actionGroupKey == NO_ACTION_GROUP)
		{
			// find in DB	
			actionGroupKey = TA_Base_Core::RightsAccessFactory::getInstance().getActionGroup(
				profileKey,
				subsystemKey,
				subsystemState);
			
			// insert in list
			TA_Base_App::PSSA pssa;
			pssa.profileKey = profileKey;
			pssa.subsystemKey = subsystemKey;
			pssa.subsystemState = subsystemState;
			pssa.actionGroupKey = actionGroupKey;
			m_actionGroupsDB.push_back(pssa);
		}
	}
	catch (...)
	{
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in getActionGroupDB()");
	}
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::getActionGroupDB");

    return actionGroupKey;
}

TA_Base_App::ProfilesWithDuties OtherSesSubDutyPropertyPage::getProfilesWithDuties(
																				 const TA_Base_Bus::SubsystemKey subsystemKey,
																				 const TA_Base_Bus::RegionKey regionKey)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::getProfilesWithDuties");

	TA_Base_App::ProfilesWithDuties profilesWithDuties;
	
    SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
    SubsystemDutyList subsystemDuties(sdmElement.getSubsystemDuties(m_selectSessionInfo.SessionId));

	LOG_GENERIC(SourceInfo,
		DebugUtil::DebugDebug,
		"OtherSesSubDutyPropertyPage::getProfilesWithDuties, subsystemKey = %lu, regionKey = %lu, m_selectSessionInfo.SessionId = %s",
		subsystemKey,regionKey,(m_selectSessionInfo.SessionId).c_str());

    if (!subsystemDuties.empty())
    {
        for (SubsystemDutyList::const_iterator dutyIt = subsystemDuties.begin(); dutyIt != subsystemDuties.end(); dutyIt++)
        {
			TA_Base_App::PSSA profileWithDuty;
			profileWithDuty.profileKey = dutyIt->profileKey;
			profileWithDuty.actionGroupKey = dutyIt->actionGroupKey;
			profilesWithDuties.push_back(profileWithDuty);
        }
    }
	else
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"OtherSesSubDutyPropertyPage::getProfilesWithDuties, subsystemDuties is empty");
	}

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::getProfilesWithDuties");

    return profilesWithDuties;
}

TA_Base_Bus::RegionKeyList OtherSesSubDutyPropertyPage::getLocations()
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::getLocations");

	TA_Base_Bus::RegionKeyList regionKeys;

	/* NOTE: for local profile, only current location should be displayed.
	 * for central profile, all locations should be displayed.
	 * if local override with central, then all locations are displayed.
	 * if local override with local, same location is displayed
	 * if central is override with local, there is no such case.
	 * this has nothing to do with rights or duties, only the location.
	 */

	//based on session, get location from DB
	//if 0 then central, if not then local
	//TA_Base_Core::ISession* session;
	//session = TA_Base_Core::SessionAccessFactory::getInstance().getSession( m_currentSessionInfo.SessionId );

   

	// check if it has ended or not
	unsigned long locationKey =  m_selectSessionInfo.LocationId;
	if (locationKey == ALL_LOCATION) 
	{
		//central profile
		regionKeys.push_back(locationKey);

		//no need to check overrided profile's location
	}
	else
	{
		//local profile
		regionKeys.push_back(locationKey);
		
		//need to check overrided profile's location
		//always assume base is 0th element and override is 1st element

		try
		{
			//overrided
			if (m_selectSessionInfo.ProfileId.size() == 2)
			{
				//get profile from DB if local or central for base
				unsigned long baseProfile = m_selectSessionInfo.ProfileId[0];
				TA_Base_Core::IProfile* profileB;
				profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );
				if (profileB->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same

				//get profile from DB if local or central for override
				unsigned long overrideProfile = m_selectSessionInfo.ProfileId[1];
				TA_Base_Core::IProfile* profileO;
				profileO = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( overrideProfile );
				if (profileO->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same
			}
			//single operator
			else if (m_selectSessionInfo.ProfileId.size() == 1)
			{
				//get profile from DB if local or central for base
				unsigned long baseProfile = m_selectSessionInfo.ProfileId[0];
				TA_Base_Core::IProfile* profileB;
				profileB = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile( baseProfile );
				if (profileB->getType() == CENTRAL_PROFILE_TYPE)
				{
					//currently we detected a local profile, so need to add central profile
					regionKeys.push_back(ALL_LOCATION); 
				}
				//no need to add local profile with assumption that location is the same
			}
			//else... profile cant be 0
		}
		catch (...) 
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in getLocations()");
		}
	}    
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::getLocations");

	return regionKeys;
}

void OtherSesSubDutyPropertyPage::updateOtherSesSubDrid(TA_Base_Bus::SessionInfoEx sesInfEx)
{
	if ((!sesInfEx.OperatorId.empty()) && (!sesInfEx.ProfileId.empty()))
	{
		LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,
			"in OtherSesSubDutyPropertyPage::updateOtherSesSubDrid(), after reinterpret_cast SessionInfoEx, the SessionInfoEx in the parameter is: OperatorId=%ld, ProfileId=%ld, LocationId=%ld, WorkStationId=%ld",
			sesInfEx.OperatorId[0],sesInfEx.ProfileId[0],sesInfEx.LocationId,sesInfEx.WorkstationId);

		{
			updateOtherSessionInfoEdit(sesInfEx);

			m_selectSessionInfo.LocationId = sesInfEx.LocationId;
			m_selectSessionInfo.OperatorId = sesInfEx.OperatorId;
			m_selectSessionInfo.ProfileId = sesInfEx.ProfileId;
			m_selectSessionInfo.SessionId = sesInfEx.SessionId;
			m_selectSessionInfo.sessionKey = sesInfEx.sessionKey;
			m_selectSessionInfo.UserId = sesInfEx.UserId;
			m_selectSessionInfo.WorkstationId = sesInfEx.WorkstationId;
		}
		m_selectSessionId = sesInfEx.SessionId;
	}

	    // Initialise the colours to used when rendering the grid.
    m_normalDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_NORMALDUTYCOLOUR ),
                                                     NORMAL_DUTY_COLOUR );
    m_delegatedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DELEGATEDDUTYCOLOUR ),
                                                     DELEGATED_DUTY_COLOUR );
    m_degradedDutyColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_DEGRADEDDUTYCOLOUR ),
                                                     DEGRADED_DUTY_COLOUR );
    m_unallocatedSubsystemColour = convertColourStrToColorref( TA_Base_Core::RunParams::getInstance().get( RPARAM_UNALLOCATEDSUBSYSTEMCOLOUR ),
                                                     UNALLOCATED_SUBSYSTEM_COLOUR );

    // By default only those subsystems are shown which the session currently has access to.
    //SubsystemKeyList subsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_currentSessionInfo.ProfileId));
	SubsystemKeyList subsystemKeys;
	if(m_isShowAllSubsystems)
	{
		subsystemKeys = m_subsystemDutyMatrix.getRowIds();
	}
	else
	{
		subsystemKeys = m_subsystemDutyMatrix.getRowIdsByRights(m_selectSessionInfo.ProfileId);
	}
    RegionKeyList regionKeys(m_subsystemDutyMatrix.getColumnIds());

    m_sesSubDutyGrid.SetImageList(&GridAssistant::getGridImages());
    m_sesSubDutyGrid.SetFont(AfxGetMainWnd()->GetFont());
    m_sesSubDutyGrid.SetDefaultRowHeight(DEFAULT_ROW_HEIGHT);
    m_sesSubDutyGrid.SetDefaultColumnWidth(DEFAULT_COLUMN_WIDTH);
    m_sesSubDutyGrid.SetListMode(TRUE);
    m_sesSubDutyGrid.EnableTitleTips(TRUE);
    m_sesSubDutyGrid.EnableSelection(FALSE);
    m_sesSubDutyGrid.EnableDragAndDrop(FALSE);
    m_sesSubDutyGrid.SetEditable(FALSE);
    m_sesSubDutyGrid.SetHeaderSort(FALSE);

    m_sesSubDutyGrid.SetFixedRowCount(FIXED_ROW_COUNT);        // for region names
    m_sesSubDutyGrid.SetFixedColumnCount(FIXED_COLUMN_COUNT);  // for subsystem names
    m_sesSubDutyGrid.SetRowCount(m_sesSubDutyGrid.GetFixedRowCount());  // this will grow as subsystems are added to the grid
    m_sesSubDutyGrid.SetColumnCount(m_sesSubDutyGrid.GetFixedColumnCount() + regionKeys.size());
	
    m_sesSubDutyGrid.SetRowResize(FALSE);//CL-19268--lkm
    // Set the fixed column labels.
    m_sesSubDutyGrid.SetItemText(REGION_ROW, SUBSYSTEM_COLUMN, SUBSYSTEM_COLUMN_HEADING);

    // Set the cells in the first row with the names of the regions.
    // Use cell item data to store the corresponding region keys so that the region
    // representing each column can be easily identified.
    int column(SUBSYSTEM_COLUMN + 1);
	CString regionName = "";

    for (RegionKeyList::const_iterator regionIt = regionKeys.begin(); regionIt != regionKeys.end(); regionIt++)
    {
		regionName = "";

        try
        {
            GV_ITEM gridItem;
            gridItem.mask = GVIF_TEXT | GVIF_PARAM;
            gridItem.row = REGION_ROW;
            gridItem.col = column;
            gridItem.szText = m_dataCache.getRegionDisplayName(*regionIt).c_str();
            gridItem.lParam = *regionIt;

			regionName = m_dataCache.getRegionName(*regionIt).c_str();

			if(0 == regionName.CompareNoCase("All Locations"))
			{
				m_sesSubDutyGrid.DeleteColumn(column);
				continue;
			}
            m_sesSubDutyGrid.SetItem(&gridItem);

            column++;
        }
        catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
        {
            // Get rid of one column
            m_sesSubDutyGrid.DeleteColumn(m_sesSubDutyGrid.GetColumnCount() - 1);

            MessageBox::error("Error while adding region to subsystem duty grid.", ex);
        }
    }

	//TD15533++
	TA_Base_Bus::RegionKeyList regionKeysAccessible; 
	unsigned int retryTime = 0;
	//while (true) //if session not persisted in DB, retry
	while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		try
		{
			++retryTime;
			regionKeysAccessible = getLocations(); 
			break;
		}
		catch (...)
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
		}
	}
	
	if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "updateOtherSesSubDrid(): After retry %d times, Still no Session persisted in DB",
			CHECK_SESSION_IN_DB_RETRY_TIMES);
		
		CString errMsg = "During initialise Subsystem DutyGrid, no session persisted in DB";
		if (MessageBox(errMsg,
                   "Still failed after retry",
                   MB_ICONWARNING | MB_OK | MB_DEFBUTTON2) == IDOK)
		{
			throw;
		}
	}
	//++TD15533

    // Add individual subsystems to the grid. Subsystem names are used as entries in the first column.
    for (SubsystemKeyList::const_iterator subsystemIt = subsystemKeys.begin(); subsystemIt != subsystemKeys.end(); subsystemIt++)
    {
       addSubsystem(*subsystemIt, m_selectSessionInfo.ProfileId, regionKeysAccessible); //TD15533
    }

    // Sort subsystems by name.
    m_sesSubDutyGrid.SortTextItems(SUBSYSTEM_COLUMN, TRUE);

    // Size subsystem column width to fit subsystem names.
    m_sesSubDutyGrid.AutoSizeColumn(SUBSYSTEM_COLUMN);

	//delete sesInfEx;
	//sesInfEx = NULL;
	//updateOtherSessionInfoEdit(sesInfEx);
	
	//enableButtonsAndSessionInfoEdit(true);

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::updateOtherSesSubDrid()");
}

void OtherSesSubDutyPropertyPage::updateOtherSessionInfoEdit(TA_Base_Bus::SessionInfoEx sesInfEx)
{
	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"begin OtherSesSubDutyPropertyPage::updateOtherSessionInfoEdit()");

	std::string operatorName("");
	std::string profileName("");
	std::string locationName("");
	std::string workstationName("");
				
	if ( sesInfEx.SessionId == "" 
		||sesInfEx.OperatorId.empty() 
		|| sesInfEx.ProfileId.empty() )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "[CL-21882] OperatorId is empty, or ProfileId is empty.");
		return;
	}
				
	//operatorName = m_dataCache.getOperatorDescription(sesInfEx.OperatorId[0]);
	operatorName = m_dataCache.getOperatorName(sesInfEx.OperatorId[0]);
	profileName = m_dataCache.getProfileName(sesInfEx.ProfileId[0]);
	if ( (sesInfEx.OperatorId.size() > 1) && (sesInfEx.ProfileId.size() > 1) )
	{
		operatorName += "/";
		//operatorName += m_dataCache.getOperatorDescription(sesInfEx.OperatorId[1]);
		operatorName += m_dataCache.getOperatorName(sesInfEx.OperatorId[1]);
					
		profileName += "/";
		profileName += m_dataCache.getProfileName(sesInfEx.ProfileId[1]);
	}
	locationName = m_dataCache.getLocationDisplayName(sesInfEx.LocationId);
	workstationName = m_dataCache.getWorkstationName(sesInfEx.WorkstationId);

	CString csText;
	csText.Format("Selected Session:  Operator: %s,    Profile: %s,    Location: %s,    Workstation: %s,    SessionId: %s",operatorName.c_str(),profileName.c_str(),locationName.c_str(),workstationName.c_str(),sesInfEx.SessionId.c_str());
	m_selectedSessionEdit.SetWindowText(csText);
	UpdateData(FALSE);
	enableButtonsAndSessionInfoEdit(true);

	LOG_GENERIC(SourceInfo,DebugUtil::DebugDebug,"end OtherSesSubDutyPropertyPage::updateOtherSessionInfoEdit()");
}

void OtherSesSubDutyPropertyPage::sessionChanged(const TA_Base_Bus::SessionUpdate& sessionUpdate)
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

LRESULT OtherSesSubDutyPropertyPage::OnReceiveSessionUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
	//std::auto_ptr<SessionUpdate> sessionUpdate(reinterpret_cast<SessionUpdate*>(wParam));
	boost::shared_ptr<SessionUpdate> sessionUpdate(reinterpret_cast<SessionUpdate*>(wParam));
	{
		sessionUpdateRegionDuty(sessionUpdate);
	}
	{
		sessionUpdateSubsystemDuty(sessionUpdate);
	}

	return 0;
}

void OtherSesSubDutyPropertyPage::sessionUpdateRegionDuty(boost::shared_ptr<TA_Base_Bus::SessionUpdate> sessionUpdate)
{
	//
}

void OtherSesSubDutyPropertyPage::sessionUpdateSubsystemDuty(boost::shared_ptr<TA_Base_Bus::SessionUpdate> sessionUpdate)
{
	TA_Base_Bus::SessionId selectSessionId(m_selectSessionId);

	if (sessionUpdate->updateType == TA_Base_Bus::TA_Security::SessionEnd ||
        sessionUpdate->sessionId != selectSessionId)
    {
        return ;
    }

	TA_Base_Bus::ProfileKeyList previousProfileId(m_selectSessionInfo.ProfileId);
    m_selectSessionInfo = m_sessionCache.getSessionInfo(selectSessionId);

	SubsystemKeyList previousSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(previousProfileId));
    SubsystemKeyList currentSubsystemKeys(m_subsystemDutyMatrix.getRowIdsByRights(m_selectSessionInfo.ProfileId));

	SubsystemKeyList empty;
	SubsystemKeyList allSubsystemKeys(m_subsystemDutyMatrix.getRowIds());

	if (m_showAllSubsystemsOtherSesBtn.GetCheck() == BST_UNCHECKED)
	{
		updateSubsystems(previousSubsystemKeys, empty, previousProfileId);
		updateSubsystems(empty, currentSubsystemKeys, m_currentSessionInfo.ProfileId);
	}
	else
	{
		updateSubsystems(allSubsystemKeys, empty, m_currentSessionInfo.ProfileId);
        updateSubsystems(empty, allSubsystemKeys, m_currentSessionInfo.ProfileId);
    }
}

void OtherSesSubDutyPropertyPage::enableButtonsAndSessionInfoEdit(bool isable)
{
	//m_selectedSessionEdit.EnableWindow(isable);
	m_selectedSessionEdit.ShowWindow(isable);
	m_showAllSubsystemsOtherSesBtn.EnableWindow(isable);
	m_ReloadDataBtn.EnableWindow(isable);
}

void OtherSesSubDutyPropertyPage::dutyChanged(const TA_Base_Bus::SubsystemDutyUpdate& dutyUpdate)
{
    // Let the thread that created the window process the update.
    HWND wndHandle(GetSafeHwnd());
	
    if (wndHandle != NULL)
    {
        // Post the message so as not to hold up the notifying thread.
        // Clone the duty update and throw onto the heap to make sure 
        // that by the time the message is processed we still have a valid
        // object. The message handler takes care of destroying it once consumed.
        ::PostMessage(wndHandle, WM_USER_RECEIVE_SUB_DUTY_UPDATE, 
			reinterpret_cast<WPARAM>(new SubsystemDutyUpdate(dutyUpdate)), 0);
    }
}

//OnReceiveOhterSesSubDutyUpdate
LRESULT OtherSesSubDutyPropertyPage::OnReceiveOhterSesSubDutyUpdate(WPARAM wParam, LPARAM /*lParam*/)
{
    // Make sure the duty update gets destroyed when we leave here.
    std::auto_ptr<SubsystemDutyUpdate> dutyUpdate(reinterpret_cast<SubsystemDutyUpdate*>(wParam));

    if ( ( TA_Base_Bus::DUT_ADD == dutyUpdate->updateType ) || ( TA_Base_Bus::DUT_REMOVE == dutyUpdate->updateType ) )
    {
        for (SubsystemDutyList::const_iterator subDutyIt = dutyUpdate->duties.begin(); subDutyIt != dutyUpdate->duties.end(); subDutyIt++)
        {
            try
            {
                // Ignore subsystem duty updates for sessions other than the current one.
                if (m_selectSessionInfo.SessionId != subDutyIt->session.in())
                {
                    continue;
                }

                // Get the row index corresponding to the subsystem affected by duty change.
                // Ignore the update if no such row is found.
                int row(GridAssistant::findGridRow(m_sesSubDutyGrid, subDutyIt->subsystemKey));

                if (row < 0)
                {
                    continue;
                }
        
                // Get the column index corresponding to the region affected by duty change.
                // Ignore the update if no such column is found.
                int column(GridAssistant::findGridColumn(m_sesSubDutyGrid, subDutyIt->regionKey));  
        
                if (column < 0)
                {
                    continue;
                }

				//TD15533++

                // Update the relevant grid cell.
                //std::string actionGrp(GridAssistant::generateActionGroupString(m_currentSessionInfo, subDutyIt->subsystemKey, subDutyIt->regionKey));
                //int image(actionGrp.empty()? GridAssistant::GI_CROSS : GridAssistant::GI_TICK);

				TA_Base_Bus::RegionKeyList regionKeysAccessible; 
				//while (true) //if session not persisted in DB, retry
				unsigned int retryTime = 0;
				while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					try
					{
						++retryTime;
						regionKeysAccessible = getLocations(); 
						break;
					}
					catch (...)
					{
						LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
					}
				}
				if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnReceiveSubsystemDutyUpdate(): After retry %d times, Still no Session persisted in DB",
						CHECK_SESSION_IN_DB_RETRY_TIMES);
		
					CString errMsg = "During receive Subsystem Duty update, no session persisted in DB";
					if (MessageBox(errMsg,
						"Still failed after retry",
						MB_ICONWARNING | MB_OK | MB_DEFBUTTON2) == IDOK)
					{
						throw;
					}
				}
				
				TA_Base_Bus::ProfileKeyList profileKeys = getProfileKeyList(subDutyIt->subsystemKey, subDutyIt->regionKey);

				SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subDutyIt->subsystemKey, subDutyIt->regionKey);
				COLORREF bkColour = determineCellBkColour( true, sdmElement.getSubsystemState() );

				//TD19148---- LiDan 25,Jun,2007
				//2) Duty Manager of Local MFT Subsystem tab populate action group for other locations.
				if (!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible) ||  
					(!isSubsystemAccessible(profileKeys, subDutyIt->subsystemKey) &&
					!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible))|| 
					!sdmElement.isSubsystemInRegion()) 
					continue;
				//----TD19148
				TA_Base_App::CellData cellData = generateCellData(subDutyIt->subsystemKey, subDutyIt->regionKey, sdmElement.getSubsystemState());
				m_sesSubDutyGrid.SetItemImage(row, column, cellData.baseImage);
				m_sesSubDutyGrid.SetItemText(row, column, cellData.baseText.c_str());
                m_sesSubDutyGrid.SetItemImageS(row, column, cellData.overrideImage);
				m_sesSubDutyGrid.SetItemTextS(row, column, cellData.overrideText.c_str()); 
				m_sesSubDutyGrid.SetItemOverrided(row, column, cellData.isOverrided); 
				
				/*if (!isRegionAccessible(subDutyIt->regionKey, regionKeysAccessible) ||  //requirement #4
					!isSubsystemAccessible(profileKeys, subDutyIt->subsystemKey) || //requirement #2
					!sdmElement.isSubsystemInRegion()) 
				{
					// force to gray if not accessible
					bkColour = determineCellBkColour( false, sdmElement.getSubsystemState() );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, "");
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, ""); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}
				else*/ if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					bkColour = determineCellBkColour( false, sdmElement.getSubsystemState() );

					m_sesSubDutyGrid.SetItemImage(row, column, -1);
					m_sesSubDutyGrid.SetItemText(row, column, "");
					m_sesSubDutyGrid.SetItemImageS(row, column, -1);
					m_sesSubDutyGrid.SetItemTextS(row, column, ""); 
					m_sesSubDutyGrid.SetItemOverrided(row, column, false); 
				}

				m_sesSubDutyGrid.SetItemBkColour(row, column, bkColour);
                m_sesSubDutyGrid.RedrawCell(row, column);

				//++TD15533
            }
            catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
            }
            catch (const TA_Base_Core::TransactiveException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch (...)
            {
                //MessageBox::error("Error while processing subsystem duty update.");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
            }        
        }
    }
    else if ( TA_Base_Bus::DUT_STATE == dutyUpdate->updateType )
    {
        for (SubsystemStateList::const_iterator subStateIt = dutyUpdate->states.begin(); subStateIt != dutyUpdate->states.end(); subStateIt++)
        {
            try
            {
                // Get the row index corresponding to the subsystem affected by duty change.
                // Ignore the update if no such row is found.
                int row(GridAssistant::findGridRow(m_sesSubDutyGrid, subStateIt->subsystemKey));

                if (row < 0)
                {
                    continue;
                }
        
                // Get the column index corresponding to the region affected by duty change.
                // Ignore the update if no such column is found.
                int column(GridAssistant::findGridColumn(m_sesSubDutyGrid, subStateIt->regionKey));  
        
                if (column < 0)
                {
                    continue;
                }

                // Update the relevant grid cell.

				//TD15533++
				//almost the same now with DUT_ADD and DUT_REMOVE but use the current state from the update

				TA_Base_Bus::RegionKeyList regionKeysAccessible; 
				//while (true) //if session not persisted in DB, retry
				unsigned int retryTime = 0;
				while(retryTime < CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					try
					{
						++retryTime;
						regionKeysAccessible = getLocations(); 
						break;
					}
					catch (...)
					{
						LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "TD15533: Unknown exception in calling getLocations()");
					}
				}
				if(retryTime > CHECK_SESSION_IN_DB_RETRY_TIMES)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "OnReceiveSubsystemDutyUpdate(): After retry %d times, Still no Session persisted in DB",
						CHECK_SESSION_IN_DB_RETRY_TIMES);
		
					CString errMsg = "During receive Subsystem Duty update, no session persisted in DB";
					if (MessageBox(errMsg,
						"Still failed after retry",
						MB_ICONWARNING | MB_OK | MB_DEFBUTTON2) == IDOK)
					{
						throw;
					}
				}

				

				TA_Base_Bus::ProfileKeyList profileKeys = getProfileKeyList(subStateIt->subsystemKey, subStateIt->regionKey);

				SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subStateIt->subsystemKey, subStateIt->regionKey);
				COLORREF bkColour = determineCellBkColour( true, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );
				//TD19148---- LiDan 25,Jun,2007
				//2) Duty Manager of Local MFT Subsystem tab populate action group for other locations.

				if (!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible) ||  
					(!isSubsystemAccessible(profileKeys, subStateIt->subsystemKey) && 
					!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible)) || 
					!sdmElement.isSubsystemInRegion()) 
					continue;
				//----TD19148

				TA_Base_App::CellData cellData = generateCellData(subStateIt->subsystemKey, subStateIt->regionKey, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState));
				m_sesSubDutyGrid.SetItemImage(row, column, cellData.baseImage);
				m_sesSubDutyGrid.SetItemText(row, column, cellData.baseText.c_str());
                m_sesSubDutyGrid.SetItemImageS(row, column, cellData.overrideImage);
				m_sesSubDutyGrid.SetItemTextS(row, column, cellData.overrideText.c_str()); 
				m_sesSubDutyGrid.SetItemOverrided(row, column, cellData.isOverrided); 
				
				/*if (!isRegionAccessible(subStateIt->regionKey, regionKeysAccessible) ||  //requirement #4
					!isSubsystemAccessible(profileKeys, subStateIt->subsystemKey) || //requirement #2
					!sdmElement.isSubsystemInRegion()) 
				{
					// force to gray if not accessible
					bkColour = determineCellBkColour( false, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );

					m_subsystemDutyGrid.SetItemImage(row, column, -1);
					m_subsystemDutyGrid.SetItemText(row, column, "");
					m_subsystemDutyGrid.SetItemImageS(row, column, -1);
					m_subsystemDutyGrid.SetItemTextS(row, column, ""); 
					m_subsystemDutyGrid.SetItemOverrided(row, column, false); 
				}
				else*/ if ((cellData.baseText.compare("") == 0) && (cellData.overrideText.compare("") == 0))
				{
					//force to gray cell if empty
					bkColour = determineCellBkColour( false, static_cast<TA_Base_Bus::ESubsystemState>(subStateIt->currState) );

					m_sesSubDutyGrid.SetItemImage(row, column, -1);
					m_sesSubDutyGrid.SetItemText(row, column, "");
					m_sesSubDutyGrid.SetItemImageS(row, column, -1);
					m_sesSubDutyGrid.SetItemTextS(row, column, ""); 
					m_sesSubDutyGrid.SetItemOverrided(row, column, false); 
				}

				m_sesSubDutyGrid.SetItemBkColour(row, column, bkColour);
                m_sesSubDutyGrid.RedrawCell(row, column);

				//++TD15533
            }
            catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
            }
            catch (const TA_Base_Core::TransactiveException& ex)
            {
                //MessageBox::error("Error while processing subsystem duty update.", ex);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch (...)
            {
                //MessageBox::error("Error while processing subsystem duty update.");
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error while processing subsystem duty update");
            }
        }
    }

    return 0;
}

TA_Base_Bus::ProfileKeyList OtherSesSubDutyPropertyPage::getProfileKeyList(
																		 const TA_Base_Bus::SubsystemKey subsystemKey,
																		 const TA_Base_Bus::RegionKey regionKey)
{
	TA_Base_Bus::ProfileKeyList profileKeys;
	
    SubsystemDutyMatrixElement sdmElement = m_subsystemDutyMatrix.getElement(subsystemKey, regionKey);
    SubsystemDutyList subsystemDuties(sdmElement.getSubsystemDuties(m_selectSessionInfo.SessionId));
	
    if (!subsystemDuties.empty())
    {
        for (SubsystemDutyList::const_iterator dutyIt = subsystemDuties.begin(); dutyIt != subsystemDuties.end(); dutyIt++)
        {
			profileKeys.push_back(dutyIt->profileKey);
        }
    }
	
    return profileKeys;
}
