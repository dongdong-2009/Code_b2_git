/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcServerAgent/src/AbstractSimpleData.h $
  * @author  Dhanshri
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2016/08/15 15:49:47 $
  * Last modified by : $Author: Dhanshri $
  *
  * The AbstractSimpleData class maintains a list of nodes/points currently requested by the OPC Client
  *
  */

#ifndef ABSSIMPLEDATA_H
#define ABSSIMPLEDATA_H

#pragma warning ( disable:4786 )

#include <list>
#include <map>
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
	typedef std::vector<DisplayStateSequence> DisplayStateSequenceVector;
	typedef std::vector<DataNodeDisplayState> DataNodeDisplayStateVector;
};

namespace TA_Base_App
{
	enum OPC_SIMPLE_ENTITY_TYPE
	{
		E_OPC_SDN = 0,
		E_OPC_SDP,
		E_OPC_SOTHER
	};

    class AbstractSimpleData
    {

    public:
		AbstractSimpleData() 		: m_entityKey(0), 		  m_agentKey(0)
		{
		}

		AbstractSimpleData(unsigned long entityKey, unsigned long agentKey):m_entityKey(entityKey), m_agentKey(agentKey)
		{
		}

		virtual ~AbstractSimpleData(){}

		/**
         * processProxyUpdate
         *
         * Update all the attributes' tag when their data had changed
         *
         */
		virtual void processProxyUpdate() {};

		unsigned long getEntityKey() { return m_entityKey; }

		/**
         * getAgentKey
         *
         * get agent key of this data object
		 *
         * @return agent key of this data object
		 *
         */
		unsigned long getAgentKey() { return m_agentKey; }

		/**
         * getLocationKey
         *
         * get location key of this data object
		 *
         * @return location key of this data object
		 *
         */

		/**
         * getAttributeType
         *
         * get Attribute Type No of this attribute
		 *
		 * @param   attrName    The attribute name of the attribute
         * 
		 * @return object Type enum No
		 *
         */
		virtual OPC_SIMPLE_ENTITY_TYPE getAttributeType() = 0;

    protected:
		unsigned long		m_entityKey;
		unsigned long		m_agentKey;
		OPC_SIMPLE_ENTITY_TYPE m_type;
    };

	class SimplePointData: public AbstractSimpleData
    {
	public:
		SimplePointData()
		{
			m_type = E_OPC_SDP;
		}
		virtual OPC_SIMPLE_ENTITY_TYPE getAttributeType() 
		{
			return m_type;
		}
		~SimplePointData()
		{
			delete m_dpUpdates;
			m_dpUpdates = 0;
		}

		TA_Base_Bus::DisplayStateSequence* m_dpUpdates;
	};

	class SimpleNodeData: public AbstractSimpleData
    {
	public:
		SimpleNodeData()
		{
			m_type = E_OPC_SDN;
		}
		virtual OPC_SIMPLE_ENTITY_TYPE getAttributeType() 
		{
			return m_type;
		}
		~SimpleNodeData()
		{
			delete m_dnUpdates;
			m_dnUpdates = 0;
		}
		TA_Base_Bus::DataNodeDisplayStateVector* m_dnUpdates;
	};

}

#endif // #ifndef ABSSIMPLEDATA_H
