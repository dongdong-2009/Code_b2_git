#include "stdafx.h"
#include "app/cctv/video_display_manager/src/ToolTipLabel.h"

BOOL ToolTipLabel::PreTranslateMessage(MSG* pMsg)
{
  if(NULL != m_toolTip.GetSafeHwnd())
  {
    m_toolTip.RelayEvent(pMsg);
  }
  return CLabel::PreTranslateMessage(pMsg);
}

void ToolTipLabel::PreSubclassWindow()
{
  CLabel::PreSubclassWindow();
  ModifyStyle(0, SS_NOTIFY);
  EnableToolTips(TRUE);
  if(::IsWindow(GetSafeHwnd()))
  {
      m_toolTip.Create(this);
      m_toolTip.Activate(TRUE);
      m_toolTip.AddTool(this);
      m_isToolTipActivate = true;
  }
}

BEGIN_MESSAGE_MAP(ToolTipLabel, CLabel)
  ON_WM_CREATE()
  ON_WM_LBUTTONDOWN()
  ON_WM_RBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()

CLabel& ToolTipLabel::SetText(const CString& strText)
{
  m_toolTip.UpdateTipText(strText, this);
  return CLabel::SetText(strText);
}

int ToolTipLabel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if(!m_isToolTipActivate)
  {
      m_toolTip.Create(this);
      m_toolTip.Activate(TRUE);
      m_toolTip.AddTool(this);
      m_isToolTipActivate = true;
  }
  return CLabel::OnCreate(lpCreateStruct);
}

// Mouse message should be process by parent
void ToolTipLabel::OnLButtonDown(UINT nFlags, CPoint point)
{
  ClientToScreen(&point);
  GetParent()->ScreenToClient(&point);
  GetParent()->PostMessage(WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
}

void ToolTipLabel::OnLButtonUp(UINT nFlags, CPoint point)
{
}

void ToolTipLabel::OnLButtonDblClk(UINT nFlags, CPoint point)
{
}

void ToolTipLabel::OnRButtonDown(UINT nFlags, CPoint point)
{
  ClientToScreen(&point);
  GetParent()->ScreenToClient(&point);
  GetParent()->PostMessage(WM_RBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));
}
