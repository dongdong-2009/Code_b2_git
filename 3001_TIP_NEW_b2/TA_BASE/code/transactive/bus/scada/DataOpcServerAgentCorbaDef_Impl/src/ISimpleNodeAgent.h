/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/scada/DataNodeAgentCorbaDef_Impl/src/ISimpleNodeAgent.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *	Description
  *
  */

#ifndef ISIMPLE_NODE_AGENT_H
#define ISIMPLE_NODE_AGENT_H

#include <map>

namespace TA_Base_Bus
{
	// forward declaration
	class SimpleNode;

	class ISimpleNodeAgent
	{
	public:

		virtual ~ISimpleNodeAgent() {};


		/**
		 * getDataNode
		 *
		 * To be implemented by the specific simple node agent.  The method allows the class
		 * DataNodeAgentCorbaDef_Impl to obtain a reference to the data node, specified by
		 * the pkey, which is created and maintained by the agent.  A null pointer
		 * is returned if the specific data node agent does not have a data node whose pkey
		 * matches with the specified pkey.
		 *
		 * @param	entityKey	The pkey of the wanted data node
		 *
		 */
		virtual SimpleNode * getSimpleNode ( unsigned long entityKey ) = 0;
        /**
		 * getAllDataNodes
		 *
		 * To be implemented by the specific data node agent.  The method allows the class
		 * DataNodeAgentCorbaDef_Impl to obtain all the references to the data nodes which 
         * is created and maintained by the agent.
		 *
		 */
        virtual void getAllSimpleNodes (std::map < unsigned long, SimpleNode *>& dataNodeList ) = 0;
	};
}

#endif

