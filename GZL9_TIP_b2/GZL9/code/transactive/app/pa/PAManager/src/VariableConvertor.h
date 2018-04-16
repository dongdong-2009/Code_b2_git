#ifndef VARIABLECONVERTOR__INCLUDE_
#define VARIABLECONVERTOR__INCLUDE_

#include <iomanip>


enum TIME_FORMAT_TYPE
{
    E_TIME_FORMAT_DAYS = 0, // "2010-04-01"
    E_TIME_FORMAT_SECONDS, // "22:22:22"
    E_TIME_FORMAT_FULL, // "2010-04-01 22:22:22"
};

interface VariableConvertor
{
    static unsigned long stringToUnsignedLong( const std::string& strSrc )
    {
        unsigned long ulDes = 0;

        std::stringstream stmConvertor;
        stmConvertor << strSrc;
        stmConvertor >> ulDes;

        return ulDes;
    }

    static std::string unsignedLongToString( unsigned long ulSrc, int nFill = 0 )
    {
        std::stringstream stmConvertor;
        if ( 0 == nFill )
        {
            stmConvertor << ulSrc;
        }
        else if ( 0 < nFill )
        {
            stmConvertor << std::setw(3) << std::setfill('0') << ulSrc;
        }

        return stmConvertor.str();
    }

    static std::string intergerToString( int nSrc )
    {
        std::stringstream stmConvertor;
        stmConvertor << nSrc;

        return stmConvertor.str();
    }

    static int stringToInterger( const std::string& strSrc )
    {
        int nDes = 0;

        std::stringstream stmConvertor;
        stmConvertor << strSrc;
        stmConvertor >> nDes;

        return nDes;
    }

    // max 32 time_t is 03:14:07 January 19, 2038 
    static std::string timeToString( time_t tmSrc, TIME_FORMAT_TYPE eTImeFormat )
    {
        // Do not delete the return pointer, it is define in c library as static pointer
        tm* pTmSrc = localtime( &tmSrc );

        std::stringstream stmConvertor;
        if ( NULL == pTmSrc )
        {
            return stmConvertor.str();
        }

        stmConvertor << std::dec;
        switch ( eTImeFormat )
        {
        case E_TIME_FORMAT_DAYS:
            stmConvertor << (pTmSrc->tm_year + 1900) << "-";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << (pTmSrc->tm_mon + 1) << "-";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_mday;
            break;
        case E_TIME_FORMAT_SECONDS:
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_hour << ":"; 
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_min<< ":";
            stmConvertor << std::setw(2) << std::setfill( '0' )  << pTmSrc->tm_sec;
            break;
        case E_TIME_FORMAT_FULL:
            stmConvertor << (pTmSrc->tm_year + 1900) << "-";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << (pTmSrc->tm_mon + 1) << "-";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_mday;
            stmConvertor << " ";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_hour << ":"; 
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_min<< ":";
            stmConvertor << std::setw(2) << std::setfill( '0' ) << pTmSrc->tm_sec;
            break;
        default:
            break;
        }

        return stmConvertor.str();
    }

    static time_t systemTimeToUnixTime( SYSTEMTIME& stmSrc )
    {
        tm stConvertor = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        stConvertor.tm_year = stmSrc.wYear - 1900;
        stConvertor.tm_mon = stmSrc.wMonth - 1;
        stConvertor.tm_mday = stmSrc.wDay;
        stConvertor.tm_hour = stmSrc.wHour;
        stConvertor.tm_min = stmSrc.wMinute;
        stConvertor.tm_sec = stmSrc.wSecond;
        stConvertor.tm_wday = stmSrc.wDayOfWeek;

        return mktime( &stConvertor );
    }
};

#endif