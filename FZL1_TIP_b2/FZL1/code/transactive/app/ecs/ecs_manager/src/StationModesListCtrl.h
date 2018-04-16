#if !defined(AFX_STATIONMODESLISTCTRL_H__0BB0B00F_AF06_4DB6_B6AD_FC6496FEDAB2__INCLUDED_)
#define AFX_STATIONMODESLISTCTRL_H__0BB0B00F_AF06_4DB6_B6AD_FC6496FEDAB2__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/StationModesListCtrl.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Specialised derivation of CListCtrl to display list of available Station Modes along with thier current
  * proposal state.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#include <algorithm>

namespace TA_IRS_App
{
    class StationModesListCtrl : public CListCtrl
    {

    public:

        /** 
          * Constructor
          */
        StationModesListCtrl();


        /** 
          * Destructor
          */
        virtual ~StationModesListCtrl();


        /** 
          * populateStationModeDetails
          *
          * Populate list ctrl from vector of Station Mode Details.
          *
          * @param stationModeDetails - Vector of Station Mode Details to populate.
          */
        void populateStationModeDetails( const TA_IRS_App::TA_ECSManagerCommonDefs::StationModeDetails& stationModeDetails );

    
        /** 
          * setStationModeProposalState
          *
          * Set display of proposal state for particular Station Mode.
          *
          * @param stationModeIndex - Index of Station Mode to set display of proposal mode for (index relative to vector of Station Mode Details
          *                           passed using setStationModeDetails).
          * @param proposalState    - Enum corresponding to proposal state to set display of Station Mode to.
          */
        void setStationModeProposalState( int stationModeIndex, TA_IRS_App::TA_ECSManagerCommonDefs::EModeProposalState stationModeProposalState );

    
        /** 
          * getStationModeProposalState
          *
          * Get proposal state of particular Station Mode.
          *
          * @param stationModeIndex - Index of Station Mode to get proposal state of. (index relative to vector of Station Mode Details
          *                           passed using setStationModeDetails).
          *
          * @return Enum corresponding to cuurent proposal state of specified Station Mode.
          */
        TA_IRS_App::TA_ECSManagerCommonDefs::EModeProposalState getStationModeProposalState( int stationModeIndex );

		void setCurrentSelection(int index);

		int getCurrentSelection();

		void setCurrentAppliedMode( int index, bool isOverriding);

    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(StationModesListCtrl)
        //}}AFX_VIRTUAL


        // Generated message map functions
    protected:
        //{{AFX_MSG(StationModesListCtrl)
        afx_msg void onCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnTimer(UINT nIDEvent);
        //}}AFX_MSG

        DECLARE_MESSAGE_MAP()


    private:
        std::vector< int > m_blinkingItemIndexes;

        bool m_isFlashing;
        bool m_flashToggle;
		int m_currentSelectionIndex;
		int m_currentAppliedIndex;
    };
} // namespace TA_IRS_App

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATIONMODESLISTCTRL_H__0BB0B00F_AF06_4DB6_B6AD_FC6496FEDAB2__INCLUDED_)
