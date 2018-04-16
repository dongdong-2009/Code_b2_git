/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/StartDvaRecording.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef STARTDVARECORDING_H
#define STARTDVARECORDING_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class StartDvaRecording : public PasTransaction
{
public:
    StartDvaRecording( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~StartDvaRecording();

    void setDvaMessageId( const unsigned short usDvaMessageId );
	void setSourceId( const unsigned short usSourceId );
	void setMsgType( const unsigned short usMsgType );
private:
    StartDvaRecording();
	StartDvaRecording( const StartDvaRecording& );
	const StartDvaRecording& operator=( const StartDvaRecording& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usDvaMessageId;
	unsigned short m_usSourceId;
	unsigned short m_usMsgType;
};

} // namespace TA_IRS_App

#endif // STARTDVARECORDING_H
