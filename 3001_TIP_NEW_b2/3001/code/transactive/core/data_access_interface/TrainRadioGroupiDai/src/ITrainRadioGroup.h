/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $14:14 2007-11-5$
  * Last modified by:  $Lan Yuan$
  *
  */
#if !defined(ITrainRadioGroup_H)
#define ITrainRadioGroup_H

#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include <string>
#include <set>
#include <boost/shared_ptr.hpp>


namespace TA_IRS_Core
{
    /**
     * This is the configuration for a group TSI and its coverage area used to call
     * trains.
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:15 PM
     */
    class ITrainRadioGroup : public TA_Base_Core::IItem,
                             public TA_Base_Core::IConfigItem
    {

    public:

        /**
         * typedef the set of location
         * @author Adam Radics
         * @version 1.0
         * @created 31-Aug-2007 2:18:16 PM
         */ 
         typedef std::set< unsigned long > LocationSet;


        /**
         * Gets the TSI
         * @return the TSI
         */
        virtual std::string getTsi() = 0;


        /**
         * Gets the location set
         * 
         * @return the location set
         *
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database  
         */
        virtual LocationSet getLocationCoverage() = 0;


        /**
         * Sets the train Id
         * 
         * @param tsi    The TSI
         *
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database  
         */
        virtual void setTsi(const std::string& tsi) = 0;


        /**
         * Sets the Location Coverage
         * 
         * @param locations    The locations
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database  
         */
        virtual void setLocationCoverage(const LocationSet& locations) = 0;

 
        /**
         * Add the location
         * 
         * @param location    one location
         *
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database   
         */
        virtual void addLocation(unsigned long location) = 0;

  
        /**
         * Remove the location
         * 
         * @param location    one location which will be removed
         *
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database  
        */
        virtual void removeLocation(unsigned long location) = 0;


        /**
         * Will delete this group from the database
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        virtual void deleteThisItem() = 0;

    };


    /**
     * typedef a shard pointer for ITrainRadioGroupPtr
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:16 PM
     */
    typedef boost::shared_ptr< ITrainRadioGroup > ITrainRadioGroupPtr;

}
#endif // !defined(ITrainRadioGroup_H)
