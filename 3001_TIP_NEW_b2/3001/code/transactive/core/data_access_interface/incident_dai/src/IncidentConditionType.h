/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/IncidentConditionType.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This structure captures the condition of the environment at the time the
  * incident occurred.
  */
#if !defined(IncidentConditionType_822419E3_DB7D_40ce_B3EA_2D2F770CEE20__INCLUDED_)
#define IncidentConditionType_822419E3_DB7D_40ce_B3EA_2D2F770CEE20__INCLUDED_

/**
 * This structure captures the condition of the environment at the time the
 * incident occurred.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:51 PM
 */

#include <string>

namespace TA_IRS_Core
{	
struct IncidentConditionType
{

public:
	/**
	 * Set this to true if it is dry
	 */
	bool dry;
	/**
	 * Set this to true if it is wet
	 */
	bool wet;
	/**
	 * Set this to true if it is clean
	 */
	bool clean;
	/**
	 * Set this to true if it is dirty
	 */
	bool dirty;
	/**
	 * Set this to true if it is well lit
	 */
	bool wellLit;
	/**
	 * Set this to true if it is poorly lit
	 */
	bool poorlyLit;
	/**
	 * This contains the conditions other than the ones that are predefined
	 */
	std::string other;

	};
}// end TA_IRS_Core
#endif // !defined(IncidentConditionType_822419E3_DB7D_40ce_B3EA_2D2F770CEE20__INCLUDED_)