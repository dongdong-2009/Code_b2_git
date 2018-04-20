/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainSelectFreeTextPage.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The free text page that resides on the train display tab.
  *
  */

#if !defined(AFX_TRAINSELECTFREETEXTPAGE_H__1F54C806_4BCC_421E_B9CA_05B43338F2F9__INCLUDED_)
#define AFX_TRAINSELECTFREETEXTPAGE_H__1F54C806_4BCC_421E_B9CA_05B43338F2F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValidatingEdit.h"

#include "app/signs/ttis_manager/src/resource.h"//limin
#include "app/signs/ttis_manager/src/IMessageSelectionListener.h"

#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <string>

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
}

namespace TA_IRS_App
{

    class CTrainSelectFreeTextPage : public CDialog
    {

    public:
        struct AdHocMessage
        {
            std::string messageContent;
            TA_Base_Core::ELEDFontSize fontSize;
            TA_Base_Core::EJustification justification;
            TA_Base_Core::ETTISLEDIntensity intensity;
            TA_Base_Core::EDisplayMode displayMode;
        };

    // Construction
    public:
	    CTrainSelectFreeTextPage(CWnd* pParent = NULL);   // standard constructor

		void Init();

        // Dialog Data
	    //{{AFX_DATA(CTrainSelectFreeTextPage)
	    enum { IDD = IDD_TRAIN_SELECT_FREETEXT };
	    CListBox	m_freeTextList;
	    ValidatingEdit	m_messageContent;
	    CComboBox	m_displayMode;
	    CComboBox	m_fontSize;
	    CComboBox	m_justification;
	    CComboBox	m_intensity;
    	//}}AFX_DATA



        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTrainSelectFreeTextPage)
	    public:
	    virtual BOOL PreTranslateMessage(MSG* pMsg);
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

	    void SetDefaults();

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CTrainSelectFreeTextPage)
	    afx_msg void OnSpellCheck();
	    virtual BOOL OnInitDialog();
	    afx_msg void OnSelchangeDisplayMode();
	    afx_msg void onLoadFreetext();
	    afx_msg void onSaveFreetext();
	    afx_msg void onSelchangeFreetextList();
	    afx_msg void onChangeTrainFreeText();
		// TD17778
	    afx_msg void OnDestroy();
	    afx_msg LRESULT OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam);
		// TD17778
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

        /**
         * OnOK
         *
         * This method has been implemented to hide accidental calls when
         * the Enter key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnOK();

        /**
         * OnCancel
         *
         * This method has been implemented to hide accidental calls when
         * the ESC key is pressed. It can be overriden with new behaviour if required.
         */
        virtual afx_msg void OnCancel();

    public:

        /** 
          * hasValidMessage
          *
          * test if a message has been entered.
          *
          * @return true if a valid message has been entered.
          */
        bool hasValidMessage() const;

        /** 
          * getMessage
          *
          * Returns the message data set on this page.
          * This can only be called when a valid message
          * is entered, as determined by hasValidMessage
          * @see hasValidMessage
          *
          * @return A structure with the message data
          */
        AdHocMessage getMessage() const;


        /** 
          * windowShown
          *
          * The owning tab calls this when the window is shown.
          * OnShowWindow is not used because the owning tab has not finished
          * switching over when it is called here.
          */
        void windowShown();

		//TD 15349
		//zhou yuan++
		bool findAndSelectStationMessage(const std::string& messageName);
		void setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener);
		//++zhou yuan

    private:


        /** 
          * notifyParentWindowOfMessage
          *
          * Tells the display window whether a message is selected or not.
          */
        void notifyParentWindowOfMessage();

        // setters and getters for display attributes

        TA_Base_Core::ELEDFontSize getFontSize() const;
        void setFontSize(TA_Base_Core::ELEDFontSize fontSize);


        TA_Base_Core::EJustification getJustification() const;
        void setJustification(TA_Base_Core::EJustification justification);


        TA_Base_Core::ETTISLEDIntensity getIntensity() const;
        void setIntensity(TA_Base_Core::ETTISLEDIntensity intensity);


        TA_Base_Core::EDisplayMode getDisplayMode() const;
        void setDisplayMode(TA_Base_Core::EDisplayMode displayMode);

        //TD 15349
        //zhou yuan++
        bool findAndSelectMessageNameInList(CListBox& list, const std::string& messageName);

        IMessageSelectionListener* m_messageSelectionListener;
        //++zhou yuan


        // used to store and retrieve saved free text messages
        TA_Base_Bus::SettingsMgr m_userSettingsManager;

        TTISPredefinedMessages* m_ttisPredefinedMessages;

        // these are the user setting names for the free text messages
        static const char* FREE_TEXT_1_NAME;
        static const char* FREE_TEXT_2_NAME;
        static const char* FREE_TEXT_3_NAME;
        static const char* FREE_TEXT_4_NAME;
        static const char* FREE_TEXT_5_NAME;
        static const char* FREE_TEXT_6_NAME;

    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINSELECTFREETEXTPAGE_H__1F54C806_4BCC_421E_B9CA_05B43338F2F9__INCLUDED_)
