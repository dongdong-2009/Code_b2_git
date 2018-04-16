// ymAlarmData.h: interface for the ymAlarmData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_YMALARMDATA_H__7586C865_7D10_4B9A_A477_CD8D602D638F__INCLUDED_)
#define AFX_YMALARMDATA_H__7586C865_7D10_4B9A_A477_CD8D602D638F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>


class agAlarmData  
{
public:
	time_t m_tSubmit;
	unsigned long m_nEntityTypeKey;
	unsigned long m_nEntityKey;
	unsigned long m_nSubsystemKey;
	unsigned long m_nLocationKey;
	std::string m_strID;

	agAlarmData();
	virtual ~agAlarmData();

};

#endif // !defined(AFX_YMALARMDATA_H__7586C865_7D10_4B9A_A477_CD8D602D638F__INCLUDED_)
