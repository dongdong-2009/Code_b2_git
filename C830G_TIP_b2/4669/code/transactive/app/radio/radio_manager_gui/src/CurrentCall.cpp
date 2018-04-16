/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/CurrentCall.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/02/10 16:25:11 $
  * Last modified by:  $Author: builder $
  *
  */
// CurrentCall.cpp: implementation of the CCurrentCall class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "radiomanager.h"
#include "CurrentCall.h"
#include "RadioManagerDlg.h"
#include "RmLayoutHelper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define WM_REFRESH_REQUEST ( WM_USER + 1 )

BEGIN_MESSAGE_MAP(CCurrentCall, RadioCurrentCallListCtrl)
	//{{AFX_MSG_MAP(CCurrentCall)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGING, onItemchangingCurrentCallList)
    ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, onItemDeleted)
    ON_MESSAGE(WM_REFRESH_REQUEST, onRefreshRequest)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//static CCurrentCall  s_Instance;     // Singleton
CCurrentCall* CCurrentCall::GetInstance(void)
{
	return &((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->m_CurrentCallList;
}


CCurrentCall::CCurrentCall() 
: 
RadioCurrentCallListCtrl(), 
m_headingsDisplayed(false),
m_internalStateChange(false)
{    
	m_currentCallID = 0;
	m_callDisplayed = false;
	//m_currentCallType = CRadioSession::ERadioCallType::SDS_CALL;
}

CCurrentCall::~CCurrentCall()
{
}


BOOL CCurrentCall::onItemDeleted(NMHDR* pNMHDR, LRESULT* pResult) 
{    
    *pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    DWORD state = GetItemState(pNMListView->iItem, LVIS_SELECTED);

    // Item about to be removed is being deselected
    if ((state & LVIS_SELECTED) == LVIS_SELECTED)
    {
        int vIndex = GetVirtualIndexFromActual(pNMListView->iItem);

        // If a parent node, means this call being removed from list
        if (0 == getVItemLevel(vIndex))
        {
            DWORD callID = GetVItemData(vIndex);
            m_callGroupsSelected.erase(callID);        
            onListRefreshed();
        }
    }

    return FALSE;
}


BOOL CCurrentCall::onItemchangingCurrentCallList(NMHDR* pNMHDR, LRESULT* pResult) 
{    
    *pResult = 0;

    if (isWithinCustomDraw() )
    {
        // Don't care about notification received while drawing update is being performed
        return FALSE;
    }

    if (m_internalStateChange)
    {
        // Block notifications being sent back to parent
        return TRUE;
    }

    // The following restrictions are present on selection:
    // There must always be at least one item selected
    // Full duplex calls are single selection
    // Half duplex calls are multiple selection
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	// Only interested in state changes
	if (pNMListView->uChanged != LVIF_STATE) 
	{
		return FALSE;
	}
    
    int vIndex = GetVirtualIndexFromActual(pNMListView->iItem);

    if ( LVIS_SELECTED == (pNMListView->uNewState & LVIS_SELECTED) &&
         0 == (pNMListView->uOldState & LVIS_SELECTED))
    {
        // New item being selected, *may* need to deselect all of currently
        // selected items
        long callID = GetVItemData(vIndex);
        bool deselectCurrent = false;
        
        try
        {
            if (isCallHalfDuplex(callID))
            {
                // Selected a conference call - if currently have full duplex call selected
                // we must deselect it (otherwise OK to have multiple conference calls selected)
                for (std::set<unsigned long>::iterator itr = m_callGroupsSelected.begin();
                        itr != m_callGroupsSelected.end();
                        ++ itr)
                {
                    if (!isCallHalfDuplex(*itr))
                    {
                        // Full duplex selection active
                        deselectCurrent = true;
                    }
                }
            }
            else
            {
                // Selecting a full duplex call, necessarily means current selection must be cleared
                deselectCurrent = true;
            }
        }
        catch (...)
        {
            deselectCurrent = true;
        }

        if (deselectCurrent)
        {
            // To allow us to deselect all items, clear them
            // from the m_callGroupsSelected set
            m_callGroupsSelected.clear();
        }
    }
    
    bool selStateChanged = (pNMListView->uNewState & LVIS_SELECTED) !=
                                    (pNMListView->uOldState & LVIS_SELECTED);

    // Whenever selecting an item that may be a group node, ensure all members of group
    // are selected as well
    
    // (do not perform state change checks - because if we ctrl
    // click on a group already selected, the group will not be deselected
    // if we don't consider the non-state changed item that was clicked on)
   
    DWORD callID = GetVItemData(vIndex);
    if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        // Don't allow a mix of full duplex and half duplex calls (when user is shift-selecting
        // items, items already selected are inserted back at this point of code - *without*
        // a state change)
        bool isHalfDuplex = isCallHalfDuplex(callID);
        bool allowInsertion = true;

        for (std::set<unsigned long>::iterator itr = m_callGroupsSelected.begin();
                        itr != m_callGroupsSelected.end();
                        ++ itr)
        {
            if (isCallHalfDuplex(*itr) != isHalfDuplex)
            {
                // Exists an item of different type already in control, reject insert request
                allowInsertion = false;
                break;
            }
        }

        if (allowInsertion)
        {
            m_callGroupsSelected.insert(callID);
        }
    }
    else 
    {
        // Only respond to deselected state change
        if ((pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
        {
            onItemDeselected(pNMListView->iItem);
        }
    }       
 
    onListRefreshed();

    // Even through the MSDN documentation says return TRUE from an ON_NOTIFY_REFLECT_EX
    // to allow the parent to process the message, we actually have to return FALSE...
    return FALSE;
}


void CCurrentCall::onItemDeselected(int iItem)
{
    int vIndex = GetVirtualIndexFromActual(iItem);
    DWORD callID = GetVItemData(vIndex);

    DWORD state = GetItemState(iItem, LVIS_FOCUSED);
    
    // (this took a bit of trial and error, be very careful if changing)
    // Only do a deselect operation for a group when the item in focus has
    // its selection cleared (this effectively handles ctrl as opposed to
    // normal selection semantics)
    // OR the item being deselected is not part of the group that contains
    // the item with the current focus
    int focusItem = GetSelectionMark();
    
    if (focusItem >= 0)
    {
        int vFocusItem = GetVirtualIndexFromActual(focusItem);
        DWORD focusCallID = GetVItemData(vFocusItem);
                  
        if (0 != (state & LVIS_FOCUSED) ||
            focusCallID != callID)
        {            
            m_callGroupsSelected.erase(callID);
        }
    }
}


bool CCurrentCall::IsCallDisplayed(void)
{
	return m_callDisplayed;
}


bool CCurrentCall::IsOnHold(void)
{
	return false;
}

bool CCurrentCall::IsCallSelected()
{
 	FUNCTION_ENTRY("IsCallSelected");

	POSITION SelPos = GetFirstSelectedItemPosition();

	FUNCTION_EXIT;
	return (SelPos!=NULL);	
}

bool CCurrentCall::IsHalfDuplexCallSelected()
{
	// Check if any call is selected
	if (!IsCallSelected())
    {
        return false;
    }

	// Ok, is it a half duplex call?
	return isSelectedCallHalfDuplex();
}


CString CCurrentCall::GetID()
{
	return GetControlText(IDC_CA_ID);
}

CString CCurrentCall::GetTetraID()
{
	return GetControlText(IDC_CA_TETRAID);	
}


CString CCurrentCall::GetDestinationTetraID(int callID)
{
    try
    {
        TA_Base_Bus::IRadioCorbaDef::CallDetailsType* ret = NULL;
        CORBA_CALL_RETURN( ret, getRadioCorbaDef(), getCallDetails, ( callID,
						((CRadioManagerDialog*)(AfxGetApp()->m_pMainWnd))->getConsoleKey() ) );//limin
        std::auto_ptr<TA_Base_Bus::IRadioCorbaDef::CallDetailsType> callDetails( ret );

        if (0 != callDetails.get())
        {
            return CString(callDetails->calledTSI.in());
        }
        else
        {
            return "";
        }
    }
    SILENT_RM_CATCH_HANDLER("CCurrentCall::getCallDetails");

    return "";    
}


CString CCurrentCall::GetLocation()
{
	return GetControlText(IDC_CA_LOCATION);
}

CString CCurrentCall::GetControlText(UINT CtrlID)
{
	CDialog* pDlg = (CDialog*)AfxGetApp()->m_pMainWnd;
	ASSERT (pDlg!=NULL);

	CString controlText;

	pDlg->GetDlgItemText(CtrlID,controlText);

	return controlText;
}


void CCurrentCall::SetListHeadings()
{
	if (m_headingsDisplayed) return;

	ASSERT (AfxGetApp()->m_pMainWnd != NULL);

//	this->SubclassDlgItem(IDC_CURRENTCALLLIST,AfxGetApp()->m_pMainWnd);
    const DWORD TIME_COL_WIDTH = 150;
    const DWORD CALL_COL_WIDTH = 250;
    const DWORD TSI_COL_WIDTH = 120;
    const DWORD LOC_COL_WIDTH = 50;
    const DWORD STATUS_COL_WIDTH = 60;

	setColumnName(COL_TIME,     _T("Date/Time"));
	setColumnName(COL_CALL,     _T("Call"));
	setColumnName(COL_NUMBER,   _T("TSI"));
	setColumnName(COL_LOCATION, _T("Loc"));
	setColumnName(COL_STATUS ,  _T("Status"));

	SetColumnWidth(COL_TIME,    TIME_COL_WIDTH);
	SetColumnWidth(COL_CALL,    CALL_COL_WIDTH);
	SetColumnWidth(COL_NUMBER,  TSI_COL_WIDTH);
	SetColumnWidth(COL_LOCATION,LOC_COL_WIDTH);
	SetColumnWidth(COL_STATUS,  STATUS_COL_WIDTH);

	m_il.Create(IDB_CALLICONS, 16, 1, RGB(255, 0, 255));
	SetImageList(&m_il, LVSIL_SMALL);

    TA_IRS_App::RmLayoutHelper::getInstance().layoutListControl(*this);

	m_headingsDisplayed = true;
/*
	// Add some test data to box
	int iPosition;

	// Single call
	iPosition= AddItem("18/07 12:33:32",0,0);
	SetItemText(iPosition,1,"BLV SO");
	SetItemText(iPosition,2,"124-234-233");
	SetItemText(iPosition,3,"BLV");
	SetItemText(iPosition,4,"Connected");

	// Group call 1
	iPosition= AddItem("18/07 12:33:55",0,0);
	SetItemText(iPosition,1,"CONFERENCE");
	SetItemText(iPosition,2,"DBG Talking");

	iPosition= AddItem(" ",0,1);
	SetItemText(iPosition,1,"DBG SO");
	SetItemText(iPosition,2,"127-234-233");
	SetItemText(iPosition,3,"DBG");
	SetItemText(iPosition,4,"Talking");

	iPosition= AddItem(" ",0,1);
	SetItemText(iPosition,1,"MLN SO");
	SetItemText(iPosition,2,"157-234-233");
	SetItemText(iPosition,3,"MLN");
	SetItemText(iPosition,4,"Listening");

	// Group call 2
	iPosition= AddItem("18/07 12:34:05",0,0);
	SetItemText(iPosition,1,"CONFERENCE");
	SetItemText(iPosition,2,"Train 034/0011 Talking");

	iPosition= AddItem(" ",0,1);
	SetItemText(iPosition,1,"Train 034/0011");
	SetItemText(iPosition,2,"127-211-233");
	SetItemText(iPosition,3,"DBG");
	SetItemText(iPosition,4,"Talking");

	iPosition= AddItem(" ",0,1);
	SetItemText(iPosition,1,"Train 212/0033");
	SetItemText(iPosition,2,"157-211-233");
	SetItemText(iPosition,3,"MLN");
	SetItemText(iPosition,4,"Listening");

	iPosition= AddItem(" ",0,1);
	SetItemText(iPosition,1,"Train 123/0001");
	SetItemText(iPosition,2,"157-231-243");
	SetItemText(iPosition,3,"MLN");
	SetItemText(iPosition,4,"Listening");
*/
}


void CCurrentCall::PreSubclassWindow() 
{
	RadioCurrentCallListCtrl::PreSubclassWindow();

	// Setup headings
	SetListHeadings();
}


void CCurrentCall::onListRefreshed()
{   
    MSG msg;
    if (!PeekMessage(&msg, m_hWnd, WM_REFRESH_REQUEST, WM_REFRESH_REQUEST, PM_NOREMOVE))
    {
        // Only post if not already posted
        PostMessage(WM_REFRESH_REQUEST, 0, 0);
    }
}


LRESULT CCurrentCall::onRefreshRequest(WPARAM wParam, LPARAM lParam)
{
    refreshSelectionConstraints();

    return 0;
}


void CCurrentCall::refreshSelectionConstraints()
{
    if (GetVItemCount() > 0 && 0 == m_callGroupsSelected.size())
    {
        // If there are currently no calls selected, then select the first
        // call by default (to guarantee that there will always be a call selected)
        m_callGroupsSelected.insert(GetVItemData(0));        
    }

    // Update selection as required in response to a change in selection
    m_internalStateChange = true;
            
    // Ensure that for each selected call group, the entire group is selected
    for (int i = 0; i < (int) GetItemCount(); ++i )
    {
        int vIndex = GetVirtualIndexFromActual(i);
    
        if (m_callGroupsSelected.end() != m_callGroupsSelected.find(GetVItemData(vIndex)) )
        {
            if (LVIS_SELECTED != GetItemState(i, LVIS_SELECTED))
            {                
                // Select this row, represents one of the selected calls groups
                SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }            
        }    
        else
        {
            // Should be deselected
            if (0 != GetItemState(i, LVIS_SELECTED))
            {                
                // Deselect this row
                SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
            }
        }    
    }
    
    m_internalStateChange = false;
    
    if (m_prevCallGroupsSelected != m_callGroupsSelected)
    {
        // Only notify the parent window upon actual state change in selected call groups
        m_prevCallGroupsSelected = m_callGroupsSelected;
        ::PostMessage(AfxGetApp()->GetMainWnd()->m_hWnd, CURRENT_CALL_UPDATED_MSG, 0, 0);
    }
}


void CCurrentCall::Collapse(int nPos, int nItem)
{
    // When collapsing an item, the child nodes being collapsed are first
    // deselected (presumably something the tree list control does, as a normal
    // deleteItem will not cause an LVN_ITEMCHANGING notification), must suppress this
    // causing the call group to be deselected, because of LVN_ITEMCHANGING notification..
    m_internalStateChange = true;
    
    RadioCurrentCallListCtrl::Collapse(nPos, nItem);

    m_internalStateChange = false;
}


void CCurrentCall::removeCall(long callID)
{
    RadioCurrentCallListCtrl::removeCall(callID);

    MSG msg;
    if (!PeekMessage(&msg, m_hWnd, WM_REFRESH_REQUEST, WM_REFRESH_REQUEST, PM_NOREMOVE))
    {
        // Only post if not already posted
        PostMessage(WM_REFRESH_REQUEST, 0, 0);
    }
}
