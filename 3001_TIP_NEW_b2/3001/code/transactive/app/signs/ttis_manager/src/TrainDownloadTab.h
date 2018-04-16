/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TrainDownloadTab.h $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is the train download tab. It just holds the
  * schedule and library download tabs.
  *
  * It receives train list updates, and pushes these down to the list sin the tabs.
  * Along with the updates it also submits the current version of the libraries on the train.
  *
  * It also populates the train download lists, and keeps those updated in each tab.
  *
  */

#if !defined(AFX_TRAINDOWNLOADTAB_H__1DB5D490_5207_4DCA_AD5E_F350BFA06CB3__INCLUDED_)
#define AFX_TRAINDOWNLOADTAB_H__1DB5D490_5207_4DCA_AD5E_F350BFA06CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LibraryDownloadPage.h"
#include "ScheduleDownloadPage.h"
#include "TrainDownloadManager.h"
#include "TrainVersionManager.h"

#include "bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelector.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"


/////////////////////////////////////////////////////////////////////////////
// CTrainDownloadTab window

namespace TA_IRS_App
{

    class CTrainDownloadTab : public CTabDialogCtrl
    {
    
    public:
	    
        CTrainDownloadTab();

        virtual ~CTrainDownloadTab();

            void Init(REBProgressManager & mgr);

            // Overrides
            // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTrainDownloadTab)
	    protected:
	    virtual void PreSubclassWindow();
	    //}}AFX_VIRTUAL


	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CTrainDownloadTab)
		afx_msg LRESULT OnUpdateInboundList(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateOutboundList(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateSelectionCombo(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateTrainVersion(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT OnUpdateDownloadList(WPARAM wParam, LPARAM lParam);
	    afx_msg void OnDestroy();
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:

        /** 
          * updateTrainLists
          *
          * Update the train lists in the child pages.
          *
              * @param action
              * @param trainDetails
              *
              */
            void updateTrainLists( TA_IRS_Bus::ITrainSelectorGUI::EAction action,
                                   const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);


	    CLibraryDownloadPage m_libraryDownloadPage;
	    CScheduleDownloadPage m_scheduleDownloadPage;

        // this updates the versions next to each train in the list
        TrainVersionManager* m_trainVersionManager;

        // this updates the current download lists
        // and is also used to start and stop downloads
        TrainDownloadManager* m_trainDownloadManager;

         // train selection component
        TA_IRS_Bus::MFCTrainSelectorGUI* m_mfcTrainSelector;
        TA_IRS_Bus::TrainSelector* m_trainSelector;

    };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_TRAINDOWNLOADTAB_H__1DB5D490_5207_4DCA_AD5E_F350BFA06CB3__INCLUDED_)
