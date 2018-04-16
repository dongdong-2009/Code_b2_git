

#pragma once

#include "boost\shared_ptr.hpp"
#include <vector>
#include <windows.h>


#define ALLTRAIN		"ALL"
#define ONETRAIN		"1 Train"
#define TWOTRAIN		"2 Trains"
#define ALLTRAINNUM		-1
#define ONETRAINNUM		 1
#define TWOTRAINNUM		 2

struct WindowCtrlInfo
{
	long lCtrlId;
	HWND wndCtrl;
};

typedef std::vector <WindowCtrlInfo> ControlIdBufferType;



struct AutoSendingPecConfInfo
{
	bool bAutoSendPecAnswer;
	bool bAutoSendPecHold;
	bool bAutoSendPecReset;

	AutoSendingPecConfInfo ()
	{
		bAutoSendPecAnswer = false;
		bAutoSendPecHold = false;
		bAutoSendPecReset = false;
	}
};

struct AutoSendingPAConfInfo
{
	bool bAutoSendLive;
	bool bAutoSendPreRecorded;
	bool bAutoSendLiveReset;
	bool bAutoSendPreRecordedReset;
	bool bContinue;
	bool bAutoSendPreRecCompl;
	bool bPreRecEmergency;

	AutoSendingPAConfInfo ()
	{
		bAutoSendLive = false;
		bAutoSendPreRecorded = false;
		bAutoSendLiveReset = false;
		bAutoSendPreRecordedReset = false;
		bContinue = false;
		bAutoSendPreRecCompl = false;
		bPreRecEmergency = false;
	}
};


struct AutoSendingTTISConfInfo
{
	bool bAutoSendTTISLib;
	bool bSendTtisCmdRecvFailed;

	AutoSendingTTISConfInfo ()
	{
		bAutoSendTTISLib = false;
		bSendTtisCmdRecvFailed = false;
	}
};


struct ChangeOverConInfo
{
	bool bStatus;
	bool bTtisStatus;
	bool bPecAnsStatus;
	bool bPaLiveStatus;
	bool bPaPreRecStatus;
	bool bUpdateTtisStatus;

	ChangeOverConInfo ()
	{
		bStatus = false;
		bTtisStatus =false;
		bPecAnsStatus = false;
		bPaLiveStatus = false;
		bPaPreRecStatus = false;
		bUpdateTtisStatus = false;
	}
};

struct TrainCountInfo
{
	std::string strTrainCount;
	
	TrainCountInfo ()
	{
		strTrainCount = ALLTRAIN;
	}
};


struct RadioIdSetupInfo
{
	bool bIsUsePrimary;

	RadioIdSetupInfo ()
	{
		bIsUsePrimary = true;;
	}
};



typedef boost::shared_ptr<AutoSendingPecConfInfo> AutoSendingSharedPtrType;

enum EAutoSendType
{
	EPAType = 0,
	EPecType,
	ETTISType,
	EChangeOver,
	ETrainCount,
	ERadioIdSetup
};


class IConfigDataItem
{
	public:
		virtual EAutoSendType getType () = 0;
};

typedef boost::shared_ptr<IConfigDataItem> IConfigDataItemPtrType;


class PecAutoSendInfo : public IConfigDataItem
{
	public:
		
		PecAutoSendInfo ()
		{
			m_bAutoSendPecAnswer = true;
			m_bAutoSendPecHold = true;
			m_bAutoSendPecReset = true;
		}
		
		virtual EAutoSendType getType ()
		{
			return EPecType;
		}

		void setAutoSendPecAnswer (bool bAutoSendPecAnswer)
		{
			m_bAutoSendPecAnswer = bAutoSendPecAnswer;
		}

		void setAutoSendPecHold (bool bAutoSendPecHold)
		{
			m_bAutoSendPecHold = bAutoSendPecHold;
		}

		void setAutoSendPecReset (bool bAutoSendPecReset)
		{
			m_bAutoSendPecReset = bAutoSendPecReset;
		}
		
		bool getAutoSendPecAnswer ()
		{
			return m_bAutoSendPecAnswer;
		}

		bool getAutoSendPecHold ()
		{
			return m_bAutoSendPecHold;
		}

		bool getAutoSendPecReset ()
		{
			return m_bAutoSendPecReset;
		}

	private:
		bool m_bAutoSendPecAnswer;
		bool m_bAutoSendPecHold;
		bool m_bAutoSendPecReset;
};


class PAAutoSendInfo : public IConfigDataItem
{
	public:

		PAAutoSendInfo ()
		{
			m_bAutoSendPALive = true;
			m_bAutoSendPAPreRecorded = true;
			m_bAutoSendPALiveReset = true;
			m_bAutoSendPALiveReset = true;
			m_bAutoSendPAContinue = true; 
			m_bAutoSendPAPreRecCompl = true; 
		}

		virtual EAutoSendType getType ()
		{
			return EPAType;
		}

		void setAutoSendPALive (bool bValue)
		{
			m_bAutoSendPALive = bValue;
		}

		void setAutoSendPAPreRecorded (bool bValue)
		{
			m_bAutoSendPAPreRecorded = bValue;
		}

		void setAutoSendPALiveReset (bool bValue)
		{
			m_bAutoSendPALiveReset = bValue;
		}

		void setAutoSendPAPreRecordedReset (bool bValue)
		{
			m_bAutoSendPAPreRecordedReset = bValue;
		}


		void setAutoSendPAContinue (bool bValue)
		{
			m_bAutoSendPAContinue = bValue;
		}


		bool getAutoSendPALive ()
		{
			return m_bAutoSendPALive;
		}

		bool getAutoSendPAPreRecorded ()
		{
			return m_bAutoSendPAPreRecorded;
		}

		bool getAutoSendPALiveReset ()
		{
			return m_bAutoSendPALiveReset;
		}

		bool getAutoSendPAPreRecordedReset ()
		{
			return m_bAutoSendPAPreRecordedReset;
		}

		bool getAutoSendPAContinue ()
		{
			return m_bAutoSendPAContinue;
		}
		
		bool getAutoSendPAPreRecCompl ()
		{
			return m_bAutoSendPAPreRecCompl;
		}

		void setAutoSendPAPreRecCompl (bool bValue)
		{
			m_bAutoSendPAPreRecCompl = bValue;
		}
		
		bool getAutoSendPAPreRecEmergency ()
		{
			return m_bAutoSendPreRecEmergency;
		}

		void setAutoSendPAPreRecEmergency (bool bValue)
		{
			m_bAutoSendPreRecEmergency = bValue;
		}

	private:
		bool m_bAutoSendPALive;
		bool m_bAutoSendPAPreRecorded;
		bool m_bAutoSendPALiveReset;
		bool m_bAutoSendPAPreRecordedReset;
		bool m_bAutoSendPAContinue;
		bool m_bAutoSendPAPreRecCompl;
		bool m_bAutoSendPreRecEmergency;

};


class TTISAutoSendInfo : public IConfigDataItem
{
public:

	TTISAutoSendInfo ()
	{
		m_bAutoSendLibDonwload= true;
		m_bSendTtisCmdRecvFailed = true;
		
	}

	virtual EAutoSendType getType ()
	{
		return ETTISType;
	}

	void setAutoSendTTISLib(bool bValue)
	{
		m_bAutoSendLibDonwload = bValue;
	}

	bool getAutoSendTTISLib ()
	{
		return m_bAutoSendLibDonwload;
	}
	
	void setSendTtisCmdRecvFailed(bool bValue)
	{
		m_bSendTtisCmdRecvFailed = bValue;
	}

	bool getSendTtisCmdRecvFailed ()
	{
		return m_bSendTtisCmdRecvFailed;
	}

private:
	bool m_bAutoSendLibDonwload;
	bool m_bSendTtisCmdRecvFailed;

};


class ChangeOverConfig : public IConfigDataItem
{
	public:
		ChangeOverConfig ()
		{
			m_lStatus = true;
			m_lTtisStatus = true;
			m_lPecAnsStatus = true;
			m_lPaLiveStatus = true;
			m_lPaPreRecStatus = true;
			m_lUpdateTtisStatus = true;
		}

		virtual EAutoSendType getType ()
		{
			return EChangeOver;
		}

		void setStatusFailure(long lValue)
		{
			m_lStatus = lValue;
		}

		long getStatusFailure ()
		{
			return m_lStatus;
		}

		void setTtisStatusFailure(long lValue)
		{
			m_lTtisStatus = lValue;
		}

		long getTtisStatusFailure ()
		{
			return m_lTtisStatus;
		}
		

		void setPECAnsStatusFailure(long lValue)
		{
			m_lPecAnsStatus = lValue;
		}

		long getPECAnsStatusFailure ()
		{
			return m_lPecAnsStatus;
		}

		void setPAPreRecStatusFailure(long lValue)
		{
			m_lPaPreRecStatus = lValue;
		}

		long getPAPreRecStatusFailure ()
		{
			return m_lPaPreRecStatus;
		}

		void setPALiveStatusFailure(long lValue)
		{
			m_lPaLiveStatus = lValue;
		}

		long getPALiveStatusFailure ()
		{
			return m_lPaLiveStatus;
		}
			
		void setUpgradeTtisStatusFailure(long lValue)
		{
			m_lUpdateTtisStatus = lValue;
		}

		long getUpgradeTtisStatusFailure ()
		{
			return m_lUpdateTtisStatus;
		}

	private:	
		long m_lStatus;
		long m_lTtisStatus;
		long m_lPecAnsStatus;
		long m_lPaLiveStatus;
		long m_lPaPreRecStatus;
		long m_lUpdateTtisStatus;

};

class TrainCountForAutoSend : public IConfigDataItem
{
	public:
		
		virtual EAutoSendType getType ()
		{
			return ETrainCount;
		}

		TrainCountForAutoSend ()
		{
			m_lTrainCount = -1;
		}	

		void setTrainCount (std::string strTrainCount)
		{
			if (strTrainCount.compare(ALLTRAIN) == 0)
			{
				m_lTrainCount = ALLTRAINNUM;
			}
			else if (strTrainCount.compare(ONETRAIN) == 0)
			{
				m_lTrainCount = ONETRAINNUM;
			}
			else if (strTrainCount.compare(TWOTRAIN) == 0)
			{
				m_lTrainCount = TWOTRAINNUM;
			}
		}

		std::string getTrainCount ()
		{
			std::string strReturn;
			
			switch (m_lTrainCount)
			{
				case ALLTRAINNUM:
					strReturn = ALLTRAIN;
					break;
				case ONETRAINNUM:
					strReturn = ONETRAIN;
					break;
				case TWOTRAINNUM:
					strReturn = TWOTRAIN;
					break;
			}

			return strReturn;
		}

		long getTrainCountNumber ()
		{
			return m_lTrainCount;
		}

	private:
		long m_lTrainCount;
};


class RadioIdSetupConfig : public IConfigDataItem
{
	public:
		RadioIdSetupConfig ()
		{
			m_bisPrimary = true;
		}

		virtual EAutoSendType getType ()
		{
			return ERadioIdSetup;
		}
		
		bool getRadioConfigValue ()
		{
			return m_bisPrimary;
		}

		void setRadioConfigValue (bool bValue)
		{
			m_bisPrimary = bValue;
		}

	private:
		bool m_bisPrimary;
};