/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source: /cvs/1TS-9999-T/code/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/Attic/CRadioStatusPage.h,v $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $Date: 2009/12/15 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#if !defined(CRADIOSTATUSPAGE_H)
#define CRADIOSTATUSPAGE_H

#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/CResourceButton.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/IResourceButtonContainer.h"
#include "app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h" 

#include <vector>
#include <set>
#include "afxwin.h"




class CRadioStatusPage : public CPropertyPage, public IResourceButtonContainer
{

	public:
		
		CRadioStatusPage(RECT* parentRect = NULL);
		virtual ~CRadioStatusPage();
        
        void test();

		//{{AFX_DATA(CRadioStatusPage)
		enum { IDD = IDD_RADIOSTATUS_PAGE };
		CStatic	m_cadText;
		CStatic m_broadcastText;
		//CButton m_emergencyBtn;
		//}}AFX_DATA


		//{{AFX_VIRTUAL(CRadioStatusPage)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	public:

		/**
		 * Check if the dropped resource is already monitored
		 * 
		 */
		bool isMonitored(int nProfile,const RadioResource& resource);

		int getTalkgroupSize();
		int getCadSize();

		//CResourceButton* getRadioMonitorBtn();
		//void setRadioMonitorBtn(CResourceButton* btn);
		
		CResourceButton* getResourceButton(unsigned int butRef);

		static _inline COLORREF _stdcall GetParentDlgBkColor2(IN CONST HWND hWnd);


	// Implementation
	protected:
		//{{AFX_MSG(CRadioStatusPage)
		virtual BOOL OnInitDialog();
		afx_msg void OnTimer(UINT nIDEvent);
		afx_msg LRESULT onNotifiedDatabaseReady(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onNotifiedEntitySelectionChanged(WPARAM wParam = 0, LPARAM lParam = 0);
		afx_msg LRESULT onNotifiedInitialiseMode(WPARAM wParam = 0, LPARAM lParam = 0); // Fix TD#13911
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		virtual void OnPaint();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
				
	public:

		/**
		 * Callback for WM_TIMER message. This will be used to toggle the "flashing" of
		 * the buttons.
		 */
		void addTimerObserver(CWnd* observer);
		void removeTimerObserver(CWnd* observer);

		void setButtonState(int butRef, RadioButtonState state);
		RadioButtonState getButtonState(int butRef);
		bool isAnyButtonPress();

		bool isMonitoredTgButRef(int butRef, const RadioResource& resource);

	private:

		//void createProfileButtons(RECT rc, int xPos, int yPos);
		//void createUserDefinedButtons(RECT rc, int xPos, int yPos);
		void createTalkgroupStatusButtons(RECT rc, int xPos, int yPos);
		void createCADStatusButtons(RECT rc, int xPos, int yPos);
		void createBroadcastStatusButtons(RECT rc, int xPos, int yPos);

		void clearButtons();

		void unpressAllButtons();

	private:
		
		IResourceButtonStrategy* m_resourceButtonStrategy;
		/**
		 * A collection of all buttons subscribed to receive timer notifications
		 */
		std::set<CWnd*> m_timerSubscribedButtonCollection;

		/**
		 * A collection of profile-based monitored resources
		 */
		std::vector<CResourceButton*> m_profileButtons;

		/**
		 * A collection of user-defined monitored resources
		 */
		std::vector<CResourceButton*> m_userButtons;

		std::vector<CResourceButton*> m_talkgroupButtons;
		std::vector<CResourceButton*> m_cadButtons;
		std::vector<CResourceButton*> m_broadcastButtons;

		//CResourceButton* m_radioMonitorBtn;

		BOOL m_isTimerToggleOn;

		std::map<int, RadioButtonState> m_buttonState;
		int pressingInProcessRef;

//fixed TD14302
		static const int BUTTON_SPACE_H;
		static const int BUTTON_SPACE_V;
//fixed TD14302

		static const int MAX_PROFILE_BUTTONS;
		static const int MAX_USERDEFINED_BUTTONS;
		static int BUTTON_WIDTH;
		static int BUTTON_HEIGHT;
		static unsigned long BUTTON_REFERENCE_NUM;

        RECT m_parentRect;
};
#endif // !defined(CRADIOSTATUSPAGE_H)
