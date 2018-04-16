#ifndef PASCHEDULEMANAGER_H__INCLUDED_
#define PASCHEDULEMANAGER_H__INCLUDED_

struct BroadcastScheduleInfo 
{
    std::string strName;
    time_t tmStartTime;
    time_t tmEndTime;
    int nFrequencyType;
    int nFrequencyDetail;
    bool bAutoDelete;
};

class PaScheduleManager
{
public:
    PaScheduleManager();
    virtual ~PaScheduleManager();

    void InsertNewSchedule( unsigned long ulKey, const std::string& strName, time_t tmStart, time_t tmEnd, int nFrequencyType, int nFrequencyDetail, bool bAutoDel );
    void EditSchedule( unsigned long ulKey, const std::string& strName, int nFrequencyType, int nFrequencyDetail, bool bAutoDel );

    std::string getFrequencyString( int nFrequencyType, int nFrequencyDetail );
    std::string getAutoDelString( bool bAutoDel );

protected:
    std::map<unsigned long, BroadcastScheduleInfo*> m_mapBroadcastScheduleInfos;
    BroadcastScheduleInfo* m_pCurEditSchedule;
};

#endif