
#ifndef _GENA_PA_TABLE_PARSER_H_
#define _GENA_PA_TABLE_PARSER_H_

#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaBaseTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaRawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "core/data_access_interface/ats/src/AtsStationAccessFactory.h"

namespace TA_IRS_App
{
	typedef std::vector<AttributePtr>	T_paDataAttrVec;
	typedef T_paDataAttrVec::iterator	T_paDataAttrVecItr;

	class GenaPATableParser : public GenaBaseTableParser
	{
	public:
		/**
		 * Constructor
		 */
		GenaPATableParser();
		/**
		 * Destructor
		 */
		virtual ~GenaPATableParser();

		/**
		 * parseAtsTable
		 *
		 * Method to use to process Ats Table
		 *
		 * @param rawTable : Gena RawTable data
		 */
		virtual void parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable);

	private:
		/**
		 * Disable default, copy and assignment constructor
		 */
		GenaPATableParser(const GenaPATableParser&);
		GenaPATableParser& operator= (const GenaPATableParser&);
		TA_IRS_Bus::PAStationInfoVec m_paStationInfoVec;


		/**
		 * importRecord
		 *
		 * Method to use to import the raw data table
		 *
		 * @param rawTable         : Gena RawTable data
		 * 
		 */
		void importRecord(TA_IRS_App::GenaRawTable& rawTable);
		/**
		 * constructEmptypaInfo
		 *
		 * Method to use to set init value to paInfo
		 *
		 * @param PAStationInfo
		 * 
		 */
		void constructEmptypaInfo(TA_IRS_Bus::PAStationInfo& paInfo);
		/**
		 * getAttributeValue
		 *
		 * Method to use to get attribute value
		 *
		 * @param uri:			attribute name
		 * @param AttributePtr: pointer to attribute object
		 * 
		 * @return attribute value
		 */
		std::string getAttributeValue(const std::string& uri, AttributePtr attr);
		/**
		 * convertTimeFormat
		 *
		 * Method to use to convert string to struct time
		 *
		 * @param std::string
		 * @param struct time
		 * 
		 * @return : if successful return true, otherwise return false
		 */
		bool convertTimeFormat(const std::string & strTime,struct time& tmTime);
		/**
		 * getFirstArrTrain
		 *
		 * Method to use to get first arrival train attribute
		 *
		 * @param T_paDataAttrVec: train attributes list
		 * 
		 * @return T_paDataAttrVecItr: first arrival train attribute
		 */
		T_paDataAttrVecItr getFirstArrTrain (T_paDataAttrVec& pisDataVec);
		/**
		 * timeCompare
		 *
		 * Method to use to compare time
		 *
		 * @return bool : true - tmTime1 is earlier than tmTime2, otherwise is false
		 * 
		 */
		bool timeCompare(struct time& tmTime1,struct time& tmTime2);
		/**
		 * isErlierArrTrain
		 *
		 * Method to use to compare which train arrived earlier
		 *
		 * @return bool : true - strArrTime2 is earlier than strArrTime1
		 * 
		 */
		bool isErlierArrTrain(const std::string & strArrTime1,const std::string & strArrTime2);
		/**
		 * getLocationId
		 *
		 * Method to use to get station Id from AT_STATION_LOCATION_MAP
		 *
		 * @return int: location Id
		 * 
		 */
		int getLocationId(const std::string& stationId);

		std::vector<AttributePtr> getPidDataAttrVec(const std::string& attrbName, T_AttribValueMap& pidDataMap);

		TA_IRS_Core::IAtsStations m_atsStations;

	};
}
#endif //_GENA_PA_TABLE_PARSER_H_