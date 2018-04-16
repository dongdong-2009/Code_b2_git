/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/AuditMessageUtility.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "ecsmanager.h"
#include "AuditMessageUtility.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_IRS_App
{
	CAuditMessageUtility::CAuditMessageUtility()
	{
		m_auditMsgSender = NULL;
		// wen ching++
		// bug 30 (TD12345)
		m_ecsManager = NULL;
		// ++wen ching
		// bug 30 (TD12345)
	}

	CAuditMessageUtility::~CAuditMessageUtility()
	{
	}

	void CAuditMessageUtility::LogParameterChangeAuditMessage(std::string &aParameterName, 
																std::string &aParameterNewValue, 
																const MessageType &aMessageType)
	{
	
		if (m_auditMsgSender == NULL)
			m_auditMsgSender = MessagePublicationManager::getInstance().getAuditMessageSender(ECSAudit::Context);	
		
		if (m_auditMsgSender == NULL)
			return;

		// wen ching++
		// bug 30 (TD12345)
		if (m_ecsManager == NULL)
			m_ecsManager = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());

		if (m_ecsManager == NULL)
			return;

		TA_Base_Core::IResource* resource = 0;
		try
		{				
			std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

			resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_ecsManager->getKey());
			unsigned long resourceId = resource->getKey();
			delete resource;

			DescriptionParameters descriptionParameters;
//			CL17278++ Sheet4 - Event value field will show the first parametervalue			
// 			NameValuePair pair1("ParameterName", aParameterName);
// 			NameValuePair pair2("ParameterNewValue", aParameterNewValue);
			NameValuePair pair1("ParameterName", aParameterNewValue);
			NameValuePair pair2("ParameterNewValue", aParameterName);
//			++CL17278

			descriptionParameters.push_back(&pair1);
			descriptionParameters.push_back(&pair2);

			m_auditMsgSender->sendAuditMessage(							
					aMessageType,				// message type
					resourceId,					// entity key of ECS
					descriptionParameters,		// description parameters
					"",							// additional details
					sessionId,					// sessionID
					"",                         // AlarmID if an alarm associated with event
					"",                         // IncidentKey if incident associated with event
					"");                        // EventID of a parent event, used to link events

		} 	
		catch(...)
		{
			delete resource;
		}

		// ++wen ching
		// bug 30 (TD12345)

	}

	void CAuditMessageUtility::LogTimetableChangeAuditMessage(std::string &aStationName, 
																std::string &aTimetableNewValue, 
																const MessageType &aMessageType)
	{
	
		if (m_auditMsgSender == NULL)
			m_auditMsgSender = MessagePublicationManager::getInstance().getAuditMessageSender(ECSAudit::Context);	
		
		if (m_auditMsgSender == NULL)
			return;

		// wen ching++
		// bug 30 (TD12345)
		if (m_ecsManager == NULL)
			m_ecsManager = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());

		if (m_ecsManager == NULL)
			return;

		TA_Base_Core::IResource* resource = 0;
		try
		{				
			std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

			resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_ecsManager->getKey());
			unsigned long resourceId = resource->getKey();
			delete resource;

			DescriptionParameters descriptionParameters;
			NameValuePair pair1("TimetableName", aTimetableNewValue);
			NameValuePair pair2("StationName", aStationName);			

			descriptionParameters.push_back(&pair1);
			descriptionParameters.push_back(&pair2);

			m_auditMsgSender->sendAuditMessage(							
					aMessageType,				// message type
					resourceId,					// entity key of ECS
					descriptionParameters,		// description parameters
					"",							// additional details
					sessionId,					// sessionID
					"",                         // AlarmID if an alarm associated with event
					"",                         // IncidentKey if incident associated with event
					"");                        // EventID of a parent event, used to link events

		} 	
		catch(...)
		{
			delete resource;
		}

		// ++wen ching
		// bug 30 (TD12345)

	}

} // namespace
