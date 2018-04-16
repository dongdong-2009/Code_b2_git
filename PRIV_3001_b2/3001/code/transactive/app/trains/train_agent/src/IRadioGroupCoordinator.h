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
#if !defined(IRadioGroupCoordinator_H)
#define IRadioGroupCoordinator_H

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:43 PM
     */
    class IRadioGroupCoordinator
    {

    public:

        /**
         * Gets the current radio group list
         *
         * @return all radio groups
         */
        virtual TA_IRS_Bus::RadioGroupTypes::TrainRadioGroupMap getRadioGroups() = 0;


        /**
         * Gets the current radio group list
         *
         * @return the matching radio group
         *
         * @exception InvalidParameterException if the group tsi doesnt exist
         *
         * @param groupTsi    The group
         */
        virtual TA_IRS_Bus::RadioGroupTypes::TrainRadioGroup getRadioGroup( const std::string& groupTsi ) = 0;


        /**
         * This will attempt to lock a radio group for use.
         * It will ensure that it is not already locked (either directly or indirectly).
         * It will then lock it, and will notify all other agents and applications of the new status.
         *
         * @exception GroupInUseException if the group is already locked
         *
         * @param groupTsi    The group to attempt to lock
         */
        virtual void lockRadioGroup( const std::string& groupTsi ) = 0;


        /**
         * This will unlock a radio group that was previously locked.
         * It will notify all other agents and applications of the new status.
         *
         * @param groupTsi    The group to attempt to lock
         */
        virtual void unlockRadioGroup( const std::string& groupTsi ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IRadioGroupCoordinator_H)
