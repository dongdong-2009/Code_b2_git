#pragma once
#include "DlgVideoPlayImos.h"
#include "IPTZCommandListener.h"
#include "CCTVCommon.h"

namespace TA_IRS_App
{
class IPTZControlView;
class VideoDisplayManagerPresenter;
class StationDlgVideoPlayImos : public DlgVideoPlayImos,
            public IPTZCommandListener
{
    DECLARE_DYNAMIC(StationDlgVideoPlayImos)

public:
    StationDlgVideoPlayImos(TA_IRS_App::Monitor* local,
                            const VirtualQuadArray& quadArray,
                            IPTZControlView* ptzCtrl,
                            CWnd* pParent);
    virtual ~StationDlgVideoPlayImos();
public:
    virtual bool assignMonitor(TA_IRS_App::Monitor* monitor, TA_IRS_App::VideoInput* input);
    // IPTZCommandListener
    virtual bool onPTCommand(unsigned short ptDirection, bool isStopCommand);
    virtual bool onZoomCommand(unsigned short in, bool isStopCommand);
    virtual bool onPanCommand(unsigned short panDirection, bool isStopCommand);
    virtual bool onTiltCommand(unsigned short tiltDirection, bool isStopCommand);
    virtual std::string getErrorMessage();
protected:
    virtual void CheckShowOrHideWindow();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
private:
    virtual bool assignVideoToLocal(TA_IRS_App::VideoInput* videoInput);
    virtual bool assignVideoToQuad(TA_IRS_App::VideoInput* videoInput, int quadNumber); // quadNumber = 1 mean quad1
    // quad, 4 monitor
    /*
    -------------
    |  1  |  2  |
    -------------
    |  3  |  4  |
    -------------
    */
    VirtualQuadArray m_quad; // index 0 mean quad 1,... index 3 mean quad 4
    TA_IRS_App::Monitor* m_localMonitor;
    std::string m_strError;
    IPTZControlView* m_ptzCtrl;
    bool m_isQuadMode;
};
}