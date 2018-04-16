/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/globals.h $
  * @author:  B. Carbonaro
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/07/30 17:47:04 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This is a header file that contains global application definitions.
  */

#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef TRANSACTIVE_BUILD

//using namespace TA_Base_Core;

namespace TA_Base_Core
{
    // External versioning variables
    // See VersionGen.cpp
    //
    extern char buildDate[];
    extern char componentName[];
    extern char componentVersion[];
}
#endif // TRANSACTIVE_BUILD

const static char* COMPANY_NAME = "Ripple Systems";
const static char* EXECUTABLE_NAME = "ReportManager.exe";
const static char* APPLICATION_NAME = "ReportManager";

#ifdef TRANSACTIVE_BUILD
const static char* CAPTION_TITLE = TA_Base_Core::componentName;
const static char* APPLICATION_VERSION = TA_Base_Core::componentVersion;
const static char* BUILD_DATE = TA_Base_Core::buildDate;
#else
const static char* APPLICATION_VERSION = "1.04";
const static char* BUILD_DATE = "29/04/2004";
#endif // TRANSACTIVE_BUILD

#define APPLICATION_RC_VERSION 1,4,0,0

const static char* REG_USELOGON_LABEL = "UseLogon";
const static char* REG_DLLNAME_LABEL = "DllName";
const static char* REG_SERVERNAME_LABEL = "ServerName";
const static char* REG_DATABASENAME_LABEL = "DatabaseName";
const static char* REG_USERID_LABEL = "UserId";
const static char* REG_PASSWORD_LABEL = "Password";
const static char* REG_WINDOWTOP_LABEL = "WindowTop";
const static char* REG_WINDOWLEFT_LABEL = "WindowLeft";
const static char* REG_WINDOWBOTTOM_LABEL = "WindowBottom";
const static char* REG_WINDOWRIGHT_LABEL = "WindowRight";

const static char* VERSION_LABEL = "Version";
const static char* FULLSCREEN_LABEL = "FullScreen";
const static char* REPORTNAME_LABEL = "File";
const static char* VIEWREPORT_LABEL = "View";
const static char* PRINTREPORT_LABEL = "Print";
const static char* EXPORTREPORT_LABEL = "Export";
const static char* ENABLEPROMPT_LABEL = "Prompt";
const static char* ENABLETIMESTAMP_LABEL = "Timestamp";
const static char* EXPORTFORMAT_LABEL = "Format"; 
const static char* OUTFILE_LABEL = "OutFile"; 
const static char* OUTPATH_LABEL = "OutPath";
const static char* DLLNAME_LABEL = "DllName";
const static char* SERVERNAME_LABEL = "ServerName";
const static char* DATABASENAME_LABEL = "DatabaseName";
const static char* USERID_LABEL = "UserId"; 
const static char* PASSWORD_LABEL = "Password";
const static char* NOPARAMPROMPTING_LABEL = "NoParameters";
const static char* HELP_LABEL = "Help";
const static char* H_LABEL = "H";
const static char* QUESTION_LABEL = "?";
const static char* REPORTPARAMETER_LABEL = "ReportParameters";

const static char* YES_STRING = "yes";
const static char* NO_STRING = "no";

const static char* RPARAM_ENTITYKEY = "ReportManagerEntityKey";

const static char* PRINT_INCIDENT_LABEL = "PrintIncident";
const static char* INCIDENT_REPORT_ID_LABEL = "IncidentId";

//TD18040
//xxs add
const static char * HISTORY_REPORT_PATH_LABEL = "HistoryreportPath"; //--historyreport-path
//end xxs add

// The rate of ProgressBar show in ReportManager's Statusbar.
static int OPEN_REPORT_FILE_RATE = 0;

// Coz that we don't know the opening and viewing report's rate from the inside of Crystal Report 9,
// we set some milestones for the rate of viewReport and openReport.
const static int OPEN_REPORT_RATE_INIT_TO_ZERO = 0;
const static int OPEN_REPORT_RATE_MILESTONE_ONE = 200;
const static int OPEN_REPORT_RATE_MILESTONE_TWO = 400;
const static int OPEN_REPORT_RATE_MILESTONE_THREE = 600;
const static int OPEN_REPORT_RATE_MILESTONE_FOUR = 800;
const static int OPEN_REPORT_RATE_COMPLETED = 1000;
// If the progress bar already come to current milestone, it should continue step to nex milestone, 
// we set the value between current milestone and next milestone in our ReportManager project to 200.
const static int OPEN_REPORT_RATE_STEP = 200;

#endif
