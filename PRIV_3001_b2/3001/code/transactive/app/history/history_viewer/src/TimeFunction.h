#if !defined(AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_)
#define AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_

/**
  * TimeFunction.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/history/history_viewer/src/TimeFunction.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class contains time conversion routines.
  * This class is a singleton.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>
#ifdef USE_OCCI
#include <occi.h>
#endif // USE_OCCI
#include <string>

namespace TA_App
{
    //
    // TimeFunction
    //
    class TimeFunction  
    {
    public:

        /**
		  * getInstance
		  * 
		  * Returns the one and only TimeFunction instance.
		  *
          * @return The TimeFunction instance
          *
		  */
        static TimeFunction& getInstance()
        {
	        static TimeFunction e;
	        return e;
        }

        /**
		  * ~TimeFunction
		  * 
		  * Destructor.
		  *
          */
        virtual ~TimeFunction();

        /**
		  * ConvertToUTC
		  * 
		  * Convert a local COleDateTime into a UTC string.
          *
          * @param inTime - Local time to convert
		  *
          * @return The UTC string
          *
		  */
        CString ConvertToUTC(const COleDateTime& inTime);
    
    #ifdef USE_OCCI
        
        /**
		  * ConvertToLocal
		  * 
		  * Convert from a UTC oracle Date in a occi::Date to a local COleDateTime.
		  *
          * @param inDate - Oracle date
          * @param outTime - Converted time
          *
          */
        void ConvertToLocal(const oracle::occi::Date& inDate, COleDateTime& outTime);

    #endif // USE_OCCI

        /**
		  * ConvertToLocal
		  * 
		  * Convert from a UTC oracle Date in a std::string to a local COleDateTime.
		  *
          * @param inDate - Oracle date
          * @param outTime - Converted time
          *
          */
        void ConvertToLocal(const std::string& inDate, COleDateTime& outTime);

        /**
		  * ConvertToLocal
		  * 
		  * Convert from a time_t Date to a local COleDateTime.
		  *
          * @param inDate - Time_t date
          * @param outTime - Converted time
          *
		  */
        void ConvertToLocal(time_t inDate, COleDateTime& outTime);

    private:
        
        TimeFunction();
        TimeFunction(TimeFunction&) {}
        TimeFunction& operator=(const TimeFunction &) {}

        time_t ParseDateTime(const std::string& inDate);

    private:

        TIME_ZONE_INFORMATION m_TimeZoneInformation;
    };

} // TA_App

#endif // !defined(AFX_TIMEFUNCTION_H__20B53633_9856_11D6_B25D_0050BAB25370__INCLUDED_)




