/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef SCHEDULEBROADCASTMODEL_H__INCLUDED_
#define SCHEDULEBROADCASTMODEL_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include <list>

namespace TA_Base_Core
{
    class IPaScheduleBroadcast;
}

interface IScheduleBroadcastGUI
{
public:
    virtual int insertScheduleToList( unsigned long ulKey, const std::string& strName, const std::string& strStartTime, const std::string& strStatus ) = 0;
    virtual int updateScheduleStateToList( unsigned long ulSchedulekey, const std::string& strStatus ) = 0;
    virtual int deleteScheduleFromList( unsigned long ulSchedulekey ) = 0;
    virtual int updateBroadcastParamContent( const std::string& strMsg, const std::string& strCylicTimes, const std::vector<std::string>& vecZones ) = 0;
    virtual unsigned long getScheduleKeyByListItemIndex( int nIndex ) = 0;
    virtual void clearScheduleList() = 0;
};

struct ScheduleBroadcastDetail 
{
    unsigned long 	ulPkeyInDb;
    unsigned long 	ulDvaMsgKey;
    unsigned long 	ulCyclicTimes;
    std::string 	strName;
    std::string 	strStatus;
    std::vector<unsigned long> lstZoneKeys;
    time_t 			tmStartTime;
};

class ScheduleBroadcastModel
{
public:
	ScheduleBroadcastModel( IScheduleBroadcastGUI& refGUI );
    virtual ~ScheduleBroadcastModel();

    int updateSchedulesFromDatabase();
    int updateScheduleList();
    int updateScheduleListCurSel( int nCurSel );
    int onScheduleListItemDeleted( int nItemDeleted );
    int removeSchedule();
    int getScheduleListCurSel();
    void initialise();
    void scheduleBroadcastUpdate( ScheduleBroadcastDetail* pDetail, int nUpdateType );
    void onScheduleCreate( ScheduleBroadcastDetail* pDetail );
    void onScheduleRemove( ScheduleBroadcastDetail* pDetail );
    void onScheduleModify( ScheduleBroadcastDetail* pDetail );

private:
    void clearLocalScheduleMap();
    int insertScheduleToList( ScheduleBroadcastDetail* pSchedule );
    std::string getScheduleStatus( ScheduleBroadcastDetail* pSchedule );
    void formatBroadcastParams( ScheduleBroadcastDetail* pSchedule, std::string& strMsg, std::string& strCylicTimes, std::vector<std::string>& vecZones );

private:
    // Disable default constructor, copy constructor and operator=
    ScheduleBroadcastModel();
    const ScheduleBroadcastModel& operator=( const ScheduleBroadcastModel& );
    ScheduleBroadcastModel( const ScheduleBroadcastModel& );

private:
    typedef std::map<unsigned long, ScheduleBroadcastDetail*> KeyToScheduleMap;
    typedef std::map<unsigned long, ScheduleBroadcastDetail*>::iterator KeyToScheduleMapIt;

    KeyToScheduleMap m_mapKeyToSchedule;
    IScheduleBroadcastGUI& m_refGUI;
    int m_nCurSelScheduleIndex;
};

//{{AFX_INSERT_LOCATION}}

#endif // ifndef SCHEDULEBROADCASTMODEL_H__INCLUDED_

