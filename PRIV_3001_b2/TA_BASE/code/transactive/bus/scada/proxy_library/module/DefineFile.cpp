#include "DefineFile.h"

GDefine* GDefine::m_pGDefine =NULL;

GDefine* GDefine::getInstance()
{
	if ( NULL == m_pGDefine )
	{
		m_pGDefine = new GDefine();
	}
	return m_pGDefine;
}

GDefine::~GDefine()
{
	if (pDataNodeCorbaTypes != NULL)
	{
		delete pDataNodeCorbaTypes;
	}
}

GDefine::GDefine()
{		

		std::string cmdLine = "--run-param-file=D:/LocalTesting/3001_TIP_NEW/unitTest_start.ini";
		::TA_Base_Core::UtilityInitialiser::initialiseUtilities(cmdLine);
		
		pDataNodeCorbaTypes = new ::TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence();
		b_DataNodeBulkPollingSynchronizerException = false;

		::TA_Base_Core::gSetRuntimeBreakAction(BREAK_NO);
		::TA_Base_Core::gSetRuntimeAssertAction(FAIL_EXCEPTION);
}

void GDefine::reset()
{
	
}

boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> GDefine::getDataNodeStaticConfiguration()
{
	//boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> pDataNodeStaticConfigruation( &dataNodeStaticConfigruation );
	if (NULL == pDataNodeStaticConfigruation.get())
	{
		boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> t_pDataNodeStaticConfigruation( new TA_Base_Bus::DataNodeStaticConfiguration() );
		pDataNodeStaticConfigruation = t_pDataNodeStaticConfigruation;
	}	
	return pDataNodeStaticConfigruation;
}
	
