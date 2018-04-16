/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/incidents/incident_manager/src/CIncidentManagerListView.cpp $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Provide GUI for user to Create/View/Edit/Print/Open/Close incident reports
  */
#include "stdafx.h"
#include <sstream>
#include <iomanip>

#include "core/utilities/src/FunctionEntry.h"
#include "app/incidents/incident_manager/src/CIncidentManagerListView.h"
#include "app/incidents/incident_manager/src/CMainFrame.h"
#include "bus/mfc_extensions/src/Resource.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/application_types/src/apptypes.h"
#include "core/utilities/src/DateFormat.h" 

// CL-16128 ++
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IProfile.h"
// ++ CL-16128

//++Location Display name
#include "core/data_access_interface/src/LocationAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;
using namespace TA_Base_Bus;
using namespace TA_Base_Core;

namespace TA_IRS_App
{

	const unsigned long CIncidentManagerListView::MAX_INCIDENT_TO_DISPLAY = 1000;
	const unsigned long CIncidentManagerListView::MAX_INCIDENT_REPORT_DIALOG = 10;
	const std::string CIncidentManagerListView::STRING_TO_LAUNCH_REPORT_MANAGER = "--PrintIncident=1";
	const std::string CIncidentManagerListView::FILE_NAME_PARAM = " --file=";
	const std::string CIncidentManagerListView::REPORT_ID_PARAM = " --IncidentId=";
	//Mintao ++
	//Bug 643
	const std::string CIncidentManagerListView::DB_CONNECTION_STRING =" --db-connection=";
	//Mintao ++
	//Bug 643
	//const std::string CIncidentManagerListView::
	const std::string CIncidentManagerListView::RPARAM_REPORTTEMPLATE = "ReportTemplate";
	// Settings for each column

	//libo
	const std::string CIncidentManagerListView::DB_CONNECTION_FILE =" --db-connection-file=";
	//libo 



	const CIncidentManagerListView::ColumnInfo CIncidentManagerListView::COLUMN_INFO[CIncidentManagerListView::COLUMNS_COUNT] = 
	{ 
		//TD15204 - reverting back the changes of TD15041
		{ID,130, IDS_LISTVIEWCOLUMN_ID, AutoSortListCtrl::BY_STRING},
		{CREATED, 130, IDS_LISTVIEWCOLUMN_CREATED, AutoSortListCtrl::BY_CALLBACK},
		{TITLE, 210, IDS_LISTVIEWCOLUMN_TITLE, AutoSortListCtrl::BY_STRING},
		{LOCATION, 80, IDS_LISTVIEWCOLUMN_LOCATION, AutoSortListCtrl::BY_STRING},
		{TRACK, 50, IDS_LISTVIEWCOLUMN_TRACK, AutoSortListCtrl::BY_CALLBACK},
		{TRAIN, 90, IDS_LISTVIEWCOLUMN_TRAIN, AutoSortListCtrl::BY_STRING},
		{ALARM, 60, IDS_LISTVIEWCOLUMN_ALARM, AutoSortListCtrl::BY_STRING},
		{ASSET, 150, IDS_LISTVIEWCOLUMN_ASSET, AutoSortListCtrl::BY_STRING},
		{SUBSYSTEM, 100, IDS_LISTVIEWCOLUMN_SUBSYSTEM, AutoSortListCtrl::BY_STRING},
		{CREATOR, 100, IDS_LISTVIEWCOLUMN_CREATOR, AutoSortListCtrl::BY_STRING},
		{PROFILE, 50, IDS_LISTVIEWCOLUMN_PROFILE, AutoSortListCtrl::BY_STRING},
		{CLOSED, 120, IDS_LISTVIEWCOLUMN_CLOSED, AutoSortListCtrl::BY_CALLBACK},

	};

	//-----------------------------------------------------------------------------
	int CIncidentManagerListView::GetSelectedIncidents( CListCtrl &clc, vector<ItaIncidentReportData *> &arraySelectedIncidents )
	{
		FUNCTION_ENTRY("GetSelectedIncidents");

		//empty the map first
		m_selectedItemMaps.clear();

		// get id of all selected items of a list control
		int iItem = -1;                                         // start from begin of the list
		do {
			iItem = clc.GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED );
			if ( iItem != -1 )                         // found!
			{
				// get the data associated with this item
				ItaIncidentReportData *pData = (ItaIncidentReportData *) clc.GetItemData( iItem );
				//Bugzilla Bug 175: Mintao
				if(pData != NULL)
				{
					arraySelectedIncidents.push_back(pData); // and keep it in the array
		//Mintao ++
		//Bug 652 (TD12313)
					m_selectedItemMaps.insert(std::pair<std::string, int>(pData->getReportId(), iItem));
		//Mintao ++
		//Bug 652 (TD12313)
				}
			}
		} while( iItem != -1 );          

		FUNCTION_EXIT;
		return arraySelectedIncidents.size();
	}
		//Mintao ++
		//Bug 652 (TD12313)
	//-----------------------------------------------------------------------------
	static int GetSelectedIds( CListCtrl &clc, vector<std::string> &arraySelectedIds )
	{
		FUNCTION_ENTRY("GetSelectedIds");

		// get id of all selected items of a list control
		int iItem = -1;                                         // start from begin of the list
		do {
			iItem = clc.GetNextItem( iItem, LVNI_ALL | LVNI_SELECTED );
			if ( iItem != -1 )                                  // found!
			{
				// get the data associated with this item
				ItaIncidentReportData *pData = (ItaIncidentReportData *) clc.GetItemData( iItem );
				//Bugzilla Bug 175: Mintao
				if (pData != NULL)
				{
					// and keep its ID in the array
					arraySelectedIds.push_back(pData->getReportId());           
				}
			}
		} while( iItem != -1 );          

		FUNCTION_EXIT;
		return arraySelectedIds.size();
	}
		//Mintao ++
		//Bug 652 (TD12313)

	//Mintao ++
	//Bug 652 (TD12313)
	//-----------------------------------------------------------------------------
	//TD13540 - modified parameter to follow programming guidelines
	static int GetItemFromId( CListCtrl& clc,const std::string& id )
	{
		FUNCTION_ENTRY("GetItemFromId");
		
		// TD13540 - initialize return variable with the value for item not found
		int iItem = -1; 

		int nCount = clc.GetItemCount();
		for( int i = 0; i < nCount; ++i )
		{
			ItaIncidentReportData *pData = (ItaIncidentReportData *) clc.GetItemData(i);
			if (pData != NULL)
			{
				if (pData->getReportId() == id)
				{
					// return i;
					iItem = i; // TD13540
					break;
				}
			}
		}

		FUNCTION_EXIT;
		// return -1;  // not found  
		return iItem;  //TD13540 - exit with the function's return variable
	}
	//Mintao ++
	//Bug 652 (TD12313)

	//-----------------------------------------------------------------------------
	static void SetItemInformation( CListCtrl& clc, int iItem, ItaIncidentReportData* incident )
	{
		FUNCTION_ENTRY("SetItemInformation");

		if (incident != NULL)
		{
			std::ostringstream sBuffer;

			// report ID
		//Mintao ++
		//Bug 652 (TD12313)
			//TD15204 - reverting back the changes of TD15041 ++lizettejl 
			//clc.SetItemText( iItem, ID, incident->getReportId().c_str() );
		//Mintao ++
		//Bug 652 (TD12313)

	
			//Shiva++
			//DTL-1174 - display of 7 digit number
			//clc.SetItemText( iItem, ID, incident->getDisplayReportId().c_str() );
			//Shiva++

			///Shiva++
			///Changed to save 7 digit number to database also
			/// Assumed incident report will be created only in OCC location.
			/// So only one location and not possible to have same id for different location suitation.
			clc.SetItemText( iItem, ID, incident->getReportId().c_str() );
			///Shiva++
    
			// set the created date/time column
			COleDateTime createDateTime(incident->getCreatedDateTime());
			CString createDateTimeString = createDateTime.Format("%d-%b-%y %H:%M:%S");   //TD15802
			clc.SetItemText( iItem, CREATED, createDateTimeString.GetBuffer(createDateTimeString.GetLength()));

			// set the title
			clc.SetItemText( iItem, TITLE, incident->getTitle().c_str() );

			// location //Support location display name
			if (false == incident->getLocation1().empty())
			{
				std::auto_ptr<ILocation> pLocation(LocationAccessFactory::getInstance().getLocationByName(incident->getLocation1()));
				clc.SetItemText( iItem, LOCATION, pLocation->getDisplayName().c_str() );
			}
			else
			{
				clc.SetItemText( iItem, LOCATION, incident->getLocation1().c_str() );
			}
			
			// set Track
			sBuffer.str(""); // empty
			if ( incident->getInnerTrack() )
				sBuffer <<  _T("IT");//_T("Inner"); //TD15802

			if ( incident->getOuterTrack() ){
				if ( incident->getInnerTrack() ) sBuffer <<  _T(" & ");
				sBuffer <<  _T("OT");//_T("Outer"); //TD15802
			}

			clc.SetItemText( iItem, TRACK, sBuffer.str().c_str() );

			// set Train Id
			clc.SetItemText( iItem, TRAIN, incident->getTrain().c_str() );

			//Mintao++
			// set Associated alarm
			clc.SetItemText( iItem, ALARM, incident->getAssociatedAlarmDetail().c_str() );
			//Mintao++

			// set Asset
			clc.SetItemText( iItem, ASSET, incident->getAsset().description.c_str() );

			// set Creator
			clc.SetItemText( iItem, CREATOR, incident->getCreator().c_str() );

			clc.SetItemText( iItem, PROFILE, incident->getCreatorProfile().c_str() );
			//Bugzilla Bug 158: Mintao
			//set subsystem 
			clc.SetItemText( iItem,SUBSYSTEM, incident->getAsset().subsystem.c_str());

			if (incident->isClosed())
			{
				// set the closed date/time column
				//TD15043 - do not use the created date and time as the incident's closed
				// date and time 
				//COleDateTime closeDateTime(incident->getCreatedDateTime());			
				//TD15439 - do not use the current time, rather use the closed date/time of
				// the incident report
				// COleDateTime closeDateTime = COleDateTime::GetCurrentTime();
				COleDateTime closeDateTime = incident->getCloseDate();
				//++lizettejl
				CString closeDateTimeString = closeDateTime.Format("%d-%b-%y %H:%M:%S");	//TD15802			
				clc.SetItemText( iItem, CLOSED, closeDateTimeString.GetBuffer(closeDateTimeString.GetLength()));
			}
			else
			{
				clc.SetItemText( iItem, CLOSED, "");
			}
		}

		FUNCTION_EXIT;
	}

	/////////////////////////////////////////////////////////////////////////////
	// CIncidentManagerListView

	const UINT CIncidentManagerListView::WM_APP_CREATEALARM = WM_APP + 1;
	const UINT CIncidentManagerListView::WM_APP_RELOADINCIDENTS = WM_APP + 2;
	const UINT CIncidentManagerListView::WM_APP_UPDATEINCIDENT = WM_APP + 3;
	const UINT CIncidentManagerListView::WM_APP_REMOVEINCIDENT = WM_APP + 4;
	const UINT CIncidentManagerListView::WM_APP_UPDATERIGHTS = WM_APP + 5;
	//-----------------------------------------------------------------------------
	IMPLEMENT_DYNCREATE(CIncidentManagerListView, CListView)
	//-----------------------------------------------------------------------------
	BEGIN_MESSAGE_MAP(CIncidentManagerListView, CListView)
		//{{AFX_MSG_MAP(CIncidentManagerListView)
		ON_COMMAND(ID_INCIDENT_NEW, onIncidentNew)
		ON_COMMAND(ID_INCIDENT_CLOSE, onIncidentClose)
		ON_COMMAND(ID_INCIDENT_DELETE, onIncidentDelete)
		ON_COMMAND(ID_INCIDENT_EDIT, onIncidentEdit)
		ON_COMMAND(ID_INCIDENT_VIEW, onIncidentView)
		ON_COMMAND(ID_INCIDENT_PRINT, onPrint)
		ON_WM_CLOSE()
		ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemChanged)
		ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
		ON_COMMAND(ID_INCIDENT_REOPEN, onIncidentReopen)
		//}}AFX_MSG_MAP
		// Standard printing commands
		ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)

		ON_MESSAGE( CIncidentManagerListView::WM_APP_RELOADINCIDENTS, OnReloadIncidents )
		ON_MESSAGE( CIncidentManagerListView::WM_APP_CREATEALARM, OnCreateIncident )
		ON_MESSAGE( CIncidentManagerListView::WM_APP_REMOVEINCIDENT, OnRemoveIncident )
		ON_MESSAGE( CIncidentManagerListView::WM_APP_UPDATEINCIDENT, OnUpdateIncident )
		ON_MESSAGE( CIncidentManagerListView::WM_APP_UPDATERIGHTS, OnUpdateRight )

	    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)

	END_MESSAGE_MAP()
	//-----------------------------------------------------------------------------
	BOOL CIncidentManagerListView::PreCreateWindow(CREATESTRUCT& cs)
	{
		FUNCTION_ENTRY("PreCreateWindow");
		FUNCTION_EXIT;
		return CListView::PreCreateWindow(cs);
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::OnInitialUpdate() 
	{
		FUNCTION_ENTRY("OnInitialUpdate");

		// We change the list view into report style
		// with grid lines and full row selected
		CListCtrl &clc = GetListCtrl();
		clc.ModifyStyle( NULL, LVS_REPORT | LVS_SHOWSELALWAYS);
		clc.SetExtendedStyle( LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT| LVS_EX_INFOTIP );//Bugzilla Bug 158: Mintao

		for (int i = 0; i < COLUMNS_COUNT; i++)
		{
			CString columnName;
			TA_VERIFY(columnName.LoadString(COLUMN_INFO[i].name) != 0, "String does not exist in string table");
			clc.InsertColumn (i, columnName, LVCFMT_LEFT, COLUMN_INFO[i].width);
			m_sortInfo.defineSortingSemantic(i, COLUMN_INFO[i].sortBy);
		}
		// sort callback for item data
		m_sortInfo.setCallback(this);
		// Sort columns based on label
		m_sortInfo.setCurrentSort(CREATED, AutoSortListCtrl::DESCENDING);
		// Make label column the active sorting column
		m_sortInfo.activateSort(CREATED);

		m_IncidentManagerDoc = GetDocument();

		FUNCTION_EXIT;
	}

	void CIncidentManagerListView::OnDraw(CDC* pDC)
	{
		FUNCTION_ENTRY("OnDraw");

		IncidentManagerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		FUNCTION_EXIT;
	}

	BOOL CIncidentManagerListView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		FUNCTION_ENTRY("OnPreparePrinting");
		// default preparation
		FUNCTION_EXIT;
		return DoPreparePrinting(pInfo);
	}

	void CIncidentManagerListView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		FUNCTION_ENTRY("OnBeginPrinting");
		FUNCTION_EXIT;
		// TODO: add extra initialization before printing
	}

	void CIncidentManagerListView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		FUNCTION_ENTRY("OnEndPrinting");
		FUNCTION_EXIT;
		// TODO: add cleanup after printing
	}

	/////////////////////////////////////////////////////////////////////////////
	// CIncidentManagerListView diagnostics

	#ifdef _DEBUG
	void CIncidentManagerListView::AssertValid() const
	{
		FUNCTION_ENTRY("AssertValid");
		 CView::AssertValid();
		FUNCTION_EXIT;
	}

	void CIncidentManagerListView::Dump(CDumpContext& dc) const
	{
		FUNCTION_ENTRY("Dump");
		CView::Dump(dc);
		FUNCTION_EXIT;
	}

	IncidentManagerDoc* CIncidentManagerListView::GetDocument() // non-debug version is inline
	{
		FUNCTION_ENTRY("GetDocument");
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(IncidentManagerDoc)));
		FUNCTION_EXIT;
		return (IncidentManagerDoc*)m_pDocument;
	}
	#endif //_DEBUG

	/////////////////////////////////////////////////////////////////////////////
	// CIncidentManagerListView message handlers


	void CIncidentManagerListView::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
	{
		FUNCTION_ENTRY("OnItemChanged");

		vector<ItaIncidentReportData*> *selectedData = new vector<ItaIncidentReportData*>();
		GetSelectedIncidents( GetListCtrl(), *selectedData );

		// post message to CMainFrame: Change in Incident Selection
		CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
		pMainFrame->PostMessage( CMainFrame::WM_APP_RESELECTINCIDENT, (WPARAM) selectedData, (LPARAM) 0 );

		*pResult = 0;
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		// TODO: Add your control notification handler code here
		FUNCTION_ENTRY("OnDblClk");
		onIncidentView();
		*pResult = 0; 
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
	{
		FUNCTION_ENTRY("OnUpdate");
		updateIncidentList();
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	long CIncidentManagerListView::OnReloadIncidents( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnReloadIncidents");
		updateIncidentList();
		FUNCTION_EXIT;
		return 0;
	}    
	//-----------------------------------------------------------------------------          
	long CIncidentManagerListView::OnCreateIncident( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnCreateIncident");
		// CL18904++
		try
		{
			
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"[createIncident] alarmID addr[%p] alarmID:%s", wParam, 
				reinterpret_cast<std::string*>(wParam)->c_str());
			//const string *psAlarmId = (const string*) wParam;
			//the lParam is 0, do not use this
			//const string *psAlarmValue = ( const string *) lParam; 
			onIncidentNewAlarmAssociation( *reinterpret_cast<std::string*>(wParam), "ALARM" );
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"[OnCreateIncident] reinterpret wParam error! Raise exception!!!");
		}
		// ++CL18904
		FUNCTION_EXIT;
		return 0;
	}
	//-----------------------------------------------------------------------------
	long CIncidentManagerListView::OnRemoveIncident( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnRemoveIncident");
		//Mintao ++
		//Bug 652 (TD12313)
		string topass = string((char*)wParam);
		onIncidentRemoved(topass);
		//Mintao ++
		//Bug 652 (TD12313)
		FUNCTION_EXIT;
		return 0;
	}
	//-----------------------------------------------------------------------------
	long CIncidentManagerListView::OnUpdateIncident( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnUpdateIncident");
		onIncidentUpdated( (ItaIncidentReportData*) wParam );
		FUNCTION_EXIT;
		return 0;
	}
	//-----------------------------------------------------------------------------
	long CIncidentManagerListView::OnUpdateRight( WPARAM wParam, LPARAM lParam )
	{
		FUNCTION_ENTRY("OnUpdateRight");
		//Mintao ++
		//Bug 642 
		updateIncidentList();
		//Mintao ++
		//Bug 642 
		FUNCTION_EXIT;
		return 0;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::OnClose()
	{
		FUNCTION_ENTRY("OnClose");
		onPrepareForClose();
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	CIncidentManagerListView::CIncidentManagerListView():
		m_sortInfo(COLUMNS_COUNT), m_bToolTipCtrlCustomizeDone(FALSE), m_pchTip(NULL),
		m_pwchTip(NULL)

	{
		FUNCTION_ENTRY("CIncidentManagerListView");
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	CIncidentManagerListView::~CIncidentManagerListView()
	{
		FUNCTION_ENTRY("~CIncidentManagerListView");
		//if (!m_arrayDisplayedIncidentReports.empty())
		//{
		//    std::vector<IncidentReportDialog*>::iterator it;
		//    for (it = m_arrayDisplayedIncidentReports.begin(); it != m_arrayDisplayedIncidentReports.end(); ++it)
		//    {
		//        delete *it;
		//    }
		//    m_arrayDisplayedIncidentReports.clear();
		//}
    
	 
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::addIncidentReportDialog( IncidentReportDialog* pDlg )
	{
		FUNCTION_ENTRY("AddIncidentReportDialog");
		m_arrayDisplayedIncidentReports.push_back( pDlg );
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::removeIncidentReportDialog( IncidentReportDialog* pDlg )
	{
		FUNCTION_ENTRY("RemoveIncidentReportDialog");

		// TD15865 Yanrong++
		if( pDlg != NULL )
		{
			pDlg->getIncident()->endEdit();
		}
		// ++Yanrong

		int nCount = m_arrayDisplayedIncidentReports.size();
		for( std::vector<IncidentReportDialog*>::iterator it = m_arrayDisplayedIncidentReports.begin();
				it != m_arrayDisplayedIncidentReports.end(); ++it )
			if ( *it == pDlg )
			{
				m_arrayDisplayedIncidentReports.erase(it);
				break;
			}
    
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	int CIncidentManagerListView::getTotalDisplayIncidentReportDialogs() const
	{
		FUNCTION_ENTRY("getTotalDisplayIncidentReportDialogs");
		FUNCTION_EXIT;

		return m_arrayDisplayedIncidentReports.size();
	}
	//-----------------------------------------------------------------------------
		//Mintao ++
		//Bug 652 (TD12313)
	IncidentReportDialog* CIncidentManagerListView::getIncidentReportDialogFromId(const std::string& id)
	{
		FUNCTION_ENTRY("GetIncidentReportDialogFromId");

		int nCount = m_arrayDisplayedIncidentReports.size();
		for( std::vector<IncidentReportDialog*>::iterator it = m_arrayDisplayedIncidentReports.begin();
				it != m_arrayDisplayedIncidentReports.end(); ++it )
			if ( (*it)->getIncident()->getReportId() == id )
			{
				FUNCTION_EXIT;
				return *it;
			}

		FUNCTION_EXIT;
		return NULL;    // not found
	}
	//Mintao ++
	//Bug 652 (TD12313)

	//-----------------------------------------------------------------------------
	LRESULT CIncidentManagerListView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{     
		CListCtrl &clc = GetListCtrl();

		AutoSortListCtrl::WindowProc(message, wParam, lParam, clc, m_sortInfo);
		return CListView::WindowProc(message, wParam, lParam);
	}

	int CIncidentManagerListView::sortCallback( 
							LPARAM lParam1, 
							LPARAM lParam2, 
							int columnIndex, 
							TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort )
	{
		FUNCTION_ENTRY("sortCallback");

		ItaIncidentReportData *pReport1 = reinterpret_cast<ItaIncidentReportData *>( lParam1 );
		ItaIncidentReportData *pReport2 = reinterpret_cast<ItaIncidentReportData *>( lParam2 );
		//Bugzilla Bug 175: Mintao
		TA_ASSERT(pReport1 != NULL, "sortCallback has NULL pointer ItaIncidentReportData");
		TA_ASSERT(pReport2 != NULL, "sortCallback has NULL pointer ItaIncidentReportData");

		if ( currentSort == TA_Base_Bus::AutoSortListCtrl::DESCENDING )
		{
 			// swap< ItaIncidentReportData* >( pReport1, pReport2 );
			ItaIncidentReportData* temp = pReport1;
			pReport1 = pReport2;
			pReport2 = temp;
		}

		switch( columnIndex ){            
			case CREATED:
				return pReport1->getCreatedDateTime() - pReport2->getCreatedDateTime();

			case TRACK:
				{
					//
					// grouped by Inner, Outer, and Inner/Outer
					//
					int v1 = (pReport1->getInnerTrack() ? 1 : 0) + (pReport1->getOuterTrack() ? 2 : 0);
					int v2 = (pReport2->getInnerTrack() ? 1 : 0) + (pReport2->getOuterTrack() ? 2 : 0);
					FUNCTION_EXIT;
					return (v1 - v2);
				}

			case CLOSED:
				FUNCTION_EXIT;
				return  pReport1->getCloseDate() - pReport2->getCloseDate();
        
			default:
				FUNCTION_EXIT;
				return 0;
		}
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::updateIncidentList()
	{
		FUNCTION_ENTRY("updateIncidentList");

		CListCtrl &clc = GetListCtrl();
    
		// keep a track on the selected items
		//Mintao ++
		//Bug 652 (TD12313)
		vector<std::string> arraySelectedIds;
		//Mintao ++
		//Bug 652 (TD12313)
		GetSelectedIds( clc, arraySelectedIds );
		//Bugzilla Bug 175: Mintao
		TA_ASSERT(m_IncidentManagerDoc != NULL, "updateIncidentList has NULL m_IncidentManagerDoc pointer!");
		// get new list of incidents
		vector<ItaIncidentReportData*> arrayIncidentReportData = m_IncidentManagerDoc->getIncidents();

		// clear the list in the view
		clc.DeleteAllItems();

		// check the number of incidents available in the list
		int nCount = arrayIncidentReportData.size();
		if ( nCount == 0 )
		{
			// tell CMainFrame to enable/disable items accordingly
			// because this is PostMessage, we need to allocate memory first,
			// and let the recipient delete it
			vector<ItaIncidentReportData*> *tempArray = new vector<ItaIncidentReportData*>;
			CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
			pMainFrame->PostMessage( CMainFrame::WM_APP_RESELECTINCIDENT, (WPARAM) tempArray, 0 );
		}

		if ( nCount >= MAX_INCIDENT_TO_DISPLAY )
		{
			ostringstream maxstr;
			maxstr << MAX_INCIDENT_TO_DISPLAY;
			CString maximumvalue(maxstr.str().c_str());
			TransActiveMessage userMsg;
			userMsg << maximumvalue;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_060084);
			//Bugzilla Bug 175: Mintao
			nCount = MAX_INCIDENT_TO_DISPLAY;
		}


		std::ostringstream sBuffer;     // a temporary buffer for converting number to string

		CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
		TA_ASSERT(pMainFrame != NULL, "updateIncidentList has NULL pMainFrame pointer!");
		CProgressCtrl* pProgress = pMainFrame->getMainFrameProgressCtrl();
		TA_ASSERT(pProgress != NULL, "updateIncidentList has NULL pProgress pointer!");
		pProgress->SetRange(0, nCount);
		pProgress->SetStep(1);
		pProgress->SetPos(0);
		pProgress->ShowWindow(SW_SHOW);
		// and repopulate the list
		for(int i = 0; i < nCount; ++i )
		{
			pProgress->StepIt();
			ItaIncidentReportData *pData = arrayIncidentReportData[i];
			//Bugzilla Bug 175: Mintao
			if (pData != NULL)
			{
				sBuffer.str("");            // empty buffer
				sBuffer << pData->getReportId();
        
				// insert to the list
				clc.InsertItem(i, sBuffer.str().c_str() );
				clc.SetItemData( i, (LPARAM) pData ); // associate report with the item
				SetItemInformation(clc, i, pData);
				//TD15203 - if rights has changed, notify any opened incident report dialog
				// notify the incident dialog
				if (pData != NULL)
				{				
					IncidentReportDialog *pDialog = (IncidentReportDialog *) getIncidentReportDialogFromId( pData->getReportId() );
					if (pDialog != NULL)
					{
						pDialog->onIncidentUpdate(pData);
					}
				}
				//++lizettejl
			}
		}

		// reselect the original selection on the list
		nCount = arraySelectedIds.size();
		for(int i = 0; i < nCount; ++i )
		{
			// search for the item with given Id
			LVFINDINFO findInfo;
			findInfo.flags = LVFI_PARAM;
			findInfo.lParam = (LPARAM)(arraySelectedIds[i].c_str());

			int iItem = clc.FindItem( &findInfo );
			if ( iItem >= 0 )
			{
				clc.SetItemState(   iItem, 
									LVIS_SELECTED,  // state
									LVIS_SELECTED   // mask
								);
			}
		}

		pProgress->ShowWindow(SW_HIDE);

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentDelete()
	{
		FUNCTION_ENTRY("onIncidentDelete");

		//Mintao ++
		//Bug 766
		CString action = "delete the incident report(s)";
		TransActiveMessage userMsg;
		userMsg << action;
		UINT response = userMsg.showMsgBox(IDS_UW_010003);
		if (response == IDYES)
		{
			// get all selected incidents
			CListCtrl &clc = GetListCtrl();
			vector<ItaIncidentReportData *> arraySelectedIncidents;
			int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );
    
			CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
			TA_ASSERT(pMainFrame != NULL, "updateIncidentList has NULL pMainFrame pointer!");
			CProgressCtrl* pProgress = pMainFrame->getMainFrameProgressCtrl();
			TA_ASSERT(pProgress != NULL, "updateIncidentList has NULL pProgress pointer!");

			pProgress->ShowWindow(SW_SHOW);
			pProgress->SetRange(0, nCount);
			pProgress->SetStep(1);
			pProgress->SetPos(0);

			vector<string> arrayIdsFailedToDelete;
    
			for ( int i = nCount - 1; i >= 0; --i )//Bugzilla Bug 164: Mintao
			{
				pProgress->StepIt();
				ItaIncidentReportData *pReport = arraySelectedIncidents[i];

				if ( pReport->isClosed() )
				{
					//Bugzilla Bug 175: Mintao
					TA_ASSERT (NULL != m_IncidentManagerDoc, "onIncidentDelete has NULL m_IncidentManagerDoc pointer!");

					if ( m_IncidentManagerDoc->getIncidentRights().isControlPermitted( pReport ) )
					{
						//Bugzilla Bug 164: Mintao
						//Mintao ++
						//Bug 652 (TD12313)
						map<string, int>::iterator it = m_selectedItemMaps.find(pReport->getReportId());
						//Mintao ++
						//Bug 652 (TD12313)
						if (it != m_selectedItemMaps.end())
						{
							pReport->deleteThisReport();
							clc.DeleteItem(it->second);
						}                
					}
					else    // not permitted
					{
						arrayIdsFailedToDelete.push_back( pReport->getReportId() );
					}
				}
			}

			if ( arrayIdsFailedToDelete.size() )  // at least one cannot be reopened
			{
				CString action = "delete the incident report(s)";
				TransActiveMessage userMsg;
				userMsg << action;
				UINT response = userMsg.showMsgBox(IDS_UE_010021);
			}  

			pProgress->ShowWindow(SW_HIDE);
		}
		//Mintao ++
		//Bug 766

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentNew()
	{
		//
		FUNCTION_ENTRY("onIncidentNew");

		if ( getTotalDisplayIncidentReportDialogs() >= MAX_INCIDENT_REPORT_DIALOG )
		{
			// Reach the limit, display the error
			std::ostringstream maxstr;
			maxstr << MAX_INCIDENT_REPORT_DIALOG;
			CString maximumvalue(maxstr.str().c_str());
			TransActiveMessage userMsg;
			userMsg << maximumvalue;
			UINT response = userMsg.showMsgBox(IDS_UE_060095);
    
			FUNCTION_EXIT;
			return;
		}

		//Mintao++
		//Bug 770 (TD12864)
		// Create a modeless Incident Report dialog
		IncidentReportDialog *pDlg = new IncidentReportDialog(
											NULL,   // new
											true,   // editable
											m_IncidentManagerDoc->getIncidentRights(),
											m_IncidentManagerDoc->getLocationAccessFactory(),
											m_IncidentManagerDoc->getUserMessage(),
											GetDesktopWindow());

		pDlg->Create( IDD_INCIDENT_REPORT_DIALOG, GetDesktopWindow() );
		pDlg->ShowWindow(SW_SHOW);
		pDlg->SetFocus();
		addIncidentReportDialog(pDlg);
		//Mintao++
		//Bug 770 (TD12864)
    
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentReopen()
	{
		FUNCTION_ENTRY("onIncidentReopen");
		// get all selected incidents
		CListCtrl &clc = GetListCtrl();
		vector<ItaIncidentReportData *> arraySelectedIncidents;
		int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );
    
		CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
		TA_ASSERT(pMainFrame != NULL, "updateIncidentList has NULL pMainFrame pointer!");
		CProgressCtrl* pProgress = pMainFrame->getMainFrameProgressCtrl();
		TA_ASSERT(pProgress != NULL, "updateIncidentList has NULL pProgress pointer!");
		pProgress->ShowWindow(SW_SHOW);
		pProgress->SetRange(0, nCount);
		pProgress->SetStep(1);
		pProgress->SetPos(0);
    
		//Bugzilla Bug 175: Mintao
		TA_ASSERT (NULL != m_IncidentManagerDoc, "onIncidentReopen has NULL m_IncidentManagerDoc pointer!");
		// reopen them if it is permitted
		ItaIncidentRights &incidentRights = m_IncidentManagerDoc->getIncidentRights();
		//Mintao ++
		//Bug 652 (TD12313)
		vector<string> arrayIdsFailedToReopen;
		//Mintao ++
		//Bug 652 (TD12313)
		std::string reopen;//jiangshengguang++ CL-15863
		reopen = "{";	
		for ( int i = 0; i < nCount; ++i )
		{       
			pProgress->StepIt(); // must update progress bar
			ItaIncidentReportData *pData = arraySelectedIncidents[i];
			if ( incidentRights.isControlPermitted( pData ) )
			{
				if ( pData->isClosed() )    // reopen only the closed ones
				{
					pData->reopen();
                
					// if user is viewing this report, switch to edit mode
					IncidentReportDialog* pDlg = getIncidentReportDialogFromId( pData->getReportId() );
					if ( pDlg )
					{
						pDlg->onIncidentUpdate( pData );
					}
				}
			}
			else    // not permitted to reopen. Keep its id
			{
				reopen.append(pData->getReportId());
				reopen.append(",");
				arrayIdsFailedToReopen.push_back( pData->getReportId() );
			}
		}
		reopen.erase(reopen.size()-1,1);
		reopen.append("}");

		//if ( arrayIdsFailedToReopen.size() )  // at least one cannot be reopened
		//{
		//	CString action = "reopen the incident report(s)";
		//	TransActiveMessage userMsg;
		//	userMsg << action;
		//	UINT response = userMsg.showMsgBox(IDS_UE_010021);
		//}

		//jiangshengguang++ CL-15863
		if ( arrayIdsFailedToReopen.size() )  // at least one cannot be reopened
		{
			std::ostringstream ostr;
			ostr<<"Insufficient privilege to reopen the following ID ";
			ostr<<reopen.c_str();
			MessageBox(ostr.str().c_str(),"IncidentManager");
		}
		//jiangshengguang++ CL-15863
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentClose()
	{
		FUNCTION_ENTRY("onIncidentClose");

		// get all selected incidents
		CListCtrl &clc = GetListCtrl();
		vector<ItaIncidentReportData *> arraySelectedIncidents;
		
		// TD15865 Yanrong++
		//int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );
//		vector<ItaIncidentReportData*> tmpArraySelectedIncidents;
		int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );
//		vector<ItaIncidentReportData*>::iterator iter = tmpArraySelectedIncidents.begin();
		std::string invalidId = "";
// 		for(; iter != tmpArraySelectedIncidents.end(); ++iter)
// 		{
// 			ItaIncidentReportData* pIIR = *iter;
// 			if( pIIR->canClose() )
// 			{
// 				arraySelectedIncidents.push_back(pIIR);
// 			}
// 			else
// 			{
// 				invalidId += pIIR->getReportId();
// 				invalidId += "\r\n";
// 			}
// 		}

// 		if( !invalidId.empty() )
// 		{
// 			std::string msg = "The following report(s) can not be closed,\r\n";
// 			msg += "they are edited by another operater!";
// 			msg += "These reportIds are: \r\n";
// 			msg += invalidId;
// 			AfxMessageBox(msg.c_str());
// 		}
		// ++Yanrong

		CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
		TA_ASSERT(pMainFrame != NULL, "updateIncidentList has NULL pMainFrame pointer!");
		CProgressCtrl* pProgress = pMainFrame->getMainFrameProgressCtrl();
		TA_ASSERT(pProgress != NULL, "updateIncidentList has NULL pProgress pointer!");
		pProgress->ShowWindow(SW_SHOW);
		pProgress->SetRange(0, nCount);
		pProgress->SetStep(1);
		pProgress->SetPos(0);

		//Bugzilla Bug 175: Mintao
		TA_ASSERT (NULL != m_IncidentManagerDoc, "onIncidentReopen has NULL m_IncidentManagerDoc pointer!");
		// close them
		ItaIncidentRights& incidentRights = m_IncidentManagerDoc->getIncidentRights();
		//Mintao ++
		//Bug 652 (TD12313)
		vector<string> arrayIdsFailedToClose;
		//Mintao ++
		//Bug 652 (TD12313)

		for ( int i = 0; i < nCount; ++i )
		{
			pProgress->StepIt(); // must update progress bar
			ItaIncidentReportData *pData = arraySelectedIncidents[i];
			if ( incidentRights.isControlPermitted( pData ) )
			{ 
				if ( !pData->isClosed() )   //TD15810 - close only reports that aren't closed
				{					
					pData->close();
					// if user is editing this report, switch to view mode
					IncidentReportDialog* pDlg = getIncidentReportDialogFromId( pData->getReportId() );
					if ( pDlg )
					{
						pDlg->onIncidentUpdate( pData );
					}
				}
			}
			else
			{
				arrayIdsFailedToClose.push_back( pData->getReportId() );
			}
		}

		if ( arrayIdsFailedToClose.size() )  // at least one cannot be reopened
		{
			CString action = "close the incident report(s)";
			TransActiveMessage userMsg;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_010021);
		}  

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentEdit()
	{
		FUNCTION_ENTRY("onIncidentEdit");

		// get all selected incidents
		CListCtrl &clc = GetListCtrl();
		vector<ItaIncidentReportData *> arraySelectedIncidents;
		int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );

		for ( int i = 0; i < nCount; ++i )
		{
			if ( getTotalDisplayIncidentReportDialogs() >= MAX_INCIDENT_REPORT_DIALOG )
			{
				// Reach the limit, display the error
				std::ostringstream maxstr;
				maxstr << MAX_INCIDENT_REPORT_DIALOG;
				CString maximumvalue(maxstr.str().c_str());
				TransActiveMessage userMsg;
				userMsg << maximumvalue;
				UINT response = userMsg.showMsgBox(IDS_UE_060095);
    
				return;
			}
			else
			{
				bool dlgexisted = false;
				std::vector<IncidentReportDialog*>::iterator it;
            
				for (it = m_arrayDisplayedIncidentReports.begin(); it != m_arrayDisplayedIncidentReports.end(); ++it)
				{
					if (((*it)->getIncident())->getReportId() == (arraySelectedIncidents[i])->getReportId())
					{
						dlgexisted = true;
						break;
					}
				}            
				if (dlgexisted)//already there
				{
					(*it)->cleanupDialog();
				}

				if(!(arraySelectedIncidents[i]->isStationReportValid()))
				{		
					MessageBox("Station Report Id is not valid!","Incident Manager");
					//return;					
				}

				// TD15865 Yanrong++
				arraySelectedIncidents[i]->openForEdit();
				// ++Yanrong

				//Mintao++
				//Bug 770 (TD12864)
				// Otherwise, display the report
    			IncidentReportDialog *pDlg = new IncidentReportDialog(
												arraySelectedIncidents[i],
												true,   // editable
												m_IncidentManagerDoc->getIncidentRights(),
												m_IncidentManagerDoc->getLocationAccessFactory(),
												m_IncidentManagerDoc->getUserMessage(),
												GetDesktopWindow());

				pDlg->Create( IDD_INCIDENT_REPORT_DIALOG, GetDesktopWindow() );
				addIncidentReportDialog( pDlg );                
				pDlg->ShowWindow(SW_SHOW);
				pDlg->SetFocus();
				//Mintao++
				//Bug 770 (TD12864)
			}
		}
    
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentView()
	{
		FUNCTION_ENTRY("onIncidentView");

		// get the all selected incidents
		CListCtrl &clc = GetListCtrl();
		vector<ItaIncidentReportData *> arraySelectedIncidents;
		int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );

		ItaIncidentRights& incidentRights = m_IncidentManagerDoc->getIncidentRights(); // lizettejl

		for ( int i = 0; i < nCount; ++i )
		{
			if ( getTotalDisplayIncidentReportDialogs() >= MAX_INCIDENT_REPORT_DIALOG )
			{
				// Reach the limit, display the error			
				std::ostringstream maxstr;
				maxstr << MAX_INCIDENT_REPORT_DIALOG;
				CString maximumvalue(maxstr.str().c_str());
				TransActiveMessage userMsg;
				userMsg << maximumvalue;
				UINT response = userMsg.showMsgBox(IDS_UE_060095);			
				return;
			}
			else
			{
				// lizettejl			
				ItaIncidentReportData *pData = arraySelectedIncidents[i];
				if (pData != NULL)
				{
					//check for view rights of the report before attempting to launch
					// the incident report dialog 
					if(incidentRights.isViewPermitted(*pData))
					{
						bool dlgexisted = false;
						std::vector<IncidentReportDialog*>::iterator it;
						
						for (it = m_arrayDisplayedIncidentReports.begin(); it != m_arrayDisplayedIncidentReports.end(); ++it)
						{
							if (((*it)->getIncident())->getReportId() == (arraySelectedIncidents[i])->getReportId())
							{
								dlgexisted = true;
								break;
							}
						}            
						if (dlgexisted) //already there
						{
							if ((*it)->isModified())//check whether modified by the user 
							{
								CString action = "discard changes";
								TransActiveMessage userMsg;
								userMsg << action;
								UINT msgId = userMsg.showMsgBox(IDS_UW_010003);       
								if ( msgId == IDNO )
								{
									return;
								}
							}
							(*it)->cleanupDialog();
						}

						if(!(arraySelectedIncidents[i]->isStationReportValid()))
						{		
							MessageBox("Station Report Id is not valid!","Incident Manager");
							//return;
						}

						//Mintao++
						//Bug 770 (TD12864)
						// Otherwise, display the report
						IncidentReportDialog *pDlg = new IncidentReportDialog(
							arraySelectedIncidents[i],
							false,   // view only
							m_IncidentManagerDoc->getIncidentRights(),
							m_IncidentManagerDoc->getLocationAccessFactory(),
							m_IncidentManagerDoc->getUserMessage(),
							GetDesktopWindow() );
						
						pDlg->Create( IDD_INCIDENT_REPORT_DIALOG, GetDesktopWindow() );
						pDlg->ShowWindow(SW_SHOW);
						pDlg->SetFocus();
						addIncidentReportDialog( pDlg );
						//Mintao++
						//Bug 770 (TD12864)
					}
				}
          
			}
		}
		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentNewAlarmAssociation(const std::string& alarmId, const std::string & alarmValue)
	{
		FUNCTION_ENTRY("onIncidentNewAlarmAssociation");

		if ( m_IncidentManagerDoc->getIncidentRights().isControlPermitted( (ItaIncidentReportData*) NULL ))
		{
			TA_ASSERT(m_IncidentManagerDoc != NULL, "onIncidentNewAlarmAssociation has NULL pointer m_IncidentManagerDoc!");
			//Bugzilla Bug 175: Mintao
			if (m_arrayDisplayedIncidentReports.size() <= MAX_INCIDENT_REPORT_DIALOG)
			{
				//Mintao++
				//Bug 770 (TD12864)
				IncidentReportDialog *pDlg = new IncidentReportDialog(
													alarmId,
													alarmValue,	
													true,
    												m_IncidentManagerDoc->getIncidentRights(),
													m_IncidentManagerDoc->getLocationAccessFactory(),
													m_IncidentManagerDoc->getUserMessage(),
													GetDesktopWindow() );
				// CL18904++
				if (pDlg->isAlarmExists())
				{
					pDlg->Create( IDD_INCIDENT_REPORT_DIALOG, GetDesktopWindow() );
					pDlg->ShowWindow(SW_SHOW);
					pDlg->SetFocus();
					addIncidentReportDialog( pDlg );
				}
				else
				{
					delete pDlg;
					//	IDS_UE_050071		"16|0|0|Report {0} cannot be generated."
					CString action = "for Alarm";
					TransActiveMessage userMsg;
					userMsg << action;
					UINT response = userMsg.showMsgBox(IDS_UE_050071);
				}
				// ++CL18904

				//Mintao++
				//Bug 770 (TD12864)
			}
			else
			{
				// Reach the limit, display the error
				std::ostringstream maxstr;
				maxstr << MAX_INCIDENT_REPORT_DIALOG;
				CString maximumvalue(maxstr.str().c_str());
				TransActiveMessage userMsg;
				userMsg << maximumvalue;
				UINT response = userMsg.showMsgBox(IDS_UE_060095);
			}
		}
		else
		{
// 			CString action = "create new incident report";
// 			TransActiveMessage userMsg;
// 			userMsg << action;
// 			UINT response = userMsg.showMsgBox(IDS_UE_010021);
			// CL-16128 ++
			// using MSG (IDS_UE_060096) format: 16|0|1|{0} has insufficient privilege to {1}
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_Base_Core::ISession* session = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
			std::string strOperName = session->getOperatorName();
			TA_Base_Core::IProfile* profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(session->getProfileKey());
			std::string strProfileName = profile->getName();

			std::string strOperator("Operator ");
			strOperator.append(strOperName);
			strOperator.append("/");
			strOperator.append(strProfileName);
			delete session;
			session = NULL;
			delete profile;
			profile = NULL;
			CString action = "Create Incident Report";
			TransActiveMessage userMsg;
			userMsg << strOperator;
			userMsg << action;
			UINT response = userMsg.showMsgBox(IDS_UE_060096);
			// ++ CL-16128
		}

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onPrepareForClose()
	{
		FUNCTION_ENTRY("onPrepareForClose");

		int nCount = m_arrayDisplayedIncidentReports.size();
		for ( int i = 0; i < nCount; i++ )
		{
			IncidentReportDialog *pDlg = m_arrayDisplayedIncidentReports[i];
			if (pDlg != NULL)
			{
				// lizettejl (TD13867) 
				// call immediately DestroyWindow() compared to adding a message
				// to the windows message queue. 
				pDlg->DestroyWindow(); 
				// pDlg->SendMessage( IncidentReportDialog::WM_APP_PREPAREFORCLOSE );
				// lizettejl
			}
		}

		// make sure everything is freed up 
		if (!m_arrayDisplayedIncidentReports.empty())
		{
			std::vector<IncidentReportDialog*>::iterator it;
			for (it = m_arrayDisplayedIncidentReports.begin(); it != m_arrayDisplayedIncidentReports.end(); ++it)
			{
				delete *it;
			}
			m_arrayDisplayedIncidentReports.clear();
		}

		m_selectedItemMaps.clear();

		FUNCTION_EXIT;
	}
	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onPrint()
	{
		FUNCTION_ENTRY("onPrint");
		try
		{
			// get all selected incidents
			CListCtrl &clc = GetListCtrl();
			vector<ItaIncidentReportData *> arraySelectedIncidents;
			int nCount = GetSelectedIncidents( clc, arraySelectedIncidents );
			if (nCount != 1)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Only one report can be printed at one time.");
				return;
			}

			//Mintao ++
			//Bug 643
			//The DB connection parameter is required to be passed to the report manager to print
			string dbstring = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION);
			if (dbstring.empty())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Can not get the DB connection string.");
				return;
			}

			//libo
			string dbConectinFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
			if (dbConectinFile.empty())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"Can not get the DB connection file.");
				return;
			}
			//libo
        
			//get the current incident report
			ItaIncidentReportData *pReport = arraySelectedIncidents[0];

			//get the directory path
			ostringstream idstring;
			idstring << pReport->getReportId();
			string idtest = idstring.str();

			//get the path from the ctaRuntime 
			std::string pathname = TA_Base_Core::RunParams::getInstance().get(RPARAM_REPORTTEMPLATE.c_str());
        
			if (pathname.empty())
			{
						LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error occurs while trying get the template path for the report template. Can not print!");
				return;
			}
        
			//get the actual run parameters for the reporting manager
			string appparam = STRING_TO_LAUNCH_REPORT_MANAGER + FILE_NAME_PARAM + pathname + REPORT_ID_PARAM + idstring.str() + DB_CONNECTION_STRING + dbstring + DB_CONNECTION_FILE + dbConectinFile;
			CPoint pt(GetMessagePos());
		//	AfxMessageBox(appparam.c_str());
			RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_GenericGui::SCREEN_CURRENT, TA_GenericGui::AREA_SCHEMATIC, pt.x);
			TA_Base_Bus::AppLauncher::getInstance().launchApplication(REPORT_MANAGER_GUI_APPTYPE, 
																	   appparam,
																	   TA_GenericGui::POS_NONE,
																	   TA_GenericGui::ALIGN_HORIZONTAL,
																	   NULL,
																	   &boundary);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Launch Reporting Manager for printing Incident ID: %d", pReport->getReportId());
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Libo RepartManger Paramter = %s",appparam.c_str());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ReportManger_Paramter = %s",appparam.c_str());
			//Mintao ++
			//Bug 643

		}
		catch (exception& e)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Error(%s) occurs while trying to print the incident report.", e.what());
		}
		FUNCTION_EXIT;
	}

	//-----------------------------------------------------------------------------
		//Mintao ++
		//Bug 652 (TD12313)
	//TD13540 - modified parameter to follow programming guidelines
	void CIncidentManagerListView::onIncidentRemoved(const std::string& incidentId)
	{
		FUNCTION_ENTRY("onIncidentRemoved");   
		// notify the incident dialog
		IncidentReportDialog *pDialog = (IncidentReportDialog *) getIncidentReportDialogFromId( incidentId );
		if (pDialog != NULL)
		{
			//Bugzilla Bug 175: Mintao
			CListCtrl &clc = GetListCtrl();
			int iItem = GetItemFromId( clc, incidentId );
			if (iItem < 0) return;	// not found!

			ItaIncidentReportData *pIncident = (ItaIncidentReportData *) clc.GetItemData(iItem);
			clc.DeleteItem(iItem);	// remove this item from the list view

			pDialog->onIncidentRemoved(incidentId);
			delete pIncident;       // delete the incident
			pIncident = NULL;
		}

		FUNCTION_EXIT;
	}
    //Mintao ++
    //Bug 652 (TD12313)

	//-----------------------------------------------------------------------------
	void CIncidentManagerListView::onIncidentUpdated(ItaIncidentReportData* incident)
	{
		FUNCTION_ENTRY("onIncidentUpdated");
		
		if (incident != NULL)
		{
			//Bugzilla Bug 159: Mintao
			CMainFrame *pMainFrame = (CMainFrame *) GetParentFrame();
			TA_ASSERT(pMainFrame != NULL, "updateIncidentList has NULL pMainFrame pointer!");

			// TD15865 Yanrong++
			vector<ItaIncidentReportData*> *selectedData = new vector<ItaIncidentReportData*>();
			GetSelectedIncidents( GetListCtrl(), *selectedData );

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15865 onIncidentUpdated PostMessage (WM_APP_RESELECTINCIDENT) to MainFrame" );
			pMainFrame->PostMessage( CMainFrame::WM_APP_RESELECTINCIDENT, (WPARAM) selectedData, (LPARAM) 0 );
			// ++Yanrong

			CProgressCtrl* pProgress = pMainFrame->getMainFrameProgressCtrl();
			TA_ASSERT(pProgress != NULL, "updateIncidentList has NULL pProgress pointer!");
			int thelower, theupper;
			pProgress->GetRange(thelower, theupper);
			int currentpos = pProgress->GetPos();
			if (currentpos < theupper)
			{
				pProgress->StepIt();
			}
			
			currentpos = pProgress->GetPos();
			if(currentpos == theupper)
			{
				pProgress->ShowWindow(SW_HIDE);
			}
			CListCtrl &clc = GetListCtrl();
			int iItem = GetItemFromId( clc, incident->getReportId() );
			if (iItem < 0) 
			{
				FUNCTION_EXIT; // TD13540 - added to log all function exit
				return;	// not found!
			}
			
			SetItemInformation( clc, iItem, incident );
			
			// notify the incident dialog
			IncidentReportDialog *pDialog = (IncidentReportDialog *) getIncidentReportDialogFromId( incident->getReportId() );
			
			if (pDialog != NULL)
			{
				pDialog->onIncidentUpdate(incident);
			}
		}
		
		FUNCTION_EXIT;
	}

//Mintao++
//Bug 770(TD12864)
//Remove the extra setFocus function
//Mintao++
//Bug 770(TD12864)

	void CIncidentManagerListView::PreSubclassWindow() 
	{
	 
		EnableToolTips(); // Enables list contrl ToolTip control	 
		CListView::PreSubclassWindow();
	}
	
	BOOL CIncidentManagerListView::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		UINT nID = pNMHDR->idFrom;
		
		*pResult = 0;
		
		// We want to disable automatic tooltips completely.
		if( 0 == nID )
		{
			return TRUE;
		}
		
		// I want to implement this in PreSubclassWindow(), but it crashes.
		if(!m_bToolTipCtrlCustomizeDone)
		{
			/*
			When a tooltip was displayed in CWnd::DestroyWindow, an assertion error would occur. As a result, in MFC 7.0, 
			the member variables were moved from AFX_THREAD_STATE to AFX_MODULE_THREAD_STATE: 
			//*/
			//AFX_MODULE_THREAD_STATE* pThreadState = (AFX_MODULE_THREAD_STATE *)AfxGetThreadState();
			AFX_MODULE_THREAD_STATE* pThreadState = AfxGetModuleThreadState();
// 			_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
			CToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
			
			// Set max tip width in pixel.
			// you can change delay time, tip text or background color also. enjoy yourself!
			pToolTip->SetMaxTipWidth(500);
			m_bToolTipCtrlCustomizeDone = TRUE;
		}
		
		// need to handle both ANSI and UNICODE versions of the message
		TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
		TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
		
		// Extract the row and column numbers.
		int row = ((nID-1) >> 10) & 0x3fffff ;
		int col = (nID-1) & 0x3ff;
		
		CListCtrl &clc = GetListCtrl();
		CString strTip;
		strTip = clc.GetItemText(row, col);
		
		// Convert the text as required.
		if(pNMHDR->code == TTN_NEEDTEXTA)
		{
			m_pchTip = std::auto_ptr<TCHAR>( new TCHAR[strTip.GetLength()+1] );
			lstrcpyn( m_pchTip.get(), strTip, strTip.GetLength() );
			m_pchTip.get()[strTip.GetLength()] = NULL;
			pTTTW->lpszText = (WCHAR*)m_pchTip.get();
		}
		else
		{
			m_pwchTip = std::auto_ptr<WCHAR>( new WCHAR[strTip.GetLength()+1] );
			_mbstowcsz( m_pwchTip.get(), strTip, strTip.GetLength() );
			m_pwchTip.get()[strTip.GetLength()] = NULL;
			pTTTW->lpszText = (WCHAR*)m_pwchTip.get();
		}
		
		return TRUE;
	}

	int CIncidentManagerListView::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		int row, col;
		RECT cellrect;
		row = CellRectFromPoint(point, &cellrect, &col );
		
		if ( row == -1 )
		{
			return -1;
		}
		
		// Encode the cell coordinates the ID.
		pTI->hwnd = m_hWnd;
		pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
		pTI->lpszText = LPSTR_TEXTCALLBACK;
		pTI->rect = cellrect;
		
		return pTI->uId;	
	}

	int CIncidentManagerListView::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
	{
		// Borrowed from: http://www.codeguru.com/Cpp/controls/listview/tooltiptitletip/article.php/c909/
		
		int colnum;
		
		// Make sure that the ListView is in LVS_REPORT
		if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
		{
			return -1;
		}
		
		CListCtrl &clc = GetListCtrl();

		// Get the top and bottom row visible
		int row = clc.GetTopIndex();
		int bottom = row + clc.GetCountPerPage();
		if ( bottom > clc.GetItemCount() )
		{
			bottom = clc.GetItemCount();
		}
		
		// Get the number of columns
		CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
		int nColumnCount = pHeader->GetItemCount();
		
		// Loop through the visible rows
		for ( ; row<=bottom; row++ )
		{
			// Get bounding rect of item and check whether point falls in it.
			CRect rect;
			clc.GetItemRect( row, &rect, LVIR_BOUNDS );
			if ( rect.PtInRect(point) )
			{
				// Now find the column
				for ( colnum = 0; colnum < nColumnCount; colnum++ )
				{
					int colwidth = clc.GetColumnWidth(colnum);
					if ( ( point.x >= rect.left ) && ( point.x <= (rect.left + colwidth ) ) )
					{
						RECT rectClient;
						clc.GetClientRect( &rectClient );
						if ( col ) *col = colnum;
						rect.right = rect.left + colwidth;
						
						// Make sure that the right extent does not exceed the client area
						if( rect.right > rectClient.right )
						{
							rect.right = rectClient.right;
						}
						*cellrect = rect;
						return row;
					}
					rect.left += colwidth;
				}
			}
		}
		return -1;
	}
	
}// end TA_IRS_App
