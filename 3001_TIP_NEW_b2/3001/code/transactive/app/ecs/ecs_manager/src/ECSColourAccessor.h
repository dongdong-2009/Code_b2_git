#ifndef ECSCOLOURACCESSOR_H
#define ECSCOLOURACCESSOR_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSColourAccessor.h $
  * @author:  Andy Siow
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/06/06 09:00:06 $
  * Last modified by:  $Author: qi.huang $
  *
  * Singleton to manage Access to the ECS Colours.
  *
  * NOTE: TA_Base_Core::ECSManager is thread-safe.
  *
  */

//  -Prasanna- Prevent Warning C4786 being generated.
#pragma warning(disable: 4786) 
//  -Prasanna- 

#include "app\ecs\ecs_manager\src\ECSManagerCommonDefs.h"
#include <string>

namespace TA_Base_Core
{
	class ReEntrantThreadLockable;
	class IECSStationColourData;
    class ECSManager;
}

namespace TA_IRS_App
{
    class ECSColourAccessor
    {
    public:
        virtual ~ECSColourAccessor();

   
        /** 
          * getInstance
          *
          * Return reference to this singleton.
          *
          * @return reference to this singleton.
          */
        static ECSColourAccessor& getInstance();


       /**
         * removeInstance
         *
         * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         */
        static void removeInstance();

		unsigned long returnColourGivenStationModeType(ECurrentStationModeType currentStationModeType);

		// marvin++
		//zn++ CL-20840
		unsigned long returnColourGivenStationMode(int stationMode);
		//COLORREF returnColourGivenStationMode(unsigned int stationMode);
		//++zn
		// ++marvin

    private:
        /** 
          * Default Constructor
          *
          * Constructor is private so can control number of instances
          */
        ECSColourAccessor();


        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ECSColourAccessor( const ECSColourAccessor& );


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ECSColourAccessor& operator=( const ECSColourAccessor& );


    private:

        // pointer to the single instance of this class
        static ECSColourAccessor* s_theInstance;

        // Semaphore to thread guard this singleton
        static TA_Base_Core::ReEntrantThreadLockable s_singletonLock;

		std::vector<TA_Base_Core::IECSStationColourData*> m_ecsStationColourData;
    };
} // namespace TA_IRS_App

#endif // ECSCOLOURACCESSOR_H
