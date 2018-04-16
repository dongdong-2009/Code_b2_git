/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/IncidentType.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This struct just encapsulates all the predefined types of incidents that could
  * be captured in a report.
  */

#if !defined(IncidentType_5A8E8570_C2A0_4c5e_A28C_E348B3AD1B06__INCLUDED_)
#define IncidentType_5A8E8570_C2A0_4c5e_A28C_E348B3AD1B06__INCLUDED_

/**
 * This struct just encapsulates all the predefined types of incidents that could
 * be captured in a report.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:33:52 PM
 */
#include <string>

namespace TA_IRS_Core
{	
	struct IncidentType
	{
		
	public:
	/**
	* Set this to true if the incident involves a train service delay.
		*/
		bool trainServiceDelay;
		/**
		* Set this to true if the incident involves derailment
		*/
		bool derailment;
		/**
		* Set this to true if the incident involves damaged facility or property
		*/
		bool damagedFacilityOrProperty;
		/**
		* Set this to true if the incident involves fire
		*/
		bool fire;
		/**
		* Set this to true if the incident involves equipment abnormally
		*/
		bool equipmentAbnormally;
		/**
		* Set this to true if the incident involves equipment malfunction.
		*/
		bool equipmentMalfunction;
		/**
		* Set this to true if the incident involves trespassing
		*/
		bool trespassing;
		/**
		* Set this to true if the incident involves a criminal act.
		*/
		bool criminalAct;
		/**
		* Set this to true if the incident involves an arrest of a person
		*/
		bool arrestOfPerson;
		/**
		* Set this to true if the incident involves an assault
		*/
		bool assault;
		/**
		* Set this to true if the incident involves a heated argument
		*/
		bool heatedArgument;
		/**
		* This contains any incident types besides the predefined ones.
		*/
		std::string other;
		
	};
}// end TA_IRS_Core
#endif // !defined(IncidentType_5A8E8570_C2A0_4c5e_A28C_E348B3AD1B06__INCLUDED_)