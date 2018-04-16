/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DpControlState.h $
  * @author  
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2016/11/01 16:40:39 $
  * Last modified by : $Author: wang.wang $
  *
  * Class representing the control status of the value of a DataPoint
  * For blinking using
  *
  */

#ifndef	DPCONTROLSTATE_H
#define DPCONTROLSTATE_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
	class DataPointAttributes;

	class DpControlState : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DpControlState
		//	Constructor
		//------------------------------------------------------------------------------------------
		DpControlState(DataPointAttributes* dpAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DpControlState
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpControlState();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_CONTROLSTATE;}

	private:
		DataPointAttributes* m_dpAttribute;
	};
}

#endif
