// PTZButton.cpp : implementation file
//

#include "stdafx.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/PTZButton.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CPTZButton

//IMPLEMENT_DYNAMIC(CPTZButton, CButton)

namespace TA_IRS_App
{
	Monitor* CPTZButton::m_activeMonitor = NULL;
	int CPTZButton::s_ptzRequestInterval = 1000;

	CPTZButton::CPTZButton()
	{
	}

    CPTZButton::~CPTZButton()
    {
    }


    BEGIN_MESSAGE_MAP ( CPTZButton, CButton )

        //ON_WM_TIMER()
        ON_WM_LBUTTONUP()
        ON_WM_LBUTTONDOWN()
		ON_WM_TIMER()
        //ON_WM_CREATE()
    END_MESSAGE_MAP()


	void CPTZButton::setBtnDownActionHandler(boost::function<void (Camera*)> actionHandler)
	{
		m_btnDownActionHandler = actionHandler;
	}

	void CPTZButton::setBtnUpActionHandler(boost::function<void (Camera*)> actionHandler)
	{
		m_btnUpActionHandler = actionHandler;
	}

	void CPTZButton::OnTimer(UINT_PTR nIDEvent)
	{
		Camera* camera = dynamic_cast<Camera*>(m_activeMonitor->getInput());
		if(camera != NULL)
		{
			try
			{
				m_btnDownActionHandler(camera);
			}
			catch (...)
			{
			}
		}
	}

    void CPTZButton::OnLButtonUp ( UINT nFlags, CPoint point )
    {
		//KillTimer(1);
        SetIcon ( m_iconUp );
        CRect rect;
        GetWindowRect ( &rect );
        ICONINFO info;
        GetIconInfo ( m_iconUp, &info );
        CDC *pDC = GetDC();
        pDC->DrawIcon ( CPoint ( rect.Width() / 2 - info.xHotspot, rect.Height() / 2 - info.yHotspot ) , m_iconUp );
        CButton::OnLButtonUp ( nFlags, point );
		TA_ASSERT(m_btnUpActionHandler, "");
		if(m_activeMonitor != NULL)
		{
			Camera* camera = dynamic_cast<Camera*>(m_activeMonitor->getInput());
			if(camera != NULL)
			{
				try
				{
					m_btnUpActionHandler(camera);
				}
				catch (...)
				{
				}
			}
		}
    }

    void CPTZButton::OnLButtonDown ( UINT nFlags, CPoint point )
    {
        SetIcon ( m_iconDown );
        CRect rect;
        GetWindowRect ( &rect );
        ICONINFO info;
        GetIconInfo ( m_iconDown, &info );
        CDC *pDC = GetDC();
        pDC->DrawIcon ( CPoint ( rect.Width() / 2 - info.xHotspot, rect.Height() / 2 - info.yHotspot ) , m_iconDown );
        CButton::OnLButtonDown ( nFlags, point );
		TA_ASSERT(m_btnDownActionHandler, "");
		if(m_activeMonitor != NULL)
		{
			Camera* camera = dynamic_cast<Camera*>(m_activeMonitor->getInput());
			if(camera != NULL)
			{
				//SetTimer(1, s_ptzRequestInterval, NULL);		//fuzhou project only send command once
				try
				{
					m_btnDownActionHandler(camera);
				}
				catch (...)
				{
				}
			}
		}
    }


    void CPTZButton::setIcons ( UINT m_iconMouseUp )
    {
        HINSTANCE   hInstResource   = NULL;
        HINSTANCE   hInstResource1  = NULL;
        BOOL Ret = TRUE;
        hInstResource = AfxFindResourceHandle ( MAKEINTRESOURCE ( m_iconMouseUp ), RT_GROUP_ICON );
        m_iconUp = ( HICON ) ::LoadIcon ( hInstResource, MAKEINTRESOURCE ( m_iconMouseUp ) );
        if ( m_iconUp == NULL )
        {
            return;
        }
        m_iconDown = CreateDarkerIcon ( m_iconUp );
        if ( m_iconDown == NULL )
        {
            return;
        }
        SetIcon ( m_iconUp );
        this->Invalidate();
        return;
    }

    HICON CPTZButton::CreateDarkerIcon ( HICON hIcon )
    {
        HICON       hGrayIcon = NULL;
        HDC         hMainDC = NULL, hMemDC1 = NULL, hMemDC2 = NULL;
        BITMAP      bmp;
        HBITMAP     hOldBmp1 = NULL, hOldBmp2 = NULL;
        ICONINFO    csII, csGrayII;
        BOOL        bRetValue = FALSE;
        bRetValue = ::GetIconInfo ( hIcon, &csII );
        if ( bRetValue == FALSE ) return NULL;
        hMainDC = ::GetDC ( NULL );
        hMemDC1 = ::CreateCompatibleDC ( hMainDC );
        hMemDC2 = ::CreateCompatibleDC ( hMainDC );
        if ( hMainDC == NULL || hMemDC1 == NULL || hMemDC2 == NULL )  return NULL;
        if ( ::GetObject ( csII.hbmColor, sizeof ( BITMAP ), &bmp ) )
        {
            DWORD   dwWidth = csII.xHotspot * 2;
            DWORD   dwHeight = csII.yHotspot * 2;
            csGrayII.hbmColor = ::CreateBitmap ( dwWidth, dwHeight, bmp.bmPlanes, bmp.bmBitsPixel, NULL );
            if ( csGrayII.hbmColor )
            {
                hOldBmp1 = ( HBITMAP ) ::SelectObject ( hMemDC1, csII.hbmColor );
                hOldBmp2 = ( HBITMAP ) ::SelectObject ( hMemDC2, csGrayII.hbmColor );
                //::BitBlt(hMemDC2, 0, 0, dwWidth, dwHeight, hMemDC1, 0, 0, SRCCOPY);
                DWORD       dwLoopY = 0, dwLoopX = 0;
                COLORREF    crPixel = 0;
                for ( dwLoopY = 0; dwLoopY < dwHeight; dwLoopY++ )
                {
                    for ( dwLoopX = 0; dwLoopX < dwWidth; dwLoopX++ )
                    {
                        crPixel = ::GetPixel ( hMemDC1, dwLoopX, dwLoopY );
                        if ( crPixel )
						{
                            ::SetPixel ( hMemDC2, dwLoopX, dwLoopY, DarkenColor ( crPixel, 0.25 ) );
						}
                        else
						{
                            ::SetPixel ( hMemDC2, dwLoopX, dwLoopY, crPixel );
						}
                    }
                }
                ::SelectObject ( hMemDC1, hOldBmp1 );
                ::SelectObject ( hMemDC2, hOldBmp2 );
                csGrayII.hbmMask = csII.hbmMask;
                csGrayII.fIcon = TRUE;
                hGrayIcon = ::CreateIconIndirect ( &csGrayII );
            } // if
            ::DeleteObject ( csGrayII.hbmColor );
            //::DeleteObject(csGrayII.hbmMask);
        }
        ::DeleteObject ( csII.hbmColor );
        ::DeleteObject ( csII.hbmMask );
        ::DeleteDC ( hMemDC1 );
        ::DeleteDC ( hMemDC2 );
        ::ReleaseDC ( NULL, hMainDC );
        return hGrayIcon;
    }


    COLORREF CPTZButton::DarkenColor ( COLORREF crColor, double dFactor )
    {
        if ( dFactor > 0.0 && dFactor <= 1.0 )
        {
            BYTE red, green, blue, lightred, lightgreen, lightblue;
            red = GetRValue ( crColor );
            green = GetGValue ( crColor );
            blue = GetBValue ( crColor );
            lightred = ( BYTE ) ( red - ( dFactor * red ) );
            lightgreen = ( BYTE ) ( green - ( dFactor * green ) );
            lightblue = ( BYTE ) ( blue - ( dFactor * blue ) );
            crColor = RGB ( lightred, lightgreen, lightblue );
        }
        return crColor;
    } // End of DarkenColor

	void CPTZButton::setActiveMonitor(Monitor* monitor)
	{
		m_activeMonitor = monitor;
	}

    Monitor* CPTZButton::getActiveMonitor()
	{
		return m_activeMonitor;
	}

	void CPTZButton::setPtzRequestInterval( int ptzRequestInterval )
	{
		s_ptzRequestInterval = ptzRequestInterval;
	}
}
// CPTZButton message handlers


