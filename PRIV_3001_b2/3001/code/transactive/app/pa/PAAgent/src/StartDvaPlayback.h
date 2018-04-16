/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StartDvaPlayback.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef STARTDVAPLAYBACK_H
#define STARTDVAPLAYBACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StartDvaPlayback : public PasTransaction
{
public:
    StartDvaPlayback( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StartDvaPlayback();

    void setDvaMessageId( const unsigned short usDvaMessageId );
	void setSourceId( const unsigned short usSourceId );

private:
    StartDvaPlayback();
	StartDvaPlayback( const StartDvaPlayback& );
	const StartDvaPlayback& operator=( const StartDvaPlayback& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usDvaMessageId;
	unsigned short m_usSourceId;
};

} // namespace TA_IRS_App

#endif // STARTDVAPLAYBACK_H
