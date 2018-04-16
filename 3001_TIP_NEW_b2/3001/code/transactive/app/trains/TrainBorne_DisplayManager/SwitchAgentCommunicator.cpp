#include "SwitchAgentCommunicator.h"

#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/cctv/video_switch_agent/agent_access/src/VideoSwitchAgentFactory.h"
#include "bus/cctv/video_switch_agent/idl/src/ExceptionsCorbaDef.h"

#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/naming/src/inamedobject.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
namespace TA_IRS_App
{
    SwitchAgentCommunicator* SwitchAgentCommunicator::m_theInstance = 0;
    TA_Base_Core::ReEntrantThreadLockable SwitchAgentCommunicator::m_singletonLock;
	std::string SwitchAgentCommunicator::m_locationName="";

	SwitchAgentCommunicator::SwitchAgentCommunicator()
	{
		//m_trainCorbaProxy=TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainCctvCorbaProxy();
		TA_Base_Core::CorbaNameList names = TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( 
			"TVSSAgent", m_locationName,true );
		m_tvssManagerNameObj.setCorbaName(names[0]);
	}

	SwitchAgentCommunicator::~SwitchAgentCommunicator()
	{
		m_tvssManagerNameObj.clear();
	}

	void SwitchAgentCommunicator::setLocationName(std::string locationName)
	{
		m_locationName = locationName;
	}

    SwitchAgentCommunicator& SwitchAgentCommunicator::getInstance()
    {
        // Guard the method to ensure access by client code is synchronous
        ThreadGuard guard( m_singletonLock );

        if( m_theInstance == 0 )
        {
            m_theInstance = new SwitchAgentCommunicator();
        }

        return *m_theInstance;
    }

	bool SwitchAgentCommunicator::mappingTrainBorneMonitor(unsigned long monitorKey)
	{
		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"mappingTrainBorneMonitor(%ld)",monitorKey);
		bool result=false;
		try
		{
			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* monitorObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutput(monitorKey);
			CORBA_CALL_RETURN(result,(*monitorObj),mappingTrainBorneMonitor,(session.c_str(),true));
			return result;
		}
		catch (TA_Base_Core::TransactiveException& te)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,te.what(),"TransactiveException",MB_OK);
		}
		catch (TA_Base_Bus::VideoSwitchAgentException& ex)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in() );
			MessageBox(NULL,"Failed to mapping TrainBorneMonitor","VideoSwitchAgentException",MB_OK);
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "Exception thrown while mapping TrainBorneMonitor: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA",MB_OK);
		}
		catch (...)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );
			MessageBox(NULL,"Unknown exception","Unknown",MB_OK);
		}
		return result;
	}

	bool SwitchAgentCommunicator::setSingleScreen(long trainID,unsigned char monitorID,unsigned char cameraID)
	{
		try
		{
			LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"setSingleScreen(%ld,%d,%d)",trainID,monitorID,cameraID);
			CORBA_CALL(m_tvssManagerNameObj,tvssSingleScreen,(trainID,monitorID,cameraID));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while setSingleScreen: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }
		return false;
	}

//	bool SwitchAgentCommunicator::setQuad(long trainID,unsigned char monitorID,unsigned char cameraW,unsigned char cameraX,unsigned char cameraY,unsigned char cameraZ)
	bool SwitchAgentCommunicator::setQuad(long trainID,unsigned char monitorID,unsigned char quadId)
	{
		try
		{
			char cameras[64];
		//	sprintf(cameras,"%d,%d,%d,%d",cameraW,cameraX,cameraY,cameraZ);
			LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"setQuad(%ld,%d,%d)",trainID,monitorID,quadId);
			CORBA_CALL(m_tvssManagerNameObj,tvssQuadScreen,(trainID,monitorID,quadId));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while setQuad: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }
		return false;
	}
	bool SwitchAgentCommunicator::setSequence(long trainID,unsigned char monitorID,unsigned char sequenceID)
	{
		try
		{
			LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"setSequence(%ld,%d,%d)",trainID,monitorID,sequenceID);
			CORBA_CALL(m_tvssManagerNameObj,tvssSequencingScreen,(trainID,monitorID,sequenceID));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while setSequence: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }
		return false;
	}

	bool SwitchAgentCommunicator::vssClearMonitor(unsigned long monitorKey)
	{
		LOG1(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"vssClearMonitor(%lu)",monitorKey);
		try
		{
			std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_Base_Bus::VideoSwitchAgentFactory::VideoOutputNamedObject* monitorObj = TA_Base_Bus::VideoSwitchAgentFactory::getInstance().getVideoOutput(monitorKey);
			CORBA_CALL((*monitorObj),requestSwitchOfInput,(0, session.c_str(),true));
			return true;
		}
		catch (TA_Base_Core::TransactiveException& te)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,te.what(),"TransactiveException",MB_OK);
		}
		catch (TA_Base_Bus::VideoSwitchAgentException& ex)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::VideoSwitchAgentException", ex.what.in() );
			MessageBox(NULL,"Failed to clear TrainBorneMonitor","VideoSwitchAgentException",MB_OK);
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "Exception thrown while clearing monitor: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA",MB_OK);
		}
		catch (...)
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Unknown" );
			MessageBox(NULL,"Unknown exception","Unknown",MB_OK);
		}
		return false;
	}

	bool SwitchAgentCommunicator::clearMonitor(long trainID,unsigned char monitorID)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"clearMonitor(%ld,%d)",trainID,monitorID);
			CORBA_CALL(m_tvssManagerNameObj,tvssClearImage,(trainID,monitorID));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while clearMonitor: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }
		return false;
	}

	bool SwitchAgentCommunicator::trainBorneAlarmEvent(long trainID,const char* eventStr)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"trainBorneAlarmEvent(%ld,%s)",trainID,eventStr);
			CORBA_CALL(m_tvssManagerNameObj,tvssTrainBorneEvent,(trainID,eventStr));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "Exception thrown while trainBorneAlarmEvent: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
		}
		return false;
	}

	bool SwitchAgentCommunicator::trainBorneAcknowleageAlarm(unsigned long entityKey,const char* sessionID)
	{
		try
		{
			LOG2(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"trainBorneAcknowleageAlarm(%ld,%s)",entityKey,sessionID);
			CORBA_CALL(m_tvssManagerNameObj,tvssAcknowledgeAlarm,(entityKey,sessionID));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
	//		MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "Exception thrown while trainBorneAcknowleageAlarm: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
		}
		return false;
	}

	bool SwitchAgentCommunicator::setAcknowledgeEvent(long trainID,unsigned char monitorID,unsigned char cameraW,unsigned char cameraX)
	{
		try
		{
			char cameras[64] = {0};
			sprintf(cameras,"%d,%d",cameraW,cameraX);
			LOG3(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"setAcknowledgeEvent(%ld,%d,%s)",trainID,monitorID,cameras);
			CORBA_CALL(m_tvssManagerNameObj,tvssAcknowledgeEvent,(trainID,monitorID,cameraW,cameraX));
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			//		MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "Exception thrown while trainBorneAcknowleageAlarm: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
		}
		return false;
	}

	bool SwitchAgentCommunicator::equipmentAlarmQuery()
	{
		try
		{
			CORBA_CALL(m_tvssManagerNameObj,tvssEquipmentAlarmQuery,());
			return true;
		}
		catch (TA_Base_Bus::TVSSManagerCorbaDef::TVSSException& e)
		{
			// object resolution etc
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TVSSException", e.what.in() );
			MessageBox(NULL,e.what,"TVSSException",MB_OK);
		}
		catch (TA_Base_Core::TransactiveException& te)
        {
            // object resolution etc
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TransactiveException", te.what() );
			MessageBox(NULL,"Agent cannot be  contacted","TransactiveException",MB_OK);
        }
        catch( const CORBA::Exception& cex )
        {
			std::string exceptionMsg( "Exception thrown while equipmentAlarmQuery: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
			MessageBox(NULL,exceptionMsg.c_str(),"CORBA Exception",MB_OK);
        }
		return false;
	}
	/*void SwitchAgentCommunicator::setSingleScreen(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera)
	{
		std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		try
		{
			m_trainCorbaProxy->setSingleScreen(trainID,camera,session);
		}
		catch( TA_IRS_Bus::AgentCommunicationException& e )
		{
			;
		}
		catch(TA_IRS_Bus::OperationNotSupportedException& e)
		{
			;
		}
		catch(TA_IRS_Bus::TrainAccessDeniedException& e)
		{
			;
		}
		catch(TA_IRS_Bus::InvalidTrainException& e)
		{
			;
		}
		catch(TA_IRS_Bus::InvalidParameterException& e)
		{
			;
		}
		catch(TA_IRS_Bus::TrainTransmissionFailureException& e )
		{
			;
		}
		catch(TA_IRS_Bus::TrainTimeoutException& e)
		{
			;
		}
		catch(TA_IRS_Bus::TransactionCancelledException& e)
		{
			;
		}
		catch(TA_IRS_Bus::MpuChangeoverException& e)
		{
			;
		}
		catch(TA_IRS_Bus::BadCommandException& e)
		{
			;
		}
		catch(TA_IRS_Bus::TrainRadioException& e)
		{
			;
		}

		catch(TA_IRS_Bus::CctvConflictException& e)
		{
			;
		}
		catch(TA_IRS_Bus::CctvException& e)
		{
			;
		}
		catch(TA_IRS_Bus::PecException& e)
		{
			;
		}
		catch(TA_IRS_Bus::InvalidSelectionListException& e)
		{
			;
		}
		catch(TA_IRS_Bus::GroupInUseException& e)
		{
			;
		}
		//catch(TA_IRS_Bus::AgentCommunicationException& e)
		//{
		//	;
		//}
		catch(...)
		{
			;
		}
	}
	void SwitchAgentCommunicator::setQuadScreen(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ)
	{
		std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		m_trainCorbaProxy->setQuadScreen(trainID,cameraW,cameraX,cameraY,cameraZ,session);
	}

	void SwitchAgentCommunicator::setSequence(TA_IRS_Bus::CommonTypes::TrainIdType trainID,TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequenceNo,TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode sequenceMode)
	{
		std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		try
		{
			m_trainCorbaProxy->setSequence(trainID,sequenceNo,sequenceMode,session);
		}
		catch( TA_IRS_Bus::AgentCommunicationException& e )
		{
			MessageBox(NULL,"AgentCommunicationException","",MB_OK);
		}
		catch(TA_IRS_Bus::OperationNotSupportedException& e)
		{
			MessageBox(NULL,"OperationNotSupportedException","",MB_OK);
		}
		catch(TA_IRS_Bus::TrainAccessDeniedException& e)
		{
			MessageBox(NULL,"TrainAccessDeniedException","",MB_OK);
		}
		catch(TA_IRS_Bus::InvalidTrainException& e)
		{
			MessageBox(NULL,"InvalidTrainException","",MB_OK);
		}
		catch(TA_IRS_Bus::InvalidParameterException& e)
		{
			MessageBox(NULL,"InvalidParameterException","",MB_OK);
		}
		catch(TA_IRS_Bus::TrainTransmissionFailureException& e )
		{
			MessageBox(NULL,"TrainTransmissionFailureException","",MB_OK);
		}
		catch(TA_IRS_Bus::TrainTimeoutException& e)
		{
			MessageBox(NULL,"TrainTimeoutException","",MB_OK);
		}
		catch(TA_IRS_Bus::TransactionCancelledException& e)
		{
			MessageBox(NULL,"TransactionCancelledException","",MB_OK);
		}
		catch(TA_IRS_Bus::MpuChangeoverException& e)
		{
			MessageBox(NULL,"MpuChangeoverException","",MB_OK);
		}
		catch(TA_IRS_Bus::BadCommandException& e)
		{
			MessageBox(NULL,"BadCommandException","",MB_OK);
		}
		catch(TA_IRS_Bus::TrainRadioException& e)
		{
			MessageBox(NULL,"TrainRadioException","",MB_OK);
		}
		catch(TA_IRS_Bus::CctvConflictException& e)
		{
			MessageBox(NULL,"CctvConflictException","",MB_OK);
		}
		catch(TA_IRS_Bus::CctvException& e)
		{
			MessageBox(NULL,"CctvException","",MB_OK);
		}
		catch(TA_IRS_Bus::PecException& e)
		{
			MessageBox(NULL,"PecException","",MB_OK);
		}
		catch(TA_IRS_Bus::InvalidSelectionListException& e)
		{
			MessageBox(NULL,"InvalidSelectionListException","",MB_OK);
		}
		catch(TA_IRS_Bus::GroupInUseException& e)
		{
			MessageBox(NULL,"GroupInUseException","",MB_OK);
		}
		//catch(TA_IRS_Bus::AgentCommunicationException& e)
		//{
		//	;
		//}
		catch(...)
		{
			MessageBox(NULL,"...","",MB_OK);
		}
	}*/
}