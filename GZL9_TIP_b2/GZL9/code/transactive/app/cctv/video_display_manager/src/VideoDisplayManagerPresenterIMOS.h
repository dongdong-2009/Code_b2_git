#pragma once

#include "VideoDisplayManagerPresenter.h"

namespace TA_IRS_App
{

class VideoDisplayManagerPresenterIMOS : public VideoDisplayManagerPresenter
{

public:
    VideoDisplayManagerPresenterIMOS(ILoadStationDiagramService * loadStationDiagramService);
    virtual ~VideoDisplayManagerPresenterIMOS();
    virtual bool assignMonitor(Monitor* monitor, VideoInput * input);
    virtual bool clearMonitor(Monitor * monitor);
    virtual void onCompleteInitStation();
    virtual void OnTimer(UINT nEventID, VideoSourcesTreeCtrl * sequenceTreeCtrl = NULL);
    virtual void setUpMonitor();
};

}
