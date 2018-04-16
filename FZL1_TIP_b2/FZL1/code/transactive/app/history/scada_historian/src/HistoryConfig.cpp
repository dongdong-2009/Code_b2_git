/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/scada_historian/src/HistoryConfig.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include <assert.h>
#include <string>
#include <string.h>
#include <time.h>
#include "app/history/scada_historian/src/HistoryConfig.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ScadaHistoryConfigData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/naming/src/Naming.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/database/src/DBException.h"
#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

//#include "HistorianAlarmHandler.h"


#define ENT_CFG_TIMEPERIOD "TimePeriod"
#define ENT_CFG_BASEDATA "BaseData"
#define ENT_CFG_DATATYPE "DataType"
#define ENT_CFG_SHORTDESCRIPTION "ShortDescription"
#define ENT_CFG_LONGDESCRIPTION "LongDescription"
#define ENT_CFG_ARCHIVEFLAG "ArchiveFlag"
#define ENT_CFG_STORAGETIME "StorageTime"
#define ENT_CFG_STARTOFDAY "StartOfDay"
#define ENT_CFG_QUALITYMASK "QualityMask"
#define ENT_CFG_TABLENAME "TableName"

//#define RPARAM_CORBANAME "CorbaName"

#define MIN_TIME_PERIOD 10


using TA_Core::DebugUtil;
using TA_Core::RunParams;
using TA_Core::DatabaseException;
using TA_Core::DataException;


namespace TA_App
{
    //
    // HistoryConfig
    //
    HistoryConfig::HistoryConfig(IEntityData& entityData)
    {
	    LOG(SourceInfo, DebugUtil::FunctionEntry, "HistoryConfig");

        try
        {
            ScadaHistoryConfigData& historyEntityData = dynamic_cast <ScadaHistoryConfigData&> (entityData);

            m_entityKey = historyEntityData.getKey();
	        m_entityName = historyEntityData.getName();
	        m_timePeriod = historyEntityData.getTimePeriod();
            m_tempDataType = historyEntityData.getDataType();
            m_baseData = historyEntityData.getBaseData();
	        m_shortDesc = historyEntityData.getShortDescription();
	        m_longDesc = historyEntityData.getLongDescription();
	        m_archiveFlag = historyEntityData.getArchiveFlag();
	        m_storageTime = historyEntityData.getStorageTime();
            m_tempStartOfDay = historyEntityData.getStartOfDay();
            //m_qualityMask = historyEntityData.getQualityMask();
            m_tableName = historyEntityData.getTableName();

            if(!isValid())
            {
                deactivateServant();
                throw HistorianCfgErrorException("Error in entity configuration", 
                    HistorianCfgErrorException::InvalidParam, m_entityKey);
            }
	    
            //////////////////////////////////////////////

            //std::string corbaName = EntityAccessFactory::getInstance().getEntityAsString( m_entityKey );
            //TA_Core::Naming::registerName( this->_this(), corbaName );
            //RunParams::getInstance().set(RPARAM_CORBANAME, corbaName.c_str());

            activateServantWithName(m_entityName);
        }
        catch ( DatabaseException& ex )
        {
            deactivateServant();
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", ex.what() );
            throw HistorianCfgErrorException(ex.what(), HistorianCfgErrorException::InvalidParam, m_entityKey);
        }
        catch ( DataException& ex )
        {
            deactivateServant();
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "DataException", ex.what() );
            throw HistorianCfgErrorException(ex.what(), HistorianCfgErrorException::InvalidParam, m_entityKey);
        }
        catch ( ... )
        {
  	        // deactivate the servant
	        deactivateServant();

	        // throw the exception on if necessary
	        throw;
        }

        //
        // Set the parameter holding the time for the process execution
        // call. This needs to be called AFTER isValid() is called as
        // isValid() sets up variables that setBeginProcessTime() uses.
        //
        setBeginProcessTime();
    }


    //
    // ~HistoryConfig
    //
    HistoryConfig::~HistoryConfig()
    { 	
        LOG(SourceInfo, DebugUtil::FunctionEntry, "~HistoryConfig");

        deactivateServant();

        LOG(SourceInfo, DebugUtil::FunctionExit, "~HistoryConfig");
    }


    //
    // isValid
    //
    bool HistoryConfig::isValid()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "isValid");

	    if(m_timePeriod == 0)
	    {
            logError("TimePeriod", MISSING);
		    return false;
        }
	    
	    if(m_timePeriod < 0)
	    {
		    logError("TimePeriod", INVALID, 
                "Must be a positive value greater than 0. Failed to create new entity due to invalid time period.");
            return false;
		}

        if(m_timePeriod > 0 && m_timePeriod < 10)
	    {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                "Value for TimePeriod is less than minimum allowed. TimePeriod has been reset to 10 seconds.");
	        m_timePeriod = MIN_TIME_PERIOD;
        }

#if defined(WIN32)
        if(_strnicmp(m_tempDataType.c_str(), "sample", m_tempDataType.size()) == 0)
#else
        if(strcasecmp(m_tempDataType.c_str(), "sample") == 0)
#endif
        {
            m_dataType = TA_App::DataCollectionDmIf::Sample;
        }
#if defined(WIN32)
	    else if(_strnicmp(m_tempDataType.c_str(), "average", m_tempDataType.size()) == 0)
#else
        else if(strcasecmp(m_tempDataType.c_str(), "average") == 0)
#endif
        {
            m_dataType = TA_App::DataCollectionDmIf::Average;
        }
#if defined(WIN32)
	    else if(_strnicmp(m_tempDataType.c_str(), "minimum", m_tempDataType.size()) == 0)
#else
        else if(strcasecmp(m_tempDataType.c_str(), "minimum") == 0)
#endif    
        {
            m_dataType = TA_App::DataCollectionDmIf::Minimum;
        }
#if defined(WIN32)
	    else if(_strnicmp(m_tempDataType.c_str(), "maximum", m_tempDataType.size()) == 0)
#else
        else if(strcasecmp(m_tempDataType.c_str(), "maximum") == 0)
#endif
        {
            m_dataType = TA_App::DataCollectionDmIf:: Maximum;
        }
        else
        {
            logError("DataType", INVALID, 
                "Must be SAMPLE, AVERAGE, MINIMUM or MAXIMUM. Failed to create new entity due to invalid data type.");
		    return false;
        }  

   	    if(m_baseData == "null")
	    {
		    logError("BaseData", MISSING);
            return false;
	    }
	    else
	    {
		    if(!baseDataValid(m_baseData))
		    {
			    logError("BaseData", INVALID, "Failed to create new entity due to invalid base data.");
                return false;
		    }
	    }		

	    if(m_shortDesc == "null")
	    {
		    logError("ShortDescription", MISSING);	
            return false;
	    }

	    if(m_longDesc == "null")
	    {
		    logError("LongDescription", MISSING);	
            return false;
	    }

	    if(m_storageTime == 0)
	    {
		    logError("StorageTime", MISSING);
            return false;
	    }

	    if(m_storageTime < 0)
	    {
		    logError("StorageTime", INVALID, 
                "Must be a positive value greater than 0. Failed to create new entity due to invalid storage time.");
		    return false;
	    }

	    std::string::size_type pos = m_tempStartOfDay.find(":");
        if( pos < 1 || pos > 2 )
        {
            logError("StartOfDay", INVALID, 
                "Must be a valid time in format HH:MM. Failed to create new entity due to invalid start of day.");
		    return false;
        }

	    std::string hourStr = m_tempStartOfDay.substr(0, pos);
	    std::string minuteStr = m_tempStartOfDay.substr(pos+1, m_tempStartOfDay.size()-pos);

	    m_startOfDay.hour = atoi(hourStr.c_str());
        m_startOfDay.minute = atoi(minuteStr.c_str());
	    
	    if(m_tableName == "null")
	    {
		    logError("TableName", MISSING);
            return false;
	    }

        return true;
    }


    //
    // GetDataCollection
    //
    TA_App::DataCollectionDmIf::DataCollection* HistoryConfig::GetDataCollection()
    {
	    TA_App::DataCollectionDmIf::DataCollection* dataCollection = new TA_App::DataCollectionDmIf::DataCollection;

	    dataCollection->entityKey = m_entityKey;
	    dataCollection->dataType = m_dataType;
	    dataCollection->timePeriod = m_timePeriod;
	    dataCollection->baseData = CORBA::string_dup(m_baseData.c_str());
	    dataCollection->shortDescription = CORBA::string_dup(m_shortDesc.c_str());
	    dataCollection->longDescription = CORBA::string_dup(m_longDesc.c_str());
	    dataCollection->archiveOn = m_archiveFlag;
	    dataCollection->storageTime = m_storageTime;
	    dataCollection->startOfDay.hour = m_startOfDay.hour;
	    dataCollection->startOfDay.minute = m_startOfDay.minute;
	    dataCollection->tableName = CORBA::string_dup(m_tableName.c_str());

	    return dataCollection;
    }


    //
    // SetDataCollection
    //
    void HistoryConfig::SetDataCollection(const TA_App::DataCollectionDmIf::DataCollection& newDataCollection, const char* sessionId)
    {
	    //unsigned long entityKey = newDataCollection.entityKey;

        // Check that the operator has sufficent access levels to control
	    // this entity. (ie. The session is allowed to control the entity)
	    //try 
        //{
	    //    if(!EntityControl::GetInstance().AllowControl(sessionId, entityKey))
	    //    {
	    //	    nexus::NoPermission noPermission;
	    //	    throw noPermission;
	    //   }   
        //}
        //catch(BadSessionIdException &)
        //{
        //    nexus::BadParam badParam;
	    //	badParam.reason = CORBA::string_dup("Bad SessionId");
	    //	throw badParam;
        //}
        //catch(BadEntityKeyException &)
        //{
        //    nexus::BadParam badParam;
	    //	badParam.reason = CORBA::string_dup("Bad EntityKey");
	    //	throw badParam;
        //}
	    //catch(...)
	    //{
		    // If we catch anything else, just throw it again
		    // let the client deal with it
	    //	throw;
	    //}

	    TA_ASSERT(newDataCollection.entityKey > 0, "Invalid Entity Key");
	    m_entityKey = newDataCollection.entityKey;

	    m_dataType = newDataCollection.dataType;
	    
        // 
        // Sampling period is a 10 second minimum. If it is less than this
        // then set it to 10.
        //
	    if(newDataCollection.timePeriod < 10)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                "Value for TimePeriod is less than minimum allowed. TimePeriod has been reset to 10 seconds.");
            m_timePeriod = MIN_TIME_PERIOD;
        }
        else
        {
	        m_timePeriod = newDataCollection.timePeriod;
        }

	    TA_ASSERT(std::string(newDataCollection.baseData) != "", "Invalid Base Data");
        
        if(!baseDataValid(std::string(newDataCollection.baseData)))
		{
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "BaseData is invalid. DataCollection attribute could not be set.");
		}
        else
        {
	        m_baseData = newDataCollection.baseData;
        }

	    TA_ASSERT(std::string(newDataCollection.shortDescription) != "", "Invalid Short Description");
	    m_shortDesc = newDataCollection.shortDescription;

	    TA_ASSERT(std::string(newDataCollection.longDescription) != "", "Invalid Long Description");
	    m_longDesc = newDataCollection.longDescription;

	    m_archiveFlag = newDataCollection.archiveOn;
	    
	    TA_ASSERT(newDataCollection.storageTime > 0, "Invalid Storage Time");
	    m_storageTime = newDataCollection.storageTime;

	    m_startOfDay.hour = newDataCollection.startOfDay.hour;
	    m_startOfDay.minute = newDataCollection.startOfDay.minute;
	    
	    //Assert(std::string(newDataCollection.tableName) != "");
	    m_tableName = newDataCollection.tableName;
    }


    //
    // isTimeToProcess
    //
    bool HistoryConfig::isTimeToProcess(time_t currentTime)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, 
            "%s: isTimeToProcess - currentTime: %lu, nextProcessTime: %lu", 
            m_shortDesc.c_str(), currentTime, m_nextProcessTime);
        //
        // Compare the current time with the next processing time
        //
        if( currentTime >= m_nextProcessTime )
        {
            //
            // Update the next process time
            //
            m_nextProcessTime += m_timePeriod;
            return true;
        }

        return false;
    }


    //
    // setBeginProcessTime
    //
    void HistoryConfig::setBeginProcessTime()
    {
        //
        // Get the current time
        //
	    struct tm currentTime;
	    time_t theTime;
	    time(&theTime);

        ACE_OS::localtime_r(&theTime, &currentTime);
        int currentTimeHour = currentTime.tm_hour;
        int currentTimeMin = currentTime.tm_min;

	    int SODHour = m_startOfDay.hour;
	    int SODMinute = m_startOfDay.minute;
	    
	    if(SODHour > currentTimeHour)
	    {
            //
		    // StartOfDay hasn't occurred yet
            //
		    m_nextProcessTime = 
			    calculateBeginProcessTime(theTime, currentTimeHour, currentTimeMin, false);
            return;
	    }
	    else if(SODHour == currentTimeHour)
	    {
            //
		    // Same hour, compare minutes
            //
		    if(SODMinute > currentTimeMin)
		    {
                //
			    // StartOfDay hasn't occurred yet
                //
			    m_nextProcessTime = 
				    calculateBeginProcessTime(theTime, currentTimeHour, currentTimeMin, false);
                return;
		    }
		    else if(SODMinute == currentTimeMin)
		    {
                //
			    // Start of Day is NOW
                //
			    m_nextProcessTime = theTime + m_timePeriod;
                return;
		    }
		    else
		    {
                //
			    // Start of Day has occurred
                //
			    m_nextProcessTime = 
				    calculateBeginProcessTime(theTime, currentTimeHour, currentTimeMin, true);
                return;
		    }
	    }
	    else
	    {
            //
		    // Start of Day has occurred
            //
		    m_nextProcessTime = 
			    calculateBeginProcessTime(theTime, currentTimeHour, currentTimeMin, true);
            return;
	    }
    }


    //
    // calculateBeginProcessTime
    //
    time_t HistoryConfig::calculateBeginProcessTime
    (
	    time_t currentTime, 
	    int currentHour, 
	    int currentMinute, 
	    bool pastStartOfDay
    )
    {
        //
	    // The only difference in the Current time and the Start of Day time would be the 
	    // Hour and Minute attributes. All other attributes should be the same, since it 
	    // is the same day, year, etc. Hence to find the difference, we only need to 
	    // compute the difference between the Hour and Minute by converting them into
	    // seconds first.
        //
	    int SODSeconds = m_startOfDay.hour * 60 * 60 + m_startOfDay.minute * 60;
	    int currentSeconds = currentHour * 60 * 60 + currentMinute * 60;
	    time_t beginProcessTime;

        if(pastStartOfDay)
	    {
            //
		    // Find the number of seconds that has elapsed from the 
		    // Start of Day time to the Current time.
            //
		    int elapsedSeconds = currentSeconds - SODSeconds;

            //
		    // Divide elapsed by 'Sample Time' and truncate to determine
		    // how many intervals are within this elapsed time, and which
		    // interval we are up to.
            //
		    int interval = (int) (elapsedSeconds / m_timePeriod);

            //
		    // Move up to the next interval and convert this back to seconds
            //
		    int extraTime = (interval + 1) * m_timePeriod;

            //
		    // Calculate the time (in seconds) at which to begin processing
            //
		    beginProcessTime = currentTime - (long) elapsedSeconds + (long) extraTime;
	    }
	    else
	    {
		    beginProcessTime = currentTime - (long) currentSeconds + (long) SODSeconds; 
	    }

	    return beginProcessTime;
    }


    //
    // logError
    //
    void HistoryConfig::logError(const std::string& parameterName, ErrorType eType, const std::string& details)
    {
        std::string error("");

        switch(eType)
        {
            case MISSING:   error = "Missing ";
                            break;
            case INVALID:   error = "Invalid ";
                            break;
            default:        // Will never go here
                            break;
        }

        error += "Configuration parameter: ";
        error += parameterName;

        if(!details.empty())
        {
            error += " ";
            error += details;
        }

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, error.c_str());
    }


    //
    // baseDataValid
    //
    bool HistoryConfig::baseDataValid(const std::string& baseData)
    {
	    if(m_dataType == TA_App::DataCollectionDmIf::Sample)
		    return true;

        std::string dbStr = RunParams::getInstance().get(RPARAM_DBCONNECTION);
	    std::vector<std::string> dbComps;
		SimpleDb::ParseConnectStr(dbStr, dbComps);

		SimpleDb::Buffer data;
		try
		{
			SimpleDb db(dbComps[0].c_str(), dbComps[1].c_str(), dbComps[2].c_str()); 

			db.Open();
			
			std::string sql("select NAME from ENTITY where NAME='");
			sql += baseData + "'";

			db.Exec(sql.c_str(), 1, data);
		}
		catch( const DBException& e )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Core::DBException", e.getSummary() );
			return false;
		}

	    if(data.size() != 0)
		    return true;
	    else
		    return false;
    }	


    void HistoryConfig::HistorianCfgErrorException::GenerateAlarm()
    {
	    //HistorianAlarmHandler::AlarmType alarmType;

	    //switch(m_type)
	    //{
	    //case InvalidParam:
	    //	alarmType = HistorianAlarmHandler::ManCfgInvalid;
	    //	break;
	    //case MissingParam:
	    //	alarmType = HistorianAlarmHandler::ManCfgMissing;
	    //	break;
	    //}

         // ANITA: TODO: Handle Alarms

	    //HistorianAlarmHandler::GetInstance().GenerateAlarm(
        //    alarmType, 
	    //	m_eKey,
	    //	m_description, 
	    //	"");
    }


    //
    // update
    //
    void HistoryConfig::update(const TA_Core::ConfigUpdateDetails& updateEvent)
    {
        // TODO
    }

    
    //
    // remove
    //
    void HistoryConfig::remove()
    {
        //TA_Core::Naming::removeNameRegistration(RPARAM_CORBANAME);
        removeNamedObjectRegistration();
    }


    //
    // setToControlMode
    //
    void HistoryConfig::setToControlMode()
    {
        // TODO
    }


    //
    // setToMonitorMode
    //
    void HistoryConfig::setToMonitorMode()
    {
        // TODO
    }

} // TA_App

