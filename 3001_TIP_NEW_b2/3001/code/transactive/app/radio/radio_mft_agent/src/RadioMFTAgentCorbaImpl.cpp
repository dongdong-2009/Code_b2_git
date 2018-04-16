const int RadioMFTAgentCorbaImpl::CONNECTION_LIMIT = 3;




void RadioMFTAgentCorbaImpl::assignResource(RadioResource& resource)
{
    FUNCTION_ENTRY("assignResource");

	checkApState();

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	TA_IRS_App::Mca::AssignResource pFunction(
		resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL);
	 
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization of TA_IRS_App::Mca::AssignResource");

	try
	{
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::AssignResource");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute AssignResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::AssignResource");

		throw e;
	}

	FutureResultT future;
	future.set(ASSIGN_RESOURCE);
    future.m_resourceType = resource.type;
	future.m_resourceId = resource.id;

	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::deassignResource(RadioResource& resource,LogicalChannel channel /* =0 */)
{
    FUNCTION_ENTRY("deassignResource");

	checkApState();

	if ( channel == 0)
	{
		channel = The_StatusManager::instance()->getChannel(resource.id);
	}

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	TA_IRS_App::Mca::DeassignResource pFunction(channel,clientID, MCA_DEASSIGN_NORMAL);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::DeassignResource");

	try
	{
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::DeassignResource");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute DeassignResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::DeassignResource");

		throw e;
	}

	FutureResultT future;
	future.set(DEASSIGN_RESOURCE);
    future.m_resourceType = resource.type;
	future.m_resourceId = resource.id;
    future.m_channel = channel;

	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::selectResource(RadioResource& resource)
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

				TA_IRS_App::Mca::SelectResource pFunction(channel);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");


				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");
				
				future.set(SELECT_RESOURCE);
                future.m_resourceType = resource.type;
                future.m_resourceId = resource.id;

				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
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

				    TA_IRS_App::Mca::SelectResource pFunction(channel);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");
				    pFunction.execute();
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");
				    
				    future.set(SELECT_RESOURCE);
                    future.m_resourceType = dynGroup.type;
                    future.m_resourceId = dynGroup.id;

				    aceFuture.set(future);

				    m_pFutureManager->attachFuture(aceFuture);

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

				TA_IRS_App::Mca::SelectResource pFunction(channel);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");

				future.set(SELECT_RESOURCE);
                future.m_resourceType = (*iter).type;
                future.m_resourceId = (*iter).id;

				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);

				The_StatusManager::instance()->setFeatureAttributeIsPatchInvolved(*iter,resource.id); // TD14133
			}
			break;
			
			case RR_MSELECT:
			case RR_MSITE:
			{	int grpNum = The_StatusManager::instance()->getGroupNumber(resource.id);

				if ( grpNum != 0)
				{
					TA_IRS_App::Mca::OpenMultiselect pFunction(grpNum);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::OpenMultiselect");
					pFunction.execute();
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::OpenMultiselect");
					
					future.set(OPEN_MSEL);
					aceFuture.set(future);
					m_pFutureManager->attachFuture(aceFuture);
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


void RadioMFTAgentCommandImpl::deselectResource(RadioResource& resource,LogicalChannel channel /* =0 */)
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

	TA_IRS_App::Mca::DeselectResource pFunction(channel);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::DeselectResource");

	try
	{
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::DeselectResource");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute DeselectResource on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::DeselectResource");

		throw e;
	}

	FutureResultT future;
	future.set(DESELECT_RESOURCE);
    future.m_resourceType = resource.type;
    future.m_resourceId = resourceId;
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);
	
    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::changeVolume(RadioResource& resource, unsigned int volume)
{
    FUNCTION_ENTRY("changeVolume");

	checkApState();

	LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

	TA_IRS_App::Mca::ChangeVolume pFunction(channel, volume);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::ChangeVolume");

	try
	{
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::ChangeVolume");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ChangeVolume on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::ChangeVolume");
		throw e;
	}

	FutureResultT future;
	future.set(CHANGE_VOLUME);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::connectPatchGroup(
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
            if (The_StatusManager::instance()->hasPendingConnectResource(RR_PATCHGROUP))
            {
                throw TA_Base_Core::TransactiveException("Another patch group is being connected");
            }

			std::vector<RadioResource> members = 
				The_StatusManager::instance()->getGroupMembers(resource.id);

            if (members.size() < 2)
            {
                throw TA_Base_Core::TransactiveException("Cannot connect less than 2 members in the group");
            }

			try
			{
	            FutureResultT future;
	            ACE_Future<FutureResultT> aceFuture;

				TA_IRS_App::Mca::GetFreePatchGroup pFunction;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::GetFreePatchGroup");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::GetFreePatchGroup");
				
				future.set(GET_FREE_PATCHGRP);
				future.m_param = resource.id;

                future.m_resourceType = resource.type;
                future.m_resourceId = resource.id;

				aceFuture.set(future);
				m_pFutureManager->attachFuture(aceFuture); 

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

						TA_IRS_App::Mca::AssignResource pFunction(
							iter->id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AssignResource");

						pFunction.execute();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AssignResource");

						future.set(ASSIGN_RESOURCE);
						future.m_param = grpNo;
                        future.m_resourceType = iter->type;
                        future.m_resourceId = iter->id;
						future.m_state = ADD_PATCH_MEMBER;

						aceFuture.set(future);
						m_pFutureManager->attachFuture(aceFuture); 
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

void RadioMFTAgentCommandImpl::addPatchMember(RadioResource& resource, unsigned int grpNum)
{
    FUNCTION_ENTRY("addPatchMember");
    
	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

		TA_IRS_App::Mca::AddPatchMember pCommand(channel,grpNum);			
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AddPatchMember");
		pCommand.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AddPatchMember");

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

void RadioMFTAgentCommandImpl::releasePatchGroup(RadioResource& resource)

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

		TA_IRS_App::Mca::DeletePatchMember pDelete(channel);		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::DeletePatchMember");
		try
		{
			pDelete.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::DeletePatchMember");
			
			FutureResultT pFuture;
			pFuture.set(DELETE_PATCH_MEMBER);
			
            pFuture.m_channel = channel; // use this field to store channel

			ACE_Future<FutureResultT> pAceFuture;
			pAceFuture.set(pFuture);
			
			m_pFutureManager->attachFuture(pAceFuture);
		}
		catch(TA_IRS_App::ConsoleDispatchException& e)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::DeletePatchMember");
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
	
    FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::connectMselGroup(
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
            if (members.size() < 2)
            {
                throw TA_Base_Core::TransactiveException("Cannot connect less than 2 members in the group");
            }

			TA_IRS_App::Mca::GetFreeMultiselectGroup pFunction;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::GetFreeMultiselectGroup");

	        FutureResultT future;
	        ACE_Future<FutureResultT> aceFuture;

			pFunction.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::GetFreeMultiselectGroup");

			future.set(GET_FREE_MSEL);
			future.m_param = resource.id;
			future.m_resourceId = resource.id;
			future.m_resourceType = resource.type;
			
			aceFuture.set(future);

			m_pFutureManager->attachFuture(aceFuture);

            // limit the msel group connection to one at a time
            The_StatusManager::instance()->setPendingConnectResource(resource);

            The_StatusManager::instance()->setFeatureAttributeIsGrpConnected(resource, CONNECTION_BUSY);
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

						TA_IRS_App::Mca::AssignResource pFunction(
							iter->id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AssignResource");

						pFunction.execute();
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AssignResource");

	                    FutureResultT future;
	                    ACE_Future<FutureResultT> aceFuture;

						future.set(ASSIGN_RESOURCE);
						future.m_param = grpNo;
                        future.m_resourceId = iter->id;
                        future.m_resourceType = iter->type;
						future.m_state = ADD_MSEL_MEMBER;

						aceFuture.set(future);
						m_pFutureManager->attachFuture(aceFuture); 
					}
					catch(TA_IRS_App::ConsoleDispatchException& e)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::AssignResource");
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

void RadioMFTAgentCommandImpl::addMselMember(RadioResource& resource, unsigned int grpNum)
{
    FUNCTION_ENTRY("addMselMember");

	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);

		TA_IRS_App::Mca::AddMultiselectMember pCommand(channel,grpNum);			
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AddMultiselectMember");

		pCommand.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AddMultiselectMember");
		
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
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::AddMultiselectMember");
	}	
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,e.what());					
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::releaseMselGroup(RadioResource& resource,bool bSolicited)
{
    FUNCTION_ENTRY("releaseMselGroup");

	checkApState();
	
	int mselGrp = The_StatusManager::instance()->getGroupNumber(resource.id);

	if (mselGrp==0)
	{
		TA_THROW(TA_Base_Core::TransactiveException("MselGroup number is 0"));
	}

	try
	{
		TA_IRS_App::Mca::CloseMultiselect pFunction(mselGrp);
		pFunction.execute();
		
	    FutureResultT future;
	    ACE_Future<FutureResultT> aceFuture;

		future.set(CLOSE_MSEL);
		aceFuture.set(future);

		m_pFutureManager->attachFuture(aceFuture);
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute CloseMultiselect on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
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

			TA_IRS_App::Mca::DeleteMultiselectMember pCommand(channel,mselGrp);	
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::DeleteMultiselectMember");

			pCommand.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::DeleteMultiselectMember");
			
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
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::DeleteMultiselectMember");
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

	FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::makePrivateCall(RadioResource& resource,CommandState state/* =START */)
{
    FUNCTION_ENTRY("makePrivateCall");

	checkApState();

	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);


		switch(state)
		{
			case START:
			{
				TA_IRS_App::Mca::SelectResource pFunction(channel);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");

		        FutureResultT future;
		        ACE_Future<FutureResultT> aceFuture;
				
				future.set(SELECT_RESOURCE);
				future.m_state = BEGIN_PRIVATE_CALL;
				future.m_param = resource.ssi;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;

				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
			}
            break;

			case ACTION:
			{
				unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

				TA_IRS_App::Mca::BeginPrivateCall pFunction(channel, clientID, resource.ssi);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::BeginPrivateCall");
				pFunction.execute();				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::BeginPrivateCall");

				FutureResultT future;
				future.set(BEGIN_PRIVATE_CALL);
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
			}
            break;
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute makePrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::makePrivateCall");
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


void RadioMFTAgentCommandImpl::answerPrivateCall(RadioResource& resource,CommandState state/* =START */)
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
				TA_IRS_App::Mca::SelectResource pFunction(channel);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");
				
	            FutureResultT future;
	            ACE_Future<FutureResultT> aceFuture;

				future.set(SELECT_RESOURCE);
				future.m_state = ANSWER_PRIVATE_CALL;
				future.m_param = resource.ssi;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;

				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
			}
            break;

			case ACTION:
			{
				unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

				TA_IRS_App::Mca::AnswerPrivateCall pFunction(channel, clientID, resource.ssi);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AnswerPrivateCall");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AnswerPrivateCall");

				FutureResultT future;
				future.set(ANSWER_PRIVATE_CALL);
				future.m_state = BEGIN_GENERAL_XMIT;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
			}
            break;
		}

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute AnswerPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::AnswerPrivateCall");

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


void RadioMFTAgentCommandImpl::endPrivateCall(RadioResource& resource)
{
    FUNCTION_ENTRY("endPrivateCall");

	checkApState();

	try
	{
		// End private call then deselect PrivateCall channel
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		TA_IRS_App::Mca::EndPrivateCall pFunction(channel);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::EndPrivateCall");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EndPrivateCall");

		FutureResultT future;
		future.set(END_PRIVATE_CALL);
		
		ACE_Future<FutureResultT> aceFuture;
		aceFuture.set(future);

		m_pFutureManager->attachFuture(aceFuture);

		deselectResource(pc,channel);
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::EndPrivateCall");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::EndPrivateCall");

		throw e;
	}
	
    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::makeEmergencyCall(RadioResource& resource)
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

		TA_IRS_App::Mca::BeginConsoleEmergencyCall pFunction(channel, clientID);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::BeginConsoleEmergencyCall");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::BeginConsoleEmergencyCall");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginConsoleEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::BeginConsoleEmergencyCall");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginConsoleEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::BeginConsoleEmergencyCall");

		throw e;
	}

	FutureResultT future;
	future.set(BEGIN_EMERGENCY_CALL);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::endEmergencyCall(RadioResource& resource)
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
		TA_IRS_App::Mca::EndEmergency pFunction(channel,NULL_UNIT_ID);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App:: Mca::EndEmergency");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EndEmergency");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::EndEmergency");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endEmergencyCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::EndEmergency");

		throw e;
	}
	
	FutureResultT future;
	future.set(END_EMERGENCY_CALL);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

    FUNCTION_EXIT;	
}

void RadioMFTAgentCommandImpl::ackEmergency(RadioResource& resource)
{
    FUNCTION_ENTRY("ackEmergency");

	try
	{
		LogicalChannel channel = The_StatusManager::instance()->getChannel(resource.id);	

		unsigned int radioUnit = The_StatusManager::instance()->getFeatureAttributeEmergencyRadioUnit(resource);

		TA_IRS_App::Mca::EmergencyAlarmRecognized pFunction(channel,radioUnit);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::EmergencyAlarmRecognized");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EmergencyAlarmRecognized");

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ackEmergency on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::EmergencyAlarmRecognized");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute ackEmergency on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::EmergencyAlarmRecognized");

		throw e;
	}

    FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::makeAmbienceListeningCall(RadioResource& resource)
{
    FUNCTION_ENTRY("makeAmbienceListeningCall");

	checkApState();

	unsigned char clientID = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		TA_IRS_App::Mca::BeginAmbienceListeningCall pFunction(channel,clientID,resource.id);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::BeginAmbienceListeningCall");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::BeginAmbienceListeningCall");

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::BeginAmbienceListeningCall");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute BeginAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::BeginAmbienceListeningCall");

		throw e;
	}
	
	FutureResultT future;
	future.set(BEGIN_AMBIENCE_LISTENING);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

	FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::endAmbienceListeningCall(RadioResource& resource)
{
    FUNCTION_ENTRY("endAmbienceListeningCall");

	checkApState();
	
	try
	{
		RadioResource pc = The_StatusManager::instance()->getPrivateCallResource();
		LogicalChannel channel = The_StatusManager::instance()->getChannel(pc.id);

		TA_IRS_App::Mca::EndAmbienceListeningCall pFunction(channel);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::EndAmbienceListeningCall");

		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EndAmbienceListeningCall");
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::EndAmbienceListeningCall");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute EndAmbienceListeningCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::EndAmbienceListeningCall");

		throw e;
	}
	
	FutureResultT future;
	future.set(END_AMBIENCE_LISTENING);
	
	ACE_Future<FutureResultT> aceFuture;
	aceFuture.set(future);

	m_pFutureManager->attachFuture(aceFuture);

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::connectSite(RadioResource& resource,CommandState state/* =START */)
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

			TA_IRS_App::Mca::AssignResource pFunction(
				resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AssignResource");

			pFunction.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AssignResource");

			FutureResultT future;
			future.set(ASSIGN_RESOURCE);
			future.m_param = resource.id;
            future.m_resourceId = resource.id;
            future.m_resourceType = resource.type;

			future.m_state = SELECT_RESOURCE;

			ACE_Future<FutureResultT> aceFuture;
			aceFuture.set(future);
			m_pFutureManager->attachFuture(aceFuture);
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

void RadioMFTAgentCommandImpl::generalTransmit(CommandState state/* =START */)
{
    FUNCTION_ENTRY("generalTransmit");
    
	try
	{
		switch(state)
		{
			case START:
			{
				TA_IRS_App::Mca::BeginGeneralTransmit pFunction(TA_IRS_App::TheConsoleStateManager::instance()->getClientID());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: Initialization TA_IRS_App::Mca::BeginGeneralTransmit");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::BeginGeneralTransmit");

				FutureResultT future;
				future.set(BEGIN_GENERAL_XMIT);
				
				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);

				m_pFutureManager->attachFuture(aceFuture);
			}
            break;

			case END:
			{
				TA_IRS_App::Mca::EndGeneralTransmit pFunction;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::EndGeneralTransmit");
				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EndGeneralTransmit");
			}
            break;
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_Mca::EndGeneralTransmit");
		throw e;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_Mca::EndGeneralTransmit");
		TA_THROW(TA_Base_Core::TransactiveException("Failed to generalTransmit"));
	}


	FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::selectAG(RadioResource& resource,CommandState state/* =START */)
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

				TA_IRS_App::Mca::AssignResource pFunction(
					resource.id, NULL_LC_ID, clientID, NULL_SPEAKER_ID, NULL_SPEAKER_ID, NULL_VOLUME_LEVEL);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AssignResource");

				pFunction.execute();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AssignResource");

				FutureResultT future;			
				future.set(ASSIGN_RESOURCE);
				future.m_param = resource.id;
                future.m_resourceId = resource.id;
                future.m_resourceType = resource.type;
				future.m_state = SELECT_RESOURCE;

				ACE_Future<FutureResultT> aceFuture;
				aceFuture.set(future);
				m_pFutureManager->attachFuture(aceFuture); 
			}
			catch(TA_IRS_App::ConsoleDispatchException& e)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::AssignResource");
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
			TA_IRS_App::Mca::SelectResource pFunction(channel);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::SelectResource");
			pFunction.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::SelectResource");
			
			FutureResultT future;
			future.set(SELECT_RESOURCE);
            future.m_resourceId = resource.id;
            future.m_resourceType = resource.type;

			ACE_Future<FutureResultT> aceFuture;
			aceFuture.set(future);

			m_pFutureManager->attachFuture(aceFuture);		
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Failed to execute selectAG()");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: exception TA_IRS_App::Mca::SelectResource");
		}	
	}
	
    FUNCTION_EXIT;
}

void RadioMFTAgentCommandImpl::callback_initialisation()
{
    FUNCTION_ENTRY("callback_initialisation");

    if (m_pEventDispatcher != NULL)
    {
        m_pEventDispatcher->start();
    }

	try
	{
		TA_IRS_App::Mcdi::RegisterClient pFunction;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mcdi::RegisterClient");
		pFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mcdi::RegisterClient");
        
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to register client");
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mcdi::RegisterClient");
		throw e;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mcdi::RegisterClient");
		TA_THROW(TA_Base_Core::TransactiveException("Failed to register client"));
	}

    FUNCTION_EXIT;
}


void RadioMFTAgentCommandImpl::assignSelectSpeaker(RadioResource& resource)
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

void RadioMFTAgentCommandImpl::cancelPrivateCall(RadioResource &resource, PrivateCallState ptCallState)
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
			TA_IRS_App::Mca::AnswerPrivateCall pAnswerFunction(channel, clientID, resource.ssi);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::AnswerPrivateCall");
			pAnswerFunction.execute();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::AnswerPrivateCall");
		}

//		else if(ptCallState == PC_OUTGOING)
//		{
//			
//		}

		//ending a private call without starting transmition will actually cancel the call.
		TA_IRS_App::Mca::EndPrivateCall pEndFunction(channel);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: initialization TA_IRS_App::Mca::EndPrivateCall");
		pEndFunction.execute();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: execute TA_IRS_App::Mca::EndPrivateCall");

// TD14319 ++
	}

	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute cancelPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: ConsoleDispatchException TA_IRS_App::Mca::EndPrivateCall");

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute cancelPrivateCall on "<< resource.alias <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba implementation: TransactiveException TA_IRS_App::Mca::EndPrivateCall");

		throw e;
	}
}

//++Fixed TD14294
void RadioMFTAgentCommandImpl::makePhoneCall(std::string number,CommandState state/* =START */)
{
	FUNCTION_ENTRY("makePhoneCall");

	LogicalChannel channel = 0; 

	FutureResultT future;
	ACE_Future<FutureResultT> aceFuture;	
	
	try
	{
		if (state == START)
		{
			RadioResource phResource = The_StatusManager::instance()->getPhoneResource();

			channel = The_StatusManager::instance()->getChannel(phResource.id);

			future.set(PHONE_SELECT);
			future.m_param = atoi(number.c_str());
			future.m_state = DIAL_PHONE;

			aceFuture.set(future);

			m_pFutureManager->attachFuture(aceFuture);

			TA_IRS_App::Mca::SelectPhone pFunction(channel,TA_IRS_App::TheConsoleStateManager::instance()->getClientID());
			pFunction.execute();
		}
		else if (state==ACTION)
		{
			int clientId = TA_IRS_App::TheConsoleStateManager::instance()->getClientID();

			future.set(DIAL_PHONE);
			future.m_param = atoi(number.c_str());

			aceFuture.set(future);

			m_pFutureManager->attachFuture(aceFuture);

			TA_IRS_App::Mca::DialNumber pFunction(clientId,number.size(),number.c_str());
			pFunction.execute();
		}
	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute makePhoneCall on "<< number<<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute makePhoneCall on "<< number <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

}


void RadioMFTAgentCommandImpl::endPhoneCall(std::string number)
{
	FUNCTION_ENTRY("endPhoneCall");
	
	FutureResultT future;
	ACE_Future<FutureResultT> aceFuture;	
	
	try
	{
		future.set(END_PHONE_CALL);
		future.m_param = atoi(number.c_str());

		aceFuture.set(future);
		m_pFutureManager->attachFuture(aceFuture);

		RadioResource phResource = The_StatusManager::instance()->getPhoneResource();

		LogicalChannel channel = The_StatusManager::instance()->getChannel(phResource.id);

		TA_IRS_App::Mca::PhoneOnHook pFunction(channel);
		pFunction.execute();

	}
	catch(TA_IRS_App::ConsoleDispatchException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endPhoneCall on "<< number <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		std::ostringstream logErr;
		logErr << "Failed to execute endPhoneCall on "<< number <<std::endl;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,logErr.str().c_str());

		throw e;
	}

	FUNCTION_EXIT;

}
