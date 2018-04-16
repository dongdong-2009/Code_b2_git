/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DnAlarmStateSummary.h $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  * Class representing the Alarm Status attribute of a DataNode
  *
  */

#ifndef DNALARMSTATUSSUMMARY_H
#define DNALARMSTATUSSUMMARY_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
    class DataNodeAttributes;

	class DnAlarmStateSummary : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DnAlarmStateSummary
		//	Constructor
		//------------------------------------------------------------------------------------------
		DnAlarmStateSummary(DataNodeAttributes* dnAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DnAlarmStateSummary
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DnAlarmStateSummary();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DN_ALARMSTATE;}

	private:
		DataNodeAttributes* m_dnAttribute;
	};
}

#endif
