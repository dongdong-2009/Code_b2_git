#pragma once
#include "resource.h"
#include "ValidatingEdit.h"
#include "TTISPredefinedMessages.h"
#include "IMessageSelectionListener.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include <string>
#include "DisplayAttributeManager.h"

namespace TA_IRS_App
{
    class IMessageSelectionListener;

    class PIDSAdhocMessagePage : public CDialog
    {

    public:

        struct AdHocMessage
        {
            std::string messageContent;
            TA_Base_Core::ETTISLEDFontSize fontSize;
            TA_Base_Core::EJustification justification;
            TA_Base_Core::ETTISLEDIntensity intensity;
            TA_Base_Core::EDisplayMode displayMode;
        };

        PIDSAdhocMessagePage( CWnd* pParent = NULL ); // standard constructor

        void Init();
        void windowShown();

        // Dialog Data
        //{{AFX_DATA(PIDSAdhocMessagePage)
        enum { IDD = IDD_DIALOG_ADHOC_MESSAGE_PAGE };
        ValidatingEdit	m_messageContent;
        CComboBox	m_displayMode;
        CComboBox	m_fontSize;
        CComboBox	m_justification;
        CComboBox	m_intensity;
        //}}AFX_DATA



        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PIDSAdhocMessagePage)
        virtual BOOL PreTranslateMessage( MSG* pMsg );
    protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //}}AFX_VIRTUAL
        void SetDefaults();
    protected:
        // Generated message map functions
        //{{AFX_MSG(PIDSAdhocMessagePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnChangeTrainFreeText();
		afx_msg void OnDisplayModeChange();
        afx_msg void OnDestroy();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


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

        void setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener );

    private:

        struct DefaultDisplayAttributes
        {
            TA_Base_Core::EDisplayMode displayMode;
            TA_Base_Core::ETTISLEDFontSize latinFontSize;
            TA_Base_Core::EJustification justification;
            TA_Base_Core::ETTISLEDIntensity intensity;

            DefaultDisplayAttributes()
                : displayMode( TA_Base_Core::SCROLL_LEFT ),
                  latinFontSize( TA_Base_Core::TTIS_FONT_SIZE_SMALL ),
                  justification( TA_Base_Core::LEFT ),
                  intensity( TA_Base_Core::NORMAL )
            {}
        };

        void notifyParentWindowOfMessage();
        // setters and getters for display attributes

        TA_Base_Core::ETTISLEDFontSize getFontSize() const;
        void setFontSize( TA_Base_Core::ETTISLEDFontSize fontSize );


        TA_Base_Core::EJustification getJustification() const;
        void setJustification( TA_Base_Core::EJustification justification );


        TA_Base_Core::ETTISLEDIntensity getIntensity() const;
        void setIntensity( TA_Base_Core::ETTISLEDIntensity intensity );


        TA_Base_Core::EDisplayMode getDisplayMode() const;
        void setDisplayMode( TA_Base_Core::EDisplayMode displayMode );


        IMessageSelectionListener* m_messageSelectionListener;

        static const CString FREE_TEXT_VALID_CHARACTERS;
        static const UINT MAX_FREE_TEXT_CHARACTERS;
        static const DefaultDisplayAttributes m_defaultAttributes;
	public:
		typedef std::map<unsigned int, std::string> UintToStringMap;
		static UintToStringMap m_fontSizeToNameMap;
		static UintToStringMap m_displayModeToNameMap;
		static UintToStringMap m_justificationToNameMap;
		static UintToStringMap m_intensityToNameMap;
		static void initAllMaps();
    };
}