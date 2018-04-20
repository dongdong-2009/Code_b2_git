/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/PlanType.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//  PlanType.h
//  Implementation of the Class PlanType
//  Created on:      16-May-2005 12:33:58 PM
//  Original author: San Teo
///////////////////////////////////////////////////////////

#if !defined(PlanType_652B97C0_890E_43a5_80AE_631409EBAF3F__INCLUDED_)
#define PlanType_652B97C0_890E_43a5_80AE_631409EBAF3F__INCLUDED_

/**
 * This contains the plan details that are required by the incident manager.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:58 PM
 */
#include <string>
#include "core/utilities/src/FunctionEntry.h"

namespace TA_IRS_Core
{
	typedef std::string PlanType;
}// end TA_IRS_Core

#endif // !defined(PlanType_652B97C0_890E_43a5_80AE_631409EBAF3F__INCLUDED_)