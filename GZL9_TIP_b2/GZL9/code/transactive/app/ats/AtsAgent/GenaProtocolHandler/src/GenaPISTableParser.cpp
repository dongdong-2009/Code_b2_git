
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPISTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainPISDataNotification.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>

namespace TA_IRS_App
{

	GenaPISTableParser::GenaPISTableParser()
	{
		FUNCTION_ENTRY("GenaPISTableParser");
		FUNCTION_EXIT;
	}

	GenaPISTableParser::~GenaPISTableParser()
	{
		FUNCTION_ENTRY("~GenaPISTableParser");

		m_pisStationInfoVec.clear();

		FUNCTION_EXIT;
	}

	void GenaPISTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importAtsTable");

		m_pisStationInfoVec.clear();
		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="PIDLOCATION")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received Public Addressing Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_pisStationInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for Pis Data Event
			TA_IRS_Bus::TrainPISDataNotificationPtr pisDataPtr(new TA_IRS_Bus::TrainPISDataNotification(&m_pisStationInfoVec));
			//Set the Data Pis Quality
			pisDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(pisDataPtr);
		}
		FUNCTION_EXIT;
	}

	void GenaPISTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr PisData = rawTable.getTopicData();

		//Parse PIS Information
		TA_IRS_Bus::PISInfo pisInfo;
		constructEmptyPisInfo(pisInfo);

		pisInfo.PlatformId = PisData->getObjectID();

		T_trainArrTimeVec firstArrTimeVec;
		std::string tmpArrTime;
		T_pidDataAttrVec trainlistVec = PisData->getAttribute(TRAINLIST);
		if(!trainlistVec.empty())
		{
			T_pidDataAttrVecItr iter = trainlistVec.begin();
			T_pidDataAttrVec pisDataVec = getPidDataAttrVec(PIDDATA,(*iter)->subAttribute);
			iter = pisDataVec.begin();
			for( ; iter!= pisDataVec.end(); iter++)
			{
				tmpArrTime = getAttributeValue(ARRIVALTIME, (*iter));
				boost::to_upper(tmpArrTime);
				firstArrTimeVec.push_back(tmpArrTime);
			}

			std::string firstArrTime = get1stTrainArrTime(firstArrTimeVec);
			pisInfo.Train1st_ArrivalTime = firstArrTime;
			pisInfo.isNeedUpdateTime = true;
		}
		m_pisStationInfoVec.push_back(pisInfo);

		FUNCTION_EXIT;
	}

	void GenaPISTableParser::constructEmptyPisInfo(TA_IRS_Bus::PISInfo& pisInfo)
	{
		pisInfo.PlatformId = TA_IRS_Bus::EMPTY_VAL;;
		pisInfo.Train1st_ArrivalTime = TA_IRS_Bus::EMPTY_VAL;;
		pisInfo.isNeedUpdateTime = false;
	}

	std::string GenaPISTableParser::get1stTrainArrTime(const T_trainArrTimeVec & trainArrTimeVec)
	{  
		FUNCTION_ENTRY("get1stTrainArrTime");

		std::string firstTrainArrTime = TA_IRS_Bus::EMPTY_VAL;
		
		T_trainArrTimeVec::const_iterator itr = trainArrTimeVec.begin();

		for( ; itr!=trainArrTimeVec.end(); itr++)
		{
			firstTrainArrTime = getErlierTime(firstTrainArrTime,(*itr));
		}
	
		return firstTrainArrTime;
		FUNCTION_EXIT;
	}  

	std::string GenaPISTableParser::getErlierTime(const std::string & strArrTime1,const std::string & strArrTime2)
	{
		FUNCTION_ENTRY("getErlierTime");

		struct time tm1, tm2;

		bool isTime1ConvertSucessful = convertTimeFormat(strArrTime1, tm1);
		bool isTime2ConvertSucessful = convertTimeFormat(strArrTime2, tm2);

		if((isTime1ConvertSucessful)&&(isTime2ConvertSucessful))
		{
			if(timeCompare(tm1, tm2))
			{
				return strArrTime1;
			}
			else
			{
				return strArrTime2;
			}
		}
		else if(isTime1ConvertSucessful)
		{
			return strArrTime1;
		}
		else if(isTime2ConvertSucessful)
		{
			return strArrTime2;
		}

		return TA_IRS_Bus::EMPTY_VAL;

		FUNCTION_EXIT;
	}

	 bool GenaPISTableParser::convertTimeFormat(const std::string & strTime,struct time& tmTime)
	 {
		 if(strTime.compare(""))
		 {	   
			 int dataConvertedNum = sscanf(strTime.c_str(), "%d-%d-%d %d:%d:%d.%d" ,       
				 &(tmTime.tm_year),   
				 &(tmTime.tm_mon),   
				 &(tmTime.tm_mday),  
				 &(tmTime.tm_hour),  
				 &(tmTime.tm_min),  
				 &(tmTime.tm_sec),
				 &(tmTime.tm_msec));  

			 if(dataConvertedNum == 7)
			 {
				return true;
			 }
		 }
		 return false;
	 }

	 bool GenaPISTableParser::timeCompare(struct time& tmTime1,struct time& tmTime2)
	 {
		 FUNCTION_ENTRY("timeCompare");

		 if(tmTime1.tm_year != tmTime2.tm_year)
		 {
			 if(tmTime1.tm_year < tmTime2.tm_year)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_mon != tmTime2.tm_mon)
		 {
			 if(tmTime1.tm_mon < tmTime2.tm_mon)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_mday != tmTime2.tm_mday)
		 {
			 if(tmTime1.tm_mday < tmTime2.tm_mday)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_hour != tmTime2.tm_hour)
		 {
			 if(tmTime1.tm_hour < tmTime2.tm_hour)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_min != tmTime2.tm_min)
		 {
			 if(tmTime1.tm_min < tmTime2.tm_min)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_sec != tmTime2.tm_sec)
		 {
			 if(tmTime1.tm_sec < tmTime2.tm_sec)
				 return true;
			 else
				 return false;
		 }
		 else if(tmTime1.tm_msec != tmTime2.tm_msec)
		 {
			 if(tmTime1.tm_msec < tmTime2.tm_msec)
				 return true;
			 else
				 return false;
		 }
		 else
		 {
			 return true;
		 }

		 FUNCTION_EXIT;
	 }

	 std::string GenaPISTableParser::getAttributeValue(const std::string& uri, AttributePtr attr)
	 {
		 FUNCTION_ENTRY("getAttributeValue");
		 std::string strRetVal = "";

		 T_AttribValueMap::iterator itFind = attr->subAttribute.find(uri);
		 if (itFind != attr->subAttribute.end())
			 strRetVal = itFind->second->value;

		 FUNCTION_EXIT;
		 return strRetVal;
	 }

	 std::vector<AttributePtr> GenaPISTableParser::getPidDataAttrVec(const std::string& attrbName, T_AttribValueMap& pidDataMap)
	 {
		 FUNCTION_ENTRY("getAttribute");
		 std::vector<AttributePtr> pRetVal;

		 T_AttribValueMap::iterator itr = pidDataMap.begin();
		 for (itr; itr!=pidDataMap.end(); itr++)
		 {
			 if (itr->first == attrbName)
			 {
				 pRetVal.push_back(itr->second);
			 }
		 }

		 FUNCTION_EXIT;
		 return pRetVal;
	 }

}