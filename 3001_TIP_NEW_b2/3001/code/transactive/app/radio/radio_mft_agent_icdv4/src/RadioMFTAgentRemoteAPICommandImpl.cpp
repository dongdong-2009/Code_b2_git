#include "RadioMFTAgentRemoteAPICommandImpl.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/idl/src/CommonExceptionsCorbaDef.h"

#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentCommandImpl.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Function.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h"
#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleStateManager.h"

#include "Mca/mca.h"
#include "Mca/volume.h"
#include "Mca/critical_resource_select.h"

#include <vector>
#include <ctime>
#include <sstream>
#define NULL_HEADSET_SELECT 0
#include "app/radio/radio_manager_gui_icdv4/src/DebugMsg.h"

const int RadioMFTAgentRemoteAPICommandImpl::CONNECTION_LIMIT = 3;

RadioMFTRemoteAPI::IRemoteApiRadioCommand::IRemoteApiRadioCommand(MCC7500SDK::IAPIRequest_ptr &ptr,MCC7500SDK::METHOD_ID mid):pAPIRequest(ptr),method_id(mid)
{
}

RadioMFTRemoteAPI::IRemoteApiRadioCommand::~IRemoteApiRadioCommand()
{
}
void RadioMFTRemoteAPI::IRemoteApiRadioCommand::execute()
{
try
{
	pAPIRequest->ApiRequest(method_id,in,out,result);

}
catch(...)
{
	char error[1024];
	sprintf(error,"Exception while calling the method : %u",method_id);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,error);
	string error_str= "Unable to perform requested operation";
	TA_IRS_App::ConsoleDispatchException ex(error_str);
	throw ex;
}
}
std::vector<RadioResource> RadioMFTAgentRemoteAPICommandImpl::getResourceCollection(RadioResourceType type)
{
    FUNCTION_ENTRY("getResourceCollection");

	checkApState();
	
	std::vector<RadioResource> resources = 
		The_StatusManager::instance()->getMFTResourceCollection(type);
	
    FUNCTION_EXIT;

    return resources;
}

RadioMFTAgentRemoteAPICommandImpl::RadioMFTAgentRemoteAPICommandImpl(string ior,string username,string password):m_pEventDispatcher(&m_pFutureManager,IAPIRequest),connection(IAPIRequest,ior.c_str(),username.c_str(),password.c_str(),m_pEventDispatcher)
{
	m_pEventDispatcher.setMFTCommand(this);
	m_serverAgent.setEntityName(RadioMFTAgentHelper::getRadioServerAgentEntityName(),true);
}

RadioMFTAgentRemoteAPICommandImpl::~RadioMFTAgentRemoteAPICommandImpl(void)
{
}


void RadioMFTAgentRemoteAPICommandImpl::assignResource(RadioResource& resource)
{
    FUNCTION_ENTRY("assignResource");

	checkApState();

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling AssignResource API for resource ID = %u",resource.id);
	RadioMFTRemoteAPI::AssignResource pFunction(IAPIRequest,resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL, MCA_NULL_CRITICAL_RESOURCE, NULL_HEADSET_SELECT); //NULL_HEADSET is not find on API document
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Resource : %u  ClientID: %u",resource.id,clientID);
	try
	{
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute AssignResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

	FutureResultT future;
	future.set(ASSIGN_RESOURCE);
    future.m_resourceType = resource.type;
	future.m_resourceId = resource.id;

	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::deassignResource(RadioResource& resource,LogicalChannel channel /* =0 */)
{
    FUNCTION_ENTRY("deassignResource");

	checkApState();

	if ( channel == 0)
	{
		channel = The_StatusManager::instance()->getChannel(resource.id);
	}

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	RadioMFTRemoteAPI::DeassignResource pFunction(IAPIRequest, channel,clientID, MCA_DEASSIGN_NORMAL);

	try
	{
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute DeassignResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

	FutureResultT future;
	future.set(DEASSIGN_RESOURCE);
    future.m_resourceType = resource.type;
	future.m_resourceId = resource.id;
    future.m_channel = channel;

	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::selectResource(RadioResource& resource)
{
    FUNCTION_ENTRY("selectResource");

	checkApState();

	LogicalChannel channel = 0; 

	FutureResultT future;
	ACE_Future<FutureResultT> aceFuture;	
	
	try	
	{
		switch(resource.type)
		{
			case RR_RADIO:
			case RR_TALKGROUP:
			case RR_SITE:
			{
				channel = The_StatusManager::instance()->getChannel(resource.id);

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Calling McaSelectResource with channel %u id %u",channel,resource.id);
				RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest, channel);
				pFunction.execute();
				
				future.set(SELECT_RESOURCE);
                future.m_resourceType = resource.type;
                future.m_resourceId = resource.id;

				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}	
			break;

			case RR_ANNGROUP:
				selectAG(resource);	
                break;

			case RR_DYNGROUP:
			{
                RadioResource dynGroup = The_StatusManager::instance()->getDynamicGroupForResource(resource);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DynamicGroupForResource = %d", dynGroup.id);
                if (dynGroup.type != RR_NIL)
                {
				    channel = The_StatusManager::instance()->getChannel(dynGroup.id);

					RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest, channel);
				    pFunction.execute();
				    
				    future.set(SELECT_RESOURCE);
                    future.m_resourceType = dynGroup.type;
                    future.m_resourceId = dynGroup.id;

				    aceFuture.set(future);

				    m_pFutureManager.attachFuture(aceFuture);

					// TD14133
					The_StatusManager::instance()->setSelectedDG(dynGroup,resource.id);
                }
			}
			break;

			case RR_PATCHGROUP:
			{	
				std::vector<RadioResource> members = The_StatusManager::instance()->getGroupMembers(resource.id);
                if (members.empty())
                {
                    return;
                }

				// select first TG member
				std::vector<RadioResource>::iterator iter = members.begin(); 
				channel = The_StatusManager::instance()->getChannel((*iter).id);

				RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest, channel);
				pFunction.execute();

				future.set(SELECT_RESOURCE);
                future.m_resourceType = (*iter).type;
                future.m_resourceId = (*iter).id;

				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);

				The_StatusManager::instance()->setFeatureAttributeIsPatchInvolved(*iter,resource.id); // TD14133
			}
			break;
			
			case RR_MSELECT:
			case RR_MSITE:
			{	int grpNum = The_StatusManager::instance()->getGroupNumber(resource.id);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling openMsel API");
				if ( grpNum != 0)
				{
					RadioMFTRemoteAPI::OpenMSel pFunction(IAPIRequest, grpNum);
					pFunction.execute();
					
					future.set(OPEN_MSEL);
					aceFuture.set(future);
					m_pFutureManager.attachFuture(aceFuture);
				}
				else
				{
					TA_THROW(TA_Base_Core::TransactiveException("MselGroup number is 0"));
				}
			}
			break;
			
			default:
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Cannot select resource of this type.");
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute SelectResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::deselectResource(RadioResource& resource,LogicalChannel channel /* =0 */)
{
    FUNCTION_ENTRY("deselectResource");

	checkApState();
	
    ResourceIdentifier resourceId = resource.id;

	if ( channel == 0)
	{
		switch (resource.type)
		{
			case RR_RADIO:
			case RR_TALKGROUP:
			case RR_SITE:
			case RR_ANNGROUP: // TD16878
				break;

			case RR_PATCHGROUP:
			{
				std::vector<RadioResource> rList = The_StatusManager::instance()->getResourceMembers(resource);
				int nSize = rList.size();

				for ( int i= 0; i<nSize; i++ )
				{
					// find the first member that is selected
					if (The_StatusManager::instance()->getFeatureAttributeIsSelected(rList[i])==SELECTED_SPEAKER)
					{
						resourceId = rList[i].id;
						The_StatusManager::instance()->setFeatureAttributeIsPatchInvolved(rList[i],0); // TD14133
						break;
					}
					if (i == nSize -1)
					{
						// nothing to be deselected
						return;
					}
				}
			}
			break;


			case RR_DYNGROUP:
			{
        		RadioResource dynGroup = The_StatusManager::instance()->getDynamicGroupForResource(resource);
				resourceId = dynGroup.id;
				
				The_StatusManager::instance()->setSelectedDG(dynGroup,0); // TD14133
			}
			break;
			default:
				// cannot deselect
				return;
		}

		channel = The_StatusManager::instance()->getChannel(resourceId);
	}

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling DeselectResource API channel = %u", channel);
	RadioMFTRemoteAPI::DeselectResource pFunction(IAPIRequest, channel);

	try
	{
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute DeselectResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

	FutureResultT future;
	future.set(DESELECT_RESOURCE);
    future.m_resourceType = resource.type;
    future.m_resourceId = resourceId;
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);
	
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::changeVolume(RadioResource& resource, unsigned int volume)
{
    FUNCTION_ENTRY("changeVolume");

	checkApState();

	LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

	RadioMFTRemoteAPI::ChangeVolume pFunction(IAPIRequest, channel, volume);

	try
	{
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ChangeVolume on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		throw e;
	}

	FutureResultT future;
	future.set(CHANGE_VOLUME);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::connectPatchGroup(
	RadioResource& resource,
	CommandState state/* =START */,
	unsigned int grpNo/* =0 */)
{
    FUNCTION_ENTRY("connectPatchGroup");

	checkApState();

	switch(state)
	{
		case START:
		{
			
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConnectPatchGroup, starting...");
            if (The_StatusManager::instance()->hasPendingConnectResource(RR_PATCHGROUP))
            {
                throw TA_Base_Core::TransactiveException("Another patch group is being connected");
            }

			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);

            if (members.size() < 1)
            {
                throw TA_Base_Core::TransactiveException("Cannot connect with no members members in the group");
            }

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ConnectPatchGroup, Number of members: %u members.", members.size());
			try
			{
	            FutureResultT future;
	            ACE_Future<FutureResultT> aceFuture;

				RadioMFTRemoteAPI::GetFreePatchGroup pFunction(IAPIRequest);
				pFunction.execute();
				
				future.set(GET_FREE_PATCHGRP);
				future.m_param = resource.id;

                future.m_resourceType = resource.type;
                future.m_resourceId = resource.id;

				aceFuture.set(future);
				m_pFutureManager.attachFuture(aceFuture); 

                // limit the patch group connection to one at a time
                The_StatusManager::instance()->setPendingConnectResource(resource);

                The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTION_BUSY);
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				TA_THROW(TA_Base_Core::TransactiveException(e.what()));
			}
		}
		break;

		case ACTION:
		{
			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);		

			std::vector<RadioResource>::iterator iter = members.begin();

			// assign first to ensure that a channel is associated with this member resource
			for (; iter != members.end(); ++iter)
			{
				if ((iter->type==RR_PRIVATECALL) || (iter->type==RR_SPI))
				{
					addPatchMember(*iter,grpNo);
				}
				else if ((!The_StatusManager::instance()->getFeatureAttributeIsAssigned(*iter)) ||
					     (iter->type==RR_BIM))
				{
					try
					{
	                    FutureResultT future;
	                    ACE_Future<FutureResultT> aceFuture;

						unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

						RadioMFTRemoteAPI::AssignResource pFunction(IAPIRequest, iter->id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL, MCA_NULL_CRITICAL_RESOURCE, NULL_HEADSET_SELECT);

						pFunction.execute();

						future.set(ASSIGN_RESOURCE);
						future.m_param = grpNo;
                        future.m_resourceType = iter->type;
                        future.m_resourceId = iter->id;
						future.m_state = ADD_PATCH_MEMBER;

						aceFuture.set(future);
						m_pFutureManager.attachFuture(aceFuture); 
					}
					catch(TA_IRS_App::ConsoleDispatchException& e)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
					}
				}
				else
				{
					addPatchMember(*iter,grpNo);
				}
			}
		}
		break;

		case END:
			addPatchMember(resource,grpNo);
            break;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::addPatchMember(RadioResource& resource, unsigned int grpNum)
{
    FUNCTION_ENTRY("addPatchMember");
    
	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling AddPatchMember API with group no = %u, channel = %u", grpNum,channel);
		RadioMFTRemoteAPI::AddPatchMember pCommand(IAPIRequest,grpNum,channel);			
		pCommand.execute();

		FutureResultT future;
		ACE_Future<FutureResultT> aceFuture;
		
		future.set(ADD_PATCH_MEMBER);
        future.m_resourceId = resource.id;
        future.m_resourceType = resource.type;
		future.m_channel = channel; // use this to store channel
		aceFuture.set(future);
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
	}	
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());					
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::releasePatchGroup(RadioResource& resource)

{
	FUNCTION_ENTRY("releasePatchGroup");

	checkApState();

	std::vector<RadioResource> members = 
		The_StatusManager::instance()->getGroupMembers(resource.id);		

	std::vector<RadioResource>::iterator iter = members.begin();

	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, RELEASED);
	// delete each member 
	for (; iter != members.end(); ++iter)
	{
		LogicalChannel channel = 0;
		try
		{
			channel = The_StatusManager::instance()->getChannel((*iter).id);
		}
		catch(const TA_Base_Core::TransactiveException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());					
		}

		 //{TG involved} deselectResource()
        try
        {
//		if ( The_StatusManager::instance()->getFeatureAttributeIsSelected((*iter)) == SELECTED_SPEAKER )
		    {
			    deselectResource(*iter,channel);
		    }
        }
        catch (...)
        {
        }

		RadioMFTRemoteAPI::DeletePatchMember pDelete(IAPIRequest, channel);			
		try
		{
			pDelete.execute();
			
			FutureResultT pFuture;
			pFuture.set(DELETE_PATCH_MEMBER);
			
            pFuture.m_channel = channel; // use this field to store channel

			ACE_Future<FutureResultT> pAceFuture;
			pAceFuture.set(pFuture);
			
			m_pFutureManager.attachFuture(pAceFuture);
		}
		catch(TA_IRS_App::ConsoleDispatchException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());		
		}

		/* Note: Deassign resource after member is deleted
		if (channel != 0)
		{
            if ((The_StatusManager::instance()->canDeassignResource(*iter, &resource)) &&
				(iter->type != RR_PRIVATECALL) && (iter->type != RR_SPI) && (iter->type != RR_BIM))
            {
    			deassignResource(*iter, channel);
            }
		}*/
	}
	The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
	
    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::connectMselGroup(
	RadioResource& resource,
	CommandState state/* =START */,
	unsigned int grpNo/* =0 */)
{
    FUNCTION_ENTRY("connectMselGroup");

	checkApState();	

	switch(state)
	{
		case START:
		{
            if (The_StatusManager::instance()->hasPendingConnectResource(RR_MSELECT))
            {
                throw TA_Base_Core::TransactiveException("Another msel group is being connected");
            }

			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);		
            if (members.size() < 1)
            {
                throw TA_Base_Core::TransactiveException("Cannot connect with no members in the group");
            }

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling API GetFreeMselGroup"); 
			RadioMFTRemoteAPI::GetFreeMselGroup pFunction(IAPIRequest);

	        FutureResultT future;
	        ACE_Future<FutureResultT> aceFuture;

			pFunction.execute();

			future.set(GET_FREE_MSEL);
			future.m_param = resource.id;
			future.m_resourceId = resource.id;
			future.m_resourceType = resource.type;
			
			aceFuture.set(future);

			m_pFutureManager.attachFuture(aceFuture);

            // limit the msel group connection to one at a time
            The_StatusManager::instance()->setPendingConnectResource(resource);

            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTION_BUSY);
		}
		break;

		case OPEN:
			try
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Calling OpenMultiSelect API with group no = %u",grpNo);
				RadioMFTRemoteAPI::OpenMSel pFunction(IAPIRequest,grpNo);
				pFunction.execute();

				FutureResultT future;
				ACE_Future<FutureResultT> aceFuture;

				future.set(OPEN_MSEL);
				future.m_param = grpNo;
				future.m_resourceId = resource.id;
				future.m_resourceType = resource.type;

				aceFuture.set(future);
				m_pFutureManager.attachFuture(aceFuture); 
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
			}
			break;


		case ACTION:
		{
			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);		

			std::vector<RadioResource>::iterator iter = members.begin();

			// assign first to ensure that a channel is associated with this member resource
			for (; iter != members.end(); ++iter)
			{
				if ( !The_StatusManager::instance()->getFeatureAttributeIsAssigned(*iter) )
				{
					try
					{
						unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "calling AssignResource for resource ID = %u",iter->id);
						RadioMFTRemoteAPI::AssignResource pFunction(IAPIRequest, iter->id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL, MCA_NULL_CRITICAL_RESOURCE, NULL_HEADSET_SELECT);

						pFunction.execute();

	                    FutureResultT future;
	                    ACE_Future<FutureResultT> aceFuture;

						future.set(ASSIGN_RESOURCE);
						future.m_param = grpNo;
                        future.m_resourceId = iter->id;
                        future.m_resourceType = iter->type;
						future.m_state = ADD_MSEL_MEMBER;

						aceFuture.set(future);
						m_pFutureManager.attachFuture(aceFuture); 
					}
					catch(TA_IRS_App::ConsoleDispatchException& e)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
					}
				}
				else
				{
					addMselMember(*iter,grpNo);			
				}
			}
		}
		break;

		case END:
			addMselMember(resource,grpNo);			
            break;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::addMselMember(RadioResource& resource, unsigned int grpNum)
{
    FUNCTION_ENTRY("addMselMember");

	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling API addMselMember, channel = %u, grpNum, %u",channel, grpNum);
		RadioMFTRemoteAPI::AddMselMember pCommand(IAPIRequest, grpNum, channel);			

		pCommand.execute();
		
		FutureResultT mFuture;
		mFuture.set(ADD_MSEL_MEMBER);
        mFuture.m_resourceId = resource.id;
        mFuture.m_resourceType = resource.type;
        mFuture.m_channel = channel;
		
		ACE_Future<FutureResultT> mAceFuture;
		mAceFuture.set(mFuture);
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
			"Failed to execute AddMultiselectMember on %s",resource.alias.c_str());
	}	
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());					
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::releaseMselGroup(RadioResource& resource,bool bSolicited)
{
	FUNCTION_ENTRY("releaseMselGroup");

	checkApState();

	int mselGrp = The_StatusManager::instance()->getGroupNumber(resource.id);

	if (mselGrp==0)
	{
		TA_THROW(TA_Base_Core::TransactiveException("MselGroup number is 0"));
	}

			// delete each member 
			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);		

			std::vector<RadioResource>::iterator iter = members.begin();

			for (; iter != members.end(); ++iter)
			{
				LogicalChannel channel = 0;
				try
				{
					channel = The_StatusManager::instance()->getChannel((*iter).id);

					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Calling API DeleteMselMember");
					RadioMFTRemoteAPI::DeleteMselMember pCommand(IAPIRequest, mselGrp,channel);	

					pCommand.execute();

					FutureResultT mFuture;
					mFuture.set(DELETE_MSEL_MEMBER);
					mFuture.m_resourceId = (*iter).id;
					mFuture.m_resourceType = (*iter).type;
					mFuture.m_channel = channel;

					ACE_Future<FutureResultT> mAceFuture;
					mAceFuture.set(mFuture);
				}
				catch(TA_IRS_App::ConsoleDispatchException& e)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
						"Failed to execute DeleteMultiselectMember on %s",(*iter).alias.c_str());
				}
				catch(const TA_Base_Core::TransactiveException& e)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());					
				}

				// deassign if not involved in any other activity
				if (The_StatusManager::instance()->canDeassignResource(*iter, &resource))
				{
					if ( (!The_StatusManager::instance()->getFeatureAttributeIsActive(*iter)) &&
						(The_StatusManager::instance()->getFeatureAttributeIsSelected(*iter)!=SELECTED_SPEAKER))
					{
						deassignResource(*iter,channel);
					}
				}
			}
			
			try
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug, "Calling API CloseMselMember");
				RadioMFTRemoteAPI::CloseMselMember pFunction(IAPIRequest,mselGrp);
				pFunction.execute();

				FutureResultT future;
				ACE_Future<FutureResultT> aceFuture;

				future.set(CLOSE_MSEL);
				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				std::ostringstream logErr;
				logErr << "Failed to execute CloseMultiselect on "<< resource.alias <<std::endl;

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
			}		
			The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, RELEASED);
			The_StatusManager::instance()->clearPendingConnectResource(RR_PATCHGROUP);
	FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::makePrivateCall(RadioResource& resource,CommandState state/* =START */)
{
    FUNCTION_ENTRY("makePrivateCall");

	checkApState();

	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);//33554433/*Try to put the PVC ID*/);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"PVC Call : SSI=%u ID=%u",resource.ssi,resource.id);
		switch(state)
		{
			case START:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling Select resource API, channel = %u, SSI = %u", channel, resource.ssi);
				RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest,channel);
				pFunction.execute();

		        FutureResultT future;
		        ACE_Future<FutureResultT> aceFuture;
				
				future.set(SELECT_RESOURCE);
				future.m_state = BEGIN_PRIVATE_CALL;
				future.m_param = resource.ssi;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;

				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
            break;

			case ACTION:
			{
				unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling BeginPrivateCall API with channel = %u, SSI = %u", channel, resource.ssi);
				RadioMFTRemoteAPI::BeginPrivateCall pFunction(IAPIRequest, channel, resource.ssi, clientID);
				pFunction.execute();				

				FutureResultT future;
				future.set(BEGIN_PRIVATE_CALL);
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
            break;
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute makePrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute makePrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::answerPrivateCall(RadioResource& resource,CommandState state/* =START */)
{
    FUNCTION_ENTRY("answerPrivateCall");

	checkApState();


	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		switch(state)
		{
			case START:
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling SelectResource API (answerPrivateCall) with private resource ID = %u and channel = %u", resource.ssi, channel);
				RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest, channel);
				pFunction.execute();
				
	            FutureResultT future;
	            ACE_Future<FutureResultT> aceFuture;

				future.set(SELECT_RESOURCE);
				future.m_state = ANSWER_PRIVATE_CALL;
				future.m_param = resource.ssi;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;

				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
            break;

			case ACTION:
			{
				unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling answerPrivateCall API with resource SSI = %u, channel = %u", resource.ssi, channel);
				RadioMFTRemoteAPI::AnswerPrivateCall pFunction(IAPIRequest, channel, resource.ssi,clientID);
				pFunction.execute();

				FutureResultT future;
				future.set(ANSWER_PRIVATE_CALL);
				future.m_state = BEGIN_GENERAL_XMIT;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
            break;
		}

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute AnswerPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute AnswerPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::endPrivateCall(RadioResource& resource)
{
    FUNCTION_ENTRY("endPrivateCall");

	checkApState();

	try
	{
		// End private call then deselect PrivateCall channel
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling EndPrivateCall API with resource SSI = %u, channel = %u", resource.ssi, channel);
		RadioMFTRemoteAPI::EndPrivateCall pFunction(IAPIRequest,channel);
		pFunction.execute();

		FutureResultT future;
		future.set(END_PRIVATE_CALL);
		
		ACE_Future<FutureResultT> aceFuture;
		aceFuture.set(future);

		m_pFutureManager.attachFuture(aceFuture);

		deselectResource(pc,channel);
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::makeEmergencyCall(RadioResource& resource)
{
    FUNCTION_ENTRY("makeEmergencyCall");

	checkApState();

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	try
	{
        LogicalChannel channel;
        if (resource.type == RR_RADIO)
        {
		    RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		    channel = The_StatusManager::instance()->getChannel(pc.id);

        }
        else if (resource.type == RR_TALKGROUP)
        {
		    channel = The_StatusManager::instance()->getChannel(resource.id);
        }
        else
        {
            return;
        }

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling BeginConsoleEmergencyCall API with channel = %u", channel);
		RadioMFTRemoteAPI::BeginConsoleEmergencyCall pFunction(IAPIRequest,channel, clientID);
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginConsoleEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginConsoleEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

	FutureResultT future;
	future.set(BEGIN_EMERGENCY_CALL);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::endEmergencyCall(RadioResource& resource)
{
    FUNCTION_ENTRY("endEmergencyCall");

	checkApState();

	try
	{
		LogicalChannel channel;
        if (resource.type == RR_RADIO)
        {
		    RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		    channel = The_StatusManager::instance()->getChannel(pc.id);
        }
        else if ((resource.type == RR_TALKGROUP) || (resource.type == RR_ANNGROUP)) // TD18141
        {
		    channel = The_StatusManager::instance()->getChannel(resource.id);
        }
        else
        {
            return;
        }

//		TA_IRS_App::Mca::EndEmergency pFunction(channel,resource.ssi);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling EndEmergency API");
		RadioMFTRemoteAPI::EndEmergency pFunction(IAPIRequest,channel);
		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	
	FutureResultT future;
	future.set(END_EMERGENCY_CALL);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

    FUNCTION_EXIT;	
}

void RadioMFTAgentRemoteAPICommandImpl::ackEmergency(RadioResource& resource)
{
    FUNCTION_ENTRY("ackEmergency");

	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);	

		unsigned int radioUnit = The_StatusManager::instance()->getFeatureAttributeEmergencyRadioUnit(resource);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Calling AcknowledgeEmergency API");
		RadioMFTRemoteAPI::AcknowledgeEmergency pFunction(IAPIRequest,channel,radioUnit);
		pFunction.execute();

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ackEmergency on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ackEmergency on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::makeAmbienceListeningCall(RadioResource& resource)
{
    FUNCTION_ENTRY("makeAmbienceListeningCall");

	checkApState();

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		RadioMFTRemoteAPI::BeginAmbienceListeningCall pFunction(IAPIRequest,channel,clientID,resource.id);
		pFunction.execute();

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	
	FutureResultT future;
	future.set(BEGIN_AMBIENCE_LISTENING);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

	FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::endAmbienceListeningCall(RadioResource& resource)
{
    FUNCTION_ENTRY("endAmbienceListeningCall");

	checkApState();
	
	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		RadioMFTRemoteAPI::EndAmbienceListeningCall pFunction(IAPIRequest, channel);

		pFunction.execute();
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	
	FutureResultT future;
	future.set(END_AMBIENCE_LISTENING);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager.attachFuture(aceFuture);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::connectSite(RadioResource& resource,CommandState state/* =START */)
{
    FUNCTION_ENTRY("connectSite");

	if (state==START)
	{
		checkApState();

		if (!The_StatusManager::instance()->getFeatureAttributeIsAssigned(resource))
		{
            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTION_BUSY);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[connectSite] %s", resource.alias.c_str()); 
            assignResource(resource);
        }
        else
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"[connectSite] CONNECTED %s", resource.alias.c_str());
            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTED);
        }
        
		if (!The_StatusManager::instance()->getFeatureAttributeIsAssigned(resource))
		{
			unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

			RadioMFTRemoteAPI::AssignResource pFunction(IAPIRequest,
				resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL,MCA_NULL_CRITICAL_RESOURCE, NULL_HEADSET_SELECT);

			pFunction.execute();

			FutureResultT future;
			future.set(ASSIGN_RESOURCE);
			future.m_param = resource.id;
            future.m_resourceId = resource.id;
            future.m_resourceType = resource.type;

			future.m_state = SELECT_RESOURCE;

			ACE_Future<FutureResultT> aceFuture;
			aceFuture.set(future);
			m_pFutureManager.attachFuture(aceFuture);
		}
        else // already assigned, just select
        {
    		selectResource(resource);		
        }
	}
	else if (state==ACTION)
	{
		selectResource(resource);		
	}
		
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::releaseSite(RadioResource& resource)
{
    //// Triggered via release of HW PTT
    //[ handled elsewhere ] <<
    //McaGeneralTransmitStatus

    FUNCTION_ENTRY("releaseSite");

	checkApState();
	
	deselectResource(resource);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::connectMselSite(
	RadioResource& resource,
	CommandState state/* =START */,
	unsigned int grpNo/* =0 */)
{
    //as for Msel
    FUNCTION_ENTRY("connectMselSite");

	connectMselGroup(resource,state,grpNo);

    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::releaseMselSite(RadioResource& resource)
{
    //as for Msel
    FUNCTION_ENTRY("releaseMselSite");

	releaseMselGroup(resource,true);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::connectDynRegroup(RadioResource& resource)
{
	FUNCTION_ENTRY("connectDynRegroup");

	if (The_StatusManager::instance()->getNumberOfActiveDG() > CONNECTION_LIMIT) // Fixed TD14320
	{
		TA_THROW(TA_Base_Core::TransactiveException("Number of active DG connection already reached limit."));
	}


	//RadioResource 	dynGroup = The_StatusManager::instance()->getFreeDynamicGroup(resource);
	//TD18526 marc Check connecting profile, if super, prevent it from connecting 
	//TD18803
	//	if (dynGroup.ssi = 0xFFFFFFFF)
	//if (The_StatusManager::instance()->bPermitProfileAcquireDG() == false)
	//{
	//	TA_THROW(TA_Base_Core::TransactiveException("Profile not allowed to make a Dynamic regroup connection."));
	//}
	//TD18803 marc Check connecting profile, if super, prevent it from connecting 

	//if (dynGroup.type != RR_NIL)
	//{
	//const char* talkgroupAlias = dynGroup.alias.c_str();
	//unsigned long talkgroupId = dynGroup.id;
	const char* talkgroupAlias = resource.alias.c_str();

	bool status;
	CORBA::Boolean statusCorba;
	CORBA_CALL_RETURN(status,m_serverAgent,_non_existent,());
	if( status )
	{
		TA_THROW(TA_Base_Core::TransactiveException("RadioServerAgent object reference is non-existent."));
	}

	std::vector<RadioResource> members = The_StatusManager::instance()->getResourceMembers(resource);
	{
		std::vector<RadioResource>::iterator iter = members.begin();
		for ( ; iter != members.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) break;
		}
		if (iter != members.end()) {
			members.erase(iter);
		}
	}
	for (std::vector<RadioResource>::iterator it = members.begin(); it != members.end(); ++it)
	{
		TA_IRS_Bus::Radio::RadioResource nResource;
		RadioMFTAgentHelper::convertRadioResourceToIDL(*it, nResource);

		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Calling RadioAgent for DGNA with alias = %s", talkgroupAlias);
			CORBA_CALL_RETURN(statusCorba, m_serverAgent,connectDynamicRegroup,(nResource, talkgroupAlias));
		}
		catch(...)
		{
			//marc_bugfix if corba call was unsuccessful, then release the dynamic group and show error message
			//The_StatusManager::instance()->releaseDynamicGroup(resource, talkgroupId);
			TA_THROW(TA_Base_Core::TransactiveException("Error connecting to the radio server agent. Please retry the operation. "));
		}
	} 
	The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,CONNECTED);

	//assignResource(dynGroup);
	//   }
	//else
	//{
	//	TA_THROW(TA_Base_Core::TransactiveException("Unable to get a free DG Talkgroup"));
	//}

	FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::releaseDynRegroup(RadioResource& resource)
{
    FUNCTION_ENTRY("releaseDynRegroup");

	//RadioResource dynGroup = The_StatusManager::instance()->getDynamicGroupForResource(resource);
	//const char* talkgroupAlias = dynGroup.alias.c_str();
	//unsigned long talkgroupID = dynGroup.id;
	const char* talkgroupAlias = resource.alias.c_str();
	try
	{
		bool status;
		CORBA::Boolean statusCorba;
		CORBA_CALL_RETURN(status,m_serverAgent,_non_existent,());
	    if( status )
	    {
		    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			    "RadioServerAgent object reference is non-existent." );
		    return;
	    }


        std::vector<RadioResource> members = The_StatusManager::instance()->getResourceMembers(resource);
		{
			std::vector<RadioResource>::iterator iter = members.begin();
			for ( ; iter != members.end(); ++iter)
			{
				if (RR_TALKGROUP == (*iter).type) break;
			}
			if (iter != members.end()) {
				members.erase(iter);
			}
		}	
        for (std::vector<RadioResource>::iterator it = members.begin(); it != members.end(); ++it)
        {
            TA_IRS_Bus::Radio::RadioResource nResource;
            RadioMFTAgentHelper::convertRadioResourceToIDL(*it, nResource);

			try
			{
				CORBA_CALL_RETURN(statusCorba, m_serverAgent,releaseDynamicRegroup,(nResource, talkgroupAlias));
			}
			catch(...)
			{
				//marc_bugfix if corba call was unsuccessful
				TA_THROW(TA_Base_Core::TransactiveException("Error connecting to the radio server agent. Please retry the operation. "));
			}
        }
		The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource,RELEASED);
        //deassignResource(dynGroup);
	}
	catch(const CORBA::Exception& e)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,  SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
               TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e ).c_str() );

	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
			"Exception occurred in releaseDynRegroup().");
	}
        
	//The_StatusManager::instance()->releaseDynamicGroup(resource, talkgroupID);

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::generalTransmit(CommandState state/* =START */)
{
    FUNCTION_ENTRY("generalTransmit");
    
	try
	{
		switch(state)
		{
			case START:
			{
				RadioMFTRemoteAPI::BeginGeneralTransmit pFunction(IAPIRequest, TA_IRS_App::TheConsoleStateManager::instance()->getClientID());
				pFunction.execute();

				FutureResultT future;
				future.set(BEGIN_GENERAL_XMIT);
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager.attachFuture(aceFuture);
			}
            break;

			case END:
			{
				RadioMFTRemoteAPI::EndGeneralTransmit pFunction(IAPIRequest,TA_IRS_App::TheConsoleStateManager::instance()->getClientID());
				pFunction.execute();
			}
            break;
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		throw e;
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Failed to generalTransmit"));
	}


	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::selectAG(RadioResource& resource,CommandState state/* =START */)
{
    FUNCTION_ENTRY("selectAG");

	if (state==START)
	{
		checkApState();
		if (!The_StatusManager::instance()->getFeatureAttributeIsAssigned(resource))
		{
			try
			{
				unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

				RadioMFTRemoteAPI::AssignResource pFunction(IAPIRequest,
					resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL, MCA_NULL_CRITICAL_RESOURCE, NULL_HEADSET_SELECT);

				pFunction.execute();

				FutureResultT future;			
				future.set(ASSIGN_RESOURCE);
				future.m_param = resource.id;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;
				future.m_state = SELECT_RESOURCE;

				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);
				m_pFutureManager.attachFuture(aceFuture); 
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
			}
		}
		else
		{
			state = ACTION;
		}
	}
	if (state==ACTION)
	{
		try
		{
			LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);
			RadioMFTRemoteAPI::SelectResource pFunction(IAPIRequest,channel);
			pFunction.execute();
			
			FutureResultT future;
			future.set(SELECT_RESOURCE);
            future.m_resourceId = resource.id;
            future.m_resourceType = resource.type;

			ACE_Future<FutureResultT> aceFuture;
			aceFuture.set(future);

			m_pFutureManager.attachFuture(aceFuture);		
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Failed to execute selectAG()");
		}	
	}
	
    FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::callback_initialisation()
{
    FUNCTION_ENTRY("callback_initialisation");

        m_pEventDispatcher.start();


	try
	{
		/*TA_IRS_App::Mcdi::RegisterClient pFunction;
		pFunction.execute();*/ //No need to register cause the RemoteAPIConnection takes care of this.
        
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to register client");
		throw e;
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Failed to register client"));
	}

    FUNCTION_EXIT;
}

bool RadioMFTAgentRemoteAPICommandImpl::registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback)
{
    	return m_pEventDispatcher.addCallback(pCallback);
    return false;
}


void RadioMFTAgentRemoteAPICommandImpl::callback_termination()
{
    FUNCTION_ENTRY("callback_termination");

	try
	{
		TA_IRS_App::Mcdi::DeregisterClient pFunction;
		pFunction.execute();
//        ACE_OS::sleep(1); // give time to execute
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to deregister client");
	}
	catch(...)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Failed to deregister client"));
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::assignSelectSpeaker(RadioResource& resource)
{
    FUNCTION_ENTRY("assignSelectSpeaker");

	/**
	 * ?? Was this intended for moving/readying group resources for transmission??
	 * Possibly needed, eg, PG requires a member TG to be SingleSelected, such that hw
	 * ptt transmits to entire patch.
	 * 
	 */

    FUNCTION_EXIT;
}




void RadioMFTAgentRemoteAPICommandImpl::checkApState()
{

	if ( m_pApStateMgr->getApState() == RA_INITIALISATION_MODE )
	{
		std::string logStr = "Cannot execute command in initialization.";
		TA_THROW(TA_Base_Core::TransactiveException(logStr.c_str()));
	}

}

void RadioMFTAgentRemoteAPICommandImpl::createPatchGroup(RadioResource& resource, bool bIsDLTSPatch/* =true */)
{
    FUNCTION_ENTRY("createPatchGroup");
    
	std::vector<RadioResource> patchGroup = The_StatusManager::instance()->getMFTResourceCollection(RR_PATCHGROUP);


	int count = 0;

	for (int i=0;i<patchGroup.size();i++)
	{
		if ((The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(patchGroup[i]) == CONNECTED) ||
			(The_StatusManager::instance()->getFeatureAttributeIsGrpConnected(patchGroup[i]) == CONNECTION_BUSY)) 
		{
			count++;
		}
	}

	if (count >= CONNECTION_LIMIT)
	{
		TA_THROW(TA_Base_Core::TransactiveException("Number of active patch groups exceed API limit"));
	}

	RadioResource pResource;
	pResource.id    = The_StatusManager::instance()->getNextTransResourceId();
	pResource.type  = RR_PATCHGROUP;
	if (bIsDLTSPatch)
	{
		pResource.alias = "(BIM)Patch Group";
	}
	else
	{
		pResource.alias = "(SPI)Patch Group";
	}
	The_StatusManager::instance()->setTempPatchGroup(pResource);
	
	// add members
	std::vector<RadioResource> members;

	if (bIsDLTSPatch)
	{
		members.push_back(The_StatusManager::instance()->getBIMResource());
	}
	else
	{
		members.push_back(The_StatusManager::instance()->getPhoneResource());
	}
	
    if (resource.type == RR_TALKGROUP)
    {
    	members.push_back(resource);
    }
	else if (resource.type == RR_DYNGROUP)
	{
		//get the DG resource
		RadioResource dynGroup = The_StatusManager::instance()->getDynamicGroupForResource(resource);
		members.push_back(dynGroup);
	}
    else
    {
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
	    members.push_back(pc);
    }

	The_StatusManager::instance()->addResource(pResource,members,pResource.id);

	connectPatchGroup(pResource);
	

	FUNCTION_EXIT;
}


void RadioMFTAgentRemoteAPICommandImpl::cancelPrivateCall(RadioResource &resource, PrivateCallState ptCallState)
{
    FUNCTION_ENTRY("cancelPrivateCall");

	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);
// ++ Huangxin TD14319 
		if(ptCallState == PC_INCOMING)
		{
			unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();
			RadioMFTRemoteAPI::AnswerPrivateCall pAnswerFunction(IAPIRequest, channel, resource.ssi, clientID);
			pAnswerFunction.execute();
		}

//		else if(ptCallState == PC_OUTGOING)
//		{
//			
//		}

		//ending a private call without starting transmition will actually cancel the call.
		RadioMFTRemoteAPI::EndPrivateCall pEndFunction(IAPIRequest, channel);
		pEndFunction.execute();

// TD14319 ++
	}

	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute cancelPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute cancelPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
}


void RadioMFTAgentRemoteAPICommandImpl::beginTone(unsigned int tone)
{
	FUNCTION_ENTRY("beginTone");
	try
	{
		unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();
		RadioMFTRemoteAPI::BeginTone pBeginTone(IAPIRequest, tone, clientID);
		pBeginTone.execute();
	}

	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute beginTone"<< std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute beginTone"<< std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	FUNCTION_EXIT;
}

void RadioMFTAgentRemoteAPICommandImpl::endTone()
{
	FUNCTION_ENTRY("endTone");
	try
	{
		RadioMFTRemoteAPI::EndTone pEndTone(IAPIRequest);
		pEndTone.execute();
	}

	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endTone"<< std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endTone"<< std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	FUNCTION_EXIT;
}
