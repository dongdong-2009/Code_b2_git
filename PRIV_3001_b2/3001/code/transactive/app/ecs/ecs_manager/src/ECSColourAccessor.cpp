/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/ECSColourAccessor.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * NOTE: TA_Base_Core::ECSManager is thread-safe.
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// identifier was truncated to '255' characters in the browser information
#endif // defined (_MSC_VER)

#include "app/ecs/ecs_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/src/ECSStationColourAccessFactory.h"
#include "core/data_access_interface/src/IECSStationColourData.h"
#include "core/exceptions/src/GenericGuiException.h"
#include "core/data_access_interface/src/DatabaseKey.h"

#include "app/ecs/ecs_manager/src/ECSColourAccessor.h"
#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // Initialise singleton and threadlock Statics
    ECSColourAccessor* ECSColourAccessor::s_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable ECSColourAccessor::s_singletonLock;

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
        TA_Base_Core::ThreadGuard guard( s_singletonLock );

        if( s_theInstance == 0 )
        {
            s_theInstance = new ECSColourAccessor();
        }

        return *s_theInstance;
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
        TA_Base_Core::ThreadGuard guard( s_singletonLock );

        if( s_theInstance != 0 )
        {
            delete s_theInstance;
            s_theInstance = 0;
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
			if(m_ecsStationColourData.size() != STATION_COLOUR_NUM)
			{
		
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
					"The number of ECS station colours retrieved from the database is incorrect. Expecting %d but retrieved %d", STATION_COLOUR_NUM, m_ecsStationColourData.size());
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


	COLORREF ECSColourAccessor::returnColourGivenStationModeType(ECurrentStationModeType currentStationModeType)
	{
		if (!(currentStationModeType >=0 && currentStationModeType <=STATION_COLOUR_NUM-1))
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "An invalid station mode has been passed in!" );
			 return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
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
			return m_ecsStationColourData[CURRENT_STATION_MODE_FAILED]->getStationColourKey();
		}
		// ++wenching (TD13587)

		//zn++ CL-20840 should use colour grey when station mode = 1, means SHUTDOWN mode
		if (stationMode == 1)
		{
			COLORREF stationColor = RGB(128, 128, 128);		// Grey Color
			return stationColor;
		}
		//++zn

		if(stationMode >0 && stationMode <= STATION_MODE_NORMAL_UPPERBOUND)
		{
			return m_ecsStationColourData[CURRENT_STATION_MODE_NORMAL]->getStationColourKey();
		}
		else if(stationMode > STATION_MODE_NORMAL_UPPERBOUND && 
			stationMode <= STATION_MODE_CONGESTION_UPPERBOUND)
		{
			return m_ecsStationColourData[CURRENT_STATION_MODE_CONGESTION]->getStationColourKey();
		}
		else if(stationMode > STATION_MODE_CONGESTION_UPPERBOUND &&
			stationMode <= STATION_MODE_EMERGENCY_UPPERBOUND)
		{
			return m_ecsStationColourData[CURRENT_STATION_MODE_EMERGENCY]->getStationColourKey();
		}
		else if ( stationMode > STATION_MODE_EMERGENCY_UPPERBOUND)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid mode value [%d], return Blue color RGB(0 , 0 , 255)", stationMode);
			return RGB(0 , 0 , 255);
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid mode value [%d], return CURRENT_STATION_MODE_WARNING color", stationMode);
			return m_ecsStationColourData[CURRENT_STATION_MODE_FAILED]->getStationColourKey();
		}
	}
	// ++marvin


} // namespace TA_IRS_App
