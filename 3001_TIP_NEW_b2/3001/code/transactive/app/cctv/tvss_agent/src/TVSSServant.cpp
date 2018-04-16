
#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include "app/cctv/tvss_agent/src/TVSSServant.h"
//#include "app/cctv/tvss_agent/src/TVSSException.h"
#include "app/cctv/tvss_agent/TCPCommunication/src/TcpMethodTimeoutException.h"
#include "app/cctv//tvss_agent/src/TVSSAlarmProcessor.h"
#include "app/cctv//tvss_agent/src/TrainBorneHeartBeatProcessor.h"
#include "core/message/src/MessagePublicationManager.h""
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmModificationHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"

//#include "core/message/src/namevaluepair.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/types/TrainCctvCameraAlarms_MessageTypes.h"
#include "core/message/types/TVSSComms_MessageTypes.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"

#include <list>
using namespace TA_Base_Core;
using namespace TA_IRS_App;
using namespace TA_Base_Bus;

TVSSServant::TVSSServant(std::string strTCPAddress1,std::string strTCPAddress2,std::string strTrainBorneAddress1,std::string strTrainborneAddress2,unsigned long timeout)
: m_primaryAddress(strTCPAddress1),
  m_secondaryAddress(strTCPAddress2),
  m_primaryTrainborneAddress(strTrainBorneAddress1),
  m_secondaryTrainborneAddress(strTrainborneAddress2),
  m_tvss(NULL), 
  m_active(false),
  m_timeout(timeout),
  m_alarmProcessor(NULL),
  m_trainborneHeartbeatProcessor(NULL),
  m_msgSender( std::auto_ptr< TA_Base_Core::CommsMessageSender >( 
  TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( 
  TA_Base_Core::TVSSAgentComms::Context ) ) )
{
	FUNCTION_ENTRY("TVSSServant: Constructor");

	m_serverLinkStatusThread = new TVSSServerLinkStatusUpdateThread();
	m_tvss = new TVSS(this,m_serverLinkStatusThread);
	m_alarmAddressMap.clear();
	m_alarmKeyMap.clear();
  TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, "TvssPec");

  try
  {
	std::string locationKeyStr = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
	if(locationKeyStr.empty())
	{
		throw TransactiveException("Locationkey runparam is not configured");
	}
	unsigned long locationKey = atoi(locationKeyStr.c_str());
    TA_Base_Core::IEntityDataList monitors = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(VideoMonitor::getStaticType(), locationKey);
    for(TA_Base_Core::IEntityDataList::iterator it = monitors.begin(); it != monitors.end(); ++it)
    {
      unsigned long key = (*it)->getKey();
      std::string address = (*it)->getAddress();
      unsigned char monitorId = atoi(address.c_str());
      m_monitorIdToKeyMap.insert(std::make_pair(monitorId, key));
      delete *it;
    }
  }
  catch(const TransactiveException& e)
  {
    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, e.what());
  }
	FUNCTION_EXIT;

}

TVSSServant::~TVSSServant()
{
	FUNCTION_ENTRY("TVSSServant: Destructor");
	
	if(m_tvss){
		delete m_tvss;
		m_tvss = NULL;
	}
	
	m_serverLinkStatusThread->terminateAndWait();
	delete m_serverLinkStatusThread;
	m_alarmAddressMap.clear();
	m_alarmKeyMap.clear();
	FUNCTION_EXIT;
}

void TVSSServant::setEntity(TA_Base_Core::IEntityData &EntityData)
{
	FUNCTION_ENTRY("TVSSServant: setEntity");

	LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "setEntity(%s)", EntityData.getDescription().c_str());

	m_objectName      = EntityData.getAgentName();
	m_EntityKey       = EntityData.getKey();
	m_entityLocation  = EntityData.getLocation();
	m_entitySubsystem = EntityData.getSubsystem();
	m_entityTypeKey   = EntityData.getTypeKey();
  m_locationName    = EntityData.getLocationName();
	//string strLocationName = EntityData.getLocationName();
	//m_alarmProcessor = new TVSSAlarmProcessor(strLocationName,this);
	m_tvss->setParameters(m_primaryAddress, m_secondaryAddress,m_primaryTrainborneAddress, m_secondaryTrainborneAddress,m_timeout);
	m_tvss->setEntity(EntityData);

	FUNCTION_EXIT;

}

void TVSSServant::setAlarmProcessor(TVSSAlarmProcessor* processor)
{
	m_alarmProcessor = processor;
}
/*****************************************
 * Implement TVSSManagerCorbaDef Methods *
 *****************************************/

void TVSSServant::tvssClearImage(CORBA::Long trainId,CORBA::Char monitorId )
{
	FUNCTION_ENTRY("TVSSServant: tvssClearImage");
	
	TA_Base_Bus::GenericAgent::ensureControlMode();

	TVSSClearImageMethod clearImageMethod((unsigned char)trainId,monitorId);
	try
	{
		LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssClearImage(%ld,%d)",trainId,monitorId);
		m_tvss->sendMethod(clearImageMethod);
	}	
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssClearImage exception %s",e.what.in() );
		throw(e);
	}
	catch(TcpMethodTimeoutException& te)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("No response from TVSS hardware before time-out"));
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Unknown Exception"));
	}
  m_monitorIdToAssignedTrainMap.erase(monitorId);
	FUNCTION_EXIT;

}
//void TVSSServant::tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId, CORBA::Char cameraW, CORBA::Char cameraX, CORBA::Char cameraY, CORBA::Char cameraZ )
void TVSSServant::tvssQuadScreen(CORBA::Long trainId,CORBA::Char monitorId, CORBA::Char quadId )

{
	FUNCTION_ENTRY("TVSSServant: tvssQuadScreen");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	std::string strErrorMsg = "tvssQuadScreen error:";
	if(trainId<=0 || monitorId <=0 || quadId<=0 )
	{
		strErrorMsg.append("input parameter error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
//	std::vector<unsigned char>Cameras;
	//Put the four cameras in vector container.
//	Cameras.push_back(cameraW);
//	Cameras.push_back(cameraX);
//	Cameras.push_back(cameraY);
//	Cameras.push_back(cameraZ);

	TVSSQuadScreenMethod quadScreen((unsigned char)trainId,monitorId,quadId);
	try
	{
		//char chCameras[64];
		//sprintf(chCameras,"%d,%d,%d,%d",cameraW,cameraX,cameraY,cameraZ);
		LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"quadScreen(%ld,%d,%d)",trainId,monitorId,quadId);
		m_tvss->sendMethod(quadScreen);
	}
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"quadScreen exception %s",e.what.in());
		throw(e);
	}
	catch(TcpMethodTimeoutException& te)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("No response from TVSS hardware before time-out"));
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Unknown Exception"));
	}
  m_monitorIdToAssignedTrainMap[monitorId] = trainId;
	FUNCTION_EXIT;
}
void TVSSServant::tvssSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX )
{
	FUNCTION_ENTRY("TVSSServant: tvssSingleScreen");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	std::string strErrorMsg = "tvssSingleScreen error:";
	if(trainId<=0 || monitorId <=0 || cameraX <=0)
	{
		strErrorMsg.append("input parameter error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	TVSSSingleScreenMethod singleScreen((unsigned char)trainId,monitorId,cameraX);
	try
	{
		LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssSingleScreen(%ld,%d,%d)",trainId,monitorId,cameraX);
		m_tvss->sendMethod(singleScreen);
	}
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssSingleScreen exception %s",e.what.in());
		throw(e);
	}
	catch(TcpMethodTimeoutException& te)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("No response from TVSS hardware before time-out"));
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Unknown Exception"));
	}
  m_monitorIdToAssignedTrainMap[monitorId] = trainId;
	FUNCTION_EXIT;
}

void TVSSServant::tvssPecSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX )
{
	FUNCTION_ENTRY("TVSSServant: tvssPecSingleScreen");

	TA_Base_Bus::GenericAgent::ensureControlMode();

  if(m_monitorIdToAssignedTrainMap.find(monitorId) != m_monitorIdToAssignedTrainMap.end())
  {
    tvssClearImage(m_monitorIdToAssignedTrainMap[monitorId], monitorId);
    // Update trainborne GUI
  	TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand tvssCommand;
  	CORBA::Any cobTvssCommand;
  	tvssCommand.trainId = 0;
  	tvssCommand.monitorId = monitorId;
  	tvssCommand.videoInput[0] = 0;
  	tvssCommand.videoInput[1] = 0;
  	tvssCommand.videoInput[2] = 0;
  	tvssCommand.videoInput[3] = 0;
		tvssCommand.cmdType = TA_Base_Bus::TVSSManagerCorbaDef::ClearImage;
		cobTvssCommand <<= tvssCommand;
		m_msgSender->sendCommsMessage(TA_Base_Core::TVSSAgentComms::TrainBorneMonitorUpdate,m_EntityKey,cobTvssCommand,m_entitySubsystem,m_entityLocation );
  }
  tvssSingleScreen(trainId, monitorId, cameraX);
  // Update trainborne GUI
	TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand tvssCommand;
	CORBA::Any cobTvssCommand;
	tvssCommand.trainId = trainId;
	tvssCommand.monitorId = monitorId;
	tvssCommand.videoInput[0] = cameraX;
	tvssCommand.videoInput[1] = 0;
	tvssCommand.videoInput[2] = 0;
	tvssCommand.videoInput[3] = 0;
	tvssCommand.cmdType = TA_Base_Bus::TVSSManagerCorbaDef::SingleScreen;
	cobTvssCommand <<= tvssCommand;
	m_msgSender->sendCommsMessage(TA_Base_Core::TVSSAgentComms::TrainBorneMonitorUpdate,m_EntityKey,cobTvssCommand,m_entitySubsystem,m_entityLocation );

  // Call vss agent, send 0x06 & 0x25 to vss
  if(m_monitorIdToKeyMap.find(monitorId) == m_monitorIdToKeyMap.end())
  {
    std::stringstream ss;
    ss << "Can not find the monitor,address:" << (unsigned long)monitorId;
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(ss.str().c_str()));
  }
  try
  {
    VideoSwitchAgentFactory::getInstance().setLocationName(m_locationName);
    VideoSwitchAgentFactory::VideoOutputNamedObject* monObj =
      VideoSwitchAgentFactory::getInstance().getVideoOutput(m_monitorIdToKeyMap[monitorId]);
    // Vss clear monitor 0x06
    CORBA_CALL((*monObj), requestSwitchOfInput, (0, "", true));
    // 0x25
    CORBA_CALL((*monObj), mappingTrainBorneMonitor, ("", true));
  }
  catch (...)
  {
    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Vss command failed");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Vss command failed"));
  }
	FUNCTION_EXIT;
}

void TVSSServant::tvssSequencingScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char sequenceId )
{
	FUNCTION_ENTRY("TVSSServant: tvssSingleScreen");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	std::string strErrorMsg = "tvssSequencingScreen error:";
	if(trainId<=0 || monitorId <=0 || sequenceId <=0)
	{
		strErrorMsg.append("input parameter error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	TVSSSequencingScreenMethod sequencingScreen((unsigned char)trainId,monitorId,sequenceId);
	try
	{
		LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssSequencingScreen(%ld,%d,%d)",trainId,monitorId,sequenceId);
		m_tvss->sendMethod(sequencingScreen);
	}	
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssSequencingScreen exception %s",e.what.in());
		throw(e);
	}
	catch(TcpMethodTimeoutException& te)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("No response from TVSS hardware before time-out"));
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Unknown Exception"));
	}
  m_monitorIdToAssignedTrainMap[monitorId] = trainId;
	FUNCTION_EXIT;

}

void TVSSServant::tvssAcknowledgeEvent(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX,CORBA::Char cameraY )
{
	FUNCTION_ENTRY("TVSSServant: tvssAcknowledgeEvent");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	std::string strErrorMsg = "tvssAcknowledgeEvent error:";
	if(trainId<=0 || monitorId <=0 || cameraX <=0)
	{
		strErrorMsg.append("input parameter error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	//TVSSClearImageMethod clearImageMethod((unsigned char)trainId,monitorId);
	TVSSAcknowledgeMethod acknowledgeMethod((unsigned char)trainId,monitorId,cameraX,cameraY);
	try
	{
		LOG4(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssAcknowledgeEvent(%ld,%d,%d,%d)",trainId,monitorId,cameraX,cameraY);
		//m_tvss->sendMethod(clearImageMethod);
		m_tvss->sendMethod(acknowledgeMethod);
	}	
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssAcknowledgeEvent exception %s",e.what.in());
		throw(e);
	}
	catch(TcpMethodTimeoutException& te)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("No response from TVSS hardware before time-out"));
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException("Unknown Exception"));
	}
  m_monitorIdToAssignedTrainMap[monitorId] = trainId;
	FUNCTION_EXIT;

}

void TVSSServant::HandleTVSSTrainBorneEvent(long trainID,const char*eventStr)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();

	tvssTrainBorneEvent(trainID,eventStr);
}

void TVSSServant::tvssTrainBorneEvent(CORBA::Long trainID,const char* eventStr)
{
	FUNCTION_ENTRY("TVSSServant: tvssTrainBorneEvent");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	char chTrainID[16];
	std::string strEvent;
	for(int eventIndex = 0 ;eventIndex<TRAINBORNE_EVENT_NUMBER;eventIndex++)
	{
		unsigned char thisByte = eventStr[eventIndex/8];
		//bool bAlaramActive=(eventStr[eventIndex-1]=='1');
		unsigned char maskByte = 1<<(eventIndex%8);
		bool bAlaramActive= (thisByte & maskByte)!=0;
		strEvent.append(bAlaramActive?"1":"0");
	}
	LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssTrainBorneEvent(%ld,%s)",trainID,strEvent.c_str());
	std::string strErrorMsg = "tvssTrainBorneEvent error:";
	if(trainID<=0)
	{
		strErrorMsg.append("input parameter error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	sprintf(chTrainID,"%ld",trainID);
	char chEntityAddress[64];
	std::string entityAddress;

	//sqlStatement.append(chTrainID);
	//sqlStatement.append(":%");

	if(m_alarmAddressMap.empty())
		return;
	
	for(int eventIndex = 0 ;eventIndex<TRAINBORNE_EVENT_NUMBER;eventIndex++)
	{
		unsigned char thisByte = eventStr[eventIndex/8];
		//bool bAlaramActive=(eventStr[eventIndex-1]=='1');
		unsigned char maskByte = 1<<(eventIndex%8);
		bool bAlaramActive= (thisByte & maskByte)!=0;
		sprintf(chEntityAddress,"%ld:%d",trainID,eventIndex+1);
		entityAddress=chEntityAddress;
		std::map<std::string,TrainBorneCameraAlarm*>::iterator map_it = m_alarmAddressMap.find(entityAddress);
		if(map_it == m_alarmAddressMap.end())
		{
			LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"TrainBorne Camera with address %s can not be found in database",chEntityAddress);
			continue;
		}
		
		std::map<std::string,bool>::iterator cache_it = m_eventAlarmCache.find(entityAddress);
		
		if(cache_it==m_eventAlarmCache.end() ||(cache_it!=m_eventAlarmCache.end() && m_eventAlarmCache[entityAddress]!=bAlaramActive))
		{
			if(ChangeAlaram(map_it->second,trainID,bAlaramActive))
				m_eventAlarmCache[entityAddress]=bAlaramActive;
			else
			{
				strErrorMsg.append( bAlaramActive ?"failed to submitAlarm":"failed to closeAlarm");
				throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
			}
		}
	}
	FUNCTION_EXIT;
}

bool TVSSServant::ChangeAlaram(TrainBorneCameraAlarm* entityData,long trainID,bool bAlarmActived)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();

	if(bAlarmActived)
	{
		return RaiseAlaram(entityData,trainID);
	}
	else
	{
		return CloseAlaram(entityData);
	}
}

bool TVSSServant::RaiseAlaram(TrainBorneCameraAlarm* entityData,long trainID)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();

	try
	{
	//	std::string assetName;
		unsigned long key = entityData->pkey;
	//	TA_Base_Core::EntityHelper help(key ,TYPE_TRAINBORNE_CAMERA_ALARM);
		char chTrainID[32];
		sprintf(chTrainID,"%ld",trainID);
		std::string strTrainID(chTrainID);
	//	assetName = help.getParameter("AssetName");
	//	TA_Base_Core::NameValuePair pair0( "TRAINID", strTrainID );
	//	TA_Base_Core::NameValuePair pair1( "ADDRESS", entityData->getAddress() );
		TA_Base_Core::NameValuePair pair0( "CAMERAALARM_DESCRIPTION", entityData->description);
		TA_Base_Core::DescriptionParameters descriptionParameters;
	//	descriptionParameters.push_back(&pair0);
	//	descriptionParameters.push_back(&pair1);
		descriptionParameters.push_back(&pair0);
		TA_Base_Core::EntityHelper helper(entityData->pkey, TYPE_TRAINBORNE_CAMERA_ALARM);
		TA_Base_Core::NameValuePair omFlag("OMFlag", helper.getParameter("OMFlag"));
        descriptionParameters.push_back(&omFlag);
		TA_Base_Core::NameValuePair paramValue("Value","TRIGGERED");
		descriptionParameters.push_back(&paramValue);
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm,
			key,			// entity key
			entityData->typeKey,		// entity type key
			descriptionParameters,
			entityData->name,		// entity name
			entityData->locationKey,	// location key
			entityData->subSystemKey,	// subystem key
			entityData->assetName,
			TA_Base_Core::AlarmConstants::defaultTime );
		LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Alarm raised.Entity key[%ld],trainID[%ld]",key,trainID);
		return true;
	}
	catch (...)
	{
		LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Fail to raise alarm.Entity key[%ld],trainID[%ld]",entityData->pkey,trainID);
		return false;
	}

}

bool TVSSServant::CloseAlaram(TrainBorneCameraAlarm* entityData)
{
	TA_Base_Bus::GenericAgent::ensureControlMode();

	try
	{
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
			TA_Base_Core::TrainCctvCameraAlarms::TrainCctvCameraAlarm, entityData->pkey, entityData->locationKey, "NORMAL");
		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Alarm closed.Entity key[%ld]",entityData->pkey);
		return true;
	}
	catch (...)
	{
		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Fail to close alarm.Entity key[%ld]",entityData->pkey);
		return false;
	}
}

void TVSSServant::setEntityAddressMap(std::map<std::string,TrainBorneCameraAlarm*> alarmAddressMap)
{
	if(alarmAddressMap.empty())
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Entity Address map is empty");
		return;
	}
	m_alarmAddressMap = alarmAddressMap;
}

void TVSSServant::setEntityKeyMap(std::map<unsigned long,TrainBorneCameraAlarm*> entityKeyMap)
{
	if(entityKeyMap.empty())
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Entity Key map is empty");
		return;
	}
	m_alarmKeyMap=entityKeyMap;
}

void TVSSServant::setMonitorConsoleMap(std::map<unsigned long,TrainBorneMonitor*> monitorConsoleMap)
{
	if(monitorConsoleMap.empty())
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Monitor Console Map is empty");
		return;
	}
	m_monitorConsoleMap=monitorConsoleMap;
}

void TVSSServant::tvssEquipmentAlarmQuery( )
{
	FUNCTION_ENTRY("TVSSServant: tvssEquipmentAlarmQuery");

	std::string strErrorMsg = "tvssEquipmentAlarmQuery error:";

	TVSSEquipmentQueryMethod equipmentAlarmQuery;
	int dpCount = m_alarmProcessor->GetDPCount();
	bool* alarmStatus = new bool[dpCount];
	//memset(alarmStatus,0,sizeof(alarmStatus));			// sizeof(alarmStatus) will get 4 always, it is point size
	memset(alarmStatus,0,dpCount * sizeof(bool));
	try
	{
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssEquipmentAlarmQuery");
		m_tvss->sendEquipmentAlarmMethod(equipmentAlarmQuery,alarmStatus);
		for(int i =0; i<dpCount; i++)
		{
			m_alarmProcessor->SetDataPointValue(i,alarmStatus[i]);
		}
	}	
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		m_alarmProcessor->setDataPointsAsBadValue();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssEquipmentAlarmQuery exception %s",e.what.in());
	}
	catch(TcpMethodTimeoutException& te)
	{
		m_alarmProcessor->setDataPointsAsBadValue();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"No response from TVSS hardware before time-out");
	}
	catch(...)
	{
		m_alarmProcessor->setDataPointsAsBadValue();
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
	}
	delete []alarmStatus;
	FUNCTION_EXIT;
}

void TVSSServant::tvssAcknowledgeAlarm(::CORBA::ULong entityKey, const char* sessionID)
{
	FUNCTION_ENTRY("TVSSServant: tvssAcknowledgeAlarm");

	TA_Base_Bus::GenericAgent::ensureControlMode();

	LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssAcknowledgeAlarm(%ld,%s)",entityKey,sessionID);
	std::string strErrorMsg="tvssAcknowledgeAlarm error:";
	if(m_alarmKeyMap.empty())
	{
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"TrainBorne Alarm Key Map is empty");
		strErrorMsg.append("Alarm EntityKey Map empty");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		return;
	}
	std::map<unsigned long,TrainBorneCameraAlarm*>::iterator map_it = m_alarmKeyMap.find(entityKey);
	if(map_it==m_alarmKeyMap.end())
	{
		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"TrainBorne Alarm with enityKey %lu can not be found in database",entityKey);
		strErrorMsg.append("Alarm EntityKey not found");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		return;
	}	
	//Get TrainID according to CameraAlarm Address
	std::string strAddress =(*map_it).second->address;
	int trainID=0,alarmIndex;
	int rtn = sscanf(strAddress.c_str(),"%d:%d",&trainID,&alarmIndex);
	if(0==rtn || EOF ==rtn)
	{
		LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"TrainBorne CameraAlarm address format error.EntityKey[%lu],Address[%s]",entityKey,strAddress.c_str());
		strErrorMsg.append("TrainBorne CameraAlarm address format error");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		return;
	}
	//Get the camera index according to CameraAlarm's CameraNumber parameter

	//TA_Base_Core::EntityHelper help( entityKey,TYPE_TRAINBORNE_CAMERA_ALARM);
	//std::string strCameraNumbers=help.getParameter("CameraNumbers");
	std::string strCameraNumbers = (*map_it).second->cameraNumbers;
	int cameraIndex[2]={0,0};
	if(strCameraNumbers.find(','))
	{
		rtn = sscanf(strCameraNumbers.c_str(),"%d,%d",&cameraIndex[0],&cameraIndex[1]);
		if(0==rtn || EOF ==rtn)
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"TrainBorne CameraAlarm parameter CameraNumbers format error.EntityKey[%lu],CameraNumber[%s]",entityKey,strCameraNumbers.c_str());
			strErrorMsg.append("TrainBorne CameraAlarm address format error");
			throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		}
	}
	else
	{
		cameraIndex[0]=atoi(strCameraNumbers.c_str());
		if(cameraIndex[0]<=0)
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"TrainBorne CameraAlarm parameter CameraNumbers format error.EntityKey[%lu],CameraNumber[%s]",entityKey,strCameraNumbers.c_str());
			strErrorMsg.append("TrainBorne CameraAlarm address format error");
			throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		}
	
	}
	//Get monitor address according to sessionID
	TA_Base_Core::IConsole* console;
	console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession( sessionID );
	unsigned long consoleKey = console->getKey();
	std::map<unsigned long,TrainBorneMonitor*>::iterator monitor_it = m_monitorConsoleMap.find(consoleKey);
	if(monitor_it == m_monitorConsoleMap.end())
	{
		LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Can not find TrainBorneMonitor according to sessionID[%s],consoleKey[%lu]",sessionID,consoleKey);
		strErrorMsg.append("local monitor map is empty");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
		return;
	}
	std::string strMonitorAddress= m_monitorConsoleMap[consoleKey]->m_addr;
	int monitorAddress = atoi(strMonitorAddress.c_str() );
	TA_Base_Bus::TVSSManagerCorbaDef::TVSSCommand tvssCommand;
	CORBA::Any cobTvssCommand;
	tvssCommand.trainId = trainID;
	tvssCommand.monitorId = monitorAddress;
	tvssCommand.videoInput[0] =cameraIndex[0];
	tvssCommand.videoInput[1] =cameraIndex[1];
	tvssCommand.videoInput[2] =0;;
	tvssCommand.videoInput[3] =0;
	if(cameraIndex[1]!=0)
	{
		tvssCommand.cmdType = TA_Base_Bus::TVSSManagerCorbaDef::QuadScreenAlarm;
	//	tvssCommand.videoInput[1] = cameraIndex[1];
	//	tvssQuadScreen(trainID,monitorAddress,cameraIndex[0],cameraIndex[1],0,0);
	//	tvssAcknowledgeEvent(trainID,monitorAddress,cameraIndex[0],cameraIndex[1]);
	}
	else 
	{
		tvssCommand.cmdType = TA_Base_Bus::TVSSManagerCorbaDef::SingleScreenAlarm;
	//	tvssSingleScreen(trainID,monitorAddress,cameraIndex[0]);
		
	}
	//tvssAcknowledgeEvent(trainID,monitorAddress,cameraIndex[0],cameraIndex[1]);
	try
	{
		cobTvssCommand <<= tvssCommand;
		m_msgSender->sendCommsMessage(TA_Base_Core::TVSSAgentComms::TrainBorneMonitorUpdate,consoleKey,cobTvssCommand,m_entitySubsystem,m_entityLocation );
		
	}
	catch(...)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError, 
			"Failed to send the TrainBorneMonitorUpdate data");
		strErrorMsg.append("Failed to send the TrainBorneMonitorUpdate data");
		throw(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException(strErrorMsg.c_str()));
	}
	FUNCTION_EXIT;
}

// Implement IEntity Interface Functions
bool TVSSServant::isValid(void)
{
	FUNCTION_ENTRY("TVSSServant: isValid");
	FUNCTION_EXIT;
	return true;
}
void TVSSServant::start(void)
{
	FUNCTION_ENTRY("TVSSServant: start");
	m_tvss->start();
	activateServantWithName(m_objectName);
	FUNCTION_EXIT;
}
void TVSSServant::stop(void)
{
	FUNCTION_ENTRY("TVSSServant: stop");
	deactivateServant();
	FUNCTION_EXIT;
}
void TVSSServant::update(const class TA_Base_Core::ConfigUpdateDetails &details)
{
	FUNCTION_ENTRY("TVSSServant: update");
	FUNCTION_EXIT;
}
void TVSSServant::remove(void)
{
	FUNCTION_ENTRY("TVSSServant: remove");
	FUNCTION_EXIT;
}
void TVSSServant::setToControlMode(void)
{
	FUNCTION_ENTRY("TVSSServant: setToControlMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting TVSS servant %s to Control mode",m_objectName.c_str());
	m_active = true;	
	m_eventAlarmCache.clear();
	m_tvss->setToControlMode();
	m_alarmProcessor->SetControlMode();
    m_trainborneHeartbeatProcessor->setControlMode();
	FUNCTION_EXIT;
}
void TVSSServant::setToMonitorMode(void)
{
	FUNCTION_ENTRY("TVSSServant: setToMonitorMode");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Setting TVSS servant %s to Monitor mode",m_objectName.c_str());
	m_active = false;
	m_tvss->setToMonitorMode();
	m_alarmProcessor->SetMonitorMode();
    m_trainborneHeartbeatProcessor->setMonitorMode();
	FUNCTION_EXIT;
}
void TVSSServant::onTerminate(bool peerAvailable)
{
	FUNCTION_ENTRY("TVSSServant: setToMonitorMode");
	m_tvss->onTerminate(peerAvailable);
	FUNCTION_EXIT;
}

void TA_IRS_App::TVSSServant::onRunParamChange( const std::string& name, const std::string& value )
{
//void TVSSServant::tvssPecSingleScreen(CORBA::Long trainId, CORBA::Char monitorId, CORBA::Char cameraX )
  size_t pos1 = value.find_first_of(';');
  size_t pos2 = value.find_first_of(';', pos1 + 1);
  std::string trainIdStr = value.substr(0, pos1);
  std::string monitorIdStr = value.substr(pos1+1, pos2-pos1-1);
  std::string cameraIdStr = value.substr(pos2+1);
  unsigned long trainId = atol(trainIdStr.c_str());
  unsigned char monitorId = atoi(monitorIdStr.c_str());
  unsigned char cameraId = atoi(cameraIdStr.c_str());
  try
  {
    tvssPecSingleScreen(trainId, monitorId, cameraId);
  }
  catch (...)
  {
  }
}

int TVSSServant::GetDPCount()
{
	return m_alarmProcessor->GetDPCount();
}

void TA_IRS_App::TVSSServant::tvssTrainborneHeartbeat()
{
	FUNCTION_ENTRY("TVSSServant: tvssTrainborneHeartbeat");

	try
	{
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"tvssTrainborneHeartbeat");
		m_tvss->sendTrainbornHeartbeatMethod();
	}	
	catch(TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"tvssTrainborneHeartbeat exception %s",e.what.in());
	}
    catch(...)
	{
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Unknown Exception");
	}

	FUNCTION_EXIT;
}

void TA_IRS_App::TVSSServant::setTrainborneHeartbeatProcessor( TrainBorneHeartBeatProcessor* trainborneHeartbeatProcessor )
{
    m_trainborneHeartbeatProcessor = trainborneHeartbeatProcessor;
}
