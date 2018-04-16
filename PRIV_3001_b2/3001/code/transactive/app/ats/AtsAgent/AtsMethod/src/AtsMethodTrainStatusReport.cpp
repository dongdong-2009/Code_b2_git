/**
* The implentation for the AtsMethodTrainStatusReport class which encapulates the data associated with the ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:45:15
*/

#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodTrainStatusReport.h"

using namespace TA_IRS_App;

AtsMethodTrainStatusReport::AtsMethodTrainStatusReport(DataGramVector criticalAlarmEvents)
: AtsITcpMethod(criticalAlarmEvents)
{
}

AtsMethodTrainStatusReport::~AtsMethodTrainStatusReport()
{
}
