/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/IncidentReportHelper.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #10 $
  *
  * Last modification: $DateTime: 2014/06/24 16:24:23 $
  * Last modified by:  $Author: Shiva $
  *
  * IncidentReportHelper is an object that is held by CtaIncidentReportData objects. 
  * It contains all data used by IncidentReport, and manipulation
  * methods for the data. It helps avoid re-writing code for both CtaIncidentReportData
  */
#include <sstream>
#include <iomanip>
#include <algorithm>

#include "core/data_access_interface/incident_dai/src/IncidentReportHelper.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/IOperator.h"
//TD15204
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/DBCounterTypes.h" 
//++lizettejl
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/AlarmAccessFactory.h"
//Mintao ++
//Bug 652 (TD12313)
#include "core/uuid/src/TAuuid.h"
//Mintao ++
//Bug 652 (TD12313)

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
//#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/IncidentException.h"
#include "core/exceptions/src/DataException.h"

#include "core/alarm/src/AlarmConfig.h"
#include "core/utilities/src/DateFormat.h" 


#include "ace/OS.h"

using namespace std;
using namespace TA_Base_Core;

namespace TA_IRS_Core
{
const std::string IncidentReportHelper::IN_ININCI_ID_COL		           = "inc.ININCI_ID";
const std::string IncidentReportHelper::IN_TITLE_COL	                   = "inc.TITLE";
const std::string IncidentReportHelper::IN_CREATE_OPERATOR_ID_COL         = "inc.CREATE_OPERAT_ID";
const std::string IncidentReportHelper::IN_CREATE_OPERATOR_PROFILE_ID_COL = "inc.CREATE_SEPROF_ID";
const std::string IncidentReportHelper::IN_CREATE_DATE_COL                = "inc.CREATE_DATE";
const std::string IncidentReportHelper::IN_MODIFY_OPERATOR_ID_COL         = "inc.MODIFY_OPERAT_ID";
const std::string IncidentReportHelper::IN_MODIFY_OPERATOR_PROFILE_ID_COL = "inc.MODIFY_SEPROF_ID";
const std::string IncidentReportHelper::IN_MODIFY_DATE_COL                = "inc.MODIFY_DATE";
const std::string IncidentReportHelper::IN_FALSE_INCIDENT_COL             = "inc.FALSE_INCIDENT";
const std::string IncidentReportHelper::IN_ENTITY_ID_COL                  = "inc.ENTITY_ID";
const std::string IncidentReportHelper::IN_LOCATION_ID_1_COL              = "inc.LOCATI_ID_1";
const std::string IncidentReportHelper::IN_LOCATION_ID_2_COL              = "inc.LOCATI_ID_2";
const std::string IncidentReportHelper::IN_TRAIN_ID_COL                   = "inc.TRAIN_ID";
const std::string IncidentReportHelper::IN_START_DATE_COL                 = "inc.START_DATE";
const std::string IncidentReportHelper::IN_END_DATE_COL                   = "inc.END_DATE";
const std::string IncidentReportHelper::IN_INNER_TRACK_COL                = "inc.INNER_TRACK";
const std::string IncidentReportHelper::IN_OUTER_TRACK_COL                = "inc.OUTER_TRACK";
const std::string IncidentReportHelper::IN_KM_LOSS_COL                    = "inc.KM_LOSS";
const std::string IncidentReportHelper::IN_INITIAL_DELAY_COL              = "inc.INITIAL_DELAY";
const std::string IncidentReportHelper::IN_TOTAL_DELAY_COL                = "inc.TOTAL_DELAY";
const std::string IncidentReportHelper::IN_ALARM_ID_COL                   = "inc.ALARM_ID";
const std::string IncidentReportHelper::IN_ALARM_DESC_COL                 = "inc.ALARM_DESC";
const std::string IncidentReportHelper::IN_ALARM_VALUE_COL                = "inc.ALARM_VALUE";
const std::string IncidentReportHelper::IN_OPERATORS_COL                  = "inc.OPERATORS";
const std::string IncidentReportHelper::IN_COMMENTS_COL                   = "inc.COMMENTS";
const std::string IncidentReportHelper::IN_RECOMMENDATION_COL             = "inc.RECOMMENDATION";
const std::string IncidentReportHelper::IN_STATION_ID_COL                 = "inc.INSTAT_ID";
const std::string IncidentReportHelper::IN_CLOSE_DATE_COL                 = "inc.CLOSE_DATE";
const std::string IncidentReportHelper::IN_CREATE_LOCATION_ID_COL         = "inc.CREATE_LOCATI_ID";

const std::string IncidentReportHelper::IN_LOCATION_NAME_1_COL              = "loc1.NAME";
const std::string IncidentReportHelper::IN_LOCATION_NAME_2_COL              = "loc2.NAME";

const std::string IncidentReportHelper::IN_PLAN_ID_COL           = "plan.PLAN_ID";
unsigned long IncidentReportHelper::m_consoleLocationKey = 0;
std::string IncidentReportHelper::m_consoleLocationName = "";

//++shiva
//changed to 7 digit max value
const int IncidentReportHelper::MAX_INCIDENTCOUNTER = 9999999;//99999;

IncidentReportHelper::IncidentReportHelper() :
    m_isValidData(false),
    m_isNew(true),
    m_isValidSummaryData(false),
    m_plansChanged(false),
	//TD15204 - additional initializations
	m_dbConnection(NULL), 
	m_reportId(""),
	m_reportIdCounter(0)
{
    FUNCTION_ENTRY("Default Constructor");

    initialize();

    string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
    //Bugzilla Bug 149: Mintao
    ISession* session = NULL;
    //Bugzilla Bug 148: Mintao
    IProfile* profileData = NULL;

    try
    {
        session  = SessionAccessFactory::getInstance().getSession(sessionId);

        m_createOprId = session->getOperatorKey();
        m_creator = session->getOperatorName();
        m_createProfileOprId = session->getProfileKey();
        
         m_createLocationId = session->getLocationKey();
       //Bugzilla Bug 149: Mintao
        delete session;
        session = NULL;

        profileData = ProfileAccessFactory::getInstance().getProfile(m_createProfileOprId);
        m_creatorProfile = profileData->getName();
        
        //Bugzilla Bug 148: Mintao 
        delete profileData;
        profileData = NULL;
    }
    catch (exception&)
    {
        
        //Bugzilla Bug 149: Mintao
        delete session;
        session = NULL;

        //Bugzilla Bug 148: Mintao
        delete profileData;
        profileData = NULL;
    }

    //Mintao ++
    //Bug 652 (TD12313)
    //Bugzilla Bug 167: Mintao
	// TD15204 - removed 
    // m_reportId = TA_Base_Core::TA_Uuid::generateUUID();
	m_reportId = retrieveReportID(); //TD15204
    //Mintao ++
    //Bug 652 (TD12313)
    //Bug 646 

    FUNCTION_EXIT;
}

//Mintao ++
//Bug 652 (TD12313)
IncidentReportHelper::IncidentReportHelper(std::string& id, time_t createDate,
        const std::string& title, 
        const std::string& location1, const std::string& location2,
        bool innerTrack, bool outerTrack, const std::string& train, 
        const std::string& alarmId, const std::string& asset, 
        const std::string& subsystem, const std::string& creator, const std::string& profile, 
        time_t closeDate) :
    m_isValidData(false),
    m_isNew(false),
    m_isValidSummaryData(true),
    m_plansChanged(false),
	//TD15204 - additional initializations
	m_dbConnection(NULL), 
	m_reportId(""),
	m_reportIdCounter(0)
{
    FUNCTION_ENTRY("Constructor");

    initialize();

    m_reportId = id;
    //Mintao ++
    //Bug 652 (TD12313)
    m_createDate = createDate;

    m_title = title;
    m_location1 = location1;
    m_location2 = location2;
    m_innerTrack = innerTrack;
    m_outerTrack = outerTrack;
    m_train = train;
    m_alarmId = alarmId;
	m_alarmValueFromCmdline ="";

    m_assetType.id = 0;
    m_assetType.description = asset;
    m_assetType.subsystem = subsystem;

    m_creator = creator;
    m_creatorProfile = profile;
    m_closeDate = closeDate;

    FUNCTION_EXIT;
}

//Mintao ++
//Bug 652 (TD12313)
IncidentReportHelper::IncidentReportHelper(const std::string idKey, bool isIDKey) :
    m_isValidData(false),
    m_isNew(false),
    m_isValidSummaryData(false),
    m_plansChanged(false),
	//TD15204 - additional initializations
	m_dbConnection(NULL), 
	m_reportId(""),
	m_reportIdCounter(0)
{
    FUNCTION_ENTRY("Constructor");

    initialize();
    m_reportId = idKey;

    FUNCTION_EXIT;
}
//Mintao ++
//Bug 652 (TD12313)

IncidentReportHelper::IncidentReportHelper(const std::string & alarmId, const std::string & alarmValue):
    m_isValidData(false),
    m_isNew(true),
    m_isValidSummaryData(false),
    m_plansChanged(false),
	//TD15204 - additional initializations
	m_dbConnection(NULL), 
	m_reportId(""),
	m_reportIdCounter(0)
{
    FUNCTION_ENTRY("Constructor");

    initialize();

    string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
    //Bugzilla Bug 149: Mintao
    ISession* session = NULL;
    //Bugzilla Bug 148: Mintao
    IProfile* profileData = NULL;

    try
    {
        session = SessionAccessFactory::getInstance().getSession(sessionId);

        m_createOprId = session->getOperatorKey();
        m_creator = session->getOperatorName();
        m_createProfileOprId = session->getProfileKey();
        m_createLocationId = session->getLocationKey();
        
        //Bugzilla Bug 149: Mintao
        delete session;
        session = NULL;

        profileData = ProfileAccessFactory::getInstance().getProfile(m_createProfileOprId);
        m_creatorProfile = profileData->getName();

        //Bugzilla Bug 148: Mintao 
        delete profileData;
        profileData = NULL;
    }
    catch (exception&)
    {
        //Bugzilla Bug 149: Mintao
        delete session;
        session = NULL;

        //Bugzilla Bug 148: Mintao
        delete profileData;
        profileData = NULL;
    }

    // get the alarm data, throw exception if alarm is not valid
    m_alarmId = alarmId;
	m_alarmValueFromCmdline = alarmValue;
    getAlarmDetailFromID(m_alarmId, m_alarmValueFromCmdline );
	loadAssetDetailsFromAlarm(m_alarmId); //TD15806
	loadPlanFromAlarm(m_alarmId); //TD15871, hongzhi
    //Mintao ++
    //Bug 652 (TD12313)
    //Bugzilla Bug 167: Mintao
	//TD15204 - removed 
    //m_reportId = TA_Base_Core::TA_Uuid::generateUUID();	
	m_reportId = retrieveReportID(); //TD15204
    //Mintao ++
    //Bug 652 (TD12313)

    FUNCTION_EXIT;
}

void IncidentReportHelper::getAlarmDetailFromID(const std::string & alarmId, const std::string & alarmValue)
{
    FUNCTION_ENTRY("getAlarmDetailFromID");
    if (alarmId.empty())
    {
        m_alarmDetail = "";
        return;
    }

    try
    {
        IAlarmData* alarmdata = AlarmAccessFactory::getInstance().getAlarm(alarmId);   
        timeb sourceTime = alarmdata->getSourceTime();
        string alarmAsset = alarmdata->getAssetName();
        // string alarmDescription = alarmdata->getDescription();
		// Wenguang Alarm Message Size Reduction
		string alarmDescription = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarmdata->getTypeKey(), alarmdata->getParamList());
        unsigned long alarmServerity = alarmdata->getSeverity();
        //IAlarmData::EAlarmState alarmState = alarmdata->getState();
		IAlarmData::EAlarmAVLStatus alarmAVLState = alarmdata->getAVLStatus();
		std::string alarmAVLStateDesc = getAlarmAVLStateDesc( alarmAVLState );
        string alarmPlanStatus = alarmdata->getPlanStatus();
        IAlarmData::EMmsState alarmEmmsState = alarmdata->getMmsStateType();
		std::string mmsStateDesc = getMmsStateDesc( alarmEmmsState );
        string alarmAck = alarmdata->getAcknowledgedBy();
        string alarmComments = alarmdata->getComments();
        string sourceTimeDesc = getTimeDesc(sourceTime.time);

        string opname = "";
        //Mintao++
        //Bug 770 (TD12864)
        if (!alarmAck.empty())
        {
            ISession* session = NULL;
            session = SessionAccessFactory::getInstance().getSession(alarmAck);
            unsigned long opkey = session->getOperatorKey();
            opname = session->getOperatorName();
            delete session;
            session = NULL;
        }
        //Mintao++
        //Bug 770 (TD12864)

        //Mintao++
        //bug 644
		// TD15042 - remove brackets to comply with GDN, and added commas 
        //m_alarmDetail = "[Date/Time = " + displaysrctime + "]" ;
		m_alarmDetail = "Date/Time = " + sourceTimeDesc + ", " ;
        ostringstream serstr;
        serstr << alarmServerity;
        //m_alarmDetail += "[Sev = " + serstr.str() + "]";
		m_alarmDetail += "Sev = " + serstr.str() + ", ";
        //m_alarmDetail += "[Asset = " + alarmAsset + "]";
		m_alarmDetail += "Asset = " + alarmAsset + ", ";
        //m_alarmDetail += "[Description = " + alarmDescription + "]";
		m_alarmDetail += "Description = " + alarmDescription + ", ";
        //m_alarmDetail += "[Value = " + alarmValue + "]";
		m_alarmDetail += "Value = " + alarmValue + ", ";
        
        //m_alarmDetail += "[D = " + alarmPlanStatus + "]";
		m_alarmDetail += "D = " + alarmPlanStatus + ", ";
        //m_alarmDetail += "[A = " + alarmstatestr.str() + "]";
		m_alarmDetail += "A = " + alarmAVLStateDesc + ", ";
        //m_alarmDetail += "[M = " + mmsstr.str() + "]";
		m_alarmDetail += "M = " + mmsStateDesc + ", ";

        //m_alarmDetail += "[Ack By = " + opname + "]";
		m_alarmDetail += "Ack By = " + opname + ", ";
        //m_alarmDetail += "[Comments = " + alarmComments + "]";
		m_alarmDetail += "Comments = " + alarmComments ;
		// ++lizettejl
    //Mintao++
    //bug 644
    }
    catch (...)
    {
        return;
    }
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"ReportId: %s, AlarmId: %s, getAlarmDetailFromID: %s", 
				m_reportId.c_str(), m_alarmId.c_str(), m_alarmDetail.c_str() );
    FUNCTION_EXIT;
}

void IncidentReportHelper::loadPlanFromAlarm(std::string alarmid)		//TD15871, hongzhi
{
    FUNCTION_ENTRY("loadPlans");

    m_plansExecuted.clear();
    IDatabase* database = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);
    if (database == NULL)
    {
        TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
    }

    // create the SQL string to retrieve the data of the IncidentReport
    // based upon the key
    // create the SQL string to retrieve the data of the IncidentReportData
    //ostringstream sql;

/*    sql << "select alarmplanmaps1.PLANPATH as \"" << IN_PLAN_ID_COL << "\""
		<< " from alarmplanmaps alarmplanmaps1, alarm alarm1"
		<< " where alarmplanmaps1.entitykey = alarm1.ENTITYKEY"
		<< " and alarm1.TYPE = alarmplanmaps1.ALARMTYPEKEY"
		<< " and alarm1.ALARM_ID = '" << alarmid << "'"
		<< " union"
		<< " select ae_alarm_rule_param1.ACTION_PARAMETER_VALUE as \"" << IN_PLAN_ID_COL << "\""
		<< " from alarmrule alarmrule1, alarm alarm1, ae_alarm_rule_param ae_alarm_rule_param1"
		<< " where alarmrule1.entitykey = alarm1.ENTITYKEY"
		<< " and alarm1.TYPE = alarmrule1.ALARMTYPE"
		<< " and ae_alarm_rule_param1.ALARM_RULE_KEY = alarmrule1.PKEY"
		<< " and ae_alarm_rule_param1.ACTION_PARAMETER_TYPE = 'PLAN_PATH'"
		<< " and alarm1.ALARM_ID = '" << alarmid << "'";
*/

	/*sql	<< " SELECT alarmplanmaps1.planpath AS \"" << IN_PLAN_ID_COL << "\""
		<< " FROM alarmplanmaps alarmplanmaps1, alarm alarm1, entity entity1"
		<< " WHERE((alarmplanmaps1.entitykey = alarm1.entitykey	AND alarm1.TYPE = alarmplanmaps1.alarmtypekey)"
		<<  " OR(alarmplanmaps1.entitytypekey = entity1.typekey AND alarm1.TYPE = alarmplanmaps1.alarmtypekey))"
		<< " AND alarm1.alarm_id = '" << alarmid << "'"
		<< " AND entity1.pkey = alarm1.entitykey"
		<< " UNION"
		<< " SELECT ae_alarm_rule_param1.action_parameter_value AS \"" << IN_PLAN_ID_COL << "\""
		<< " FROM alarmrule alarmrule1,alarm alarm1,ae_alarm_rule_param ae_alarm_rule_param1,entity entity1"
		<< " WHERE((alarmrule1.entitykey = alarm1.entitykey	AND alarm1.TYPE = alarmrule1.alarmtype)"
		<<  " OR(alarmrule1.entitytypekey = entity1.typekey	AND alarm1.TYPE = alarmrule1.alarmtype))"
		<< " AND ae_alarm_rule_param1.alarm_rule_key = alarmrule1.pkey"
		<< " AND ae_alarm_rule_param1.action_parameter_type = 'PLAN_PATH'"
		<< " AND alarm1.alarm_id = '" << alarmid << "'"
		<< " AND entity1.pkey = alarm1.entitykey";*/
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(),ALARMPLANMAPS_STD_SELECT_2001,alarmid,alarmid);
//	std::string strSql  = database->prepareSQLStatement(ALARMPLANMAPS_SELECT_2001,alarmid,alarmid);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, ALARMPLANMAPS_SELECT_2001,alarmid,alarmid);
    // Set up the columnNames vector to be passed to executeQuery()
    vector<string> columnNames;
	columnNames.push_back(IN_PLAN_ID_COL);

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    IData* data = database->executeQuery(strSql,columnNames);
    if (data == NULL)
    {
        TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
    }

	if(data->getNumRows() > 0)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"ReportId: %s, AlarmId: %s, m_plansChanged Set to true", 
				m_reportId.c_str(), m_alarmId.c_str());

		m_plansChanged = true;
	}
    do
    {
        for (unsigned long i = 0; i < data->getNumRows() ; ++i )
        {
            PlanType pType;
            pType = data->getStringData(i, IN_PLAN_ID_COL);
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"ReportId: %s, AlarmId: %s, PlanID: %s", 
				m_reportId.c_str(), m_alarmId.c_str(), pType.c_str() );

            m_plansExecuted.push_back(pType);
        }

        delete data;
        data = NULL;
    }while( database->moreData(data) );


    FUNCTION_EXIT;
}

IncidentReportHelper::~IncidentReportHelper()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getLocalDatabaseName()
{
    if (m_localDatabase.empty())
    {
        // have to find out what is the local db name
        // this should be the first in the connection list so grab it
        if (! RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
        {
            throw DatabaseException("db-connection-file not set");
        }
        try
        {
            /*DbConnectionStrings connectionStrings(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
            DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Incident_Ad, Write);
            m_localDatabase = connections[0].first; */ 
			std::string strDbConFile(RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
			m_localDatabase = DatabaseFactory::getInstance().getDatabaseName(strDbConFile, TA_Base_Core::Incident_Ad, Write);
        }
        catch(...) // most likely a file not found error
        {
            throw DatabaseException("Unable to find database connection infomation");
        }

    }
    return m_localDatabase;
}

void IncidentReportHelper::close()
{
    FUNCTION_ENTRY("close");
	// lizettejl - TD13794
	// handle assertion failure
    // TA_ASSERT(m_closeDate == 0, "report is already closed");
	if (m_closeDate == 0)
	{
		// get the modifier profile & modify date
		updateModifyData();
		try
		{
			IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
			if (databaseConnection == NULL)
			{
				TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
			}
			
			//ostringstream sql;
			TA_Base_Core::SQLTypeAdapter sqlParam;
			time_t closeDate = time(NULL);			
			getUpdateDateSql(sqlParam, closeDate, true);
			
			// delete station report first, then the incident report
		/*	sql << "BEGIN ";
			sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
			sql << "update IN_INCIDENT_REPORT"
				<< " set CLOSE_DATE = " << databaseConnection->escapeQueryString(getUpdateDateSql(closeDate))*/
				// TD15865 Yanrong++
				//<< ", EDIT_OPERATOR_COUNT = 0"
				// ++Yanrong
				//<<      "MODIFY_OPERAT_ID=" << m_modifyOprId << ","
				//<<      "MODIFY_SEPROF_ID=" << m_modifyProfileOprId 
				//TD15864
			/*	<< " where ININCI_ID = ''" << m_reportId << "''";
			sql << "');";
			sql << "END;";*/
//			std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_UPDATE_52702,
//			std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_UPDATE_52702,
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_UPDATE_52702,
				getLocalDatabaseName(), sqlParam, m_reportId);
			
			databaseConnection->executeModification(strSql);
			
			
			m_closeDate = closeDate;
		}
		catch (DataException&) 
		{
			ostringstream reason;
			reason << "Cannot close report = " << m_reportId;			
			TA_THROW(IncidentException(reason.str(),IncidentException::DATABASE_ERROR));
		}
	}
    FUNCTION_EXIT;
}

void IncidentReportHelper::reopen()
{
    FUNCTION_ENTRY("reopen");
    
    TA_ASSERT(m_closeDate != 0, "report is already open");

    // get the modifier profile & modify date
    updateModifyData();

    try
    {
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
        if (databaseConnection == NULL)
        {
            TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
        }

        //ostringstream sql;

        // delete station report first, then the incident report
     /*   sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        sql << "update IN_INCIDENT_REPORT"
            << " set CLOSE_DATE = NULL"*/
			// TD15865 Yanrong++
			//<< ", EDIT_OPERATOR_COUNT = 0"
			// ++Yanrong
            //<<      "MODIFY_OPERAT_ID=" << m_modifyOprId << ","
            //<<      "MODIFY_SEPROF_ID=" << m_modifyProfileOprId 
            //TD15864
		/*	<< " where ININCI_ID = ''" << m_reportId << "''";
        sql << "');";
        sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),IN_INCIDENT_REPORT_Oracle_UPDATE_52703, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_UPDATE_52703, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_UPDATE_52703,
			getLocalDatabaseName(),m_reportId);

        databaseConnection->executeModification(strSql);
    }
    catch (DataException&) 
    {
        ostringstream reason;
		reason << "Cannot reopen report = " << m_reportId;			
        TA_THROW(IncidentException(reason.str(),IncidentException::DATABASE_ERROR));
    }

    FUNCTION_EXIT;
}

void IncidentReportHelper::deleteThisReport()
{
    FUNCTION_ENTRY("deleteThisReport");
    TA_ASSERT(!m_isNew, "This report does not yet exist in the database, and therefore cannot be deleted");
    
    try
    {
        IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
		//int nDbServerType = databaseConnection->GetDbServerType();
        if (databaseConnection == NULL)
        {
            TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
        }
        //ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
        
        // must remove the plan first
        // remove existing association first
        if (m_plansExecuted.size() > 0)
        {
           /* sql << "BEGIN ";
            sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
            sql << "delete IN_PLAN where ININCI_ID=''" << m_reportId <<"''";
            sql << "');";
            sql << "END;";*/
			//strSql = defPrepareSQLStatement(nDbServerType, IN_PLAN_Oracle_DELETE_53951, getLocalDatabaseName(), m_reportId);
//			strSql = databaseConnection->prepareSQLStatement(IN_PLAN_DELETE_53951, getLocalDatabaseName(), m_reportId);

		databaseConnection->prepareSQLStatement(strSql, IN_PLAN_DELETE_53951, getLocalDatabaseName(), m_reportId);
            databaseConnection->executeModification(strSql);
        }

        //sql.str("");

        // delete incident report first, then the station report
       /* sql << "BEGIN ";
        sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
        sql << "delete IN_INCIDENT_REPORT where ININCI_ID = ''" << m_reportId << "''";
        sql << "');";
        sql << "END;";*/
		//strSql = defPrepareSQLStatement(nDbServerType, IN_INCIDENT_REPORT_Oracle_DELETE_52951,getLocalDatabaseName(), m_reportId);
//		strSql = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_DELETE_52951, getLocalDatabaseName(), m_reportId);

		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_DELETE_52951, getLocalDatabaseName(), m_reportId);
        databaseConnection->executeModification(strSql);

        //sql.str("");
        if (!m_stationId.empty())
        {
           /* sql << "BEGIN ";
            sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
            sql << "delete IN_STATION_REPORT where INSTAT_ID = ''" << m_stationId << "''";
            sql << "');";
            sql << "END;";*/
			//strSql = defPrepareSQLStatement(nDbServerType, IN_STATION_REPORT_Oracle_DELETE_54451,getLocalDatabaseName(),m_stationId );
//			strSql = databaseConnection->prepareSQLStatement(IN_STATION_REPORT_DELETE_54451, getLocalDatabaseName(),m_stationId);

		databaseConnection->prepareSQLStatement(strSql, IN_STATION_REPORT_DELETE_54451, getLocalDatabaseName(),m_stationId);
            databaseConnection->executeModification(strSql);
        }


    }
    catch (DataException&) 
    {
        ostringstream reason;
		reason << "Cannot delete report = " << m_reportId;			
        TA_THROW(IncidentException(reason.str(),IncidentException::DATABASE_ERROR));
    }

    FUNCTION_EXIT;
}

//Mintao ++
//Bug 652 (TD12313)

void IncidentReportHelper::updateStationReportId(std::string  stationId)
{
    FUNCTION_ENTRY("updateStationReportId");

    TA_ASSERT(m_isNew == false, "Cannot set station id for an unsaved incident report");

    // get the modifier profile & modify date
    updateModifyData();

    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
    if (databaseConnection == NULL)
    {
        TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
    }

    // commit change to stationId field to database
   /* ostringstream sql;
    sql << "BEGIN ";
    sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
    sql << "UPDATE IN_INCIDENT_REPORT"
        << " SET INSTAT_ID=''" << stationId << "'',"
        <<      "MODIFY_OPERAT_ID=" << m_modifyOprId << ","
        <<      "MODIFY_SEPROF_ID=" << m_modifyProfileOprId << ","
        <<      "MODIFY_DATE=" << databaseConnection->escapeQueryString(getUpdateDateSql(m_modifyDate))
        << " WHERE ININCI_ID=''" << m_reportId << "''";
    sql << "');";
    sql << "END;";*/
//	std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_UPDATE_52701,getLocalDatabaseName(),
//	std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_UPDATE_52701,getLocalDatabaseName(),
	TA_Base_Core::SQLTypeAdapter sqlParam;
	getUpdateDateSql(sqlParam, m_modifyDate, true);

	TA_Base_Core::SQLStatement strSql;
	databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_UPDATE_52701,getLocalDatabaseName(),
		stationId,m_modifyOprId,m_modifyProfileOprId, sqlParam, m_reportId);

	databaseConnection->executeModification(strSql);

    FUNCTION_EXIT;
}
//Mintao ++
//Bug 652 (TD12313)



time_t IncidentReportHelper::getCloseDate()
{
    FUNCTION_ENTRY("getCloseDate");
	//TD15439 - must retrieve correct closed date data
	if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
	// ++lizettejl
    FUNCTION_EXIT;
    return m_closeDate;
}

//Mintao ++
//Bug 652 (TD12313)

std::string IncidentReportHelper::getReportId()
{
    FUNCTION_ENTRY("getReportId");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

	if(m_isNew == true)
	{	//TD15867, hongzhi
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"A New Report, ReportId: %s, so Conceal the report Id", m_reportId.c_str());
		return "";
	}
    return m_reportId;
}
//Mintao ++
//Bug 652 (TD12313)

//Mintao++
//bug 644
std::string IncidentReportHelper::getAssociatedAlarmDetail()
{
    FUNCTION_ENTRY("getAssociatedAlarmDetail");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;
    
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getAssociatedAlarmDetail, alarmID=%s, alarmValue=%s", 
	 	m_alarmId.c_str(), m_alarmValueFromCmdline.c_str());
    getAlarmDetailFromID(m_alarmId, m_alarmValueFromCmdline );
    return m_alarmDetail;
}
//Mintao++
//bug 644

void IncidentReportHelper::setTitle(const std::string& title)
{
    FUNCTION_ENTRY("setTitle");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    m_title = title;
}

std::string IncidentReportHelper::getTitle()
{
    FUNCTION_ENTRY("getTitle");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_title;
}

std::string IncidentReportHelper::getCreator()
{
    FUNCTION_ENTRY("getCreator");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_creator;
}

std::string IncidentReportHelper::getCreatorProfile()
{
    FUNCTION_ENTRY("getCreatorProfile");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_creatorProfile;
}

time_t IncidentReportHelper::getCreatedDateTime()
{
    FUNCTION_ENTRY("getCreatedDateTime");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_createDate;
}

std::string IncidentReportHelper::getModifier()
{
    FUNCTION_ENTRY("getModifier");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_modifier;
}

std::string IncidentReportHelper::getModifierProfile()
{
    FUNCTION_ENTRY("getModifierProfile");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_modifierProfile;
}

time_t IncidentReportHelper::getModifiedDateTime()
{
    FUNCTION_ENTRY("getModifiedDateTime");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_modifyDate;
}

void IncidentReportHelper::setFalseIncident(bool state)
{
    FUNCTION_ENTRY("setFalseIncident");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_falseIncident = state;

    FUNCTION_EXIT;
}

bool IncidentReportHelper::isFalseIncident()
{
    FUNCTION_ENTRY("isFalseIncident");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }
    FUNCTION_EXIT;

    return m_falseIncident;
}

void IncidentReportHelper::setAsset(AssetType* asset)
{
    FUNCTION_ENTRY("setAsset");
    TA_ASSERT(asset != NULL, "NULL asset");

    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_assetType.description = asset->description;
    m_assetType.id = asset->id;
    m_assetType.subsystem = asset->subsystem;

    m_entityId = asset->id;

    FUNCTION_EXIT;
}

AssetType IncidentReportHelper::getAsset()
{
    FUNCTION_ENTRY("getAsset");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }
    //TD15806
	if (!m_alarmId.empty())
	{
		loadAssetDetailsFromAlarm(m_alarmId); 
	}
    FUNCTION_EXIT;

    return m_assetType;
}

void IncidentReportHelper::setTrain(const std::string& train)
{
    FUNCTION_ENTRY("setTrain");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_train = train;

    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getTrain()
{
    FUNCTION_ENTRY("getTrain");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_train;
}

void IncidentReportHelper::setInnerTrack(bool state)
{
    FUNCTION_ENTRY("setInnerTrack");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_innerTrack = state;

    FUNCTION_EXIT;
}

bool IncidentReportHelper::getInnerTrack()
{
    FUNCTION_ENTRY("getInnerTrack");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_innerTrack;
}

void IncidentReportHelper::setOuterTrack(bool state)
{
    FUNCTION_ENTRY("setOuterTrack");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_outerTrack = state;

    FUNCTION_EXIT;
}

bool IncidentReportHelper::getOuterTrack()
{
    FUNCTION_ENTRY("getOuterTrack");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_outerTrack;
}

void IncidentReportHelper::setKilometreLoss(unsigned long kilometres)
{
    FUNCTION_ENTRY("setKilometreLoss");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_kmLoss = kilometres;
    FUNCTION_EXIT;
}

unsigned long IncidentReportHelper::getKilometreLoss()
{
    FUNCTION_ENTRY("getKilometreLoss");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_kmLoss;
}

void IncidentReportHelper::setLocation1(std::string location)
{
    FUNCTION_ENTRY("setLocation1");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_location1 = location;
    m_location1Changed = true;
    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getLocation1()
{
    FUNCTION_ENTRY("getLocation1");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_location1;
}

void IncidentReportHelper::setLocation2(std::string location)
{
    FUNCTION_ENTRY("setLocation2");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_location2 = location;
    m_location2Changed = true;
    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getLocation2()
{
    FUNCTION_ENTRY("getLocation2");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_location2;
}

void IncidentReportHelper::setStartDateTime(time_t dateTime)
{
    FUNCTION_ENTRY("setStartDateTime");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_startDate = dateTime;
    FUNCTION_EXIT;
}

time_t IncidentReportHelper::getStartDateTime()
{
    FUNCTION_ENTRY("getStartDateTime");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_startDate;
}

void IncidentReportHelper::setEndDateTime(time_t dateTime)
{
    FUNCTION_ENTRY("setEndDateTime");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_endDate = dateTime;
    FUNCTION_EXIT;
}

time_t IncidentReportHelper::getEndDateTime()
{
    FUNCTION_ENTRY("getEndDateTime");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_endDate;
}

void IncidentReportHelper::setInitialDelay(unsigned long delay)
{
    FUNCTION_ENTRY("setInitialDelay");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    m_initialDelay = delay;
}

unsigned long IncidentReportHelper::getInitialDelay()
{
    FUNCTION_ENTRY("getInitialDelay");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_initialDelay;
}

void IncidentReportHelper::setAccumulatedDelay(unsigned long delay)
{
    FUNCTION_ENTRY("setAccumulatedDelay");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    m_totalDelay = delay;
}

unsigned long IncidentReportHelper::getAccumulatedDelay()
{
    FUNCTION_ENTRY("getAccumulatedDelay");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_totalDelay;
}

std::string IncidentReportHelper::getAssociatedAlarm()
{
    FUNCTION_ENTRY("getAssociatedAlarm");
    if (!m_isValidSummaryData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_alarmId;
}

void IncidentReportHelper::setAdditionalAlarmDescription(const std::string& description)
{
    FUNCTION_ENTRY("setAdditionalAlarmDescription");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_alarmAdditinalDesc = description;

    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getAdditionalAlarmDescription()
{
    FUNCTION_ENTRY("getAdditionalAlarmDescription");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_alarmAdditinalDesc;
}

void IncidentReportHelper::setPlansExecuted(std::vector<PlanType> plans)
{
    FUNCTION_ENTRY("setPlansExecuted");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_plansExecuted = plans;
    m_plansChanged = true;

    FUNCTION_EXIT;
}

std::vector<PlanType> IncidentReportHelper::getPlansExecuted()
{
    FUNCTION_ENTRY("getPlansExecuted");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_plansExecuted;
}

void IncidentReportHelper::setOperators(const std::string& operators)
{
    FUNCTION_ENTRY("setOperators");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_operators = operators;

    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getOperators()
{
    FUNCTION_ENTRY("getOperators");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_operators;
}

void IncidentReportHelper::setComments(const std::string& comments)
{
    FUNCTION_ENTRY("setComments");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_comments = comments;

    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getComments()
{
    FUNCTION_ENTRY("getComments");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_comments;
}

void IncidentReportHelper::setRecommendation(const std::string& recommendation)
{
    FUNCTION_ENTRY("setRecommendation");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    m_recommendation = recommendation;

    FUNCTION_EXIT;
}

std::string IncidentReportHelper::getRecommendation()
{
    FUNCTION_ENTRY("getRecommendation");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_recommendation;
}

//Mintao ++
//Bug 652 (TD12313)
std::string IncidentReportHelper::getStationId()
{
    FUNCTION_ENTRY("getStationId");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_stationId;
}
//Mintao ++
//Bug 652 (TD12313)

unsigned long IncidentReportHelper::getCreateLocationId()
{
    FUNCTION_ENTRY("getCreateLocationId");
    if (!m_isValidData && !m_isNew)
    {
        reload();
    }

    FUNCTION_EXIT;
    return m_createLocationId;
}


// TD15865 Yanrong++
void IncidentReportHelper::updateIncidentTable( const std::string & column, const std::string & value )
{
	FUNCTION_ENTRY( "updateIncidentTable" );
	try
	{
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
		if (databaseConnection == NULL)
		{
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		//ostringstream sql;
		// update the report state base on the report id
		/*sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "update IN_INCIDENT_REPORT"
			<< " set " << column << " = " << value
			<< " where ININCI_ID = ''" << m_reportId << "''";
		sql << "');";
		sql << "END;";*/
		
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_UPDATE_52704, column, 
//		std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_UPDATE_52704, getLocalDatabaseName(), column, 
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_UPDATE_52704, getLocalDatabaseName(), column,
			value, m_reportId);
		databaseConnection->executeModification(strSql);
	}
	catch (DataException&) 
	{
		ostringstream reason;
		reason << "Cannot close report = " << m_reportId;			
		TA_THROW(IncidentException(reason.str(),IncidentException::DATABASE_ERROR));
	}
	FUNCTION_EXIT;
}


void IncidentReportHelper::increaseEditOperator()
{
	FUNCTION_ENTRY( "setState" );
	unsigned int eoc;
	if( getEditOperatorCount(eoc) )
	{
		++eoc;

		std::stringstream ss;
		ss << eoc;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15865 after decrease edit count string=%s", ss.str().c_str() );
		updateIncidentTable( "EDIT_OPERATOR_COUNT", ss.str() );
	}
	FUNCTION_EXIT;
}


void IncidentReportHelper::decreaseEditOperator()
{
	FUNCTION_ENTRY( "decreaseEditOperator" );
	// Only update saved record
	if( !m_isNew )
	{
		unsigned int eoc;
		if( getEditOperatorCount(eoc) )
		{
			if( eoc != 0 )
				--eoc;

			std::stringstream ss;
			ss << eoc;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TD15865 after decrease edit count string=%s", ss.str().c_str() );
			updateIncidentTable( "EDIT_OPERATOR_COUNT", ss.str() );
		}
	}
	FUNCTION_EXIT;
}


bool IncidentReportHelper::getEditOperatorCount(unsigned int& count)
{
	FUNCTION_ENTRY( "getEditOperatorCount" );

	count = 0;

	// get a connection to the database
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);
    if (databaseConnection == NULL)
    {
        TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
    }

	// constructor the SQL statement
	/*ostringstream sql;
	sql << "select EDIT_OPERATOR_COUNT from IN_INCIDENT_REPORT"
        << " where ININCI_ID = '" << m_reportId << "'";*/
//	std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),IN_INCIDENT_REPORT_STD_SELECT_52501, m_reportId);
//	std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_SELECT_52501, m_reportId);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_SELECT_52501, m_reportId);

	vector<string> columnNames;
	columnNames.push_back("EDIT_OPERATOR_COUNT");

	IData* data = databaseConnection->executeQuery(strSql,columnNames);
    if (data == NULL)
    {
        TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
    }

    if (0 == data->getNumRows()) 
    {
        // record not found
        delete data;
        data = NULL;

        ostringstream reasonMessage;
		reasonMessage << "No data found for incident Id = " << m_reportId;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "getEditOperatorCount, query data from database failed, %s", reasonMessage.str().c_str());

		return false;
    }
    
    count = data->getUnsignedLongData( 0, "EDIT_OPERATOR_COUNT" );

	delete data;
    data = NULL;
	
	FUNCTION_EXIT;
	return true;
}
// ++Yanrong

void IncidentReportHelper::writeData()
{
    FUNCTION_ENTRY("writeData");
    //Bugzilla Bug 150: Mintao
    //removed unnecessary code
    try
    {
        if (m_isNew) // This is a new record
        {
            addNewRecord();
        }
        else // This is an existing record
        {
            modifyExistingRecord();
        }

        // Now that the data has been written, what we hold is valid data.
        m_isValidData = true;
        m_isValidSummaryData = true;
        m_isNew = false;
        m_plansChanged = false;
    }
    catch (DatabaseException& e)
    {
        throw e;
    }

    FUNCTION_EXIT;
}


void IncidentReportHelper::reload()
{
    FUNCTION_ENTRY("reload");

    // Ensure the IncidentReport has already been written to the database. It doesn't make
    // any sense to reload an IncidentReport that doesn't exist in the database.
    TA_ASSERT(!m_isNew, "This IncidentReport does not yet exist in the database. Therefore it cannot be reloaded.");

    // get a connection to the database
    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);
    if (databaseConnection == NULL)
    {
        TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
    }

    // create the SQL string to retrieve the data of the IncidentReport
    // based upon the key
    // create the SQL string to retrieve the data of the IncidentReportData
    //ostringstream sql;
    //Bugzilla Bug 402: Mintao
    //breaking down the sql statement into a few section
	//sql << "select " << IN_TITLE_COL << ", "
 //       << IN_CREATE_OPERATOR_ID_COL << ", " 
 //       << IN_CREATE_OPERATOR_PROFILE_ID_COL << ", "
 //       << "TO_CHAR(" << IN_CREATE_DATE_COL << ",'YYYYMMDDHH24MISS'), "
 //       << "NVL(" << IN_MODIFY_OPERATOR_ID_COL << ",0) as MODIFY_OPERAT_ID , " 
 //       << "NVL(" << IN_MODIFY_OPERATOR_PROFILE_ID_COL << ",0) as MODIFY_SEPROF_ID , "
 //       << "TO_CHAR(" << IN_MODIFY_DATE_COL << ",'YYYYMMDDHH24MISS'), "
 //       << IN_FALSE_INCIDENT_COL << ", ";

 //   sql << "NVL(" << IN_ENTITY_ID_COL << ",0) as ENTITY_ID, "
 //       << IN_LOCATION_ID_1_COL << ", " 
 //       << IN_LOCATION_ID_2_COL << ", "
 //       << IN_LOCATION_NAME_1_COL << ", "
 //       << IN_LOCATION_NAME_2_COL << ", "
 //       << IN_TRAIN_ID_COL << ", " 
 //       << "TO_CHAR(nvl(" << IN_START_DATE_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
 //       << "TO_CHAR(nvl(" << IN_END_DATE_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
 //       << IN_INNER_TRACK_COL << ", " 
 //       << IN_OUTER_TRACK_COL << ", " ;

 //   sql << "NVL(" << IN_KM_LOSS_COL << ",0) as KM_LOSS, " 
 //       << "NVL(" << IN_INITIAL_DELAY_COL << ",0) as INITIAL_DELAY, " 
 //       << "NVL(" << IN_TOTAL_DELAY_COL << ",0) as TOTAL_DELAY, " 
 //       << IN_ALARM_ID_COL << ", " 
 //       << IN_ALARM_DESC_COL << ", " 
	//	<< IN_ALARM_VALUE_COL << ", "
 //       << IN_OPERATORS_COL << ", " 
 //       << IN_COMMENTS_COL << ", " 
 //       << IN_RECOMMENDATION_COL << ", ";
//Mintao ++
//Bug 652 (TD12313)

   /* sql << IN_STATION_ID_COL << ", "
        << "TO_CHAR(nvl(" << IN_CLOSE_DATE_COL << ",TO_DATE('12:00:00 01/01/1990','hh:mi:ss dd/mm/yyyy')),'YYYYMMDDHH24MISS'), "
        << IN_CREATE_LOCATION_ID_COL
        << " from IN_INCIDENT_REPORT inc, LOCATION loc1, LOCATION loc2"
        << " where " << IN_ININCI_ID_COL << " = '" << m_reportId << "'"
        <<      " AND inc.LOCATI_ID_1 = loc1.PKEY (+) AND inc.LOCATI_ID_2 = loc2.PKEY (+)";*/
//Mintao ++
//Bug 652 (TD12313)
//	std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_SELECT_52552, m_reportId);
//	std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_SELECT_52552, m_reportId);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_SELECT_52552, m_reportId);
    

    // Set up the columnNames vector to be passed to executeQuery()
    vector<string> columnNames;
	columnNames.push_back(IN_TITLE_COL);
	columnNames.push_back(IN_CREATE_OPERATOR_ID_COL);
	columnNames.push_back(IN_CREATE_OPERATOR_PROFILE_ID_COL);
	columnNames.push_back(IN_CREATE_DATE_COL);
	columnNames.push_back(IN_MODIFY_OPERATOR_ID_COL);
	columnNames.push_back(IN_MODIFY_OPERATOR_PROFILE_ID_COL);
	columnNames.push_back(IN_MODIFY_DATE_COL);

    columnNames.push_back(IN_FALSE_INCIDENT_COL);
    columnNames.push_back(IN_ENTITY_ID_COL);
    columnNames.push_back(IN_LOCATION_ID_1_COL);
    columnNames.push_back(IN_LOCATION_ID_2_COL);
    columnNames.push_back(IN_LOCATION_NAME_1_COL);
    columnNames.push_back(IN_LOCATION_NAME_2_COL);
    columnNames.push_back(IN_TRAIN_ID_COL);
    columnNames.push_back(IN_START_DATE_COL);
    columnNames.push_back(IN_END_DATE_COL);
    columnNames.push_back(IN_INNER_TRACK_COL);
    columnNames.push_back(IN_OUTER_TRACK_COL);
    columnNames.push_back(IN_KM_LOSS_COL);
    columnNames.push_back(IN_INITIAL_DELAY_COL);
    columnNames.push_back(IN_TOTAL_DELAY_COL);
    columnNames.push_back(IN_ALARM_ID_COL);
    columnNames.push_back(IN_ALARM_DESC_COL);
	columnNames.push_back(IN_ALARM_VALUE_COL);
    columnNames.push_back(IN_OPERATORS_COL);
    columnNames.push_back(IN_COMMENTS_COL);
    columnNames.push_back(IN_RECOMMENDATION_COL);
    columnNames.push_back(IN_STATION_ID_COL);
    columnNames.push_back(IN_CLOSE_DATE_COL);
    columnNames.push_back(IN_CREATE_LOCATION_ID_COL);

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    IData* data = databaseConnection->executeQuery(strSql,columnNames);
    if (data == NULL)
    {
        TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
    }

    if (0 == data->getNumRows()) 
    {
        // record not found
        
        delete data;
        data = NULL;

        ostringstream reasonMessage;
		reasonMessage << "No data found for incident Id = " << m_reportId;
        TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"Incident Id" ) );
    }
    
    // Assign the data as appropriate to the member variables.
    // These calls can throw DataExceptions, as documented in the comment of this method.
    m_title = data->getStringData(0, IN_TITLE_COL);
    m_createOprId = data->getUnsignedLongData(0, IN_CREATE_OPERATOR_ID_COL);
    m_createProfileOprId = data->getUnsignedLongData(0, IN_CREATE_OPERATOR_PROFILE_ID_COL);

	//TD15125 - catch DataExceptions thrown when operatorIDs could not be found in the
	// database
	IOperator* operatorData = NULL;
	try
	{
		//Bugzilla Bug 165: Mintao
		operatorData = OperatorAccessFactory::getInstance().getOperator(m_createOprId, false);
		m_creator = operatorData->getName();
		//Bugzilla Bug 151: Mintao	
		
	}
	catch(DataException&)
	{
		// Must empty the string if the operatorData could not be found or 
		m_creator = "";
	}
	delete operatorData;
	operatorData = NULL;
	
	
	//TD15125 - catch DataExceptions thrown when profileIDs could not be found in the
	// database
	IProfile* profileData = NULL;
	try
	{
		profileData = ProfileAccessFactory::getInstance().getProfile(m_createProfileOprId);
		//Bugzilla Bug 165: Mintao
		if (profileData != NULL)
		{
			m_creatorProfile = profileData->getName();
		}
		//Bugzilla Bug 148: Mintao
		
	}
	catch(DataException&)
	{	// current profile doesn't exist
		m_creatorProfile = ""; 		

	}
	delete profileData;
    profileData = NULL;

    getDateData(data, 0, IN_CREATE_DATE_COL, m_createDate);

    m_modifyOprId = data->getUnsignedLongData(0, IN_MODIFY_OPERATOR_ID_COL);
    m_modifyProfileOprId = data->getUnsignedLongData(0, IN_MODIFY_OPERATOR_PROFILE_ID_COL);
    
    //Mintao ++
    //Bug 647
    if (m_modifyOprId == 0 && m_modifyProfileOprId == 0)
    {
        m_modifier = "";
    }
    else
    {
		
		//TD15125 - catch DataExceptions thrown when operatorIDs could not be found in the
		// database
		try
		{
			operatorData = OperatorAccessFactory::getInstance().getOperator(m_modifyOprId, false);
			if (operatorData != NULL)
				m_modifier = operatorData->getName();			
		}
		catch(DataException&)
		{
			 m_modifier = "";
		}
    }
    //Mintao ++
    //Bug 647

    //Bugzilla Bug 151: Mintao
    delete operatorData;
    operatorData = NULL;

	
	//TD15125 - catch DataExceptions thrown when profileIDs could not be found in the
	// database
	try
	{
		profileData = ProfileAccessFactory::getInstance().getProfile(m_modifyProfileOprId);
		if (profileData != NULL)
			m_modifierProfile = profileData->getName();		
	}
	catch(DataException&)
	{
		 m_modifierProfile = "";
	}

    //Bugzilla Bug 148: Mintao
    delete profileData;
    profileData = NULL;

    getDateData(data, 0, IN_MODIFY_DATE_COL, m_modifyDate);

    m_falseIncident = data->getBooleanData(0, IN_FALSE_INCIDENT_COL);

    m_entityId = data->getUnsignedLongData(0, IN_ENTITY_ID_COL);

    if (data->isNull(0, IN_LOCATION_ID_1_COL))
    {
        m_location1 = "";
        m_locationId1 = 0;
    }
    else
    {
        m_locationId1 = data->getUnsignedLongData(0, IN_LOCATION_ID_1_COL);
        m_location1 = data->getStringData(0, IN_LOCATION_NAME_1_COL);
    }

    // if (data->isNull(0, IN_LOCATION_ID_1_COL)) -- //TD14370
	if (data->isNull(0, IN_LOCATION_ID_2_COL))
    {
        m_location2 = "";
        m_locationId2 = 0;
    }
    else
    {
        m_locationId2 = data->getUnsignedLongData(0, IN_LOCATION_ID_2_COL);
        m_location2 = data->getStringData(0, IN_LOCATION_NAME_2_COL);
    }

    if (data->isNull(0, IN_TRAIN_ID_COL))
    {
        m_train = "";
    }
    else
    {
        m_train = data->getStringData(0, IN_TRAIN_ID_COL);
    }

    getDateData(data, 0, IN_START_DATE_COL, m_startDate);
    getDateData(data, 0, IN_END_DATE_COL, m_endDate);

    m_innerTrack = data->getBooleanData(0, IN_INNER_TRACK_COL);
    m_outerTrack = data->getBooleanData(0, IN_OUTER_TRACK_COL);
    m_kmLoss = data->getUnsignedLongData(0, IN_KM_LOSS_COL);
    m_initialDelay = data->getUnsignedLongData(0, IN_INITIAL_DELAY_COL);
    m_totalDelay = data->getUnsignedLongData(0, IN_TOTAL_DELAY_COL);

    m_alarmId = data->getStringData(0, IN_ALARM_ID_COL);
    m_alarmAdditinalDesc = data->getStringData(0, IN_ALARM_DESC_COL);
	m_alarmValueFromCmdline = data->getStringData(0, IN_ALARM_VALUE_COL);

    m_operators = data->getStringData(0, IN_OPERATORS_COL);
    m_comments = data->getStringData(0, IN_COMMENTS_COL);
    m_recommendation = data->getStringData(0, IN_RECOMMENDATION_COL);
//Mintao ++
//Bug 652 (TD12313)

    m_stationId = data->getStringData(0, IN_STATION_ID_COL);
//Mintao ++
//Bug 652 (TD12313)

    getDateData(data, 0, IN_CLOSE_DATE_COL, m_closeDate);

    m_createLocationId = data->getUnsignedLongData(0, IN_CREATE_LOCATION_ID_COL);

    // Now that we're done with the IData object, it's our responsibility to delete it
    delete data;
    data = NULL;

	// get asset details for this incident
    loadAssetDetails(databaseConnection);

    // get plans executed for this incident
    loadPlans(databaseConnection);
    
    // Need to record that we now have valid data
    m_isValidData = true;
    m_isValidSummaryData = true;

    FUNCTION_EXIT;
}

void IncidentReportHelper::loadPlans(TA_Base_Core::IDatabase* database)
{
    FUNCTION_ENTRY("loadPlans");

    m_plansExecuted.clear();

    // create the SQL string to retrieve the data of the IncidentReport
    // based upon the key
    // create the SQL string to retrieve the data of the IncidentReportData
  /*  ostringstream sql;

	sql << "select " << IN_PLAN_ID_COL 
        << " from IN_INCIDENT_REPORT inc, IN_PLAN plan"
        << " where inc.ININCI_ID = plan.ININCI_ID"
        << " and plan.ININCI_ID = '" << m_reportId << "'";*///Bugzilla Bug 168 and 171 Mintao
//	std::string strSql = defPrepareSQLStatement(database->GetDbServerType(), IN_INCIDENT_REPORT_STD_SELECT_52502, m_reportId);
//	std::string strSql  = database->prepareSQLStatement(IN_INCIDENT_REPORT_SELECT_52502, m_reportId);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_SELECT_52502, m_reportId);
    
    // Set up the columnNames vector to be passed to executeQuery()
    vector<string> columnNames;
	columnNames.push_back(IN_PLAN_ID_COL);

    // Execute the query. The method can throw a DatabaseException.
    // This is documented in the comment of this method.
    // We are responsible for deleting the returned IData object when we're done with it
    IData* data = database->executeQuery(strSql,columnNames);
    if (data == NULL)
    {
        TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
    }

    do
    {
        for (unsigned long i = 0; i < data->getNumRows() ; ++i )
        {
            PlanType pType;
            pType = data->getStringData(i, IN_PLAN_ID_COL);
            m_plansExecuted.push_back(pType);
        }

        delete data;
        data = NULL;
    }while( database->moreData(data) );


    FUNCTION_EXIT;
}

void IncidentReportHelper::savePlans(TA_Base_Core::IDatabase* database)
{
    FUNCTION_ENTRY("savePlans");
	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"ReportId: %s, AlarmId: %s, m_plansChanged = %d", 
			m_reportId.c_str(), m_alarmId.c_str(), m_plansChanged);

    if (!m_plansChanged)
    {
        return;
    }

    //ostringstream sql;

    // remove existing association first
  /*  sql << "delete IN_PLAN where ININCI_ID='" << m_reportId <<"'";*/
	//int nDbServerType = database->GetDbServerType();
	//std::string strSql = defPrepareSQLStatement(nDbServerType, IN_PLAN_STD_DELETE_53901, m_reportId);
//	std::string strSql = database->prepareSQLStatement(IN_PLAN_DELETE_53901, m_reportId);
		TA_Base_Core::SQLStatement strSql;
		database->prepareSQLStatement(strSql, IN_PLAN_DELETE_53951, getLocalDatabaseName(), m_reportId);
		database->executeModification(strSql);

    // insert each record
    for (vector<PlanType>::const_iterator it = m_plansExecuted.begin(); it != m_plansExecuted.end(); ++it)
    {
//Mintao ++
//Bug 652 (TD12313)
        //sql.str("");
        std::string planid = TA_Base_Core::TA_Uuid::generateUUID();
       /* sql << "insert into IN_PLAN (INPLAN_ID, ININCI_ID, PLAN_ID) values ("
            << "'" << planid << "','" 
            << m_reportId << "', '" << (*it) << "')";*/
		//strSql = defPrepareSQLStatement(nDbServerType, IN_PLAN_STD_INSERT_53801, planid, m_reportId, (*it));
//		strSql = database->prepareSQLStatement(IN_PLAN_INSERT_53801, planid, m_reportId, (*it));

		database->prepareSQLStatement(strSql, IN_PLAN_INSERT_53801, getLocalDatabaseName(), planid, m_reportId, (*it));
//Mintao ++
//Bug 652 (TD12313)
// 		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
// 			"ReportId: %s, AlarmId: %s, saveplan sql: %s", 
// 			m_reportId.c_str(), m_alarmId.c_str(), strSql.c_str());
			

		std::string strExSql;
		int nExDbType= database->getDbServerType();
		switch (nExDbType)
		{
		case TA_Base_Core::DbServerType::enumOracleDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strOracle : strSql.strCommon);
			break;
		case TA_Base_Core::DbServerType::enumMysqlDb:
			strExSql = (strSql.strCommon.empty() ? strSql.strMySQL : strSql.strCommon);
			break;		
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"ReportId: %s, AlarmId: %s, saveplan sql: %s", 
			m_reportId.c_str(), m_alarmId.c_str(), strExSql.c_str());	
			
		
        database->executeModification(strSql);
    }


    FUNCTION_EXIT;
}


void IncidentReportHelper::getDateData(TA_Base_Core::IData* data, const unsigned long rowIndex, const std::string& fieldName, time_t& out)
{
    FUNCTION_ENTRY("getDateData");

    if (data->isNull(rowIndex, fieldName))
    {
        out = 0;
    }
    else
    {
        // should be in YYYYMMDDHH24MISS format
        string dateStr = data->getStringData(rowIndex, fieldName);
        if (dateStr.empty())
        {
            out = 0;
        }
        else
        {
            TA_ASSERT(dateStr.length() == 14, "Wrong SQL date format");
            struct tm outtm;
            outtm.tm_year = atoi(dateStr.substr(0, 4).c_str()) - 1900;
            outtm.tm_mon = atoi(dateStr.substr(4, 2).c_str()) - 1;
            outtm.tm_mday = atoi(dateStr.substr(6, 2).c_str());
            outtm.tm_hour = atoi(dateStr.substr(8, 2).c_str());
            outtm.tm_min = atoi(dateStr.substr(10, 2).c_str());
            outtm.tm_sec = atoi(dateStr.substr(12, 2).c_str());
            out = mktime(&outtm);

            struct tm newtm;

            // Set up the date created structure to equal the value we set it to when we get NULL
            // from the database
		    newtm.tm_sec = 0;		//seconds
		    newtm.tm_min = 0;		// minutes 
		    newtm.tm_hour = 12;		// hours 
		    newtm.tm_mday = 1;		// day of the month 
		    newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
		    newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
									    // between 1901 and 2038
		    newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

		    time_t value;
		    value = mktime(&newtm);

            if (value == out) // The date was actually NULL
            {
                out = 0;
            }
        }
    }

    FUNCTION_EXIT;
}

//Mintao ++
//Bug 652 (TD12313)
void IncidentReportHelper::addNewRecord()
{
    FUNCTION_ENTRY("addNewRecord");
    
    //make sure the new report will have a uuid with it
    if (m_reportId.empty())
    {
		//TD15204 
        // m_reportId = TA_Base_Core::TA_Uuid::generateUUID();
		m_reportId = retrieveReportID();
    }
    
	if (!m_reportId.empty())
	{
		IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
		if (databaseConnection == NULL)
		{
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		string locationId1, locationId2;
		getForeignKeyValues(databaseConnection, locationId1, locationId2);
		
		m_createDate = time(NULL);
		string trainId;
		if (m_train.empty())
		{
			trainId = "";
		}
		else
		{
			trainId = m_train;
		}
		
		//ostringstream sql;
		
		string staionid;
		if (m_stationId.empty())
		{
			staionid = "";
		}
		else
		{
			staionid = m_stationId;
		}
		
		//breaking down the sql statement into a few section
		/*sql << "BEGIN ";
    		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "insert into IN_INCIDENT_REPORT ("
			<< "ININCI_ID, "
			<< "TITLE, "
			<< "CREATE_OPERAT_ID, "
			<< "CREATE_SEPROF_ID, "
			<< "CREATE_DATE, "
			<< "MODIFY_OPERAT_ID, "
			<< "MODIFY_SEPROF_ID, "
			<< "MODIFY_DATE, "
			<< "FALSE_INCIDENT, "
			<< "ENTITY_ID, "
			<< "LOCATI_ID_1, "
			<< "LOCATI_ID_2, "
			<< "TRAIN_ID, ";
		
		sql << "START_DATE, "
			<< "END_DATE, "
			<< "INNER_TRACK, "
			<< "OUTER_TRACK, "
			<< "KM_LOSS, "
			<< "INITIAL_DELAY, "
			<< "TOTAL_DELAY, "
			<< "ALARM_ID, "
			<< "ALARM_DESC, "
			<< "ALARM_VALUE, "
			<< "OPERATORS, "
			<< "COMMENTS, "
			<< "RECOMMENDATION, "
			<< "INSTAT_ID, "
			<< "CLOSE_DATE, "*/
			// TD15865 Yanrong++
			//<< "EDIT_OPERATOR_COUNT, "
			// ++Yanrong
			//<< "CREATE_LOCATI_ID";
		
		//sql << ") values (''"
		//	<< m_reportId << "'', " 
		//	<< "''" << databaseConnection->escapeAQSQLString(m_title) << "'', "        
		//	<< m_createOprId << ","
		//	<< m_createProfileOprId << ","
		//	// << getUpdateDateSql(m_createDate) << ", " // TD14224 - lizettejl - correct fix
		//	<< databaseConnection->escapeQueryString(getUpdateDateSql(m_createDate)) << ", "
		//	<< "NULL,"
		//	<< "NULL,"
		//	<< "NULL,"
		//	<< m_falseIncident << ", "
		//	<< m_entityId << ", "
		//	<< "''" << locationId1 << "'', "
		//	<< "''" << locationId2 << "'', "
		//	<< "''" << trainId << "''" <<", "
		//	<< databaseConnection->escapeQueryString(getUpdateDateSql(m_startDate)) << ","
		//	<< databaseConnection->escapeQueryString(getUpdateDateSql(m_endDate)) << ",";
		//
		//sql << m_innerTrack << ", "
		//	<< m_outerTrack << ", "
		//	<< m_kmLoss << ", "
		//	<< m_initialDelay << ", "
		//	<< m_totalDelay << ", "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_alarmId )<< "'', "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_alarmAdditinalDesc) << "'', "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_alarmValueFromCmdline) << "'', "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_operators) << "'', "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_comments) << "'', "
		//	<< "''" << databaseConnection->escapeAQSQLString(m_recommendation) << "'', ''"
		//	<< staionid << "'', "
		//	<< "NULL,"
			// TD15865 Yanrong - this is a new report(someone is editing it) so the value must be 1.
			//<< "1 ,"
			// ++Yanrong
		/*	<< m_createLocationId
			<< ")";        
		sql << "');";
		sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_INSERT_52851,getLocalDatabaseName(),
//		std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_INSERT_52851,getLocalDatabaseName(),
		TA_Base_Core::SQLTypeAdapter sqlParamCreateDate, sqlParamStartDate, sqlParamEndDate;
		getUpdateDateSql(sqlParamCreateDate, m_createDate, true);
		getUpdateDateSql(sqlParamStartDate, m_startDate, true);
		getUpdateDateSql(sqlParamEndDate, m_endDate, true);

		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_INSERT_52851,getLocalDatabaseName(),
			m_reportId, databaseConnection->escapeAQSQLString(m_title),m_createOprId,m_createProfileOprId, sqlParamCreateDate,
			m_falseIncident,m_entityId,locationId1,locationId2,trainId, sqlParamStartDate,
			sqlParamEndDate, m_innerTrack,m_outerTrack,m_kmLoss,m_initialDelay,m_totalDelay,
			databaseConnection->escapeAQSQLString(m_alarmId ),databaseConnection->escapeAQSQLString(m_alarmAdditinalDesc),
			databaseConnection->escapeAQSQLString(m_alarmValueFromCmdline) , databaseConnection->escapeAQSQLString(m_operators),
			databaseConnection->escapeAQSQLString(m_comments),databaseConnection->escapeAQSQLString(m_recommendation),staionid,m_createLocationId);
		// TD15204 -- make sure the insert is successful before incrementing the next value counter
		try
		{		
			databaseConnection->executeModification(strSql);
			
			savePlans(databaseConnection);
		}
		catch (...) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Failure in inserting incident report.");
		}		
		// updateReportIDCounter(); 
		//++lizettejl
	}
	
    FUNCTION_EXIT;
}

void IncidentReportHelper::modifyExistingRecord()
{
    FUNCTION_ENTRY("modifyExistingRecord");

    // sets the modifier profile and modify date
    updateModifyData();

    IDatabase* databaseConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
    if (databaseConnection == NULL)
    {
        TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
    }

    string locationId1, locationId2;
    getForeignKeyValues(databaseConnection, locationId1, locationId2);

    time_t modifyDate = time(NULL);
    string trainId;
    if (m_train.empty())
    {
        trainId = "";
    }
    else
    {
        trainId = m_train;
    }

    string staionid;
//Mintao ++
//Bug 652 (TD12313)
    if (m_stationId.empty())
    {
        staionid = "";
    }
    else
    {
        staionid = m_stationId;
    }

    //std::ostringstream sql;

    //breaking down the sql statement into a few section
   /* sql << "BEGIN ";
    sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
    sql << "update IN_INCIDENT_REPORT set " 
        << "TITLE=''" << databaseConnection->escapeAQSQLString(m_title) << "'',"
        << "MODIFY_OPERAT_ID=" << m_modifyOprId << ","
        << "MODIFY_SEPROF_ID=" << m_modifyProfileOprId << ","
        << "MODIFY_DATE=" << databaseConnection->escapeQueryString(getUpdateDateSql(modifyDate)) << ","
        << "FALSE_INCIDENT=" << m_falseIncident << ","
        << "ENTITY_ID=" << m_entityId << ",";
        
    sql << "LOCATI_ID_1=''" << locationId1 << "'',"
        << "LOCATI_ID_2=''" << locationId2 << "'',"
        << "TRAIN_ID=" << "''" << trainId << "''" << ","
        << "START_DATE=" << databaseConnection->escapeQueryString(getUpdateDateSql(m_startDate)) << ","
        << "END_DATE=" << databaseConnection->escapeQueryString(getUpdateDateSql(m_endDate)) << ","
        << "INNER_TRACK=" << m_innerTrack << ","
        << "OUTER_TRACK=" << m_outerTrack << ","
        << "KM_LOSS=" << m_kmLoss << ",";

    sql << "INITIAL_DELAY=" << m_initialDelay << ","
        << "TOTAL_DELAY=" << m_totalDelay << ","
        << "ALARM_ID=''" << databaseConnection->escapeAQSQLString(m_alarmId) << "'',"
        << "ALARM_DESC=''" << databaseConnection->escapeAQSQLString(m_alarmAdditinalDesc) << "'',"
        << "ALARM_VALUE=''" << databaseConnection->escapeAQSQLString(m_alarmValueFromCmdline) << "'',"
        << "OPERATORS=''" << databaseConnection->escapeAQSQLString(m_operators) << "'',"
        << "COMMENTS=''" << databaseConnection->escapeAQSQLString(m_comments) << "'',"
        << "RECOMMENDATION=''" << databaseConnection->escapeAQSQLString(m_recommendation) << "'',";
    
    sql << "INSTAT_ID=''" << staionid << "'',"
        << "CLOSE_DATE=" << databaseConnection->escapeQueryString(getUpdateDateSql(m_closeDate))
        << " where " 
        << "ININCI_ID=''" << m_reportId << "''";
    sql << "');";
    sql << "END;";*/


//	std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(),IN_INCIDENT_REPORT_Oracle_UPDATE_52705,getLocalDatabaseName(),
//	std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_UPDATE_52705,getLocalDatabaseName(),
	TA_Base_Core::SQLTypeAdapter sqlParamModifyDate, sqlParamStartDate, sqlParamEndDate, sqlParamCloseDate;
	getUpdateDateSql(sqlParamModifyDate, modifyDate, true);
	getUpdateDateSql(sqlParamStartDate, m_startDate, true);
	getUpdateDateSql(sqlParamEndDate, m_endDate, true);
	getUpdateDateSql(sqlParamCloseDate, m_closeDate, true);

	TA_Base_Core::SQLStatement strSql;
	databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_UPDATE_52705,getLocalDatabaseName(),
		databaseConnection->escapeAQSQLString(m_title),m_modifyOprId,m_modifyProfileOprId, sqlParamModifyDate,
		m_falseIncident,m_entityId,locationId1,locationId2,trainId, sqlParamStartDate,
		sqlParamEndDate,m_innerTrack,m_outerTrack,m_kmLoss,m_initialDelay,m_totalDelay,
		databaseConnection->escapeAQSQLString(m_alarmId),databaseConnection->escapeAQSQLString(m_alarmAdditinalDesc),databaseConnection->escapeAQSQLString(m_alarmValueFromCmdline),
		databaseConnection->escapeAQSQLString(m_operators),databaseConnection->escapeAQSQLString(m_comments),databaseConnection->escapeAQSQLString(m_recommendation),
		staionid,sqlParamCloseDate,m_reportId);

    databaseConnection->executeModification(strSql);
    m_modifyDate = modifyDate;

    savePlans(databaseConnection);


    FUNCTION_EXIT;
}

void IncidentReportHelper::initialize()
{
    FUNCTION_ENTRY("initialize");

    m_reportId = "";
    m_createOprId = 0;
    m_createProfileOprId = 0;
    m_createDate = 0;
    m_modifyOprId = 0;
    m_modifyProfileOprId = 0;
    m_modifyDate = 0;
    m_falseIncident = false;
    m_entityId = 0;

    m_locationId1 = 0;
    m_locationId2 = 0;
    m_train = "";

    m_startDate = 0;
    m_endDate = 0;
    m_innerTrack = false;
    m_outerTrack  = false;
    m_kmLoss = 0;
    m_initialDelay = 0;
    m_totalDelay = 0;
    m_stationId = "";
    m_closeDate = 0;
    m_createLocationId = 0;

    m_assetType.description = "";
    m_assetType.subsystem = "";
    m_assetType.id = 0;

    m_location1Changed = false;
    m_location2Changed = false;

    FUNCTION_EXIT;
}

void IncidentReportHelper::getForeignKeyValues(TA_Base_Core::IDatabase* database, std::string& locationId1, std::string& locationId2)
{
    FUNCTION_ENTRY("getForeignKeyValues");

    ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
    vector<string> columnNames;
    columnNames.push_back("PKEY");

    if (m_location1Changed)
    {
        m_locationId1 = 0;
        if (!m_location1.empty())
        {
            /*sql << "SELECT PKEY from LOCATION"
                << " where NAME='" << database->escapeQueryString(m_location1) << "'";*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), LOCATION_STD_SELECT_7005,database->escapeQueryString(m_location1));
//			strSql  = database->prepareSQLStatement(LOCATION_SELECT_7005,database->escapeQueryString(m_location1));

		database->prepareSQLStatement(strSql, LOCATION_SELECT_7005,database->escapeQueryString(m_location1));
            IData* data = database->executeQuery(strSql, columnNames);
    
            if (data == NULL)
            {
                TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
            }

            try 
            {
                m_locationId1 = data->getUnsignedLongData(0, columnNames[0]);

                // Clean up
                //Bugzilla Bug 182: Mintao
                delete data;
                data = NULL;
            }
            catch (DataException& e) 
            {
                // Invalid location name, Clean up before rethrowing
                //Bugzilla Bug 182: Mintao
                delete data;
                data = NULL;
                throw e;
            }
        }

        m_location1Changed = false;
    }

    if (m_location2Changed)
    {
        m_locationId2 = 0;
        if (!m_location2.empty())
        {
           /* sql.str("");
            sql << "SELECT PKEY from LOCATION"
                << " where NAME='" << database->escapeQueryString(m_location2) << "'";*/
//			strSql = defPrepareSQLStatement(database->GetDbServerType(), LOCATION_STD_SELECT_7005,database->escapeQueryString(m_location2));
//			strSql  = database->prepareSQLStatement(LOCATION_SELECT_7005,database->escapeQueryString(m_location2));

		database->prepareSQLStatement(strSql, LOCATION_SELECT_7005,database->escapeQueryString(m_location2));
            IData* data = database->executeQuery(strSql, columnNames);
    
            if (data == NULL)
            {
                TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
            }

            try 
            {
                m_locationId2 = data->getUnsignedLongData(0, columnNames[0]);

                // Clean up
                //Bugzilla Bug 182: Mintao
                delete data;
                data = NULL;
            }
            catch (DataException& e) 
            {
                // Invalid location name, Clean up before rethrowing
                //Bugzilla Bug 182: Mintao
                delete data;
                data = NULL;
                throw e;
            }
        }

        m_location2Changed = false;
    }

    if (m_location1.empty())
    {
         locationId1 = "NULL"; // TD14370
		//locationId1 = "";
    }
    else
    {
        sql.str("");
        sql << m_locationId1;
        locationId1 = sql.str();
    }

    if (m_location2.empty())
    {
        locationId2 = "NULL"; // TD14370
		//locationId2 = "";
    }
    else
    {
        sql.str("");
        sql << m_locationId2;
        locationId2 = sql.str();
    }

    FUNCTION_EXIT;
	}


	void IncidentReportHelper::getUpdateDateSql(TA_Base_Core::SQLTypeAdapter& sqlParam, time_t datetime, bool bIsAQ)
	{
		FUNCTION_ENTRY("getUpdateDateSql");

		if (datetime == 0 )
		{
			FUNCTION_EXIT;
			sqlParam.add(std::string("NULL"));
			return ;
		}
		
		//int nDbType = pDbConnection->getDbServerType();
		struct tm dateStruct;
		ostringstream oStream;

		if (ACE_OS::localtime_r(&datetime, &dateStruct) != NULL)
		{
			//switch ( nDbType )
			//{
			//case enumOracleDb:
			//// TD14224 - lizettejl - retract changes / wrong fix
			//oStream << "TO_DATE('"
			//		<< setfill('0')
			//		<< std::setw(2) << dateStruct.tm_mday
			//		<< std::setw(2) << dateStruct.tm_mon + 1
			//		<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
			//		<< std::setw(2) << dateStruct.tm_hour
			//		<< std::setw(2) << dateStruct.tm_min 
			//		<< std::setw(2) << dateStruct.tm_sec
			//		<< "','DDMMYYYYHH24MISS')";
			//	break;
			//case enumMysqlDb:
			//	oStream << "STR_TO_DATE('"
			//		<< setfill('0')
			//		<< std::setw(2) << dateStruct.tm_mday
			//		<< std::setw(2) << dateStruct.tm_mon + 1
			//		<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
			//		<< std::setw(2) << dateStruct.tm_hour
			//		<< std::setw(2) << dateStruct.tm_min 
			//		<< std::setw(2) << dateStruct.tm_sec
			//		<<"','%d%m%Y%H%i%S')";
			//	break;
			//}

			oStream << setfill('0')
				<< std::setw(2) << dateStruct.tm_mday
				<< std::setw(2) << dateStruct.tm_mon + 1
				<< std::setw(4) << dateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
				<< std::setw(2) << dateStruct.tm_hour
				<< std::setw(2) << dateStruct.tm_min 
				<< std::setw(2) << dateStruct.tm_sec;			
		}
		sqlParam.add(enumDateFormat_3, enumFuncFormat_2, oStream.str(), bIsAQ);

		FUNCTION_EXIT;		
		//return oStream.str();
	}


	void IncidentReportHelper::updateModifyData()
	{
		FUNCTION_ENTRY("updateModifyData");

		// get modifier profile
		string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
		//Bugzilla Bug 149: Mintao
		ISession* session = NULL;
		//Bugzilla Bug 148: Mintao
		IProfile* profileData = NULL;
		try
		{
			session = SessionAccessFactory::getInstance().getSession(sessionId);

			m_modifyOprId = session->getOperatorKey();
			m_modifier = session->getOperatorName();
			m_modifyProfileOprId = session->getProfileKey();

			//Bugzilla Bug 149: Mintao
			delete session;
			session = NULL;

			profileData = ProfileAccessFactory::getInstance().getProfile(m_modifyProfileOprId);
			m_modifierProfile = profileData->getName();
			//Bugzilla Bug 148: Mintao
			delete profileData;
			profileData = NULL;
		}
		catch (exception&)
		{
			//Bugzilla Bug 149: Mintao
			delete session;
			session = NULL;

			//Bugzilla Bug 148: Mintao
			delete profileData;
			profileData = NULL;
		}

		m_modifyDate = time(NULL);

		FUNCTION_EXIT;
	}

	unsigned long IncidentReportHelper::getCreatorProfileId()
	{
		FUNCTION_ENTRY("getCreatorProfileId");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}

		FUNCTION_EXIT;
		return m_createProfileOprId;
	}

	//TD15204
	std::string IncidentReportHelper::retrieveReportID()
	{
		FUNCTION_ENTRY("retrieveReportID");
		
		if (m_dbConnection == NULL)
			m_dbConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Common_Ad, TA_Base_Core::Write);
		else
			m_dbConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Common_Ad, TA_Base_Core::Write, m_dbConnection);

		if (m_dbConnection == NULL)
		{
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		std::string strReportID;
		/*ostringstream sql;

		sql.str("");
		sql << "select NEXT from CO_COUNTER"
			<< " where COCOUN_ID = " 
			<< INCIDENT_COUNTTYPE 
			<< " and LOCATION_ID = " << getConsoleLocationKey()
			<< " FOR UPDATE OF NEXT";*/
//		std::string strSql = defPrepareSQLStatement(m_dbConnection->GetDbServerType(), CO_COUNTER_STD_SELECT_54501, 
//		std::string strSql  = m_dbConnection->prepareSQLStatement(CO_COUNTER_SELECT_54501, 
		TA_Base_Core::SQLStatement strSql;
		m_dbConnection->prepareSQLStatement(strSql, CO_COUNTER_SELECT_54501,
			INCIDENT_COUNTTYPE, getConsoleLocationKey());

		vector<string> columnNames;
		columnNames.push_back("NEXT");

		try
		{
			IData* data = m_dbConnection->executeQuery(strSql, columnNames);
			if (data == NULL)
			{
				TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
			}
			if (0 == data->getNumRows()) 
			{
				// record not found				
				delete data;
				data = NULL;
				
				ostringstream reasonMessage;
				reasonMessage << "NO next counter value retrieved ";
				TA_THROW( DataException(reasonMessage.str().c_str(),DataException::NO_VALUE,"Next Counter Value" ) );
			}
			m_reportIdCounter = data->getUnsignedLongData(0, "NEXT");
			ostringstream reportId;
			// TD15204 - reformat ID syntax
			char reportIdBuffer[20]; // maxim // increase the buffer to allow automatic NULL termination during sprintf
			//sprintf(reportIdBuffer,"%05d",m_reportIdCounter);			
			// reportId <<  m_reportIdCounter  << "-" << getConsoleLocationName() << "-" << getUpdateDate();
			//++lizettejl

			//++shiva
			//changed to 7 digit counter value
			sprintf(reportIdBuffer,"%07d",m_reportIdCounter);
			///

			///++shiva
			///Changed to save the new incident id to 7 digit only
			/// Assumed incident report will be created only in OCC location.
			/// So only one location and not possible to have same id for different location suitation.
			reportId <<  reportIdBuffer;  //<< "-" << getConsoleLocationName() << "-" << getUpdateDate();
			/// To fix print 
			/// ++shiva

			strReportID = reportId.str();
			updateReportIDCounter();  
		}
		catch(DataException&)
		{
			strReportID = "";
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Failed to retrieve report ID");
		}
		catch (TransactiveException& ex) 
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					"Transactive Exception: %s caught in IncidentReportHelper::retrieveReportID()!", ex.what());    
		}	
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown Exception");
		}

		FUNCTION_EXIT;
		return strReportID;
	}

	//TD15204
	void IncidentReportHelper::updateReportIDCounter()
	{
		// if (m_reportIdCounter == TA_Base_Core::MAX_COUNTERVAL)
		 if (m_reportIdCounter == MAX_INCIDENTCOUNTER)
			m_reportIdCounter = 1;
		else
			m_reportIdCounter++;
		
		//m_dbConnection must be initialized when retrieving for a reportID
		// m_dbConnection = DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Common_Ad, TA_Base_Core::Write,m_dbConnection);

		if (m_dbConnection == NULL)
		{
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
	/*	ostringstream sql;
		sql.clear();
		sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "update CO_COUNTER set NEXT = "
			<< m_reportIdCounter
			<< " where COCOUN_ID = " 
			<< INCIDENT_COUNTTYPE 
			<< " and LOCATION_ID = " << getConsoleLocationKey();
		sql << "');";
		sql << "END;";*/
//		std::string strSql = defPrepareSQLStatement(m_dbConnection->GetDbServerType(), CO_COUNTER_Oracle_UPDATE_54701, getLocalDatabaseName(),
//		std::string strSql  = m_dbConnection->prepareSQLStatement(CO_COUNTER_UPDATE_54701, getLocalDatabaseName(),
		TA_Base_Core::SQLStatement strSql;
		m_dbConnection->prepareSQLStatement(strSql, CO_COUNTER_UPDATE_54701, getLocalDatabaseName(),
			m_reportIdCounter,INCIDENT_COUNTTYPE,getConsoleLocationKey());

		m_dbConnection->executeModification(strSql);
	}

	 //TD15204 - retrieve console location key
	unsigned long IncidentReportHelper::getConsoleLocationKey()
	{
		FUNCTION_ENTRY("getConsoleLocationKey");
		
		if (m_consoleLocationKey == 0)
		{
			string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
			ISession* session = NULL;   
			try
			{
				session  = SessionAccessFactory::getInstance().getSession(sessionId);
				IEntityData *pEntityData = EntityAccessFactory::getInstance().getEntity(session->getConsoleKey());
				m_consoleLocationKey = pEntityData->getLocation();			
				delete pEntityData;
				pEntityData = NULL;
			}
			catch (...) 
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Fail to get session profiles from the Authentication Agent");
			}
		}
		FUNCTION_EXIT;
		return m_consoleLocationKey;    
	}

	//TD15204 - retrieve console location Name
	std::string IncidentReportHelper::getConsoleLocationName()
	{
		FUNCTION_ENTRY("getConsoleLocationKey");
		
		if (m_consoleLocationName.empty())
		{
			string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
			ISession* session = NULL;   
			try
			{
				session  = SessionAccessFactory::getInstance().getSession(sessionId);
				IEntityData *pEntityData = EntityAccessFactory::getInstance().getEntity(session->getConsoleKey());
				m_consoleLocationName = pEntityData->getLocationName();
				delete pEntityData;
				pEntityData = NULL;
			}                                                                                                  
			catch (...) 
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Fail to get session profiles from the Authentication Agent");
			}
		}
		FUNCTION_EXIT;
		return m_consoleLocationName;    
	}


	std::string IncidentReportHelper::getUpdateDate()
	{
		FUNCTION_ENTRY("getUpdateDateSql");
		
		time_t ltime;
		struct tm *t;
		char tmpbuf[10];
		ostringstream oStream;
		
		time(&ltime);
		
		t = localtime(&ltime);	 
		strftime(tmpbuf,10, "%d%b%y", t);         
		
		//TD15204 - date string must be all caps
		for (int i=0;i<10;i++)
			tmpbuf[i] = toupper(tmpbuf[i]);
		
		oStream << tmpbuf;
		
		FUNCTION_EXIT;
		return oStream.str();
	}

	//TD15806
	void IncidentReportHelper::loadAssetDetails(TA_Base_Core::IDatabase* databaseConnection)
	{
		FUNCTION_ENTRY("loadAssetDetails");

		const std::string ENTITY_NAME = "ENTITYNAME";
		const std::string SUBSYSTEM = "SUBSYSTEM";
		const std::string ASSETPARAMETER = "AssetName";
		const std::string ASSETVALUE = "VALUE";

		vector<string> columnNames;
		//ostringstream sql;
		//std::string strSql;
		TA_Base_Core::SQLStatement strSql;
		//int nDbServerType = databaseConnection->GetDbServerType();
  
		// get asset data
		if (m_entityId != 0)
		{
			/*sql.str("");
			sql << "select asset.NAME, SUBSYSTEM.NAME"
				<< " from ENTITY asset, SUBSYSTEM"
				<< " where asset.PKEY = " << m_entityId
 				<< " AND asset.PHYSICAL_SUBSYSTEM_KEY  = SUBSYSTEM.PKEY";*/
			//strSql = defPrepareSQLStatement(nDbServerType, Entity_STD_SELECT_4512, m_entityId);
//			strSql = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4512, m_entityId); 					

		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4512, m_entityId);
			columnNames.clear();
			columnNames.push_back(ENTITY_NAME);
			columnNames.push_back(SUBSYSTEM);
			
			IData* data = databaseConnection->executeQuery(strSql,columnNames);
			if (data == NULL)
			{
				TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
			}
			
			if (0 == data->getNumRows()) 
			{
				// record not found
				delete data;
				data = NULL;
				
				// no physical subsystem associated to the incident
 				/*sql.str("");
				sql << "select asset.NAME, SUBSYSTEM.NAME"
					<< " from ENTITY asset, SUBSYSTEM"
					<< " where asset.PKEY = " << m_entityId			
					<<      " AND asset.SUBSYSTEMKEY = SUBSYSTEM.PKEY";*/
				//strSql = defPrepareSQLStatement(nDbServerType, Entity_STD_SELECT_4513, m_entityId);
//				strSql = databaseConnection->prepareSQLStatement(ENTITY_SELECT_4513, m_entityId);

		databaseConnection->prepareSQLStatement(strSql, ENTITY_SELECT_4513, m_entityId);
				
				columnNames.clear();
				columnNames.push_back(ENTITY_NAME);
				columnNames.push_back(SUBSYSTEM);
				
				try
				{
					data = databaseConnection->executeQuery(strSql,columnNames);
				}
				catch(...)
				{
					TA_THROW( IncidentException("Execute Query Exception", IncidentException::DATABASE_ERROR) );
				}
				
				if (data == NULL)
				{
					TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
				}
				
				if (data->getNumRows() == 0)
				{
					ostringstream reasonMessage;
					reasonMessage << "Invalid asset ID = " << m_entityId;
					TA_THROW( IncidentException(reasonMessage.str(), IncidentException::DATABASE_ERROR) );
				}
			}
			
			m_assetType.id = m_entityId;
			if (m_assetType.id != 0)
			{
				m_assetType.subsystem = data->getStringData(0, SUBSYSTEM);
				std::string entityName = data->getStringData(0, ENTITY_NAME);
				
				IData* pData = NULL;
				try
				{	
					pData = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
						entityName, ASSETPARAMETER);
					
				}
				catch(...)
				{
					TA_THROW( IncidentException("Invalid asset name value", IncidentException::DATABASE_ERROR) );
				}
				
				if(pData != NULL)
				{
					if (pData->getNumRows() == 1)
					{
						try
						{
							m_assetType.description = pData->getStringData(0,"VALUE");
						}
						catch (TA_Base_Core::DataException& e)
						{
							std::string text = "loadAssetDetails() - " + std::string(e.what());
							LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
							
							m_assetType.description = "";
						}
					}
					else
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
							"loadAssetDetails() - AssetName Entity Parameter missing for %s",
							entityName.c_str());
						// just supply the current entityName if the Asset name is not available
						std::replace(entityName.begin(),entityName.end(),'.','/');
						m_assetType.description = entityName;
						
					}
					delete pData;
				}
				else
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"loadAssetDetails() - AssetName Entity Parameter can not be retrieved for %s",
						entityName.c_str());
					
					// just supply the current entityName if the Asset name is not available
					std::replace(entityName.begin(),entityName.end(),'.','/');
					m_assetType.description = entityName;
				}			
			}
			else
			{
				m_assetType.description = "";
				m_assetType.subsystem = "";
			}
			delete data;
			data = NULL;
		}

		FUNCTION_EXIT;

	}

	//TD15806		
	void IncidentReportHelper::loadAssetDetailsFromAlarm(std::string alarmid)
	{
		FUNCTION_ENTRY("loadAssetDetailsFromAlarm");
		if (alarmid.empty())
		{
			m_assetType.description = "";
			m_assetType.id = 0;
			m_assetType.subsystem = "";
			return;
		}
		
		try
		{
			IAlarmData* alarmdata = AlarmAccessFactory::getInstance().getAlarm(alarmid);			
			unsigned long ulEntityKey = alarmdata->getEntityKey();                
			IEntityData *entitydata = EntityAccessFactory::getInstance().getEntity(ulEntityKey);
					
			m_assetType.id = entitydata->getKey();
			m_assetType.subsystem =  entitydata->getPhysicalSubsystemName();
			// check if no physical subsystem associated to the incident
			if (m_assetType.subsystem.empty())
			{
				m_assetType.subsystem =  entitydata->getSubsystemName();
			}
			m_assetType.description = alarmdata->getAssetName();
		
			unsigned long locationkey = alarmdata->getLocationKeyOfAssociatedEntity();
			ILocation *locationdata = LocationAccessFactory::getInstance().getLocationByKey(locationkey);                
			setLocation1(locationdata->getName());		    
			
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"Can not get the alarm details.");
		}
		
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
					"ReportId: %s, AlarmId: %s, Asset subsystem: %s, AssetName: %s",
					m_reportId.c_str(),m_alarmId.c_str(),m_assetType.subsystem.c_str(),m_assetType.description.c_str() );
		
		FUNCTION_EXIT;
		
	}
	std::string IncidentReportHelper::getMmsStateDesc( TA_Base_Core::IAlarmData::EMmsState alarmEmmsState )
	{
		switch(alarmEmmsState)
		{
		case TA_Base_Core::IAlarmData::TYPE_NONE:
			return "NONE";
		case TA_Base_Core::IAlarmData::TYPE_SEMI:
			return "MMS_SEMI";
		case TA_Base_Core::IAlarmData::TYPE_AUTO:
			return "AUTO";
		case TA_Base_Core::IAlarmData::TYPE_JR_SENT:
			return "JR_SENT";
		case TA_Base_Core::IAlarmData::TYPE_ALM_SENT:
			return "ALM_SENT";
		case TA_Base_Core::IAlarmData::TYPE_INHIBIT:
			return "INHIBIT";
		case TA_Base_Core::IAlarmData::TYPE_FAILED:
			return "Normal_degraded";
		default:
			return "unknown";
}// end TA_IRS_Core
	}
	std::string IncidentReportHelper::getAlarmAVLStateDesc( TA_Base_Core::IAlarmData::EAlarmAVLStatus alarmState )
	{
		switch(alarmState)
		{
		case TA_Base_Core::IAlarmData::NO_ASSOC_AVL:
			return "NONE";
		case TA_Base_Core::IAlarmData::HEAD_AVL:	
			return  "H";
		case TA_Base_Core::IAlarmData::CHILD_AVL:
			return "F";
		default:
			return "unknown";
		}
	}
	std::string IncidentReportHelper::getTimeDesc(time_t & sourceTime)
	{
		TA_Base_Core::DateFormat dateFormat; 
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4).c_str(); 
		char szTimeBuffer[256];
		szTimeBuffer[0] = 0x0;
		if ( sourceTime > 0 )
        {
			struct tm *newtime = localtime( &sourceTime ); // Convert to local time.
			strftime( szTimeBuffer, 255, dateFormatStr.c_str(), newtime);
        }
		return szTimeBuffer;
	}
}// end TA_IRS_Core
