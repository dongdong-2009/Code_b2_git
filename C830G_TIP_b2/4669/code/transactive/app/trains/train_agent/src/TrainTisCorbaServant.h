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
#if !defined(TrainTisCorbaServant_H)
#define TrainTisCorbaServant_H


#include "bus/trains/TrainAgentCorba/idl/src/ITrainTisCorba.h"

#include "core/corba/src/ServantBase.h"


namespace TA_IRS_App
{
    class ITimsTisManager;
}


namespace TA_IRS_App
{

    /**
     * This is the corba servant for the ITrainTisCorba interface
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:37 PM
     */
    class TrainTisCorbaServant : public TA_Base_Core::ServantBase,
                                 public POA_TA_Base_Bus::ITrainTisCorba
    {

    public:

        /**
         *
         * @param timsTisManager    to delegate calls to
         */
        TrainTisCorbaServant( ITimsTisManager& timsTisManager );


        /**
         * Destructor
         */
        virtual ~TrainTisCorbaServant();


        /**
         * Gets the library versions for all known trains
         *
         * @return the library versions for all kown trains
         */
        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionList* getLibraryVersions();


        /**
         * Gets the library versions for the given train.
         *
         * @return the library version for the train specified
         *
         * @exception ITrainExceptionsCorba::InvalidTrainException if the train ID given is not known or not valid for this operation
         *
         * @param trainId    The train to get the library versions for
         */
        TA_Base_Bus::ITrainTisCorba::TisLibraryVersionUpdate getLibraryVersionsForTrain( TA_Base_Bus::ITrainCommonCorba::TrainId trainId );


        /**
         * Displays a pre defined text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The predefined display parameters
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void displayStoredMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                   const TA_Base_Bus::ITrainTisCorba::TtisPredefinedDisplayParameters& displayParameters,
                                   CORBA::Long timestamp,
                                   const char* sessionId );


        /**
         * Displays a free text message to the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to display the message on
         * @param displayParameters    The parameters required to display the message
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void displayFreeTextMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                                     const TA_Base_Bus::ITrainTisCorba::TtisFreeTextDisplayParameters& displayParameters,
                                     CORBA::Long timestamp,
                                     const char* sessionId );


        /**
         * Sends a reset/clear request for messages of a certain priority on the given PIDs on the given trains.
         *
         * The result will be sent asynchronously back in a TisReturnStatus message
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The list of trains to clear the message on
         * @param displayParameters    The parameters for the reset message
         * @param timestamp    The time of the request submission. This will be set along with the session ID in the result message
         * @param sessionId    The operator submitting the message
         */
        void resetMessage( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                           const TA_Base_Bus::ITrainTisCorba::TtisResetParameters& displayParameters,
                           CORBA::Long timestamp,
                           const char* sessionId );


        /**
         * This will start a predefined message library download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate the download start action, and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these messages will be sent indicating the status.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given library to
         * @param libraryData    The message library data to download to the train
         * @param libraryVersion    The library version of the library Data
         * @param sessionId    The operator initiating the download
         */
        void libraryDownload( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                              const TA_Base_Bus::ITrainTisCorba::LibraryDataType& libraryData,
                              CORBA::UShort libraryVersion,
                              const char* sessionId );


        /**
         * This will start a time schedule download to the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate the download start action, and whether it was successful or failed, and give a failure reason.
         * If the download started, once the download completes another one of these messages will be sent indicating the status.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to download the given schedule to
         * @param scheduleData    The time schedule data to download to the train
         * @param scheduleVersion    The version of the time schedule data
         * @param sessionId    The operator initiating the download
         */
        void scheduleDownload( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains,
                               const TA_Base_Bus::ITrainTisCorba::LibraryDataType& scheduleData,
                               CORBA::UShort scheduleVersion,
                               const char* sessionId );


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a library upgrade command to
         * @param sessionId    The operator initiating the upgrade
         */
        void libraryUpgrade( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains, const char* sessionId );


        /**
         * This will upgrade the predefined message library on the given trains.
         *
         * The result will be returned via a TisLibraryUpdate message. It will indicate whether the upgrade was successful or failed, and give a failure reason.
         *
         * @exception TA_Base_Core::OperationModeException If the agent is not in the control state
         * @exception TA_Base_Core::AccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         * @exception ITrainExceptionsCorba::InvalidParameterException if one of the options given for the operation is erroneous
         *
         * @param trains    The trains to send a schedule upgrade command to
         * @param sessionId    the operator initiating the upgrade
         */
        void scheduleUpgrade( const TA_Base_Bus::ITrainCommonCorba::TrainList& trains, const char* sessionId );


        /**
         * FlexibleTimeoutNamedObject calls this method to calculate the timoeut value based on the current configuration
         */
        CORBA::UShort getFlexibleTimeOutValue();


    private:


        /**
         * Private default constructor
         */
        TrainTisCorbaServant();


        /**
         * Used to delegate calls to
         */
        ITimsTisManager& m_timsTisManager;

    };

} // TA_IRS_App

#endif // !defined(TrainTisCorbaServant_H)
