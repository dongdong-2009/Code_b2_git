#ifndef TRAIN_TIME_SCHEDULE_ACCESS_FACTORY_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_ACCESS_FACTORY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * CameraPresetsAccessFactory is a singleton that is used to generate and retrieve 
  * ICameraPresets objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_Core
{
   class ITrainTimeSchedule;    // Forward declaration.
}

namespace TA_IRS_Core
{
    
    class TrainTimeScheduleAccessFactory
    {
    public:

        /**
         * ~TrainTimeScheduleAccessFactory
         *
         * Standard destructor.
         */

        virtual ~TrainTimeScheduleAccessFactory() {};

        /**
         * getInstance
         *
         * Creates and returns an instance of this object.
         *
         * @return      TrainTimeScheduleAccessFactory&
         *              A reference to an instance of a TrainTimeScheduleAccessFactory object.
         */

		static TrainTimeScheduleAccessFactory& getInstance();


        /**
         * getTrainTimeSchedules
         *
         * Retrieves all train time schedules in the ti_train_time_schedule table
         *
         * @return      std::vector<ITrainTimeSchedule*>
         *              A vector of pointers to objects conforming to the ITrainTimeSchedule interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		std::vector<ITrainTimeSchedule*> getTrainTimeSchedules();

	   /**
         * getTrainTimeSchedule
         *
         * Retrieves the train time schedules in the ti_train_time_schedule table
		 * with the specified key
         *
         * @return      std::vector<ITrainTimeSchedule*>
         *              A pointers to an object conforming to the ITrainTimeSchedule interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

		ITrainTimeSchedule* getTrainTimeSchedule( unsigned long key );
        /**
         * createNewTrainTimeSchedule
         *
         * Creates a new train time schedule for saving into the ti_train_time_schedule table
         *
         * @return      ITrainTimeSchedule*
         *              A pointers to an object conforming to the ITrainTimeSchedule interface.
         *              NOTE: The caller is responsible for cleaning up this pointer.
         *
         */

		ITrainTimeSchedule* createNewTrainTimeSchedule();


        /** 
          * copyTrainTimeSchedule
          *
          * creates a new train time schedule as a copy of the given schedule.
          *
          * @param originalSchedule the schedule to copy (asserts if null)
          *
          * @return a new schedule
          */
        ITrainTimeSchedule* copyTrainTimeSchedule(ITrainTimeSchedule* originalSchedule);

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        TrainTimeScheduleAccessFactory() {};
        TrainTimeScheduleAccessFactory& operator=(const TrainTimeScheduleAccessFactory &) {};
        TrainTimeScheduleAccessFactory( const TrainTimeScheduleAccessFactory& ) {};

		/** 
          * getPredefinedMessageLibrariesWhere
          *
          * gets the pre-defined message libraries based on some criteria
          *
          * @param whereSQL an SQL where clause.
          *
          * @return the pre-defined message libraries retrieved.
          *
          * @exception   TA_Base_Core::DatabaseException
          *              Thrown if:
          *              1. There is some error with the database connection
          *              2. An SQL statement failed to execute for some reason.
          * @exception   DataException
          *              Thrown if the data cannot be converted to the correct format.
          */
        std::vector<ITrainTimeSchedule*> getTrainTimeSchedulesWhere( std::string whereSQL = "" );

		static TrainTimeScheduleAccessFactory* m_theClassInstance;

		//
        // Thread lock to protect singleton creation.
        //
        
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;
    };

} // end namespace TA_IRS_Core

#endif
