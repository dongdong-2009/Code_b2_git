#include "RemoteApiRadioCommand.h"

#include "core/utilities/src/DebugUtil.h"
IRemoteApiRadioCommand::IRemoteApiRadioCommand(MCC7500SDK::IAPIRequest_ptr &ptr,MCC7500SDK::METHOD_ID mid):pAPIRequest(ptr),method_id(mid)
{
}

IRemoteApiRadioCommand::~IRemoteApiRadioCommand()
{
}
void IRemoteApiRadioCommand::execute()
{
try
{
	pAPIRequest->ApiRequest(method_id,in,out,result);

}
catch(...)
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Exception while calling the method!");
}
}