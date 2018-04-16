#ifndef ATS_SIM_TABLE_LOG_DATA_H
#define ATS_SIM_TABLE_LOG_DATA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableLogData.h $
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
#include "app/ats/ATSSimulatorLibrary/src/ATSSimCommon.h"

namespace TA_IRS_App
{
namespace ATS_Sim
{
    class ATSSimTableLogSupport;
    class ATSSimTable;
	class UserQuery;

    /** A class used by ATSSimTableQueryProcessor, to support data required
      * to log table data. Started life as a map and a struct and concomitant
      * functions embedded in the aforementioned class.
      */
    class ATSSimTableLogData
    {
    public:
        /** 
          * 
          */
        ATSSimTableLogData( UserQuery * query, ATSSimTableLogSupport * parent, ATSSimTable * t );
        
        /** Destructor */
        virtual ~ATSSimTableLogData( );

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
        ATSSimTable * m_table;

        ///
        /// The ready-made "ShowTable" query. This is used to 
        UserQuery * m_userQuery;
        
        ///
        /// Parent object. For upward navigation to ATSSimTableQueryProcessor
        ATSSimTableLogSupport * m_parent;
    };
} // namespace ATS_Sim
} // namespace TA_IRS_App

#endif // ifndef ATS_SIM_TABLE_LOG_DATA

