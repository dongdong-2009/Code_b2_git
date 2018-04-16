/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/DisplayRequestManager.h $
  * @author:   Adam Radics
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Manages the display and clear buttons.
  * Submits requests when pressed, and manages enabling/disabling.
  */


#if !defined(DisplayRequestManager_H)
#define DisplayRequestManager_H

#include "app/signs/ttis_manager/src/DisplayAttributeManager.h"
#include "app/signs/ttis_manager/src/DisplayRequestStatusDialog.h"
#include "app/signs/ttis_manager/src/PidSelectionManager.h"
#include "app/signs/ttis_manager/src/TrainSelectFreeTextPage.h"

#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/BackwardsCompatibility.h"

namespace TA_IRS_App
{
    class TTISPredefinedMessages;
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


        /**
         * constructor
         *
         * @param clearAllMessages    A clear button
         * @param clearEmergencyMessages    A clear button
         * @param clearNormalMessages    A clear button
         * @param clearPriority6    A clear button
         * @param display    The display button
         */
        DisplayRequestManager( CButton& clearAllMessages,
                               CButton& clearEmergencyMessages,
                               CButton& clearNormalMessages,
                               CButton& clearPriority6,
                               CButton& display );


        /**
         * Destructor
         */
        ~DisplayRequestManager();


        /**
         * Enable the clear and display buttons based on what is selected.
         *
         * @param trainsSelected    Whether any trains are selected
         * @param pidsSelected    Whether any PIDs are selected
         * @param messageSelected    Whether a message is selected
         * @param hasRightsToDisplay    Whether the operator has rights to send display requests
         * @param hasRightsToClear    Whether the operator has rights to send clear requests
         */
        void enableButtons( bool trainsSelected,
                            bool pidsSelected,
                            bool messageSelected,
                            bool hasRightsToDisplay,
                            bool hasRightsToClear );


        /**
         * Called when the display button is pressed.
         * Willl collect all required information from the GUI and pass the display request on to the operation that makes the call.
         *
         * @param pidSelectionManager    Used to get the selected PIDs
         * @param trainSelectorGUI    Used to get the selected trains
         * @param displayAttributeManager    Used to get the display attributes
         * @param selectedMessage    The selected message (to display)
         */
        void onDisplay( const PidSelectionManager& pidSelectionManager,
                        const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                        const DisplayAttributeManager& displayAttributeManager,
                        const TA_Base_Core::PredefinedMessage& selectedMessage );


        /**
         * Called when the display button is pressed.
         * Willl collect all required information from the GUI and pass the display request on to the operation that makes the call.
         *
         * @param pidSelectionManager    Used to get the selected PIDs
         * @param trainSelectorGUI    Used to get the selected trains
         * @param displayAttributeManager    Used to get the display attributes
         * @param selectedMessage    The selected message (to display)
         */
        void onDisplay( const PidSelectionManager& pidSelectionManager,
                        const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                        const DisplayAttributeManager& displayAttributeManager,
                        const CTrainSelectFreeTextPage::AdHocMessage& selectedMessage );


        /**
         * Called when one of the clear buttons is pressed.
         *
         * @param clearType    The type of clear request
         * @param pidSelectionManager    Used to get the selected PIDs
         * @param trainSelectorGUI    Used to get the selected trains
         */
        void onClear( TA_Base_Core::ETTISClearType clearType,
                      PidSelectionManager& pidSelectionManager,
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


        /**
         * Actually submits the request and tracks the progress
         *
         * @param trainList    The list of trains
         * @param displayParameters    The parameters for the display request
         * @param timestamp    The time of the request (used for tracking)
         * @param sessionId    The session ID of the request (used for tracking)
         */
        void submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trainList,
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


        /**
         * Gets the selected PIDs and converts to the CORBA type expected by the agent
         *
         * @return the PID list suitable for the TIS agent
         *
         * @param pidSelectionManager    Used to get and process PID selection
         */
        TA_Base_Core::TrainPIDList getSelectedPids( const PidSelectionManager& pidSelectionManager );


        /**
         * Gets the start and end times to submit the request with
         *
         * @return true if the selected times are valid, false if not
         *
         * @param displayAttributeManager    Used to access the time controls
         * @param startTime    The result start time
         * @param endTime    The result end time
         */
        bool getTimes( const DisplayAttributeManager& displayAttributeManager,
                       std::string& startTime,
                       std::string& endTime );


        /**
         * Gets the priority to submit the request with
         *
         * @param displayAttributeManager
         */
        TA_Base_Core::ETTISMessagePriority getPriority( const DisplayAttributeManager& displayAttributeManager );
        /**
         * the buttons
         */
        CButton* m_clearAllMessages;


        CButton* m_clearEmergencyMessages;


        CButton* m_clearNormalMessages;


        CButton* m_clearPriority6;


        CButton* m_display;


        /**
         * This is a dialog that will display the status of ongoing display requests
         */
        DisplayRequestStatusDialog m_statusDialog;

        TTISPredefinedMessages* m_ttisPredefinedMessages;
    };
}
#endif // !defined(DisplayRequestManager_H)

