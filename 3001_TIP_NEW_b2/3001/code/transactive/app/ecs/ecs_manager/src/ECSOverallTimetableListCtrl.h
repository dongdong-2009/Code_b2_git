/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSOverallTimetableListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined (_AFX_ECSOVERALLTIMETABLELISTCTRL_H_)
#define _AFX_ECSOVERALLTIMETABLELISTCTRL_H_

namespace TA_IRS_App
{
	class RowHighlight
	{
	public:
		int iCurrentSelection;
		int iClickedSelection;
		unsigned long lLocationKey;
		RowHighlight(){
			iCurrentSelection = -1;
			iClickedSelection = -1;
			lLocationKey      = -1;
		}
	};
}

namespace TA_IRS_App
{
	class MappedECSDataCache;

	class COverallECSTimetableListCtrl : public CListCtrl
	{
		// Construction
	public:
		COverallECSTimetableListCtrl();		
		// Attributes
	public:		
		CFont			m_ArialFont;
		CRect			m_SelectionRect;
		BOOL			m_SelectionFlag;
		CPoint			m_Point;
		int				m_Columns;
		int				m_PrevItem;
		int				m_iCurRow;
		int				m_iCurCol;
		CImageList		m_StationSelectionImageList;
		// Operations
	public:
		void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		void OnInitialUpdate();
		void SetNoOfColumns    (int n=1)	{ m_Columns = n ; }
		void GetRowColumn();
		void SelectColumn(int isubItem);
		void ClearClickedItems();
		
		//{{AFX_VIRTUAL(COverallECSTimetableListCtrl)
	protected:
		//}}AFX_VIRTUAL	
		
	public:
		void SetStyles(DWORD dwStyle=LVS_EX_GRIDLINES| LVS_EX_HEADERDRAGDROP);
		virtual ~COverallECSTimetableListCtrl();	// Implementation();
		
		// Generated message map functions
	protected:
		//{{AFX_MSG(COverallECSTimetableListCtrl)
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);	
		//}}AFX_MSG
		
		DECLARE_MESSAGE_MAP()
	};
}

#endif