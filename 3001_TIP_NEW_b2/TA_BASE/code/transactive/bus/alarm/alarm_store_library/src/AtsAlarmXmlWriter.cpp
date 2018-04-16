#include "AtsAlarmXmlWriter.h"
#include "AtsAlarmExportMgr.h"
#include "core/utilities/src/DebugUtil.h"

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "string"
#include "iostream"

#include <time.h>
#include "ace/OS.h"

using namespace rapidxml;

namespace TA_Base_Bus
{
	AtsAlarmXmlWriter::AtsAlarmXmlWriter(const std::string& filePath, unsigned long writeInterval)
		: m_filePath(filePath)
		, m_writeInterval(writeInterval)
		, m_terminate(false)
		, m_defaultFileName("AtsAlarm.xml")
		, m_fileCount(1)
		, m_lastDayLog(0)
	{
	}

	AtsAlarmXmlWriter::~AtsAlarmXmlWriter()
	{

	}

	void AtsAlarmXmlWriter::run()
	{
		FUNCTION_ENTRY("AtsAlarmXmlWriter::run");
		m_terminate = false;
		while(!m_terminate)
		{
			if (m_condition.timedWait(m_writeInterval) == 0)
			{	// Need to write to file
				writeToFile();
			}
		}

		FUNCTION_EXIT;
	}

	void AtsAlarmXmlWriter::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_terminate = false;
		m_condition.signal();

		FUNCTION_EXIT;
	}

	int AtsAlarmXmlWriter::getCacheSize()
	{
		TA_THREADGUARD(m_threadLock);
		return m_vecCache.size();
	}

	void AtsAlarmXmlWriter::insert(AtsXmlDataPtr atsData)
	{
		TA_THREADGUARD(m_threadLock);
		m_vecCache.push_back(atsData);
	}

	std::string AtsAlarmXmlWriter::getNextFileName()
	{
		std::string newFileName = m_defaultFileName;
		newFileName.erase(newFileName.find_last_of("."), std::string::npos);

		//append the date 
		time_t artime = time(0); // get current date time
		char fileDate[24] = {0};
		struct tm timeStampStruct;
		if (ACE_OS::localtime_r(&artime, &timeStampStruct) != NULL)
		{
			// check if we need to reset file count
			if (m_lastDayLog != timeStampStruct.tm_mday)
			{
				m_lastDayLog = timeStampStruct.tm_mday;
				m_fileCount = 1; // Reset file count
			}

			sprintf(fileDate, "%04d%02d%02d_%02d%02d%02d_%d",
				timeStampStruct.tm_year + 1900,
				timeStampStruct.tm_mon + 1,
				timeStampStruct.tm_mday,
				timeStampStruct.tm_hour,
				timeStampStruct.tm_min,
				timeStampStruct.tm_sec,
				m_fileCount);
			// increment file count
			m_fileCount++;
		}
		newFileName += "-" + std::string(fileDate) + ".xml";
		
		return newFileName;
	}

	void AtsAlarmXmlWriter::writeToFile()
	{
		if (getCacheSize() == 0)
			return; // No need to write file
		
		std::vector<AtsXmlDataPtr> vecData;
		{
			TA_THREADGUARD(m_threadLock);
			vecData.swap(m_vecCache);
		}
		std::string tempFileName = "TempAtsAlarm.temp";
		std::string tempFullPath = m_filePath + tempFileName;
		//Remove file if any.
		remove(tempFullPath.c_str());

		std::ofstream newFile (tempFullPath.c_str());
		if (!newFile.is_open())
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to create Ats export file with path: %s", tempFullPath.c_str());
			return; // File did not open
		}
		LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Writing Ats Data to xml file with size: %d", vecData.size());
		// Create an empty property tree object.
		xml_document<> doc;
		// XML Header
		xml_node<>* header = doc.allocate_node(node_declaration);
		header->append_attribute(doc.allocate_attribute(ATTRIB_VERSION, XML_VERSION));
		header->append_attribute(doc.allocate_attribute(ATTRIB_ENCODING, XML_ENCODING));
		doc.append_node(header);

		// XML body
		xml_node<>* root = doc.allocate_node(node_element, ELEM_BODY);
		doc.append_node(root);
		
		// Alarm Data
		for (int idx=0; idx<vecData.size(); idx++)
		{	
			xml_node<>* xmlAtsData = doc.allocate_node(node_element, ELEM_ALARM);
			//Alarm ID
			xml_node<>* ndAlarmID = doc.allocate_node(node_element, ELEM_ALARMID);
			ndAlarmID->value(vecData[idx]->alarmID.c_str());
			xmlAtsData->append_node(ndAlarmID);
			// Source Time
			xml_node<>* ndSourceTime = doc.allocate_node(node_element, ELEM_SOURCE_TIME);
			xml_node<>* ndSrcTime = doc.allocate_node(node_element, ELEM_TIME_PART);
			ndSrcTime->value(vecData[idx]->sourceTime_time.c_str());
			ndSourceTime->append_node(ndSrcTime);
			xml_node<>* ndSrcMili = doc.allocate_node(node_element, ELEM_MILI_PART);
			ndSrcMili->value(vecData[idx]->sourceTime_mili.c_str());
			ndSourceTime->append_node(ndSrcMili);
			xmlAtsData->append_node(ndSourceTime);
			//Acknowledge Time
			xml_node<>* ndAckTime = doc.allocate_node(node_element, ELEM_ACK_TIME);
			ndAckTime->value(vecData[idx]->ackTime.c_str());
			xmlAtsData->append_node(ndAckTime);
			// Close Time
			xml_node<>* ndCloseTime = doc.allocate_node(node_element, ELEM_CLOSE_TIME);
			ndCloseTime->value(vecData[idx]->closeTime.c_str());
			xmlAtsData->append_node(ndCloseTime);
			// Asset Name
			xml_node<>* ndAssetName = doc.allocate_node(node_element, ELEM_ASSET_NAME);
			ndAssetName->value(vecData[idx]->assetName.c_str());
			xmlAtsData->append_node(ndAssetName);
			// Alarm Severity
			xml_node<>* ndSeverity = doc.allocate_node(node_element, ELEM_SEVERITY);
			ndSeverity->value(vecData[idx]->alarmSeverity.c_str());
			xmlAtsData->append_node(ndSeverity);
			// Alarm Description
			xml_node<>* ndDescr = doc.allocate_node(node_element, ELEM_DESCRIPTION);
			ndDescr->value(vecData[idx]->alarmDescription.c_str());
			xmlAtsData->append_node(ndDescr);
			// Acknowledge by
			xml_node<>* ndAckBy = doc.allocate_node(node_element, ELEM_ACK_BY);
			ndAckBy->value(vecData[idx]->alarmAcknowledgeBy.c_str());
			xmlAtsData->append_node(ndAckBy);
			// Alarm State
			xml_node<>* ndState = doc.allocate_node(node_element, ELEM_STATE);
			ndState->value(vecData[idx]->state.c_str());
			xmlAtsData->append_node(ndState);
			// Location ID
			xml_node<>* ndLocationID = doc.allocate_node(node_element, ELEM_LOCATIONID);
			ndLocationID->value(vecData[idx]->locationId.c_str());
			xmlAtsData->append_node(ndLocationID);
			// Parent Alarm ID
			xml_node<>* ndParentID = doc.allocate_node(node_element, ELEM_PARENT_ALARMID);
			ndParentID->value(vecData[idx]->parentAlarmID.c_str());
			xmlAtsData->append_node(ndParentID);
			// Avalance Head ID
			xml_node<>* ndAvlHeadID = doc.allocate_node(node_element, ELEM_AV_HEADID);
			ndAvlHeadID->value(vecData[idx]->avalancheHeadID.c_str());
			xmlAtsData->append_node(ndAvlHeadID);
			// Is Head of Avalanche
			xml_node<>* ndIsHeadAvl = doc.allocate_node(node_element, ELEM_IS_AV_HEAD);
			ndIsHeadAvl->value(vecData[idx]->isHeadOfAvalanche.c_str());
			xmlAtsData->append_node(ndIsHeadAvl);
			// Is Child of Avalanche
			xml_node<>* ndIsChildAvl = doc.allocate_node(node_element, ELEM_IS_CHILD_AV);
			ndIsChildAvl->value(vecData[idx]->isChildOfAvalanche.c_str());
			xmlAtsData->append_node(ndIsChildAvl);
			// MMS State
			xml_node<>* ndMMSState = doc.allocate_node(node_element, ELEM_MMS_STATE);
			ndMMSState->value(vecData[idx]->mmsState.c_str());
			xmlAtsData->append_node(ndMMSState);
			// Alarm Value
			xml_node<>* ndAlarmValue = doc.allocate_node(node_element, ELEM_ALARM_VALUE);
			ndAlarmValue->value(vecData[idx]->alarmValue.c_str());
			xmlAtsData->append_node(ndAlarmValue);
			// Operational Maintenance Type
			xml_node<>* ndOmAlarm = doc.allocate_node(node_element, ELEM_OM_ALARM);
			ndOmAlarm->value(vecData[idx]->omAlarm.c_str());
			xmlAtsData->append_node(ndOmAlarm);
			// Alarm Type
			xml_node<>* ndAlarmType = doc.allocate_node(node_element, ELEM_ALARM_TYPE);
			ndAlarmType->value(vecData[idx]->alarmType.c_str());
			xmlAtsData->append_node(ndAlarmType);
			// SubSystem Key
			xml_node<>* ndSubSystemKey = doc.allocate_node(node_element, ELEM_SUBSYSTEM_KEY);
			ndSubSystemKey->value(vecData[idx]->subsytemkey.c_str());
			xmlAtsData->append_node(ndSubSystemKey);
			// System Key
			xml_node<>* ndSystemKey = doc.allocate_node(node_element, ELEM_SYSTEM_KEY);
			ndSystemKey->value(vecData[idx]->systemkey.c_str());
			xmlAtsData->append_node(ndSystemKey);
			// Alarm Comments
			xml_node<>* ndComments = doc.allocate_node(node_element, ELEM_COMMENTS);
			ndComments->value(vecData[idx]->alarmComments.c_str());
			xmlAtsData->append_node(ndComments);
			// Alarm Type
			xml_node<>* ndStrAlarmType = doc.allocate_node(node_element, ELEM_STR_ALARM_TYPE);
			ndStrAlarmType->value(vecData[idx]->strAlarmType.c_str());
			xmlAtsData->append_node(ndStrAlarmType);
			// SubSystem Type
			xml_node<>* ndSubSystemType = doc.allocate_node(node_element, ELEM_SUBSYSTEM_TYPE);
			ndSubSystemType->value(vecData[idx]->subsytemType.c_str());
			xmlAtsData->append_node(ndSubSystemType);
			// System Key
			xml_node<>* ndSystemType = doc.allocate_node(node_element, ELEM_SYSTEMKEY_TYPE);
			ndSystemType->value(vecData[idx]->systemkeyType.c_str());
			xmlAtsData->append_node(ndSystemType);
			// Location Key
			xml_node<>* ndLocationKey = doc.allocate_node(node_element, ELEM_LOCATION_KEY);
			ndLocationKey->value(vecData[idx]->locationKey.c_str());
			xmlAtsData->append_node(ndLocationKey);
			
			root->append_node(xmlAtsData);
		}

		newFile << doc;
		newFile.close();
		doc.clear();

		std::string oldfullPath = tempFullPath;
		std::string newfullPath = m_filePath + getNextFileName();
		if(rename( oldfullPath.c_str() , newfullPath.c_str() ) != 0)
		{
			LOG2(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to rename file from %s to %s", oldfullPath.c_str(), newfullPath.c_str());
		}

	}
}
