#include "StdAfx.h"
#include "ImosCtrl.h"
#include "CCTVCommon.h"
#include "core/utilities/src/DebugUtil.h"

IMPLEMENT_DYNCREATE(ImosCtrl, CImos_mediaocxctrl1)

BEGIN_MESSAGE_MAP(ImosCtrl, CImos_mediaocxctrl1)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void ImosCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	TA_ASSERT(GetParent() != NULL, "");
	GetParent()->PostMessage(FULLSCREEN_REQUEST, 0, 0);
}