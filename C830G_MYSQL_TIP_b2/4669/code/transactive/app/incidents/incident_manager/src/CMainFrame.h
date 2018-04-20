/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/CMainFrame.h $
  * @author:  Song Toan
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The CMainFrame class inherits TransActiveFrame. It encapsulates the 
  * functionality of a Windows single document interface frame window. 
  */
#if !defined(CMainFrame_976909C3_D31F_4bbe_A6E3_A391C10217B8__INCLUDED_)
#define CMainFrame_976909C3_D31F_4bbe_A6E3_A391C10217B8__INCLUDED_

#include "bus/generic_gui/src/TransActiveFrame.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "app/incidents/incident_manager/src/CtaIncidentSummary.h"
#include "app/incidents/incident_manager/src/IncidentManagerDoc.h"
#include "app/incidents/incident_manager/src/IncidentManagerFramework.h"

/**
 * @version 1.0
 * @created 16-May-2005 12:18:51 PM
 */
namespace TA_IRS_App
{	
	class CMainFrame: public TA_Base_Bus::TransActiveFrame
	{
		
		DECLARE_DYNCREATE(CMainFrame)
	public:
		void init();
			// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CMainFrame)
	public:
		virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
		//}}AFX_VIRTUAL
		
		/**
		* OnUpdateFrameTitle
		* 
		* override function to set the frame title
		* 
		* @return virtual void 
		* @param : BOOL bAddToTitle
		* 
		* @exception <exceptions> Optional
		*/
		virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
		
#ifdef _DEBUG
		virtual void AssertValid() const;
		virtual void Dump(CDumpContext& dc) const;
#endif
		
		
		// Generated message map functions
	protected:
		//{{AFX_MSG(CMainFrame)
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnClose();
		afx_msg long onIncidentSelectionChange( WPARAM wParam, LPARAM lParam);
		afx_msg void OnUpdateCommandUI(CCmdUI* pCmdUI);
		afx_msg void OnAppAbout();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
			//-------------------------------------------------------------------------
			
	public:
	/**
	* CMainFrame
	* 
	* constructor
	* 
	* @return 
	* 
	* @exception <exceptions> Optional
		*/
		CMainFrame();
		
		/**
		* ~CMainFrame
		* 
		* destructor
		* 
		* @return virtual 
		* 
		* @exception <exceptions> Optional
		*/
		virtual ~CMainFrame();
		/**
		* getMainFrameToolBar
		* 
		* return the control of the toolbar in the mainframe
		* 
		* @return CToolBar* 
		* 
		* @exception <exceptions> Optional
		*/
		CToolBar* getMainFrameToolBar() {return &m_wndToolBar;}
		
		/**
		* getMainFrameProgressCtrl
		* 
		* return the control of the progress control in the mainframe
		* 
		* @return CProgressCtrl* 
		* 
		* @exception <exceptions> Optional
		*/
		CProgressCtrl* getMainFrameProgressCtrl() {return &m_progress;}
	private:
	/**
	* setCallbackObject  This not only sets the callback IGUIAccess object it also
	* performs the generic initialisation required by all windows. This means this
	* must be called after the Window has been created.
	* @param IGuiAccess& - The IGUIAccess object to perform calls on.
	* @param gui
	* 
		*/
		virtual void setCallbackObject(TA_Base_Bus::IGUIAccess& gui);
		
	private:

		// TD15865 Yanrong++
		bool m_beEdited;
		// ++Yanrong
		
		// status bar embedded member
		CStatusBar  m_wndStatusBar;
		//toolbar embedded member
		CToolBar    m_wndToolBar;
		//ProgressCtrl
		CProgressCtrl m_progress;
		/**
		* Points to the document object
		*/
		IncidentManagerDoc *m_IncidentManagerDoc;
		/**
		* Points to the managerframework
		*/
		IncidentManagerFramework *m_IncidentManagerFramework;
		/**
		* Points to the actual object that performs all rights related logics.
		*/
		ItaIncidentRights* m_incidentRights;
		bool m_bNewEnable, m_bEditEnable, m_bViewEnable, m_bDeleteEnable, 
            m_bCloseEnable, m_bReopenEnable, m_bPrintEnable;
		
	public:
		static UINT WM_APP_RESELECTINCIDENT;
		
	};
}// end TA_IRS_App
#endif // !defined(CMainFrame_976909C3_D31F_4bbe_A6E3_A391C10217B8__INCLUDED_)
