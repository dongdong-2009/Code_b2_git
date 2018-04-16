// SelectListCtrl.cpp : implementation file
//
#include "app\radio\radio_manager_gui\src\RadioSelectedEntity\SelectedEntityDetails\SelectListCtrl.h"
#include "core\utilities\src\DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif 
/////////////////////////////////////////////////////////////////////////////
// CSelectListCtrl

#define BUFFERSTRING	"                                                          "
#define NOTFOUND		-1

CSelectListCtrl::CSelectListCtrl()
{
	FUNCTION_ENTRY("CSelectListCtrl");
	readOnly = true;
	m_bColInitialized = false;
	FUNCTION_EXIT;
}

CSelectListCtrl::~CSelectListCtrl()
{
	FUNCTION_ENTRY("~CSelectListCtrl");
	FUNCTION_EXIT;
}

BEGIN_MESSAGE_MAP(CSelectListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSelectListCtrl)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectListCtrl message handlers
 


void CSelectListCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{

	FUNCTION_ENTRY("OnCustomDraw"); 

	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// By default set the return value to do the default behavior.
	*pResult = CDRF_DODEFAULT;     

	//obtain row and column of item
	int iRow = pCD->nmcd.dwItemSpec;
	int iCol = pCD->iSubItem;


	//Remove standard highlighting of selected (sub)item.
	pCD->nmcd.uItemState = CDIS_DEFAULT;
			
	switch( pCD->nmcd.dwDrawStage )
	{
		case  CDDS_PREPAINT:  // First stage (for the whole control)
		{

			*pResult = CDRF_NOTIFYITEMDRAW;

		}
		break;

		case CDDS_ITEMPREPAINT:
		{

			if (!isReadOnly())
			{
				if( (m_iRow == iRow /*&& m_iCol == iCol*/ ) )
				{
					pCD->clrText	= RGB(0,0,0);          //selected text color
					pCD->clrTextBk	= RGB(255,255,0);      //selected basckground color
				}
			}

			
			*pResult = CDRF_NOTIFYSUBITEMDRAW;

		}
		break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM : // Stage three 
		{
			*pResult =  CDRF_NOTIFYPOSTPAINT;
		}
		break;
		
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{

			*pResult = CDRF_NOTIFYSUBITEMDRAW;
		}

		default:// it wasn't a notification that was interesting to us.
		{

       	  *pResult = CDRF_DODEFAULT;

		}
		break;
	}
	
	FUNCTION_EXIT;
}


void CSelectListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{

    FUNCTION_ENTRY ("OnKeyDown");

	CHeaderCtrl* pHeaderCtrl = GetHeaderCtrl();
    int nColumnCount;
    nColumnCount = pHeaderCtrl->GetItemCount();

    if(nChar == VK_LEFT && m_iCol > 0)
	  m_iCol -= 1;

	if(nChar == VK_UP && m_iRow > 0)
	  m_iRow -= 1;

	if(nChar == VK_RIGHT && m_iCol < (nColumnCount-1) ) 
 	  m_iCol += 1;

	if(nChar == VK_DOWN &&  m_iRow < (GetItemCount()-1) )
	  m_iRow += 1;

	Invalidate();
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);

	FUNCTION_EXIT;
}



void CSelectListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	FUNCTION_ENTRY ("OnNMClick");
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    int iRow = pNMListView->iItem;
    int iCol = pNMListView->iSubItem;
	if (iRow != -1 && iCol != -1)
	{
	   //set current item
		m_iRow = iRow;
		m_iCol = iCol;
		
		
		CString strItemText = GetItemText(m_iRow,0);
		
		if (NOTFOUND == strItemText.Find(BUFFERSTRING))
		{
			char * pszData = new char [1023];
			memset(pszData, 0, 1023);
			sprintf(pszData,"%s%s",strItemText.GetBuffer(0),BUFFERSTRING);
			int nReturnIndex = this->SetItemText(m_iRow,0,pszData);

			delete [] pszData;
		}

		Invalidate();
	}
	
	*pResult = 0;
	FUNCTION_EXIT;
}

void CSelectListCtrl::setReadOnly(bool val)
{
	FUNCTION_ENTRY("setReadOnly");
	
	readOnly = val;
	RedrawWindow();

	m_iRow = -1;
	m_iCol = -1;
	
	FUNCTION_EXIT;
}

bool CSelectListCtrl::isReadOnly()
{
	FUNCTION_ENTRY("isReadOnly");
	FUNCTION_EXIT;
	
	return readOnly;
}	


void CSelectListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonDblClk");
	
	if (!readOnly) 
	{
		CListCtrl::OnLButtonDblClk(nFlags,point);
	}
	
	FUNCTION_EXIT;
}

void CSelectListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonDown");
	
	if (!readOnly)
	{
		CListCtrl::OnLButtonDown(nFlags,point);
	}
}

void CSelectListCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonUp");
	
	if (!readOnly) 
	{
		CListCtrl::OnLButtonUp(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnMButtonDblClk");
	
	if (!readOnly) 
	{
		CListCtrl::OnMButtonDblClk(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnMButtonDown");
	
	if (!readOnly) 
	{
		CListCtrl::OnMButtonDown(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	
	FUNCTION_ENTRY("OnMButtonUp");
	
	if (!readOnly) 
	{
		CListCtrl::OnMButtonUp(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnRButtonDblClk");

	if (!readOnly) 
	{
		CListCtrl::OnRButtonDblClk(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnRButtonDown");

	if (!readOnly) 
	{
		CListCtrl::OnRButtonDown(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnRButtonUp");
	
	if (!readOnly) 
	{
		CListCtrl::OnRButtonUp(nFlags,point);
	}

	FUNCTION_EXIT;
}

void CSelectListCtrl::OnSetFocus(CWnd* hWnd) 
{
	
	FUNCTION_ENTRY("OnSetFocus");

	if (!readOnly) 
	{
		CListCtrl::OnSetFocus(hWnd);
	}

	FUNCTION_EXIT;
}

int CSelectListCtrl::GetCount ()
{
	
	FUNCTION_ENTRY("GetCount");

	int nCount = 0;
	
	nCount = this->GetItemCount();
	
	FUNCTION_EXIT;

	return nCount;
}

void CSelectListCtrl::ResetContent ()
{
	FUNCTION_ENTRY("ResetContent");
	this->DeleteAllItems();
	FUNCTION_EXIT;
}

int CSelectListCtrl::GetCurSel ()
{
	// Value set by NMClick Event	
	FUNCTION_ENTRY("GetCurSel");
	FUNCTION_EXIT;
	return m_iRow;
	
}
int CSelectListCtrl::InsertString (int nIndex,LPCTSTR lpszItem)
{
	FUNCTION_ENTRY("InsertString");
	
	CString strItemText = lpszItem;
	int nReturnIndex = -1;

	if (NOTFOUND == strItemText.Find(BUFFERSTRING))
	{
		char * pszData = new char [1023];
		memset(pszData, 0, 1023);
		sprintf(pszData,"%s%s",strItemText.GetBuffer(0),BUFFERSTRING);
		
		nReturnIndex = this->InsertItem(nIndex,pszData);

		delete [] pszData;
	}
 
	FUNCTION_EXIT;
	return nReturnIndex;
}

void CSelectListCtrl::DeleteString (int nIndex) 
{
	FUNCTION_ENTRY ("DeleteString");
	
	bool bResult = this->DeleteItem(nIndex);

	if (bResult)
	{
		if (nIndex == GetItemCount())
		{
			m_iRow = -1;
		}
	}
	
	FUNCTION_EXIT;
}

int CSelectListCtrl::FindStringExact (int nIndexStart,LPCTSTR lpszFind,bool bSubStringSearch)
{
	FUNCTION_ENTRY ("FindStringExact");
	int nRetIndex = 0;

	LVFINDINFO findInfo;
	
	if (bSubStringSearch)
		findInfo.flags = LVFI_PARTIAL;
	else
		findInfo.flags = LVFI_STRING;
	
	// add buffer data that was add for higglighting purposes

	findInfo.psz = lpszFind;

	nRetIndex = this->FindItem(&findInfo);

	FUNCTION_EXIT;
	return nRetIndex;
}

void CSelectListCtrl::SetDefColWidth  ()
{
	FUNCTION_ENTRY ("SetDefColWidth");
	this->SetColumnWidth(0,2000);
	FUNCTION_EXIT;
}


void CSelectListCtrl::InsertDefColoumn ()
{
	FUNCTION_ENTRY ("SetDefColWidth");
	this->InsertColumn (0,"",LVCFMT_CENTER,2000);
	FUNCTION_EXIT;
}

int CSelectListCtrl::AddString (LPCTSTR lpszItem)
{
	FUNCTION_ENTRY ("AddString");
	
	int nReturn = -1;

	nReturn = this->InsertString(0,lpszItem);

	FUNCTION_EXIT;

	return nReturn;
}
