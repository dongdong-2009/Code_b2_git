
#pragma once
#include "afxwin.h"
#include "bus\radio\radio_bus\src\radiotypes.h"


class CRadioSubsResourceText : public CEdit
{
public:
CRadioSubsResourceText();
~CRadioSubsResourceText();
	
afx_msg void OnRButtonUp(UINT nFlags, CPoint point);	
afx_msg void OnSetFocus(CWnd* pOldWnd);

protected:
	DECLARE_MESSAGE_MAP()
};