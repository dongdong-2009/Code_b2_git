//#using <mscorlib.dll>
//#using "FZ_NET.dll"  //ÒýÓÃDLL
/*using namespace System;
using namespace System::Runtime::InteropServices;
using namespace FZ_SDK;
using namespace System::Text;
#pragma once
*/
#pragma once

#include "core/data_access_interface/src/ILocation.h"
#include "IVideoDisplayManagerDAI.h"
#include "ILoadStationDiagramService.h"
#include "IGFVideoSwitchDisplayView.h"
#include "DisplayItemManager.h"
#include "ISequenceEditorView.h"
#include <vector>
#include "MonitorDisplayItem.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"

const static unsigned int HALF_MINUTE_EVENT = 1;

namespace TA_IRS_App
{

    class VideoSourcesTreeCtrl;
	class VirtualMonitorAssigner;
	class SDKCommunicator;
    class VideoDisplayManagerPresenter : public TA_Base_Core::Thread
    {

        public:
            VideoDisplayManagerPresenter(
                IVideoDisplayManagerDAI* videodisplayManagerDAI ,
                ILoadStationDiagramService * loadStationDiagramService
            );
            virtual ~VideoDisplayManagerPresenter(void);
            TA_Base_Core::ILocation::ELocationType  getLocationType();
            void SetIView(IGFVideoSwitchDisplayView * view);
            void SetSeqEditorView(ISequenceEditorView* seqEditorview);
            //  void setSequenceConfig( Sequence* sequence,unsigned short dwellTime,const std::list< Camera* > cameraInputs,bool con );
            void saveSequenceConfig(Sequence* sequence, unsigned short dwellTime, const std::list< Camera* > cameraInputs);

            void setSequenceDescription(Sequence* sequence,
                                        const std::string& newDescription);

            void loadStationDiagram(unsigned long level);
            void SelectSequence(unsigned long sequenceId);
            void SelectStation();

	        bool impPlayBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair);
			bool playBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair);
			bool playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd);
			bool impAssignMonitor(Monitor* monitor, VideoInput* input);
            bool assignMonitor(Monitor * monitor, VideoInput * input);
			bool setMonitorPicCount(Monitor* monitor, int picCount);

	        bool assignQuad( Monitor* monitor,
	                         Camera* topLeftCam,
	                         Camera* topRightCam,
	                         Camera* bottomLeftCam,
	                         Camera* bottomRightCam );

            void setSDKService(SDKCommunicator* sdkService);
            /*
              invoked by view to edit sequence
            */
            void editSequence(unsigned long sequenceId);
            void setUpMonitor();
            void setLocationType(TA_Base_Core::ILocation::ELocationType locationType);
            BOOL isOcc();
            void MainUILButtonDown(CPoint& point);

            void MainUILButtonUp(CPoint& point);

            void MainUIMouseMove(CPoint& point);

            void MainUIContextMenu(CPoint& point);


            //void MainUIPosMove(int x, int y);

            MonitorDisplayItem* getOverMonitor(CPoint& point);

            bool clearMonitor(Monitor * monitor);
			bool clearCameraFromMonitor(Monitor * monitor);
			bool clearSequenceFromMonitor(Monitor * monitor);

			bool impClearMonitor(Monitor * monitor);
			bool impClearCameraFromMonitor(Monitor * monitor);
			bool impClearSequenceFromMonitor(Monitor * monitor);

            void onRunParamChange(const std::string& name, const std::string& value);

            void beginDrage(CPoint & point , CImageList * image);

            void onCompleteInitStation();

            void loadMonitorRelationship();
            /*
              operator select the camera from the list box of sequence
            */
            void selectCameraFromSequenceList();
            unsigned long getLocationId();
            void closeSchematic();
            void stopSchematic();
            void refreshSchematic();
            void OnTimer(UINT nEventID);
	        void monitorAssignChanged(Monitor* monitor);
            virtual void run();
            virtual void terminate();
            void setUpdateSequenceTreeCtrl(VideoSourcesTreeCtrl* sequenceTreeCtrl);
        private:
			unsigned long m_currentSelectSequenceId;
            TA_Base_Core::ILocation::ELocationType m_locationType;
            IVideoDisplayManagerDAI* m_videoDisplayDai;
            ILoadStationDiagramService* m_LoadStationDiagramService;
            IGFVideoSwitchDisplayView* m_IView;
            VirtualMonitorAssigner* m_virtualMonitorAssigner;
            // SequenceEditorDlg  m_seqEditorView;
            ISequenceEditorView* m_seqEditorView;
            SDKCommunicator* m_sdkService;
            // TA_Base_Core::ILocation::ELocationType m_locationType;

            std::vector < MonitorDisplayItem *> m_monitorDisplayItems;
			
            std::vector<unsigned long> m_monitorkeys ;

            MonitorWnd* m_hightedMonitorWnd;
            MonitorWnd* m_prvHilitedMonitorWnd;

            bool m_isAssignedBySchematic;

            bool  m_mouseTracking;

            bool m_contextActive;
            void updateMonitor();

            void updateMonitorView();

            MonitorDisplayItem* getMonitor(unsigned long monitorId);

            MonitorDisplayItem* getMonitorByCtrlId(unsigned long monitorId);

            void handleTransactiveException(TA_Base_Core::TransactiveException& te);

            MonitorDisplayItem* m_currentSelectedMonitor;

            Camera * getSelectedCamera();


            void handleHighLightMonitor(MonitorDisplayItem * item);

            BOOL isCameraSelected();

			bool isVirtualMonitor(Monitor* monitor);

            MonitorDisplayItem * getMonitorDisplayItemByIndex(unsigned short index);

            unsigned long m_locationId;
            unsigned long m_localMonitorKey;
            Monitor* m_localMonitor;
            TA_Base_Core::ILocation* m_location;

            CString strErrorMessage;

			bool isLocalMonitor(Monitor* monitor);
            void invalidateControlLabels();
            void CheckSequeceUpdate();
            bool m_running;
            TA_Base_Core::Condition m_condition;
            VideoSourcesTreeCtrl* m_checkUpdateSequenceTreeCtrl;
    };
}
