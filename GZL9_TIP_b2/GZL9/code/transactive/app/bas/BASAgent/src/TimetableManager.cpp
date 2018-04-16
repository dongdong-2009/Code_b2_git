/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/TimetableManager.cpp $
  * @author:	Huang Jian
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2015/11/27 14:18:31 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */
#include <iomanip>
#include "app/bas/BASAgent/src/TimetableManager.h"
#include "app/bas/BASAgent/src/BASAgent.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "app/bas/BASAgent/src/BasEventSubmitter.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/bas/src/BASTimeTableTimeTableAccessFactory.h"

namespace TA_IRS_App
{
    const int MAXTIMETABLELENGTH = 100;

    TimetableManager::TimetableManager() :
    m_modbusHelper(),
    m_currentDayTTPtBGN(NULL),
    m_nextDayTTPtBGN(NULL),
    m_TTLock(),
    m_runningInControlMode(false),
    m_schedule( NULL)
    {
        m_currentDayTTPtBGN = BasCachedConfig::getInstance().getCurrentDayTTPt();
        m_nextDayTTPtBGN	= BasCachedConfig::getInstance().getNextDayTTPt();

        try
        {
            m_modbusHelper = TA_Base_Bus::ModbusHelperFactory::createHelper(BasCachedConfig::getInstance().getLocationKey(), TA_Base_Bus::ModbusHelperFactory::BAS);
        }
        catch (TA_Base_Bus::NotFoundFEPConfigException& e)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Get Modbus Helper Configuration Error: %s", e.what());
            //TA_ASSERT(false, "Modbus Helper Initialisation Failure!!!");
        }

        if (NULL != m_nextDayTTPtBGN)
        {
			std::string scheduleTimeStr = TA_IRS_App::BasCachedConfig::getInstance().getTTScheduleTime();
			if(!scheduleTimeStr.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timetable will be sending at %s",scheduleTimeStr.c_str());

				std::string::size_type indexColon = scheduleTimeStr.find_first_of(":");
			    if (indexColon != std::string::npos)
			    {
				    const int TIME_LEN = 2;
				    std::string scheduleHrsStr = scheduleTimeStr.substr(0, TIME_LEN);
				    std::string scheduleMinStr = scheduleTimeStr.substr(indexColon + 1, TIME_LEN);
				    int scheduleHrs = atoi(scheduleHrsStr.c_str());
				    int scheduleMin = atoi(scheduleMinStr.c_str());
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "After convert, Timetable will be sending at %d:%d", scheduleHrs, scheduleMin);

				    m_schedule = new TA_IRS_App::PeriodicThread(scheduleHrs, scheduleMin, *this);
				    if ( NULL != m_schedule)
				    {
					    m_schedule->start();
				    }
			    }
			    else
			    {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Set time format error for time table schedule parameter,should be [Hour]:[Minute] ([Hour] and [Minute] use two numbers respectively )");
			    }			
		    }
		    else
		    {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "No set time table schedule parameter");
		    }
        }
		else
		{
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No configure next day timetable datapoint, we don't need to start thread for timetable schedule sent.");
		}
    }

    TimetableManager::~TimetableManager()
    {
        if (NULL != m_schedule)
        {
            m_schedule->terminateAndWait();
            delete m_schedule;
            m_schedule = NULL;
        }
    }

    void TimetableManager::executeTimeTable( TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType timeTableType, CORBA::Long tableKey, const char* sessionID, long locationKey)
    {
    	FUNCTION_ENTRY("executeTimeTable");

    	//TODO check permission

    	TA_THREADGUARD(m_TTLock);

    	TA_IRS_Core::TimeTableScheduleInfoVct timetable;
		unsigned long ulEntityKey = 0;
		std::string strAssetName = "";
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Execute Timetable For Location: %lu, Table: %lu", locationKey, tableKey);

    	try
    	{
    		//use this key to select out the details of the timetable
    		TA_IRS_Core::BASTimeTableMappingAccessFactoryInstance::instance()->getAllTimeTableScheduleInfoByTableTableKey(tableKey, timetable);
    	}
    	catch (TA_Base_Core::DatabaseException& e)
    	{
    		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught Exception When Get Timetable Data: %lu, Reason: %s", tableKey, e.what());
    	}

    	if (timetable.size() > MAXTIMETABLELENGTH)
    	{
    		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Timetable size is wrong: [%lu] size is: [%d]", tableKey, timetable.size());
			throw (TA_Base_Core::NotAvailableException("Timetable data configuration error"));
    	}

    	unsigned long timeTableAddress = 0;
    	if (TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_Today == timeTableType)
    	{
			if (m_currentDayTTPtBGN == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Current day timetable point is non-existent!!!");
				throw (TA_Base_Core::NotAvailableException("Current day timetable point configuration error"));
			}
    		timeTableAddress = m_currentDayTTPtBGN->getDataPointRegisterAddress();
			ulEntityKey = m_currentDayTTPtBGN->getEntityKey();
			strAssetName = m_currentDayTTPtBGN->getAssetName();
    	}
    	else
    	{
			if (m_nextDayTTPtBGN == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Next day timetable point is non-existent!!!");
				throw (TA_Base_Core::NotAvailableException("Next day timetable point configuration error"));
			}
    		timeTableAddress = m_nextDayTTPtBGN->getDataPointRegisterAddress();
			ulEntityKey = m_nextDayTTPtBGN->getEntityKey();
			strAssetName = m_nextDayTTPtBGN->getAssetName();
    	}
    	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Send %s Time Table, timetableKey:%lu , TimeTable size:%lu, address:%lu",
    		TA_IRS_Bus::IStationBasAgentCorbaDef::ETimeTableType_Today == timeTableType ? "Today" : "NextDay", tableKey, timetable.size(), timeTableAddress);

        bool bSucceed = false;

    	try
    	{
            sendTimeTable(timetable, timeTableAddress);
            bSucceed = true;
    	}
    	catch(const TA_Base_Core::TransactiveException& ex)
    	{
    		LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
    	}
		catch(const std::exception& e)
		{
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(e).name(), e.what());
		}
    	catch(...)
    	{
    		LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when send Time Table.");
    	}

        std::string strTableName = BasCachedConfig::getInstance().getTimetableDisplayNameByKey(tableKey);
        BasEventSubmitter::instance()->submitBasTimetableDownloadEvent(timeTableType, strTableName, bSucceed, sessionID, ulEntityKey, strAssetName);

        if ( !bSucceed )
        {
            throw (TA_IRS_Bus::IStationBasAgentCorbaDef::StationWriteDataPointError("Failed to send Time Table."));
        }

    	FUNCTION_EXIT;
    }
	
    void TimetableManager::sendTimeTable( const TA_IRS_Core::TimeTableScheduleInfoVct& timeTable, unsigned long registerAddress )
    {
        FUNCTION_ENTRY("sendTimeTable");

        TA_Base_Bus::DataBlock <WORD> commands;
        //Modified by lixiaoxia(GF-385)
        commands.setStartAndLength(registerAddress, MAXTIMETABLELENGTH * 2);	// each timetable record will contain 2 WORD: 1 WORD mode value + 1 WORD time begin
        int wordCount = registerAddress;
		int nLoop = 0;

        //unsigned long registerAddress = m_currentDayTTPtBGN->getDataPointRegisterAddress();
        //registerAddress = registerAddress - m_offsetAddress; m_offsetAddress is 0
        //registerAddress = m_rtu.adjustAddress(registerAddress); no need adjust(-1), cuz STE modbus
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timetable command start address: %lu", registerAddress);

        for (nLoop = 0; nLoop < timeTable.size(); ++nLoop)
        {
            WORD modeValue = 0;
            WORD modeTime = 0;

            conversionHelper(timeTable[nLoop]->m_modeValue, modeValue);
            conversionHelperForTime(timeTable[nLoop]->m_tmBegin, modeTime);

            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Timetable[%d]:Value[%d];Time[%d]", nLoop, modeValue, modeTime);

            commands.set(wordCount++, modeValue);
            commands.set(wordCount++, modeTime);
        }
		
		for (nLoop = timeTable.size(); nLoop < MAXTIMETABLELENGTH; ++nLoop)
		{
			WORD defaultValue = 0;
			commands.set(wordCount++, defaultValue);
			commands.set(wordCount++, defaultValue);
		}

        m_modbusHelper->SendData(commands);
        FUNCTION_EXIT;
    }

    void TimetableManager::conversionHelper( const std::string& input, WORD& output )
    {
        FUNCTION_ENTRY("conversionHelper");

        try
        {
            output = boost::lexical_cast<WORD>(input);
        }
        catch (boost::bad_lexical_cast& ex)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "timetable data: %s is illegal: %s", input.c_str(), ex.what());
            //TA_ASSERT(false, "timetable data is illegal!!!")		
        }

        FUNCTION_EXIT;
    }

    void TimetableManager::conversionHelperForTime( const time_t tmInput, WORD& output )
    {
        FUNCTION_ENTRY("conversionHelperForTime");

        WORD hrs = 0u;
        WORD mins = 0u;

        try
        {
            tm tmConv = *(localtime( &tmInput ));

            hrs = tmConv.tm_hour;
            mins = tmConv.tm_min;
        }
        catch ( ... )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception caught when call localtime %lu", tmInput );
        }

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Mode Execution Time %d:%d", hrs, mins);

        WORD DISPLACEMENT = 0x100;
        output = hrs * DISPLACEMENT + mins;

        FUNCTION_EXIT;

    }

    void TimetableManager::executeScheduleTimeTable( const std::string& weekday )
    {
        FUNCTION_ENTRY("executeStationMode");

        TA_THREADGUARD(m_TTLock);

        TA_IRS_Core::TimeTableScheduleInfoVct timetable;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Execute Timetable For Location: %lu, On: %s", BasCachedConfig::getInstance().getLocationKey(), weekday.c_str());

		if (NULL == m_nextDayTTPtBGN)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Next day timetable point is non-existent.");
			return;
		}

        try
        {
            //use this key to select out the details of the timetable
            TA_IRS_Core::BASTimeTableMappingAccessFactoryInstance::instance()->getAllTimeTableScheduleInfoByLocationAndType(
                BasCachedConfig::getInstance().getLocationKey(), weekday, timetable);
        }
        catch (const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
		catch(const std::exception& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
		}
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when getAllTimeTableScheduleInfoByLocationAndType");
        }

        if (timetable.size() > MAXTIMETABLELENGTH)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Timetable size is wrong: size is: [%d]", timetable.size());
            return;
        }

        try
        {
            sendTimeTable(timetable, m_nextDayTTPtBGN->getDataPointRegisterAddress());
        }
        catch(const TA_Base_Core::TransactiveException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch(...)
        {
            std::string strException = "Unknown exception when send Time Table.";
            LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", strException.c_str());
        }
        FUNCTION_EXIT;
    }

    void TimetableManager::setControlMode( bool bControlMode )
    {
        m_runningInControlMode = bControlMode;

    }

    bool TimetableManager::isAgentRunningControlMode()
    {
        return m_runningInControlMode;
    }

	std::string TimetableManager::formatTimeString( const time_t tmTime )
	{
        tm tmConvert = *(localtime( &tmTime ));

		std::stringstream stmConvertor;
		stmConvertor << std::setw(2) << std::setfill( '0' )<< tmConvert.tm_hour << std::setw(2) << std::setfill( '0' ) << tmConvert.tm_min;

		return stmConvertor.str().c_str();
	}

	void TimetableManager::deleteTimeTableScheduleInfoByLocationAndType(unsigned long locationKey, const std::string& strType)
	{
		FUNCTION_ENTRY("deleteTimeTableScheduleInfoByLocationAndType");

        //get timetable key.
		unsigned long timeTableKey = TA_IRS_Core::BASTimeTableTimeTableAccessFactoryInstance::instance()->getTimeTableKeyByLocationkeyAndType(locationKey, strType);

		// delete from DB by timetablekey
		TA_IRS_Core::BASTimeTableMappingAccessFactoryInstance::instance()->deleteTimeTableMapping(timeTableKey);

		FUNCTION_EXIT;
	}
}
