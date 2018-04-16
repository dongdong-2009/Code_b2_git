#pragma once
#include <vector>
#include "app/cctv/video_display_manager/src/CCTVCommon.h"

namespace TA_IRS_App
{
	class Camera;
    class Monitor;
    class VideoInput;
    class VideoDisplayManagerPresenter;
	class CDlgVideoPlay;
	class PlayBackDlg;
    class VirtualMonitorAssigner
    {
        public:
            VirtualMonitorAssigner(VideoDisplayManagerPresenter& displayPresenter,
                                   std::vector<Monitor*>& quadMonitors,
                                   Monitor* localMonitor);
            ~VirtualMonitorAssigner();
            bool assignMonitor(Monitor* monitor, VideoInput* input);
			bool playBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair);
			bool playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd);
			Monitor* getLocalMonitor();
            void clearAll();
        private:
            bool assignVideoToLocal(VideoInput* videoInput);
            bool assignCameraToQuad(Camera* videoInput, int quadNumber);
			bool isQuadHasInput();
			void changePlayMode();
            bool clearLocal();
            bool clearQuad();
            std::vector<Monitor*> m_quadMonitors;
            Monitor* m_localMonitor;
            VideoDisplayManagerPresenter& m_videoDisplayPresenter;
			CDlgVideoPlay* m_playDlg;
			PlayBackDlg* m_playBackDlg;
    };
}