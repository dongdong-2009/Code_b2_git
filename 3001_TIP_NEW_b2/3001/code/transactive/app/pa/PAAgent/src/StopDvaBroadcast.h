/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StopDvaBroadcast.h $
 * @author  HuangQi
 * @version $Revision: #3 $
 * Last modification : $DateTime: 2012/11/29 22:02:45 $
 * Last modified by : $Author: qi.huang $
 *
 * 
 */

#ifndef STOPDVABROADCAST_H
#define STOPDVABROADCAST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StopDvaBroadcast : public PasTransaction
{
public:
    StopDvaBroadcast( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StopDvaBroadcast();

    void setDvaMessageIds( const std::vector<unsigned short>& vecMsgIds );
    void setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys );

private:
    StopDvaBroadcast();
	StopDvaBroadcast( const StopDvaBroadcast& );
    const StopDvaBroadcast& operator=( const StopDvaBroadcast& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    std::vector<unsigned short> m_vecDvaMessageIds;
    std::vector<unsigned long> m_vecPasZoneKeys;
};

} // namespace TA_IRS_App

#endif // STOPDVABROADCAST_H
