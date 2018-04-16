// This class implement a status bar that contain a progress control.

#if !defined(AFX_STATUSBARWITHPROGRESS_H__INCLUDED_)
#define AFX_STATUSBARWITHPROGRESS_H__INCLUDED_

// StatusBarWithProgress
namespace TA_Base_App
{
	namespace TA_Reporting
	{
		class CStatusBarWithProgress : public CStatusBar
		{
			DECLARE_DYNAMIC(CStatusBarWithProgress)

		public:
			CStatusBarWithProgress(int nProgressBarWidth = 100);
			virtual BOOL Create(CWnd* pParentWnd, DWORD dwStyle  = WS_CHILD | WS_VISIBLE | CBRS_BOTTOM , UINT nID  = AFX_IDW_STATUS_BAR );
			virtual ~CStatusBarWithProgress();

		public:
			/**
		      * IsProgressBarVisible
		      * 
		      * This method return the value of progress control visible or not.
		      *
		      * @param p_cx is an integer value that specifies the width of the 
		      *        application window.
		      *
			  * @return true if the progress control is visible, otherwise return false.
			  *  
		      */
			BOOL IsProgressBarVisible() const   
			{ 
				return  m_bProgressVisible;
			}

			/**
		      * SetProgressBarWidth
		      * 
		      * This method set Progress control bar's width to the specified value.
		      *
		      * @param nWidth is an integer value that specifies the width of the 
		      *        Progress control.
			  *  
		      */
			void SetProgressBarWidth(int nWidth)  
			{
				m_bProgressVisible = nWidth;
			}

			/**
		      * ShowProgressBar
		      * 
		      * This method set the Progress control bar visible or not.
		      *
		      * @param bShow is an boolean value that specifies to show or hide the  
		      *        Progress control.
			  *  
			  * @return the old visible state of Progress bar.
			  *
		      */
			BOOL ShowProgressBar(BOOL bShow = TRUE);

			/**
		      * SetRange
		      * 
		      * This method set the range of Progress control bar.
		      *
		      * @param nLower is an integer value that specifies lower limit for the range of   
		      *        Progress control.
			  * @param nUpper is an integer value that specifies upper limit for the range of
			  *        Progress control.
			  *
		      */
			void SetRange(int nLower, int nUpper)
			{
				m_Progress.SetRange(nLower,nUpper);
			}

			/**
		      * GetPos
		      * 
		      * This method get current position of the Progress control bar.
		      *
			  * @return the value of current position.
			  *  
		      */
			int GetPos()
			{
				return m_Progress.GetPos();
			}

			/**
		      * SetPos
		      * 
		      * This method set Progress control bar to the position specified.
			  *
			  * @param nPos is an integer value that specifies the value of position that
			  *        should be set to Progress control.
			  *
			  * @return the value of current position after set it.
			  *  
		      */
			int SetPos(int nPos)
			{
				return m_Progress.SetPos(nPos);
			}

			/**
		      * OffsetPos
		      * 
		      * This method set Progress control bar offset position.
			  *
			  * @param nPos is an integer value that specifies the value of offset position that
			  *        should be set to Progress control.
			  *
			  * @return the value of offset position.
			  *  
		      */
			int OffsetPos(int nPos)
			{
				return m_Progress.OffsetPos(nPos);
			}

			/**
		      * SetStep
		      * 
		      * This method set the step of Progress control bar.
			  *
			  * @param nStep is an integer value that specifies step value 
			  *         to Progress control.
			  *
			  * @return the old value of step.
			  *  
		      */
			int SetStep(int nStep)
			{
				return m_Progress.SetStep(nStep);
			}

			/**
		      * StepIt
		      * 
		      * This method step once for the Progress bar.
			  *
			  * @return the old value of position before step.
			  *  
		      */
			int StepIt()
			{
				return m_Progress.StepIt();
			}

		protected:
			afx_msg void OnSize(UINT nType, int cx, int cy);
			DECLARE_MESSAGE_MAP()

		// Member variable:
		protected:
			CProgressCtrl m_Progress;   // Progress control bar
			int m_nProgressWidth;		// Width of Progress control bar
			BOOL m_bProgressVisible;   // the value of the Progress control bar visible or not

		public:
			void AdjustProgressBarPosition();

		};
	}
}

#endif
