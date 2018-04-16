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

#ifndef ISIMPLE_POINT_AGENT_H
#define ISIMPLE_POINT_AGENT_H

#include <map>

namespace TA_Base_Bus
{
	// forward declaration
	class SimplePointObserver;
    class SimplePointAgentSynHandler;
	class SimplePoint;
    class SimplePointDisplayClient;
    class SimplePointFactory;

	class ISimplePointAgent
	{
	public:

		virtual ~ISimplePointAgent() {};


		/**
		 * getSimplePoint
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * SimplePointAgentCorbaDef_Impl to obtain a reference to the data point, specified by
		 * the pkey, which is created and maintained by the agent.  A null pointer
		 * is returned if the specific data point agent does not have a data point whose pkey
		 * matches with the specified pkey.
		 *
		 * @param	entityKey	The pkey of the wanted data point
		 *
		 */
		virtual TA_Base_Bus::SimplePoint * getSimplePoint ( unsigned long entityKey ) = 0;

        // liuyu++
        /**
		 * getAllSimplePoints
		 *
		 * To be implemented by the specific data point agent.  The method allows the class
		 * DataPointAgentCorbaDef_Impl to obtain all the references to the data points which 
         * is created and maintained by the agent.
		 *
		 */
        virtual void getAllSimplePoints (std::map < unsigned long, TA_Base_Bus::SimplePoint *>& SimplePointList ) = 0;
        // ++liuyu
	};

}
#endif

