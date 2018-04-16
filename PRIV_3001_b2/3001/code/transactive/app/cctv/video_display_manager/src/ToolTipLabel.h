#pragma once

#include "app/cctv/video_display_manager/src/Label.h"

class ToolTipLabel : public CLabel
{
public:
    ToolTipLabel():m_isToolTipActivate(false)
    {
    }
    virtual CLabel& SetText(const CString& strText);
protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual void PreSubclassWindow();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct );
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()
private:
    bool m_isToolTipActivate;
    CToolTipCtrl m_toolTip;
};