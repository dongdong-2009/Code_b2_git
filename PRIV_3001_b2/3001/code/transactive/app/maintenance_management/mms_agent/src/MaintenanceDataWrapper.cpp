/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
* 
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/DPScadaWorker.cpp $
* @author:   huirong.luo
* @version:  $Revision: #1 $
*
* Last modification: $DateTime: 2011/02/28 15:06:30 $
* Last modified by:  $Author: weikun.lin $
*
* 
*/
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif

#include <time.h>
#include "ace/OS.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/maintenance_management/mms_agent/src/MaintenanceDataWrapper.h"

 
using TA_Base_Core::DebugUtil;
 

namespace TA_IRS_App
{


	MaintenanceDataWrapper::MaintenanceDataWrapper()
	{
		m_data=NULL;		 
		m_latestPlanDate.dstflag=-1;
		m_latestPlanDate.timezone=-1;
		m_latestPlanDate.time=0;
		m_latestPlanDate.millitm=0;
	}

	MaintenanceDataWrapper::~MaintenanceDataWrapper()
	{
		if (m_data!=NULL)
		{
			delete m_data;
			m_data=NULL;
		}		 
	}

	MaintenanceDataWrapper::MaintenanceDataWrapper(const MaintenanceDataWrapper& other)
	{  
		m_data= new ScadaUtilities::MaintenanceData(*(other.m_data)); 
		m_latestPlanDate=other.m_latestPlanDate;
	}  

	MaintenanceDataWrapper& MaintenanceDataWrapper::operator= (const MaintenanceDataWrapper& other)  
	{  			 
		if(this == &other)  
			return *this;  
		m_data= new ScadaUtilities::MaintenanceData(*(other.m_data)); 
		m_latestPlanDate=other.m_latestPlanDate;
		return *this;  
	}  

	ScadaUtilities::MaintenanceData* MaintenanceDataWrapper::getData()
	{
		return m_data;
	}
	void MaintenanceDataWrapper::setData(ScadaUtilities::MaintenanceData* value)
	{
		if (m_data==value) return;
		delete m_data;
		m_data=value;
	}
	timeb MaintenanceDataWrapper::getLatestPlanDate()
	{
		return m_latestPlanDate;
	}

	timeb MaintenanceDataWrapper::getNextPlanDate()
	{
		return m_nextPlanDate;
	}
	void MaintenanceDataWrapper::setLatestPlanDate(timeb value)
	{
		m_latestPlanDate=value;
	}

	void MaintenanceDataWrapper::initSchedulingTime()
	{
		struct timeb timeNow;		 
		struct tm tmStruct;
		ftime(&timeNow);				
		m_nextPlanDate=timeNow;
		time_t current_timet = timeNow.time;
		if (ACE_OS::localtime_r(&timeNow.time, &tmStruct) == NULL)
		{			 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"cannot get time struct tm" );
			return ;
		}		 
		tmStruct.tm_min=0;
		tmStruct.tm_sec=0;
		 unsigned long factor =  86400;
		if (m_data->reportPeroidType==ScadaUtilities::MaintenanceData::DAILY_BASIS)
		{
			    tmStruct.tm_hour=m_data->reportPeriodTime;
				factor =  86400;
				
		}
		else if (m_data->reportPeroidType==ScadaUtilities::MaintenanceData::HOURLY_BASIS)
		{
				tmStruct.tm_hour=0;
				factor =  3600;
		}		
		time_t nextTime= ACE_OS::mktime (&tmStruct);
		if( nextTime >= current_timet)
		{
			m_nextPlanDate.time = nextTime;
		}
		else
		{
			unsigned long remainder = (current_timet - nextTime) % ( factor);
			m_nextPlanDate.time = current_timet + ( factor - remainder );
		} 
		m_latestPlanDate=m_nextPlanDate;

	}
	void MaintenanceDataWrapper::calcNextSchedulingTime()
	{		 
		m_latestPlanDate=m_nextPlanDate;
		if (m_data->reportPeroidType==ScadaUtilities::MaintenanceData::DAILY_BASIS)
		{
			m_nextPlanDate.time=m_nextPlanDate.time+ 86400;

		}
		else if (m_data->reportPeroidType==ScadaUtilities::MaintenanceData::HOURLY_BASIS)
		{
			m_nextPlanDate.time=m_nextPlanDate.time+ 3600;
		}	
	}
};
