/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  Adam Radics
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */
#if !defined(ITrainInformation_H)
#define ITrainInformation_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:52 PM
     */
    class ITrainInformation
    {

    public:

        /**
         * Get details about all known trains.
         *
         * This will get the current details about all known trains.
         *
         * @return a list of all valid trains
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetailsList getTrainList() = 0;


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
        virtual TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetails( TA_IRS_Bus::CommonTypes::TrainIdType trainId ) = 0;


        /**
         * Gets the names of all named selection lists.
         *
         * @return a sequence of names of the existing selection lists
         */
        virtual TA_IRS_Bus::TrainInformationTypes::TrainSelectionList getTrainSelectionList() = 0;


        /**
         * Gets the trains involved in a single named train selection list.
         *
         * @return a list of train IDs that comprise the selection list
         *
         * @exception InvalidSelectionListException if the given train list is not valid
         *
         * @param selectionName
         */
        virtual TA_IRS_Bus::CommonTypes::TrainIdList getTrainSelection( const std::string& selectionName ) = 0;


        /**
         * Save a selection of trains into a named selection list.
         * Creates a new list if there isnt already one by this name, otherwise overwrites any previously saved list with this name.
         *
         * @exception TrainAccessDeniedException If the operator does not have the correct rights or duty to perform this operation
         *
         * @param selectionName    The name of the selection to save
         * @param trainIDs    The list of trains included in the train selection
         * @param sessionId    The operator saving the selection list
         */
        virtual void saveTrainSelection( const std::string& selectionName,
                                         const TA_IRS_Bus::CommonTypes::TrainIdList& trainIDs,
                                         const std::string& sessionId ) = 0;


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
                                           const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(ITrainInformation_H)
