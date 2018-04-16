#if !defined(AFX_ZIPANDCLEARLOGS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_)
#define AFX_ZIPANDCLEARLOGS_H__6FE842D6_6799_4B54_9CEA_F03424267ACD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core\threads\src\Thread.h"
#include <boost/filesystem.hpp>


class CLogsThread : public TA_Base_Core::Thread
{
public:
    CLogsThread( const boost::filesystem::path& dir );
    ~CLogsThread();
    
    virtual void run();
    virtual void terminate() {}
    
private:

    const boost::filesystem::path m_dir;
};

#endif
