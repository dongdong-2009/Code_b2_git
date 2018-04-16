#pragma once;
#include "app/response_plans/plan_step_plugin/common/src/ParameterType.h"
#include <time.h>
#include <sstream>

class TimeConvertHelper
{
public:
    static const unsigned long MIN_SECONDS;
    static const unsigned long HOUR_SECONDS;
    static const unsigned long DAY_SECONS;
    static const time_t INVALID_TIME;

    static std::string converToDisplayValue( const unsigned long paramTime, ParamType timeType )
    {
        time_t realTime = convertToRealTime( paramTime, timeType );
        if( INVALID_TIME == realTime )
        {
            return "";
        }

        tm tmTime;
        localtime_s( &tmTime, &realTime );

        std::stringstream ss;

        switch( timeType )
        {
            case eHhMmSsTime:
                ss << tmTime.tm_hour << "h " << tmTime.tm_min << "m " << tmTime.tm_sec << "s";
                return ss.str();
            case eHhMmTime:
                ss << tmTime.tm_hour << "h " << tmTime.tm_min << "m";
                return ss.str();
            case eMmSsTime:
                ss << tmTime.tm_min << "m " << tmTime.tm_sec << "s";
                return ss.str();
            case eSsTime:
                ss << tmTime.tm_sec << "s";
                return ss.str();
            case eYyMmDdTime: // Make no sense, what's paramTime mean when time exceed one day?
            case eMmDdTime:
            case eFullTime:
            default:
                break;
        }

        return "";
    }

    static time_t convertToRealTime( const unsigned long paramTime, ParamType timeType )
    {
        time_t realTime = time( NULL );
        tm tmTime;
        localtime_s( &tmTime, &realTime );

        switch( timeType )
        {
            case eHhMmSsTime:
                tmTime.tm_hour = paramTime / HOUR_SECONDS;
                tmTime.tm_min = ( paramTime % HOUR_SECONDS ) / MIN_SECONDS;
                tmTime.tm_sec = paramTime % MIN_SECONDS;
                return mktime( &tmTime );
            case eHhMmTime:
                tmTime.tm_hour = paramTime / HOUR_SECONDS;
                tmTime.tm_min = ( paramTime % HOUR_SECONDS ) / MIN_SECONDS;
                tmTime.tm_sec = 0;
                return mktime( &tmTime );
            case eMmSsTime:
                tmTime.tm_hour = 0;
                tmTime.tm_min = ( paramTime % HOUR_SECONDS ) / MIN_SECONDS;
                tmTime.tm_sec = paramTime % MIN_SECONDS;
                return mktime( &tmTime );
            case eSsTime:
                tmTime.tm_hour = 0;
                tmTime.tm_min = 0;
                tmTime.tm_sec = paramTime % MIN_SECONDS;
                return mktime( &tmTime );
            case eYyMmDdTime: // Make no sense, what's paramTime mean when time exceed one day?
            case eMmDdTime:
            case eFullTime:
            default:
                break;
        }

        return INVALID_TIME;
    }

    static unsigned long convertToParamTime( const time_t realTime, ParamType timeType )
    {
        tm tmTime;
        localtime_s( &tmTime, &realTime );

        switch( timeType )
        {
            case eHhMmSsTime:
                return tmTime.tm_hour * HOUR_SECONDS + tmTime.tm_min * MIN_SECONDS + tmTime.tm_sec;
            case eHhMmTime:
                return tmTime.tm_hour * HOUR_SECONDS + tmTime.tm_min * MIN_SECONDS;
            case eMmSsTime:
                return tmTime.tm_min * MIN_SECONDS + tmTime.tm_sec;
            case eSsTime:
                return tmTime.tm_sec;
            case eYyMmDdTime:
            case eMmDdTime:
            case eFullTime:
            default:
                break;
        }

        return -1;
    }
};

__declspec( selectany ) const unsigned long TimeConvertHelper::MIN_SECONDS = 60u;
__declspec( selectany ) const unsigned long TimeConvertHelper::HOUR_SECONDS = 3600u;
__declspec( selectany ) const unsigned long TimeConvertHelper::DAY_SECONS = TimeConvertHelper::HOUR_SECONDS * 24u;
__declspec( selectany ) const time_t TimeConvertHelper::INVALID_TIME = -1;


