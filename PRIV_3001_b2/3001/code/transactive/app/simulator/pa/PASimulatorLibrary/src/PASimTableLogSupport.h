#ifndef PA_SIM_TABLE_LOG_SUPPORT_H
#define PA_SIM_TABLE_LOG_SUPPORT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableLogSupport.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  */
#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32

#include <map>
#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"

namespace TA_IRS_App
{
namespace PA_Sim
{

    class PASimTableLogData;
    class PASimTableQueryProcessor;
    class PASimTable;
    class UserQuery;
    class UserResponse;
    /** A class used by PASimTableQueryProcessor, to support data required
      * to log table data. Started life as a map and a struct and concomitant
      * functions embedded in the aforementioned class.
      */
    class PASimTableLogSupport
    {
    public:
        PASimTableLogSupport(PASimTableQueryProcessor * parent);
        
        virtual ~PASimTableLogSupport();

        /** 
          * 
          */
        UserResponse * processLogQuery( UserQuery * query );


        /** 
          * 
          */
        void doLogTable( PASimTable * t );


        /** 
          * 
          */
        PASimTableQueryProcessor * getParent() const;

    private:
        ///
        /// Type definition of map of tables' records to log
        typedef std::map< int, PASimTableLogData * > LogDataMap;

        ///
        /// map of information required for login
        LogDataMap m_logDataMap;

        ///
        /// Parent object. For upward navigation to PASimTableQueryProcessor
        PASimTableQueryProcessor * m_parent;

        ///
        ///
        TableOutputFormat m_outputFormat;
    };
} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // ifndef PA_SIM_TABLE_LOG_SUPPORT

