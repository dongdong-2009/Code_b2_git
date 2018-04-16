#include "app\radio\radio_manager_gui_icdv4\src\RadioSelectedEntity\SelectedEntityDetails\CEntityDetailsAreaListBox.h"

CEntityDetailsAreaListBox::CEntityDetailsAreaListBox()
{
	readOnly = true;
}

CEntityDetailsAreaListBox::~CEntityDetailsAreaListBox()
{
}

BEGIN_MESSAGE_MAP(CEntityDetailsAreaListBox, CListBox)
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

void CEntityDetailsAreaListBox::setReadOnly(bool val)
{
	readOnly = val;
	RedrawWindow();
}

bool CEntityDetailsAreaListBox::isReadOnly()
{
	return readOnly;
}


void CEntityDetailsAreaListBox::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnLButtonDblClk(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnLButtonDown(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnLButtonUp(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnMButtonDblClk(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnMButtonDown(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnMButtonUp(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnRButtonDblClk(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnRButtonDown(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (!readOnly) {
		CListBox::OnRButtonUp(nFlags,point);
	}
}

void CEntityDetailsAreaListBox::OnSetFocus(CWnd* hWnd) 
{
	if (!readOnly) {
		CListBox::OnSetFocus(hWnd);
	}
}