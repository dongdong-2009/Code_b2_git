/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/CreateScheduleDva.h $
 * @author  HuangQi
 * @version $Revision: #7 $
 * Last modification : $DateTime: 2013/09/30 16:53:25 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef CREATESCHEDULEDVA_H
#define CREATESCHEDULEDVA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class CreateScheduleDva : public PasTransaction
{
public:
    CreateScheduleDva( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~CreateScheduleDva();

    void setWithChime( const bool bWithChime );
    void setDaysInWeek( const unsigned short usDaysInWeek );
    void setStartTime( const unsigned short usStartTime, const unsigned short usStartSecond );
    void setEndTime( const unsigned short usEndTime, const unsigned short usEndSecond );
    void setInterval( const unsigned short usInterval, const unsigned short usIntervalSecond );
    void setScheduleId( const unsigned short usScheduleId );
    void setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds );
    void setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys );
	void setDwellPeriod(const unsigned short usDwellPeriod);

private:
    CreateScheduleDva();
	CreateScheduleDva( const CreateScheduleDva& );
    const CreateScheduleDva& operator=( const CreateScheduleDva& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    bool m_bWithChime;
    unsigned short m_usScheduleId;
    unsigned short m_usDaysInWeek;
    unsigned short m_usStartTime;
    unsigned short m_usEndTime;
    unsigned short m_usStartSecond;
    unsigned short m_usEndSecond;
    unsigned short m_usInterval;
    unsigned short m_usIntervalSecond;
	unsigned short m_usDwellPeriod;
    std::vector<unsigned short> m_vecDvaMessageIds;
    std::vector<unsigned long> m_vecPasZoneKeys;
};

} // namespace TA_IRS_App

#endif // CREATESCHEDULEDVA_H
