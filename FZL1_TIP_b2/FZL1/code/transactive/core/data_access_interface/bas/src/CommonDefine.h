#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_CommonDefine_H)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_CommonDefine_H
#pragma once



#include "boost/shared_ptr.hpp"
#include "ace/Singleton.h"
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

namespace TA_IRS_Core
{
	enum LineType{enum_UpBound, enum_DownBound, enum_Unknown};
	enum SegmentType{enum_SegmentU, enum_SegmentA, enum_SegmentB};
	enum TrainPositionByShaft{enum_TrainPos_Any,enum_TrainPos_Front,enum_TrainPos_Back};

	// define the line data
	#define UpBoundData static_cast<unsigned long>('U')
	#define DownBoundData static_cast<unsigned long>('D')

	//define the segment data
	#define SegmentA static_cast<unsigned long>('A')
	#define SegmentB static_cast<unsigned long>('B')

	LineType stringToLineType(const std::string& strValue)
	{
		FUNCTION_ENTRY("stringToLineType");
		using namespace std;
		LineType bRet = enum_UpBound;
		if (false == strValue.empty())
		{
			std::string temp = strValue;
			transform(temp.begin(), temp.end(), temp.begin(), (int (*)(int))toupper);
			if (UpBoundData == temp[0])
			{
				bRet = enum_UpBound;
			}
			else
			{
				bRet = enum_DownBound;
			}
		}

		FUNCTION_EXIT;
		return bRet;
	}

	std::string lineTypeToString(LineType lineType)
	{
		std::string strRet;
		if (lineType == enum_DownBound)
		{
			strRet = "D";
		} 
		else if (lineType == enum_UpBound)
		{
			strRet = "U";
		}
		return strRet;
	}

	TrainPositionByShaft stringToTrainPositionByShaft(const std::string& strTrain)
	{
		TrainPositionByShaft pos = enum_TrainPos_Any;
		if (!strTrain.empty())
		{
			switch (strTrain[0])
			{
			case 'A':
				pos = enum_TrainPos_Any;
				break;
			case 'F':
				pos = enum_TrainPos_Front;
				break;
			case 'B':
				pos = enum_TrainPos_Back;
				break;
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown TrainPositionByShaft:%s", strTrain.c_str());
			}
		}
		return pos;
	}

	std::string trainPositionByShaftToString(TrainPositionByShaft pos)
	{
		std::string strReturn;
		switch(pos)
		{
		case enum_TrainPos_Any:
			strReturn = "A";
			break;
		case enum_TrainPos_Front:
			strReturn = "F";
			break;
		case enum_TrainPos_Back:
			strReturn = "B";
			break;
		default:
			strReturn = "A";
		}
		return strReturn;
	}

	SegmentType stringToSegment(const std::string& strSegment)
	{
		SegmentType type = enum_SegmentU;
		if (!strSegment.empty())
		{
			if (strSegment[0] == 'A')
			{
				type = enum_SegmentA;
			}
			else if(strSegment[0] == 'B')
			{
				type = enum_SegmentB;
			}
			else
			{
				type = enum_SegmentU;
			}
		}
		return type;
	}

	std::string segmentToString(SegmentType type)
	{
		std::string strReturn;
		switch(type)
		{
		case enum_SegmentA:
			strReturn = "A";
			break;
		case enum_SegmentB:
			strReturn = "B";
			break;
		case enum_SegmentU:
			strReturn = "U";
			break;
		default:
			strReturn = "U";
		}
		return strReturn;
	}

	unsigned long stringToUnsignedLong(const std::string& strValue)
	{
		unsigned long retValue;
		std::stringstream ss;
		ss << strValue;
		ss >> retValue;
		return retValue;
	}
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_CommonDefine_H
