/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/IncidentLocationType.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This structure specifies the locations where the incident occurred at.
  */
#if !defined(IncidentLocationType_6611E66B_AD37_4912_9B47_F2845A2D264D__INCLUDED_)
#define IncidentLocationType_6611E66B_AD37_4912_9B47_F2845A2D264D__INCLUDED_

/**
 * This structure specifies the locations where the incident occurred at.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:52 PM
 */
#include <string>
namespace TA_IRS_Core
{
struct IncidentLocationType
{

public:
	/**
	 * Set this to true if the incident has occurred on the train.
	 */
	bool train;
	/**
	 * Set this to true if the incident has occurred on the trackside.
	 */
	bool trackside;
	/**
	 * Set this to true if the incident has occurred at a station.
	 */
	bool station;
	/**
	 * Set this to true if the incident has occurred at the depot.
	 */
	bool depot;
	/**
	 * This contains the location(s) other than the ones that are predefined.
	 */
	std::string other;

	};
}// end TA_IRS_Core
#endif // !defined(IncidentLocationType_6611E66B_AD37_4912_9B47_F2845A2D264D__INCLUDED_)