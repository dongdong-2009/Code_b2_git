/**
  * The source code this file is the property of
  * Ripple Systems and is not for redistribution
  * any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioMFTAgentCallbackSubscriber.h $
  * @author:  Frik Stuart
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  * <description>
  */


#if !defined(AFX_RADIOMFTAGENTCALLBACKSUBSCRIBER_H__A36A6AB4_0EAB_496F_AA43_25F72B0225D1__INCLUDED_)
#define AFX_RADIOMFTAGENTCALLBACKSUBSCRIBER_H__A36A6AB4_0EAB_496F_AA43_25F72B0225D1__INCLUDED_

#include "bus/radio/Radio_Agent/idl/src/RadioMFTAgentCallbackIDL.h"
#include "bus/radio/radio_agent/idl/src/CommonRadioAgentIDL.h"
#include "bus/radio/radio_agent/idl/src/RadioCommonExceptionsIDL.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/corba/src/ServantBase.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RadioMFTAgentCallbackSubscriber : public virtual POA_TA_IRS_Bus::RadioMFTAgentCallbackIDL 
{
	public:

		RadioMFTAgentCallbackSubscriber();

		virtual ~RadioMFTAgentCallbackSubscriber();

	
	public:

		virtual void applicationStateMessage (
			TA_IRS_Bus::Radio::EApplicationState state, 
			CORBA::Short percent
		);

		virtual void applicationErrorMessage (const char* message);

		virtual void operationCancelMessage(
			TA_IRS_Bus::Radio::EApiState state,
			const TA_IRS_Bus::Radio::RadioResource& resource
		);

		virtual void groupStateMessage (
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::EGroupConnectionState status
		);

		virtual void resourceAssignmentMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			CORBA::Short buttonSet, 
			CORBA::Short buttonRef, 
			TA_IRS_Bus::Radio::EAssignmentState status
		);

		virtual void resourceSelectMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::ESpeakerState status
		);

		virtual void resourceVolumeMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::ESpeakerState speaker, 
			CORBA::Short volume
		);

		virtual void inboundCallMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& caller, 
			TA_IRS_Bus::Radio::EInboundCallState status
		);

		virtual void privateCallMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& caller, 
			TA_IRS_Bus::Radio::EPrivateCallState status
		);

		virtual void privateCallMessageRejected ( 
			const TA_IRS_Bus::Radio::RadioResource& caller
			);

		virtual void resourceActivityMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::EResourceActivityType status
		);

		virtual void resourcePTTMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& listener, 
			const char*  srcAlias, 
			TA_IRS_Bus::Radio::EPTTState status
		);

		virtual void resourceEmergencyModeMessage ( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::EmergencyModeState status, 
			const TA_IRS_Bus::Radio::RadioResource& changedBy
		);

		virtual void resourceAmbListeningMessage( 
			const TA_IRS_Bus::Radio::RadioResource& resource, 
			TA_IRS_Bus::Radio::EAmbienceListeningState status
		);

		virtual void resourceChangeMessage(
			const TA_IRS_Bus::Radio::RadioResource& oldResourceC,
			const TA_IRS_Bus::Radio::RadioResource& newResourceC
		);

		virtual CORBA::Long sendErrorMessage(CORBA::Long messageID);

		void privateCallRemoved(RadioResource& resource);

		
	private:

//		RadioResourceType convertResourceType(TA_IRS_Bus::ERadioResourceType etype);

		TA_Base_Core::CommsMessageSender* m_commsSender;
		
		unsigned long m_entityKey;
		unsigned long m_entitySubsystem;
		unsigned long m_entityLocation;
};

#endif 
