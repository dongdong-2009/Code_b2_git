#include "app\radio\radio_manager_gui\src\RadioCallstackPage\CallStackFilterList.h"



CallStackFilterList::CallStackFilterList()
: TA_Base_Bus::ListCtrlSelNoFocus()
{
}

CallStackFilterList::~CallStackFilterList()
{
}

BEGIN_MESSAGE_MAP(CallStackFilterList, CListCtrl)
	ON_WM_LBUTTONDBLCLK()
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRButtonUp)
	ON_COMMAND(ID_MAKE_CALL, OnMakeCall)
	ON_COMMAND(ID_REMOVE_CALL, OnRemoveCall)
END_MESSAGE_MAP()

void CallStackFilterList::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	FUNCTION_ENTRY("OnLButtonUp");
	int itemPosition = HitTest(point);

	if (-1 != itemPosition) {

		std::string typeStr = GetItemText(itemPosition, 1).GetString();
		RadioResourceType type = (typeStr == "Train") ? RR_TRAIN : RR_RADIO;
		if (RR_RADIO == type) {
			RadioResource res;
			res.alias = GetItemText(itemPosition, 0).GetString();
			res.id = atol(GetItemText(itemPosition, 2));
			res.ssi = res.id;
			res.type = RR_RADIO;

			CString status = GetItemText(itemPosition, 3);
			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[temp] %s", status);
			if ( !status.Compare("Missed Call") )
			{				
				The_RadioOperations::instance()->makePrivateCall(res);
			} else {
				The_RadioOperations::instance()->answerPrivateCall(res);
			}


		} else if (RR_TRAIN == type) {
			The_RadioOperations::instance()->makeDriverCall(GetItemData(itemPosition));
		}
	}

	FUNCTION_EXIT;
	CListCtrl::OnLButtonDblClk(nFlags, point);
}

void CallStackFilterList::OnRButtonUp(NMHDR* pNMHDR, LRESULT* pResult)
//UINT nFlags, CPoint point)
{
	LPNMITEMACTIVATE act = (LPNMITEMACTIVATE) pNMHDR;
	RadioResourceType type = RR_NIL;
	int itemPosition = 0;
	if ( -1 != act->iItem) {
		std::string typeStr = GetItemText(act->iItem, 1).GetString();
		type = (typeStr == "Train") ? RR_TRAIN : RR_RADIO;
		itemPosition = act->iItem;
	}
	if ( RR_RADIO == type ) {

		m_rightClickResource.initialise();
		m_rightClickResource.alias = GetItemText(itemPosition, 0).GetString();
		m_rightClickResource.id = atol(GetItemText(itemPosition, 2));
		m_rightClickResource.ssi = m_rightClickResource.id;
		m_rightClickResource.type = RR_RADIO;
		
		CRect rect;
		GetWindowRect(&rect);

		CMenu menu;
		menu.CreatePopupMenu();

		menu.AppendMenu(MF_ENABLED, ID_MAKE_CALL, "Make Call");
		menu.AppendMenu(MF_ENABLED,	ID_REMOVE_CALL, "Remove Call");

		CPoint mousePoint;
		::GetCursorPos(&mousePoint);

		menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, this, rect);

		menu.DestroyMenu();
	}
}

void CallStackFilterList::OnMakeCall()
{
	The_RadioOperations::instance()->removeResource(m_rightClickResource);
	The_RadioOperations::instance()->makePrivateCall(m_rightClickResource);
	The_RadioSystemStatus::instance()->removeResource(m_rightClickResource);
}

void CallStackFilterList::OnRemoveCall()
{
	The_RadioOperations::instance()->removeResource(m_rightClickResource);
	The_RadioSystemStatus::instance()->removeResource(m_rightClickResource);
}