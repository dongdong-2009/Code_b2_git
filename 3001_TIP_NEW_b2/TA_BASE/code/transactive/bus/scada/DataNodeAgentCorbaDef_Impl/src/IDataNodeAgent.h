/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/IDataNodeAgent.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef IDATA_NODE_AGENT_H
#define IDATA_NODE_AGENT_H

#include <map>

namespace TA_Base_Bus
{
	// forward declaration
	class DataNode;

	class IDataNodeAgent
	{
	public:

		virtual ~IDataNodeAgent() {};


		/**
		 * getDataNode
		 *
		 * To be implemented by the specific data node agent.  The method allows the class
		 * DataNodeAgentCorbaDef_Impl to obtain a reference to the data node, specified by
		 * the pkey, which is created and maintained by the agent.  A null pointer
		 * is returned if the specific data node agent does not have a data node whose pkey
		 * matches with the specified pkey.
		 *
		 * @param	entityKey	The pkey of the wanted data node
		 *
		 */
		virtual TA_Base_Bus::DataNode * getDataNode ( unsigned long entityKey ) = 0;
        /**
		 * getAllDataNodes
		 *
		 * To be implemented by the specific data node agent.  The method allows the class
		 * DataNodeAgentCorbaDef_Impl to obtain all the references to the data nodes which 
         * is created and maintained by the agent.
		 *
		 */
        virtual void getAllDataNodes (std::map < unsigned long, TA_Base_Bus::DataNode *>& dataNodeList ) = 0;
	};
}

#endif

