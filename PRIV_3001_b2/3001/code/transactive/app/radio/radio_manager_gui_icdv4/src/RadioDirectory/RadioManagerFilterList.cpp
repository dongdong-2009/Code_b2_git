
#include "app/radio/radio_manager_gui_icdv4/src/RadioDirectory/RadioManagerFilterList.h"



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