/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StopDvaBroadcast.h $
 * @author  HuangQi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
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
