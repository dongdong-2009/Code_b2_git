
#ifndef AFX_ICURSELDVAMSGOBSERVER_H_INCLUDED_
#define AFX_ICURSELDVAMSGOBSERVER_H_INCLUDED_

class ICurSelDVAMsgObserver
{
public:
    virtual void processDVAMsgSelChanged( int nType ) = 0;
};

#endif