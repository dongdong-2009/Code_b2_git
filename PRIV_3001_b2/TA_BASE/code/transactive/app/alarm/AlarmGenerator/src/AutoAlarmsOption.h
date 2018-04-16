// AutoAlarmsOption.h: interface for the AutoAlarmsOption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOALARMSOPTION_H__E0FE293D_E867_415C_A3C8_CFA2C3292427__INCLUDED_)
#define AFX_AUTOALARMSOPTION_H__E0FE293D_E867_415C_A3C8_CFA2C3292427__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class AutoAlarmsOption  
{
public:
	AutoAlarmsOption();
	virtual ~AutoAlarmsOption();

	unsigned long m_nAlarmCount;
	unsigned long m_nThreadCount;
	unsigned long m_nInterval;			// millisec
};

#endif // !defined(AFX_AUTOALARMSOPTION_H__E0FE293D_E867_415C_A3C8_CFA2C3292427__INCLUDED_)
