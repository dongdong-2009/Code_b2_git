/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/MainTab.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This creates and manages each of the three main tabs at the top of the app.
  *
  */

// MainTab.cpp : implementation file
//

#include "stdafx.h"

#include "TTISManager.h"
#include "MainTab.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CMainTab

    CMainTab::CMainTab()
        : CTabDialogCtrl()
        , m_trainDisplayPage( new CTrainDisplayPage(), true )
        , m_trainTimeSchedulePage( new CTrainTimeSchedulePage(), true )
        , m_trainDownloadPage( new CTrainDownloadPage(), true )
        , m_previousSelect(0)
    {
        FUNCTION_ENTRY( "CMainTab" );

        AddPage(*m_trainDisplayPage.m_dialog,IDD_TRAIN_DISPLAY,CString("Train Display"));
        AddPage(*m_trainTimeSchedulePage.m_dialog,IDD_TRAIN_TIME_SCHEDULE,CString("Train Time Schedule"));
        AddPage(*m_trainDownloadPage.m_dialog,IDD_TRAIN_DOWNLOAD,CString("Train Download/Upgrade"));

        // TD 8342 - Stations not able to select Timetable functionality
        TA_Base_Core::ILocation::ELocationType locationType =
            TA_IRS_Bus::LocationCache::getInstance().getLocationTypeByKey(TA_IRS_Bus::LocationCache::getInstance().getLocationKey());
        if (locationType == TA_Base_Core::ILocation::STATION)
        {
            this->m_trainTimeSchedulePage.setEnabled( false );
        }

        FUNCTION_EXIT;
    }


    CMainTab::~CMainTab()
    {
        FUNCTION_ENTRY( "~CMainTab" );

        delete m_trainDisplayPage.m_dialog;
        delete m_trainTimeSchedulePage.m_dialog;
        delete m_trainDownloadPage.m_dialog;

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(CMainTab, CTabDialogCtrl)
        // {{AFX_MSG_MAP(CMainTab)
        ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
            // NOTE - the ClassWizard will add and remove mapping macros here.
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CMainTab message handlers


    void CMainTab::Init(REBProgressManager & mgr)
    {
        FUNCTION_ENTRY( "Init" );

        mgr.SetStaticText(0, "Initializing the Train Display Page...");
        m_trainDisplayPage.m_dialog->Init(mgr);
        mgr.SetProgress(33);
        mgr.SetStaticText(0, "Initializing the Train Time Schedule Page...");
        m_trainTimeSchedulePage.m_dialog->Init(mgr);
        mgr.SetProgress(66);
        mgr.SetStaticText(0, "Initializing the Train Download Page...");
        m_trainDownloadPage.m_dialog->Init(mgr);
        mgr.SetProgress(98);

        FUNCTION_EXIT;
    }


    BOOL CMainTab::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                case VK_RETURN:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                default:
                    {
                        // do nothing
                    }
                    break;
                }
            }
            break;
        default:
            {
                // do nothing
            }
        }

        FUNCTION_EXIT;
        return CTabDialogCtrl::PreTranslateMessage(pMsg);
    }


    void CMainTab::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        // Perform normal stuff
        CTabDialogCtrl::PreSubclassWindow();

        ModifyStyle(0, TCS_OWNERDRAWFIXED);

        FUNCTION_EXIT;
    }


    //////////////////
    // Selection is changing: disallow if tab is disabled
    // 
    BOOL CMainTab::OnSelchange(NMHDR* pnmh, LRESULT* pRes)
    {
        FUNCTION_ENTRY( "OnSelchange" );

        // Figure out index of new tab we are about to go to, as opposed
       // to the current one we're at. Believe it or not, Windows doesn't
       // pass this info
       // 
       TC_HITTESTINFO htinfo;
       GetCursorPos(&htinfo.pt);
       ScreenToClient(&htinfo.pt);
       int iNewTab = HitTest(&htinfo);

       // TD16095
       // zhou yuna++
       if (iNewTab == 0 && !m_trainDisplayPage.isEnabled())
       {
          *pRes = TRUE; // tab disabled: prevent selection
          SetCurSel(m_previousSelect);

          FUNCTION_EXIT;
          return TRUE;
       }
       else if (iNewTab == 1 && !m_trainTimeSchedulePage.isEnabled())
       {
          *pRes = TRUE; // tab disabled: prevent selection
          SetCurSel(m_previousSelect);

          FUNCTION_EXIT;
          return TRUE;
       }
       else if (iNewTab == 2 && !m_trainDownloadPage.isEnabled())
       {
          *pRes = TRUE; // tab disabled: prevent selection
          SetCurSel(m_previousSelect);

          FUNCTION_EXIT;
          return TRUE;
       }

       m_previousSelect = iNewTab;
       // ++zhou yuan
       FUNCTION_EXIT;
       return CTabDialogCtrl::OnSelchange( pnmh, pRes );
    }


    //////////////////
    // Draw the tab: mimic SysTabControl32, except use gray if tab is disabled
    // 
    void CMainTab::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        FUNCTION_ENTRY( "DrawItem" );

        DRAWITEMSTRUCT& ds = *lpDrawItemStruct;

        int iItem = ds.itemID;

        // Get tab item info
        char text[128];
        TCITEM tci;
        tci.mask = TCIF_TEXT;
        tci.pszText = text;
        tci.cchTextMax = sizeof(text);
        GetItem(iItem, &tci);

        // use draw item DC
        CDC dc;
        dc.Attach(ds.hDC);

        // draw text background
        CRect bkRc = ds.rcItem;
        bkRc.top += ::GetSystemMetrics(SM_CYEDGE);
        dc.SetBkMode(TRANSPARENT);
        dc.FillSolidRect(bkRc, ::GetSysColor(COLOR_BTNFACE));

        // calculate text rectangle
        CRect rc = ds.rcItem;
        rc += CPoint(0,3);

        // draw the text
        if (iItem == 0 && !m_trainDisplayPage.isEnabled())
        {
            OnDrawText(dc, rc, text, true);
        }
        else if (iItem == 1 && !m_trainTimeSchedulePage.isEnabled())
        {
            OnDrawText(dc, rc, text, true);
        }
        else if (iItem == 2 && !m_trainDownloadPage.isEnabled())
        {
            OnDrawText(dc, rc, text, true);
        }
        else
        {
            OnDrawText(dc, rc, text, false);
        }

        dc.Detach();

        FUNCTION_EXIT;
    }


    //////////////////
    // Draw tab text. You can override to use different color/font.
    // 
    void CMainTab::OnDrawText(CDC& dc, CRect rc,
       CString sText, BOOL bDisabled)
    {
        FUNCTION_ENTRY( "OnDrawText" );

        if (bDisabled)
        {
            // Create a shadow effect by first drawing the text in light highlight colour
            // at a slight south-east offset.
            CRect shadowRc(rc);
            shadowRc += CPoint(1,1);
            dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            dc.DrawText(sText, &shadowRc, DT_CENTER | DT_VCENTER);
        }

        // Draw the text in normal or disabled text colour
        dc.SetTextColor(::GetSysColor(bDisabled? COLOR_GRAYTEXT : COLOR_BTNTEXT));
        dc.DrawText(sText, &rc, DT_CENTER | DT_VCENTER);

        FUNCTION_EXIT;
    }


    CTrainDisplayPage* CMainTab::getTrainDisplayPage()
    {
        FUNCTION_ENTRY( "getTrainDisplayPage" );
        FUNCTION_EXIT;
        return m_trainDisplayPage.m_dialog;
    }


} // TA_IRS_App
