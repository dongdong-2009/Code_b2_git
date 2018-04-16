#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/DlgVideoPlay.h"
#include "app/cctv/video_display_manager/src/PlayBackDlg.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenter.h"
#include "app/cctv/video_display_manager/src/VirtualMonitorAssigner.h"
#include "core/utilities/src/RunParams.h"
#include <boost/foreach.hpp>

namespace TA_IRS_App
{
    VirtualMonitorAssigner::VirtualMonitorAssigner(VideoDisplayManagerPresenter& displayPresenter,
                                                   std::vector<Monitor*>& quadMonitors,
                                                   Monitor* localMonitor)
        : m_videoDisplayPresenter(displayPresenter),
          m_quadMonitors(quadMonitors),
          m_playDlg(NULL),
          m_playBackDlg(NULL),
          m_localMonitor(localMonitor)
    {

		// Before create play dialog, get aspectRatio first
		float aspectRatio = 0.0f;
		int w = 0, h = 0;
		std::string ws, hs;
        std::string aspectRatioStr = TA_Base_Core::RunParams::getInstance().get(VIDEOASPECTRATIOPARAMETERNAME);
		std::string::size_type pos = aspectRatioStr.find(':');
		if(pos != std::string::npos)
		{
			ws = aspectRatioStr.substr(0, pos);
		    hs = aspectRatioStr.substr(pos+1);
		}
		if(!ws.empty() && !hs.empty())
		{
			w = atoi(ws.c_str());
			h = atoi(hs.c_str());
		}
		if(w > 0 && h > 0)
		{
			aspectRatio = (float)w/(float)h;
		}

        m_playDlg = new CDlgVideoPlay(*this, aspectRatio);
        m_playDlg->Create(CDlgVideoPlay::IDD, NULL);
        m_playBackDlg = new PlayBackDlg(*this);
        m_playBackDlg->Create(PlayBackDlg::IDD, NULL);
    }

    VirtualMonitorAssigner::~VirtualMonitorAssigner()
    {
        delete m_playDlg;
        delete m_playBackDlg;
    }

    Monitor* VirtualMonitorAssigner::getLocalMonitor()
    {
        return m_localMonitor;
    }

    void VirtualMonitorAssigner::changePlayMode()
    {
        if(m_localMonitor->getInput() != NULL || isQuadHasInput())
        {
            m_playDlg->play();
        }
        else
        {
            m_playDlg->stop();
        }
    }

    bool VirtualMonitorAssigner::isQuadHasInput()
    {
        BOOST_FOREACH(Monitor * i, m_quadMonitors)
        {
            if(i->getInput() != NULL)
            {
                return true;
            }
        }
        return false;
    }

    bool VirtualMonitorAssigner::playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd)
    {
        TA_ASSERT(monitor == m_localMonitor, "");
        return m_videoDisplayPresenter.playBackCtrl(m_localMonitor, cmd);
    }

    bool VirtualMonitorAssigner::playBack(Monitor* monitor, Camera* camera, PlayBackTimePair timePair)
    {
        TA_ASSERT(monitor == m_localMonitor, "");
        bool success = m_videoDisplayPresenter.impPlayBack(m_localMonitor, camera, timePair);
        if(success)
        {
            BOOST_FOREACH(Monitor * i, m_quadMonitors)
            {
                i->assignInput(NULL);
                m_videoDisplayPresenter.monitorAssignChanged(i);
            }
            m_localMonitor->assignInput(NULL);
            m_videoDisplayPresenter.monitorAssignChanged(m_localMonitor);
            changePlayMode();
            m_playBackDlg->play();
        }
        return success;
    }

    bool VirtualMonitorAssigner::assignMonitor(Monitor* monitor, VideoInput* input)
    {
        bool success = false;
        Sequence* seq = dynamic_cast<Sequence*>(input);
        // Quad can not assign sequence
        if(monitor->isVirtualQuadMonitor() && seq != NULL)
        {
            return false;
        }
        if(monitor == m_localMonitor)
        {
            success = assignVideoToLocal(input);
        }
        else if(monitor->isVirtualQuadMonitor())
        {
            Camera* camera = dynamic_cast<Camera*>(input);
            success = assignCameraToQuad(camera, monitor->getDisplayOrder());
        }
        if(success)
        {
            m_playBackDlg->stop();
        }
        changePlayMode();
        return success;
    }

    bool VirtualMonitorAssigner::assignCameraToQuad(Camera* videoInput, int quadNumber)
    {
        TA_ASSERT(1 <= quadNumber && quadNumber <= 4, "");

        #define QUAD_INPUT(quadMonitor, input, quadNumber) \
        (quadMonitor->getDisplayOrder() == quadNumber ? input : dynamic_cast<Camera*>(quadMonitor->getInput()))

        bool success = false;
        do
        {
            Monitor* monitor = m_quadMonitors[quadNumber - 1];
            VideoInput* currentInput = monitor->getInput();
            if(videoInput == currentInput)
            {
                success = true;
                break;
            }
            Camera* camera1 = QUAD_INPUT(m_quadMonitors[0], videoInput, quadNumber);
            Camera* camera2 = QUAD_INPUT(m_quadMonitors[1], videoInput, quadNumber);
            Camera* camera3 = QUAD_INPUT(m_quadMonitors[2], videoInput, quadNumber);
            Camera* camera4 = QUAD_INPUT(m_quadMonitors[3], videoInput, quadNumber);
            if(camera1 == NULL && camera2 == NULL && camera3 == NULL && camera4 == NULL)
            {
                return clearQuad();
            }
            if(!(success = m_videoDisplayPresenter.assignQuad(monitor, camera1, camera2, camera3, camera4)))
            {
                break;
            }
            m_quadMonitors[quadNumber - 1]->assignInput(videoInput);
            m_videoDisplayPresenter.monitorAssignChanged(m_quadMonitors[quadNumber - 1]);
            if(NULL != videoInput)
            {
                m_localMonitor->assignInput(NULL);
                m_videoDisplayPresenter.monitorAssignChanged(m_localMonitor);
            }
        }
        while(0);
        return success;
    }

    bool VirtualMonitorAssigner::assignVideoToLocal(VideoInput* videoInput)
    {
        bool success = (NULL == videoInput ? m_videoDisplayPresenter.impClearMonitor(m_localMonitor) :
                        m_videoDisplayPresenter.impAssignMonitor(m_localMonitor, videoInput));
        if(success)
        {
            m_localMonitor->assignInput(videoInput);
            m_videoDisplayPresenter.monitorAssignChanged(m_localMonitor);
            if(isQuadHasInput())
            {
                BOOST_FOREACH(Monitor * i, m_quadMonitors)
                {
                    i->assignInput(NULL);
                    m_videoDisplayPresenter.monitorAssignChanged(i);
                }
            }
        }
        return success;
    }

    bool VirtualMonitorAssigner::clearQuad()
    {
        TA_ASSERT(m_quadMonitors.size() == 4, "");
        bool success = m_videoDisplayPresenter.impClearMonitor(m_quadMonitors[0]);
        if(success)
        {
            for(int i = 0; i < 4; ++i)
            {
                m_quadMonitors[i]->assignInput(NULL);
                m_videoDisplayPresenter.monitorAssignChanged(m_quadMonitors[i]);
            }
        }
        return success;
    }

    bool VirtualMonitorAssigner::clearLocal()
    {
        TA_ASSERT(m_localMonitor != NULL, "");
        bool success = m_videoDisplayPresenter.impClearMonitor(m_localMonitor);
        if(success)
        {
            m_localMonitor->assignInput(NULL);
            m_videoDisplayPresenter.monitorAssignChanged(m_localMonitor);
        }
        return success;
    }

    void VirtualMonitorAssigner::clearAll()
    {
        if(m_localMonitor->getInput() != NULL)
        {
            clearLocal();
        }
        else if(isQuadHasInput())
        {
            clearQuad();
        }
    }

}