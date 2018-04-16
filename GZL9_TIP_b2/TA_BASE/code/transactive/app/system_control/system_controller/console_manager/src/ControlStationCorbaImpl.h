#pragma once
#include "Singleton.h"
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/corba/src/ServantBase.h"

class ControlStationCorbaImpl
    : public virtual POA_TA_Base_Core::IControlStationCorbaDef,
      public virtual TA_Base_Core::ServantBase,
      public Singleton<ControlStationCorbaImpl>
{
public:

    ControlStationCorbaImpl();
    TA_Base_Core::IControlStationCorbaDef::RECT getRect(unsigned long targetScreen, unsigned long targetArea, long val);
    void launchAnApplication(unsigned long appType,
                             const char* extraCommandLine,
                             unsigned long posFlag,
                             unsigned long alignFlag,
                             const TA_Base_Core::IControlStationCorbaDef::RECT& objectDim,
                             const TA_Base_Core::IControlStationCorbaDef::RECT& boundaryDim) {}

    virtual void launchDisplay(const char* display, const char* asset, long leftPosition) {}
    void poll() {}

    virtual void activate();
    virtual void deactivate();

protected:

    ControlStationCorbaImpl(const ControlStationCorbaImpl& theControlStationCorbaImpl);
    ControlStationCorbaImpl& operator=(const ControlStationCorbaImpl& pm);
    void calculateMonitor(unsigned long& monitorNumber, unsigned long& monitorWidth, unsigned long& monitorHeight);

protected:

    unsigned long m_monitorNumber;
    unsigned long m_monitorWidth;
    unsigned long m_monitorHeight;
};
