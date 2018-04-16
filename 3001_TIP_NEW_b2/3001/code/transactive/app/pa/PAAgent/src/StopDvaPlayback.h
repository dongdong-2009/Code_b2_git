/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAAgent/src/StopDvaPlayback.h $
 * @author  HuangJian
 * @version $Revision: #2 $
 * Last modification : $DateTime: 2012/02/06 16:15:14 $
 * Last modified by : $Author: haijun.li $
 *
 * 
 */

#ifndef STOPDVAPLAYBACK_H
#define STOPDVAPLAYBACK_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StopDvaPlayback : public PasTransaction
{
public:
    StopDvaPlayback( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StopDvaPlayback();

	void setSourceId( const unsigned short usSourceId );

private:
    StopDvaPlayback();
	StopDvaPlayback( const StopDvaPlayback& );
    const StopDvaPlayback& operator=( const StopDvaPlayback& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
	unsigned short m_usSourceId;
};

} // namespace TA_IRS_App

#endif // STOPDVAPLAYBACK_H
