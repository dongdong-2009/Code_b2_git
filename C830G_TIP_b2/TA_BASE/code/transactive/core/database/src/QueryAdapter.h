#ifndef QUERYADAPTER_H
#define QUERYADAPTER_H

#include "core/database/src/CommonType.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace ocl
{
    class OraQuery;
}

namespace TA_Base_Core
{

class QueryAdapter
{
public:
    QueryAdapter( ocl::OraQuery* pQuery );
    virtual ~QueryAdapter();

    bool hasMore();

    void fetchInitial( int numColumns, Buffer& data, long lMaxRows );

    void fetchMore( int numColumns, Buffer& data, long lMaxRows );

    bool cleanQuery();

    bool isActive( unsigned long ulMaxTimeoutInSecs );

private:
    void fillDataBuffer( int numColumns, Buffer& data, long lMaxRows );
    void updateLatestActionTime();

private:
    // Disable default constructor, copy constructor, operator =
    QueryAdapter();
    const QueryAdapter& operator=( const QueryAdapter& refSrc );
    QueryAdapter( const QueryAdapter& refSrc );

private:
    ocl::OraQuery* m_pQuery;
    bool m_bConnectionClosed;

    time_t m_tmLatestActionTime;

    TA_Base_Core::ReEntrantThreadLockable m_queryLock;
};

}

#endif // ifndef QUERYADAPTER_H