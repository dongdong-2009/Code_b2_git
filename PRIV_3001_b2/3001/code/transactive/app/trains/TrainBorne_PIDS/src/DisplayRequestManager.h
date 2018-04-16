/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/DisplayRequestManager.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/10/31 12:15:01 $
  * Last modified by:  $Author: grace.koh $
  *
  * Manages the display and clear buttons.
  * Submits requests when pressed, and manages enabling/disabling.
  */

#pragma once

#include "DisplayAttributeManager.h"
#include "DisplayRequestStatusDialog.h"
#include "PIDSAdhocMessagePage.h"
#include "PredefinedMsgDisplayRequestDlg.h"
#include "AdhocDisplayRequestStatusDlg.h"


#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/BackwardsCompatibility.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainTisCorba.h"
#include "core/naming/src/NamedObject.h"

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
    class PIDSManagerDlg;
}

namespace TA_IRS_App
{
    /**
     * @version 1.0
     * @created 04-Apr-2008 1:48:10 PM
     */
    class DisplayRequestManager : public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
    {
    public:

        DisplayRequestManager();


        DisplayRequestManager( CButton& resetBtn,
                               PIDSManagerDlg& dlg );

        ~DisplayRequestManager();



        void enableButtons( bool trainsSelected,
                            bool messageSelected,
                            bool hasRightsToDisplay,
                            bool hasRightsToClear );



        //called when press "apply" button in trainborne manager
        void onDisplay( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                        const DisplayAttributeManager& displayAttributeManager,
                        const TA_Base_Core::PredefinedMessage& selectedMessage );



        void onDisplay( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                        const DisplayAttributeManager& displayAttributeManager,
                        const PIDSAdhocMessagePage::AdHocMessage& selectedMessage );


        void onClear( TA_Base_Core::ETTISClearType clearType,
                      TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI );


        /**
         * This will receive display request result messsages
         *
         * @param payload    The incoming message
         */
        virtual void receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload );

    private:

        /**
         * Subscribes to display request result messages from the local TIS agent
         */
        void subscribeToMessages();

        void unsubscribeToMessages();

        void initStatusDialogs();

        void deleteStatusDialogs();


        /**
         * Actually submits the request and tracks the progress
         *
         * @param trainList    The list of trains
         * @param displayParameters    The parameters for the display request
         * @param timestamp    The time of the request (used for tracking)
         * @param sessionId    The session ID of the request (used for tracking)
         */
        void submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                             const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                             const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
                                             time_t timestamp,
                                             const std::string& sessionId );


        /**
         * Actually submits the request and tracks the progress
         *
         * @param trainList    The list of trains
         * @param displayParameters    The parameters for the display request
         * @param timestamp    The time of the request (used for tracking)
         * @param sessionId    The session ID of the request (used for tracking)
         */
        void submitFreeTextDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                           const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                           const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                           time_t timestamp,
                                           const std::string& sessionId );


        /**
         * Actually submits the request and tracks the progress
         *
         * @param trainList    The list of trains
         * @param displayParameters    The parameters for the clear request
         * @param timestamp    The time of the request (used for tracking)
         * @param sessionId    The session ID of the request (used for tracking)
         */
        void submitClearRequest( const TA_Base_Core::TrainList& trainList,
                                 const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                 const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                 time_t timestamp,
                                 const std::string& sessionId );


        /**
         * Gets the selected trains and converts to the CORBA type expected by the agent
         *
         * @return the train list suitable for the TIS agent
         *
         * @param trainSelectorGUI    Used to get and process Train selection
         */
        TA_Base_Core::TrainList getSelectedTrains( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI );//limin, add const




        CButton* m_resetBtn;
        PIDSManagerDlg* m_pidsManagerDlg;


        /**
         * This is a dialog that will display the status of ongoing display requests
         */
        PredefinedMsgDisplayRequestDlg *m_predefinedMsgDisplayStatusDialog;
        AdhocDisplayRequestStatusDlg *m_adhocMsgDisplayStatusDialog;
        //DisplayRequestStatusDialog *m_clearMsgDialog;


        TTISPredefinedMessages* m_ttisPredefinedMessages;


        //used by display status dialog
        TA_Base_Core::PredefinedMessage m_currentPredefinedMessage;
        PIDSAdhocMessagePage::AdHocMessage m_currentAdhocMessage;

        bool m_subscribedToMessages;

    };
}

