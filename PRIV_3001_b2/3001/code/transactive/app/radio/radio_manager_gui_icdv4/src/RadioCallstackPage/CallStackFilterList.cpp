#include "app\radio\radio_manager_gui_icdv4\src\RadioCallstackPage\CallStackFilterList.h"



CallStackFilterList::CallStackFilterList()
: TA_Base_Bus::ListCtrlSelNoFocus()
{
}

CallStackFilterList::~CallStackFilterList()
{
}

BEGIN_MESSAGE_MAP(CallStackFilterList, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

void CallStackFilterList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonUp");
	int itemPosition = HitTest(point);

	if (-1 != itemPosition) {
		//LV_FINDINFO findinfo;
		//findinfo.flags = LVFI_PARAM;
		//findinfo.lParam = resource.id;

		RadioResource res;
		res.id = atol(GetItemText(itemPosition, 2));
		res.ssi = res.id;
		res.type = RR_RADIO;
		The_RadioOperations::instance()->answerPrivateCall(res);
	}

	FUNCTION_EXIT;
	CListCtrl::OnLButtonDblClk(nFlags, point);
}