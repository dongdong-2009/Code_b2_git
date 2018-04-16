/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainInformationCorbaServant_H)
#define TrainInformationCorbaServant_H

#include "bus/trains/TrainAgentCorba/idl/src/ITrainInformationCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class IOperationModeManagerServantInterface;
    class ITrainInformation;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainInformationCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:29 PM
     */
    class TrainInformationCorbaServant : public TA_Base_Core::ServantBase,
                                         public POA_TA_Base_Bus::ITrainInformationCorba
    {

    public:

        /**
         *
         * @param trainInformation    Object to delegate calls to
         * @param operationModeManager    Object to delegate calls to
         */
        TrainInformationCorbaServant( ITrainInformation& trainInformation,
                                      IOperationModeManagerServantInterface& operationModeManager );


        /**
         * Destructor
         */
        virtual ~TrainInformationCorbaServant();


        /**
         * Given a train ID, get the current details for it.
         * The train might not be valid or at a valid location, but the current known details will be given.
         *
         * @return the details of the given train
         *
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known
         *
         * @param trainId    The train to get the details for
         */
        TA_Base_Bus::ITrainInformationCorba::TrainDetails* getTrainDetails( TA_Base_Bus::ITrainCommonCorba::TrainId trainId );


        /**
         * Get details about all known valid trains.
         *
         * This will get the current details about all known trains, that are valid - meaning they are communicating either by radio, or they are present in ATS.
         *
         * @return a list of all valid trains
         */
        TA_Base_Bus::ITrainInformationCorba::TrainDetailsList* getTrainList();


        /**
         * Gets the names of all named selection lists.
         *
         * @return a sequence of names of the existing selection lists
         */
        TA_Base_Bus::ITrainInformationCorba::TrainSelectionList* getTrainSelectionList();


        /**
         * Save a selection of trains into a named selection list.
         * Creates a new list if there isnt already one by this name, otherwise overwrites any previously saved list with this name.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    The name of the selection to save
         * @param trainIDs    The list of trains included in the train selection
         * @param sessionId    The operator saving the selection list
         */
        void saveTrainSelection( const char* selectionName,
                                 const TA_Base_Bus::ITrainCommonCorba::TrainList& trainIDs,
                                 const char* sessionId );


        /**
         * Deletes an existing selection list. If none existed this has no effect.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    The named selection list to delete.
         * @param sessionId    The operator deleting the selection
         */
        void deleteTrainSelection( const char* selectionName, const char* sessionId );


        /**
         * Gets the trains involved in a single named train selection list.
         *
         * @return a list of train IDs that comprise the selection list
         *
         * @exception ITrainExceptionsCorba::InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName
         */
        TA_Base_Bus::ITrainCommonCorba::TrainList* getTrainSelection( const char* selectionName );


        /**
         * This checks the current agent state to determine its operation mode. Updates are also sent when this changes, this allows the GUI to enable/disable train lists, and locations within train lists.
         *
         * @return operation mode information about controlled locations, radio fallback, ats failure, group offline, and local duty.
         */
        TA_Base_Bus::ITrainInformationCorba::AgentOperationMode* getAgentOperationMode();


    private:


        /**
         * Private default constructor
         */
        TrainInformationCorbaServant();


        /**
         * This is used to delegate operation mode calls to
         */
        IOperationModeManagerServantInterface& m_operationModeManager;


        /**
         * Used to delegate calls to
         */
        ITrainInformation& m_trainInformation;

    };

} // TA_IRS_App

#endif // !defined(TrainInformationCorbaServant_H)
