
#include "app/radio/radio_manager_gui/src/RadioDirectory/RadioManagerFilterList.h"



RadioManagerFilterList::RadioManagerFilterList(int maxColumns)
: TA_Base_Bus::MultiFilterListCtrl(maxColumns)
{
}

RadioManagerFilterList::~RadioManagerFilterList()
{
}

BEGIN_MESSAGE_MAP(RadioManagerFilterList, CListCtrl)
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

void RadioManagerFilterList::OnLButtonUp(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonUp");
	int itemPosition = HitTest(point);

	if (-1 == itemPosition) {
		The_RadioOperations::instance()->entityEmptySelection(m_resType);
	}

	FUNCTION_EXIT;
	CListCtrl::OnLButtonUp(nFlags, point);
}

void RadioManagerFilterList::useCallback(int columnNo)
{
	m_sortInfo.setCallback(this);
}

void RadioManagerFilterList::setRadioResourceType(RadioResourceType type)
{
	FUNCTION_ENTRY("setRadioResourceType");
	m_resType = type;
	FUNCTION_EXIT;
}

RadioResourceType RadioManagerFilterList::getRadioResourceType()
{
	FUNCTION_ENTRY("getRadioResourceType");
	FUNCTION_EXIT;
	return m_resType;
}

void RadioManagerFilterList::OnPaint() 
{
	CPaintDC* dc = NULL;
	try {
		dc = new CPaintDC(this);
	}
	catch (...) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occurs");
		return;
	}
	CBrush b;
	b.CreateSolidBrush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = NULL;
	pOldBrush = dc->SelectObject(&b);
	CRect r(0,0,0,0);
	GetClientRect(&r);
	dc->FillRect(&r, &b);
	
	if (NULL != dc){
		delete dc;
		dc = NULL;
	}
}

int RadioManagerFilterList::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort)
{
	FUNCTION_ENTRY("sortCallback");

	if (lParam1 == 0 && lParam2 == 0)
		return 0;

	CString desc1;
	CString desc2;

	desc1 = GetItemText(lParam1, columnIndex);
	desc2 = GetItemText(lParam2, columnIndex);

	int result;		
	if (desc1.IsEmpty()) {
			result = 1;
	} else  
	if (desc2.IsEmpty()) {
		result = -1;
	} else {
		const char* s1 = (LPCTSTR) desc1;
		const char* s2 = (LPCTSTR) desc2;
		result = stricmp(s1,s2);
		if (0 == result) {
			result = strcmp(s1,s2);
		}
	}

	if (currentSort == TA_Base_Bus::AutoSortListCtrl::DESCENDING) {
		result *= -1;
	}

	FUNCTION_EXIT;
	return result;
}