
#ifndef DEFINE_HELP_FILE_H__
#define DEFINE_HELP_FILE_H__

#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/scada/common_library/IDL/src/DataNodeCorbaTypes.h"
#include "bus/scada/common_library/src/CommonDefs.h"


enum ExceptionEnum
{
	EENoException		= 0,
	EENull,
	EETransactive,
	EEObjectResolution,
	EEScadaProxy,
	EEBadParameter,
	EEOperationMode,
	EECorbaException
};


class GDefine
{
public:
	static GDefine* getInstance();
	void reset();
	~GDefine();
	boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> getDataNodeStaticConfiguration();
private:
	GDefine();
public:
	static GDefine* m_pGDefine;
	bool DataNodeAccessHelp_delete;
	// data
	::TA_Base_Bus::DataNodeCorbaTypes::SyncUpdateSequence * pDataNodeCorbaTypes;	
	//::TA_Base_Bus::DataNodeStaticConfiguration				dataNodeStaticConfigruation;	
	boost::shared_ptr<TA_Base_Bus::DataNodeStaticConfiguration> pDataNodeStaticConfigruation;
	ExceptionEnum g_exception;
	bool b_DataNodeBulkPollingSynchronizerException;
};

//GDefine* GDefine::m_pGDefine = NULL;

#define g_pDefine GDefine::getInstance()

#endif //DEFINE_HELP_FILE_H__