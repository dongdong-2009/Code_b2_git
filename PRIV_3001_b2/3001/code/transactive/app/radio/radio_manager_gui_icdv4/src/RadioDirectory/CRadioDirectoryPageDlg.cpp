/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.cpp $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioManager.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/CRadioDirectoryPageDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/RadioDirectoryPageDlgStrategyFactory.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/CResourceButton.h"
#include "app\radio\radio_manager_gui_icdv4\src\RadioSubsMonitoringPage\CRadioSubsResourceStatic.h"

#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#include <afxdisp.h >       // MFC OLE automation classes
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioDirectoryPageDlg dialog
using namespace TA_Base_Bus;

short CRadioDirectoryPageDlg::DGNA_INDEX = 0;
short CRadioDirectoryPageDlg::RADIO_INDEX = 1;
short CRadioDirectoryPageDlg::TALKGROUP_INDEX = 2;

CRadioDirectoryPageDlg::CRadioDirectoryPageDlg(CWnd* pParent /*=NULL*/)
: CDialog(CRadioDirectoryPageDlg::IDD, pParent),
  m_currentPage(0),
  m_currentRadioResourceType(RR_DYNGROUP),
  m_pRadioDirectoryPageDlgStrategy(NULL),
  m_bDragging(false),
  m_pDragImage(NULL),
  m_selectedFromDirectory(false)
{
	//{{AFX_DATA_INIT(CRadioDirectoryPageDlg)
	//}}AFX_DATA_INIT
}

CRadioDirectoryPageDlg::~CRadioDirectoryPageDlg()
{
	if (m_pRadioDirectoryPageDlgStrategy != NULL)
	{
		delete m_pRadioDirectoryPageDlgStrategy;
		m_pRadioDirectoryPageDlgStrategy = NULL;
	}
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_UPDATED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
//TD18869
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_BEING_DELETED);
//TD18869	
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_ENTITY_EMPTY_SELECTION);
}

void CRadioDirectoryPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioDirectoryPageDlg)
	DDX_Control(pDX, IDC_EDIT1, m_filterELName);
	DDX_Control(pDX, IDC_FILTER_ID, m_filterELID);
	DDX_Control(pDX, IDC_TAB1, m_directoryTabCtrl);
	DDX_Control(pDX, IDC_DIRECTORY_LIST, m_directoryListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioDirectoryPageDlg, CDialog)
	//{{AFX_MSG_MAP(CRadioDirectoryPageDlg)
	ON_MESSAGE(WM_ENTITY_EDIT_MODE,onNotifiedEditMode)
	ON_MESSAGE(WM_ENTITY_UPDATED,onNotifiedEntityUpdated)
	ON_MESSAGE(WM_OPERATION_MODE,onNotifiedOperationMode)
	ON_MESSAGE(WM_DATABASE_READY,onNotifiedDatabaseReady)
	ON_MESSAGE(WM_ENTITY_BEING_CREATED,onNotifiedEntityBeingCreated)
	ON_MESSAGE(WM_ENTITY_SELECTION_CHANGED,onNotifiedEntitySelectionChanged)
//TD18869
	ON_MESSAGE(WM_ENTITY_BEING_DELETED,onNotifiedEntityDeleted)
//TD18869
	ON_MESSAGE(WM_ENTITY_EMPTY_SELECTION, onNotifiedEntityEmptySelection)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_DIRECTORY_LIST, OnBegindragList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTabSelChange)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TAB1, OnTabSelChanging)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_DIRECTORY_LIST, OnItemchanged)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_EN_CHANGE(IDC_FILTER, OnChangeFilterName)
	ON_EN_CHANGE(IDC_FILTER_ID, OnChangeFilterID)
	ON_EN_SETFOCUS(IDC_FILTER, OnSetfocusFilterName)
	ON_EN_SETFOCUS(IDC_FILTER_ID, OnSetfocusFilterID)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioDirectoryPageDlg message handlers

BOOL CRadioDirectoryPageDlg::OnInitDialog() 
{
    FUNCTION_ENTRY("OnInitDialog");

	CDialog::OnInitDialog();
	
	// set TabControl headers
	// peter.wong
    RadioResourceType tabId[] = { 
		RR_DYNGROUP, RR_RADIO, RR_TALKGROUP, RR_PATCHGROUP, RR_MSELECT };
		//RR_RADIO, RR_TALKGROUP, RR_ANNGROUP, RR_PATCHGROUP,
		//RR_DYNGROUP, RR_MSELECT, RR_SITE, RR_MSITE };

    DirectoryTabState state;
    state.enabled = true;

	for (int i = 0; i < RADIO_TABS_NUM; ++i )
    {
        // load the tab title from resource
		m_directoryTabCtrl.InsertItem(i, L"      " +  CRadioHelper::getTypeText(tabId[i]) + L"      ");

        state.type = tabId[i];
        m_tabStates[i] = state;
	} 

    m_directoryTabCtrl.Install(&m_directoryTabCtrl);
	//m_directoryTabCtrl.SetItemSize(*(new CSize(10,343/5)));

	// initialize FilterListControl
	m_directoryListCtrl.SetExtendedStyle( m_directoryListCtrl.GetExtendedStyle() | 
										  LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	std::vector<TA_Base_Bus::MultiFilterListCtrl::ColumnAttributes> columnAttrList;
	TA_Base_Bus::MultiFilterListCtrl::ColumnAttributes columnAttr;

	CString sColumn;
	sColumn.LoadString(IDS_COL_HEADER_DESC);
	columnAttr.name = sColumn;
	columnAttr.width = 165;
	columnAttr.isFilterable = true;
	columnAttr.sortBy = AutoSortListCtrl::ESortType::BY_STRING;
	columnAttr.editFilter = &m_filterELName;
	columnAttrList.push_back(columnAttr);

	sColumn.LoadString(IDS_COL_HEADER_ID);
	columnAttr.name = sColumn;
	columnAttr.width = 0; // auto size below
	columnAttr.isFilterable = true;
	columnAttr.editFilter = &m_filterELID;
	columnAttrList.push_back(columnAttr);
	columnAttr.sortBy = AutoSortListCtrl::ESortType::BY_INTEGER;
	//Fixed TD14301 by adding the true to enable the case sensitiva in filter edit window
	m_directoryListCtrl.initialise(columnAttrList, false);
    m_directoryListCtrl.setAutoSizeColumn(1);
	m_directoryListCtrl.setRadioResourceType(RR_DYNGROUP);

	m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(RR_DYNGROUP);
	IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
	pDirectoryPageStrategy->initialise(*this);


	// register user-defined messages
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EDIT_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_UPDATED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_OPERATION_MODE);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_DATABASE_READY);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_BEING_CREATED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_SELECTION_CHANGED);
//TD18869
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_BEING_DELETED);
//TD18869
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_ENTITY_EMPTY_SELECTION);

	refreshList = false;
	descending = false;

	resTypeEditMode = RR_NIL;

	FUNCTION_EXIT;

	return TRUE; 
}


void CRadioDirectoryPageDlg::OnBegindragList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnBegindragList");
    
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;


    CBitmap bitmap;
	bitmap.LoadBitmap(IDB_DRAG_RADIO);
    BITMAP bitmapInfo;
    bitmap.GetBitmap(&bitmapInfo);

	m_pDragImage = new CImageList();
    m_pDragImage->Create(IDB_DRAG_RADIO, bitmapInfo.bmWidth, 0, RGB(255, 255, 255));
  
	// Change the cursor to the drag image
	//	(still must perform DragMove() in OnMouseMove() to show it moving)
	m_pDragImage->BeginDrag(0, CPoint(-8, -12)); //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
	m_pDragImage->DragEnter(GetDesktopWindow(), pNMListView->ptAction);
	
	// Set dragging flag 
	m_bDragging = TRUE;	

	// Capture all mouse messages
	SetCapture (); 
	
	*pResult = 0;

	FUNCTION_EXIT;
}



void CRadioDirectoryPageDlg::OnMouseMove(UINT nFlags, CPoint point) 
{	
	if (m_bDragging)
	{
		CPoint pt(point);	//current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates

		//	lets check if this one wants use
		CWnd* iPossibleWnd = WindowFromPoint(pt);

		if ( iPossibleWnd != NULL )
		{
			if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
			{
				if (RR_TALKGROUP == m_selectedResource.type && RR_DYNGROUP == resTypeEditMode) {
					SetCursor(LoadCursor(NULL, IDC_NO));
				} else {
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
			}
			else if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CButton)))
			{
				CResourceButton* pButton = dynamic_cast<CResourceButton*>(iPossibleWnd);
				if (pButton != NULL)
				{
					SetCursor(LoadCursor(NULL, IDC_ARROW));
				}
				else
				{
					SetCursor(LoadCursor(NULL, IDC_NO));
				}
			}
			else if (iPossibleWnd->IsKindOf(RUNTIME_CLASS(CEdit)) && IDC_DG_TALKGROUP == ((CEdit *)iPossibleWnd)->GetDlgCtrlID() && RR_TALKGROUP == m_selectedResource.type && RR_DYNGROUP == resTypeEditMode) 
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}			
			else if(iPossibleWnd->IsKindOf(RUNTIME_CLASS(CStatic)) && IDC_MONITORING_TEXT == ((CStatic *)iPossibleWnd)->GetDlgCtrlID())
			{
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
			else
			{
				//If we are not hovering over a CListCtrl or CResourceButton, change the cursor
				// to note that we cannot drop here
				SetCursor(LoadCursor(NULL, IDC_NO));
			}
		}

		//move the drag image to those coordinates
		m_pDragImage->DragMove(pt); //move the drag image to those coordinates
		m_pDragImage->DragShowNolock(true);

    }
	
	CDialog::OnMouseMove(nFlags, point);
}



void CRadioDirectoryPageDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
    FUNCTION_ENTRY("OnLButtonUp");
	if (m_bDragging)
	{
		ReleaseCapture ();
		
		m_bDragging = false;

		m_pDragImage->DragLeave (GetDesktopWindow ());
		m_pDragImage->EndDrag();
		
		delete m_pDragImage; 

		CPoint pt(point);	//current mouse coordinates
		ClientToScreen(&pt); //convert to screen coordinates

		//	lets check if this one wants use
		CWnd* iPossibleWnd = WindowFromPoint(pt);

		if ( iPossibleWnd != NULL )
		{
			//	check if this is one of us...
			if(iPossibleWnd->IsKindOf(RUNTIME_CLASS(CListBox)))
			{

				int MAX_MEMBERS = 0;
				switch (resTypeEditMode) {
					case RR_DYNGROUP:
						MAX_MEMBERS = 10; break;
					case RR_MSELECT:
						MAX_MEMBERS = 20; break;
					case RR_PATCHGROUP:
						MAX_MEMBERS = 12; break;
					default:
						MAX_MEMBERS = 0;
				}
				if (MAX_MEMBERS < ((CListBox *)iPossibleWnd)->GetCount()) {
					AfxMessageBox("Max number of Members reached");
					CDialog::OnLButtonUp(nFlags, point);
					return;
				}

#if 0		//TD18696 marc_bugfix implementation for multiple entry dragging 
				int pos = ((CListBox *)iPossibleWnd)->FindStringExact(-1,m_selectedResource.alias.c_str());
				//	could check if its already present...
				if ( pos == LB_ERR )
				{
					// allowed resources that can be added to members of configurable entity
					if ( (m_selectedResource.type == RR_RADIO && m_tabStates[TALKGROUP_INDEX].enabled) || (m_selectedResource.type == RR_TALKGROUP && !m_tabStates[RADIO_INDEX].enabled))
					{
						int index = ((CListBox *)iPossibleWnd)->GetCount();
						
						((CListBox *)iPossibleWnd)->InsertString(index,m_selectedResource.alias.c_str());	
						
						int x = m_directoryListCtrl.GetSelectionMark();
						//((CListBox *)iPossibleWnd)->SetItemDataPtr(index,&m_ResourceTypeCollectionarray[x]);
						((CListBox *)iPossibleWnd)->SetItemData(index,m_selectedResource.id);
					}
					else
					{
						AfxMessageBox("Cannot add this resource");
					}
				}
				else
				{
					AfxMessageBox("Resource is already in the list.");
				}
#else
				int curItemCount = ((CListBox *)iPossibleWnd)->GetCount();
				POSITION pos = m_directoryListCtrl.GetFirstSelectedItemPosition();
				static TCHAR tBuf[50];
				{
					while(pos)
					{
						if (curItemCount >= MAX_MEMBERS) {
							curItemCount = -1;
							break;
						}
						int nItem = m_directoryListCtrl.GetNextSelectedItem(pos);

						std::string alias(m_directoryListCtrl.GetItemText(nItem, 0));
						std::string id(m_directoryListCtrl.GetItemText(nItem, 1));

						int pos_listbox = ((CListBox *)iPossibleWnd)->FindStringExact(-1,alias.c_str());
						//	could check if its already present...
						if ( pos_listbox == LB_ERR )
						{
							// allowed resources that can be added to members of configurable entity
							if ( (m_selectedResource.type == RR_RADIO && m_tabStates[TALKGROUP_INDEX].enabled) || (m_selectedResource.type == RR_TALKGROUP && !m_tabStates[RADIO_INDEX].enabled))
							{
								int index = ((CListBox *)iPossibleWnd)->GetCount();
								
								((CListBox *)iPossibleWnd)->InsertString(index,alias.c_str());	
								
								int x = m_directoryListCtrl.GetSelectionMark();
								//((CListBox *)iPossibleWnd)->SetItemDataPtr(index,&m_ResourceTypeCollectionarray[x]);
								((CListBox *)iPossibleWnd)->SetItemData(index,atol(id.c_str()));

								curItemCount++;
							}
							else
							{
								wsprintf(tBuf, "Cannot add resource %ld", atol(id.c_str()));
								AfxMessageBox(tBuf);
							}
						}
						else
						{
							wsprintf(tBuf, "Resource %ld is already on the list.", atol(id.c_str()));
							AfxMessageBox(tBuf);
						}
					}
					if (-1 == curItemCount) {
						wsprintf(tBuf, "Max number of Members (%ld) reached", MAX_MEMBERS);
						::MessageBox(this->m_hWnd, tBuf, "Warning Message", MB_OK|MB_ICONWARNING);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, tBuf);
					}
				}

				
#endif
			}
			else if(iPossibleWnd->IsKindOf(RUNTIME_CLASS(CButton)))
			{
				CResourceButton* pButton = dynamic_cast<CResourceButton*>(iPossibleWnd);
				
				if (pButton != NULL)
				{
					pButton->OnDropResource(m_selectedResource);
				}
			}
			else if(iPossibleWnd->IsKindOf(RUNTIME_CLASS(CStatic)))
			{
				CRadioSubsResourceStatic* pStatic = dynamic_cast<CRadioSubsResourceStatic*>(iPossibleWnd);
				if (pStatic != NULL) 
				{
					pStatic->onDropResource(m_selectedResource);
				}
			}
			else if(iPossibleWnd->IsKindOf(RUNTIME_CLASS(CEdit)) && IDC_DG_TALKGROUP == ((CEdit *)iPossibleWnd)->GetDlgCtrlID() && RR_TALKGROUP == m_selectedResource.type) 
			{
				POSITION pos = m_directoryListCtrl.GetFirstSelectedItemPosition();
				if (NULL != pos) {
					int nItem = m_directoryListCtrl.GetNextSelectedItem(pos);
					std::string alias(m_directoryListCtrl.GetItemText(nItem, 0));
					std::string id(m_directoryListCtrl.GetItemText(nItem, 1));
					((CEdit *)iPossibleWnd)->SetWindowText(alias.c_str());

					if (LB_ERR != m_directoryListCtrl.GetNextSelectedItem(pos)) {
						::MessageBox(this->m_hWnd, "Multiple Talkgroup selected", "DGNA Talkgroup", MB_OK|MB_ICONWARNING);
					}
				}
			}
		}
	}
    
	CDialog::OnLButtonUp(nFlags, point);

	FUNCTION_EXIT;
}

void CRadioDirectoryPageDlg::OnTabSelChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
/*	int nFocus = m_directoryTabCtrl.GetCurFocus();

	if (m_tabStates[nFocus].enabled)
	{
		*pResult = 0;
	}
	else
	{
		*pResult = 1;
	}*/
}


void CRadioDirectoryPageDlg::OnTabSelChange(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnTabSelChange");

    m_filterELName.SetWindowText("");
    m_filterELID.SetWindowText("");

	// Indicates that tab has changed
	int nPage = m_directoryTabCtrl.GetCurSel();

    // if page is not disabled
	
    if (m_tabStates[nPage].enabled)
    {
		// delete m_pRadioDirectoryPageDlgStrategy associated with the previously selected resourceType
		if ( m_pRadioDirectoryPageDlgStrategy != NULL )
		{
			delete m_pRadioDirectoryPageDlgStrategy;
			m_pRadioDirectoryPageDlgStrategy = NULL;
		}		

		// update m_currentPage and m_currentRadioResourceType
		m_currentPage = nPage;

		m_currentRadioResourceType = m_tabStates[nPage].type;
		m_directoryListCtrl.setRadioResourceType(m_currentRadioResourceType);

		// Get m_pRadioDirectoryPageDlgStrategy based on current resource type
		// to populate filter list

		m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(m_currentRadioResourceType);

		if ( m_pRadioDirectoryPageDlgStrategy != NULL )
		{
			
			m_directoryListCtrl.ModifyStyle(0, LVS_SINGLESEL, 0);
			IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
			pDirectoryPageStrategy->initialise(*this);
		}
	}
	else
	{
		// Page is currently disabled
		// activate previously selected tab
		m_directoryTabCtrl.SetCurSel(m_currentPage);
	}

	*pResult = 0;
	FUNCTION_EXIT;
}


void CRadioDirectoryPageDlg::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnItemchanged");

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (m_pRadioDirectoryPageDlgStrategy != NULL) 
	{
        if (pNMListView->uChanged & LVIF_STATE) // only process state change
        {
            if (pNMListView->uNewState & LVIS_SELECTED) // only care for a new selection
            {
                int nItem = pNMListView->iItem;
                
				char alias[50] ;
				m_directoryListCtrl.GetItemText(nItem,0,alias,sizeof(alias));                

				std::vector<RadioResource>::iterator it = m_ResourceTypeCollectionarray.begin();
				for (; it!=m_ResourceTypeCollectionarray.end();++it)
				{
					if(it->alias == alias)
						break;
				}
				if (it == m_ResourceTypeCollectionarray.end()) // not found
				{
					return;
				} 
				// it has no use
				//IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
				The_RadioOperations::instance()->entitySelectionChange(*it, false);

				m_selectedFromDirectory = true;
				// store selected resource for drag&drop operation
				m_selectedResource.type = it->type;
				m_selectedResource.alias = it->alias;
				m_selectedResource.id = it->id;
				m_selectedResource.ssi = it->ssi;

            }
        }
	}

	*pResult = 0;
	
	FUNCTION_EXIT;
}


afx_msg LRESULT CRadioDirectoryPageDlg::onNotifiedEditMode(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedEditMode");
    
	if (wParam) // on Dispatch_Not_Ready Msg
	{
		EnableWindow(FALSE);
		return 0;
	}

	RadioResource* r = (RadioResource*) lParam;
	if ( r->type != RR_NIL )
	{
		resTypeEditMode = r->type;
		RadioResourceType enableType;

		// Tabs to be enabled for configuring the selected resource
		switch(r->type)
		{
			case RR_DYNGROUP:
				enableType = RR_RADIO;
				break;
			case RR_MSELECT:
				enableType = RR_TALKGROUP;
				break;
			case RR_MSITE:
				enableType = RR_SITE;
				break;
			case RR_PATCHGROUP:
				enableType = RR_TALKGROUP;
				break;

			default:
                // nothing to be disabled
                return 0;
		}

		//Disable tabs not available for modification of current entity being edited.
		for (int i = 0; i < RADIO_TABS_NUM; ++i)
		{
            if (enableType != m_tabStates[i].type)
            {
                m_tabStates[i].enabled = false;
                m_directoryTabCtrl.EnableItem(i, FALSE);
            }
			else
			{
				m_currentPage = i;
			}
		}
		// Exception for DGNA, two directory list enabled
		if (RR_DYNGROUP == r->type) {
			m_tabStates[TALKGROUP_INDEX].enabled = true;
			m_directoryTabCtrl.EnableItem(TALKGROUP_INDEX, TRUE);
		}
		m_directoryListCtrl.ModifyStyle(LVS_SINGLESEL, 0, 0);

		if ( m_currentPage != m_directoryTabCtrl.GetCurSel())
		{
			m_directoryTabCtrl.SetCurSel(m_currentPage);

			// delete m_pRadioDirectoryPageDlgStrategy associated with the previously selected resourceType
			delete m_pRadioDirectoryPageDlgStrategy;
			m_pRadioDirectoryPageDlgStrategy = NULL;

			m_currentRadioResourceType = enableType;
			m_directoryListCtrl.setRadioResourceType(m_currentRadioResourceType);

			// refresh directory list
			m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(m_currentRadioResourceType);
			TA_ASSERT(m_pRadioDirectoryPageDlgStrategy!= NULL, "m_pRadioDirectoryPageDlgStrategy is NULL");


			IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
			pDirectoryPageStrategy->initialise(*this);
		}
		
		m_filterELName.EnableWindow(FALSE);
		m_filterELID.EnableWindow(FALSE);
	}

	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CRadioDirectoryPageDlg::onNotifiedOperationMode(WPARAM wParam, LPARAM lParam)
{
	//Re-enable all tabs.
    for (int i = 0; i < RADIO_TABS_NUM; ++i)
    {
        m_tabStates[i].enabled = true;
        m_directoryTabCtrl.EnableItem(i, TRUE);
    }
	
	m_filterELName.EnableWindow(TRUE);
	m_filterELID.EnableWindow(TRUE);
	m_directoryListCtrl.ModifyStyle(0, LVS_SINGLESEL, 0);

	resTypeEditMode = RR_NIL;

	EnableWindow();
	
	return 0;

}

afx_msg LRESULT CRadioDirectoryPageDlg::onNotifiedEntityUpdated(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedEntityUpdated");

	RadioResource* resource = (RadioResource*)wParam;

	if(m_currentRadioResourceType == resource->type)
	{
		if ( m_pRadioDirectoryPageDlgStrategy != NULL )
		{
			IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
			pDirectoryPageStrategy->initialise(*this);
		}
	}

	FUNCTION_EXIT;
	return 0;
}

afx_msg LRESULT CRadioDirectoryPageDlg::onNotifiedDatabaseReady(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onNotifiedEntityUpdated");

    delete m_pRadioDirectoryPageDlgStrategy;
    m_pRadioDirectoryPageDlgStrategy = NULL;
    
	// Get m_pRadioDirectoryPageDlgStrategy based on current resource type
	// to populate filter list
	m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(m_currentRadioResourceType);

	if ( m_pRadioDirectoryPageDlgStrategy != NULL )
	{
		IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
		pDirectoryPageStrategy->initialise(*this);
	}

    FUNCTION_EXIT;
	return 0;
}


void CRadioDirectoryPageDlg::setFilterListRowData()
{
    FUNCTION_ENTRY("setFilterListRowData");
    
	// populate	FilterList
	std::vector<TA_Base_Bus::FilterListCtrl::RowData> rowData;

	std::vector<RadioResource>::iterator iter = m_ResourceTypeCollectionarray.begin();
	
	int nIndex = 0;
	for ( iter = m_ResourceTypeCollectionarray.begin(); iter!= m_ResourceTypeCollectionarray.end(); ++iter)
	{
		TA_Base_Bus::FilterListCtrl::RowData row;

		row.data.push_back((*iter).alias.c_str());

		std::ostringstream num;

        switch (iter->type)
        {
        case RR_RADIO:
        case RR_SITE:
        case RR_TALKGROUP:
        case RR_ANNGROUP:
//TD18975 marc_bugfix
            //num << iter->ssi;
			num << iter->id;
//TD18975
            break;

        }
//		num << (*iter).id; // TEMP: display SSI
		row.data.push_back(num.str().c_str());

		row.callback = NULL;

		rowData.push_back(row);
	}
		
	// set all row data to filter list
	m_directoryListCtrl.setData(rowData);
	m_directoryListCtrl.refresh();

	FUNCTION_EXIT;
}

void CRadioDirectoryPageDlg::OnChangeFilterName() 
{
	m_directoryListCtrl.refresh();
}

void CRadioDirectoryPageDlg::OnChangeFilterID() 
{
	m_directoryListCtrl.refresh();
}

//TD18869
void CRadioDirectoryPageDlg::focusDirectoryActiveResource(RadioResource &resource)
{
	int m_currentPage;	

	for (int i = 0; i < RADIO_TABS_NUM; ++i)
	{
        if (resource.type == m_tabStates[i].type)
        {
			m_currentPage = i;
			break;
        }
	}

	m_directoryTabCtrl.SetCurSel(m_currentPage);

	// delete m_pRadioDirectoryPageDlgStrategy associated with the previously selected resourceType
	delete m_pRadioDirectoryPageDlgStrategy;
	m_pRadioDirectoryPageDlgStrategy = NULL;

	m_currentRadioResourceType = resource.type;
	m_directoryListCtrl.setRadioResourceType(m_currentRadioResourceType);

	// refresh directory list
	m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(m_currentRadioResourceType);
	TA_ASSERT(m_pRadioDirectoryPageDlgStrategy!= NULL, "m_pRadioDirectoryPageDlgStrategy is NULL");


	IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
	pDirectoryPageStrategy->initialise(*this);

	//once list is initialized, set focus to selected entity
	LVFINDINFO info;
	int nIndex;

	info.flags = LVFI_STRING;
	info.psz = resource.alias.c_str();

    if ((nIndex=m_directoryListCtrl.FindItem(&info)) != -1)
	{
		m_directoryListCtrl.SetFocus();
		m_directoryListCtrl.SetItemState(nIndex, LVIS_SELECTED, LVIS_SELECTED);
	}

}


//TD18869

LRESULT CRadioDirectoryPageDlg::onNotifiedEntityBeingCreated(WPARAM wParam, LPARAM unused)
{
	RadioResource* resource = (RadioResource*)wParam;

	if ( resource->type != RR_NIL)
	{
		m_selectedResource.id = resource->id;
		m_selectedResource.type = resource->type;
		m_selectedResource.alias = resource->alias;
		m_selectedResource.ssi = resource->ssi;
//TD18869
		focusDirectoryActiveResource(m_selectedResource);
//TD18869
	}
	return 0;
}


LRESULT CRadioDirectoryPageDlg::onNotifiedEntitySelectionChanged(WPARAM wParam, LPARAM unused)
{
	RadioResource* resource = (RadioResource*)wParam;

	if ( resource->type != RR_NIL || resource->alias[0] != '\0')
	{
		// TODO: add implementation here to select the Item in m_directoryListCtrl
		// already implemented in TelephoneManager DirectoryPage; 			
		// TD18165, 18176
		m_selectedResource.id = resource->id;
		m_selectedResource.type = resource->type;
		m_selectedResource.alias = resource->alias;
		m_selectedResource.ssi = resource->ssi;
		
		if (!m_selectedFromDirectory)
		{
			focusDirectoryActiveResource(m_selectedResource);
		}
		else
		{
			m_selectedFromDirectory = false;
		}
	}

	return 0;
}

BOOL CRadioDirectoryPageDlg::PreTranslateMessage(MSG* pMsg) 
{
	// Haipeng Jiang added this function to make sure the GUI are not halted on once the user input 
	// "Enter" key in the filter edit window.
	if (pMsg->hwnd == this->m_filterELName.m_hWnd || pMsg->hwnd == this->m_filterELID.m_hWnd || 
		pMsg->hwnd == this->m_directoryListCtrl.m_hWnd)
	{
		if(pMsg->message == WM_KEYDOWN &&
		   pMsg->wParam == VK_RETURN )
		{
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void CRadioDirectoryPageDlg::OnSetfocusFilterName()
{
	m_filterELID.SetWindowText("");
}

void CRadioDirectoryPageDlg::OnSetfocusFilterID()
{
	m_filterELName.SetWindowText("");
}
//TD18869
LRESULT CRadioDirectoryPageDlg::onNotifiedEntityDeleted(WPARAM wParam , LPARAM unused )
{
	RadioResourceType rType = (RadioResourceType)wParam;

	switch(rType)
	{
	case RR_DYNGROUP:
	case RR_MSELECT:
	case RR_MSITE:
	case RR_PATCHGROUP:
		break;
	default:
		//Only group entities can be deleted
		return 0;
	}

	//
	int cur_sel = m_directoryTabCtrl.GetCurSel();

    if ((cur_sel != -1) && (rType == m_tabStates[cur_sel].type))
    {
		//need to refresh list control
		// delete m_pRadioDirectoryPageDlgStrategy associated with the previously selected resourceType
		delete m_pRadioDirectoryPageDlgStrategy;
		m_pRadioDirectoryPageDlgStrategy = NULL;

		// refresh directory list
		m_pRadioDirectoryPageDlgStrategy  = RadioDirectoryPageDlgStrategyFactory::getRadioDirectoryPageDlgStrategy(rType);
		TA_ASSERT(m_pRadioDirectoryPageDlgStrategy!= NULL, "m_pRadioDirectoryPageDlgStrategy is NULL");

		IRadioDirectoryPageDlgStrategy* pDirectoryPageStrategy = const_cast<IRadioDirectoryPageDlgStrategy*>(m_pRadioDirectoryPageDlgStrategy);
		pDirectoryPageStrategy->initialise(*this);

	}
	return 0;
}

LRESULT CRadioDirectoryPageDlg::onNotifiedEntityEmptySelection(WPARAM pRadioResourceType, LPARAM unused)
{
	FUNCTION_ENTRY("onNotifiedEntityEmptySelection");
	
	m_selectedFromDirectory = true;
	// store selected resource for drag&drop operation
	m_selectedResource.type = (RadioResourceType) pRadioResourceType;
	m_selectedResource.alias = "";
	m_selectedResource.id = 0;
	m_selectedResource.ssi = 0;

	FUNCTION_EXIT;
	return 0;
}