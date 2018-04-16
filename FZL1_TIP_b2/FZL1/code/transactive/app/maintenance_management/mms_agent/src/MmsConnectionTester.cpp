
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/maintenance_management/mms_agent/src/MmsConnectionTester.cpp $
  * @author:  C. DeWolfe
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Implimentation of MmsConnectionTester
  *
  */

#include "app/maintenance_management/mms_agent/src/MmsConnectionTester.h"
//#include "app/maintenance_management/mms_agent/src/MmsApiWrapper.h"
#include "app/maintenance_management/mms_agent/src/MmsMessageUtil.h"
#include "app/maintenance_management/mms_agent/src/MmsServerState.h"

#include "app/maintenance_management/mms_agent/src/MmsConnectionException.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsConnectionCorbaDef.h"
#include "bus/maintenance_management/mmsHelper/src/MMSAPIManager.h"

namespace TA_IRS_App
{

	MmsConnectionTester::MmsConnectionTester()
		: m_pingPeriodSec(0), 
		  m_messageCheckPeriodMin(0), m_stopRequest(true)
	{
		FUNCTION_ENTRY("MmsConnectionTester");

		time(&m_lastPingTime);
		time(&m_lastMessageCheckTime);

		FUNCTION_EXIT;
	}



	MmsConnectionTester::~MmsConnectionTester()
	{
		FUNCTION_ENTRY("~MmsConnectionTester");

		FUNCTION_EXIT;
	}

	void MmsConnectionTester::run()
	{
		FUNCTION_ENTRY("checkForMessages");
		m_stopRequest = false;

		//chenlei++
		//init MmsServerState here
		//moved from MmsConnection::setToControlMode() function
		MmsServerState::getInstance()->initMmsServerState();

		while(m_stopRequest == false)
		{
            // AZ++ TES 4673
            try
            {
				//test the connection to the server if time a test is due
			    connectionTest();
            }
            catch (TA_Base_Core::TransactiveException& e)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, e.what());
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unknown exception");
            }
            // ++AZ
            

			//have a one second nap
			sleep(1000);
		}

	}

	void MmsConnectionTester::setPingPeriodSec(unsigned int seconds)
	{
		FUNCTION_ENTRY("setPingPeriodSec");

		m_pingPeriodSec = seconds;

		if(0 == m_pingPeriodSec)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"Pinging MMS server is currently disabled");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Now pinging the MMS Server every %ui seconds", seconds);
		}

		FUNCTION_EXIT;
	}

	void MmsConnectionTester::setMsgCheckPeriodSec(unsigned int seconds)
	{
		FUNCTION_ENTRY("setMsgCheckPeriodSec");

		m_messageCheckPeriodMin = seconds;

		if(0 == m_messageCheckPeriodMin)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
				"Checking for incomming messages is currently disabled");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Now checking for incomming messages every %ui seconds", seconds);
		}

		FUNCTION_EXIT;
	}

	void MmsConnectionTester::connectionTest()
	{
		//removed this cause I am sick of the logging
		//FUNCTION_ENTRY("connectionTest");

		time_t now;
		time(&now);
		time_t diff = now - m_lastPingTime;

		//if a test of the server connection is due...
		if(diff > static_cast<long>(m_pingPeriodSec))
		{
			if(0 != m_pingPeriodSec)
			{

				try
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
						"next call is MmsApiWrapper::ping()");
					 
					bool r=TA_Base_Bus::MMSAPIManager::getInstance()->isMMSLive();
					if (r==true)
					{
						MmsServerState::getInstance()->reportMmsServerSucess();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugPeriodic,
							"Successfully pinged MMS Server");				
					}
					else
					{
						TA_Base_Bus::MmsConnectionFailure failure("cannot connect with MMS", 0);
						MmsServerState::getInstance()->reportMmsServerFailure(failure);
					}					

				}
				catch(MmsConnectionException& ex)
				{
					TA_Base_Bus::MmsConnectionFailure failure(ex.getErrorMessage().c_str(), ex.getErrorCode());
                    MmsServerState::getInstance()->reportMmsServerFailure(failure);
				}


			}
			m_lastPingTime = now;
		}		

		//FUNCTION_EXIT;
	}

	 

	void MmsConnectionTester::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_stopRequest = true;

 

		FUNCTION_EXIT;
	}

	void MmsConnectionTester::initConfiguration(TA_Base_Core::MmsConnectiontEntityDataPtr pData)
	{
		FUNCTION_ENTRY("initConfiguration");

		setPingPeriodSec( pData->getPingPeriodSeconds() );
		setMsgCheckPeriodSec( pData->getMessageCheckSeconds() );

		FUNCTION_EXIT;
	}



}

