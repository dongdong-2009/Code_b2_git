#ifndef STISTIMESCHEDULEACCESSFACTORY_H
#define STISTIMESCHEDULEACCESSFACTORY_H 

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/core/data_access_interface/tis_agent/src/StisTimeScheduleAccessFactory.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  * 
  * CameraPresetsAccessFactory is a singleton that is used to generate and retrieve 
  * ICameraPresets objects.
  */

#include <string>
#include <vector>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleData.h"

namespace TA_IRS_Core
{
    
    class StisTimeScheduleAccessFactory
    {
    public:


        virtual ~StisTimeScheduleAccessFactory() {};

		static StisTimeScheduleAccessFactory& getInstance();
		static void removeInstance();

		std::vector<StisTimeScheduleData*> getStisTimeSchedules();

		StisTimeScheduleData* getStisTimeSchedule( unsigned long key );

    private:
        //
        // This class is a singleton and so the constructor, copy constructor and
        // assignment operator are all made private.
        //
        StisTimeScheduleAccessFactory() {};
        StisTimeScheduleAccessFactory& operator=(const StisTimeScheduleAccessFactory &) {};
        StisTimeScheduleAccessFactory( const StisTimeScheduleAccessFactory& ) {};
	    std::vector<StisTimeScheduleData*> getStisTimeSchedulesWhere( std::string whereSQL = "" );

		static StisTimeScheduleAccessFactory* m_theClassInstance;
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

    };

} // end namespace TA_IRS_Core

#endif
