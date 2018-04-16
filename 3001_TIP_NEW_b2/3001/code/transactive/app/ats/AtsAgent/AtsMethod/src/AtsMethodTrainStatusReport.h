/**
* This class encapulates the data associated with a ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:32:15
*/

#ifndef ATS_METHOD_TRAIN_STATUS_REPORT_H
#define ATS_METHOD_TRAIN_STATUS_REPORT_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpMethod.h"

namespace TA_IRS_App
{
	class AtsMethodTrainStatusReport : public AtsITcpMethod
	{
	public:
	   /**
	    * Construtor
		* @param: trainID, criticalAlarmEvents
		*/
		AtsMethodTrainStatusReport(DataGramVector criticalAlarmEvents);
	   
	   /**
		* Destructor
		*/
		virtual ~AtsMethodTrainStatusReport();
	private:

		/**
		 * Disable the assignment and copy constructors
		 */
	     AtsMethodTrainStatusReport& operator=(const AtsMethodTrainStatusReport &);
		 AtsMethodTrainStatusReport( const AtsMethodTrainStatusReport& itcpMethod);

	};
};

#endif // end of ATS_METHOD_TRAIN_STATUS_REPORT_H