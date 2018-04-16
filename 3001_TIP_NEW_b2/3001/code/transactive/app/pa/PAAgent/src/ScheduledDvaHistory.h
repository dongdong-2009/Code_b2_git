/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/ScheduledDvaHistory.h $
 * @author  HuangQi
 * @version $Revision: #6 $
 * Last modification : $DateTime: 2015/03/10 09:59:07 $
 * Last modified by : $Author: huang.wenbo $
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
