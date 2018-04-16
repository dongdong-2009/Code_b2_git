/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StartLiveBroadcast.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef STARTLIVEBROADCAST_H
#define STARTLIVEBROADCAST_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StartLiveBroadcast : public PasTransaction
{
public:
    StartLiveBroadcast( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StartLiveBroadcast();

    void setWithChime( const bool bWithChime );
    void setPasZoneKeys( const std::vector<unsigned long>& vecPasZoneKeys );
	void setSourceId( const unsigned short usSourceId );
	void setTransactionId( const unsigned short usTransactionId );

private:
    StartLiveBroadcast();
	StartLiveBroadcast( const StartLiveBroadcast& );
	const StartLiveBroadcast& operator=( const StartLiveBroadcast& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
	unsigned short m_usSourceId;
	unsigned short m_usTransationId;
    bool m_bWithChime;
    std::vector<unsigned long> m_vecPasZoneKeys;
};

} // namespace TA_IRS_App

#endif // STARTLIVEBROADCAST_H
