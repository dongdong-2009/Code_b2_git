#pragma once

#include "VideoDisplayManagerPresenter.h"

namespace TA_IRS_App
{

class VideoDisplayManagerPresenterMODBUS : public VideoDisplayManagerPresenter
{

public:
    VideoDisplayManagerPresenterMODBUS(ILoadStationDiagramService * loadStationDiagramService);
    virtual ~VideoDisplayManagerPresenterMODBUS();
    virtual bool assignMonitor(Monitor* monitor, VideoInput * input);
    virtual bool clearMonitor(Monitor * monitor);
    virtual void onCompleteInitStation();
    virtual void OnTimer(UINT nEventID, VideoSourcesTreeCtrl * sequenceTreeCtrl = NULL);
    virtual void setUpMonitor();
};

}

