#pragma once
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "IMessageSelectionListener.h"
#include "TTISPredefinedMessages.h"
#include "resource.h"

// PIDSPredefinedMessagePage dialog
namespace TA_IRS_App
{
    class PIDSPredefinedMessagePage : public CDialog
    {
    public:
        PIDSPredefinedMessagePage( CWnd* pParent = NULL ); // standard constructor
        virtual ~PIDSPredefinedMessagePage();
        //This function called before oninitdialog
        void Init();
        void windowShown();
        TA_Base_Core::PredefinedMessage getSelectedMessage() const;
        bool hasValidSelection() const;

        void setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener );

        // Dialog Data
        enum { IDD = IDD_DIALOG_PREDFINED_MESSAGE_PAGE };
        TA_Base_Bus::AutoSortListCtrl m_messageListCtrl;
        CEdit m_filter;
        CEdit m_messageContent;


    protected:
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //afx_msg void OnClickPredefinedMsgList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnFilterChange();
        afx_msg LRESULT OnUpdateCurrentTTISVersion( WPARAM wParam, LPARAM lParam );
        afx_msg void OnItemchangedPredefined( NMHDR* pNMHDR, LRESULT* pResult );
        afx_msg void OnDestroy();

        DECLARE_MESSAGE_MAP()
    private:
        void clearFilter();
        void clearMessageList();
        void clearMessageContent();
        void populateMessageList();
        void updatePreviewArea();
        std::string getFilterString();
        const TA_Base_Core::PredefinedMessage* getSelectedMessageData() const;

    private:
        TTISPredefinedMessages* m_ttisPredefinedMessages;
        IMessageSelectionListener* m_messageSelectionListener;
        std::vector<TA_Base_Core::PredefinedMessage> m_allMessages;
		unsigned long m_currentTtisLibrary;
    };
}

