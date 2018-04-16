#pragma once

//#include "bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h"
#include "TisDialogCtrl.h"
#include "PIDSPredefinedMessagePage.h"
#include "PIDSAdhocMessagePage.h"

namespace TA_IRS_App
{
    class PIDSMessageTabCtrl : public CTisDialogCtrl 
    {
    public:
        PIDSMessageTabCtrl();   // standard constructor
        virtual ~PIDSMessageTabCtrl();

        void Init();

        PIDSPredefinedMessagePage& getPredefinedPage();

        PIDSAdhocMessagePage& getAdhocPage();
        void setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener );

    private:
        PIDSPredefinedMessagePage   m_predefPage;
        PIDSAdhocMessagePage		m_adhocPage;


    protected:
        // {{AFX_MSG(CTrainDisplayTab)
        afx_msg BOOL OnSelchange( NMHDR* pNMHDR, LRESULT* pResult );
        DECLARE_MESSAGE_MAP()
    };
}