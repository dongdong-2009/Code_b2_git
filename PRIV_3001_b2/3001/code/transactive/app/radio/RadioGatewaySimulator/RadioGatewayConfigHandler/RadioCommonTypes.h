

#pragma once

#include <string>
#include <vector>
#include <map>

enum ERadioOperationType
{
	ERadioSubscriberMonitoring,
	ERadioDefaultProfileMonitoring
};

class RadioResultData
{	
	public:
		RadioResultData ();
		~RadioResultData ();
	
	protected:	
		void fixRequiredAliasLen (std::string & strAlias);
		void fixRequiredNumberOfEntry(std::vector <std::string> & vctrData);
		void fixRequiredNumberOfEntry(std::string & strData,char cToInsert, bool bInsertAtBegin);
		
	private:
		void fillStringWithSpecChar (std::string & strData,char cToInsert, bool bInsertAtBegin);
		void fillStringWithSpaceChar (std::string & strData,int nPos,int nSpaceCnt);
		void deleteCharFromString (std::string & strData);
};

class RadioParamData
{
};

struct RadioSubscriberData
{
	RadioSubscriberData ()
	{ 
		strAffiliatedTG = "Talk Group 1";
		strBaseStation = "BaseStation1";
	}
	
	std::string strAffiliatedTG;
	std::string strBaseStation;
};

struct RadioCallForwardingQueryData
{
	RadioCallForwardingQueryData ()
	{
		strForwardingTsi = "000001000106";
		strDestinationTsi = "000000010011";
		lCondition = 0;
	}

	std::string strForwardingTsi;
	std::string strDestinationTsi;
	unsigned long lCondition;
};

struct ProfileDBUpdateData
{
	std::string strProfile;
	unsigned long ulOperation;
};

typedef std::vector <std::string> RadioProfileCollectionType;
typedef std::vector <ProfileDBUpdateData> ProfileDBUpdateCollectionType;
typedef std::vector <std::string> ProfileTalkGroupListType;
typedef std::vector <std::string> ProfileDBUpdateRawType;


class RadioMonitorProfileRequestParamData : public RadioParamData
{
public:
	RadioMonitorProfileRequestParamData (std::string strProfileAlias) : m_strProfileAlias (strProfileAlias)
	{}

	std::string getProfile (){return m_strProfileAlias;}

private:
	std::string m_strProfileAlias;
};

class RadioProfileRequestResultData : public RadioResultData
{
	public:

		RadioProfileRequestResultData ()
		{
			for (int nCounter = 0; nCounter < 12; nCounter++)
			{
				m_vctrTalkgroupMembers.push_back("Talk Group 1");
			}
		}

		~RadioProfileRequestResultData () {m_vctrTalkgroupMembers.clear();} 

		void setProfileTalkGroup (ProfileTalkGroupListType profileTG);
		
		ProfileTalkGroupListType getProfileTalkGroup () 
		{
			return m_vctrTalkgroupMembers;
		}

	private:

		ProfileTalkGroupListType m_vctrTalkgroupMembers;
};


class RadioSubscriberParamData : public RadioParamData
{
	public:
		RadioSubscriberParamData (unsigned long ulResourceId) : m_ulResourceId (ulResourceId)
		{}

		unsigned long getResourceId (){return m_ulResourceId;}
	
	private:
		unsigned long m_ulResourceId;
};

class RadioSubscriberResultData : public RadioResultData
{
	public:
		RadioSubscriberResultData ();
		
		void setRadioSubscriberData (RadioSubscriberData radioSubscriberData);
		RadioSubscriberData getRadioSubscriberData ();

	private:
		RadioSubscriberData m_radioSubscriberData;
};


class RadioProfileDBUpdateResultData : public RadioResultData
{
	public:
		RadioProfileDBUpdateResultData ();
		~RadioProfileDBUpdateResultData ();

		void setProfileDBUpdate (ProfileDBUpdateRawType rawDataColletion);
		ProfileDBUpdateCollectionType getProfileDBUpdate ();

	private:
		ProfileDBUpdateCollectionType m_profileDbCollectionType;
};

class RadioSystemStatusResultData : public RadioResultData
{
	public:
		RadioSystemStatusResultData ()
		{
			m_ulStatus = 0;
		}
		~RadioSystemStatusResultData ();
		
		void setRadioStatus (std::string strRadioStatus);
		unsigned long getRadioStatus () {return m_ulStatus;}

		private:
			unsigned long m_ulStatus;
};


class RadioDGNAStatusResultData : public RadioResultData
{
public:
	RadioDGNAStatusResultData () {m_ulStatus = 0;};
	~RadioDGNAStatusResultData () {};

	void setRadioStatus (std::string strRadioStatus);
	unsigned long getDGNAStatus () {return m_ulStatus;}

private:
	unsigned long m_ulStatus;
};


class RadioQueryCallForwadingResultData : public RadioResultData
{
public:
	RadioQueryCallForwadingResultData () {};
	~RadioQueryCallForwadingResultData () {};

	void setRadioCallForwardingQueryData (std::string strCondition, 
										   std::string strForwardTsi, 
										   std::string strDestionationTsi);

	RadioCallForwardingQueryData getRadioCallForwardingQueryData () {return m_readioQueryCallForwarding;}

private:
	RadioCallForwardingQueryData m_readioQueryCallForwarding;
};