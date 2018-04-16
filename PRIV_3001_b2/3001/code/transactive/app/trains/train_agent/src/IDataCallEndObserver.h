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
#if !defined(IDataCallEndObserver_H)
#define IDataCallEndObserver_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * Used to handle call end events,
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:40 PM
     */
    class IDataCallEndObserver
    {

    public:

        /**
         * This is a handler for data call end events
         *
         * @param trainId    The train the transfer was for
         * @param success    Whether the transfer was successful
         * @param details    The failure reason if the transfer wasnt successful
         */
        virtual void processDataCallEndEvent( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                              bool success,
                                              const std::string& details ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IDataCallEndObserver_H)
