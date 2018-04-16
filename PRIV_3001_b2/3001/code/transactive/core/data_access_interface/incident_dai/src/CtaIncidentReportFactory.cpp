/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/incident_dai/src/CtaIncidentReportFactory.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * CtaLocationAccessFactory is a singleton that is used to retrieve Incident Report objects from the
  * database. 
  */

#include "core/data_access_interface/incident_dai/src/CtaIncidentReportFactory.h"
#include "core/data_access_interface/incident_dai/src/CtaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/IncidentReportHelper.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/IncidentException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/IncidentManagerBroadcastComms_MessageTypes.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


#include <sstream>

namespace TA_IRS_Core
{
	
	const std::string CtaIncidentReportFactory::IN_ININCI_ID_COL		           = "inc.ININCI_ID";
	const std::string CtaIncidentReportFactory::IN_TITLE_COL	                   = "inc.TITLE";
	const std::string CtaIncidentReportFactory::IN_CREATE_OPERATOR_COL             = "cre.NAME";
	const std::string CtaIncidentReportFactory::IN_CREATE_OPERATOR_PROFILE_COL     = "creprof.NAME";
	const std::string CtaIncidentReportFactory::IN_CREATE_DATE_COL                = "inc.CREATE_DATE";
	// const std::string CtaIncidentReportFactory::IN_ASSET_COL                      = "asset.DESCRIPTION"; // TD13869 - lizettejl
	const std::string CtaIncidentReportFactory::IN_ASSET_COL                      = "asset.NAME";
	const std::string CtaIncidentReportFactory::IN_ASSET_SUBSYSTEM_COL            = "SUBSYSTEM.NAME";
	const std::string CtaIncidentReportFactory::IN_LOCATION_1_COL                 = "loc1.NAME";
	const std::string CtaIncidentReportFactory::IN_LOCATION_2_COL                 = "loc2.NAME";
	const std::string CtaIncidentReportFactory::IN_TRAIN_ID_COL                   = "TRAIN_ID";
	const std::string CtaIncidentReportFactory::IN_INNER_TRACK_COL                = "INNER_TRACK";
	const std::string CtaIncidentReportFactory::IN_OUTER_TRACK_COL                = "OUTER_TRACK";
	const std::string CtaIncidentReportFactory::IN_ALARM_ID_COL                   = "ALARM_ID";
	const std::string CtaIncidentReportFactory::IN_CLOSE_DATE_COL                 = "CLOSE_DATE";
	
	using namespace TA_Base_Core;
	
	CtaIncidentReportFactory::CtaIncidentReportFactory()
	{
		FUNCTION_ENTRY("Constructor");
		
		
		FUNCTION_EXIT;
	}
	
	CtaIncidentReportFactory::~CtaIncidentReportFactory()
	{
		FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
	}
	
	
	void CtaIncidentReportFactory::registerCallback(ItaIncidentGuiCallback* gui)
	{
		FUNCTION_ENTRY("registerCallback");
		TA_ASSERT(gui != NULL, "Incident Gui Callback cannot be NULL");
		
		MessageSubscriptionManager &manager = MessageSubscriptionManager::getInstance();
		manager.subscribeToBroadcastCommsMessage(IncidentManagerBroadcastComms::Context, this);
		m_ItaIncidentGuiCallback = gui;
		
		FUNCTION_EXIT;
		
	}
	
	void CtaIncidentReportFactory::deregister()
	{
		FUNCTION_ENTRY("deregister");
		
		MessageSubscriptionManager &manager = MessageSubscriptionManager::getInstance();
		manager.unsubscribeToMessages(this);
		
		m_ItaIncidentGuiCallback = NULL;
		
		FUNCTION_EXIT;
	}
	
	//Mintao ++
	//Bug 652 (TD12313)
	ItaIncidentReportData* CtaIncidentReportFactory::getIncidentReport(std::string id)
	{
		FUNCTION_ENTRY("getIncidentReport");
		FUNCTION_EXIT;
		return new CtaIncidentReportData(id, true);
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	ItaIncidentReportData* CtaIncidentReportFactory::createIncidentReport()
	{
		FUNCTION_ENTRY("createIncidentReport");
		FUNCTION_EXIT;
		return new CtaIncidentReportData();
	}
	
	ItaIncidentReportData* CtaIncidentReportFactory::createIncidentReport(const std::string & alarmId, const std::string & alarmValue)
	{
		FUNCTION_ENTRY("createIncidentReport(alarmId)");
		FUNCTION_EXIT;
		return new CtaIncidentReportData(alarmId, alarmValue);
	}
	
	
	std::vector<ItaIncidentReportData*> CtaIncidentReportFactory::getIncidentReports()
	{
		FUNCTION_ENTRY("getIncidentReports");
		
		// get a connection to the database
		IDatabase* databaseConnection = 
            DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);
		
		if (databaseConnection == NULL)
		{
			TA_THROW( IncidentException("Cannot connect to database", IncidentException::DATABASE_ERROR) );
		}
		
		// create the SQL string to retrieve the data of the IncidentReportData
		// TODO: filter on the operator access level (IN_ACCESS_LEVEL table)
		
		/*std::ostringstream sql;
		sql << "select " << IN_ININCI_ID_COL << ", " 
			<< "TO_CHAR(" << IN_CREATE_DATE_COL << ",'YYYYMMDDHH24MISS'), "
			<< IN_TITLE_COL << ", " 
			<< IN_LOCATION_1_COL << ", " 
			<< IN_LOCATION_2_COL << ", "
			<< IN_INNER_TRACK_COL << ", " 
			<< IN_OUTER_TRACK_COL << ", "
			<< IN_TRAIN_ID_COL << ", " 
			<< IN_ALARM_ID_COL << ", "
			<< IN_ASSET_COL << ", " 
			<< IN_ASSET_SUBSYSTEM_COL << ", "
			<< IN_CREATE_OPERATOR_COL << ", " 
			<< IN_CREATE_OPERATOR_PROFILE_COL << ", "
			<< "TO_CHAR(" << IN_CLOSE_DATE_COL << ",'YYYYMMDDHH24MISS') " << ", ";
		
		sql << "inc.ENTITY_ID"
			<< " from IN_INCIDENT_REPORT inc, LOCATION loc1, LOCATION loc2,"
			<<       "ENTITY asset, OPERATOR cre, SE_PROFILE creprof, SUBSYSTEM"
			<< " where "
			<<      " inc.LOCATI_ID_1 = loc1.PKEY (+) AND inc.LOCATI_ID_2 = loc2.PKEY (+)"
			<<      " AND inc.CREATE_OPERAT_ID = cre.PKEY (+)"
			<<      " AND inc.CREATE_SEPROF_ID = creprof.SEPROF_ID (+)"
			<<      " AND inc.ENTITY_ID = asset.PKEY (+)"
			<<      " AND asset.SUBSYSTEMKEY = SUBSYSTEM.PKEY (+)";*/
//		std::string strSql = defPrepareSQLStatement(databaseConnection->GetDbServerType(), IN_INCIDENT_REPORT_Oracle_SELECT_52551);
//		std::string strSql  = databaseConnection->prepareSQLStatement(IN_INCIDENT_REPORT_SELECT_52551);
		TA_Base_Core::SQLStatement strSql;
		databaseConnection->prepareSQLStatement(strSql, IN_INCIDENT_REPORT_SELECT_52551);
		
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(IN_ININCI_ID_COL);
		columnNames.push_back(IN_CREATE_DATE_COL);
		columnNames.push_back(IN_TITLE_COL);
		columnNames.push_back(IN_LOCATION_1_COL);
		columnNames.push_back(IN_LOCATION_2_COL);
		columnNames.push_back(IN_INNER_TRACK_COL);
		columnNames.push_back(IN_OUTER_TRACK_COL);
		columnNames.push_back(IN_TRAIN_ID_COL);
		columnNames.push_back(IN_ALARM_ID_COL);
		columnNames.push_back(IN_ASSET_COL);
		columnNames.push_back(IN_ASSET_SUBSYSTEM_COL);
		columnNames.push_back(IN_CREATE_OPERATOR_COL);
		columnNames.push_back(IN_CREATE_OPERATOR_PROFILE_COL);
		columnNames.push_back(IN_CLOSE_DATE_COL);
		columnNames.push_back("inc.ENTITY_ID");
		
		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		IData* data = databaseConnection->executeQuery(strSql,columnNames);
		
		if (data == NULL)
		{
			TA_THROW( IncidentException("Execute query error", IncidentException::DATABASE_ERROR) );
		}
		
		// create the vector of pointers to IEntityData
		std::vector<ItaIncidentReportData*> incidentReportDataVector;
		//Mintao ++
		//Bug 652 (TD12313)
		
		std::string reportId;
		//Mintao ++
		//Bug 652 (TD12313)
		
		time_t createDate;
		std::string title;
		std::string location1;
		std::string location2;
		bool innerTrack;
		bool outerTrack;
		std::string trainId;
		std::string alarmId;
		std::string asset;
		std::string assetSubsystem;
		std::string creator;
		std::string creatorProfile;
		time_t closeDate;
		unsigned long entityid;
		
		do
		{
			for (unsigned long i = 0; i < data->getNumRows() ; ++i )
			{
				//Mintao ++
				//Bug 652 (TD12313)
				
				reportId = data->getStringData(i, IN_ININCI_ID_COL);
				//Mintao ++
				//Bug 652 (TD12313)
				
				IncidentReportHelper::getDateData(data, i, IN_CREATE_DATE_COL, createDate);
				title = data->getStringData(i, IN_TITLE_COL);
				
				location1 = data->getStringData(i, IN_LOCATION_1_COL);
				location2 = data->getStringData(i, IN_LOCATION_2_COL);
				innerTrack = data->getBooleanData(i, IN_INNER_TRACK_COL);
				outerTrack = data->getBooleanData(i, IN_OUTER_TRACK_COL);
				
				if (data->isNull(i, IN_TRAIN_ID_COL))
				{
					trainId = "";
				}
				else
				{
					std::ostringstream ostream;
					ostream << data->getStringData(i, IN_TRAIN_ID_COL);
					trainId = ostream.str();
				}
				
				alarmId = data->getStringData(i, IN_ALARM_ID_COL);
				
				entityid = data->getIntegerData(i,"inc.ENTITY_ID" );
				if (entityid != 0)
				{
					asset = data->getStringData(i, IN_ASSET_COL);
					assetSubsystem = data->getStringData(i, IN_ASSET_SUBSYSTEM_COL);
				}
				else
				{
					asset = "";
					assetSubsystem = "";
				}
				
				creator = data->getStringData(i, IN_CREATE_OPERATOR_COL);
				creatorProfile = data->getStringData(i, IN_CREATE_OPERATOR_PROFILE_COL);
				
				IncidentReportHelper::getDateData(data, i, IN_CLOSE_DATE_COL, closeDate);
				
				CtaIncidentReportData *incidentReportData = new CtaIncidentReportData(
					reportId, createDate, title, location1, location2,
					innerTrack, outerTrack, trainId,
					alarmId, asset, assetSubsystem, creator, creatorProfile,
					closeDate);
				incidentReportDataVector.push_back(incidentReportData);
			}
			//Bugzilla Bug 181: Mintao
			delete data;
			data = NULL;
			
		}while( databaseConnection->moreData(data) );
		
		
		FUNCTION_EXIT;
		return incidentReportDataVector;
	}
	//Mintao ++
	//Bug 652 (TD12313)

	void CtaIncidentReportFactory::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
	{
		FUNCTION_ENTRY("receiveSpecialisedMessage");
		
		if (m_ItaIncidentGuiCallback != NULL)
		{
			std::string messageTypeKey(message.messageTypeKey);
			const char* reportId = NULL;
			unsigned long idkey = 0;
			if (messageTypeKey == TA_Base_Core::IncidentManagerBroadcastComms::IncidentReportAdded.getTypeKey())
			{
				if (0 != (message.messageState >>= CORBA::Any::to_string(reportId, 0)))
				{
                
					CtaIncidentReportData* data = new CtaIncidentReportData(reportId, true);
					m_ItaIncidentGuiCallback->add(data);
					reportId = NULL;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
						"Convertion from CORBA string to C++ string fail!");
				}
			}
			else if (messageTypeKey == TA_Base_Core::IncidentManagerBroadcastComms::IncidentReportRemoved.getTypeKey())
			{
				if (0 != (message.messageState >>= CORBA::Any::to_string(reportId, 0)))
				{
					m_ItaIncidentGuiCallback->remove(reportId);
					reportId = NULL;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
						"Convertion from CORBA string to C++ string fail!");
				}
			}
			else if (messageTypeKey == TA_Base_Core::IncidentManagerBroadcastComms::IncidentReportModified.getTypeKey())
			{
				if (0 != (message.messageState >>= CORBA::Any::to_string(reportId, 0)))
				{
					m_ItaIncidentGuiCallback->update(reportId);
					reportId = NULL;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugFatal,
						"Convertion from CORBA string to C++ string fail!");
				}
			}
		}
		
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
} // end TA_IRS_Core

