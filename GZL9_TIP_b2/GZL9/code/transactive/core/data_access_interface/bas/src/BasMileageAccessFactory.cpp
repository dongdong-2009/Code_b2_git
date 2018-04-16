
#include "core/data_access_interface/bas/src/BasMileageAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_IRS_Core
{
	bool MileageInfo::isMatchMileage(const std::string& strMileage, bool leftLine)
	{
		unsigned long mileage = stringToUnsignedLong(strMileage);
		return isMatchMileage(mileage, leftLine);
	}

	bool MileageInfo::isMatchMileage(unsigned long mileage, bool leftLine)
	{
		bool match = false;
		if ((leftLine ? enum_UpBound : enum_DownBound) == updownBound)
		{
			if (mileage >= getBeginMileage() && mileage <= getEndMileage())
			{
				match = true;
			}
		}

		return match;
	}

	LineType MileageInfo::getLineTypeByString(const std::string& strMileage)
	{
		LineType linetype = enum_Unknown;
		if (!strMileage.empty())
		{
			if ('Z' == strMileage[0] || 'z' == strMileage[0])
			{
				linetype = enum_UpBound;
			}
			else if ('Y' == strMileage[0] || 'y' == strMileage[0])
			{
				linetype = enum_DownBound;
			}
		}
		return linetype;
	}

	unsigned long MileageInfo::getBeginMileage()
	{
		return beginMileage;
	}

	unsigned long MileageInfo::getEndMileage()
	{
		return endMileage;
	}

	std::string MileageInfo::getMileageString(const std::string& strMileage)
	{
		//the strMileage format should be "ZDK2+345" or "YDK2+345"
		std::string strMileageNum;
		size_t pos = strMileage.find_first_of('+');
		if ( pos > 0)
		{
			strMileageNum = strMileage.substr(3, pos-3) + strMileage.substr(pos+1);
		}
		return strMileageNum;
		
	}

	unsigned long MileageInfo::getMileageNumber(const std::string& strMileage)
	{
		return stringToUnsignedLong(getMileageString(strMileage));
	}
 //--------------------------------------------------------------------------------------
	const std::string BasMileageAccessFactory::TableName = "BA_TUNNEL_MILEAGE";
	const std::string BasMileageAccessFactory::ColumnName_MileageNumber = "MILEAGE";
	const std::string BasMileageAccessFactory::ColumnName_SectionKey = "SECTIONKEY";
	const std::string BasMileageAccessFactory::ColumnName_UpDownBound = "UPDOWNBOUND";
	const std::string BasMileageAccessFactory::ColumnName_BeginMileage = "BEGINMILEAGE";
	const std::string BasMileageAccessFactory::ColumnName_EndMileage = "ENDMILEAGE";
	const std::string BasMileageAccessFactory::ColumnName_SegmentName = "SEGMENT";
	const std::string BasMileageAccessFactory::ColumnName_PositionByShaft = "POSITIONBYSHAFT";

	BasMileageAccessFactory::BasMileageAccessFactory(void)
	{
	}

	BasMileageAccessFactory::~BasMileageAccessFactory(void)
	{
	}

	void BasMileageAccessFactory::getAllMileageInfos(MileageInfoVct& mileageInfoVct)
	{
		FUNCTION_ENTRY("getAllMileageInfos");
		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_MileageNumber << " , "
			<< ColumnName_SectionKey << " , "
			<< ColumnName_UpDownBound << " , "
			<< ColumnName_BeginMileage << " , "
			<< ColumnName_EndMileage << " , "
			<< ColumnName_SegmentName << " , "
			<< ColumnName_PositionByShaft 
			<< " FROM " << TableName;
		std::string strSql = ss.str();*/
		/*SELECT MILEAGE,SECTIONKEY,UPDOWNBOUND,BEGINMILEAGE,ENDMILEAGE,SEGMENT,POSITIONBYSHAFT FROM BA_TUNNEL_MILEAGE*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TIMETABLE_MODETIME_SELECT_99001); 

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_MileageNumber);
		columnNames.push_back(ColumnName_SectionKey);
		columnNames.push_back(ColumnName_UpDownBound);
		columnNames.push_back(ColumnName_BeginMileage);
		columnNames.push_back(ColumnName_EndMileage);
		columnNames.push_back(ColumnName_SegmentName);
		columnNames.push_back(ColumnName_PositionByShaft);

		// get data from database
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		mileageInfoVct.clear();
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				MileageInfo_Smart_Ptr mileagePtr = MileageInfo_Smart_Ptr(new MileageInfo);
				mileagePtr->mileageNumber = data->getUnsignedLongData(index, ColumnName_MileageNumber);
				mileagePtr->sectionKey = data->getUnsignedLongData(index, ColumnName_SectionKey);
				mileagePtr->updownBound = stringToLineType(data->getStringData(index, ColumnName_UpDownBound));
				mileagePtr->beginMileage = stringToUnsignedLong(MileageInfo::getMileageString(data->getStringData(index, ColumnName_BeginMileage))) ;
				mileagePtr->endMileage = stringToUnsignedLong(MileageInfo::getMileageString(data->getStringData(index, ColumnName_EndMileage)));
				mileagePtr->segment = stringToSegment(data->getStringData(index, ColumnName_SegmentName));
				mileagePtr->positionByShaft = stringToTrainPositionByShaft(data->getStringData(index, ColumnName_PositionByShaft));
				
				mileageInfoVct.push_back(mileagePtr);
			}
			
			if (mileageInfoVct.empty())
			{
				TA_THROW(TA_Base_Core::DatabaseException("There no mileage data in database."));
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));
		FUNCTION_EXIT;
	}

};
