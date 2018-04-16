


#include "app/radio/radio_manager_gui/src/TrainAgentInterface/TrainAgentIF.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include <string>

namespace TA_IRS_App
{
	TrainAgentIF::TrainAgentIF(void)
	{
		TA_IRS_Bus::TrainAgentProxyFactory& trainAgentProxyFactory = TA_IRS_Bus::TrainAgentProxyFactory::getInstance();
		m_trainDriverCallCorbaProxy = trainAgentProxyFactory.createTrainDriverCallCorbaProxy();
	}

	TrainAgentIF::~TrainAgentIF(void)
	{
	}

	bool TrainAgentIF::answerDriverCall (unsigned long lTrainId)
	{
		
		FUNCTION_ENTRY ("answerDriverCall");
		bool bDriverCallSuccess = true;
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Activating CallDriver Call from Radio Mngr Gui");

		std::string sessionId = TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID );

		try
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Driver Call executed with Session ID : %s @ Train ID : %d",sessionId.c_str(),lTrainId);
			m_trainDriverCallCorbaProxy->answerOccCallRequest (lTrainId,sessionId);
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,
				"Exception Thrown during answering driver call");
			
			bDriverCallSuccess = false;
		}
		
		FUNCTION_EXIT;
		return bDriverCallSuccess;
	}
}