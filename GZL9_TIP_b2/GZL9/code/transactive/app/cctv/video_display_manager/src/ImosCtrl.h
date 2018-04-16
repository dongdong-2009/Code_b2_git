#pragma once
#include "imos_mediaocxctrl1.h"

class ImosCtrl : public CImos_mediaocxctrl1
{
protected:
	DECLARE_DYNCREATE(ImosCtrl)
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};