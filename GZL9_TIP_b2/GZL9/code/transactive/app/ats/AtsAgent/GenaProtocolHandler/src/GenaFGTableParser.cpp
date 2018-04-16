
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaFGTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/FGDataNotification.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"


namespace TA_IRS_App
{
	const char * RequestClose [] =
	{
		"REQUEST",	
		"NOTREQUEST", 
	};
	enum RequestCloseEnum {REQUEST = 0, NOTREQUEST, REQUNKNOWN};
	#define REQCLOSELENGTH	2

	const char * PermissionClose [] =
	{
		"PERMISSION",		
		"NOTPERMISSION",	
	};
	enum PermissionCloseEnum {PERMISSION = 0, NOTPERMISSION, PERMUNKNOWN};
	#define PERMISSIONLENGTH	2

	GenaFGTableParser::GenaFGTableParser()
	{
		FUNCTION_ENTRY("GenaFGTableParser");
		FUNCTION_EXIT;
	}

	GenaFGTableParser::~GenaFGTableParser()
	{
		FUNCTION_ENTRY("~GenaFGTableParser");
		FUNCTION_EXIT;
	}

	void GenaFGTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseGenaAtsTable");

		m_fgInfoVec.clear();
		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="FLOODGATE")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received FloodGate Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_fgInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for FloodGate Data Event
			TA_IRS_Bus::FGDataNotificationPtr fgDataPtr(new TA_IRS_Bus::FGDataNotification(&m_fgInfoVec));
			//Set the Data FloodGate Quality
			fgDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(fgDataPtr);
		}
		FUNCTION_EXIT;
	}

	void GenaFGTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr FGData = rawTable.getTopicData();

		//Parse FloodGate Information
		TA_IRS_Bus::FloodGateInfo floodgateInfo;
		//Initialize FloodGate Value
		floodgateInfo.FloodGateId = FGData->getObjectID();
		floodgateInfo.isNeedUpdatePermission = false;
		floodgateInfo.isNeedUpdateReq = false;

		std::string tmpData;
		//Update value get from Gena client
		tmpData = FGData->getAttributeValue(REQUESTCLOSE);
		if(tmpData.compare(""))
		{			
			signed short RequestStatus = convertStrToEmun(RequestClose,tmpData, REQCLOSELENGTH);
			if(RequestStatus != -1)
			{
				floodgateInfo.RequestClose = RequestStatus;
			}
			else
			{
				floodgateInfo.RequestClose = REQUNKNOWN;
			}
			floodgateInfo.isNeedUpdateReq = true;
		}

		tmpData = FGData->getAttributeValue(PERMISSIONCLOSE);
		if(tmpData.compare(""))
		{			
			signed short PermissionStatus = convertStrToEmun(PermissionClose,tmpData, PERMISSIONLENGTH);
			if(PermissionStatus != -1)
			{
				floodgateInfo.PermissionClose = PermissionStatus;
			}
			else
			{
				floodgateInfo.PermissionClose = PERMUNKNOWN;
			}
			floodgateInfo.isNeedUpdatePermission = true;
		}

		m_fgInfoVec.push_back(floodgateInfo);

		FUNCTION_EXIT;
	}


}