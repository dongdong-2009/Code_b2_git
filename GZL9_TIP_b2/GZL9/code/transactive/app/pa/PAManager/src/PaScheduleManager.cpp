#include "app/pa/PAManager/src/stdafx.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PaScheduleManager.h"

PaScheduleManager::PaScheduleManager() : 
m_mapBroadcastScheduleInfos(),
m_pCurEditSchedule( NULL )
{
}

PaScheduleManager::~PaScheduleManager()
{
}

void PaScheduleManager::InsertNewSchedule( unsigned long ulKey, const std::string& strName, time_t tmStart, time_t tmEnd, int nFrequencyType, int nFrequencyDetail, bool bAutoDel )
{
    BroadcastScheduleInfo* pNewSchedule = new BroadcastScheduleInfo();
    if ( NULL == m_pCurEditSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_FAILED_TO_CREATE_INSTANCE );
        return;
    }

    pNewSchedule->strName = strName;
    pNewSchedule->tmStartTime = tmStart;
    pNewSchedule->tmEndTime = tmEnd;
    pNewSchedule->nFrequencyType = nFrequencyType;
    pNewSchedule->nFrequencyDetail = nFrequencyDetail;
    pNewSchedule->bAutoDelete = bAutoDel;

    m_mapBroadcastScheduleInfos[ulKey] = pNewSchedule;
}

void PaScheduleManager::EditSchedule( unsigned long ulKey, const std::string& strName, int nFrequencyType, int nFrequencyDetail, bool bAutoDel )
{
    BroadcastScheduleInfo* pExistSchedule = m_mapBroadcastScheduleInfos[ulKey];
    if ( NULL == pExistSchedule )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CURRENT_EDIT_SCHEDULE_NOT_EXIST );
        return;
    }

    pExistSchedule->strName = strName;
    pExistSchedule->nFrequencyType = nFrequencyType;
    pExistSchedule->nFrequencyDetail = nFrequencyDetail;
    pExistSchedule->bAutoDelete = bAutoDel;
}

std::string PaScheduleManager::getFrequencyString( int nFrequencyType, int nFrequencyDetail )
{
    if (( FREQUENCY_ONCE > nFrequencyType ) || ( FREQUENCY_YEAR < nFrequencyType ))
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
        return std::string("");
    }

    std::string strFrequency = "";
    if (( FREQUENCY_WEEK != nFrequencyType ) && ( FREQUENCY_MONTH != nFrequencyType ))
    {
        strFrequency = SCHEDULE_FREQUENCY_LIST[nFrequencyType];
    }
    else if ( FREQUENCY_WEEK == nFrequencyType )
    {
        std::string strDayOfWeek = "";
        switch ( nFrequencyDetail )
        {
            case 0:
            {
                strDayOfWeek = "日";
                break;
            }
            case 1:
            {
                strDayOfWeek = "一";
                break;
            }
            case 2:
            {
                strDayOfWeek = "二";
                break;
            }
            case 3:
            {
                strDayOfWeek = "三";
                break;
            }
            case 4:
            {
                strDayOfWeek = "四";
                break;
            }
            case 5:
            {
                strDayOfWeek = "五";
                break;
            }
            case 6:
            {
                strDayOfWeek = "六";
                break;
            }
            default:
            {
                PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
                break;
            }
        }

        if ( strDayOfWeek.empty() )
        {
            return std::string("");
        }
        strFrequency = SCHEDULE_FREQUENCY_LIST[nFrequencyType] + strDayOfWeek;
    }
    else if ( FREQUENCY_MONTH == nFrequencyType )
    {
        if (( 0 > nFrequencyDetail ) || ( DAYS_OF_MONTH_COUNT < nFrequencyDetail ))
        {
            PaErrorHelper::logErrorMsg( SourceInfo, ERROR_INVALID_INPUT_PARAMETER );
            return std::string("");
        }
        strFrequency = SCHEDULE_FREQUENCY_LIST[nFrequencyType] + VariableConvertor::intergerToString( nFrequencyDetail ) + "日";
    }

    return strFrequency;
}

std::string PaScheduleManager::getAutoDelString( bool bAutoDel )
{
    std::string strAuto = ( bAutoDel ? "是" : "否" );
    return strAuto;
}
