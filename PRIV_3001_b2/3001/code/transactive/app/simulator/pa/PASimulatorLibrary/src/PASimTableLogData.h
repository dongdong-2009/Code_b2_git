#ifndef PA_SIM_TABLE_LOG_DATA_H
#define PA_SIM_TABLE_LOG_DATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableLogData.h $
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

#include <set>
#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"

namespace TA_IRS_App
{
namespace PA_Sim
{
    class PASimTableLogSupport;
    class PASimTable;
	class UserQuery;

    /** A class used by PASimTableQueryProcessor, to support data required
      * to log table data. Started life as a map and a struct and concomitant
      * functions embedded in the aforementioned class.
      */
    class PASimTableLogData
    {
    public:
        /** 
          * 
          */
        PASimTableLogData( UserQuery * query, PASimTableLogSupport * parent, PASimTable * t );
        
        /** Destructor */
        virtual ~PASimTableLogData( );

        /** 
          * 
          */
        const RecordRange & getRecordRange( ) const;

        /** 
          * 
          */
        void addRecordRange( const RecordRange & records );

        /** 
          * 
          */
        void removeRecordRange( const std::string & s );

        /** 
          * 
          */
        void removeRecordRange( const RecordRange & records );

        /** 
          * 
          */
        void setOutputFormat(TableOutputFormat f);

        /** 
          * 
          */
        TableOutputFormat getOutputFormat() const;

        /** 
          * 
          */
        void doLog();

    private:
        ///
        /// the table
        PASimTable * m_table;

        ///
        /// The ready-made "ShowTable" query. This is used to 
        UserQuery * m_userQuery;
        
        ///
        /// Parent object. For upward navigation to PASimTableQueryProcessor
        PASimTableLogSupport * m_parent;
    };
} // namespace PA_Sim
} // namespace TA_IRS_App

#endif // ifndef PA_SIM_TABLE_LOG_DATA

