

#include "core/data_access_interface/bas/src/BasTrackAccessFactory.h"
#include "core/data_access_interface/src/SQLCode.h"

namespace TA_IRS_Core
{
	bool TrackInfo::isApproximateMatchTrack(const std::string& strTrackName)
	{
		return (std::string::npos != trackName.find(strTrackName)) ? true : false;
	}

	bool TrackInfo::isAccurateMatchTrack(const std::string& strTrackName)
	{
		return (strTrackName == trackName) ? true : false;
	}


	bool TrackInfo::isTrackValid(const std::string& trackNum)
	{
		bool isValid = false;
		if (false == trackNum.empty())
		{
			size_t pos = trackNum.find_first_of('+');
			if (pos != std::string::npos && pos > (sizeof("ZDK")-1-1) && pos < trackNum.size() )
			{
				isValid = true;
			}
		}

		return isValid;
	}
	//////////////////////////////////////////////////////////
	const std::string BasTrackAccessFactory::TableName = "BA_TUNNEL_TRACK";
	const std::string BasTrackAccessFactory::ColumnName_TrackNumber = "TRACKNUMBER";
	const std::string BasTrackAccessFactory::ColumnName_TrackName = "TRACKNAME";
	const std::string BasTrackAccessFactory::ColumnName_SectionKey = "SECTIONKEY";
	const std::string BasTrackAccessFactory::ColumnName_UpDownBound = "UPDOWNBOUND";
	const std::string BasTrackAccessFactory::ColumnName_PositionByShaft = "POSITIONBYSHAFT";
	BasTrackAccessFactory::BasTrackAccessFactory(void)
	{
	}

	BasTrackAccessFactory::~BasTrackAccessFactory(void)
	{
	}

	void BasTrackAccessFactory::getTrackInfos(TrackInfoVct& trackInfoVct)
	{
		FUNCTION_ENTRY("getTrackInfos");

		using namespace TA_Base_Core;
		// get a connection to the database
		IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Ecs_Cd, Read);

		/*std::ostringstream ss;
		ss << "SELECT " << ColumnName_TrackNumber << " , "
			<< ColumnName_TrackName << " , "
			<< ColumnName_SectionKey << " , "
			<< ColumnName_UpDownBound << " , "
			<< ColumnName_PositionByShaft 
			<< " FROM " << TableName;
		std::string strSql = ss.str();*/
		/*SELECT TRACKNUMBER,TRACKNAME,SECTIONKEY,UPDOWNBOUND,POSITIONBYSHAFT FROM BA_TUNNEL_TRACK*/
		TA_Base_Core::SQLStatement sql;
		databaseConnection->prepareSQLStatement(sql, BA_TUNNEL_TRACK_SELECT_100001); 
		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(ColumnName_TrackNumber);
		columnNames.push_back(ColumnName_TrackName);
		columnNames.push_back(ColumnName_SectionKey);
		columnNames.push_back(ColumnName_UpDownBound);
		columnNames.push_back(ColumnName_PositionByShaft);

		// get data from database
		//IData* data = databaseConnection->executeQuery(strSql, columnNames);
		IData* data = databaseConnection->executeQuery(sql, columnNames);

		trackInfoVct.clear();
		do 
		{
			for (unsigned long index = 0; index < data->getNumRows(); index++)
			{
				TrackInfo_SmartPtr trackInfo_Ptr = TrackInfo_SmartPtr(new TrackInfo);
				trackInfo_Ptr->trackNumber = data->getUnsignedLongData(index, ColumnName_TrackNumber);
                trackInfo_Ptr->trackName = data->getStringData(index, ColumnName_TrackName);
				trackInfo_Ptr->sectionKey = data->getUnsignedLongData(index, ColumnName_SectionKey);
				trackInfo_Ptr->lineType = stringToLineType(data->getStringData(index, ColumnName_UpDownBound));
				trackInfo_Ptr->firePosition = stringToTrainPositionByShaft(data->getStringData(index, ColumnName_PositionByShaft));

				trackInfoVct.push_back(trackInfo_Ptr);
			}

			if (trackInfoVct.empty())
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[No Data]there is no track data found in database");
				return;
			}
			delete data;
			data = NULL;
		} while (databaseConnection->moreData(data));
		FUNCTION_EXIT;
	}
}
