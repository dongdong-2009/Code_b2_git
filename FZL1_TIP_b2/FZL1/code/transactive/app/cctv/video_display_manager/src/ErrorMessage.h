#pragma once;

#include <string>
#include "core/exceptions/src/TransactiveException.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "core/utilities/src/DebugUtilMacros.h"

const std::string NO_REMOTE_MONITOR = "远程监视器对象不存在";
const std::string NO_REMOTE_CAMERA = "远程摄像机对象不存在";
const std::string NO_REMOTE_SEQUENCE = "远程序列对象不存在";
const std::string ERROR_OCCUR_WHEN_OPERATION = "显示控制服务错误";
const std::string NO_RIGHTS = "没有操作权限";
const std::string SEQUENCE_DEWELLTIME_INVALID = "非空序列间隔时间不能为0";
const std::string NO_RIGHTS_ENGLISH = "No rights";

#define CATCH_ALL_EXCEPTION \
	    catch(TA_Base_Core::TransactiveException& e) \
        { \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", e.what()); \
			if(NO_REMOTE_MONITOR != e.what()) \
			{ \
	            throw TA_Base_Core::TransactiveException(ERROR_OCCUR_WHEN_OPERATION); \
			} \
			throw; \
        } \
        catch(TA_Base_Bus::VideoSwitchAgentException& e) \
        { \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "VideoSwitchAgentException", e.what.in()); \
			if(strcmp(e.what.in(), NO_RIGHTS_ENGLISH.c_str()) == 0) \
			{ \
				throw TA_Base_Core::TransactiveException(NO_RIGHTS); \
			} \
            throw TA_Base_Core::TransactiveException(ERROR_OCCUR_WHEN_OPERATION); \
        } \
        catch(const CORBA::Exception& e) \
        { \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()); \
            throw TA_Base_Core::TransactiveException(ERROR_OCCUR_WHEN_OPERATION); \
        }


#define CATCH_ALL_EXCEPTION_WITH_MSGBOX \
	    catch(TA_Base_Core::TransactiveException& e) \
        { \
			std::string errorMsg = e.what(); \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", e.what()); \
			if(NO_REMOTE_MONITOR != errorMsg) \
			{ \
			    errorMsg = ERROR_OCCUR_WHEN_OPERATION; \
			} \
	        AfxMessageBox(errorMsg.c_str() , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION); \
        } \
        catch(TA_Base_Bus::VideoSwitchAgentException& e) \
        { \
			std::string errorMsg = e.what.in(); \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "VideoSwitchAgentException", e.what.in()); \
			if(NO_RIGHTS_ENGLISH == errorMsg) \
			{ \
				errorMsg = NO_RIGHTS; \
			} \
			else \
			{ \
				errorMsg = ERROR_OCCUR_WHEN_OPERATION; \
			} \
	        AfxMessageBox(errorMsg.c_str() , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION); \
        } \
        catch(const CORBA::Exception& e) \
        { \
            LOG_GENERIC(SourceInfo, DebugUtil::ExceptionCatch, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str()); \
		    std::string errorMsg = ERROR_OCCUR_WHEN_OPERATION; \
	        AfxMessageBox(errorMsg.c_str() , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION); \
        }