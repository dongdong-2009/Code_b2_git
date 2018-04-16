/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/OpcBridgeLib/attributes/DnAttributeNames.h $
  * @author:	Raymond Pau
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2018/03/14 13:51:20 $
  * Last modified by:	$Author: lim.cy $
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
