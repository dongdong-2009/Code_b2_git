/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/incident_dai/src/IncidentCauseType.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This structure captures the cause of the incident
  */
#if !defined(IncidentCauseType_0CAE8B2E_1E68_499c_AA9A_783818799FE1__INCLUDED_)
#define IncidentCauseType_0CAE8B2E_1E68_499c_AA9A_783818799FE1__INCLUDED_

/**
 * This structure captures the cause of the incident
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:50 PM
 */
#include <string>

namespace TA_IRS_Core
{	
struct IncidentCauseType
{

public:
	/**
	 * Set this to true if it the incident was caused by ESP
	 */
	bool eSP;
	/**
	 * Set this to true if it the incident was caused by BLS
	 */
	bool bLS;
	/**
	 * Set this to true if it the incident was caused by Pax
	 */
	bool pax;
	/**
	 * Set this to true if it the incident was caused by Point
	 */
	bool point;
	/**
	 * Set this to true if it the incident was caused by Power
	 */
	bool power;
	/**
	 * Set this to true if it the incident was caused by Signal
	 */
	bool signal;
	/**
	 * Set this to true if it the incident was caused by TkCct
	 */
	bool tkCct;
	/**
	 * This contains the factors that caused the accident beside the ones that are
	 * predefined.
	 */
	std::string other;

	};
}// end TA_IRS_Core
#endif // !defined(IncidentCauseType_0CAE8B2E_1E68_499c_AA9A_783818799FE1__INCLUDED_)