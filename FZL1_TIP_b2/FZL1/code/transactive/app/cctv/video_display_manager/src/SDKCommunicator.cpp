/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/3002/transactive/app/cctv/video_display_manager/src/SDKCommunicator.cpp $
  * @author:  Zhang Hongzhi
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/05/08 18:38:22 $
  * Last modified by:  $Author: zhang.hongzhi $
  *
  * Singleton to provide access to Sdk data and functions
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // defined( WIN32 )

#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/VideoInput.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoOutput.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/ErrorMessage.h"
#include "app/cctv/video_display_manager/src/AddressHelper.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/VideoSwitchAgent.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/exceptions/src/TransActiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include <boost/foreach.hpp>

using CORBA::String_var;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;

typedef TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject VideoOutputNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::VideoMonitorNamedObject VideoMonitorNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::CameraNamedObject CameraNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::SequenceNamedObject SequenceNamedObject;
typedef TA_Base_Bus::VideoSwitchAgentFactory::SwitchManagerNamedObject SwitchManagerNamedObject;

//IP长度
#define FZ_IP_LEN 16
#define FZ_NAME_LEN 64
#define FZ_CODE_LEN 48
#define WM_FZ_NET_INFO  (WM_USER + 2000)        /* XP运行信息 */

namespace TA_IRS_App
{
    SDKCommunicator& SDKCommunicator::getInstance()
    {
        return *(ACE_Singleton<SDKCommunicator, ACE_Recursive_Thread_Mutex>::instance());
    }

	void SDKCommunicator::removeInstance()
	{
		ACE_Singleton<SDKCommunicator, ACE_Recursive_Thread_Mutex>::close();
	}

	SDKCommunicator::SDKCommunicator()
    {
		Initialize();
    }

    SDKCommunicator::~SDKCommunicator()
    {
        for(CamToNamedObject::iterator it = m_camToNamedObject.begin(); it != m_camToNamedObject.end(); ++it)
        {
            delete it->second;
        }
        for(SequenceToNamedObject::iterator it = m_sequenceToNamedObject.begin(); it != m_sequenceToNamedObject.end(); ++it)
        {
            delete it->second;
        }
        for(MonitorToNamedObject::iterator it = m_monitorToNamedObject.begin(); it != m_monitorToNamedObject.end(); ++it)
        {
            delete it->second;
        }
    }

	void SDKCommunicator::Initialize()
	{
		// Get sdk parameters
		TA_Base_Core::IEntityDataList entities;
		do
		{
			/*try
			{
				unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
				entities = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoSwitchAgent::getStaticType(), locationKey);
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Got exception while get VideoSwitchAgent entity.", "");
				break;
			}*/
			//if(1 == entities.size())
			{
				try
				{
					std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
					//TA_Base_Core::VideoSwitchAgent entity(entities[0]->getKey());
					m_serverIp = console->getCCTVServerIPAddress();
					m_serverPort = console->getCCTVServerPort();
					m_userName = console->getCCTVServerUserName();
					m_userPassword = console->getCCTVServerPassword();
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Got exception while get CCTV related parameters value.", "");
					break;
				}
			}
			/*for(TA_Base_Core::IEntityDataList::size_type i = 0; i < entities.size(); ++i)
			{
				delete entities[i];
			}*/
			//实例化FZ_NET
			m_fz_net=gcnew FZ_NET();
			SDK_Init_Login();
		}
		while(0);
	}

	//转化char*到array方法
	void SDKCommunicator::CharToByte(array<System::Byte>^ b,char* c,int len)
	{
		for(int i=0;i<len;i++)
		{
			b[i]=c[i];
		}

	}

	//转化Unicode到utf-8
	int SDKCommunicator::EncodeToUTF8(char* utf8Str,const char* mbcsStr) 
	{ 
		wchar_t*  wideStr; 
		int   charLen;

		charLen = MultiByteToWideChar(936, 0, mbcsStr, -1, NULL, 0);    ////////936 ----- gb2312
		wideStr = (wchar_t*) malloc(sizeof(wchar_t)*charLen); 
		MultiByteToWideChar(936, 0, mbcsStr, -1, wideStr, charLen);
		charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
		WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);
		free(wideStr); 
		return charLen;

	} 

	//获取错误信息
	std::string SDKCommunicator::GetSDKMessage(FZ_ENUM::FZ_ENUM_ERROR err)
	{
		std::string strRes="";
		switch(err)
		{
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS:
			strRes="操作成功";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_IN_PARAM:
			strRes="SDK输入参数错误";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_IP_PARAM:
			strRes="宇视系统IP参数配置错误";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_CAMERA:
			strRes="配置错误，宇视系统没有此摄像机";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_CONNECT_MON:
			strRes="此监视器在宇视系统尚未联线";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_CONNECT_UN:
			strRes="无法连接到宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT:
			strRes="SDK尚未初始化";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN:
			strRes="尚未登陆到宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_MONITOR:
			strRes="配置错误，宇视系统没有此监视器";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_PASSWORD:
			strRes="密码错误，无法登录宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_PRIOR:
			strRes="没有权限在宇视系统上进行此操作";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_STATIONS:
			strRes="宇视系统没有配置此车站";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_USERNAME:
			strRes="此用户不存在，无法登录宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NOT_SUPPORT:
			strRes="宇视系统不支持此操作";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NULL_ORGNAME:
			strRes="宇视系统组织名称为空";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NULL_PASSWORD:
			strRes="密码为空，无法登录宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_NULL_USERNAME:
			strRes="用户名为空，无法登录宇视系统";
			break;
		case FZ_ENUM::FZ_ENUM_ERROR::ERR_SPACE_NOT_ENOUGH:
			strRes="宇视系统空间不足";
			break;
		default:
			break;
		}

		return strRes;
	}

	bool SDKCommunicator::SDK_Init_Login()
	{
		bool ret = SDK_Init();
		ret = SDK_Login();
		return ret;
	}

	bool SDKCommunicator::SDK_Init()
	{
		m_errorMessage = "";
		//unsigned char chIP[FZ_IP_LEN]={0};
		//memset(chIP,  m_serverIp.c_str() ,m_serverIp.size() *sizeof(char));
		char chIP[FZ_IP_LEN]={0};// = m_serverIp;
		strcpy(chIP, m_serverIp.c_str());
		//chIP = m_serverIp;
		UINT ulPort=0;
		FZ_STRUCT::FZ_STRUCT_SERVERINFO stFZ_STRUCT_SERVERINFO;
		stFZ_STRUCT_SERVERINFO.ucServerIP=gcnew cli::array<System::Byte>(FZ_IP_LEN);
		CharToByte(stFZ_STRUCT_SERVERINFO.ucServerIP,chIP,FZ_IP_LEN);
		stFZ_STRUCT_SERVERINFO.ulServerPort = m_serverPort;
		FZ_ENUM::FZ_ENUM_ERROR err=m_fz_net->FZ_Initiate(stFZ_STRUCT_SERVERINFO);
		m_errorMessage = GetSDKMessage(err);
		if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDK_Init failed, reason=%s", m_errorMessage.c_str());
			//m_IView->displayErrorMessage(CString(m_errorMessage.c_str()));
			return  false;
		}
		else
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDK_Init successfully");
		return true;
	}

	bool SDKCommunicator::SDK_Login()
	{
		m_errorMessage = "";
		//m_userName = "管理员";
		//m_userPassword = "1234";
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDK Login username=%s, password=%s", m_userName.c_str(), m_userPassword.c_str());
		FZ_STRUCT::FZ_STRUCT_USERINFO sUserInfo;
		sUserInfo.strLocalIP=gcnew cli::array<System::Byte>(FZ_IP_LEN);
		sUserInfo.strUserName=gcnew cli::array<System::Byte>(FZ_NAME_LEN);
		sUserInfo.strUserPassWord=gcnew cli::array<System::Byte>(FZ_CODE_LEN);
		char chUser[FZ_NAME_LEN]={0};
		strcpy(chUser, m_userName.c_str());
		//memset(chUser,  m_userName.c_str(), m_userName.size() *sizeof(char));
		char user[FZ_NAME_LEN]={0};
		int x=EncodeToUTF8(user,chUser);

		CharToByte(sUserInfo.strUserName,user,x);
		char chPass[FZ_CODE_LEN]={0};
		strcpy(chPass, m_userPassword.c_str());
		//memset(chPass,  m_userPassword.c_str(), m_userPassword.size() *sizeof(char));
		CharToByte(sUserInfo.strUserPassWord,chPass,m_userPassword.length());
		FZ_ENUM::FZ_ENUM_ERROR err=m_fz_net->FZ_LoginEx(sUserInfo);
		bool success = (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS);
		switch(sUserInfo.eUserStatus)		//sdk这里做了修改,sUserInfo内的状态,为当前登陆状态20150424
		{
		case FZ_ENUM::FZ_ENUM_USER_STATUS::ERR_PASSWORD:
			{
				m_errorMessage = "密码错误";
				break;
			}
		case FZ_ENUM::FZ_ENUM_USER_STATUS::ERR_USERNAME:
			{
				m_errorMessage = "用户名错误";
				break;
			}
		case FZ_ENUM::FZ_ENUM_USER_STATUS::LOGIN:
			{
				m_errorMessage = "登陆成功";
				success = true;
				break;
			}
		case FZ_ENUM::FZ_ENUM_USER_STATUS::LOGOUT:
			{
				m_errorMessage = "登陆失败";
				break;
			}
		}

		//m_errorMessage = GetSDKMessage(err);
		if(success == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDK_Login failed, reason=%s", m_errorMessage.c_str());
			return  false;
		}
		else
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SDK_Login successfully");
		return success;
	}

	void SDKCommunicator::requestRecordingPlayBack(Monitor* monitor, Camera* camera, PlayBackTimePair timeSet)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("requestRecordingPlayBack");
		ThreadGuard guard(m_lock);
		TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
		TA_ASSERT(camera != NULL, "Video input is NULL");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "requestRecordingPlayBack ( Monitor = %ld, Camera = %ld)",
			monitor->getKey() , camera->getKey());
		try
		{
			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			VideoMonitorNamedObject* monObj = getMonitor(monitor->getName());
			if(NULL == monObj)
			{
				throw TransactiveException(NO_REMOTE_MONITOR);
			}
			CORBA_CALL((*monObj), requestRecordingPlayBack, (camera->getKey(), timeSet.startTime, timeSet.endTime, session.c_str()));
		}
		CATCH_ALL_EXCEPTION;
		FUNCTION_EXIT;
	}

	void SDKCommunicator::playBackCtrl(Monitor* monitor, TA_Base_Bus::VideoOutputCorbaDef::PlayBackCtrlCmd cmd)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("playBackCtrl");
		ThreadGuard guard(m_lock);
		TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "playBackCtrl Monitor = %ld", monitor->getKey());
		try
		{
			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			VideoMonitorNamedObject* monObj = getMonitor(monitor->getName());
			if(NULL == monObj)
			{
				throw TransactiveException(NO_REMOTE_MONITOR);
			}
			CORBA_CALL((*monObj), playBackCtrl, (cmd, session.c_str()));
		}
		CATCH_ALL_EXCEPTION;
		FUNCTION_EXIT;
	}

    bool SDKCommunicator::assignMonitor(Monitor* monitor, VideoInput* source)
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY("assignMonitor");
        ThreadGuard guard(m_lock);
        TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
        TA_ASSERT(source != NULL, "Video Input is NULL");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignMonitor( Monitor = %ld, VideoInput = %ld)",
                    monitor->getKey() , source->getKey());
		m_errorMessage = "";
		bool success = false;
        try
        {
			Camera* camera = dynamic_cast<Camera*> ( source );
			if(camera != NULL)
			{
				success = assignCameraToMonitor(monitor, camera);
			}
			Sequence* sequence = dynamic_cast<Sequence*> ( source );
			if(sequence != NULL)
			{
				success = assignSequenceToMonitor(monitor, sequence);
			}
        }
		CATCH_ALL_EXCEPTION;
        // update the state objects
		if (success)
		{
			monitor->assignInput(source);
			// tell the GUI to update its display
			DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
		}

        FUNCTION_EXIT;
		return success;
    }

	bool SDKCommunicator::assignCameraToMonitor(Monitor* monitor, Camera* camera)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("assignCameraToMonitor");
		m_errorMessage = "";
		if(monitor == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Monitor is NULL when assignSequenceToMonitor!");
			m_errorMessage = "要操作的监视器为空";
			return false;
		}
		if(camera == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera is NULL when assignSequenceToMonitor!");
			m_errorMessage = "要操作的摄像机为空";
			return false;
		}
		FZ_STRUCT::FZ_STRUCT_CAMERAINFO pCam;
		AddressHelper::Address address = AddressHelper::splitAddress(camera->getAddress());
		pCam.usStationID = address.stationID;
		pCam.usCameraID = address.cameraID;

		unsigned short usMonID=0;
		std::stringstream ss;
		ss << monitor->getAddress();
		ss >> usMonID; 

		//check if it is a local monitor
		bool isConsoleMonitor = monitor->isConsoleMonitor();
		unsigned char ucCount = 1;
		if(!isConsoleMonitor)
		{	//Quad and OPS is treated as quad
			ucCount = 4;
		}
		if(monitor->isSingleWallMonitor())
		{
			ucCount = 1;
		}
		FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,ucCount);		//must send changemonitor count before assign
		err = m_fz_net->FZ_SelectCameraToMonitor(usMonID,pCam);
		if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
		{
			SDK_Init_Login();
			err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,ucCount);
			err = m_fz_net->FZ_SelectCameraToMonitor(usMonID,pCam);
		}
		m_errorMessage = GetSDKMessage(err);
		if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignCameraToMonitor failed, reason=%s", m_errorMessage.c_str());
			return  false;
		}
		else
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignCameraToMonitor successfully");

		FUNCTION_EXIT;
		return true;
	}

	bool SDKCommunicator::setMonitorPicCount( Monitor* monitor, int picCount )
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("setMonitorPicCount");
		m_errorMessage = "";
		if(monitor == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Monitor is NULL when assignSequenceToMonitor!");
			m_errorMessage = "要操作的监视器为空";
			return false;
		}		
	
		unsigned short usMonID=0;
		std::stringstream ss;
		ss << monitor->getAddress();
		ss >> usMonID; 

		
		FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,picCount);		//must send changemonitor count before assign
		if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
		{
			SDK_Init_Login();
			err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,picCount);
		}
		m_errorMessage = GetSDKMessage(err);
		if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setMonitorPicCount failed, reason=%s", m_errorMessage.c_str());
			return  false;
		}
		else
        {
            monitor->setSingleWallMonitor(1 == picCount);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setMonitorPicCount successfully");
        }

		FUNCTION_EXIT;
		return true;
	}

	bool SDKCommunicator::assignSequenceToMonitor(Monitor* monitor, Sequence* sequence)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("assignSequenceToMonitor");
		m_errorMessage = "";
		if(monitor == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Monitor is NULL when assignSequenceToMonitor!");
			m_errorMessage = "要操作的监视器为空";
			return false;
		}
		if(sequence == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Sequence is NULL when assignSequenceToMonitor!");
			m_errorMessage = "要操作的视频序列为空";
			return false;
		}
		unsigned short usSquID=0;
		std::stringstream ss;
		ss << sequence->getAddress();
		ss >> usSquID; 

		unsigned short usMonID=0;
		ss.clear();
		ss.str("");
		ss << monitor->getAddress();
		ss >> usMonID; 

		//check if it is a local monitor
		bool isConsoleMonitor = monitor->isConsoleMonitor();
		unsigned char ucCount = 1;
		if(!isConsoleMonitor)
		{	//Quad and OPS is treated as quad
			ucCount = 4;
		}
		if(monitor->isSingleWallMonitor())
		{
			ucCount = 1;
		}
		FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,ucCount);		//must send changemonitor count before assign
		err = m_fz_net->FZ_StartSQUInMonitor(usMonID,usSquID);
		if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
		{
			SDK_Init_Login();
			err = m_fz_net->FZ_ChangMonitorPicCount(usMonID,ucCount);
			err = m_fz_net->FZ_StartSQUInMonitor(usMonID,usSquID);
		}
		m_errorMessage = GetSDKMessage(err);
		if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignSequenceToMonitor failed, reason=%s", m_errorMessage.c_str());
			return  false;
		}
		else
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignSequenceToMonitor successfully");
		
		FUNCTION_EXIT;
		return true;
	}

    void SDKCommunicator::assignQuad(Monitor* monitor,
                                             Camera* topLeftCam,
                                             Camera* topRightCam,
                                             Camera* bottomLeftCam,
                                             Camera* bottomRightCam)
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_lock);
        // get the quad corba object
		TA_ASSERT(monitor != NULL, "");
        try
        {
	        VideoMonitorNamedObject* monitorObj = getMonitor(monitor->getName());
			if(NULL == monitorObj)
			{
                throw TransactiveException(NO_REMOTE_MONITOR);
			}
	        // if the camera is not null - get the key, if it is, then set the key to 0
	        unsigned long topLeft = (NULL != topLeftCam) ? topLeftCam->getKey() : 0;
	        unsigned long topRight = (NULL != topRightCam) ? topRightCam->getKey() : 0;
	        unsigned long bottomLeft = (NULL != bottomLeftCam) ? bottomLeftCam->getKey() : 0;
	        unsigned long bottomRight = (NULL != bottomRightCam) ? bottomRightCam->getKey() : 0;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "assignQuad(%ld, %ld, %ld, %ld, %ld)",
                        monitor->getKey(), topLeft, topRight, bottomLeft, bottomRight);
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL((*monitorObj), assignQuad, (topLeft, topRight, bottomLeft, bottomRight, session.c_str(), true));
        }
		CATCH_ALL_EXCEPTION;
    }


    bool SDKCommunicator::setSequenceConfig(Sequence* sequence,
                                                    unsigned short dwellTime,
                                                    const std::list< Camera* > cameraInputs)
    {
		FUNCTION_ENTRY("setSequenceConfig");
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard(m_lock);
		m_errorMessage = "";
		if(sequence == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Sequence is NULL when save sequence!");
			m_errorMessage = "要操作的视频序列为空";
			return false;
		}
        try
        {
			FZ_STRUCT::FZ_STRUCT_SQUINFO stSquInfo;
			unsigned short usSquID=0;
			std::stringstream ss;
			ss << sequence->getAddress();
			ss >> usSquID;
			stSquInfo.usSQUID = usSquID;
			stSquInfo.strSQUName=gcnew cli::array<System::Byte>(FZ_CODE_LEN);
			std::string str = sequence->getName();
			std::cout << str << std::endl;
			System::String^ s = gcnew System::String(str.c_str());
			int x=System::Text::Encoding::UTF8->GetBytes(s,0,s->Length,stSquInfo.strSQUName,0);
			stSquInfo.ucStepCount = cameraInputs.size();	//步骤,若为0,则清除原来序列
			stSquInfo.ucDelay = dwellTime;	//延时,单位秒
			stSquInfo.ulCAMERAINFO = gcnew array<System::UInt32>(254);   //必须初始化
			int index = 0;
			BOOST_FOREACH(Camera * i, cameraInputs)
			{
				//第一个步骤
				if (i == NULL)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Camera is NULL when save sequence!");
					m_errorMessage = "要添加的摄像机为空";
					return false;
				}
				unsigned int stationID = 0;
				unsigned int cameraID = 0;
				AddressHelper::Address address = AddressHelper::splitAddress(i->getAddress());
				stationID = address.stationID;
				cameraID = address.cameraID;
				stSquInfo.ulCAMERAINFO[index++] = stationID * 0x10000 + cameraID;
			}
			FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_SetSQUList(stSquInfo);
			if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
			{
				SDK_Init_Login();
				err = m_fz_net->FZ_SetSQUList(stSquInfo);
			}
			m_errorMessage = GetSDKMessage(err);
			if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSequenceConfig failed, reason=%s", m_errorMessage.c_str());
				return  false;
			}
			else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "setSequenceConfig successfully");
        }
		CATCH_ALL_EXCEPTION;

        FUNCTION_EXIT;
		return true;
    }

    bool SDKCommunicator::setSequenceDescription(Sequence* sequence,
                                                         const std::string& newDescription)
    {
        ThreadGuard guard(m_lock);
        TA_ASSERT(sequence != NULL, "Destination Sequence is NULL");
		m_errorMessage = "";
		bool success = false;
        try
        {
	        success = sequence->saveSequenceDescription(newDescription.c_str());
        }
		CATCH_ALL_EXCEPTION;

		return success;
    }


    void SDKCommunicator::clearMonitor(Monitor* monitor)
    {
        // Guard the method to ensure access by client code is synchronous
        FUNCTION_ENTRY("clearMonitor");
        ThreadGuard guard(m_lock);
        TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearMonitor( Monitor = %ld)", monitor->getKey());
		bool success = false;
        try
        {
			VideoInput* source = monitor->getInput();
			if (source == NULL)
			{
				success = true;
				// update the state objects
				monitor->assignInput(NULL);
				// tell the GUI to update its display
				DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
			} 
			else
			{
				Camera* camera = dynamic_cast<Camera*> ( source );
				if(camera != NULL)
				{
					success = clearCameraFromMonitor(monitor);
				}
				Sequence* sequence = dynamic_cast<Sequence*> ( source );
				if(sequence != NULL)
				{
					success = clearSequenceFromMonitor(monitor);
				}
			}
        }
		CATCH_ALL_EXCEPTION;
       
        FUNCTION_EXIT;
    }

	bool SDKCommunicator::clearCameraFromMonitor(Monitor* monitor)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("clearCameraFromMonitor");
		ThreadGuard guard(m_lock);
		m_errorMessage = "";
		//TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");		//cannot use TA_Assert because it have inlice code, cannot be used in managed C++
		if(monitor == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Destination Monitor is NULL when clearCameraFromMonitor!");
			m_errorMessage = "要操作的监视器为空";
			return false;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearCameraFromMonitor( Monitor = %ld)", monitor->getKey());
		try
		{
			FZ_STRUCT::FZ_STRUCT_CAMERAINFO pCam;
			pCam.usStationID = 0;
			pCam.usCameraID = 0;

			unsigned short usMonID=0;
			std::string strMonitorAddress = monitor->getAddress();
			std::istringstream in(strMonitorAddress);
			in >> usMonID; 
			FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_SelectCameraToMonitor(usMonID,pCam);
			if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
			{
				SDK_Init_Login();
				err = m_fz_net->FZ_SelectCameraToMonitor(usMonID,pCam);
			}
			m_errorMessage = GetSDKMessage(err);
			if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearCameraFromMonitor failed, reason=%s", m_errorMessage.c_str());
				//return  true;
			}
			else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearCameraFromMonitor successfully");
		}
		CATCH_ALL_EXCEPTION;
		// update the state objects
		monitor->assignInput(NULL);
		// tell the GUI to update its display
		DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
		FUNCTION_EXIT;
		return true;
	}

	bool SDKCommunicator::clearSequenceFromMonitor(Monitor* monitor)
	{
		// Guard the method to ensure access by client code is synchronous
		FUNCTION_ENTRY("clearSequenceFromMonitor");
		ThreadGuard guard(m_lock);
		m_errorMessage = "";
		//TA_ASSERT(monitor != NULL, "Destination Monitor is NULL");
		if(monitor == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Destination Monitor is NULL when clearSequenceFromMonitor!");
			m_errorMessage = "要操作的监视器为空";
			return false;
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearSequenceFromMonitor( Monitor = %ld)", monitor->getKey());
		try
		{
			unsigned short usMonID=0;
			std::string strMonitorAddress = monitor->getAddress();
			std::istringstream in(strMonitorAddress);
			in >> usMonID; 
			FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_StopSQUInMonitor(usMonID);
			if (err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_INIT || err == FZ_ENUM::FZ_ENUM_ERROR::ERR_NO_LOGIN)
			{
				SDK_Init_Login();
				err = m_fz_net->FZ_StopSQUInMonitor(usMonID);
			}
			m_errorMessage = GetSDKMessage(err);
			if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearSequenceFromMonitor failed, reason=%s", m_errorMessage.c_str());
				//return  false;
			}
			else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "clearSequenceFromMonitor successfully");
		}
		CATCH_ALL_EXCEPTION;
		// update the state objects
		monitor->assignInput(NULL);
		// tell the GUI to update its display
		DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(monitor);
		FUNCTION_EXIT;
		return true;
	}

    void SDKCommunicator::loadCameraNamedObjects()
    {
        using namespace TA_Base_Bus;
        VideoSwitchAgentFactory::CameraVector cameras = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getCameras();
        for(VideoSwitchAgentFactory::CameraVector::iterator it = cameras.begin(); it != cameras.end(); ++it)
        {
            String_var name;
            CORBA_CALL_RETURN(name, (*(*it)), getName, ());
            m_camToNamedObject[name.in()] = (*it);
        }
    }


    void SDKCommunicator::loadSequenceNamedObjects()
    {
        using namespace TA_Base_Bus;
        VideoSwitchAgentFactory::SequenceVector seqs = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSequences();
        for(VideoSwitchAgentFactory::SequenceVector::iterator it = seqs.begin(); it != seqs.end(); ++it)
        {
            String_var name;
            CORBA_CALL_RETURN(name, (*(*it)), getName, ());
            m_sequenceToNamedObject[ name.in() ] = (*it);
            unsigned long key;
            CORBA_CALL_RETURN(key, (*(*it)), getKey, ());
        }
    }


    void SDKCommunicator::loadMonitorNamedObjects()
    {
        using namespace TA_Base_Bus;
        // build the map of console monitors
        VideoSwitchAgentFactory::VideoMonitorVector monitors = VideoSwitchAgentFactory::getInstance().getVideoMonitors();
        for(VideoSwitchAgentFactory::VideoMonitorVector::iterator it = monitors.begin(); it != monitors.end(); ++it)
        {
            String_var name;
            unsigned long consoleKey ;
            CORBA_CALL_RETURN(name, (*(*it)), getName, ());
            CORBA_CALL_RETURN(consoleKey, (*(*it)), getConsoleKey, ());
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "loadMonitorNamedObjects: name", name);
            m_monitorToNamedObject[name.in()] = (*it);
            if(consoleKey > 0)
            {
                m_consoleMonitorToNamedObject[ name.in() ] = (*it);
            }
            else
            {
                m_commonMonitorToNamedObject[ name.in() ] = (*it);
            }
        }
    }


    SDKCommunicator::CamToNamedObject SDKCommunicator::getCameraObjects()
    {
        if(m_camToNamedObject.empty())
        {
            loadCameraNamedObjects();
        }
        return m_camToNamedObject;
    }

    SDKCommunicator::SequenceToNamedObject SDKCommunicator::getSequenceObjects()
    {
        if(m_sequenceToNamedObject.empty())
        {
            loadSequenceNamedObjects();
        }
        return m_sequenceToNamedObject;
    }

    SDKCommunicator::MonitorToNamedObject SDKCommunicator::getConsoleMonitorObjects()
    {
        if(m_consoleMonitorToNamedObject.empty())
        {
            loadMonitorNamedObjects();
        }
        return m_consoleMonitorToNamedObject;
    }

    SDKCommunicator::MonitorToNamedObject SDKCommunicator::getCommonMonitorObjects()
    {
        if(m_commonMonitorToNamedObject.empty())
        {
            loadMonitorNamedObjects();
        }
        return m_commonMonitorToNamedObject;
    }

    void SDKCommunicator::updateSequence(Sequence* sequence)
    {
        try
        {
            if(m_sequenceToNamedObject.empty())
            {
                try
                {
                    loadSequenceNamedObjects();
                }
                catch(...)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error when loading sequence name object.");
                }
            }
            SequenceToNamedObject::iterator iter = m_sequenceToNamedObject.find(sequence->getName());
            TA_ASSERT(iter != m_sequenceToNamedObject.end(), "Sequence cannot be null - there is a corresponding state object for it");
            String_var description;
            CORBA_CALL_RETURN(description, (*(iter->second)), getSequenceDescription, ());
            sequence->setSequenceDescription(description.in());
            CORBA::Boolean ret;
            CORBA_CALL_RETURN(ret, (*(iter->second)), isReadOnly, ());
            sequence->setReadOnly(ret);
        }
        catch(TransactiveException& te)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what());
        }
        catch(const CORBA::Exception& cex)
        {
            std::string exceptionMsg("Exception thrown while updating sequence: ");
            exceptionMsg.append(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(cex));
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str());
        }
    }


    VideoMonitorNamedObject* SDKCommunicator::getMonitor(const std::string name)
    {
        if(m_monitorToNamedObject.empty())
        {
            try
            {
                loadMonitorNamedObjects();
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error when loading monitor name object.");
            }
        }
        MonitorToNamedObject::iterator it = m_monitorToNamedObject.find(name);
        return it != m_monitorToNamedObject.end() ? it->second : NULL;
    }

    VideoMonitorNamedObject* SDKCommunicator::getCommonMonitor(const std::string name)
    {
        if(m_commonMonitorToNamedObject.empty())
        {
            try
            {
                loadMonitorNamedObjects();
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error when loading monitor name object.");
            }
        }
        MonitorToNamedObject::iterator it = m_commonMonitorToNamedObject.find(name);
        return it != m_commonMonitorToNamedObject.end() ? it->second : NULL;
    }


    VideoMonitorNamedObject* SDKCommunicator::getConsoleMonitor(const std::string name)
    {
        if(m_consoleMonitorToNamedObject.empty())
        {
            try
            {
                loadMonitorNamedObjects();
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error when loading monitor name object.");
            }
        }
        MonitorToNamedObject::iterator it = m_consoleMonitorToNamedObject.find(name);
        return it != m_consoleMonitorToNamedObject.end() ? it->second : NULL;
    }

    CameraNamedObject* SDKCommunicator::getCamera(const std::string name)
    {
        CamToNamedObject::iterator it = m_camToNamedObject.find(name);
        return it != m_camToNamedObject.end() ? it->second : NULL;
    }

    SequenceNamedObject* SDKCommunicator::getSequence(const std::string name)
    {
        if(m_sequenceToNamedObject.empty())
        {
            try
            {
                loadSequenceNamedObjects();
            }
            catch(...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error when loading sequence name object.");
            }
        }
        SequenceToNamedObject::iterator it = m_sequenceToNamedObject.find(name);
        return it != m_sequenceToNamedObject.end() ? it->second : NULL;
    }

	//云台控制
	bool SDKCommunicator::SendPTZ(Camera* camera, FZ_ENUM::FZ_PTZ_CMD_E cmd)
	{
		FUNCTION_ENTRY("SendPTZ");
		try
		{
			FZ_STRUCT::FZ_STRUCT_CAMERAINFO pCam; 
			AddressHelper::Address address = AddressHelper::splitAddress(camera->getAddress());
			pCam.usStationID = address.stationID;
			pCam.usCameraID = address.cameraID;
			FZ_STRUCT::FZ_STRUCT_PTZCTRLCOMMAND ptz;
			ptz.ulPTZCmdID = cmd;
			UINT panSpeed=6;
			ptz.ulPTZCmdPara1 = panSpeed;
			UINT TSpeed=6;
			ptz.ulPTZCmdPara2 = TSpeed;
			ptz.ulPTZCmdPara3 = 0;
			FZ_ENUM::FZ_ENUM_ERROR err = m_fz_net->FZ_PtzCtrlCommand(pCam,ptz);
			m_errorMessage = GetSDKMessage(err);
			if(err != FZ_ENUM::FZ_ENUM_ERROR::ERR_SUCCESS)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SendPTZ failed, reason=%s", m_errorMessage.c_str());
				return  false;
			}
			else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SendPTZ successfully");
		}
		CATCH_ALL_EXCEPTION;

		FUNCTION_EXIT;
		return true;
	}

	bool SDKCommunicator::ptzRequest(Camera* camera, PTZDirection direction)
	{
		FZ_ENUM::FZ_PTZ_CMD_E cmd = convertPTZDirectionToCmd(direction);
		return SendPTZ(camera, cmd);
	}

    void SDKCommunicator::zoomCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd zoomDirection)
    {
        FUNCTION_ENTRY("zoomCamera");
        TA_ASSERT(verifyZoomCmd(zoomDirection), "");
        getCameraObjects();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Camera=%ld, zoomDirection=%d", camera->getKey(), zoomDirection);
        try
        {
            CameraNamedObject* cameraObj = getCamera(camera->getName());
            if(NULL == cameraObj)
            {
				throw TransactiveException(NO_REMOTE_CAMERA);
            }
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL((*cameraObj), ptzRequest, (zoomDirection, session.c_str()));
        }
		CATCH_ALL_EXCEPTION_WITH_MSGBOX;
        FUNCTION_EXIT;
    }

    void SDKCommunicator::panCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection)
    {
        FUNCTION_ENTRY("panCamera");
        TA_ASSERT(verifyPanCmd(panDirection), "");
        getCameraObjects();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Camera=%ld, panDirection=%d", camera->getKey(), panDirection);
        try
        {
            CameraNamedObject* cameraObj = getCamera(camera->getName());
            if(NULL == cameraObj)
            {
				throw TransactiveException(NO_REMOTE_CAMERA);
            }
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL((*cameraObj), ptzRequest, (panDirection, session.c_str()));
        }
		CATCH_ALL_EXCEPTION_WITH_MSGBOX;
        FUNCTION_EXIT;
    }
    void SDKCommunicator::tiltCamera(Camera* camera, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection)
    {
        FUNCTION_ENTRY("tiltCamera");
        TA_ASSERT(verifyTiltCmd(tiltDirection), "");
        getCameraObjects();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Camera=%ld, tiltDirection=%d", camera->getKey(), tiltDirection);
        try
        {
            CameraNamedObject* cameraObj = getCamera(camera->getName());
            if(NULL == cameraObj)
            {
				throw TransactiveException(NO_REMOTE_CAMERA);
            }
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL((*cameraObj), ptzRequest, (tiltDirection, session.c_str()));
        }
		CATCH_ALL_EXCEPTION_WITH_MSGBOX;
        FUNCTION_EXIT;
    }
    void SDKCommunicator::panAndTiltCamera(Camera* camera,
                                                   TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection,
                                                   TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection)
    {
        FUNCTION_ENTRY("panAndTiltCamera");
        TA_ASSERT(verifyPanAndTiltCmd(panDirection, tiltDirection), "");
        getCameraObjects();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Camera=%ld, panDirection=%d, tiltDirection=%d", camera->getKey(), panDirection, tiltDirection);
        try
        {
            CameraNamedObject* cameraObj = getCamera(camera->getName());
            if(NULL == cameraObj)
            {
				throw TransactiveException(NO_REMOTE_CAMERA);
            }
            std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            CORBA_CALL((*cameraObj), ptzRequest, (panDirection, session.c_str()));
            CORBA_CALL((*cameraObj), ptzRequest, (tiltDirection, session.c_str()));
        }
		CATCH_ALL_EXCEPTION_WITH_MSGBOX;
        FUNCTION_EXIT;
    }

    bool SDKCommunicator::verifyZoomCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd zoomDirection)
    {
        return (zoomDirection == TA_Base_Bus::CameraCorbaDef::ZOOM_IN_WX
                || zoomDirection == TA_Base_Bus::CameraCorbaDef::ZOOM_OUT_WX);
    }

    bool SDKCommunicator::verifyPanCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection)
    {
        return (panDirection == TA_Base_Bus::CameraCorbaDef::PAN_LEFT_WX
                || panDirection == TA_Base_Bus::CameraCorbaDef::PAN_RIGHT_WX);
    }

    bool SDKCommunicator::verifyTiltCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection)
    {
        return (tiltDirection == TA_Base_Bus::CameraCorbaDef::TILT_UP_WX
                || tiltDirection == TA_Base_Bus::CameraCorbaDef::TILT_DOWN_WX);
    }

    bool SDKCommunicator::verifyPanAndTiltCmd(TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd panDirection, TA_Base_Bus::CameraCorbaDef::PTZCtrlCmd tiltDirection)
    {
        return verifyPanCmd(panDirection) && verifyTiltCmd(tiltDirection);
    }

	FZ_ENUM::FZ_PTZ_CMD_E SDKCommunicator::convertPTZDirectionToCmd(PTZDirection direction)
	{
		switch(direction)
		{
		case PTZ_LEFT:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_PANLEFT;
		case PTZ_RIGHT:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_PANRIGHT;
		case PTZ_UP:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_TILTUP;
		case PTZ_DOWN:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_TILTDOWN;
		case PTZ_LEFTUP:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_LEFTUP;
		case PTZ_RIGHTUP:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_RIGHTUP;
		case PTZ_LEFTDOWN:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_LEFTDOWN;
		case PTZ_RIGHTDOWN:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_RIGHTDOWN;
		case PTZ_ZOOMIN:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_ZOOMTELE;
		case PTZ_ZOOMOUT:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_ZOOMWIDE;
		case PTZ_ALLSTOP:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_ALLSTOP;
		default:
			return FZ_ENUM::FZ_PTZ_CMD_E::MW_PTZ_ALLSTOP;
		}
	}

	void SDKCommunicator::acknowledgeAlarm(unsigned long alarmEntityKey)
	{
		try
		{
	        ::TA_Base_Core::AlarmDetailCorbaDef alarmDetail;
	        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	        std::auto_ptr<TA_Base_Core::IConsole> console(TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID));
			::CORBA::ULong consoleId = console->getKey();
			alarmDetail.assocEntityKey = alarmEntityKey;
			alarmDetail.subsystemKey = 17;
			alarmDetail.locationKey = 1;
			alarmDetail.state = TA_Base_Core::AlarmOpen;
			alarmDetail.decisionSupportState = TA_Base_Core::NoRelatedPlan;
			alarmDetail.mmsState = TA_Base_Core::MMS_NONE;
		    CORBA_CALL(TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getSwitchManager(), acknowledgeAlarm, (alarmDetail, consoleId));
		}
		catch (...)
		{
		}
	}

	

	/*
	void SDKCommunicator::SetIView(IGFVideoSwitchDisplayView * view)
	{
		m_IView = view;
	}
*/
} // namespace TA_IRS_App