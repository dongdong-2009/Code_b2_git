
#ifndef AFX_IDVAMSGSHORTCUTCONFIGOBSERVER_H_INCLUDED_
#define AFX_IDVAMSGSHORTCUTCONFIGOBSERVER_H_INCLUDED_

class IDVAMsgShortcutConfigObserver
{
public:
    virtual void processDVAMsgShortcutChanged( unsigned long ulShortcutIndex ) = 0;
};

#endif