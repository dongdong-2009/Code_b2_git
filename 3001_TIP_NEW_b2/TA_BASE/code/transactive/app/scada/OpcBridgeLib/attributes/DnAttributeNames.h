/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DnAttributeNames.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #4 $
  *
  * Last modification:	$DateTime: 2013/07/12 15:49:47 $
  * Last modified by:	$Author: haijun.li $
  *
  * Description:
  *
  */

#ifndef DATANODEATTRIBUTENAMES_H
#define DATANODEATTRIBUTENAMES_H

namespace TA_Base_App
{
	// TD16297, Remove attributes
	const std::string DN_ATTR_NAME_SHORT_LABEL		= "ShortLabel";
	const std::string DN_ATTR_NAME_LABEL			= "Label"; 
	const std::string DN_ATTR_NAME_QUALITY			= "Quality";
	const std::string DN_ATTR_NAME_ALARM_STATE		= "AlarmState";
	const std::string DN_ATTR_NAME_ACK_STATE		= "AckState";
	const std::string DN_ATTR_NAME_ASSET_NAME		= "AssetName";
	const std::string DN_ATTR_NAME_ONITP			= "ONITP";
}

#endif
