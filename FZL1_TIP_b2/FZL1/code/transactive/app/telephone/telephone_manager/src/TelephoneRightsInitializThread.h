#ifndef TELEPHONERIGHTSINITIALIZTHREAD_H
#define TELEPHONERIGHTSINITIALIZTHREAD_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"

#include <string>
#include <boost/shared_ptr.hpp>


namespace TA_Base_Core
{
    class Semaphore;
}

namespace TA_Base_Bus
{
    class ItaRights;
}

struct TelephoneRights
{
    bool acaTelephoneConfigureDirectory;
    bool acaTelephoneSearchDirectory;
    bool acaTelephoneUploadDirectory;
};


class TelephoneRightsInitializThreadUserProcessRights
{
public:
    virtual void processTelephoneRights( const TelephoneRights& rights ) = 0;
};


class TelephoneRightsInitializThreadUserSetRights
{
public:
    virtual void setRights( boost::shared_ptr< TA_Base_Bus::ItaRights > rights ) = 0;
};


class TelephoneRightsInitializThread : public TA_Base_Core::Thread
{
public:
    TelephoneRightsInitializThread();
    ~TelephoneRightsInitializThread();
    void setSessionIdEntityKey( TelephoneRightsInitializThreadUserProcessRights& user, const std::string& sessionId, unsigned long entityKey );
    void setRightsSemaphore( TelephoneRightsInitializThreadUserSetRights& user2, TA_Base_Core::Semaphore& semaphore );
    virtual void run();
    virtual void terminate();
private:
    TelephoneRightsInitializThreadUserProcessRights* m_user;
    TelephoneRightsInitializThreadUserSetRights* m_user2;
    std::string m_sessionId;
    unsigned long m_entityKey;
    TA_Base_Core::Semaphore* m_semaphore;
};


#endif
