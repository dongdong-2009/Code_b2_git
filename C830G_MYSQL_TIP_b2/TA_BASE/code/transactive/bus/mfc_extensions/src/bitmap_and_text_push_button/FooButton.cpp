/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/mfc_extensions/src/bitmap_and_text_push_button/FooButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
//  FooButton.h
//  A versatile bitmap button
//  Copyright (c) 1997-2004 Ravi Bhavnani (ravib@ravib.com)
//
//  You are allowed to include the source code in any product (commercial, shareware,
//  freeware or otherwise) when your product is released in binary form.  You are allowed
//  to modify the source code in any way you want except you cannot modify the copyright
//  details at the top of each module.  If you want to distribute source code with your
//  application, then you are only allowed to distribute versions released by the author.
//  This is to maintain a single distribution point for the source code. 
//
//  sant: Added read only functionality.

#include "bus\mfc_extensions\src\stdafx.h"
#include "bus\mfc_extensions\src\bitmap_and_text_push_button\FooButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define C_BitmapBorder_X      5     // horizontal border around bitmap for text buttons
#define C_DropDown_Width      5     // width of dropdown indicator
#define C_DropDown_Height     3     // height of dropdown indicator
#define C_DropDown_X          10    // border between left edge of drop down indicator and right edge of button
#define C_DropDown_Y          8     // border between top of drop down indicator and bottom of button
#define C_BitmapCheckGap      5     // gap between right edge of bitmap and left edge of checkbox

#define C_HyperlinkColor      RGB (0, 0, 224)   // hyperlink color

CMapStringToPtr  FooButton::m_btnGroups;  // button groups

/////////////////////////////////////////////////////////////////////////////
// FooButton

//! Standard constructor.
FooButton::FooButton()
{
  // Set default properties
  setType (FooButton::Type::pushButton);
  //setTextStyle (FooButton::Text::singleLineCenter);
  setTextStyle (FooButton::Text::singleLine);
  setFocusStyle (FooButton::Focus::noFocus);
  setBitmapId (0);

  // Initialize state
  m_bChecked = false;
  m_bTracking = false;
  m_bMultiClicked = false;
  m_bDefaultButton = false;
  m_readOnly = false;

  // Load hyperlink cursor
  m_hCursor = NULL;
  loadHyperlinkCursor();
}

//! Standard destructor.
FooButton::~FooButton()
{
  // Free cursor
  if (NULL != m_hCursor)
     VERIFY (DestroyCursor (m_hCursor));
}

BEGIN_MESSAGE_MAP(FooButton, CButton)
	//{{AFX_MSG_MAP(FooButton)
  ON_WM_ERASEBKGND()
  ON_WM_KILLFOCUS()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_MOUSEMOVE()
  ON_WM_SETCURSOR()
  ON_WM_SETFOCUS()
  ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FooButton operations

//! Resets static storage used by the class.  This method should be called
//! when the application exits.
void FooButton::reset()
{
  POSITION pos = m_btnGroups.GetStartPosition();
  while (pos != NULL) {
    CString strGroupName;
    void* pGroup = NULL;
    m_btnGroups.GetNextAssoc (pos, strGroupName, pGroup);
    ASSERT (pGroup != NULL);
    delete (CPtrArray *) pGroup;
  }
}

//! Sets the button's type.
//! @param  type    The button's type.
void FooButton::setType
  (FooButton::Type type)
{
  // Initialize button properties
  m_bDropDown = false;
  m_bMulti = false;
  m_bStatic = false;
  m_bHot = false;
  m_bHyperlink = false;

  // Set button type
  switch (type) {
    case FooButton::Type::staticButton:
      m_bStatic = true;
      break;

    case FooButton::Type::checkBox:
    case FooButton::Type::radio:
    case FooButton::Type::checkButton:
    case FooButton::Type::pushButton:
      break;

    case FooButton::Type::pushButtonDropDown:
      m_bDropDown = true;
      break;

    case FooButton::Type::hotCheckButton:
    case FooButton::Type::hotPushButton:
      m_bHot = true;
      break;

    case FooButton::Type::hotPushButtonDropDown:
      m_bHot = true;
      m_bDropDown = true;
      break;

    case FooButton::Type::pushButtonMulti:
      m_bMulti = true;
      break;

    case FooButton::Type::hotPushButtonMulti:
      m_bHot = true;
      m_bMulti = true;
      break;

    case FooButton::Type::hyperlink:
      m_bHyperlink = true;
      break;

    default:
      ASSERT (FALSE);
      return;
  }
  m_type = type;

  // Reset checked mode
  if ((m_type != FooButton::Type::checkButton) &&
      (m_type != FooButton::Type::hotCheckButton))
     m_bChecked = false;

  if (GetSafeHwnd() != NULL)
     Invalidate();
}

//! Sets the button's text style.
//! @param  textStyle   The button's text style.
void FooButton::setTextStyle
  (FooButton::Text textStyle)
{
  // Reset text styles
  m_bText = false;
  m_bCenter = false;
  m_bMultiLine = false;

  // Set text style
  switch (textStyle) {
    case FooButton::Text::none:
      break;

    case FooButton::Text::singleLine:
      m_bText = true;
      break;

    case FooButton::Text::singleLineCenter:
      m_bText = true;
      m_bCenter = true;
      break;

    case FooButton::Text::multiLine:
      m_bText = true;
      m_bMultiLine = true;
      break;

    default:
      ASSERT (FALSE);
      return;
  }
  m_textStyle = textStyle;
  if (GetSafeHwnd() != NULL)
     Invalidate();
}

//! Sets the button's focus style.
//! @param  focusStyle   The button's focus style.
void FooButton::setFocusStyle
  (FooButton::Focus focusStyle)
{
  m_focusStyle = focusStyle;
  if (GetSafeHwnd() != NULL)
     Invalidate();
}

//! Sets the button's bitmap id.
//! @param  nBitmapId       Bitmap id (-1 means none).
//! @param  rgbTransparent  Bitmap's transparency color (default = RGB(255,0,255)
void FooButton::setBitmapId
  (int nBitmapId,
   COLORREF rgbTransparent)
{
  m_nBitmapId = nBitmapId;
  m_rgbTransparent = rgbTransparent;
  if (GetSafeHwnd() != NULL)
     Invalidate();
}

//! Sets the button's checked state if the button type is checkButton or
//! hotCheckButton.
//! @param    bChecked    Flag: button is checked.
//! @return   The button's checked state.
void FooButton::check
  (bool bChecked)
{
  if ((m_type == FooButton::Type::checkButton) ||
      (m_type == FooButton::Type::hotCheckButton) ||
      (m_type == FooButton::Type::checkBox) ||
      (m_type == FooButton::Type::radio)) {
      m_bChecked = bChecked;
      if (GetSafeHwnd() != NULL)
         Invalidate();
  }

  // If checked, inform the button's parent group (if any)
  if (isChecked() && !m_strButtonGroup.IsEmpty()) {
      void* pGroup;
      getButtonGroup (m_strButtonGroup, pGroup);
      ASSERT (pGroup != NULL);
      int nButtons = ((CPtrArray *) pGroup)->GetSize();
      for (int nIndex=0; (nIndex < nButtons); nIndex++) {
          FooButton* pButton = (FooButton *) ((CPtrArray *) pGroup)->GetAt (nIndex);
          ASSERT (pButton != NULL);
          if ((pButton != this) && pButton->isChecked())
             pButton->check (false);
      }
  }
}

//! Sets a multi pushbutton to its normal unpressed state.
void FooButton::clearMultiClick()
{
  if ((m_type == FooButton::Type::pushButtonMulti) ||
      (m_type == FooButton::Type::hotPushButtonMulti)) {
      m_bMultiClicked = false;
      Invalidate();
  }
}

//! Displays a popup menu if the button type is pushButton, pushButtonDropDown,
//! hotPushButton or hotPushButtonDropDown.
//! @param    pMenu   The popup menu to be displayed.
//! @return   The menu selection.
int FooButton::displayPopupMenu
  (CMenu* pMenu)
{
  Invalidate();
  if (pMenu != NULL) {
      CRect  rectButton;
      GetWindowRect (&rectButton);
      return (pMenu->TrackPopupMenu
                (TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD,
                rectButton.left, rectButton.bottom, GetParent()));
  }
  return (0);
}

//! Enables/disables the button.
//! @param    bEnable   Flag: button is enabled.
void FooButton::enable
  (bool bEnable)
{
  EnableWindow (bEnable ? TRUE : FALSE);
  Invalidate();
}

//! Adds the button to a named button group.
//! @param    strGroupName    Name of button group.
//! @return   true if successful, false otherwise.
bool FooButton::addToGroup
  (CString strGroupName)
{
  // Group name must be specified
  if (strGroupName.IsEmpty())
     return (false);

  // Create the group if it doesn't exist
  void* pGroup = NULL;
  getButtonGroup (strGroupName, pGroup);
  if (pGroup == NULL) {
      pGroup = new CPtrArray();
      ASSERT (pGroup != NULL);
      m_btnGroups.SetAt (strGroupName, pGroup);
  }

  // Remove the button from its current group
  removeFromGroup();

  // Add button to group if not already present
  m_strButtonGroup = strGroupName;
  int nButtons = ((CPtrArray *) pGroup)->GetSize();
  for (int nIndex=0; (nIndex < nButtons); nIndex++)
      if (((CPtrArray *) pGroup)->GetAt (nIndex) == this)
         return (true);
  ((CPtrArray *) pGroup)->Add (this);

  return (true);
}

//! Removes the button from its parent group.
//! @param    pGroup    The FooButtonGroup.
//! @return   true if successful, false otherwise.
bool FooButton::removeFromGroup()
{
  // Return if not a member of a button group
  if (m_strButtonGroup.IsEmpty())
     return (false);

  // Return if group doesn't exist
  void* pGroup = NULL;
  getButtonGroup (m_strButtonGroup, pGroup);
  if (pGroup == NULL) {
      ASSERT (FALSE);
      return (false);
  }

  // Remove the button from the group
  int nButtons = ((CPtrArray *) pGroup)->GetSize();
  for (int nIndex=0; (nIndex < nButtons); nIndex++) {
      if (((CPtrArray *) pGroup)->GetAt (nIndex) == this) {
          ((CPtrArray *) pGroup)->RemoveAt (nIndex);
          m_strButtonGroup.Empty();
          return (true);
      }
  }

  // Button wasn't part of the group
  ASSERT (FALSE);
  return (false);
}

/////////////////////////////////////////////////////////////////////////////
// FooButton message handlers

void FooButton::PreSubclassWindow() 
{
  // Check if it's a default button
	if (GetStyle() & 0x0FL)
     m_bDefaultButton = true;

  // Make the button owner-drawn
	ModifyStyle (0x0FL, BS_OWNERDRAW, SWP_FRAMECHANGED);
	CButton::PreSubclassWindow();
}

void FooButton::OnKillFocus(CWnd* pNewWnd)
{
  Invalidate();
  CButton::OnKillFocus (pNewWnd);
}

void FooButton::OnLButtonDown (UINT nFlags, CPoint point)
{
  multiHitTest (point);   // sets m_bMultiClicked
  CButton::OnLButtonDown (nFlags, point);
}

void FooButton::OnLButtonUp (UINT nFlags, CPoint point)
{
  // If this is a checkbutton...
  if ((m_type == FooButton::Type::checkButton) ||
      (m_type == FooButton::Type::hotCheckButton) ||
      (m_type == FooButton::Type::checkBox) ||
      (m_type == FooButton::Type::radio)) {

      // Check ourself if mouse was released over button
      CRect rectButton;
      GetClientRect (&rectButton);
      if (rectButton.PtInRect (point))
         if ((m_type != FooButton::Type::radio) && m_strButtonGroup.IsEmpty())
            check (!isChecked());
         else
            check (true);
  }

  // Standard handling
  CButton::OnLButtonUp (nFlags, point);
  m_bMultiClicked = false;
  Invalidate();
}

void FooButton::OnMouseMove(UINT nFlags, CPoint point) 
{
  if (m_bHot && !m_bTracking) {
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = HOVER_DEFAULT;
		m_bTracking = _TrackMouseEvent(&tme);
	  Invalidate();
  }

  // Do default action
	CButton::OnMouseMove(nFlags, point);
}

LRESULT FooButton::OnMouseLeave
  (WPARAM wParam,
   LPARAM lParam)
{
  // To ignore "unreferenced formal parameter" warning
  wParam = wParam;
  lParam = lParam;

	m_bTracking = false;
	m_bMultiClicked = false;
	Invalidate();
	return 0;
}

BOOL FooButton::OnEraseBkgnd(CDC* pDC) 
{
RECT    rect;   // button rectangle

  // Draw gray background
  GetClientRect (&rect);
  pDC->FillSolidRect (&rect, ::GetSysColor (COLOR_BTNFACE));
	return CButton::OnEraseBkgnd(pDC);
}

BOOL FooButton::OnSetCursor (CWnd* /* pWnd */, UINT /* nHitTest */, UINT /* message */)
{
  if (m_bHyperlink && (NULL != m_hCursor)) {
      ::SetCursor (m_hCursor);
      return TRUE;
  }
  return FALSE;
}

void FooButton::OnSetFocus(CWnd* pOldWnd)
{
  Invalidate();
  CButton::OnSetFocus (pOldWnd);
}

void FooButton::DrawItem (LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
  ASSERT (pDC != NULL);

  // Draw frame for non-radio and non-checkbox buttons
  if ((m_type != FooButton::Type::checkBox) && (m_type != FooButton::Type::radio))
     drawFrame (pDC, lpDrawItemStruct);

  // Draw button bitmap
  int nBmpRightEdge = 0;
  drawBitmap (pDC, lpDrawItemStruct, nBmpRightEdge);

  // Draw frame for radio and checkbox buttons
  if ((m_type == FooButton::Type::checkBox) || (m_type == FooButton::Type::radio))
     drawRadioCheckFrame (pDC, lpDrawItemStruct, nBmpRightEdge);

  // Draw drop-down indicator
  drawDropDown (pDC, lpDrawItemStruct);

  // Draw caption
  int nCaptionRightEdge = 0;
  drawCaption (pDC, lpDrawItemStruct, nBmpRightEdge, nCaptionRightEdge);

  // Draw focus rectangle
  drawFocus (pDC, lpDrawItemStruct, nBmpRightEdge, nCaptionRightEdge);

  // Draw default button border
  drawDefaultBorder (pDC, lpDrawItemStruct);

  return;
}

/////////////////////////////////////////////////////////////////////////////
// FooButton helper functions

//! Draws the button's frame
void FooButton::drawFrame
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Draw button outline if we're not a static button or a hyperlink
  if (!m_bStatic && !m_bHyperlink) {
      if (m_bHot && !m_bChecked)
          drawHotButtonFrame (lpDrawItemStruct);
      else {
          // Draw frame for non-hot button
          UINT uFrameCtrl = DFCS_BUTTONPUSH;
          if (m_bChecked)
             uFrameCtrl |= DFCS_CHECKED;
          else
             if (((lpDrawItemStruct->itemState & ODS_SELECTED) == ODS_SELECTED) && !m_bMulti)
                uFrameCtrl |= DFCS_PUSHED;
          if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
             uFrameCtrl |= DFCS_INACTIVE;
          pDC->DrawFrameControl (&(lpDrawItemStruct->rcItem), DFC_BUTTON , uFrameCtrl);

          // For multi pushbuttons, fill the area of the drop-down indicator with
          // a light color if the multi button is currently pressed
          if (m_bMulti && m_bMultiClicked)
             drawMultiDropDownRegion (pDC, lpDrawItemStruct);
      }
  }
}

//! Draws the button's frame for a radio/checkbox button
void FooButton::drawRadioCheckFrame
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct,
   int& nLeftEdge)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Nothing to do if not a radiobutton or checkbox
  if ((m_type != FooButton::Type::checkBox) && (m_type != FooButton::Type::radio))
     return;

  // Compute frame dimensions
  CRect rectFrame = lpDrawItemStruct->rcItem;
  rectFrame.left = nLeftEdge;
  if (nLeftEdge > 0)
     rectFrame.left += C_BitmapCheckGap;

  int nFrameWidthHeight = GetSystemMetrics(SM_CXCURSOR) - 19;
  rectFrame.right = rectFrame.left + nFrameWidthHeight;
  int nOffset = (rectFrame.Height() - nFrameWidthHeight) / 2;
  rectFrame.top += nOffset;
  rectFrame.bottom = rectFrame.top + nFrameWidthHeight;
  nLeftEdge = rectFrame.right;

  // Draw frame
  UINT uFrameCtrl = (m_type == FooButton::Type::checkBox ? DFCS_BUTTONCHECK : DFCS_BUTTONRADIO);
  if (m_bChecked)
     uFrameCtrl |= DFCS_CHECKED;
  if ((lpDrawItemStruct->itemState & ODS_SELECTED) == ODS_SELECTED)
     uFrameCtrl |= DFCS_PUSHED;
  if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
     uFrameCtrl |= DFCS_INACTIVE;
  pDC->DrawFrameControl (rectFrame, DFC_BUTTON , uFrameCtrl);
}

//! Draws the button's bitmap
void FooButton::drawBitmap
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct,
   int& nBitmapRightEdge)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Load bitmap - return if unable
  nBitmapRightEdge = 0;
  if (m_nBitmapId == 0)
     return;
  HBITMAP hBitmap = ::LoadBitmap (AfxGetResourceHandle(), MAKEINTRESOURCE (m_nBitmapId));
  ASSERT (hBitmap != NULL);
  if (hBitmap == NULL)
     return;

  // Get bitmap dimensions
  BITMAP  rBitmap;
  ::GetObject (hBitmap, sizeof (BITMAP), &rBitmap);
  CSize bitmapSize;
  bitmapSize.cx = rBitmap.bmWidth;
  bitmapSize.cy = rBitmap.bmHeight;

  // Position the bitmap - if the button contains no text or is a checkbox/radio,
  // leave C_BitmapBorder_X pixels from the L edge.  Otherwise, center it horizontally.
  CRect rectButton;
  GetClientRect (&rectButton);
  CPoint pt;
  if (m_bText || (m_type == FooButton::Type::checkBox) || (m_type == FooButton::Type::radio))
     pt.x = rectButton.left + C_BitmapBorder_X;
  else
     pt.x = (rectButton.right - rBitmap.bmWidth) / 2;
  pt.y = (rectButton.bottom - rBitmap.bmHeight) / 2;
  if ((m_type != FooButton::Type::checkBox) && (m_type != FooButton::Type::radio))
     if (!m_bStatic && !m_bHyperlink && !m_bMultiClicked)
        if (lpDrawItemStruct->itemState & ODS_SELECTED)
           pt.Offset (1, 1);

  // Draw the bitmap
  if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
      DisabledBlt (pDC->m_hDC,
                   pt.x, pt.y,
                   rBitmap.bmWidth, rBitmap.bmHeight,
                   hBitmap,
                   0, 0);
  else
      TransparentBlt (pDC->m_hDC,
                      pt.x, pt.y,
                      rBitmap.bmWidth, rBitmap.bmHeight,
                      hBitmap,
                      0, 0,
                      m_rgbTransparent,
                      NULL);

  // Return right edge of bitmap
  nBitmapRightEdge = pt.x + rBitmap.bmWidth;

  // Free bitmap
  ::DeleteObject (hBitmap);
}

//! Draws the button's caption
void FooButton::drawCaption
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct,
   int nLeftEdge,
   int& nRightEdge)
{
  // Nothing to do if the isn't set up to display text
  nRightEdge = 0;
  if (!m_bText)
     return;

  // Tweak edges
  if (!m_bCenter)
     nLeftEdge += C_BitmapBorder_X; // looks nicer
  CRect rectButton;
  GetClientRect (&rectButton);
  nRightEdge = rectButton.right;
  if (m_bDropDown)
     nRightEdge -= 6;
  else
     if (m_bMulti) {
        // bool bBitmap = (m_nBitmapId != 0);
        // nRightEdge -= (bBitmap ? 12 : 10);
        nRightEdge -= 12;
     }

  // Get available caption width - return if none
  int nAvailableWidth = nRightEdge - nLeftEdge;
  if (nAvailableWidth <= 0)
     return;

  // Get caption text
  CString strCaption;
  GetWindowText (strCaption);

  // Determine drawing format
  DWORD  dwFormat = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
  dwFormat |= (m_bCenter ? DT_CENTER : DT_LEFT);
  if (m_bMultiLine)
     dwFormat = DT_WORDBREAK | DT_LEFT | DT_WORD_ELLIPSIS | DT_VCENTER;

  // Determine dimensions of caption
  CRect rectCaption;
  GetClientRect (&rectCaption);
  int nHeight = ::DrawText (pDC->m_hDC, strCaption.GetBuffer(0),
                            strCaption.GetLength(),
                            &rectCaption,
                            dwFormat | DT_CALCRECT);
  strCaption.ReleaseBuffer();
  int nWidth = rectCaption.Width();

  // Position caption horizontally within available area
  rectCaption.left = nLeftEdge;
  rectCaption.right = nRightEdge;
  if (m_bCenter && (nAvailableWidth > nWidth)) {
      int nOffsetX = (nAvailableWidth - nWidth) / 2;
      rectCaption.left += nOffsetX;
      rectCaption.right -= nOffsetX;
  }

  // Position caption vertically within button
  int nOffsetY = (rectButton.Height() - nHeight)/2;
  rectCaption.top += nOffsetY;
  rectCaption.bottom = rectCaption.top + nHeight;

  // Offset caption if button is pressed
  if ((m_type != FooButton::Type::checkBox) && (m_type != FooButton::Type::radio))
     if (!m_bStatic && !m_bHyperlink && !m_bMultiClicked)
        if (lpDrawItemStruct->itemState & ODS_SELECTED)
           rectCaption.OffsetRect (1, 1);

  // Set text color
  pDC->SetBkMode (TRANSPARENT);
  pDC->SetBkColor (::GetSysColor (COLOR_BTNFACE));
  if (m_bHyperlink)
     pDC->SetTextColor (C_HyperlinkColor);

  // Set underline mode if hyperlink
  CFont* pOldFont = NULL;
  CFont underlineFont;
  if (m_bHyperlink) {
      LOGFONT logFont;
      VERIFY (GetFont()->GetLogFont (&logFont));
      logFont.lfUnderline = TRUE;
      VERIFY (underlineFont.CreateFontIndirect (&logFont));
      pOldFont = pDC->SelectObject (&underlineFont);
  }

  // Draw caption
  if (((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED) && !m_readOnly)
     {
       ::OffsetRect (&rectCaption, 1, 1);
       ::SetTextColor (pDC->m_hDC, ::GetSysColor (COLOR_3DHILIGHT));
       ::DrawTextEx (pDC->m_hDC, strCaption.GetBuffer(0), strCaption.GetLength(),
                     &rectCaption,
                     dwFormat,
                     NULL);
        strCaption.ReleaseBuffer();

       ::OffsetRect (&rectCaption, -1, -1);
       ::SetTextColor (pDC->m_hDC, ::GetSysColor (COLOR_GRAYTEXT));
       ::DrawTextEx (pDC->m_hDC, strCaption.GetBuffer(0), strCaption.GetLength(),
                     &rectCaption,
                     dwFormat,
                     NULL);
     }
  else
     ::DrawTextEx (pDC->m_hDC, strCaption.GetBuffer(0), strCaption.GetLength(),
                   &rectCaption,
                   dwFormat,
                   NULL);
  strCaption.ReleaseBuffer();

  // Free underline font
  if (pOldFont != NULL)
     pDC->SelectObject (pOldFont);
}

//! Draws the button's drop-down indicator
void FooButton::drawDropDown
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Drop-down indicator is drawn for drop-down or multi buttons only
  CRect rectButton;
  GetClientRect (&rectButton);
  int nDropDownLeftEdge = rectButton.right;
  if (!m_bDropDown && !m_bMulti)
     return;

  // Create pen of appropriate color
  COLORREF   rgbPen;
  if (m_bHyperlink)
      rgbPen = GetSysColor (COLOR_HIGHLIGHT);
  else {
      rgbPen = GetSysColor (COLOR_BTNTEXT);
      if ((lpDrawItemStruct->itemState & ODS_DISABLED) == ODS_DISABLED)
         rgbPen = GetSysColor (COLOR_GRAYTEXT);
  }
  CPen  pen;
  VERIFY (pen.CreatePen (PS_SOLID | PS_INSIDEFRAME, 1, rgbPen));

  // Set X and Y offsets
  int nOffsetX = 0;
  if ((lpDrawItemStruct->itemState & ODS_SELECTED) || m_bMultiClicked)
     nOffsetX = 1;
  int nOffsetY = nOffsetX;

  // Draw the indicator
  CPen* pOldPen = pDC->SelectObject (&pen);
  nDropDownLeftEdge = rectButton.right - C_DropDown_X + nOffsetX;
  DrawDropDownIndicator (pDC->m_hDC,
                         nDropDownLeftEdge,
                         rectButton.bottom - C_DropDown_Y + nOffsetY,
                         C_DropDown_Width, C_DropDown_Height);
  pDC->SelectObject (pOldPen);
  nDropDownLeftEdge = rectButton.right - (3 * C_DropDown_X)/2;

  // Draw the vertical separator for multi buttons
  if (m_bMulti && !m_bMultiClicked) {
      rectButton.top += 3;
      rectButton.bottom -= 3;
      rectButton.left = nDropDownLeftEdge;
      rectButton.right = rectButton.left + 2;
      pDC->DrawEdge (&rectButton, BDR_SUNKENOUTER, BF_RECT);
  }
}

//! Draws a multi-button's pressed drop-down region
void FooButton::drawMultiDropDownRegion
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);
  if (!m_bMulti || !m_bMultiClicked)
     return;

  // Get bounds of multi part of button
	CRect rect = lpDrawItemStruct->rcItem;
	rect.left = rect.right - (3 * C_DropDown_X)/2 + 1;
	rect.bottom--;

  // Fill area
  UINT uFrameCtrl = DFCS_BUTTONPUSH | DFCS_CHECKED;
  pDC->DrawFrameControl (&rect, DFC_BUTTON , uFrameCtrl);
}

//! Draws the button's focus rectangle
void FooButton::drawFocus
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct,
   int nLeftEdge,
   int nRightEdge)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Draw focus rectangle (for non-static non-hot buttons)
  if (!m_bStatic && !m_bHot && (getFocusStyle() != FooButton::Focus::noFocus))
     if ((lpDrawItemStruct->itemState & ODS_FOCUS) == ODS_FOCUS) {
        CRect rectButton;
        GetClientRect (&rectButton);
        rectButton.left = nLeftEdge + 4;
        rectButton.right = nRightEdge - 4;
        rectButton.top += 4;
        rectButton.bottom -= 4;
        ::DrawFocusRect (pDC->m_hDC, &rectButton);
     }
}

//! Draws the border for a default button
void FooButton::drawDefaultBorder
  (CDC* pDC,
   LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  ASSERT (pDC != NULL);
  ASSERT (lpDrawItemStruct != NULL);

  // Only pushbuttons can have a default border
  FooButton::Type btnType = getType();
  if ((btnType != FooButton::Type::pushButton) &&
      (btnType != FooButton::Type::pushButtonDropDown) &&
      (btnType != FooButton::Type::pushButtonMulti))
     return;

  // Nothing to do if the default border isn't required
  if (getFocusStyle() != FooButton::Focus::defaultFocus)
     return;

  // Nothing do if the button doesn't have focus or isn't the default button
  if (((lpDrawItemStruct->itemState & ODS_FOCUS) == 0) || !m_bDefaultButton)
     return;

  COLORREF rgbBorder = GetSysColor (COLOR_3DDKSHADOW);
  CBrush borderBrush (rgbBorder);
  CRect innerRect = lpDrawItemStruct->rcItem;
  if ((lpDrawItemStruct->itemState & ODS_SELECTED) == 0) {

      COLORREF rgbBorder = GetSysColor (COLOR_3DDKSHADOW);
      CPen borderGreyPen (PS_SOLID, 1, rgbBorder);
      CPen* pOldPen = pDC->SelectObject (&borderGreyPen );

      pDC->MoveTo (innerRect.right - 2, innerRect.top + 1);
      pDC->LineTo (innerRect.right - 2, innerRect.bottom - 2);
      pDC->LineTo (innerRect.left, innerRect.bottom - 2);

      COLORREF rgbBrGrey = GetSysColor (COLOR_3DHILIGHT);
      CPen brightGreyPen (PS_SOLID, 1, rgbBrGrey);
      pOldPen = pDC->SelectObject (&brightGreyPen);

      pDC->MoveTo (innerRect.right - 3, innerRect.top + 1);
      pDC->LineTo (innerRect.left + 1, innerRect.top + 1);
      pDC->LineTo (innerRect.left + 1, innerRect.bottom - 2);

      COLORREF rgbDkGrey = GetSysColor (COLOR_BTNSHADOW);
      CPen darkGreyPen (PS_SOLID, 1, rgbDkGrey);
      pOldPen = pDC->SelectObject (&darkGreyPen);
      pDC->MoveTo (innerRect.right - 3, innerRect.top + 2);
      pDC->LineTo (innerRect.right - 3, innerRect.bottom - 3);
      pDC->LineTo (innerRect.left + 1, innerRect.bottom - 3);
      pDC->SelectObject (pOldPen);

  } else {
      innerRect.InflateRect (1, 1, -1, -1);
      pDC->FrameRect (&innerRect, &borderBrush);
  }
  pDC->FrameRect(&lpDrawItemStruct->rcItem, &borderBrush);
}

//
//  Purpose:
//    Copies a bitmap transparently onto the destination DC.
//    See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1749/
//
//  Returns:
//    None
//
void FooButton::TransparentBlt
  (HDC      hdcDest,            // destination device context
   int      nXDest,             // x-coord of destination rectangle's upper-left corner
   int      nYDest,             // y-coord of destination rectangle's upper-left corner
   int      nWidth,             // width of destination rectangle
   int      nHeight,            // height of destination rectangle
   HBITMAP  hBitmap,            // source bitmap
   int      nXSrc,              // x-coord of source rectangle's upper-left corner
   int      nYSrc,              // y-coord of source rectangle's upper-left corner
   COLORREF colorTransparent,   // transparent color
   HPALETTE hPal)               // logical palette to be used with bitmap (may be NULL)
{
  CDC dc, memDC, maskDC, tempDC;
  dc.Attach( hdcDest );
  maskDC.CreateCompatibleDC(&dc);
  CBitmap maskBitmap;

  // These store return of SelectObject() calls
  CBitmap* pOldMemBmp = NULL;
  CBitmap* pOldMaskBmp = NULL;
  HBITMAP hOldTempBmp = NULL;

  memDC.CreateCompatibleDC (&dc);
  tempDC.CreateCompatibleDC (&dc);
  CBitmap bmpImage;
  bmpImage.CreateCompatibleBitmap (&dc, nWidth, nHeight);
  pOldMemBmp = memDC.SelectObject (&bmpImage);

  // Select and realize the palette
  if (dc.GetDeviceCaps (RASTERCAPS) & RC_PALETTE && hPal) {
     ::SelectPalette( dc, hPal, FALSE );
     dc.RealizePalette();
     ::SelectPalette( memDC, hPal, FALSE );
  }

  hOldTempBmp = (HBITMAP) ::SelectObject (tempDC.m_hDC, hBitmap);
  memDC.BitBlt (0, 0, nWidth, nHeight, &tempDC, nXSrc, nYSrc, SRCCOPY);

  // Create monochrome bitmap for the mask
  maskBitmap.CreateBitmap (nWidth, nHeight, 1, 1, NULL);
  pOldMaskBmp = maskDC.SelectObject (&maskBitmap);
  memDC.SetBkColor (colorTransparent);

  // Create the mask from the memory DC
  maskDC.BitBlt (0, 0, nWidth, nHeight, &memDC, 0, 0, SRCCOPY);

  // Set the background in memDC to black. Using SRCPAINT with black
  // and any other color results in the other color, thus making
  // black the transparent color
  memDC.SetBkColor (RGB (0,0,0));
  memDC.SetTextColor (RGB (255,255,255));
  memDC.BitBlt (0, 0, nWidth, nHeight, &maskDC, 0, 0, SRCAND);

  // Set the foreground to black. See comment above.
  dc.SetBkColor (RGB (255,255,255));
  dc.SetTextColor (RGB (0,0,0));
  dc.BitBlt (nXDest, nYDest, nWidth, nHeight, &maskDC, 0, 0, SRCAND);

  // Combine the foreground with the background
  dc.BitBlt (nXDest, nYDest, nWidth, nHeight, &memDC, 0, 0, SRCPAINT);

  if (hOldTempBmp)
     ::SelectObject (tempDC.m_hDC, hOldTempBmp);
  if (pOldMaskBmp)
     maskDC.SelectObject (pOldMaskBmp);
  if (pOldMemBmp)
     memDC.SelectObject (pOldMemBmp);

  dc.Detach();
}

//
//  Purpose:
//    Draws a bitmap in a disabled mode onto the destination DC.
//    See http://www.codeguru.com/Cpp/G-M/bitmap/specialeffects/article.php/c1699/
//
//  Returns:
//    None
//
void FooButton::DisabledBlt
  (HDC      hdcDest,    // destination DC
   int      nXDest,     // x coord of destination rectangle's upper left corner
   int      nYDest,     // y coord of destination rectangle's upper left corner
   int      nWidth,     // width of destination rectangle
   int      nHeight,    // height of destination rectangle
   HBITMAP  hBitmap,    // source bitmap
   int      nXSrc,      // x-coord of source rectangle's upper-left corner
   int      nYSrc)      // y-coord of source rectangle's upper-left corner
{
  ASSERT (hdcDest && hBitmap);
  ASSERT (nWidth > 0 && nHeight > 0);

  // Create a generic DC for all BitBlts
  HDC hDC = CreateCompatibleDC (hdcDest);
  ASSERT (hDC);

  if (hDC) {

       // Create a DC for the monochrome DIB section
       HDC bwDC = CreateCompatibleDC(hDC);
       ASSERT (bwDC);

       if (bwDC) {

            // Create the monochrome DIB section with a black and white palette
            struct
            {
              BITMAPINFOHEADER  bmiHeader;
              RGBQUAD           bmiColors[2];
            } RGBBWBITMAPINFO = {
                                  {                               // a BITMAPINFOHEADER
                                    sizeof(BITMAPINFOHEADER),     // biSize
                                    nWidth,                       // biWidth;
                                    nHeight,                      // biHeight;
                                    1,                            // biPlanes;
                                    1,                            // biBitCount
                                    BI_RGB,                       // biCompression;
                                    0,                            // biSizeImage;
                                    0,                            // biXPelsPerMeter;
                                    0,                            // biYPelsPerMeter;
                                    0,                            // biClrUsed;
                                    0                             // biClrImportant;
                                  },
                                  {
                                    { 0x00, 0x00, 0x00, 0x00 },
                                    { 0xFF, 0xFF, 0xFF, 0x00 }
                                  }
                                };
            void*     pbitsBW;
            HBITMAP   hBitmapBW = CreateDIBSection (bwDC,
                                                    (LPBITMAPINFO) &RGBBWBITMAPINFO,
                                                    DIB_RGB_COLORS, &pbitsBW, NULL, 0);
            ASSERT (hBitmapBW);

            if (hBitmapBW) {
                 // Attach the monochrome DIB section and the bitmap to the DCs
                 SelectObject (bwDC, hBitmapBW);
                 SelectObject (hDC, hBitmap);

                 // BitBlt the bitmap into the monochrome DIB section
                 BitBlt (bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);

                 // Paint the destination rectangle in gray
                 FillRect (hdcDest,
                           CRect (nXDest, nYDest, nXDest + nWidth, nYDest + nHeight),
                           GetSysColorBrush (COLOR_3DFACE));

                 // BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
                 // The magic ROP comes from the Charles Petzold's book
                 HBRUSH hb = CreateSolidBrush (GetSysColor (COLOR_3DHILIGHT));
                 HBRUSH oldBrush = (HBRUSH) SelectObject (hdcDest, hb);
                 BitBlt (hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);

                 // BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
                 hb = CreateSolidBrush (GetSysColor (COLOR_3DSHADOW));
                 DeleteObject (SelectObject(hdcDest, hb));
                 BitBlt (hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
                 DeleteObject (SelectObject (hdcDest, oldBrush));
            }
            VERIFY (DeleteDC (bwDC));
       }
       VERIFY (DeleteDC(hDC));
  }
}

//
//  Purpose:
//    Draws a dropdown indicator
//
//  Returns:
//    None
//
void FooButton::DrawDropDownIndicator
  (HDC      hdcDest,            // destination DC
   int      nX,                 // x coord of dropdown indicator's upper left vertex
   int      nY,                 // y coord of dropdown indicator's upper left vertex
   int      nWidthDropDown,     // width of indicator
   int      nHeightDropDown)    // height of indicator
{
int   nCounter;   // generic counter

  ASSERT (hdcDest);
  for (nCounter = 0; (nCounter < nHeightDropDown); nCounter++) {

      POINT   ptStart;    // starting point
      int     nWidth;     // line width

      ptStart.x = nX + nCounter;
      ptStart.y = nY + nCounter;
      nWidth = nWidthDropDown - nCounter;
      if (nWidth < 0)
         break;

      ::MoveToEx (hdcDest, ptStart.x, ptStart.y, NULL);
      ::LineTo (hdcDest, ptStart.x + nWidth - nCounter, ptStart.y);
  }
}

//! Draws the button's frame when hot tracking.
void FooButton::drawHotButtonFrame
  (LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  // Get DC
  ASSERT (lpDrawItemStruct != NULL);
  CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
  ASSERT (pDC != NULL);

  // Draw frame
  CRect rectButton;
  GetClientRect (&rectButton);
  if (m_bTracking) {
      COLORREF  rgbHighlight = GetSysColor (COLOR_BTNHIGHLIGHT);
      COLORREF  rgbShadow = GetSysColor (COLOR_BTNSHADOW);
      COLORREF  rgbBackground = GetSysColor (m_bChecked ? COLOR_3DLIGHT : COLOR_BTNFACE);

      pDC->FillSolidRect (&rectButton, rgbBackground);
      if ((lpDrawItemStruct->itemState & ODS_SELECTED) == ODS_SELECTED)
         pDC->Draw3dRect (&rectButton, rgbShadow, rgbHighlight);
      else
         pDC->Draw3dRect (&rectButton, rgbHighlight, rgbShadow);

      // For multi pushbuttons, fill the area of the drop-down indicator with
      // a light color if the multi button is currently pressed
      if (m_bMulti && m_bMultiClicked)
         drawMultiDropDownRegion (pDC, lpDrawItemStruct);
  }
}

//! Loads the default hyperlink cursor if possible.
void FooButton::loadHyperlinkCursor()
{
  // The following appeared in Paul DiLascia's Jan 1998 MSJ articles.
  // It loads a "hand" cursor from the winhlp32.exe module
  // (Lifted from Chris Maunder's CHyperLink class)

  // Get the windows directory
  CString strWndDir;
  GetWindowsDirectory(strWndDir.GetBuffer(MAX_PATH), MAX_PATH);
  strWndDir.ReleaseBuffer();

  strWndDir += _T("\\winhlp32.exe");
  // This retrieves cursor #106 from winhlp32.exe, which is a hand pointer
  HMODULE hModule = LoadLibrary (strWndDir);
  if (hModule != NULL) {
      HCURSOR hHandCursor = ::LoadCursor(hModule, MAKEINTRESOURCE(106));
      if (hHandCursor != NULL)
          m_hCursor = CopyCursor (hHandCursor);
  }
  FreeLibrary(hModule);
}

//! Checks if the mouse was clicked on multi part of button.
void FooButton::multiHitTest
  (CPoint pt)
{
  // Reset multi-clicked state
  m_bMultiClicked = false;
  if (!m_bMulti)
     return;

  // Get bounds of multi part of button
	CRect rect;
	GetWindowRect (rect);
  rect.left = rect.right - 2*C_DropDown_X + 2;

  // Check if mouse was clicked on multi part of button
  ClientToScreen (&pt);
  if (rect.PtInRect (pt))
     m_bMultiClicked = true;
}

//! Gets a button group by name
//! @param  strGroupName    Group name
//! @param  pGroup          Returned group
void FooButton::getButtonGroup
  (CString strGroupName,
   void* & pGroup)
{
  pGroup = NULL;
  m_btnGroups.Lookup (strGroupName, pGroup);
}

// End FooButton.cpp
