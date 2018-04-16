#pragma once

#include "ILoadStationDiagramService.h"
#include "IGFVideoSwitchDisplayView.h"
#include "DisplayItemManager.h"
#include "ISequenceEditorView.h"
#include "ISwitchAgentService.h"
#include <vector>
#include "MonitorDisplayItem.h"
#include "IMonitorAssignChangeObserver.h"

const static std::string    GROUP_NAME_GROUP_SEQUNCE = "GROUP_SQ";
const static std::string    GROUP_SEQ_ID1 = "201";
const static std::string    GROUP_SEQ_ID2 = "202";

const static unsigned int HALF_MINUTE_EVENT = 1;
const static unsigned int ONE_MINUTE_EVENT  = 2;

const static long SCHEMATIC_OVERVIEW_LEVEL = 0;
const static long SCHEMATIC_STATION_OFFICE_LEVEL = 1;
const static long SCHEMATIC_UP_PLT_LEVEL = 2;
const static long SCHEMATIC_DOWN_PLT_LEVEL = 3;
const static long SCHEMATIC_B1_LEVEL = 4;
const static long SCHEMATIC_B2_LEVEL = 5;


namespace TA_IRS_App
{

class VideoSourcesTreeCtrl;
class IMonitorAssigner;
class VideoDisplayManagerPresenter : public IMonitorAssignChangeObserver
{

public:
    VideoDisplayManagerPresenter(ILoadStationDiagramService * loadStationDiagramService);
    virtual ~VideoDisplayManagerPresenter(void);
    virtual bool assignMonitor(Monitor * monitor, VideoInput * input) = 0;
    virtual bool clearMonitor(Monitor * monitor) = 0;
    virtual void onCompleteInitStation() = 0;
    virtual void OnTimer(UINT nEventID, VideoSourcesTreeCtrl * sequenceTreeCtrl = NULL) = 0;
    virtual void setUpMonitor() = 0;

    void SetIView(IGFVideoSwitchDisplayView * view);
    void SetSeqEditorView(ISequenceEditorView* seqEditorview);
    void setMonitorAssigner(IMonitorAssigner* monitorAssigner);
    //  void setSequenceConfig( Sequence* sequence,unsigned short dwellTime,const std::list< Camera* > cameraInputs,bool con );
    void saveSequenceConfig(Sequence* sequence, unsigned short dwellTime, const std::list< Camera* > cameraInputs);

    void setSequenceDescription(Sequence* sequence,
                                const std::string& newDescription);

    // level: 0->over view, 1->station office, 2->up plt, 3->down plt, 4->b1, 5->b2
    void loadStationDiagram(unsigned long level = SCHEMATIC_OVERVIEW_LEVEL);
    void SelectSequence(unsigned long sequenceId);
    void SelectStation();
    void setSwtichAgentService(ISwitchAgentService * agentService);
    /*
      invoked by view to edit sequence
    */
    void editSequence(unsigned long sequenceId);
    BOOL isOcc();
    void MainUILButtonDown(CPoint& point);

    void MainUILButtonUp(CPoint& point);

    void MainUIMouseMove(CPoint& point);

    void MainUIContextMenu(CPoint& point);

    //void MainUIPosMove(int x, int y);

    MonitorDisplayItem* getOverMonitor(CPoint& point);

    void onRunParamChange(const std::string& name, const std::string& value);

    void beginDrage(CPoint & point , CImageList * image);

    void loadMonitorRelationship();

    /*
      operator select the camera from the list box of sequence
    */
    void selectCameraFromSequenceList();
    bool IsPanning();
    bool IsTilting();
    void closeSchematic();
    void stopSchematic();
    void refreshSchematic();
    void playVideo();
    void stopVideo();
    // IMonitorAssignChangeObserver
    virtual void monitorAssignChanged(Monitor* monitor);

protected:
    void invalidateControlLabels();
    void CheckSequeceUpdate(VideoSourcesTreeCtrl* sequenceTreeCtrl);
    void updateMonitor();
    void updateMonitorView();
    MonitorDisplayItem* getMonitor(unsigned long monitorId);
    void handleTransactiveException(TA_Base_Core::TransactiveException& te);
    void handleVideoSwitchException(TA_Base_Bus_GZL9::VideoSwitchAgentException& te);
    Camera * getSelectedCamera();
    void handleHighLightMonitor(MonitorDisplayItem * item);
    BOOL isCameraSelected();
    MonitorDisplayItem * getMonitorDisplayItemByIndex(unsigned short index);

    ILoadStationDiagramService* m_LoadStationDiagramService;
    IGFVideoSwitchDisplayView* m_IView;
    IMonitorAssigner* m_monitorAssigner;
    // SequenceEditorDlg  m_seqEditorView;
    unsigned long    m_currentSeqID;
    ISequenceEditorView* m_seqEditorView;
    ISwitchAgentService* m_agentService;
    std::vector < MonitorDisplayItem *> m_monitorDisplayItems;
    std::vector<unsigned long> m_monitorkeys ;
    unsigned long m_hightedMonitorId;
    unsigned long m_prvHilitedMonitorID;
    bool m_isAssignedBySchematic;
    bool  m_mouseTracking;
    bool m_contextActive;
    MonitorDisplayItem* m_currentSelectedMonitor;
    unsigned long m_localMonitorKey;
    Monitor* m_localMonitor;
    CString strErrorMessage;
};
}
