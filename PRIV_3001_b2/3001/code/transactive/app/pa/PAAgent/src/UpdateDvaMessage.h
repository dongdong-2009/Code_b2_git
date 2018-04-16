/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/UpdateDvaMessage.h $
 * @author  J. Dalin
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef UpdateDvaMessage_H
#define UpdateDvaMessage_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class UpdateDvaMessage : public PasTransaction
{
public:
    UpdateDvaMessage( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~UpdateDvaMessage();
    void setUpdateMessageType( const unsigned short usUpdateMessageType );
    void setOriginalMessageType(const unsigned short usOrgMsgType );
    void setDvaMsgId(const unsigned short usDvaMsgId );
    void setToBeDeleted( const bool bToBeDeleted );

private:
    UpdateDvaMessage();
	UpdateDvaMessage( const UpdateDvaMessage& );
	const UpdateDvaMessage& operator=( const UpdateDvaMessage& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usUpdateMessageType;
    unsigned short m_usDvaMsgId;
    unsigned short m_usDvaOrgMsgType;
    bool m_bToBeDeleted;
};

} // namespace TA_IRS_App

#endif // UpdateDvaMessage_H
