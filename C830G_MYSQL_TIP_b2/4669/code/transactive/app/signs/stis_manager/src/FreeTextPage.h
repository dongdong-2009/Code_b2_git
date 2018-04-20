/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/FreeTextPage.h $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 11:49:08 $
  * Last modified by:  $Author: builder $
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

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "bus/mfc_extensions/src/coloured_controls/ColourCombo.h"

#include "bus/user_settings/src/SettingsMgr.h"

#include <vector>

using TA_Base_Bus::ColourCombo;

namespace TA_IRS_App
{
    class STISPredefinedMessages;   // TD11310

    struct FreeTextMessage
    {
        std::string messageContent;
        TA_Base_Core::DisplayAttributes displayAttributes;
	    TA_Base_Core::PlasmaAttributes plasmaAttributes;
	    TA_Base_Core::LEDAttributes ledAttributes;
    };


    class CFreeTextPage : public CDialog,
                          public ITimeTypeListener
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

        /** 
          * timeTypeChanged
          *
          * Gets called when the time type is changed.
          *
          * @param newTimeType  The new time type.
          */
        virtual void timeTypeChanged(TimeControlManager::TimeType newTimeType);

		//TD 15349
		//zhou yuan++
		bool findAndSelectStationMessage(const std::string& messageName);
		//++zhou yuan

    protected:

        // Dialog Data
	    //{{AFX_DATA(CFreeTextPage)
	    enum { IDD = IDD_FREE_TEXT };
	    CListBox	m_freeTextList;
	    CComboBox	m_plasmaBackgroundColour;
	    CDateTimeCtrl	m_displayTime;
	    CComboBox	m_plasmaForegroundColour;
	    CComboBox	m_plasmaFontSize;
	    CComboBox	m_LEDFontSize;
		CComboBox	m_LEDFontColor;
	    CEdit	m_freeTextContent;
	    ColourCombo	m_scrollSpeed;
	    ColourCombo	m_justification;
	    CComboBox	m_displayMode;
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
	    afx_msg void OnSelchangeDisplayMode();
	    afx_msg void OnSpellCheck();
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

		//TD 15349
		//zhou yuan++
		bool findAndSelectMessageNameInList(CListBox& list, const std::string messageName);
		//++zhou yuan

        void setDefaults();


        // setters and getters for display attributes

        TA_Base_Core::DisplayAttributes getDisplayAttributes() const;
        void setDisplayAttributes(TA_Base_Core::DisplayAttributes displayAttributes);

        TA_Base_Core::PlasmaAttributes getPlasmaAttributes() const;
        void setPlasmaAttributes(TA_Base_Core::PlasmaAttributes plasmaAttributes);

        TA_Base_Core::LEDAttributes getLedAttributes() const;
        void setLedAttributes(TA_Base_Core::LEDAttributes ledAttributes);

        /** 
          * updateDisplayTimeState
          *
          * Checks if the display time control should be enabled or disabled, and
          * does so as appropriate.
          */
        void updateDisplayTimeState();


        // tell this object when message selection changes
        IMessageSelectionListener* m_messageSelectionListener;

        // default adhoc attributes
        TA_Base_Core::DisplayAttributes m_defaultDisplayAttributes;
	    TA_Base_Core::PlasmaAttributes m_defaultPlasmaAttributes;
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

        // stores the currently selected time type
        TimeControlManager::TimeType m_timeType;

        STISPredefinedMessages* m_stisPredefinedMessages;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_FREETEXTPAGE_H__6CDECE25_AE40_43DD_885C_9D547009EE0E__INCLUDED_)
