/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/ItaRadioManagerSummary.h $
 * @author:  Glen Kidd
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#if !defined(AFX_ITARADIOMANAGERSUMMARY_H__5264C679_E3AA_432F_9BAE_CCDCDD79F562__INCLUDED_)
#define AFX_ITARADIOMANAGERSUMMARY_H__5264C679_E3AA_432F_9BAE_CCDCDD79F562__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/message/public_interfaces/ItaAuditing.h"
#include "core/alarm/public_interfaces/ItaAlarms.h"
#include "core/process_management/public_interfaces/ItaRuntime.h"
#include "bus/security/rights_library/public_interfaces/ItaRights.h"


class ItaRadioManagerSummary :  public virtual TA_Base_Core::ItaRuntime,
                                //public virtual TA_Base_Core::ItaAlarms,
								public virtual TA_Base_Core::ItaAuditing,
                                public virtual TA_Base_Bus::ItaRights
{
	public:

		virtual ~ItaRadioManagerSummary() {};

};

#endif // !defined(AFX_ITARADIOMANAGERSUMMARY_H__5264C679_E3AA_432F_9BAE_CCDCDD79F562__INCLUDED_)
