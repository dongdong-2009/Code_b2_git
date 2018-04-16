/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if !defined(TrainInformation_H)
#define TrainInformation_H

#include "app/trains/train_agent/src/ITrainInformation.h"

#include "bus/trains/TrainTransactionModel/src/ITrainStateUpdateListener.h"

//#include "bus/managed_agent/src/IOperationStateCallback.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"


namespace TA_IRS_App
{
    class IRightsChecker;
    class IMessageSender;
    class IAuditSender;
    class ITrainStatusManager;
}

namespace TA_IRS_App
{

    /**
     * This manages keeping the train selection component updated, and general train information queries.
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:48:28 PM
     */
    class TrainInformation : public ITrainInformation,
                             public TA_IRS_Bus::ITrainStateUpdateListener
    {

    public:

        /**
         *
         * @param rightsChecker
         * @param messageSender    Used to send GUI updates
         * @param auditSender    Used to submit audit messages
         * @param statusManager    The train status manager
         */
        TrainInformation( IRightsChecker& rightsChecker,
                          IMessageSender& messageSender,
                          IAuditSender& auditSender,
                          ITrainStatusManager& statusManager );


        /**
         * Destructor
         */
        virtual ~TrainInformation();


        /**
         * Get details about all known valid trains.
         *
         * This will get the current details about all known trains.
         *
         * @return a list of all trains
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getTrainList();


        /**
         * Given a train ID, get the current details for it.
         * The train might not be valid or at a valid location, but the current known details will be given.
         *
         * @return the details of the given train
         *
         * @exception InvalidTrainException if the train ID given is not known
         *
         * @param trainId    The train to get the details for
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetails( TA_IRS_Bus::CommonTypes::TrainIdType trainId );


        /**
         * Gets the names of all named selection lists.
         *
         * @return a sequence of names of the existing selection lists
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainSelectionList getTrainSelectionList();


        /**
         * Gets the trains involved in a single named train selection list.
         *
         * @return a list of train IDs that comprise the selection list
         *
         * @exception InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainSelection( const std::string& selectionName );


        /**
         * Save a selection of trains into a named selection list.
         * Creates a new list if there isnt already one by this name,
         * otherwise overwrites any previously saved list with this name.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    The name of the selection to save
         * @param trainIDs    The list of trains included in the train selection
         * @param sessionId    The operator saving the selection list
         */
        virtual void saveTrainSelection( const std::string& selectionName,
                                         const TA_IRS_Bus::CommonTypes::TrainIdList& trainIDs,
                                         const std::string& sessionId );


        /**
         * Deletes an existing selection list. If none existed this has no effect.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName    The named selection list to delete.
         * @param sessionId    The operator deleting the selection
         */
        virtual void deleteTrainSelection( const std::string& selectionName,
                                           const std::string& sessionId );


        /**
         * This handles any changes to train comms state.
         * It will send out the updates to clients.
         *
         * @param state
         */
        virtual void processTrainStateChanged( TA_IRS_Bus::TrainStatePtr state );


        /**
         * The operation mode of the agent has changed.
         *
         * When going to control for the first time, current state must be sent to clients
         *
         * @param operationState    The new operation mode
         */
        virtual void notifyOperationStateChange( TA_Base_Core::EOperationMode operationState );


    private:


        /**
         * Private default constructor
         */
        TrainInformation();


        /**
         * Used to check access rights for user operations
         */
        IRightsChecker& m_rightsChecker;


        /**
         * Used to send client updates
         */
        IMessageSender& m_messageSender;


        /**
         * Used to send audit messages for operator actions
         */
        IAuditSender& m_auditSender;


        /**
         * This is used to retrieve and store information about trains and train selection lists
         */
        ITrainStatusManager& m_statusManager;

    };

} // TA_IRS_App

#endif // !defined(TrainInformation_H)
