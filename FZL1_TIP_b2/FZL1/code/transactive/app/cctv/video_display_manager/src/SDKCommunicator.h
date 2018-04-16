#ifndef SDKCommunicator_H
#define SDKCommunicator_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/3002/transactive/app/cctv/video_display_manager/src/SDKCommunicator.h $
  * @author:  Zhang hongzhi
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/05/05 18:38:22 $
  * Last modified by:  $Author: zhang.hongzhi $
  *
  * Singleton to provide access to sdk data and functions
  *
  */
#using <mscorlib.dll>
#using "lib/FZ_NET.dll"  //引用DLL
//using namespace System;
//using namespace System::Runtime::InteropServices;
using namespace FZ_SDK;
//using namespace System::Text;
#pragma once

#include <gcroot.h>
#include <map>
#include <string>
#include <vector>
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/IDL/src/CameraCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/SequenceCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"
#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/types/VisualComms_MessageTypes.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"
#include "app/cctv/video_display_manager/src/ISwitchAgentService.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
//#include "app/cctv/video_display_manager/src/IGFVideoSwitchDisplayView.h"
#include <ace/Singleton.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <boost/noncopyable.hpp>

namespace TA_IRS_App
{
    class Camera;
    class Sequence;
    class Monitor;
    class DisplayItem;
    class VideoInput;
	const static int MAX_VIDEO_SOURCES = 20;

	class SDKCommunicator : boost::noncopyable
    {
		friend class ACE_Singleton<SDKCommunicator, ACE_Recursive_Thread_Mutex>;
    public:
        typedef std::map< std::string, TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject* >        CamToNamedObject;
        typedef std::map< std::string, TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject* >      SequenceToNamedObject;
        typedef std::map< std::string, TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject* >  MonitorToNamedObject;
        typedef std::map< std::string, TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* >   OutputToNamedObject;

        static SDKCommunicator& getInstance();

		static void removeInstance();

        bool assignMonitor( Monitor* monitor, VideoInput* source );
		bool assignCameraToMonitor(Monitor* monitor, Camera* camera);
		bool assignSequenceToMonitor(Monitor* monitor, Sequence* sequence);
		bool setMonitorPicCount(Monitor* monitor, int picCount);

        void assignQuad( Monitor* monitor,
                         Camera* topLeftCam,
                         Camera* topRightCam,
                         Camera* bottomLeftCam,
                         Camera* bottomRightCam );

        bool setSequenceConfig( Sequence* sequence,
                                unsigned short dwellTime,
                                const std::list< Camera* > cameraInputs);

        bool setSequenceDescription( Sequence* sequence, const std::string& newDescription );
        void clearMonitor( Monitor* monitor);
		bool clearCameraFromMonitor( Monitor* monitor);
		bool clearSequenceFromMonitor( Monitor* monitor);

	    void requestRecordingPlayBack(Monitor* monitor, Camera* camera, PlayBackTimePair timeSet);
		void playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd);
        CamToNamedObject getCameraObjects();
        SequenceToNamedObject getSequenceObjects();
        MonitorToNamedObject getConsoleMonitorObjects();
        MonitorToNamedObject getCommonMonitorObjects();
        void updateSequence(Sequence* sequence);
		void zoomCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd zoomDirection);
		void panCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection);	
		void tiltCamera (Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection);
		void panAndTiltCamera ( Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection);
		// This method is only used to support test
		void acknowledgeAlarm(unsigned long alarmEntityKey);
		bool ptzRequest(Camera* camera, PTZDirection direction);

		//void SetIView(IGFVideoSwitchDisplayView * view);
		std::string m_errorMessage;
    private:
        SDKCommunicator();
        virtual ~SDKCommunicator();
		bool verifyZoomCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd zoomDirection);
		bool verifyPanCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection);
		bool verifyTiltCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection);
		bool verifyPanAndTiltCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection);
        /* Look up names in the maps and return corresponding named objects
           or NULL if there is none matching */
        TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject*     getCommonMonitor(const std::string name);
        TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject*    getConsoleMonitor(const std::string name);
        TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject*    getMonitor(const std::string name);
        TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject*          getCamera(const std::string name);
        TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject*        getSequence(const std::string name);

        void loadCameraNamedObjects();
        void loadSequenceNamedObjects();
        void loadMonitorNamedObjects();

		void Initialize();
		bool SDK_Init_Login();
		bool SDK_Init();
		bool SDK_Login();

		void CharToByte(array<System::Byte>^ b,char* c,int len);
		int EncodeToUTF8(char* utf8Str,const char* mbcsStr);
		std::string GetSDKMessage(FZ_ENUM::FZ_ENUM_ERROR err);
		FZ_ENUM::FZ_PTZ_CMD_E convertPTZDirectionToCmd(PTZDirection direction);
		bool SendPTZ(Camera* camera, FZ_ENUM::FZ_PTZ_CMD_E cmd);

    private:
        CamToNamedObject m_camToNamedObject;
        SequenceToNamedObject m_sequenceToNamedObject;
        MonitorToNamedObject m_monitorToNamedObject;
        MonitorToNamedObject m_consoleMonitorToNamedObject;
		MonitorToNamedObject m_commonMonitorToNamedObject;
		TA_Base_Core::ReEntrantThreadLockable m_lock;
		//IGFVideoSwitchDisplayView* m_IView;

		//这样声明实例化FZ_NET
		gcroot<FZ_NET^> m_fz_net;
		std::string m_serverIp;
		std::string m_userName;
		std::string m_userPassword;
		unsigned long m_serverPort;
    };
} // namespace TA_IRS_App

#endif // SDKCommunicator_H
