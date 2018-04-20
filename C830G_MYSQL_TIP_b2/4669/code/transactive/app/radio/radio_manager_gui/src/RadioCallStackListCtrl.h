/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4669/transactive/app/radio/radio_manager_gui/src/RadioCallStackListCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/08/04 12:51:33 $
  * Last modified by:  $Author: ripple $
  *
  */

#if !defined(AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_)
#define AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/radio/RadioAgentCorbaDef_4669/IDL/src/IRadioCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainDriverCallUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"

#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/naming/src/NamedObject.h"


namespace TA_IRS_App
{
    class CRadioCallStackListCtrl :
                public TA_Base_Bus::ListCtrlSelNoFocus,
                public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >,
                public TA_IRS_Bus::ITrainDriverCallUpdateObserver,
                public TA_IRS_Bus::ITrainInformationUpdateObserver
    {

    public:

    
        /**
         * Represents a normal radio or a train call
         */
        enum ECallSource
        {
            Radio,
            Train
        };

        /**
         * The call stack types
         */
        enum EStackType
        {
            IncomingStack,
            OthersStack,
            HeldStack
        };


        /** 
         * Constructor
         *
         * @param The type of call stack
         */
        CRadioCallStackListCtrl( EStackType stackType );

    
        /**
         * Destructor
         */
        virtual ~CRadioCallStackListCtrl();


        /**
         * Sets the local radio entity name to use to access the local radio agent.
         *
         * @param the radio entity name
         */
        void setRadioName( std::string radioName );


        /**
         * Sets whether the operator has rights to call trains
         *
         * @param canCallTrain true if the operator has rights
         */
        void setCallTrainPermitted( bool canCallTrain );


        /**
         * Sets the name of one of the columns
         *
         * @param nCol  The column index
         * @param columnName    The new name
         */
        BOOL setColumnName( int nCol, char*  columnName );

    
        /**
         * Checks if a call is selected or not
         *
         * @return true if there is a call selected
         */
        bool IsCallSelected();


        /**
         * Gets the ID of the selected call
         * Asserts if no call is selected
         *
         * @return the selected call ID
         */
        long getSelectedCallID();


        /**
         * Gets the selected call, along with the type of call it is.
         *
         * @param source    The call source return value
         */
        long getSelectedItemWithType( ECallSource& source );


        /**
         * Get a list of currently selected calls
         * Supports single or multi select list controls.
         */
        void getSelectedCallList( std::vector<long>& );


        /**
         * Select the call identified by the given call Id.
         * Clears any previous selection.
         *
         * @param callId   The call to select
         *
         * @return true if the call was present and is now selected.
         */
        bool selectCall( long callId );


        /**
         * deselectAll
         *
         * Deselect any selected calls
         *
         */
        void deselectAll();


        /**
         * This will receive a OccCallRequest message from the local train agent.
         *
         * @param event The event to process
         */
        virtual void processOccCallRequest( const TA_IRS_Bus::CallTypes::OccCallRequest& event );


        /**
         * This will receive a OccCallReset message from the local train agent.
         *
         * @param event The event to process
         */
        virtual void processOccCallReset( const TA_IRS_Bus::CallTypes::OccCallReset& event );


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         *
         * @param event    The event to process
         */
        virtual void processAgentOperationMode( const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event );


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
         * Receives call updates from the radio agent
         *
         * @param message the corba message containing the update
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message );


		/**
		 * indicator that radio manager is closing and call stack should not process message from train agent
		 *
		 * @param mode true if it is closing
		 */
		void setCloseMode(bool mode);
    

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CRadioCallStackListCtrl)
    public:
        virtual BOOL Create( LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL );

    protected:
        virtual void PreSubclassWindow();
        //}}AFX_VIRTUAL

    
    protected:


        // Generated message map functions
        //{{AFX_MSG(CRadioCallStackListCtrl)
        afx_msg LRESULT onCallNotification( WPARAM wParam, LPARAM lParam );
        afx_msg LRESULT onOccCallRequest( WPARAM wParam, LPARAM lParam );
        afx_msg LRESULT onOccCallReset( WPARAM wParam, LPARAM lParam );
        afx_msg LRESULT onTrainDetailsChanged( WPARAM wParam, LPARAM lParam );
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    private:


        /**
         * Simply creates the columns and image lists for the list control
         */
        void setupListControl();


        /**
         * Creates the train agent proxy
         */
        void createTrainProxy();


        /**
         * Starts the subscription to messages from the radio agent.
         * Also registers for updates from the train proxy.
         */
        void startSubscription();
        
        
        /**
         * Stops the subscription to messages from the radio agent.
         * Also deregisters for updates from the train proxy.
         */
        void stopSubscription();


        /**
         * Loads all calls from the radio agent
         * Loads all call requests from the train agent
         */
        void loadCalls();


        /**
         * Loads all radio calls from the radio agent
         */
        void loadRadioCalls();


        /**
         * Loads all call requests from the train agent
         */
        void loadTrainCalls();


        /**
         * Receives a new OCC Call request to add to the list
         *
         * @param occCallRequest The occ call to add
         */
        void addCall( const TA_IRS_Bus::CallTypes::OccCallRequest& occCallRequest );


        /**
         * Processes an add/update/remove for a radio call
         *
         * @param callStatus the call update
         */
        void processUpdate( TA_Base_Bus::IRadioCorbaDef::CallStatusEvent& callStatus );


        /**
         * Adds a new radio call
         *
         * @param callID the call id
         */
        void addCall( long callID );


        /**
         * Updates the status of an existing radio call
         *
         * @param callID the call id
         * @param status the call status
         */
        void updateCall( long callID, TA_Base_Bus::IRadioCorbaDef::ECallStatusType status );


        /**
         * Removes an existing radio call
         *
         * @param callID the call id
         */
        void removeCall( long callID );


        /**
         * For a radio call, gets the status to be displayed in the call stack
         *
         * @param status the call status
         *
         * @return the status text
         */
        std::string getStatusText( TA_Base_Bus::IRadioCorbaDef::ECallStatusType status ) const;

    
    private:


        /**
         * This is the definition of the radio agent object
         */
        typedef TA_Base_Core::NamedObject < TA_Base_Bus::IRadioCorbaDef,
                                            TA_Base_Bus::IRadioCorbaDef_ptr,
                                            TA_Base_Bus::IRadioCorbaDef_var > RadioAgentNamedObject;


        /**
         * Whether this object is subscribed for updates
         */
        bool m_subscribed;


        /**
         * The pretty little images to put next to items
         */
        CImageList m_ImageList;


        /**
         * The type of call stack
         */
        EStackType m_stackType;


        /**
         * Whether the operator can call trains
         */
        bool m_canCallTrain;


        /**
         * The local radio agent entity key
         */
        unsigned long m_radioEntityKey;


        /**
         * The local radio agent location
         */
        unsigned long m_radioEntityLocation;


        /**
         * The operator's console ID
         */
        unsigned long m_consoleId;


        /**
         * Access to the local radio agent
         */
        RadioAgentNamedObject m_radioAgent;


        /**
         * Access to the train agent
         */
        TA_IRS_Bus::TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;
        TA_IRS_Bus::TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;

		/**
	      use for radio manager in closing mode
		  */
		bool m_closeMode;

    };

} // TA_IRS_Bus

#endif // !defined(AFX_RADIOCALLSTACKLISTCTRL_H__C9F433A1_4D6B_4B21_89E2_27CB6627DA89__INCLUDED_)
