/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StartDvaBroadcast.h $
 * @author  HuangQi
 * @version $Revision: #4 $
 * Last modification : $DateTime: 2013/07/15 13:18:20 $
 * Last modified by : $Author: huangjian $
 *
 * 
 */

#ifndef STARTDVABROADCAST_H
#define STARTDVABROADCAST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StartDvaBroadcast : public PasTransaction
{
public:
    StartDvaBroadcast( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StartDvaBroadcast();

    void setWithChime( const bool bWithChime );
    void setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds );
    void setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys );
	void setTransactionId( const unsigned short usTransationId );
	void setDwellPeriod(const unsigned short usDwellPeriod);

private:
    StartDvaBroadcast();
	StartDvaBroadcast( const StartDvaBroadcast& );
	const StartDvaBroadcast& operator=( const StartDvaBroadcast& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    bool m_bWithChime;
	unsigned short m_usTransationId;
    std::vector<unsigned short> m_vecDvaMessageIds;
    std::vector<unsigned long> m_vecPasZoneKeys;
	unsigned short m_usDwellPeriod;
};

} // namespace TA_IRS_App

#endif // STARTDVABROADCAST_H
