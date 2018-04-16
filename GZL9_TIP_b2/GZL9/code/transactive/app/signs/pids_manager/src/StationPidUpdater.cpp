/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/signs/pids_manager/src/StationPidUpdater.cpp $
  * @author:  Anggiono
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * This class is used to communicate directly with the PID Agent. It uses its
  * NamedObject member to resolve the agent and make calls on it.
  * 
  * All of the arguments to the methods in this class take standard C++ types. This
  * class converts these types to equivalent CORBA types as necessary, makes the
  * request on the agent, and converts the CORBA types that are returned back into
  * C++ types (eg CORBA sequences into vectors).
  * 
  * If an error occurs while communicating with the agent, and an exception is
  * returned, this class converts the exception into a PidAgentException, so that
  * it can be caught by the dialog and an appropriate error message shown.
  * 
  * This class also allows dialogs to subscribe for updates to (pid and
  * predefined) messages from the PID Agent.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "StdAfx.h"
#include "app/signs/pids_manager/src/UserMessages.h"
#include "boost/tokenizer.hpp"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core\exceptions\src\DataUserSettingException.h"
#include "app/signs/pids_manager/src/StationPidUpdater.h"
#include "app/signs/pids_manager/src/PidAgentException.h"
#include "core\data_access_interface\PidsManager\src\IPidSettings.h"
#include "core\data_access_interface\PidsManager\src\PidDataAccessFactory.h"
//#include "bus\signs\pids_agent_access\src\PIDSAgentAccessFactory.h"

using namespace TA_Base_Bus;
using namespace TA_IRS_App;
using namespace TA_IRS_Core;
using namespace TA_Base_Core;
using TA_Base_Core::DataUserSettingException;

const std::string PIDAGENT_TYPE("PidsAgent");//"PidAgent");    // from entity type table	
const CString PIPE("|");

namespace TA_IRS_App
{
	TA_Base_Core::ReEntrantThreadLockable StationPidUpdater::m_singletonLock;
	StationPidUpdater* StationPidUpdater::m_instance = NULL;
	StationPidUpdater* StationPidUpdater::getInstance()
	{
		if ( 0 == m_instance )
		{
			// Double checking to prevent multiple threads
			// creating multiple instances.

			ThreadGuard guard( m_singletonLock );

			if ( 0 == m_instance )
			{
				m_instance = new StationPidUpdater();
			}
		}

		return m_instance;
	}

	void StationPidUpdater::removeInstance( )
	{
		// 
		// Guard this to prevent multiple threads atempting
		// to delete/create simultaneously
		//
		ThreadGuard guard( m_singletonLock );
		if ( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	/**
	  * throwPidAgentException
	  * 
	  * This logs the last exception and then converts it into a 
      * PidAgentException which then gets thrown back.
      *
      * IMPORTANT: This is NOT a function, it is an *exception handler* and
      * must ONLY be called WITHIN a *catch* block.
	  * 
	  * @return void - never returns
      * @param : const unsigned long lineNumber - line where exception was caught
	  * 
	  * @exception PidAgentException (always).
	  */
    void throwPidAgentException(const unsigned long lineNumber)
    {
        std::string message, exception;

        try
        {
            throw;  // re-throw the last exception
        }
        catch (const CORBA::SystemException& ex)
        {
            exception = "CORBA::SystemException";
            message   = CorbaUtil::getInstance().exceptionToString(ex);
        }
        catch (const CORBA::Exception& ex)
        {
            exception = "CORBA::Exception";
            message   = CorbaUtil::getInstance().exceptionToString(ex);
        }
        catch (const ObjectResolutionException& ex) 
        {
            exception = "ObjectResolutionException";
            message   = ex.what();
        }
        catch (const TransactiveException& ex) 
        {
            exception = "TransactiveException";
            message   = ex.what();
        }
        catch (const std::exception& ex) 
        {
            exception = "std::exception";
            message   = ex.what();
        }
        catch (...) 
        {
            exception = "...";
            message   = "Undefined exception!";            
        }

        std::ostringstream msg;
        msg << "Exception caught on line " << lineNumber 
            << " in file " << __FILE__ 
            << ". The reason is: " << message;

        LOG_EXCEPTION_CATCH(SourceInfo, exception, msg.str());

//        TA_THROW(PidAgentException(message));	// temp//test zhou yuan - for testing only
    }

	//
	/**
	  * getMyLocationKey
	  * 
	  * Retrieves the location from the runtime parameter and returns it.
      * Returns 0 if unable to get the location key.
	  * 
	  * @return unsigned long 
	  */
    unsigned long getMyLocationKey()
    {
        FUNCTION_ENTRY("StationPidUpdater");

        unsigned long locationKey = 0;   

        if (RunParams::getInstance().isSet(RPARAM_LOCATIONKEY))
        {            
            std::stringstream converter;
            converter << RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            converter >> locationKey;

            TA_ASSERT(locationKey > 0, "Location key is invalid.");
        }
        else
        {
            TA_ASSERT(false, "Location key is not available as a runtime parameter.");
        }

        FUNCTION_EXIT;
        return locationKey;
    }


	StationPidUpdater::StationPidUpdater()
	{
		FUNCTION_ENTRY("StationPidUpdater");
		m_settings = PidDataAccessFactory::getInstance().getUserSettings();
		FUNCTION_EXIT;
	}


	StationPidUpdater::~StationPidUpdater()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}


	std::vector<TA_IRS_Core::PidMessage> StationPidUpdater::getPidMessages()
	{
		FUNCTION_ENTRY("getPidMessages");                
		TA_IRS_Core::PidMessages pidMsgs;
		pidMsgs = m_settings->getPidMessages();
		FUNCTION_EXIT;
		return pidMsgs;
	}


	long StationPidUpdater::modifyPidMessage(const int id, const wstring& name, const wstring& value)
	{
		FUNCTION_ENTRY("modifyPidMessage");

		CORBA::Long pkey = -1;	// assume failure
		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"@@@@@@@@ setPidMessage (%d, %s, %s)",
					id, name.c_str(), value.c_str());
			m_settings->setSetting(id, name,value);
		}
		catch ( DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
			TA_THROW( DataUserSettingException("Error with the database somehow") );
		}
		catch ( DataUserSettingException& ex )
		{
			if ( ex.getFailType() != DataUserSettingException::SETTING_NOT_FOUND )
			{
				// This means that an error did actually occur so we will deal with it here.
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
				TA_THROW( DataUserSettingException("Configuration error with the actual user settings") );
			}
			// otherwise if the exception failure was SETTING_NOT_FOUND then we want to try 
			// adding the setting instead of updating it.
			try
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"@@@@@@@@ insert PidMessage (%d, %s, %s)",
					id, name.c_str(), value.c_str());
				m_settings->addSetting(id, name,value);
			}
			catch ( DatabaseException& )
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Don't care about the details so converting this into a generic UserSettingsException");
				//TA_THROW( UserSettingsException("Error with the database somehow") );
			}
			catch ( DataUserSettingException& )
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataUserSettingException", "Don't care about the details so converting this into a generic UserSettingsException");
				//TA_THROW( UserSettingsException("Configuration error with the actual user settings") );
			}
		}


		FUNCTION_EXIT;
		return pkey;
	}


	void StationPidUpdater::addListener(IPidMessageListener* listener)
	{
		FUNCTION_ENTRY("addListener");
	//    m_messageSubscriber.addListener(listener);
		FUNCTION_EXIT;
	}


	/************************************************************************/
	/* helper functions (corba -> app datatype)                             */
	/************************************************************************/
	//
	TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage StationPidUpdater::convertBackChineseDesc(CString chineseDesc)
	{
		FUNCTION_ENTRY("convertChineseDesc");
		IPIDSManagerCorbaDef::PidsMessage ret;
		int size = chineseDesc.GetLength();
		ret.length(size);

		std::string temp = chineseDesc.GetBuffer(size);
		std::string::iterator it = temp.begin();

		for(unsigned int i=0; i<size; ++i, ++it)
		{
			ret[i] = *it;
		}

		FUNCTION_EXIT;
		return ret;
	}

	//help function,saved unicode string to corba octet sequence
	TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage StationPidUpdater::wstringToCorbaSequence(const wstring& ws)
	{
		FUNCTION_ENTRY("wstringToCorbaSequence");
		IPIDSManagerCorbaDef::PidsMessage ret;
		const wchar_t *pwch = ws.c_str();
		const char *pch = (const char*)pwch;
		const int wsSize = ws.length()*2;
		ret.length(wsSize);
		for(int i = 0; i < wsSize; i++)
		{
			ret[i] = *(pch + i);
		}

		FUNCTION_EXIT;
		return ret;
	}
	/*
	void StationPidUpdater::clearPidMessage(bool bTrainPids, std::vector<std::string>& pids)
	{
		FUNCTION_ENTRY("clearPidMessage");	
		try
		{
			try
			{
				IPIDSManagerCorbaDef::PidsDestinationList targetPid;
				if(!bTrainPids)
				{
					//targetPid.length(pids.size());
					targetPid.length(6);
					int i = 0;
					int a_test[6] = {1,6,8,12,26,29};
					//for(std::vector<std::string>::iterator iter=pids.begin(); iter!=pids.end(); iter++)
					for(int kk = 0; kk < 6; kk++)
					{
						targetPid[i].station = a_test[kk];//(*iter).c_str();
						targetPid[i].downTrack = true;
						targetPid[i].upTrack = true;
						targetPid[i].concourse = true;
						i++;
					}
				}
				//m_pidsAgent->blankPID(pidName.c_str(),isBlank ? CORBA::Boolean(1) : CORBA::Boolean(0));
	//			CORBA_CALL(PIDSAgentAccessFactory::getInstance().getSPIDSAgent(), 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"@@@@@@@@ clear Pid Message");
				m_pidsAgent->submitClearRequest(bTrainPids, targetPid);

	//			UserMessages::getInstance().displayInfo(
	//                        str( format(UserMessages::INFO_REQUEST_SUCCESSFUL) % "信息清除" ).c_str());
				UserMessages::getInstance().displayInfo("信息清除发送成功");
			}//TD18706,jianghp, handle the operation mode
			catch ( const TA_Base_Bus::IPIDSManagerCorbaDef::PIDSServerNotConnectedException& )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "IPIDSManagerCorbaDef::SPIDSServerNotConnectedException", "" );

				UserMessages::getInstance().displayError(
					str( format(UserMessages::ERROR_REQUEST_FAILED)
					% "信息清除" % "等待PIDS服务器反应超时" ).c_str());
			}
			catch ( const TA_Base_Bus::IPIDSManagerCorbaDef::PIDSCommunicationFailureException& )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "IPIDSManagerCorbaDef::PIDSCommunicationFailureException", "" );

				UserMessages::getInstance().displayError(
					str( format(UserMessages::ERROR_REQUEST_FAILED)
					% "信息清除" % "PIDS 通信失败" ).c_str());
			}
			catch ( const TA_Base_Bus::IPIDSManagerCorbaDef::PIDSInvalidParameterException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "IPIDSManagerCorbaDef::PIDSInvalidParameterException", ex.details.in() );

				UserMessages::getInstance().displayError(
					str( format(UserMessages::ERROR_REQUEST_FAILED)
					% "信息清除" % ex.details.in() ).c_str());
			}
			catch ( const TA_Base_Core::ObjectResolutionException& ore )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::ObjectResolutionException", ore.what() );

				// TD 16346
							UserMessages::getInstance().displayError(
								str( format(UserMessages::ERROR_REQUEST_FAILED)
								% "信息清除" % "无法解析PIDS代理" ).c_str());
				// TD 16346
			}
			catch (TA_Base_Core::OperationModeException &) 
			{
				//m_pidsAgent.clear();
				//m_pidsAgent->blankPID(pidName.c_str(),isBlank ? CORBA::Boolean(1) : CORBA::Boolean(0));
			}
		}
		catch(...)
		{	
			throwPidAgentException(__LINE__);
		}
		FUNCTION_EXIT;	
	}
	*/

	void StationPidUpdater::InitCorbaObject()
	{
		UserMessages::getInstance().setMessageSuppression(false);
		try
		{
			unsigned long myLocationKey = getMyLocationKey();
			IEntityDataList pidAgentEntities =  EntityAccessFactory::getInstance().
				getEntitiesOfTypeAtLocation(PIDAGENT_TYPE, myLocationKey);  // may throw
			TA_ASSERT(pidAgentEntities.size() == 1, "Invalid number of local pid agents");
			m_pidsAgent.setEntityName(pidAgentEntities[0]->getName(),false);
		}
		catch (const DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch (const DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, 
						"Failed to initialize the m_pidsAgent parameter after "
						"calling the setEntityName method.");
		}
	}
}