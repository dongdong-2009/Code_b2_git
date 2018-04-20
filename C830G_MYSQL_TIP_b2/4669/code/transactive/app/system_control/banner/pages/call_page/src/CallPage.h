/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/CallPage.h $
  * @author:  C. DeWolfe - Heavily modified by Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides a banner that lists the communications for the
  * operator.
  * The operator may find out more information by double-clicking on an entry
  * in the list.
  *
  * This class derives from BannerPage which allows it to be a page in the Banner
  * Framework.
  */

#if !defined(AFX_CallPage_H__6490520F_C023_48DC_BB94_640B67BB73ED__INCLUDED_)
#define AFX_CallPage_H__6490520F_C023_48DC_BB94_640B67BB73ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CallPage.h : header file
//

#include "app\system_control\banner\pages\call_page\src\CallBannerListCtrl.h"
#include "app\system_control\banner\pages\call_page\src\IVettingCallback.h"
#include "app\system_control\banner\pages\call_page\src\VettingManager.h"
#include "app\system_control\banner\pages\call_page\src\CallBannerPage_rc.h"
#include "app\system_control\banner\pages\call_page\src\RightsChecker.h"
#include "bus\banner_framework\src\BannerPage.h"
#include "bus\banner_pages\call_page_library\src\ICallBanner.h"
#include "bus\banner_pages\call_page_library\src\ICallEntry.h"
#include "bus\banner_pages\call_page_library\src\CallEntryManager.h"
#include "bus\mfc_extensions\src\mfc_extensions.h"
#include "core\utilities\src\RunParams.h"

namespace TA_IRS_App
{
    // Forward declaration
    class CallBannerInitialiser;

    /////////////////////////////////////////////////////////////////////////////
    // CallPage dialog

    class CallPage : 
        public TA_Base_Bus::BannerPage, 
        public TA_IRS_Bus::ICallBanner, 
        public TA_Base_Core::RunParamUser,
        public TA_Base_Bus::ISortingCallback,
        public IVettingCallback
    {

    public:

        CallPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog);

        ~CallPage();

        /**
         * onButtonDoubleClick
         *
         * We do nothing on button double click. 
         */
        virtual void onButtonDoubleClick();

        /**
         * addBannerPage
         *
         * Tells the banner page to add itself to the banner.
         */ 
        virtual void addBannerPage();

        /**
         * initialiseBanner
         *
         * Called by the CallBannerInitialiser to initialise this dialog.
         */
        void initialiseBanner();

        /**
         *  Indicates whether initialisation of the banner has completed.
         */
        bool isInitialised();

        /**
         * initialiseCallTypeFilterInfo
         *
         * Initialises the filtering information for call types.
         */
        void initialiseCallTypeFilterInfo();

        /*
         * onRunParamChange (interface implementation)
         *
         * @see RunParamUser::onRunParamChange for more details
         *
         */
    	virtual void onRunParamChange(const std::string& name, const std::string& value);

        /*********************************************************************/
        /*                           ICallBanner methods                     */
        /*********************************************************************/

	    /**
	     * Informs the Call Banner that a call has been added.
         *
         */
        virtual void addCall(const TA_IRS_Bus::ICallEntry& call);
	    
    
        /**
	     * Informs the Call Banner that a call with the given id
         * has been deleted.
	     */
	    virtual void removeCall(unsigned long callId);


        /**
         * Informs the Call Banner that it should refresh its list of
         * calls.
         */
        virtual void refreshCalls();

        /**
         * Informs the Call Banner that the fields of a call entry have
         * changed and it should reload them.
         */
        virtual void refreshCall(const TA_IRS_Bus::ICallEntry& call);

        /**
         * Informs the Call Banner that the call entry needs to be activated
         */
        virtual void activateCall(const TA_IRS_Bus::ICallEntry& call);

        /**
         * OnSetActive (interface implementation)
         * @see TStackedPage::OnSetActive for more details
         */
    	virtual void OnSetActive(void);
        
        /**
         * sortCallback (interface implementation)
         * @see ISortingCallback::sortCallback for more details
         */
        virtual int sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, TA_Base_Bus::AutoSortListCtrl::ECurrentSort currentSort);


        /** 
          * vettingStateChanged
          *
          * Indicates that the vetting state has changed.
          *
          * @param isVetting  True if vetting is now required, and false otherwise.
          */
        virtual void vettingStateChanged(bool isVetting);


    // Dialog Data
	    //{{AFX_DATA(CallPage)
	    enum { IDD = IDD_CALLS };
	    ToggleButton m_authorBtn;
	    ToggleButton m_rxhBtn;
	    ToggleButton m_radioBtn;
	    ToggleButton m_pecBtn;
	    CallBannerListCtrl m_CallListCtrl;
        TA_Base_Bus::FixedHeaderCtrl m_fixedHeaderHelper;
	    //}}AFX_DATA
        CButton m_vettingBtn;


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CallPage)
        public:
        //virtual BOOL OnKillActive();


	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CallPage)
	    virtual BOOL OnInitDialog();
        afx_msg void OnDestroy();
	    afx_msg void OnDblclkCallList(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnRightclkCallList(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnCommandHandler(UINT nID);
        afx_msg LRESULT OnFullRefreshRequest(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onAddCall(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onRemoveCall(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onRefreshCall(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onRefreshCalls(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onActivateCall(WPARAM wParam, LPARAM lParam);
        afx_msg void OnTimer(UINT nIDEvent);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         *  Gets the given call as a string.
         */
        static std::string getCallAsString(const TA_IRS_Bus::ICallEntry& call);
   

        /**
         *  Gets the location key of the call banner.
         */
        unsigned long CallPage::getLocationKey();


    private:

        /**
         * refreshCallsInternal
         *
         * Internal implementation, must be called within GUI thread
         */
        void refreshCallsInternal();

        /**
         * refreshButtonFlashingState
         *
         * Refreshes the flashing / not flashing state of the "Calls" button
         */
        void refreshButtonFlashingState();

        /**
         * addCallToListControl
         *
         * Called to directly add the call to the internal list control
         * (all rights checking etc should have been done earlier on)
         */
        void addCallToListControl(const TA_IRS_Bus::ICallEntry& call);

	    /**
	    * setButtonPushColour
	    * 
	    * Set the colour of the button backgrounds when pressed
	    *
	    * @param colour is the colour of the button backgrounds when pressed
	    *
	    */
	    void setButtonPushColour(const COLORREF colour);

   	    /**
	    * onVettingCheckClicked
	    * 
	    * Called when the vetting checkbox is clicked.
	    *
	    */
        // void onVettingCheckClicked(); //TD15223 - removed

	    /**
	    * onButtonClicked
	    * 
	    * Called when a button is clicked on
	    *
	    */
	    void onButtonClicked(UINT btnId);
        
        /**
         *  refreshAllButtonsText
         *
         *  Sets the text on all the buttons, including the number of calls of
         *  the corresponding type that exist.
         */
        void refreshAllButtonsText();

        /**
         * initialiseButtonStates
         *
         * Initialises the toggle and enabled states of each button
         */
        void initialiseButtonStates();

        /**
         * refreshItemDetails
         *
         * Refreshes the row information for the list control at the specified index
         * @param index the row index
         * @param call the data used to populate information in the row
         */
        void refreshItemDetails(int index, const TA_IRS_Bus::ICallEntry& call);


        /**
         * 4669Pre-FAT TD#13762
         *
         * refreshPECButtonFlashingState
         *
         * Refreshes the flashing / not flashing state of the "PEC" button
         */
        void refreshPECButtonFlashingState();


        // Manages the memory for the ICallEntry objects and makes callbacks on
        // this class when entries are added and removed.
        TA_IRS_Bus::CallEntryManager* m_callEntryManager;

        // Object used to keep the vetting checkbox up-to-date.
        VettingManager* m_vettingManager;

        // Handles the initialisation of the banner in a separate thread.
        CallBannerInitialiser* m_callBannerInitialiser;

        // Set to true when the dialog has finished initialising
        volatile bool m_initialised;

        unsigned long m_locationKey;

        //
        // A struct containing information about a call type.
        // The fields are
        // callType   -  a string describing the call type, eg "Authorisation"
        // isVisible  -  visibility of the call type (ie whether it is filtered
        //               out or visible)
        // hasRights  -  indicates whether the user has the right to view this
        //               type of call
        // buttonId   -  the id of the button that hides or shows call of this
        //               type
        // buttonName -  the text on the face of the button
        // colour     -  the colour of the call type text
        //
        typedef struct CallTypeInfo
        {
            std::string   callType;
            bool          isVisible;
            bool          hasRights;
            UINT          buttonId;
            std::string   buttonName;
            COLORREF      colour;
        };

        typedef std::vector<CallTypeInfo> CallTypeInfoVector;

        CallTypeInfoVector m_callTypeFilterInfo;


        // The background colour of the list view control
        static const COLORREF BACKGROUND_COLOUR;
        
        // The default text colour of call entries
        static const COLORREF DEFAULT_TEXT_COLOUR;    

        // ++
        // 4669Pre-FAT TD#13762
        static const int PEC_FLASH_TIMER_ID;        
        static const int PEC_FLASH_TIMER_INTERVAL;
        
        bool m_isPECFlashing;
        // ++

		RightsChecker* m_rightChecker;

    };
} // TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CallPage_H__6490520F_C023_48DC_BB94_640B67BB73ED__INCLUDED_)
