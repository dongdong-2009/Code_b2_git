/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/FreeTextPage.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The free text message selection page
  *
  */

#if !defined(AFX_FREETEXTPAGE_H__6CDECE25_AE40_43DD_885C_9D547009EE0E__INCLUDED_)
#define AFX_FREETEXTPAGE_H__6CDECE25_AE40_43DD_885C_9D547009EE0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IMessageSelectionListener.h"
#include "ITimeTypeListener.h"
#include "ValidatingEdit.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"
//#include "bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "DisplayAttributesManager.h"
#include <vector>
#include "TisDialogCtrl.h"

using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310

	class HandledDlg: public CDialog
	{
	public:
		explicit HandledDlg(UINT nIDTemplate, CWnd* pParentWnd);
		void setHandleTab(CTisDialogCtrl* tab);
		CTisDialogCtrl* getHandleTab();
		UINT getResourceId();
	private:
		CTisDialogCtrl* m_tab;
		UINT m_resourceId;
	};

    struct FreeTextMessage
    {
        std::string messageContent;
		TA_Base_Core::DisplayAttributes displayAttributes;
		TA_Base_Core::LCDAttributesSTIS lcdAttributes;
		TA_Base_Core::LEDAttributes ledAttributes;
    };


    class CFreeTextPage : public HandledDlg
    {
    public:

	    CFreeTextPage(CWnd* pParent = NULL);   // standard constructor

		~CFreeTextPage();

		void init();
        /** 
          * getMessage
          *
          * Gets all attributes from the page
          *
          * @return 
          */
	    FreeTextMessage getMessage() const;

        /** 
          * windowShown
          *
          * This tab has just been switched to
          */
        void windowShown();

        /** 
          * setMessageSelectionListener
          *
          * Sets the object to notify of selection changes
          *
          * @param messageSelectionListener
          */
        void setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener);


		bool findAndSelectStationMessage(const std::string& messageName);
		DisplayAttributesManager* getDisplayAttributesManager();

    protected:

        // Dialog Data
	    //{{AFX_DATA(CFreeTextPage)
	    enum { IDD = IDD_FREE_TEXT };
	    CListBox	m_freeTextList;
	    CComboBox	m_lcdFontColour;
		CComboBox   m_lcdFontSize;
	    CDateTimeCtrl	m_displayTime;
	    CComboBox	m_LEDFontSize;
		CComboBox	m_LEDFontColor;
	    ValidatingEdit m_freeTextContent;
	    ColourCombo	m_scrollSpeed;
	    ColourCombo	m_justification;
	    CallBackComboBox m_displayMode;
		CStatic m_displayTimeLabel;
		CStatic m_justificationLabel;
		CStatic m_scrollSpeedLabel;
	    //}}AFX_DATA

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CFreeTextPage)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


	    // Generated message map functions
	    //{{AFX_MSG(CFreeTextPage)
	    virtual BOOL OnInitDialog();
	    afx_msg void onChangeFreeText();
        afx_msg LRESULT OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnDestroy();
	    afx_msg void OnSave();
	    afx_msg void OnSelchangeFreetextList();
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

    private:

		bool findAndSelectMessageNameInList(CListBox& list, const std::string messageName);

        // setters and getters for display attributes

        TA_Base_Core::DisplayAttributes getDisplayAttributes() const;
		TA_Base_Core::LCDAttributesSTIS getLcdAttributes() const;
		TA_Base_Core::LEDAttributes getLedAttributes() const;
        void setDisplayAttributes(const TA_Base_Core::DisplayAttributes& displayAttributes);
		void setLedAttributes(const TA_Base_Core::LEDAttributes& ledAttributes);
		void setLcdAttributes(const TA_Base_Core::LCDAttributesSTIS& lcdAttributes);


        /** 
          * updateDisplayTimeState
          *
          * Checks if the display time control should be enabled or disabled, and
          * does so as appropriate.
          */

        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener;

        // default adhoc attributes
		TA_Base_Core::DisplayAttributes m_defaultDisplayAttributes;
		TA_Base_Core::LCDAttributesSTIS m_defaultLcdAttributes;
		TA_Base_Core::LEDAttributes m_defaultLedAttributes;

        // true if a valid message exists
        bool m_validMessage;

        // used to store and retrieve saved free text messages
        TA_Base_Bus::SettingsMgr m_userSettingsManager;

        // these are the user setting names for the free text messages
        static const char* FREE_TEXT_1_NAME;
        static const char* FREE_TEXT_2_NAME;
        static const char* FREE_TEXT_3_NAME;
        static const char* FREE_TEXT_4_NAME;
        static const char* FREE_TEXT_5_NAME;
        static const char* FREE_TEXT_6_NAME;

        STISPredefinedMessages* m_stisPredefinedMessages;
		DisplayAttributesManager* m_displayAttributesManager;

		bool m_isNonScrollApplicable;
		static const int MAX_FREE_TEXT_CHARACTERS;
		static const CString VALID_CHARACTERS;

    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_FREETEXTPAGE_H__6CDECE25_AE40_43DD_885C_9D547009EE0E__INCLUDED_)
