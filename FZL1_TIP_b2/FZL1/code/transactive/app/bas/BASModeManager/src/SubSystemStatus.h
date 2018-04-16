#pragma once
#include "ace/Singleton.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "app/bas/BASModeManager/src/CommonDef.h"

Namespace_TA_IRS_APP_Begin
class ModeSet;
class ModeSet
{
public:
	ModeSet();
	~ModeSet();
public:
	typedef enum _subsystemModeType_
	{
		modeType_RunningMode = 0,
		modeType_CurrentMode,
		modeType_AutoManul
	}subsystemModeType;
	void updateModeInvalid(subsystemModeType modeType);
	void updateMode(unsigned long value, subsystemModeType modeType);
    void updateModeTimeoutFlag(bool isModeTimeout);
	unsigned long getMode(subsystemModeType modeType);
    bool getModeTimeoutFlag();

	static unsigned long getUnknowModeKey();
private:
	//TA_Base_Core::NonReEntrantThreadLockable m_modeLock;
	unsigned long m_runningMode;
	unsigned long m_currentMode;
	unsigned long m_modeAutoManul;
	static const unsigned long m_unknownMode;

    bool m_timeoutFlag;
};

class IsubsystemModeListenerInterface
{
public:
	virtual void updateSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType) = 0;
};

class SubSystemStatus
{
public:
	SubSystemStatus(void);
	~SubSystemStatus(void);
public:
	void updateSubsystemModeExecuteForClean(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType);
	void updateSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, unsigned long value, ModeSet::subsystemModeType modeType);
    void updateSubsystemModeTimeoutFlag(unsigned long locationKey, unsigned long subsystemKey,bool isModeTimeout);

	unsigned long getSubsystemMode(unsigned long locationKey, unsigned long subsystemKey, ModeSet::subsystemModeType modeType);
    bool getSubsystemModeTimeoutFlag(unsigned long locationKey, unsigned long subsystemKey);

public:
	// this should be called by CBasManagerDlg only
	void registerListener(IsubsystemModeListenerInterface* listener);
private:
	TA_Base_Core::NonReEntrantThreadLockable m_subsystemModeLock;
	typedef std::map<unsigned long, ModeSet> SubsystemsModeInfo; // subsystemkey, ModeSet
	typedef SubsystemsModeInfo::iterator SubsystemModeIterator;
	typedef std::map<unsigned long, SubsystemsModeInfo> LocationSubsystemsModeInfo; // locationKey SubsystemsModeInfo
	typedef LocationSubsystemsModeInfo::iterator LocationSubsystemsModeInfoIter;
	LocationSubsystemsModeInfo m_subsystemModes;

	IsubsystemModeListenerInterface* m_basManagerDlg; // this used to update the GUI
};
typedef ACE_Singleton<SubSystemStatus,ACE_Recursive_Thread_Mutex> SubSystemStatusInstance;
Namespace_TA_IRS_APP_End