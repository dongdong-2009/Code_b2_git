/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ias/ias_agent/src/CtaIasAgentSummary.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
///////////////////////////////////////////////////////////
//  CtaIasAgentSummary.h
//  Implementation of the Class CtaIasAgentSummary
//  Created on:      04-Mar-2005 08:30:28 AM
///////////////////////////////////////////////////////////

#if !defined(CtaIasAgentSummary_AC7C113A_6CD1_4fda_9951_0B07A94FD49E__INCLUDED_)
#define CtaIasAgentSummary_AC7C113A_6CD1_4fda_9951_0B07A94FD49E__INCLUDED_

#include "CtaScada.h"
#include "CtaRuntime.h"
#include "ItaIasAgentSummary.h"

/**
 * Simply a placeholder to combine all concrete TransActive interfaces to a single
 * point.
 * @version 1.0
 * @created 04-Mar-2005 08:30:28 AM
 */
class CtaIasAgentSummary : public CtaScada, public CtaRuntime, public ItaIasAgentSummary 
{

public:
	virtual ~CtaIasAgentSummary();

public:
	/**
	 * Fancy constructor containing all required parameters for the inherited classes.
	 */
	CtaIasAgentSummary();

};
#endif // !defined(CtaIasAgentSummary_AC7C113A_6CD1_4fda_9951_0B07A94FD49E__INCLUDED_)