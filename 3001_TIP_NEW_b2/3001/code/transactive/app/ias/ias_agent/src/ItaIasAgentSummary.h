/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/ItaIasAgentSummary.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  ItaIasAgentSummary.h
//  Implementation of the Interface ItaIasAgentSummary
//  Created on:      04-Mar-2005 08:24:12 AM
///////////////////////////////////////////////////////////

#if !defined(ItaIasAgentSummary_C3A5C5DB_3CA3_40de_B63C_E25AF0EE41D7__INCLUDED_)
#define ItaIasAgentSummary_C3A5C5DB_3CA3_40de_B63C_E25AF0EE41D7__INCLUDED_

#include "ItaScada.h"
#include "ItaRuntime.h"

/**
 * Simply a placeholder to combine all TransActive interfaces to a single point.
 * @version 1.0
 * @created 04-Mar-2005 08:24:12 AM
 */
class ItaIasAgentSummary : public ItaScada, public ItaRuntime 
{

};
#endif // !defined(ItaIasAgentSummary_C3A5C5DB_3CA3_40de_B63C_E25AF0EE41D7__INCLUDED_)