/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/scada/inspector_panel/src/DataPointControlHelper.h $
  * @author:  Rod Rolirad
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This singleton creates a map of permissions for InspectorPanel.
  */

#pragma warning ( disable:4786 )

#if !defined(AFX_DataPointControlHelper_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_)
#define AFX_DataPointControlHelper_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Bus
{
	class IDataPointProxyWithoutAlarmAckStatus;
}

namespace TA_Base_App
{
	class DataPointControlHelper  
	{
	public:		

        static bool isControlValueValid(TA_Base_Bus::DataPointProxySmartPtr& dataPoint, 
                                        const std::string& strValue);

	};

} // namespace

#endif // !defined(AFX_DataPointControlHelper_H__D7F32719_5D89_477E_8272_A62BC014A053__INCLUDED_)
