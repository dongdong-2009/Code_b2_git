/*
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/DeleteScheduleDva.h $
 * @author  HuangQi
 * @version $Revision: #1 $
 * Last modification : $DateTime: 2018/03/14 13:51:20 $
 * Last modified by : $Author: lim.cy $
 *
 * 
 */

#ifndef DELETESCHEDULEDVA_H
#define DELETESCHEDULEDVA_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/pa/PAAgent/src/PasTransaction.h"

namespace TA_IRS_App
{

class DeleteScheduleDva : public PasTransaction
{
public:
    DeleteScheduleDva( const bool bAtOcc, ITransactionOwner* pOwner );
    virtual ~DeleteScheduleDva();

    void setScheduleId( const unsigned short usScheduleId );

private:
    DeleteScheduleDva();
	DeleteScheduleDva( const DeleteScheduleDva& );
    const DeleteScheduleDva& operator=( const DeleteScheduleDva& );

    virtual int executeOccImp( PasConnectionAgency& refConnection );
    virtual int executeStnImp( PasConnectionAgency& refConnection );

private:
    unsigned short m_usScheduleId;
};

} // namespace TA_IRS_App

#endif // DELETESCHEDULEDVA_H
