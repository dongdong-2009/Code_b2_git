/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSColourAccessor.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * NOTE: TA_Base_Core::ECSManager is thread-safe.
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// identifier was truncated to '255' characters in the browser information
#endif // defined (_MSC_VER)

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "app/ecs/ecs_manager/src/ECSColourAccessor.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"
#include "core\data_access_interface\src\ECSStationColourAccessFactory.h"
#include "core\data_access_interface\src\IECSStationColourData.h"

#include "core/exceptions/src/GenericGuiException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    ECSColourAccessor* ECSColourAccessor::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable ECSColourAccessor::m_singletonLock;


    //
    // Destructor
    //
    ECSColourAccessor::~ECSColourAccessor()
    {
		for(unsigned int i=0; i<m_ecsStationColourData.size(); i++)
		{
			if(m_ecsStationColourData[i] != NULL)
			{
				delete m_ecsStationColourData[i];
				m_ecsStationColourData[i] = 0;
			}
		}

    }


    //
    // getInstance
    //
    ECSColourAccessor& ECSColourAccessor::getInstance()
    {
        //
        // Guard the method to ensure access by client code is synchronous
        //
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        if( m_theInstance == 0 )
        {
            m_theInstance = new ECSColourAccessor();
        }

        return *m_theInstance;
    }


    //
    // removeInstance
    //
    void ECSColourAccessor::removeInstance()
    {
        // 
        // Guard this to prevent multiple threads atempting
        // to delete/create simultaneously
        //
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        if( m_theInstance != 0 )
        {
            delete m_theInstance;
            m_theInstance = 0;
        }
    }


    //
    // Constructor
    //
    ECSColourAccessor::ECSColourAccessor()
    {
		try
		{
			m_ecsStationColourData = TA_Base_Core::ECSStationColourAccessFactory::getInstance().getAllECSStationColours();
			if(m_ecsStationColourData.size() != TA_ECSManagerCommonDefs::STATION_COLOUR_NUM)
			{
		
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
					"The number of ECS station colours retrieved from the database is incorrect. Expecting %d but retrieved %d", TA_ECSManagerCommonDefs::STATION_COLOUR_NUM, m_ecsStationColourData.size());
				TA_THROW( TA_Base_Core::GenericGUIException( "Unable to retrieve colour configuration for ECS. Application will exit.", TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS ) );
			}

		}
		catch(...)
		{
			for(unsigned int i=0; i<m_ecsStationColourData.size(); i++)
			{
				if(m_ecsStationColourData[i] != NULL)
				{
					delete m_ecsStationColourData[i];
					m_ecsStationColourData[i] = 0;
				}
			}
			TA_THROW( TA_Base_Core::GenericGUIException( "Unable to retrieve colour configuration for ECS. Application will exit.", TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS ) );
		}
    }


	COLORREF ECSColourAccessor::returnColourGivenStationModeType(TA_ECSManagerCommonDefs::ECurrentStationModeType currentStationModeType)
	{
		TA_ASSERT((currentStationModeType >=0 && currentStationModeType <=TA_ECSManagerCommonDefs::STATION_COLOUR_NUM-1), "An invalid station mode has been passed in!");
		// Note that this is only valid for items 0-3. AccessFactory needs to be changed otherwise.
		return m_ecsStationColourData[currentStationModeType]->getStationColourKey();
	}

	// marvin++
	//zn++ CL-20840
	COLORREF ECSColourAccessor::returnColourGivenStationMode(int stationMode)
	//COLORREF ECSColourAccessor::returnColourGivenStationMode(unsigned int stationMode)
	//++zn
	{
		// wenching++ (TD13587)
		//TA_ASSERT( stationMode >0 , "An invalid station mode has been passed in!");
		if(stationMode <= 0)
		{
			//COLORREF stationColor = RGB(255, 255, 255);
			LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Invalid given station mode, return default colour RGB(255, 255, 255)" );
			//return stationColor;
			return m_ecsStationColourData[TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_FAILED]->getStationColourKey();
		}
		// ++wenching (TD13587)

		//zn++ CL-20840 should use colour grey when station mode = 1, means SHUTDOWN mode
		if (stationMode == 1)
		{
			COLORREF stationColor = RGB(128, 128, 128);		// Grey Color
			return stationColor;
		}
		//++zn

		if(stationMode >0 && stationMode <= TA_ECSManagerCommonDefs::STATION_MODE_NORMAL_UPPERBOUND)
		{
			return m_ecsStationColourData[TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_NORMAL]->getStationColourKey();
		}
		else if(stationMode >=TA_ECSManagerCommonDefs::STATION_MODE_NORMAL_UPPERBOUND && 
			stationMode < TA_ECSManagerCommonDefs::STATION_MODE_CONGESTION_UPPERBOUND)
		{
			return m_ecsStationColourData[TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_CONGESTION]->getStationColourKey();
		}
		else
		{
			return m_ecsStationColourData[TA_ECSManagerCommonDefs::CURRENT_STATION_MODE_EMERGENCY]->getStationColourKey();
		}
	}
	// ++marvin


} // namespace TA_IRS_App
