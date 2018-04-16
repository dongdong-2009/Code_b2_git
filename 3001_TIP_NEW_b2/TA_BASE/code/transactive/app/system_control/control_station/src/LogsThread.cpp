// ApplyRights.cpp: implementation of the CApplyRights class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "app\system_control\control_station\src\LogsThread.h"
#include "ZipArchive.h"
#include "core\utilities\src\FunctionEntry.h"

#include "core\utilities\src\DebugUtil.h"
#include <boost/filesystem.hpp>


using TA_Base_Core::DebugUtil;


CLogsThread::CLogsThread( const boost::filesystem::path& dir )
    : m_dir( dir )
{
}


CLogsThread::~CLogsThread()
{
    terminateAndWait();
}


void CLogsThread::run()
{
    FUNCTION_ENTRY("LogsThread::run");

    CZipArchive zip;

    try
    {
        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "CLogsThread::run - archive %s", m_dir.string().c_str() );

        zip.Open( ( m_dir.string() + ".zip" ).c_str(), CZipArchive::zipCreate );

        boost::filesystem::directory_iterator it( m_dir );
        boost::filesystem::directory_iterator end;

        for ( ; it != end; ++it )
        {
            zip.AddNewFile( it->string().c_str(), -1, false );
        }

        zip.Close();

        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo, "CLogsThread::run - remov %s", m_dir.string().c_str() );

        remove_all( m_dir );
    }
    catch ( CException* ex )
    {
        LOG_GENERIC( SourceInfo,DebugUtil::DebugError, "CLogsThread::run - CException" );
    }
    catch ( std::exception& ex )
    {
        LOG_GENERIC( SourceInfo,DebugUtil::DebugError, "CLogsThread::run - std::exception: %s", ex.what() );
    }
    catch ( ... )
    {
        LOG_GENERIC( SourceInfo,DebugUtil::DebugError, "CLogsThread::run - unknown exception" );
    }

    FUNCTION_EXIT;
}
