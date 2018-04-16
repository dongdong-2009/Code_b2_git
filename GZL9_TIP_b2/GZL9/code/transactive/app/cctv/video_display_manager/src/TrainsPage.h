/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/TrainsPage.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/08/30 16:09:14 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#if !defined(AFX_TRAINSPAGE_H__60CE9356_F80C_401E_8B56_A328C2395A04__INCLUDED_)
#define AFX_TRAINSPAGE_H__60CE9356_F80C_401E_8B56_A328C2395A04__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainsPage.h : header file
//

#include "app/cctv/video_display_manager/src/stdafx.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"


namespace TA_IRS_Bus
{
    class TrainSelector;
}

namespace TA_IRS_App
{

    class TrainsPage : public CPropertyPage
    {
        DECLARE_DYNCREATE(TrainsPage)

    // Construction
    public:
        TrainsPage();
        ~TrainsPage();

    // void Init(REBProgressManager & mgr);
        // libo
        void Init();

        /**
         * getSelectedTrainId
         *
         * @return train ID of the selected train in the list (0 if none)
         */
        unsigned int getSelectedTrainId() const;


        /**
         * getSelectedTrainDetails
         *
         * @return train details structure of the selected train in the list
         *         (NULL if none)
         */
        VideoSourcesListCtrl::TrainDetails* getSelectedTrainDetails();

        /**
          * getTrainDetails
          *
          * @param trainId the train we want details for
          *
          * @return the train details structure if the train is in the list
          *         (NULL if none)
          */
        VideoSourcesListCtrl::TrainDetails* getTrainDetails(unsigned short trainId);


        // Dialog Data
        // {{AFX_DATA(TrainsPage)
        enum { IDD = IDD_TRAINS };
        VideoSourcesListCtrl    m_inboundTrainsListCtrl;
        VideoSourcesListCtrl    m_outboundTrainsListCtrl;
        // }}AFX_DATA


        // Overrides
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(TrainsPage)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(TrainsPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnDestroy();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT onUpdateInbound(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onUpdateOutbound(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onUpdateSelection(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onStageUpdated(WPARAM wParam, LPARAM lParam);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:
        void setOffset();

        bool m_isOffsetInitialised;
        int m_bottomOffset;

        TA_IRS_Bus::TrainSelector* m_trainSelector;
        TA_IRS_Bus::MFCTrainSelectorGUI* m_mfcTrainSelectorGUI;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINSPAGE_H__60CE9356_F80C_401E_8B56_A328C2395A04__INCLUDED_)
