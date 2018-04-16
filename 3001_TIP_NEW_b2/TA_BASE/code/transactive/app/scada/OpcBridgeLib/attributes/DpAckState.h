/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DpAckState.h $
  * @author  HoaVu
  * @version $Revision: #4 $
  *
  * Last modification : $DateTime: 2013/07/12 15:49:47 $
  * Last modified by : $Author: haijun.li $
  *
  * Represents the DataPoint's DpAckState attribute.  The DpAckState attribute
  * receives and processes the alarm information from the Alarm and Messaging
  * sub systems.
  *
  */

#ifndef DPACKSTATE_H
#define DPACKSTATE_H

#include "app/scada/opcbridgelib/src/Attribute.h"
#include "app/scada/OpcBridgeLib/src/DataPointAttributes.h"
#include "app/scada/opcbridgelib/src/DisplayDataPointProxy.h"
#include "bus/scada/common_library/src/CommonDefs.h"

namespace TA_Base_App
{
	class DpAckState : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: Value
		//	Constructor
		//------------------------------------------------------------------------------------------
        DpAckState(DataPointAttributes* dpAttribute, const std::string& tagName);


		//------------------------------------------------------------------------------------------
		// Operation: ~Value
		//	Default Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpAckState();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_ACKSTATE;}

	private:	
        DataPointAttributes* m_dpAttribute;
	};
}
#endif
