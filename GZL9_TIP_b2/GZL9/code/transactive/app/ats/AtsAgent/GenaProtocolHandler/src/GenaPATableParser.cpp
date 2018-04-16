
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaPATableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/PADataNotification.h"
#include "bus/protocol/Gena/src/GENATopicData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"
#include <string>

namespace TA_IRS_App
{

	GenaPATableParser::GenaPATableParser()
	{
		FUNCTION_ENTRY("GenaPATableParser");

		m_atsStations = TA_IRS_Core::AtsStationAccessFactory::getInstance().getAllStations();
		FUNCTION_EXIT;
	}

	GenaPATableParser::~GenaPATableParser()
	{
		FUNCTION_ENTRY("~GenaPATableParser");

		m_paStationInfoVec.clear();

		FUNCTION_EXIT;
	}

	void GenaPATableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importAtsTable");

		m_paStationInfoVec.clear();
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
		if((!m_paStationInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			//Notify for PA Data Event
			TA_IRS_Bus::PADataNotificationPtr paDataPtr(new TA_IRS_Bus::PADataNotification(&m_paStationInfoVec));
			//Set the Data PA Quality
			paDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(paDataPtr);
		}
		FUNCTION_EXIT;
	}

	void GenaPATableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		TA_Base_Bus::GENATopicDataPtr PAData = rawTable.getTopicData();

		//Parse PA Information
		TA_IRS_Bus::PAStationInfo paInfo;
		constructEmptypaInfo(paInfo);

		paInfo.PlatformId           = atol(PAData->getObjectID().c_str());
	
		T_paDataAttrVec trainlistVec = PAData->getAttribute(TRAINLIST);
		if(!trainlistVec.empty())
		{
			T_paDataAttrVecItr iter = trainlistVec.begin();
		
			T_paDataAttrVec pisDataVec = getPidDataAttrVec(PIDDATA,(*iter)->subAttribute);
			T_paDataAttrVecItr firstArrTrainItr = getFirstArrTrain(pisDataVec);

			if(firstArrTrainItr != pisDataVec.end())
			{
				std::string tmpArrivalTime,tmpDepartureTime;
				tmpArrivalTime = getAttributeValue(ARRIVALTIME, (*firstArrTrainItr));
				boost::to_upper(tmpArrivalTime);
				tmpDepartureTime = getAttributeValue(DEPARTURETIME, (*firstArrTrainItr));
				boost::to_upper(tmpDepartureTime);

				if (tmpArrivalTime==tmpDepartureTime)
				{
					paInfo.Skip = true;
				}
				else if (tmpDepartureTime =="INDETERMINED")
				{
					paInfo.hold = true;
				}
				else if (tmpArrivalTime =="UNSPECIFICED")
				{
					paInfo.Arrival = true;
				}


				std::string strFinalDestination;
				strFinalDestination.clear();
				strFinalDestination = getAttributeValue(FINALDESTINATION, (*firstArrTrainItr));
				if(strFinalDestination.compare(""))
				{

					paInfo.DestinationStationId = getLocationId(strFinalDestination);
				}
			}
		}
		m_paStationInfoVec.push_back(paInfo);

		FUNCTION_EXIT;
	}

	void GenaPATableParser::constructEmptypaInfo(TA_IRS_Bus::PAStationInfo& paInfo)
	{
		paInfo.PlatformId = 0;
		paInfo.DestinationStationId = 0;
		paInfo.Skip = false;
		paInfo.InDepot = false;
		paInfo.Arrival = false;
		paInfo.PreArrival = false;
		paInfo.hold = false;
	}

	std::string GenaPATableParser::getAttributeValue(const std::string& uri, AttributePtr attr)
	{
		FUNCTION_ENTRY("getAttributeValue");
		std::string strRetVal = "";

		T_AttribValueMap::iterator itFind = attr->subAttribute.find(uri);
		if (itFind != attr->subAttribute.end())
			strRetVal = itFind->second->value;

		FUNCTION_EXIT;
		return strRetVal;
	}


	bool GenaPATableParser::isErlierArrTrain(const std::string & strArrTime1,const std::string & strArrTime2)
	{
		FUNCTION_ENTRY("getErlierTime");

		struct time tm1, tm2;

		bool isTime1ConvertSucessful = convertTimeFormat(strArrTime1, tm1);
		bool isTime2ConvertSucessful = convertTimeFormat(strArrTime2, tm2);

		if((isTime1ConvertSucessful)&&(isTime2ConvertSucessful))
		{
			//If tm2 is earlier than tm1 then return true
			if(!timeCompare(tm1, tm2))
			{
				return true;
			}
		}
		else if(!isTime1ConvertSucessful)
		{
			return true;
		}

		return false;

		FUNCTION_EXIT;
	}

	bool GenaPATableParser::convertTimeFormat(const std::string & strTime,struct time& tmTime)
	{
		FUNCTION_ENTRY("convertTimeFormat");

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

		FUNCTION_EXIT;
	}

	bool GenaPATableParser::timeCompare(struct time& tmTime1,struct time& tmTime2)
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

	T_paDataAttrVecItr GenaPATableParser::getFirstArrTrain (T_paDataAttrVec& pisDataVec)
	{
		FUNCTION_ENTRY("getFirstArrTrain");

		std::string tmpArrTime;
		T_paDataAttrVecItr iter = pisDataVec.begin();	
		std::string firstTrainArrTime = TA_IRS_Bus::EMPTY_VAL;
		T_paDataAttrVecItr firstArrTrainItr = iter;
		for( ; iter!= pisDataVec.end(); iter++)
		{
			tmpArrTime = getAttributeValue(ARRIVALTIME, (*iter));
			boost::to_upper(tmpArrTime);
			//check If tmpArrTime is earlier than firstTrainArrTime, then update firstTrainArrTime
			if(isErlierArrTrain(firstTrainArrTime,tmpArrTime))
			{
				firstTrainArrTime = tmpArrTime;
				firstArrTrainItr = iter;
			}
		}
		return firstArrTrainItr;

		FUNCTION_EXIT;
	}

	int GenaPATableParser::getLocationId(const std::string& stationId)
	{
		FUNCTION_ENTRY("getLocationId");

		//Retrieve the LocationId based on the stationID
		TA_IRS_Core::IAtsStationsIt iter = m_atsStations.begin();
		unsigned long _retLocationID = 0;
		for ( iter; iter != m_atsStations.end(); iter++ )
		{
			if ( stationId == (*iter)->getStationId())
			{
				_retLocationID = (*iter)->getLocation();
				break;
			}
		}

		FUNCTION_EXIT;
		return _retLocationID;
	}

	std::vector<AttributePtr> GenaPATableParser::getPidDataAttrVec(const std::string& attrbName, T_AttribValueMap& pidDataMap)
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