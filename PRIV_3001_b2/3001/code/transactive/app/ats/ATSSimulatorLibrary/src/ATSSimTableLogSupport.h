#ifndef ATS_SIM_TABLE_LOG_SUPPORT_H
#define ATS_SIM_TABLE_LOG_SUPPORT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableLogSupport.h $
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
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{

    class ATSSimTableLogData;
    class ATSSimTableQueryProcessor;
    class ATSSimTable;
    class UserQuery;
    class UserResponse;
    /** A class used by ATSSimTableQueryProcessor, to support data required
      * to log table data. Started life as a map and a struct and concomitant
      * functions embedded in the aforementioned class.
      */
    class ATSSimTableLogSupport
    {
    public:
        ATSSimTableLogSupport(ATSSimTableQueryProcessor * parent);
        
        virtual ~ATSSimTableLogSupport();

        /** 
          * 
          */
        UserResponse * processLogQuery( UserQuery * query );


        /** 
          * 
          */
        void doLogTable( ATSSimTable * t );


        /** 
          * 
          */
        ATSSimTableQueryProcessor * getParent() const;

    private:
        ///
        /// Type definition of map of tables' records to log
        typedef std::map< int, ATSSimTableLogData * > LogDataMap;

        ///
        /// map of information required for login
        LogDataMap m_logDataMap;

        ///
        /// Parent object. For upward navigation to ATSSimTableQueryProcessor
        ATSSimTableQueryProcessor * m_parent;

        ///
        ///
        TableOutputFormat m_outputFormat;
    };
} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // ifndef ATS_SIM_TABLE_LOG_SUPPORT

