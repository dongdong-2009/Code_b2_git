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
#if !defined(IPersistedTrainState_H)
#define IPersistedTrainState_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

namespace TA_IRS_Core
{
    /**
     * Used to load persisted train state from the database
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:37 PM
     */
    class IPersistedTrainState
    {

    public:

        IPersistedTrainState(){}

        virtual ~IPersistedTrainState(){}


        /**
         * Gets the train Id
         * 
         * @return the train ID
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned char getTrainId() = 0;


        /**
         * Gets theprimary train TSI
         * 
         * @return the train TSI
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual const std::string& getPrimaryTsi() = 0;


        /**
         * Gets the secondary train TSI
         * 
         * @return the train TSI
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual const std::string& getSecondaryTsi() = 0;


        /**
         * Gets the pa library that is on the train
         * 
         * @return the version of the PA library
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        //virtual unsigned short getPaLibraryVersion() = 0;
        virtual unsigned long getPaLibraryVersion() = 0;


        /**
         * Gets the TTIS library that is on the train
         * 
         * @return the version of the TTIS library
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned long getTtisLibraryVersion() = 0;

        /**
         * Gets the TTIS schedule that is on the train
         * 
         * @return the version of the TTIS schedule
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual unsigned long getTtisScheduleVersion() = 0;

        /**
         * Gets the timestamp of this train data
         * 
         * @return the time this data was last updated
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual time_t getTimestamp() = 0;

    };

    
    /**
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:37 PM
     */
    typedef boost::shared_ptr< IPersistedTrainState > IPersistedTrainStatePtr;

    
    /**
     * Map of train ID to persisted state
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:37 PM
     */
    typedef std::map< unsigned char, IPersistedTrainStatePtr > IPersistedTrainStateMap;

}
#endif // !defined(IPersistedTrainState_H)
