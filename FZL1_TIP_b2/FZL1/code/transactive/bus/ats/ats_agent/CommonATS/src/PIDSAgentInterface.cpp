
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PIDSAgentInterface.cpp $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * The object that responsible for sending data to PIDS Agent
  */

#include "bus/ats/ats_agent/CommonATS/src/PIDSAgentInterface.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"


namespace TA_IRS_Bus
{
	PIDSAgentInterface::PIDSAgentInterface()
	{
		FUNCTION_ENTRY("PIDSAgentInterface");

		//Initialize PIDS Object
		initializePIDSObject();

		FUNCTION_EXIT;
	}

	PIDSAgentInterface::~PIDSAgentInterface()
	{
		FUNCTION_ENTRY("PIDSAgentInterface");
		FUNCTION_EXIT;
	}

	void PIDSAgentInterface::initializePIDSObject()
	{
		FUNCTION_ENTRY("initializePIDSObject");

		try
		{
			unsigned long locationKey = atol( TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() );
			TA_Base_Core::CorbaNameList pidsAgentNameList =
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation("PidsAgent", locationKey, true);

			TA_ASSERT(pidsAgentNameList.size() == 1, "No or more than one PidsAgent entity found in OCC");
			m_pidsAgent.setCorbaName(pidsAgentNameList[0]);
			// Set CORBA Timeout to 2sec. 
			m_pidsAgent.setObjectTimeout(2);

		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
		catch (const TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "...", 
				"Exception caught while attempting to resolve the trainAgent.");
		} 

		FUNCTION_EXIT;
	}

	void PIDSAgentInterface::processPIDSInformation( const TA_Base_Bus::DataBlock<WORD>& pidsData )
	{
		FUNCTION_ENTRY("processPIDSInformation");

		std::vector<unsigned char> PIDSData;
		for (int nCount=pidsData.start(); nCount <= pidsData.end(); nCount++)
		{
			unsigned short data = pidsData[nCount];
			PIDSData.push_back(data >> 8);   //For High Byte
			PIDSData.push_back(data & 0xff); //For Low Byte;
		}

		//ProcessPIDS Information by Bytes
		processPIDSInformation(PIDSData);

		FUNCTION_EXIT;
	}

	void PIDSAgentInterface::processPIDSInformation(const std::vector<unsigned char>& data)
	{
		FUNCTION_ENTRY("processPIDSInformation");

		if (data.size() == 0)
			return;

		int dataBlockSize = data.size();
		TA_Base_Bus::IPIDSManagerCorbaDef::SigData sigData;
		sigData.length(dataBlockSize);

		int count = 0;
		for ( int sizeCount=0; sizeCount < dataBlockSize; sizeCount++ )
		{
			sigData[count++] = data[sizeCount];
		}

		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"send sumbitSigRequest to PidsAgent with %d bytes of data", dataBlockSize);

			CORBA_CALL( m_pidsAgent, submitSigRequest, (sigData) );
		}
		catch (const TA_Base_Core::ObjectResolutionException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
		}
		catch(const CORBA::Exception& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", 
				TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "unknown", "unknown exception.");
		}

		FUNCTION_EXIT;
	}
}
