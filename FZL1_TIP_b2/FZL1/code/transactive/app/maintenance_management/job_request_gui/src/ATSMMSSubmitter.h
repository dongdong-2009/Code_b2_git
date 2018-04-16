/*
* The source code in this file is the property of 
* Combuilder and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/ATSMMSSubmitter.h $
* @author: Noel R. Tajanlangit
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/03/20 14:12:57 $
* Last modified by:  $Noel R. Tajanlangit $
* 
* This class inherits from IATSMMSSubmitter which provides common interface to client who wants to submit job request message to ATS
* a instance of the interface should be ensure that all functions are thread safe function.
*
*/
#pragma once
#include "IATSMMSSubmitter.h"

namespace TA_IRS_App
{
	class ATSMMSSubmitter : public IATSMMSSubmitter
	{
	public:
		// Constructor
		ATSMMSSubmitter(void);
		// Destructor
		~ATSMMSSubmitter(void);
		
		/*
		 *@function		: getDataFromAlarmStore
		 *
		 *@Description	: creates JobRequestDetailCorbaDef retrieving the alarm information from alarm store
		 *
		 *@param		: std::string alarmID - alarmId to retrieve
		 *
		 *@return		: pointer to JobRequestDetailCorbaDef
		 */	
		virtual TA_Base_Bus::JobRequestDetailCorbaDef* getDataFromAlarmStore(std::string alarmID);

		/**
		* submitJobRequest
		*
		* use to submit MMS request message in alarm GUI to ATS using shared memory by manual
		*
		* @param JobRequestDetailCorbaDef - Job request information
		* 
		* @return bool - true means submit successfully, false: submit failed.
		*                  could be determined.
		*/
		virtual bool submitJobRequest(TA_Base_Bus::JobRequestDetailCorbaDef& jobRequest);

	};

}
