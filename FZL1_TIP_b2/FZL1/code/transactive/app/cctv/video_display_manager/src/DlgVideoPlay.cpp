// DlgVideoPlay.cpp : implementation file
//

#include "stdafx.h"
#include "app/cctv/video_display_manager/src/DlgVideoPlay.h"
#include "app/cctv/video_display_manager/src/VirtualMonitorAssigner.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
//#include "app/cctv/QCAP/INC/QCAP.H"
#include <boost/bind.hpp>
#include <cfloat>

namespace TA_IRS_App
{
    IMPLEMENT_DYNAMIC(CDlgVideoPlay, QcapDlg)

    CDlgVideoPlay::CDlgVideoPlay(VirtualMonitorAssigner& virtualMonitorAssigner,
								 float xyRate,
								 CWnd* pParent)
        : QcapDlg(CDlgVideoPlay::IDD, pParent),
		  m_virtualMonitorAssigner(virtualMonitorAssigner),
		  m_xyRate(xyRate)
    {
    }

    CDlgVideoPlay::~CDlgVideoPlay()
    {
    }

    void CDlgVideoPlay::DoDataExchange(CDataExchange* pDX)
    {
        QcapDlg::DoDataExchange(pDX);
		DDX_Control(pDX, IDC_VIDEO, m_videoWnd);
    }


    BEGIN_MESSAGE_MAP(CDlgVideoPlay, QcapDlg)
        //{{AFX_MSG_MAP(CDlgVideoDisplay)
        ON_WM_CLOSE()
        ON_WM_DESTROY()
		ON_WM_LBUTTONDBLCLK()
		ON_WM_SYSCOMMAND()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

	void CDlgVideoPlay::OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		// If window in full screen then normal, else fullscreen
		if(IsZoomed())
		{
			ModifyStyle(0, WS_CAPTION);
			ShowWindow(SW_SHOWNORMAL);
		}
		else
		{
			ModifyStyle( WS_CAPTION,0);
			ShowWindow(SW_SHOWMAXIMIZED);
		}
	}

	void CDlgVideoPlay::OnClose()
	{
		QcapDlg::OnClose();
        m_virtualMonitorAssigner.clearAll();
	}

    void CDlgVideoPlay::OnDestroy()
    {
		QcapDlg::OnDestroy();
    }

    BOOL CDlgVideoPlay::OnInitDialog()
    {
		QcapDlg::OnInitDialog();
		// Adjust video window position and size
		CRect clientRect;
		GetClientRect(&clientRect);
		OnSize(SIZE_RESTORED, clientRect.Width(), clientRect.Height());

		setPlayWindowHandle(m_videoWnd.GetSafeHwnd());
        return TRUE;
    }

	void CDlgVideoPlay::OnSysCommand(UINT nID, LPARAM lParam)
	{
		if(SC_MAXIMIZE == nID)
		{
			ModifyStyle( WS_CAPTION,0);
			ShowWindow(SW_SHOWMAXIMIZED);
		}
		else
		{
			QcapDlg::OnSysCommand(nID, lParam);
		}
	}

	void CDlgVideoPlay::OnSize(UINT nType, int cx, int cy) 
	{
		if(cx <= 0 || cy <= 0)
		{
			return;
		}
		if(m_xyRate >= -FLT_EPSILON && m_xyRate <= FLT_EPSILON)
		{
			if(::IsWindow(m_videoWnd.m_hWnd))
			{
				m_videoWnd.MoveWindow(0, 0, cx, cy);
			}
			return;
		}
		int cx2 = cx;
		int cy2 = (1.0f / m_xyRate) * cx2;
		if(cy2 == 0)
		{
			cy2 = 1;
		}
		if(cy2 > cy)
		{
			cy2 = cy;
			cx2 = cy2 * m_xyRate;
			if(cx2 == 0)
			{
				cx2 = 1;
			}
		}
		int px = (cx-cx2)/2;
		int py = (cy-cy2)/2;
		if(::IsWindow(m_videoWnd.m_hWnd))
		{
			m_videoWnd.MoveWindow(px, py, cx2, cy2);
		}
	}

	// Paint the dialog background colour as black
	BOOL CDlgVideoPlay::OnEraseBkgnd(CDC* pDC)
	{
	    CRect rect;
	    GetClientRect(&rect);
	    CBrush myBrush;
		myBrush.CreateStockObject(BLACK_BRUSH);
	    CBrush *pOld = pDC->SelectObject(&myBrush);
	    BOOL bRes  = pDC->PatBlt(0, 0, rect.Width(), rect.Height(), PATCOPY);
	    pDC->SelectObject(pOld);    // restore old brush
	    return bRes;                       // CDialog::OnEraseBkgnd(pDC);
	}
}