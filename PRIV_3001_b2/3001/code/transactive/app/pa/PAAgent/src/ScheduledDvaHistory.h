/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/ScheduledDvaHistory.h $
 * @author  HuangQi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef SCHEDULEDDVAHISTORY_H
#define SCHEDULEDDVAHISTORY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <set>
#include <map>
#include <vector>
#include "ace/RW_Mutex.h"

namespace TA_Base_Core
{
	class IPaScheduleBroadcast;
	class IConfigPaScheduleBroadcast;
}

namespace TA_IRS_App
{

struct ScheduleDvaConfig;

typedef std::map<unsigned short, ScheduleDvaConfig> EventIdToScheduledDvaMap;
typedef std::map<unsigned short, ScheduleDvaConfig>::iterator EventIdToScheduledDvaMapIt;

class ScheduledDvaHistory
{
public:
    ScheduledDvaHistory();
    virtual ~ScheduledDvaHistory();

    void storeScheduledDva( const ScheduleDvaConfig& refParam );
    void removeScheduledDva( const unsigned short usEventId );
	std::vector<ScheduleDvaConfig> getAllScheduledDva();

private:
	ScheduledDvaHistory( const ScheduledDvaHistory& );
    const ScheduledDvaHistory& operator=( const ScheduledDvaHistory& );

    void reloadScheduledHistory();
	void convertScheduleDvaType(TA_Base_Core::IPaScheduleBroadcast& refSource, ScheduleDvaConfig& refTarget );
	void convertScheduleDvaType(const ScheduleDvaConfig& refSource, TA_Base_Core::IConfigPaScheduleBroadcast& refTarget );

protected:
    EventIdToScheduledDvaMap m_mapDvaParams;
    mutable ACE_RW_Mutex m_mtxDvaParamMap;
};

} // namespace TA_IRS_App

#endif // SCHEDULEDDVAHISTORY_H
