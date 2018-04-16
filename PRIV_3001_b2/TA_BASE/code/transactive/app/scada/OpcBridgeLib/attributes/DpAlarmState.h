/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DpAlarmState.h $
  * @author  HoaVu
  * @version $Revision: #1 $
  *
  * Last modification : $DateTime: 2018/03/14 13:51:20 $
  * Last modified by : $Author: lim.cy $
  *
  * Represents the DataPoint's DpAlarmState attribute.  The DpAlarmState attribute
  * receives and processes the alarm information from the Alarm and Messaging
  * sub systems.
  *
  */

#ifndef DPALARMSTATE_H
#define DPALARMSTATE_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
	class DataPointAttributes;

	class DpAlarmState : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: Value
		//	Constructor
		//------------------------------------------------------------------------------------------
        DpAlarmState(DataPointAttributes* dpAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~Value
		//	Default Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpAlarmState();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_ALARMSTATE;}

	private:
        DataPointAttributes* m_dpAttribute;
	};
}
#endif
