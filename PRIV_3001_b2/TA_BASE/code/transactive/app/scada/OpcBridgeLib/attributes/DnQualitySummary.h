/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DnQualitySummary.h $
  * @author:	Darren Sampson
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
  *
  * Description:
  * Class representing the Quality attribute of a DataNode
  *
  */

#ifndef DNQUALITYSUMMARY_H
#define DNQUALITYSUMMARY_H

#include "app/scada/opcbridgelib/src/Attribute.h"

namespace TA_Base_App
{
    class DataNodeAttributes;

	class DnQualitySummary : public Attribute
	{
	public:

		//------------------------------------------------------------------------------------------
		// Operation: DnQualitySummary
		//	Constructor
		//------------------------------------------------------------------------------------------
		DnQualitySummary(DataNodeAttributes* dnAttribute, const std::string& tagName);

		//------------------------------------------------------------------------------------------
		// Operation: ~DnQualitySummary
		//	Destructor
		//------------------------------------------------------------------------------------------
		virtual ~DnQualitySummary();

	public:

		/**
		 * processProxyUpdate
		 *
		 * Process the update messages received from the proxy
		 *
		 *
		 */
		void processProxyUpdate();
		
		virtual EOPCATTRIBUTETYPE getAttributeType() {return E_OPC_DN_QUALITY;}

	private:
		DataNodeAttributes* m_dnAttribute;
	};
}

#endif
