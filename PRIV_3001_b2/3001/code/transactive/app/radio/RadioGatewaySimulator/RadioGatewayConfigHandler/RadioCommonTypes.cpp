
#include "stdafx.h"
#include "RadioCommonTypes.h"

#define ALIAS_REQUIRED_LEN			12
#define TG_REQUIRED_NUMBER			12
#define GENERIC_NUMBER_ENTRY		12

#define INSERT_OPERATION			"INSERT"
#define EDIT_OPERATION				"EDIT"
#define DELETE_OPERATION			"DELETE"

#define RADIO_NORMAL				"RADIO_NORMAL"
#define RADIO_DEGRADED				"RADIO_DEGRADED"
#define RADIO_NORMAL_NUM				"0"
#define RADIO_DEGRADED_NUM				"1"

#define DGNA_SUCCESSFULL				"SUCCESSFULL"
#define DGNA_UNSUCCESSFULL				"UNSUCCESSFULL"

#define QCF_CONDITION_NR				"NOT_REACHABLE"
#define QCF_CONDITION_FINR				"FORWARD_IF_NOT_REACHABLE"
#define QCF_CONDITION_INF				"ISSI_NOT_FOUND"

#define EQUAL_STRING				0

RadioResultData::RadioResultData ()
{

}

RadioResultData::~RadioResultData ()
{

}

void RadioResultData::fixRequiredAliasLen (std::string & strAlias)
{
	int nLenAlias = strAlias.length();

	if (nLenAlias > ALIAS_REQUIRED_LEN)	
	{
		deleteCharFromString (strAlias);
	}
	else if (nLenAlias < ALIAS_REQUIRED_LEN)
	{
		fillStringWithSpaceChar (strAlias,nLenAlias,(ALIAS_REQUIRED_LEN - nLenAlias));
	}
}

void RadioResultData::fillStringWithSpaceChar (std::string & strData,int nPos,int nSpaceCnt)
{
	for (int nCounter = 0; nCounter < nSpaceCnt; nCounter++)
	{
		strData.insert(nPos," ");
	}
}

void RadioResultData::deleteCharFromString (std::string & strData)
{
	strData.erase(12,12-strData.length());
}

RadioSubscriberResultData::RadioSubscriberResultData()
{
}

RadioSubscriberData RadioSubscriberResultData::getRadioSubscriberData ()
{
	return m_radioSubscriberData;
}

void RadioSubscriberResultData::setRadioSubscriberData (RadioSubscriberData radioSubscriberData)
{
	fixRequiredAliasLen (radioSubscriberData.strAffiliatedTG);
	fixRequiredAliasLen (radioSubscriberData.strBaseStation);

	m_radioSubscriberData = radioSubscriberData;
}

void RadioResultData::fixRequiredNumberOfEntry (std::vector <std::string> & vctrData)
{
	int nSize = vctrData.size();

	if (nSize > GENERIC_NUMBER_ENTRY)
	{
		vctrData.erase(vctrData.begin() + GENERIC_NUMBER_ENTRY,vctrData.end());
	}
	else if (nSize < GENERIC_NUMBER_ENTRY)
	{
		int nInsertNum = GENERIC_NUMBER_ENTRY - nSize;
		
		for (int nCounter = nSize; nCounter < GENERIC_NUMBER_ENTRY; nCounter++)
		{
			vctrData.push_back("");
		}
	}
}


void RadioResultData::fixRequiredNumberOfEntry(std::string & strData,char cToInsert, bool bInsertAtBegin)
{
	int nSize = strData.size();

	if (nSize > GENERIC_NUMBER_ENTRY)
	{
		strData.erase(strData.begin() + nSize, strData.end());
	}
	else if (nSize < GENERIC_NUMBER_ENTRY)
	{
		fillStringWithSpecChar (strData, cToInsert, bInsertAtBegin);
	}
	
}

void RadioResultData::fillStringWithSpecChar (std::string & strData, char cToInsert, bool bInsertAtBegin)
{
	int nSize = GENERIC_NUMBER_ENTRY - strData.size();
	
	if (bInsertAtBegin)
	{
		strData.insert(strData.begin(),nSize,cToInsert);
		
	}
	else
	{
		strData.insert(strData.end(),nSize,cToInsert);
	}
}

void RadioProfileRequestResultData::setProfileTalkGroup (ProfileTalkGroupListType profileTG) 
{
	
	m_vctrTalkgroupMembers.clear ();
	m_vctrTalkgroupMembers = profileTG;
	
	ProfileTalkGroupListType::iterator iterProfileTG = m_vctrTalkgroupMembers.begin();

	fixRequiredNumberOfEntry (m_vctrTalkgroupMembers);
	
}


RadioProfileDBUpdateResultData::RadioProfileDBUpdateResultData ()
{
	m_profileDbCollectionType.clear();
}

RadioProfileDBUpdateResultData::~RadioProfileDBUpdateResultData ()
{

}

void RadioProfileDBUpdateResultData::setProfileDBUpdate (ProfileDBUpdateRawType rawDataColletion)
{
	m_profileDbCollectionType.clear ();
	
	std::vector <std::string> vctrProfile;
	std::vector <unsigned long> vctrOperation;

	ProfileDBUpdateRawType::iterator iterProfCollection = rawDataColletion.begin();
	
	for (;iterProfCollection != rawDataColletion.end (); iterProfCollection++)
	{
		if ((*iterProfCollection).compare(INSERT_OPERATION) == EQUAL_STRING ||
			(*iterProfCollection).compare(EDIT_OPERATION) == EQUAL_STRING ||
			(*iterProfCollection).compare(DELETE_OPERATION) == EQUAL_STRING)
		{
			if ((*iterProfCollection).compare(INSERT_OPERATION) == EQUAL_STRING)
			{
				vctrOperation.push_back(1);
			}
			else if ((*iterProfCollection).compare(EDIT_OPERATION) == EQUAL_STRING)
			{
				vctrOperation.push_back(2);
			}
			else if ((*iterProfCollection).compare(DELETE_OPERATION) == EQUAL_STRING)
			{
				vctrOperation.push_back(0);
			}
		}
		else
		{
			vctrProfile.push_back(*iterProfCollection);
		}
	}
	
	// check if equal, if not make neccessary adjustmentto make it equal
	if (vctrProfile.size() != vctrOperation.size())
	{
		if (vctrProfile.size() > vctrOperation.size())
		{
			int nSize = vctrProfile.size() - vctrOperation.size();

			for (int nCounter = 0; nCounter < nSize; nCounter++)
			{
				vctrOperation.push_back(1);
			}
		}
		else 
		{
			int nSize = vctrOperation.size() - vctrProfile.size();

			for (int nCounter = 0; nCounter < nSize; nCounter++)
			{
				vctrProfile.push_back("SO");
			}
		}
	}

	for (int nCounter = 0; nCounter < vctrProfile.size(); nCounter++)
	{
		ProfileDBUpdateData profileDbUpdate;
		
		profileDbUpdate.strProfile = vctrProfile.at (nCounter);
		profileDbUpdate.ulOperation = vctrOperation.at (nCounter);

		m_profileDbCollectionType.push_back(profileDbUpdate);
	}

}

ProfileDBUpdateCollectionType RadioProfileDBUpdateResultData::getProfileDBUpdate ()
{
	return m_profileDbCollectionType;
}	



RadioSystemStatusResultData::~RadioSystemStatusResultData ()
{

}

void RadioSystemStatusResultData::setRadioStatus (std::string strRadioStatus)
{
	if (strRadioStatus.compare(RADIO_NORMAL) == EQUAL_STRING || 
		strRadioStatus.compare(RADIO_NORMAL_NUM) == EQUAL_STRING)
	{
		m_ulStatus = 0;
	}
	else if (strRadioStatus.compare(RADIO_DEGRADED) == EQUAL_STRING ||
			 strRadioStatus.compare(RADIO_DEGRADED_NUM) == EQUAL_STRING)
	{
		m_ulStatus = 1;
	}

}

void RadioDGNAStatusResultData::setRadioStatus (std::string strRadioStatus)
{
	if (strRadioStatus.compare(DGNA_SUCCESSFULL) == EQUAL_STRING)
	{
		m_ulStatus = 0;
	}
	else if (strRadioStatus.compare(DGNA_UNSUCCESSFULL) == EQUAL_STRING)
	{
		m_ulStatus = 1;
	}

}


void RadioQueryCallForwadingResultData::setRadioCallForwardingQueryData (std::string strCondition, 
																 std::string strForwardTsi, 
																 std::string strDestionationTsi)
{
	if (strCondition.compare(QCF_CONDITION_NR) == EQUAL_STRING)
	{
		m_readioQueryCallForwarding.lCondition = 0;
	}
	else if (strCondition.compare(QCF_CONDITION_FINR) == EQUAL_STRING)
	{
		m_readioQueryCallForwarding.lCondition = 1;
	}
	else if (strCondition.compare(QCF_CONDITION_INF) == EQUAL_STRING)
	{
		m_readioQueryCallForwarding.lCondition = 9;
	}
	
	m_readioQueryCallForwarding.strDestinationTsi = strDestionationTsi;
	m_readioQueryCallForwarding.strForwardingTsi = strForwardTsi;

	fixRequiredNumberOfEntry(m_readioQueryCallForwarding.strDestinationTsi,'0',true);
	fixRequiredNumberOfEntry(m_readioQueryCallForwarding.strForwardingTsi,'0',true);
}