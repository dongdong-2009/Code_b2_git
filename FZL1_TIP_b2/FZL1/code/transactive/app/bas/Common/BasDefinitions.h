#ifndef BAS_DEFINITIONS_H
#define BAS_DEFINITIONS_H

//defined in data table
enum ModeRunningStatus
{
	eModeStateUnknown = 0,
	eModeExecuting = 1,
	eModeExecutionFail = 3,
	eModeExecutionSuccess = 2
};

/*  value  status, defined in data table
	1		Auto
    2		Manual */
enum BasControlPrivilege
{
    eControlUnknown = -1,
    eControlManual = 0,
    eControlHMI = 1,
    eControlTimeTalbe = 2,
    eControlAuto = 3,
    eControlIBP = 4,
    eControlFAS = 5,
    eControlLinkage = 6
};

enum BasSetControlType
{
    eSetControlTypeUnknown = -1,
    eSetControlTypeHMI = 1,
    eSetControlTypeTimeTable = 2,
    eSetControlTypeAuto = 3,
    eSetControlTypeRevoke = 9
};

enum FireRecoverMode
{
    eModeUnknown = 0,
    eModeReset = 1
};

#endif