/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DpQuality.h $
  * @author  
  * @version $Revision: #4 $
  *
  * Last modification : $DateTime: 2013/07/12 15:49:47 $
  * Last modified by : $Author: haijun.li $
  *
  * Class representing the status of the value of a DataPoint
  *
  */

#ifndef	DPQUALITY_H
#define DPQUALITY_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
	class DataPointAttributes;

	class DpQuality : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DpQuality
		//	Constructor
		//------------------------------------------------------------------------------------------
		DpQuality(DataPointAttributes* dpAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DpQuality
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DpQuality();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();

		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DP_QUALITY;}

	private:
		DataPointAttributes* m_dpAttribute;
	};
}

#endif
