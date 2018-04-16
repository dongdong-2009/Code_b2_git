/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/GlobalStructDefine.h $
 * @author  HuangQi
 * @version $Revision: #9 $
 * Last modification : $DateTime: 2014/03/18 16:53:46 $
 * Last modified by : $Author: huangjian $
 *
 * 
 */

#ifndef GLOBALSTRUCTDEFINE_H
#define GLOBALSTRUCTDEFINE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

namespace TA_IRS_App
{
    struct StdPasZoneId
    {
        unsigned short usStationId;
        unsigned short usZoneId;
    };

    struct StdStationDvaConfig
    {
        std::vector<unsigned long> vecZoneKeys;
        //unsigned long ulMsgKey;
		std::vector<unsigned long> vecMsgKeys;
        std::vector<unsigned short> vecMsgIds;
        unsigned short dwellInSecs;
        unsigned short usScheduleId;
		unsigned long ulOriginalStartTime;
        unsigned short usStartTime;
        unsigned short usStartSecond;
		unsigned long ulOriginalEndTime;
        unsigned short usEndTime;
        unsigned short usEndSecond;
        unsigned short usInterval;
        unsigned short usIntervalSecond;
        unsigned short usDaysInWeek;
        unsigned short usSourceId; // for playback Dva
        bool isSynchronisedWithTis;
        bool bScheduled;
        bool hasChime;
        bool isLocalPlayback;
    };

    struct StdStationLiveConfig
    {
		bool bWithChime;
        unsigned short usSourceId;
        std::vector<unsigned long> vecZoneKeys;
    };

    struct StdStationRecordingConfig
    {
        unsigned long ulMsgKey;
        unsigned short usSourceId;
        unsigned short usMsgId;
        unsigned short usMsgType;
    };

    struct ScheduleDvaConfig
    {
        std::vector<unsigned long> vecZoneKeys;
        //unsigned long ulMsgKey;
		std::vector<unsigned long> vecMsgKeys;
        unsigned long ulStartTime;
        unsigned long ulEndTime;
        unsigned long ulInterval;
        unsigned short usScheduleId;
        bool bSun;
        bool bMon;
        bool bTue;
        bool bWed;
        bool bThu;
        bool bFri;
        bool bSat;
        bool bChime;
    };

    struct StdStationMusicConfig
    {
        std::vector<unsigned long> vecZoneKeys;
        unsigned short usSourceId;
        int musicType;
    };
}


#endif // GLOBALSTRUCTDEFINE_H