/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  *	DataPointAccessCorbaDef_Impl is an abstract base class that implements
  * the DataPointAgentCorbaDef interface, providing DataPoint configurations
  * and updates to remote clients.
  *
  */


#include "time.h"

#include "core/naming/src/Naming.h"
#include "core/exceptions/src/CorbaException.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datanode_library/src/DataNode.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h"
#include "bus/scada/DataNodeAgentCorbaDef_Impl/src/DataNodeAccessCorbaDef_Impl.h"


//TD15618, use timer to trigger if the observers are timeout
namespace TA_Base_Bus
{

	DataNodeAccessCorbaDef_Impl::DataNodeAccessCorbaDef_Impl ( const std::string & agentName, TA_Base_Bus::IDataNodeAgent * specificDataNodeAgent )
	:
	m_agentName ( agentName ),
	m_operationMode ( TA_Base_Core::NotApplicable ),
	m_specificAgent ( 0 )
	{
		// save the pointer to the data point agent for later usage
		m_specificAgent = specificDataNodeAgent;

		// activate the ServantBase of the class DataNodeAccessCorbaDef_Impl
		activateServantWithName ( m_agentName + TA_Base_Bus::DN_ACCESS_INTERFACE );
	}


	DataNodeAccessCorbaDef_Impl::~DataNodeAccessCorbaDef_Impl()
	{
		// clear the reference to the data point agent, without actually deleting
		// the object
		m_specificAgent = 0;
		// CL-17309 ++
		deactivateServant();
		// ++ CL-17309
		//deactivateAndDeleteServant();
	}

	void DataNodeAccessCorbaDef_Impl::setToControl()
	{
		m_operationMode = TA_Base_Core::Control;
	}


	void DataNodeAccessCorbaDef_Impl::setToMonitor()
	{
		m_operationMode = TA_Base_Core::Monitor;
	}

	void DataNodeAccessCorbaDef_Impl::changeDataNodeTag(CORBA::ULong dnKey, const char* sessionId, 
		DataNodeCorbaTypes::ETagState tagState, ScadaCorbaTypes::EOutputInhibitState outputInhibitState, 
		ScadaCorbaTypes::EInputInhibitState inputInhibitState, const char* ptwId, const char* comment)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::changeDataNodeTag");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);

			if(0 != dataNode)
			{
				TA_Base_Bus::ETagState newTagState;
				convertETagState(tagState, newTagState);
				TA_Base_Bus::EInputInhibitState newInputInhibitState;
				convertInputInhibitState(inputInhibitState, newInputInhibitState);
				TA_Base_Bus::EOutputInhibitState newOutputInhibitState;
				convertOutputInhibitState(outputInhibitState, newOutputInhibitState);
				dataNode->changeDataNodeTag(sessionId, newTagState, 
					newOutputInhibitState, newInputInhibitState, ptwId, comment);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataNodeAccessCorbaDef_Impl::setNotes(CORBA::ULong dnKey, const char* sessionId, const char* notes)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::setNotes");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);
			
			if(0 != dataNode)
			{
				dataNode->setNotes(sessionId, notes);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

		FUNCTION_EXIT;
	}

	void DataNodeAccessCorbaDef_Impl::setInhibitMMS(CORBA::ULong dnKey, const char* sessionId, CORBA::Boolean inhibitMms)
	{
		FUNCTION_ENTRY("DataNodeAccessCorbaDef_Impl::setInhibitMMS");

		if(m_operationMode == TA_Base_Core::Control)
		{
		TA_Base_Bus::DataNode* dataNode = m_specificAgent->getDataNode(dnKey);
			
			if(0 != dataNode)
			{
				dataNode->setInhibitMMS(sessionId, inhibitMms);
			}
			else
			{
				TA_THROW_CORBA(TA_Base_Core::NotAvailableException());
			}
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}
		
		FUNCTION_EXIT;
	}

	//wenching++ (TD13621)
    //Get flexible timeout value for the data node named object
    unsigned short DataNodeAccessCorbaDef_Impl::getFlexibleTimeOutValue()
    {
		if(m_operationMode == TA_Base_Core::Control)
		{
			unsigned short flexibleTimeout = 300;
			return flexibleTimeout;
		}
		else
		{
			TA_THROW_CORBA(TA_Base_Core::OperationModeException());
		}

    }
    //++wenching (TD13621)


	
}
