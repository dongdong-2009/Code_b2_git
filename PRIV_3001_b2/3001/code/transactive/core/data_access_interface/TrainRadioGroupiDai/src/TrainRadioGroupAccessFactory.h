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
#if !defined(TrainRadioGroupAccessFactory_H)
#define TrainRadioGroupAccessFactory_H

#include "core/data_access_interface/TrainRadioGroupiDai/src/ITrainRadioGroup.h"

#include <vector>

namespace TA_IRS_Core
{
    /**
     * This is used to load and create Train Radio Groups from the database
     * @author Adam Radics
     * @version 1.0
     * @created 31-Aug-2007 2:18:18 PM
     */
    class TrainRadioGroupAccessFactory
    {
    public:
        
        
        /**
         * This will load all Train Radio Group configuration from the database
         * 
         * @return a list of all train radio groups
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         */
        static std::vector< ITrainRadioGroupPtr > loadAllTrainRadioGroups();

        
        /**
         * This will load a single Train Radio Group configuration from the database
         * 
         * @return a train radio group if there is any matching (exception otherwise)
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param pkey    The group PKEY to load
         */
        static ITrainRadioGroupPtr loadTrainRadioGroup( unsigned long pkey );

        
        /**
         * This will load a single Train Radio Group configuration from the database
         * 
         * @return a train radio group if there is any matching (exception otherwise)
         * 
         * @exception DataException if there was a problem with the data
         * @exception DataBaseException if there was a problem connecting to the database
         * 
         * @param groupTsi    The group TSI to load
         */
        static ITrainRadioGroupPtr loadTrainRadioGroup( const std::string& groupTsi );

        
        /**
         * This will create a new, blank train radio group configuration object.
         * 
         * @return a new object conforming to the ITrainRadioGroup interface
         */
        static ITrainRadioGroupPtr createNewTrainRadioGroup();
        

        /**
         * This will create a new train radio group configuration object as a copy of an existing one
         * 
         * @return a new object conforming to the ITrainRadioGroup interface
         *
         * @exception DataException if there was a problem with the data
         */
        static ITrainRadioGroupPtr copyTrainRadioGroup( ITrainRadioGroupPtr trainRadioGroup );
        
    private:
            
        TrainRadioGroupAccessFactory();
        virtual ~TrainRadioGroupAccessFactory();

    };

}
#endif // !defined(TrainRadioGroupAccessFactory_H)
