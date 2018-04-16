/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainControlDialog_H)
#define TrainControlDialog_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrainControlDialog.h : header file
//
//#pragma   comment(linker,   "/FORCE:MULTIPLE")


#include "app/trains/TrainControlManager/src/resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
//#include "bus/generic_gui/src/TransActiveDialog.h"

#include "app/trains/TrainBorneManager/ITrainBorne.h"

#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainOperationModeCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"


// forward declarations
namespace TA_Base_Bus
{
    class ItaRights;
}


namespace TA_IRS_App
{
    //take train control dialogue

	class TrainControlDialog : public TA_IRS_App::ITrainBorne,
                               public TA_IRS_Bus::ITrainInformationUpdateObserver,
                               public TA_Base_Core::RunParamUser
    {
	DECLARE_DYNAMIC(TrainControlDialog)
    public:

        /**
         * Constructor
         *
         * @param genericGUICallback The generic GUI
         * @param taRights  Used to check rights
         * @param pParent   the parent window
         */
        /*TrainControlDialog( TA_Base_Bus::IGUIAccess& genericGUICallback,
                            TA_Base_Bus::ItaRights& taRights,
                            CWnd* pParent = NULL );*/

		TrainControlDialog(CWnd* pParent = NULL );
			
		

        /**
         * Destructor
         */
        virtual ~TrainControlDialog();


        /**
         * This will do post dialog creation initialisation
         */
        void initialise();


        /**
         * Called when one of the registered runparams is updated
         *
         * @param name  The runparam that changed
         * @param value The new value
         */
        virtual void onRunParamChange( const std::string& name, const std::string& value );


        /**
         * This will receive a TrainDetails message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainDetails( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& event );


        /**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processTrainSelectionUpdate( const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& event );


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event );

		virtual bool TB_DoApply();

	//	virtual void TB_CanApply(bool bCanApplyNow);
        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TrainControlDialog)

    protected:
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation

    protected:

        // Generated message map functions
        //{{AFX_MSG(TrainControlDialog)
        afx_msg void OnClose();
        afx_msg void OnDestroy();
        afx_msg LRESULT OnUpdateTrain( WPARAM, LPARAM );
        afx_msg void OnSelectTrain();
        afx_msg void OnTakeControl();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    private:


        /**
         * creates the txo proxies
         */
        void createProxies();


        /**
         * Retrieve all the trains and information from the TrainInformationCorbaProxy
         * and store it in the m_trainDetailsStore map
         */
        void getTrainsAndDetails();


        /**
         * Checks whether the operator can take control over the train. Chechs the right
         * aca_TRAIN_MANUAL_TAKE_CONTROL
         */
        void checkRights();

        
        /**
         * enables the take control button if the user has rights and a train is selected
         */
        void enableTakeControlButton();

        
        /**
         * This get called when the user selected a train from the list. This updates the
         * details of the train
         */
        void updateTrainDetails();

		/**
		 * Used to set the Column Names of CListCtrl							       
		 */
		void columnNamesList();
		
		/**
		 * Return the trainID selected                                                                      
		 */
		int getTrainIDSelected(CString str);

    private:

        // Dialog Data
        //{{AFX_DATA(TrainControlDialog)
        enum { IDD = IDD_TRAIN_CONTROL_MANAGER_DIALOG };
        CComboBox m_trainList;
        CEdit m_secondaryTSI;
        CEdit m_primaryTSI;
        CEdit m_resultDisplay;
        CButton m_takeControl;
        CButton m_closeButton;
		CListCtrl m_listTrainCtrl;
        //}}AFX_DATA


        /**
         * to determine if a particular session is allowed to perform an action on a resource
         */
        //TA_Base_Bus::ItaRights& m_rights;


        /**
         * Whether the operator has the rigth to take control over the train
         */
        bool m_canTakeControl;


        /**
         * The local train agent entity key
         */
        unsigned long m_trainAgentKey;


        /**
         * to store the information of trains in the form of map
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap m_trainDetailsStore;


        /**
         * Used to lock the train map
         */
        TA_Base_Core::NonReEntrantThreadLockable m_trainDetailsLock;


        /**
         * Used to get information about the tains
         */
        TA_IRS_Bus::TrainInformationCorbaProxyPtr m_trainInformationProxy;


        /**
         * used to take control over the train
         */
        TA_IRS_Bus::TrainOperationModeCorbaProxyPtr m_trainOperationModeProxy;


        /**
         * Application icon
         */
        HICON m_hIcon;

		public:
			afx_msg void OnNMTrainSelected(NMHDR *pNMHDR, LRESULT *pRESULT);

        
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(TrainControlDialog_H)
