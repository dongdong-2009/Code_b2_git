/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoSourcesListBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// VideoSourcesListBox.cpp : implementation file
//

#include "stdafx.h"
#include "videodisplaymanager.h"
#include "VideoSourcesListBox.h"
#include "SequenceEditorDlg.h"
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif

namespace TA_IRS_App
{

    VideoSourcesListBox::VideoSourcesListBox()
      : m_prevSelection(0),
        m_nCurItem(-1),
        m_bCapture(FALSE),
        m_wndTip(),
        m_tipPopDelay(500),
        m_tipInitialiseDelay(250),
        m_tipTransferDelay(0),
        m_tipTimeoutDelay(5000)
    {
        FUNCTION_ENTRY( "VideoSourcesListBox" );
        FUNCTION_EXIT;
    }


    VideoSourcesListBox::~VideoSourcesListBox()
    {
        FUNCTION_ENTRY( "~VideoSourcesListBox" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(VideoSourcesListBox, CListBox)
        // {{AFX_MSG_MAP(VideoSourcesListBox)
        ON_CONTROL_REFLECT(LBN_KILLFOCUS, OnKillfocus)
        ON_CONTROL_REFLECT(LBN_SETFOCUS, OnSetfocus)
        ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
        ON_WM_MOUSEACTIVATE()
        ON_WM_LBUTTONDOWN()
        ON_WM_MOUSEMOVE()
        ON_WM_TIMER()
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VideoSourcesListBox message handlers

    void VideoSourcesListBox::OnKillfocus()
    {
        FUNCTION_ENTRY( "OnKillfocus" );

        // TODO: Add your control notification handler code here
        if ( ((SequenceEditorDlg*)(GetParent()))->enableRemoveButton(FALSE) )
        {
            HWND focusWnd = GetFocus()->GetSafeHwnd();
            if ( ( -1 != GetCurSel() ) && (focusWnd != m_wndTip.GetSafeHwnd()) )
            {
                m_prevSelection = GetCurSel();
                SetCurSel(-1);
                ((SequenceEditorDlg*)(GetParent()))
                ->enableOrderButtons(FALSE,FALSE,FALSE,FALSE);
            }
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::OnSetfocus()
    {
        FUNCTION_ENTRY( "OnSetfocus" );

        // TODO: Add your control notification handler code here
        if ( GetCount() == 0 )
        {
            FUNCTION_EXIT;
            return;
        }
        else if (GetCurSel()>=0)
        {
            updateButtonEnable();

            FUNCTION_EXIT;
            return;
        }
        else if ( m_prevSelection >= GetCount() )
        {
            while ( m_prevSelection >= GetCount() )
            {
                m_prevSelection--;
            }
        }
        else if ( m_prevSelection < 0 )
        {
            m_prevSelection = 0;
        }

        if ( GetCurSel() < 0)
        {
            SetCurSel(m_prevSelection);
        }

        updateButtonEnable();

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::OnSelchange()
    {
        FUNCTION_ENTRY( "OnSelchange" );

        // TODO: Add your control notification handler code here
        updateButtonEnable();

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::updateButtonEnable()
    {
        FUNCTION_ENTRY( "updateButtonEnable" );

        int curSel = GetCurSel();
        if ( curSel != LB_ERR )
        {
            BOOL top, up, down, bottom;
            top = up = curSel > 0;
            bottom = down = curSel < (GetCount() - 1);
            ((SequenceEditorDlg*)(GetParent()))
                ->enableOrderButtons(top,up,down,bottom);
            ((SequenceEditorDlg*)(GetParent()))->enableRemoveButton(TRUE);
        }

        FUNCTION_EXIT;
    }


    int VideoSourcesListBox::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
    {
        FUNCTION_ENTRY( "OnMouseActivate" );

        // TODO: Add your message handler code here and/or call default
        if ( (nHitTest == HTVSCROLL) || (nHitTest == HTHSCROLL) )
        {
            SetFocus();
        }

        FUNCTION_EXIT;
        return CListBox::OnMouseActivate(pDesktopWnd, nHitTest, message);
    }


    void VideoSourcesListBox::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonDown" );

        // TODO: Add your message handler code here and/or call default
        BOOL outside;
        // CRect rect;
        int nearest = ItemFromPoint(point, outside);
        // GetItemRect(nearest, &rect);
        if (!outside)
        {
            m_prevSelection = nearest;
            if ( (nearest != GetCurSel()) && m_bCapture )
            {
                m_wndTip.Cancel();
                m_wndTip.ShowDelayed(m_tipPopDelay);
            }
        }

        CListBox::OnLButtonDown(nFlags, point);
        updateButtonEnable();
        if (m_bCapture)
        {
            SetCapture();
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::addSourceString(LPCTSTR lpszItem)
    {
        FUNCTION_ENTRY( "addSourceString" );

        CString str(lpszItem);
        CString prefix("");
        int count = this->GetCount();
        count++;
        prefix.Format("%d",count);
    // if (count < 10)
            prefix = "#" + prefix + ":  ";
    // else
    // prefix = "#" + prefix + ":  ";
        str.Insert(0,prefix);
        this->AddString(str);
        CSize   sz;
        int     dx=this->GetHorizontalExtent();
        CDC*    pDC = this->GetDC();
        sz = pDC->GetTextExtent(str);
        if (sz.cx > dx)
        {
            dx = sz.cx;
            this->SetHorizontalExtent(dx);
        }
        this->ReleaseDC(pDC);

        // Set the horizontal extent so every character of all strings
        // can be scrolled to.

        // select the newly inserted item
        SelectString(-1, str);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::deleteSourceString()
    {
        FUNCTION_ENTRY( "deleteSourceString" );

        int curSelect = GetCurSel();
        if ( curSelect < 0 )
        {
            FUNCTION_EXIT;
            return;
        }

        DeleteString(curSelect);
        CString prefix, item;
        for (int i = curSelect; i < GetCount(); i++)
        {
            item.Empty();
            prefix.Empty();
            GetText(i,item);
            prefix.Format("%d",(i+1));
            DeleteString(i);
            item.Delete(0,(item.Find(":")+3));
    // if (i < 9 )
                InsertString(i, ("#" + prefix + ":  " + item));
    // else
    // InsertString(i, ("#" + prefix + ": " + item));
        }

        if (GetCount() < 1)
        {
            SetHorizontalExtent(0);
        }
        else
        {
            CString str;
            CSize   sz;
            int     dx=0;
            CDC*    pDC = this->GetDC();
            for (int i=0;i < this->GetCount();i++)
            {
              this->GetText( i, str );
              sz = pDC->GetTextExtent(str);

              if (sz.cx > dx)
              {
                  dx = sz.cx;
              }

            }
            this->ReleaseDC(pDC);
            this->SetHorizontalExtent(dx);
            m_prevSelection = curSelect;
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::moveSourceBottom()
    {
        FUNCTION_ENTRY( "moveSourceBottom" );

        int curSelect = GetCurSel();
        if ( curSelect < 0 )
        {
            FUNCTION_EXIT;
            return;
        }
        CString str;
        this->GetText(curSelect,str);
        this->DeleteString(curSelect);
        this->AddString(str);
        CString prefix, item;
        for (int i = curSelect; i < GetCount(); i++)
        {
            item.Empty();
            prefix.Empty();
            GetText(i,item);
            prefix.Format("%d",(i+1));
            DeleteString(i);
            item.Delete(0,(item.Find(":")+3));
    // if (i < 9 )
                InsertString(i, ("#" + prefix + ":  " + item));
    // else
    // InsertString(i, ("#" + prefix + ": " + item));
        }
        this->SetCurSel(GetCount() - 1);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::moveSourceUp()
    {
        FUNCTION_ENTRY( "moveSourceUp" );

        int curSelect = GetCurSel();
        if ( curSelect < 1 )
        {
            FUNCTION_EXIT;
            return;
        }
        CString str;
        this->GetText(curSelect,str);
        this->InsertString((curSelect-1),str);
        this->DeleteString(curSelect+1);
        CString prefix, item;
        for (int i = curSelect-1; i <= curSelect; i++)
        {
            item.Empty();
            prefix.Empty();
            GetText(i,item);
            prefix.Format("%d",(i+1));
            DeleteString(i);
            item.Delete(0,(item.Find(":")+3));
        // if (i < 9 )
                InsertString(i, ("#" + prefix + ":  " + item));
        // else
        // InsertString(i, ("#" + prefix + ": " + item));
        }
        this->SetCurSel(curSelect-1);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::moveSourceTop()
    {
        FUNCTION_ENTRY( "moveSourceTop" );

        int curSelect = GetCurSel();
        if ( curSelect < 1 )
        {
            FUNCTION_EXIT;
            return;
        }
        CString str;
        this->GetText(curSelect,str);
        this->DeleteString(curSelect);
        this->InsertString(0,str);
        CString prefix, item;
        for (int i = 0; i < GetCount(); i++)
        {
            item.Empty();
            prefix.Empty();
            GetText(i,item);
            prefix.Format("%d",(i+1));
            DeleteString(i);
            item.Delete(0,(item.Find(":")+3));
        // if (i < 9 )
                InsertString(i, ("#" + prefix + ":  " + item));
        // else
        // InsertString(i, ("#" + prefix + ": " + item));
        }
        this->SetCurSel(0);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::moveSourceDown()
    {
        FUNCTION_ENTRY( "moveSourceDown" );

        int curSelect = GetCurSel();
        if ( (curSelect < 0 ) || (curSelect >= GetCount()) )
        {
            FUNCTION_EXIT;
            return;
        }
        CString str;
        this->GetText(curSelect,str);
        this->DeleteString(curSelect);
        this->InsertString((curSelect+1),str);
        CString prefix, item;
        for (int i = curSelect; i <= curSelect+1; i++)
        {
            item.Empty();
            prefix.Empty();
            GetText(i,item);
            prefix.Format("%d",(i+1));
            DeleteString(i);
            item.Delete(0,(item.Find(":")+3));
            InsertString(i, ("#" + prefix + ":  " + item));
        }
        this->SetCurSel(curSelect+1);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::OnMouseMove(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnMouseMove" );

        KillTimer(1);
        SetTimer(1,m_tipTimeoutDelay,NULL);
        // Get text and text rectangle for item under mouse
       CString sText; // item text
       CRect rcText;  // item text rect
       int nItem = OnGetItemInfo(point, rcText, sText);

       if ( (nItem==-1) || (nItem!=m_nCurItem) )
       {
           BOOL tipVisible = m_wndTip.IsWindowVisible();
           m_wndTip.Cancel(); // new item, or no item: cancel popup text
           if (nItem>=0 && !IsRectCompletelyVisible(rcText))
           {
               // new item, and not wholly visible: prepare popup tip
               CRect rc = rcText;
               ClientToScreen(&rc);   // text rect in screen coords
               m_wndTip.SetWindowText(sText);   // set tip text to that of item

               // move tip window over list text
               rc.OffsetRect(-GetScrollPos(SB_HORZ),0);
               m_wndTip.SetWindowPos(NULL, rc.left-1, rc.top-2, rc.Width()+6,
                   rc.Height()+4, SWP_NOZORDER|SWP_NOACTIVATE);
               if ( FALSE == tipVisible )
               {
                   m_wndTip.ShowDelayed(m_tipInitialiseDelay); // show popup text delayed
                    KillTimer(1);
                    SetTimer(1,(m_tipTimeoutDelay + m_tipInitialiseDelay),NULL);
               }
               else
               {
                   m_wndTip.ShowDelayed(m_tipTransferDelay); // show popup text delayed
                    KillTimer(1);
                    SetTimer(1,(m_tipTimeoutDelay + m_tipTransferDelay),NULL);
               }
               if ( !m_bCapture)
               {
                    SetCapture();
                    m_bCapture = TRUE;
               }
           }
       }
       m_nCurItem = nItem;

       if ( (m_bCapture) && (nItem==-1) )
       {
           ReleaseCapture();
           m_bCapture=FALSE;
       }
       CListBox::OnMouseMove(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesListBox::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        // TODO: Add your specialized code here and/or call the base class
        if (!m_wndTip)
        {
            m_wndTip.Create(CPoint(0,0), this, PTS_TRANSPARENT);
        }
        CListBox::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    BOOL VideoSourcesListBox::IsRectCompletelyVisible(const CRect& rc)
    {
        FUNCTION_ENTRY( "IsRectCompletelyVisible" );

        CRect rcClient;
        GetClientRect(&rcClient);

        FUNCTION_EXIT;
        return rcClient.Width() > rc.Width();
    }


    int VideoSourcesListBox::OnGetItemInfo(CPoint p, CRect& rc, CString& s)
    {
        FUNCTION_ENTRY( "OnGetItemInfo" );

        BOOL bOutside;
       int nItem = ItemFromPoint(p,bOutside);
       s.Empty();
       if (!bOutside)
       {
            GetText(nItem, s);
            GetItemRect(nItem, &rc);
            CFont *pFont = GetFont();
            CClientDC dc(this);
            CFont* pOldFont = dc.SelectObject(pFont);
            dc.DrawText(s,&rc,DT_CALCRECT);
            dc.SelectObject(pOldFont);

            FUNCTION_EXIT;
            return nItem;
       }

       FUNCTION_EXIT;
       return -1;
    }


    void VideoSourcesListBox::OnTimer(UINT nIDEvent)
    {
        FUNCTION_ENTRY( "OnTimer" );

        // TODO: Add your message handler code here and/or call default
        if ( 1 == nIDEvent )
        {
            m_wndTip.Cancel();
        }
        CListBox::OnTimer(nIDEvent);

        FUNCTION_EXIT;
    }


    BOOL VideoSourcesListBox::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        // TODO: Add your specialized code here and/or call the base class
        switch ( pMsg->message )
        {
        case WM_MOUSEWHEEL:
            {
                // reset  the "tooltip wnd"
                m_nCurItem = -1;
                m_wndTip.Cancel();
            }
            break;
        default:
            {
                // do nothing
            }
            break;
        }

        FUNCTION_EXIT;
        return CListBox::PreTranslateMessage(pMsg);
    }


} // TA_IRS_App
