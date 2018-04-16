// This class implement a status bar that contain a progress control.
//

#include "stdafx.h"
#include "StatusBarWithProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_STATUS_PROGRESS  17234   // Identification for Progress control bar

namespace TA_Base_App
{
	namespace TA_Reporting
	{
		// StatusBarWithProgress

		IMPLEMENT_DYNAMIC(CStatusBarWithProgress, CStatusBar)
			CStatusBarWithProgress::CStatusBarWithProgress(int nProgressBarWidth)
			:  m_nProgressWidth( nProgressBarWidth),m_bProgressVisible(FALSE)
		{
		}

		CStatusBarWithProgress::~CStatusBarWithProgress()
		{
		}


		BEGIN_MESSAGE_MAP(CStatusBarWithProgress, CStatusBar)
			ON_WM_SIZE()
		END_MESSAGE_MAP()

		// Create status bar and progress control on status bar. Do not show the progress bar by default.
		BOOL CStatusBarWithProgress::Create(CWnd* pParentWnd, DWORD dwStyle /* = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM  */, UINT nID /* = AFX_IDW_STATUS_BAR */ )
		{
			BOOL bCreatedOK = CStatusBar::Create(pParentWnd, dwStyle, nID);
			if (bCreatedOK)
			{
				m_Progress.Create(WS_CHILD | WS_EX_STATICEDGE ,
					CRect(0, 0, m_nProgressWidth, 10), this, ID_STATUS_PROGRESS);
			}

			return bCreatedOK;
		}

		// Show/Hide Progress control bar
		BOOL CStatusBarWithProgress::ShowProgressBar(BOOL bShow /* = TRUE */)
		{
			// Store the original visible state -- visible or not
			BOOL bOldVisible = m_bProgressVisible;

			if (bOldVisible != bShow)
			{
				// Show/Hide
				m_Progress.ShowWindow(bShow ? SW_SHOWNA:SW_HIDE);
				m_bProgressVisible = bShow;

				// If show the progress control, then should adjust it to a correct position
				if (bShow)
				{
					AdjustProgressBarPosition();
					RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
				}
			}

			return bOldVisible;
		}

		// Adjust progress control to the right hand side of the left panel in status bar
		void CStatusBarWithProgress::AdjustProgressBarPosition()
		{
			// Make sure progress bar is created.
			if (!::IsWindow(m_Progress.m_hWnd))
			{
				return;
			}

			// Find out the size of panel on the left of status bar
			CString str_pane_text;
			CRect progress_rect;
			GetItemRect(0,progress_rect);
			GetPaneText(0,str_pane_text);

			CClientDC temp(this);
			CFont *pOldFont = temp.SelectObject(GetFont());

			progress_rect.left = temp.GetTextExtent(str_pane_text).cx + 5;
			/*progress_rect.right = progress_rect.left + m_nProgressWidth;*/
			progress_rect.InflateRect(0, -1);

			temp.SelectObject(pOldFont);

			// Adjust the progress bar
			m_Progress.SetWindowPos(NULL,progress_rect.left,
				progress_rect.top,
				progress_rect.Width(),
				progress_rect.Height(),
				SWP_NOZORDER | SWP_NOACTIVATE);
		}

		// OnSize, sure that this function will called when the window change
		void CStatusBarWithProgress::OnSize(UINT nType, int cx, int cy)
		{
			CStatusBar::OnSize(nType, cx, cy);

			if (m_bProgressVisible)
			{
				AdjustProgressBarPosition();
			}
		}
	}
}
