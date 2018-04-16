// DVABroadcast.h: interface for the DVABroadcast class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(BACKUPKEYMONITOR__INCLUDED_)
#define BACKUPKEYMONITOR__INCLUDED_

#include <string>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"

class BackupKeyMonitor : public TA_Base_Bus::IEntityUpdateEventProcessor
{
public:
    BackupKeyMonitor( unsigned long ulBackEntityKey );
    virtual ~BackupKeyMonitor();

    virtual void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

    void registerObserver( HWND hObserverWnd );
    void deregisterObserver();

    bool isBackupLockMode();

private:
    BackupKeyMonitor();
    const BackupKeyMonitor& operator=( const BackupKeyMonitor& );
    BackupKeyMonitor( const BackupKeyMonitor& );

    void createDatapointProxy( unsigned long ulBackEntityKey );
    void notifyObserver();
    void updateBackupKeyStatus();

private:
    TA_Base_Bus::DataPointProxySmartPtr m_spBackupKeyDatapoint;
    TA_Base_Core::NonReEntrantThreadLockable m_lockForObserver;
    HWND m_hObserver;
    int m_nBackupKeyStatus;
    bool m_bProxyNotGot;
};

#endif // !defined(BACKUPKEYMONITOR__INCLUDED_)
